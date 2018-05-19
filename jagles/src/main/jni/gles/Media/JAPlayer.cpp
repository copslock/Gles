//
// Created by LiHong on 16/3/21.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "JAPlayer.h"
#include "../GLES/ParametricManager.h"
#include "../Utils/mediabuffer.h"

#define TIME_BASE 1000000.0
#ifdef __linux__
#include <unistd.h>
#include <sys/prctl.h>
#define set_thread_name(name) \
{   \
    prctl(PR_SET_NAME, name); \
}
#elif defined(__APPLE__)
#define set_thread_name(name) \
{   \
    pthread_setname_np(name); \
}
#else
#define set_thread_name(name)
#endif

#ifdef _MSC_VER
#define usleep(a) Sleep(a/1000)
#endif

#define LOGV(...)__android_log_print(ANDROID_LOG_DEBUG,"Lee",__VA_ARGS__);

void *JAPlayer_task(void *arg) {
    JAPlayer *p = (JAPlayer *)arg;
    set_thread_name("Player_task");
    p->DoThread();
    return NULL;
}

JAPlayer::JAPlayer(void *ctx)
{
    av_register_all();
    avformat_network_init();
    pCodec         = NULL;
    pCodecCtx      = NULL;
    pFormatCtx     = NULL;
    pFrame         = NULL;
    pFrameRGB      = NULL;
    pAudioCodec    = NULL;
    pAudioCodecCtx = NULL;
    mRunthread     = false;
    mVideoHeight   = 0;
    mVideoWidth    = 0;
    mCropWidth     = 0;
    mCropHeight    = 0;
    mCropLeft      = 0;
    mCropTop       = 0;
    mCropRight     = 0;
    mCropBottom    = 0;
    mCropFishEye   = false;
    mCtx           = ctx;

	mWallMode = 0;
}

JAPlayer::~JAPlayer()
{

}

bool JAPlayer::PlayFile(char *filename,bool isimage,bool isfisheye,int64_t startime, OnPlayProgress callback)
{
    pFormatCtx = avformat_alloc_context();
	mProgress = callback;
	mStarttime = startime;
	mIsImage = isimage;
    int err;
    if ((err=avformat_open_input(&pFormatCtx, filename, NULL, NULL)) != 0) {
        char str[AV_ERROR_MAX_STRING_SIZE];
        memset(str, 0, sizeof(str));
        av_strerror(err, str, sizeof(str));
        LOGV("mediacodec error %d %s",err,str);
        return false;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        printf("Couldn't find stream information.\n");
        return false;
    }
    videoindex = -1;
    audioindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            //break;
        }
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioindex= i;
            //break;
        }
    }
    if (videoindex == -1) {
        printf("Didn't find a video stream.\n");
        return false;
    }

    pCodec = avcodec_find_decoder(pFormatCtx->streams[videoindex]->codec->codec_id);
    if (pCodec == NULL) {
        printf("Codec not found.\n");
        return false;
    }
    if(isimage) {
        pCodecCtx = avcodec_alloc_context3(pCodec);

        pCodecCtx->width = pFormatCtx->streams[videoindex]->codec->width;
        pCodecCtx->height = pFormatCtx->streams[videoindex]->codec->height;
        pCodecCtx->pix_fmt = pFormatCtx->streams[videoindex]->codec->pix_fmt;
    }
    else
        pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Could not open codec.\n");
        return false;
    }
    if(audioindex>=0) {
        pAudioCodecCtx = pFormatCtx->streams[audioindex]->codec;
        pAudioCodec = avcodec_find_decoder(pAudioCodecCtx->codec_id);
        if (pAudioCodec == NULL) {
            printf("Audio Codec not found.\n");
            return false;
        }
        if (avcodec_open2(pAudioCodecCtx, pAudioCodec, NULL) < 0) {
            printf("Could not open Audio codec.\n");
            return false;
        }
    }

    //img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR32, SWS_BICUBIC, NULL, NULL, NULL);
    mVideoWidth = pCodecCtx->width;
    mVideoHeight = pCodecCtx->height;

    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
	pFrameAudio = av_frame_alloc();

    mVideoBufferSize = avpicture_get_size(AV_PIX_FMT_BGR32,pCodecCtx->width,pCodecCtx->height);
    mVideoBuffer = (uint8_t *)malloc(mVideoBufferSize);
    avpicture_fill((AVPicture *)pFrameRGB,mVideoBuffer,AV_PIX_FMT_BGR32,pCodecCtx->width,pCodecCtx->height);

    packet = (AVPacket *)av_malloc(sizeof(AVPacket));

    mCropFishEye = isfisheye;
    mRunthread = true;
	mStarttime = mStarttime - pFormatCtx->duration / TIME_BASE;
	mSeek = false;
	pthread_mutex_init(&mLock, NULL);
    pthread_create(&mPlaythread,NULL,JAPlayer_task,this);
    return true;
}

void JAPlayer::StopPlay()
{
	if (!mRunthread)
		return;
    mRunthread = false;
    pthread_join(mPlaythread,NULL);
}

void JAPlayer::SeekFile(int mSec)
{
	pthread_mutex_lock(&mLock);
	mSeekSecond = mSec;
	mSeek = true;
	pthread_mutex_unlock(&mLock);
}


void JAPlayer::DoThread() {
    int ret, got_picture;
	int lasttime=0;
	uint64_t lasttimestamp = 0;
    int tb = 0;
	ParametricManager *mgn = (ParametricManager *)mCtx;
    while(mRunthread)
    {
		if(mSeek)
		{
			pthread_mutex_lock(&mLock);
			printf("seek file position:%d ..................\n", mSeekSecond);
			av_seek_frame(pFormatCtx, -1, mSeekSecond*TIME_BASE, AVSEEK_FLAG_BACKWARD);
			avcodec_flush_buffers(pCodecCtx);
			avcodec_flush_buffers(pAudioCodecCtx);
			mSeek = false;
			pthread_mutex_unlock(&mLock);
			printf("Seek Unlock.....\n");
			lasttimestamp = 0;
			lasttime = 0;
		}
		if(av_read_frame(pFormatCtx, packet) < 0)
		{
			usleep(10000);
			continue;
		}
		//printf("read frame.....size:%d\n", packet->size);
		if (packet->stream_index == videoindex) {
			//Decode
			got_picture = 0;
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
			if (ret < 0) {
				printf("Decode Error.\n");
				//continue;
				break;
			}
			if (!got_picture)
				continue;
			DoScale();

			if (NULL != mgn->OnTextureAvaible)
			{
				mgn->OnTextureAvaible(VideoWidth(), VideoHeight(), mVideoBuffer, mVideoBufferSize, packet->flags & AV_PKT_FLAG_KEY ? 1 : 0, 0, 0, 0);
			}
			uint64_t m_pts = av_frame_get_best_effort_timestamp(pFrame);
			AVRational avbase = { 1, AV_TIME_BASE };

			m_pts = av_rescale_q(m_pts, pFormatCtx->streams[videoindex]->time_base, avbase);
			double m_real_pts = m_pts / TIME_BASE;
            if(!tb)
                tb = (int)m_real_pts;
			//printf("m_pts:%lld %f\n", m_pts, m_real_pts);
            if(1)//!mIsImage
			{
#ifdef _MSC_VER
				int64_t lgmtime;
#else
				long lgmtime;
#endif
				lgmtime = mStarttime + (int)m_real_pts;
				struct tm *t = localtime((const time_t *)&lgmtime);
				char *txtbuf = (char *)malloc(40);
				memset(txtbuf, 0, 40);
				sprintf(txtbuf, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
				unsigned char *txttex = mgn->_Textput->GenTextbuffer((char *)txtbuf, 0xff000000, 480, 24);
				if(mgn->OnOSDTextureAvaible)
					mgn->OnOSDTextureAvaible(NULL, txttex, 480 * 24 * 4, mgn->GetCtx());
				free(txttex);
				free(txtbuf);
				if (mProgress)
					mProgress((int)m_real_pts-tb, (int)(pFormatCtx->duration / TIME_BASE),mWallMode);
			}
			int timeuse;
			if (lasttime == 0)
				timeuse = 0;
			else
				timeuse = clock_ms() - lasttime;
			int frametime;
			if (lasttimestamp == 0)
				frametime = 40;
			else
				frametime = (m_pts - lasttimestamp)/1000;
			lasttimestamp = m_pts;
		//	printf("frame time:%d use time:%d\n", frametime,timeuse);
			if (frametime - timeuse > 0)
#ifdef _MSC_VER
				Sleep(frametime - timeuse);
#else
				usleep((frametime - timeuse) * 1000);
#endif
        }
		if (packet->stream_index == audioindex)
		{
			got_picture = 0;
			//printf("audio buffer..........\n");
			ret = avcodec_decode_audio4(pAudioCodecCtx, pFrameAudio, &got_picture, packet);
			if(got_picture)
			{
				//printf("audio decode size:%d\n", pFrameAudio->nb_samples);
                if(pAudioCodecCtx->codec->id==AV_CODEC_ID_AAC)
                {
                    uint8_t *outbuffer = NULL;
                    int out_linesize = 0;
                    int data_size = av_samples_get_buffer_size(&out_linesize, pAudioCodecCtx->channels,
                                                               pFrameAudio->nb_samples, pAudioCodecCtx->sample_fmt,
                                                               1);

                    outbuffer = (uint8_t *) malloc(data_size);
                    memset((void *) outbuffer, 0, data_size);

                    struct SwrContext *swr_ctx = NULL;
                    swr_ctx = swr_alloc();
                    swr_ctx = swr_alloc_set_opts(swr_ctx, AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, 8000,
                                                 pAudioCodecCtx->channel_layout, pAudioCodecCtx->sample_fmt,
                                                 pAudioCodecCtx->sample_rate, 0, NULL);
                    swr_init(swr_ctx);
                    swr_convert(swr_ctx, &outbuffer, out_linesize, (const uint8_t **) pFrameAudio->data,
                                pFrameAudio->nb_samples);

                    mgn->PlayAudioData(outbuffer, pFrameAudio->nb_samples*2);

                }
                else
				    mgn->PlayAudioData(pFrameAudio->data[0], pFrameAudio->nb_samples*2);
			}
		}
        av_free_packet(packet);
		lasttime = clock_ms();
    }
    printf("......................................Exit Player Thread\n");
    free(mVideoBuffer);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    if(pAudioCodecCtx)
        avcodec_close(pAudioCodecCtx);
    avformat_close_input(&pFormatCtx);
    pCodec = NULL;
    pCodecCtx = NULL;
    pFormatCtx = NULL;
    pFrame = NULL;
    pFrameRGB = NULL;
    pAudioCodec = NULL;
    pAudioCodecCtx = NULL;
    mVideoHeight = 0;
    mVideoWidth = 0;
    mCropWidth = 0;
    mCropHeight = 0;
    mCropLeft = 0;
    mCropTop = 0;
    mCropRight = 0;
    mCropBottom = 0;
	pthread_exit(NULL);
}

void JAPlayer::DetectCircleData(int *left, int *right, int *top, int *bottom, int height, int width)
{
	float lw = (*right - *left);
	float lh = (*bottom - *top);
	float lcut = lw / lh;
	if (lcut > 1.30) {
		if (lw < height) {
			int lhalfw = width / 2;
			int lhalfh = height / 2;
			int lhalf = lhalfw - lhalfh;
			*left = lhalf + 50;
			*right = width - lhalf - 50;
			*top = 50;
			*bottom = height - 50;
		}
		else
		{
			*top = 0;
			*bottom = height - 1;
		}
	}
	if (lw<(float)height*0.7 || lh<(float)height*0.7)
	{
		if (height == 720 || height == 1080)
		{
			*top = 0;
			*bottom = height - 1;
			*left = 100;
			*right = width - 100;
		}
		else
		{
			int lhalfw = width / 2;
			int lhalfh = height / 2;
			int lhalf = lhalfw - lhalfh;
			*left = lhalf + 50;
			*right = width - lhalf - 50;
			*top = 50;
			*bottom = height - 50;
		}
	}
}


void JAPlayer::TestCircRound(AVFrame *f,int *left,int *right,int *top,int *bottom,unsigned char opt)
{
    int minx = mVideoWidth-1;
    int maxx = 0;
    int miny = mVideoHeight;
    int maxy = 0;
    int x,y;
    unsigned char *buf = f->data[0];
    unsigned char *dbuf =(unsigned char *) malloc(mVideoWidth*mVideoHeight);
    memset(dbuf,0,mVideoWidth*mVideoHeight);
//    int bit = f->linesize[0] / f->width;
    for ( y = 0; y<mVideoHeight; y++) {
        for ( x=0; x<mVideoWidth; x++) {
            if (buf[y*f->linesize[0]+x]>opt) {
                dbuf[y*mVideoWidth+x] = 1;
            }
        }
    }
    for (y = mVideoHeight/3; y<mVideoHeight*2/3; y++) {
        for (x=10; x<mVideoWidth; x++) {
            if (dbuf[y*mVideoWidth+x]==1) {
                if (x<minx)
                {
                    minx = x;
                }
                break;
            }
        }
    }
    for (y = mVideoHeight/3; y<mVideoHeight*2/3; y++) {
        for (x=mVideoWidth-1; x>=0; x--) {
            if (dbuf[y*mVideoWidth+x]==1) {
                if (x>maxx)
                    maxx = x;
                break;
            }
        }
    }
    for (x=mVideoWidth/3; x<mVideoWidth/2; x++) {
        for (y = 0; y<mVideoHeight; y++) {
            if (dbuf[y*mVideoWidth+x]==1) {
                if (y<miny)
                {
                    miny = y;
                }
                break;
            }
        }
    }
    for (x=mVideoWidth/3; x<mVideoWidth/2; x++) {
        for (y = mVideoHeight-1; y>0; y--) {
            if (dbuf[y*mVideoWidth+x]==1) {
                if (y>maxy)
                    maxy = y;
                break;
            }
        }
    }
    *left = minx;
    *right = maxx;
    *top = miny;
    *bottom = maxy;
	//if(!mIsImage)
 //   	DetectCircleData(left, right, top, bottom, mVideoWidth,mVideoHeight);
    free(dbuf);
}


void JAPlayer::DoScale() {
    if(mCropFishEye) {
        if (mCropWidth == 0 && mCropHeight == 0) {
            if(pCodecCtx->pix_fmt!=AV_PIX_FMT_YUV420P)
            {
                AVFrame *tmpframe = av_frame_alloc();
                int tmpsize = avpicture_get_size(AV_PIX_FMT_YUV420P,pFrame->width,pFrame->height);
                uint8_t *tmpbuf = (uint8_t *)av_malloc(tmpsize);
                avpicture_fill((AVPicture *)tmpframe,tmpbuf,AV_PIX_FMT_YUV420P,pFrame->width,pFrame->height);
                img_convert_ctx = sws_getContext(pFrame->width,pFrame->height, pCodecCtx->pix_fmt, pFrame->width,pFrame->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
                sws_scale(img_convert_ctx, (const unsigned char *const *) pFrame->data, pFrame->linesize, 0, pFrame->height, tmpframe->data, tmpframe->linesize);
                sws_freeContext(img_convert_ctx);
                TestCircRound(tmpframe, &mCropLeft, &mCropRight, &mCropTop, &mCropBottom, 80);
                av_free(tmpbuf);
                av_free(tmpframe);
            }
            else
                TestCircRound(pFrame, &mCropLeft, &mCropRight, &mCropTop, &mCropBottom, 80);
            mCropWidth = mCropRight - mCropLeft;
            mCropHeight = mCropBottom - mCropTop;
            if (mCropWidth % 2)
                mCropWidth += 1;
            if (mCropHeight % 2)
                mCropHeight += 1;
            float a1 = (float)mCropWidth/mCropHeight;
			if (a1 > 1.28)
				mWallMode = 1;
			else
				mWallMode = 0;
            mCropCut = false;
            mVideoBufferSize = avpicture_get_size(AV_PIX_FMT_BGR32,VideoWidth(),VideoHeight());
            if(mVideoBuffer)
                free(mVideoBuffer);
            mVideoBuffer = (unsigned char *)malloc(mVideoBufferSize);
            if(mCropCut) {
                int lsize = avpicture_get_size(AV_PIX_FMT_BGR32,mCropWidth,(mCropWidth-mCropHeight)/2);
                avpicture_fill((AVPicture *) pFrameRGB, mVideoBuffer+lsize, AV_PIX_FMT_BGR32, VideoWidth(), VideoHeight());
            }
            else
                avpicture_fill((AVPicture *)pFrameRGB,mVideoBuffer,AV_PIX_FMT_BGR32,VideoWidth(),VideoHeight());
            mCropPic = av_frame_alloc();
        }
        av_picture_crop((AVPicture *)mCropPic,(AVPicture *)pFrame,pCodecCtx->pix_fmt,mCropTop,mCropLeft);
        img_convert_ctx = sws_getContext(mCropWidth, mCropHeight, pCodecCtx->pix_fmt, mCropWidth, mCropHeight, AV_PIX_FMT_BGR32, SWS_BICUBIC, NULL, NULL, NULL);
        sws_scale(img_convert_ctx, (const unsigned char *const *) mCropPic->data, mCropPic->linesize, 0, mCropHeight, pFrameRGB->data, pFrameRGB->linesize);
        sws_freeContext(img_convert_ctx);
    }
    else {
        if(mCropWidth)
        {
            mCropWidth = 0;
            mCropHeight = 0;
            mCropLeft = 0;
            mCropTop = 0;
            mCropRight = 0;
            mCropBottom = 0;
            if(mVideoBuffer)
                free(mVideoBuffer);
            mVideoBufferSize = avpicture_get_size(AV_PIX_FMT_BGR32,mVideoWidth,mVideoHeight);
            mVideoBuffer = (unsigned char *)malloc(mVideoBufferSize);
            avpicture_fill((AVPicture *)pFrameRGB,mVideoBuffer,AV_PIX_FMT_BGR32,VideoWidth(),VideoHeight());
        }
        img_convert_ctx = sws_getContext(mVideoWidth, mVideoHeight, pCodecCtx->pix_fmt, mVideoWidth, mVideoHeight, AV_PIX_FMT_BGR32, SWS_BICUBIC, NULL, NULL, NULL);
        sws_scale(img_convert_ctx, (const unsigned char *const *) pFrame->data, pFrame->linesize, 0, mVideoHeight, pFrameRGB->data, pFrameRGB->linesize);
        sws_freeContext(img_convert_ctx);
    }
}

int JAPlayer::VideoWidth()
{
    if(mCropFishEye)
        return mCropWidth;
    else
        return mVideoWidth;
}

int JAPlayer::VideoHeight()
{
    if(mCropFishEye) {
        if(!mCropCut)
            return mCropHeight;
        else
            return mCropWidth;
    }
    else
        return mVideoHeight;
}

int  JAPlayer::WallMode()
{
	return mWallMode;
}

