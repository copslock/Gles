//
// Created by LiHong on 16/2/9.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "JAMedia.h"
#include "h264wh.h"
#include "h265wh.h"
#include "../Utils/mediabuffer.h"
#include "../Network/JAConnect.h"
#include "../../ffmpeg/include/libavcodec/avcodec.h"
#include <pthread.h>

#include <string.h>

#define LOG_TAG "JAVideo"
#define LOGD(...)__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGV(...)__android_log_print(ANDROID_LOG_DEBUG,"Lee",__VA_ARGS__)
#define INFO(...) {__android_log_write(ANDROID_LOG_INFO,"videoconnect",__VA_ARGS__);}
#define LOGE(...) {__android_log_print(ANDROID_LOG_ERROR, "videoconnect", __VA_ARGS__);}

#define Thumbnail 0
#define FullImage 1

#ifdef __cplusplus
extern "C" {
#endif
extern pthread_mutex_t gDecoderMutex;
#ifdef __cplusplus
}
#endif

//#define __TESTRECORD__

JAMedia::JAMedia() {
    av_register_all();
    OnCaptureImage = NULL;
    OnRecordVideo = NULL;
    mCropFishEye = false;
    isDirectTexture = false;
    directTextureWidth = 0;
    directTextureHieght = 0;
    mVideoHeight = 0;
    mVideoWidth = 0;
    mCropWidth = 0;
    mCropHeight = 0;
    mCropLeft = 0;
    mCropTop = 0;
    mCropRight = 0;
    mCropBottom = 0;
    isVideoDecoderActive = false;
    isRecord = false;
    mVideoBuffer = NULL;
    mCaptureImage = 0;
    mRecord = 0;
    mWriteRecord = false;
    mExtraSize = 0;
    mWallMode = 0;
    mRadius = 0;
    mCenterX = 0;
    mCenterY = 0;
    mPicture = NULL;
    mOutpic = NULL;
    mContext = NULL;
    img_convert_ctx = NULL;

    CaptureImageType = FullImage;
    memset(mExtraData, 0, sizeof(mExtraData));
    memset(mRecordFileName, 0, sizeof(mRecordFileName));
    memset(mCapFileName, 0, sizeof(mCapFileName));
#ifdef __ANDROID__
    mGraphicBuffer = NULL;
    mGraphicBufferOSD = NULL;
    OnNotSupportDirectTexture = NULL;
#endif
    av_log_set_level(0);

    //  mVideo_pkt_queue = new PacketQueue;
    //  mAudio_pkt_queue = new PacketQueue;

    mISP720Dev = false;
    mAudioDecoderAvaliable = false;
}

JAMedia::~JAMedia() {
    //   mVideo_pkt_queue->packet_queue_destroy();
    //   mAudio_pkt_queue->packet_queue_destroy();
    CloseVideoDecoder();
#ifdef __ANDROID__
    if (mGraphicBuffer)
        delete mGraphicBuffer;
    if (mGraphicBufferOSD)
        delete mGraphicBufferOSD;
#endif
}

int JAMedia::AnalystHeader(enum AVCodecID codec, unsigned char *header, int headersize,
                           AVPixelFormat fmt) {
    int lw, lh;

    switch (codec) {
        case AV_CODEC_ID_H264:
            if (!GetWidthHeight((char *) header, headersize, &lw, &lh)) {
                return -3;
            }
            break;
        case AV_CODEC_ID_HEVC:
            if (H265GetWidthHeight((char *) header, headersize, &lw, &lh)) {
                return -3;
            }
            break;
        default:
            return -3;
            break;
    }

    pthread_mutex_lock(&gDecoderMutex);
    if (isVideoDecoderActive) {
        if (lw != mVideoWidth || lh != mVideoHeight) {
            CloseVideoDecoder();
            mVideoWidth = lw;
            mVideoHeight = lh;
            isVideoDecoderActive = false;
            if (mCropFishEye) {
                mCropWidth = 0;
                mCropHeight = 0;
                mCropLeft = 0;
                mCropTop = 0;
                mCropRight = 0;
                mCropBottom = 0;
            }
        } else {
            pthread_mutex_unlock(&gDecoderMutex);
            return 0;
        }
    } else {
        mVideoWidth = lw;
        mVideoHeight = lh;
    }
    mExtraSize = 0;
    if (codec == AV_CODEC_ID_H264) {        //h264的ExtraSize的计算
        memcpy(mExtraData, header, headersize > 1024 ? 1024 : headersize);
        int size = headersize > 1024 ? 1019 : headersize - 5;
        mExtraSize = size;
        for (int i = 0; i < size; i++) {
            //LOGV1("found extract data...........%d %x %x %x %x %x %x",mExtraSize,header[i+0],header[i+1],header[i+2],header[i+3],header[i+4],header[i+5],header[i+6]);
            if ((header[i] == 0 && header[i + 1] == 0 && header[i + 2] == 0 && header[i + 3] == 1 &&
                 (header[i + 4] & 0x1F) == 5) ||
                (header[i] == 0 && header[i + 1] == 0 && header[i + 2] == 0 &&
                 (header[i + 3] & 0x1F) == 5)) {
                mExtraSize = i;
                break;
            }
        }
    }

    if (codec == AV_CODEC_ID_HEVC)            //h265的ExtraSize的计算
    {
        memcpy(mExtraData, header, headersize > 1024 ? 1024 : headersize);
        int size = headersize > 1024 ? 1019 : headersize - 5;
        mExtraSize = size;
        for (int i = 0; i < size; i++) {
            uint32_t lv =
                    header[i] << 24 | header[i + 1] << 16 | header[i + 2] << 8 | header[i + 3];
            if ((lv & 0xffffff00) == 0x100 && (((lv & 0xff) >> 1) & 0x3f) == 0x13) {
                mExtraSize = i;
                break;
            }
        }
    }
    pthread_mutex_unlock(&gDecoderMutex);
    return 0;
}

int JAMedia::InitVideoDecoder(enum AVCodecID codec, unsigned char *header, int headersize,
                              AVPixelFormat fmt) {

    int ret = AnalystHeader(codec, header, headersize, fmt);
    if (ret)
        return ret;

    if (isVideoDecoderActive)
        return -3;

    mCodec = avcodec_find_decoder(codec);
    if (!mCodec) {
        pthread_mutex_unlock(&gDecoderMutex);
        return -1;
    }

    if (mContext)
        avcodec_close(mContext);
    mContext = avcodec_alloc_context3(mCodec);
    mContext->width = mVideoWidth;
    mContext->height = mVideoHeight;
    //TODO: 2018-05-07
    mContext->thread_count = 4;
    if (mPicture)
        av_frame_free(&mPicture);
    if (mOutpic)
        av_frame_free(&mOutpic);
    mPicture = av_frame_alloc();
    mOutpic = av_frame_alloc();

    mVideoBufferSize = avpicture_get_size(fmt, mVideoWidth, mVideoHeight);

    if (mVideoBuffer)
        free(mVideoBuffer);
    mVideoBuffer = (unsigned char *) malloc(mVideoBufferSize);
    avpicture_fill((AVPicture *) mOutpic, mVideoBuffer, fmt, mVideoWidth, mVideoHeight);
    /* open it */
    if (avcodec_open2(mContext, mCodec, NULL) < 0) {
        pthread_mutex_unlock(&gDecoderMutex);
        return -2;
    }
    mDecoderFmt = fmt;
    if (img_convert_ctx)
        sws_freeContext(img_convert_ctx);

    img_convert_ctx = sws_getContext(mContext->width, mContext->height, AV_PIX_FMT_YUV420P,
                                     mContext->width, mContext->height, mDecoderFmt, SWS_BICUBIC,
                                     NULL, NULL, NULL);
    isVideoDecoderActive = true;
    pthread_mutex_unlock(&gDecoderMutex);
    return 0;
}

void
JAMedia::DetectCircleData(int *left, int *right, int *top, int *bottom, int height, int width) {
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
        } else {
            *top = 0;
            *bottom = height - 1;
        }
    }
    if (lw < (float) height * 0.7 || lh < (float) height * 0.7) {
        if (height == 720 || height == 1080) {
            *top = 0;
            *bottom = height - 1;
            *left = 100;
            *right = width - 100;
        } else {
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

void JAMedia::TestCircRound(AVFrame *f, int *left, int *right, int *top, int *bottom,
                            unsigned char opt) {
    if (mRadius) {
        *left = mCenterX - mRadius;
        *right = mCenterX + mRadius;
        *top = mCenterY - mRadius;
        *bottom = mCenterY + mRadius;
        if (*left < 0)
            *left = 0;
        if (*right > f->width)
            *right = f->width;
        if (*top < 0)
            *top = 0;
        if (*bottom > f->height)
            *bottom = f->height;
        printf("TestCircleRound left:%d,top:%d,right:%d,bottom:%d\n", *left, *top, *right, *bottom);
        return;
    }
    //    if (f->height == 1944)		//p6固定切割范围
    //    {
    //        *left = 85;
    //        *right = 1879;
    //        *top = 158;
    //        *bottom = 1669;
    //        return;
    //    }
    int minx = f->width - 1;
    int maxx = 0;
    int miny = f->height;
    int maxy = 0;
    int x, y;
    unsigned char *buf = (unsigned char *) malloc(f->linesize[0] * f->height);
    memcpy(buf, f->data[0], f->linesize[0] * f->height);
    unsigned char *dbuf = (unsigned char *) malloc(f->width * f->height);
    memset(dbuf, 0, f->width * f->height);
    for (y = 0; y < f->height; y++) {
        for (x = 0; x < f->width; x++) {
            if (buf[y * f->linesize[0] + x] > opt) {
                dbuf[y * f->width + x] = 1;
            }
        }
    }
    for (y = f->height / 3; y < f->height * 2 / 3; y++) {
        for (x = 10; x < f->width; x++) {
            if (dbuf[y * f->width + x] == 1) {
                if (x < minx) {
                    minx = x;
                }
                break;
            }
        }
    }
    for (y = f->height / 3; y < f->height * 2 / 3; y++) {
        for (x = f->width - 1; x >= 0; x--) {
            if (dbuf[y * f->width + x] == 1) {
                if (x > maxx)
                    maxx = x;
                break;
            }
        }
    }
    for (x = f->width / 3; x < f->width / 2; x++) {
        for (y = 0; y < f->height; y++) {
            if (dbuf[y * f->width + x] == 1) {
                if (y < miny) {
                    miny = y;
                }
                break;
            }
        }
    }
    for (x = f->width / 3; x < f->width / 2; x++) {
        for (y = f->height - 1; y > 0; y--) {
            if (dbuf[y * f->width + x] == 1) {
                if (y > maxy)
                    maxy = y;
                break;
            }
        }
    }

    *left = minx;
    *right = maxx;
    *top = miny;
    *bottom = maxy;

    mRadius = (*right - *left) / 2;
    mCenterX = *left + mRadius;
    mCenterY = *top + (*bottom - *top) / 2;

    //	if (f->height == 720)			//p1固定切割范围
    //	{
    //		*left = 59;
    //		*right = 1244;
    //		*top = 4;
    //		*bottom = 719;
    //	}
    //	else
    DetectCircleData(left, right, top, bottom, f->height, f->width);
    free(dbuf);
    free(buf);
}

int JAMedia::DecodeVideo(unsigned char *inbuf, int insize) {
    int got;
    int graWidth, graHeight;
    bool isunlock = false;
    //static struct SwsContext *img_convert_ctx;
    AVPacket packet;
    if (!isVideoDecoderActive)
        return -2;

    //printf("h264:%d,h265:%d\n",AV_CODEC_ID_H264,AV_CODEC_ID_HEVC);
    pthread_mutex_lock(&gDecoderMutex);
    int testtime = clock_ms();
    av_new_packet(&packet, insize);
    memcpy(packet.data, inbuf, insize);
    avcodec_decode_video2(mContext, mPicture, &got, &packet);
    av_free_packet(&packet);
    //	printf("decode time:%d got:%d\n", clock_ms() - testtime,got);
    if (got == 0) {
        pthread_mutex_unlock(&gDecoderMutex);
        return -1;
    }
    //printf("[JAMEDIA]-----------------------------size:%d\n", insize);
//	if (mCropFishEye && !mISP720Dev) {
//		LOGV("[jasonchan] need crop....");
//		if (mCropWidth == 0 && mCropHeight == 0)
//		{
//			TestCircRound(mPicture, &mCropLeft, &mCropRight, &mCropTop, &mCropBottom, 130);
//			mCropWidth = mCropRight - mCropLeft;
//			mCropHeight = mCropBottom - mCropTop;
//			if (mCropWidth % 2)
//				mCropWidth += 1;
//			if (mCropHeight % 2)
//				mCropHeight += 1;
//			float a1 = (float)mCropWidth / mCropHeight;
//			//		JACONNECT_INFO("A1:%f",a1);
//			if (a1 > 1.33&&a1 < 1.9)
//			{
//				mWallMode = 1;
//			}
//			else
//			{
//				mWallMode = 0;
//			}
//			//            if(a1>1.39&&a1<1.9)
//			//                mCropCut = true;
//			//            else
//			mCropCut = false;
//			mVideoBufferSize = avpicture_get_size(mDecoderFmt, VideoWidth(), VideoHeight());
//			if (mVideoBuffer)
//				free(mVideoBuffer);
//			mVideoBuffer = (unsigned char *)calloc(1, mVideoBufferSize);
//			if (mCropCut) {
//				if (mDecoderFmt == AV_PIX_FMT_BGR32) {
//					int lsize = avpicture_get_size(mDecoderFmt, mCropWidth, (mCropWidth - mCropHeight) / 2);
//					avpicture_fill((AVPicture *)mOutpic, mVideoBuffer + lsize, mDecoderFmt, VideoWidth(), VideoHeight());
//				}
//				else if (mDecoderFmt == AV_PIX_FMT_YUV420P)
//				{
//					memset(mOutpic, 0, sizeof(AVPicture));
//					memset(mVideoBuffer, 0, mCropWidth*mCropWidth);
//					memset(mVideoBuffer + mCropWidth*mCropWidth, 128, mVideoBufferSize - mCropWidth*mCropWidth);
//					mOutpic->data[0] = mVideoBuffer + mCropWidth*(mCropWidth - mCropHeight) / 2;
//					mOutpic->data[1] = mVideoBuffer + mCropWidth*mCropWidth + (mCropWidth / 2)*(mCropWidth - mCropHeight) / 4;
//					mOutpic->data[2] = mVideoBuffer + mCropWidth*mCropWidth + (mCropWidth / 2)*(mCropWidth / 2) + (mCropWidth / 2)*(mCropWidth - mCropHeight) / 4;
//					mOutpic->linesize[0] = mCropWidth;
//					mOutpic->linesize[1] = mCropWidth / 2;
//					mOutpic->linesize[2] = mCropWidth / 2;
//				}
//			}
//			else
//				avpicture_fill((AVPicture *)mOutpic, mVideoBuffer, mDecoderFmt, VideoWidth(), VideoHeight());
//			mCropPic = av_frame_alloc();
//			mOSDPic = av_frame_alloc();
//			mOSDOutPic = av_frame_alloc();
//			mOSDWidth = mVideoWidth * 0.3;
//			mOSDHeight = mVideoHeight * 0.04;
//			mOSDBufferSize = avpicture_get_size(mDecoderFmt, mOSDWidth, mOSDHeight);
//			mOSDBuffer = (unsigned char *)malloc(mOSDBufferSize);
//			avpicture_fill((AVPicture *)mOSDOutPic, mOSDBuffer, mDecoderFmt, mOSDWidth, mOSDHeight);
//			//printf("......................mCropWidth:%d,mCropHeight:%d\n",mCropWidth,mCropHeight);
//		}
//		else
//		{
//			float a1 = (float)mCropWidth / mCropHeight;
//			if (a1 > 1.33&&a1 < 1.9)
//			{
//				mWallMode = 1;
//			}
//			else
//			{
//				mWallMode = 0;
//			}
//		}
//		av_picture_crop((AVPicture *)mCropPic, (AVPicture *)mPicture, AV_PIX_FMT_YUV420P, mCropTop, mCropLeft);
//		img_convert_ctx = sws_getContext(mCropWidth, mCropHeight, AV_PIX_FMT_YUV420P, mCropWidth, mCropHeight, mDecoderFmt, SWS_BICUBIC, NULL, NULL, NULL);
//		sws_scale(img_convert_ctx, (const unsigned char *const *)mCropPic->data, mCropPic->linesize, 0, mCropHeight, mOutpic->data, mOutpic->linesize);
//		sws_freeContext(img_convert_ctx);
//
//		//if(mCropCut)
//		//    av_picture_crop((AVPicture *)mOSDPic,(AVPicture *)mPicture,AV_PIX_FMT_YUV420P,0,mVideoWidth*0.16);
//		//else
//		//    av_picture_crop((AVPicture *)mOSDPic,(AVPicture *)mPicture,AV_PIX_FMT_YUV420P,0,0);
//		//img_convert_ctx = sws_getContext(mOSDWidth, mOSDHeight, AV_PIX_FMT_YUV420P, mOSDWidth, mOSDHeight, mDecoderFmt, SWS_BICUBIC, NULL, NULL, NULL);
//		//sws_scale(img_convert_ctx, (const unsigned char *const *) mOSDPic->data, mCropPic->linesize, 0, mOSDHeight, mOSDOutPic->data, mOSDOutPic->linesize);
//		//sws_freeContext(img_convert_ctx);
//		//if(mDecoderFmt==AV_PIX_FMT_BGR32) {
//		//    int lcount = mOSDBufferSize / 4;
//		//    unsigned char *lbuf = mOSDBuffer + 3;
//		//    for (int i = 0; i < lcount; i++) {
//		//        *lbuf = 0;
//		//        lbuf += 4;
//		//    }
//		//}
//		if (mCaptureImage)
//		{
//			AtomicDec(&mCaptureImage);
//			if (CaptureImageType)
//				WritePNG(mOutpic, mCapFileName, mCropWidth, mCropHeight);
//			else
//				WritePNG(mOutpic, mCapFileName, mCropWidth, mCropHeight, 100, 100);
//		}
//		pthread_mutex_unlock(&gDecoderMutex);
//		isunlock = true;
//		graWidth = VideoWidth(); graHeight = VideoHeight();			//android direct texture
//	}
//	else {
//		if (mCropWidth)
//		{
//			mCropWidth = 0;
//			mCropHeight = 0;
//			mCropLeft = 0;
//			mCropTop = 0;
//			mCropRight = 0;
//			mCropBottom = 0;
//			if (mVideoBuffer)
//				free(mVideoBuffer);
//			mVideoBufferSize = avpicture_get_size(mDecoderFmt, mVideoWidth, mVideoHeight);
//			mVideoBuffer = (unsigned char *)malloc(mVideoBufferSize);
//			avpicture_fill((AVPicture *)mOutpic, mVideoBuffer, mDecoderFmt, VideoWidth(), VideoHeight());
//		}
    int l, r, t, b;
    if (!mRadius)
        TestCircRound(mPicture, &l, &r, &t, &b, 130);
    sws_scale(img_convert_ctx, (const unsigned char *const *) mPicture->data, mPicture->linesize, 0,
              mContext->height, mOutpic->data, mOutpic->linesize);

    LOGD("DecodeVideo----->mCaptureImage：%d,CaptureImageType：%d,mCapFileName：%d", mCaptureImage,
         CaptureImageType, mCapFileName);
    if (mCaptureImage) {
        AtomicDec(&mCaptureImage);
        if (CaptureImageType)
            WritePNG(mOutpic, mCapFileName, mContext->width, mContext->height);
        else
            WritePNG(mOutpic, mCapFileName, mContext->width, mContext->height, 100, 100);
    }
    pthread_mutex_unlock(&gDecoderMutex);
    isunlock = true;
    graWidth = VideoWidth();
    graHeight = VideoHeight();        //android direct texture
//	}

    if (mISP720Dev) {
        graWidth = mContext->width;
        graHeight = mContext->height;
    }


    //	JACONNECT_INFO("graWidth%d, grawHeight%d", graWidth, graHeight);
#ifdef __ANDROID__  //android direct texture buffer
    if (isDirectTexture) {
        if (mGraphicBuffer == NULL || directTextureWidth != graWidth ||
            directTextureHieght != graHeight) {
            directTextureHieght = graHeight;
            directTextureWidth = graWidth;
            if (mGraphicBuffer != NULL)
                delete mGraphicBuffer;
            mGraphicBuffer = new GraphicBuffer(directTextureWidth, directTextureHieght,
                                               PIXEL_FORMAT_RGBA_8888,
                                               GraphicBuffer::USAGE_SW_READ_OFTEN);
            if (!mGraphicBuffer->IsSupportDirectTexture()) {
                isDirectTexture = false;
                delete mGraphicBuffer;
                if (OnNotSupportDirectTexture)
                    OnNotSupportDirectTexture(false, pJAConnect);
            }
        }
    }

    if (isDirectTexture) {
        void *writePtr;
        void *readPtr;
        mGraphicBuffer->lock(GraphicBuffer::USAGE_SW_READ_OFTEN, &writePtr);

        readPtr = mVideoBuffer;
        int stride = mGraphicBuffer->getStride();
        for (int i = 0; i < directTextureHieght; i++) {
            memcpy(writePtr, readPtr, directTextureWidth * 4);
            writePtr = (void *) (int(writePtr) + stride * 4);
            readPtr = (void *) (int(readPtr) + directTextureWidth * 4);
        }
        mGraphicBuffer->unlock();
    }
#endif
    if (!isunlock)
        pthread_mutex_unlock(&gDecoderMutex);

//    gdecodeTime += clock_ms() - decodeStartTime;
//    gdecodeFrames++;
//    if(gdecodeFrames%500 == 0 && gdecodeFrames != 0)
//    {
//        int Average = gdecodeTime/gdecodeFrames;
//        if (gDebugFile) {
//            char info[256] = {0};
//            sprintf(info,"$$ decodFrames:%d Time:%d average:%d (W:%d H:%d)...\n",
//                    (int)gdecodeFrames,(int)gdecodeTime, Average, mVideoWidth,mVideoHeight);
//            int wcount = write(gDebugFile, info, strlen(info));
//            LOGV("$$ decodFrames:%d Time:%d average:%d ...\n",(int)gdecodeFrames,(int)gdecodeTime, Average);
//        }
//    }

    return 0;
}

void JAMedia::CloseVideoDecoder() {
    if (!isVideoDecoderActive)
        return;
    //pthread_mutex_lock(&gDecoderMutex);
    if (img_convert_ctx) {
        sws_freeContext(img_convert_ctx);
        img_convert_ctx = NULL;
    }
    if (mContext) {
        avcodec_close(mContext);
        av_free(mContext);
        mContext = NULL;
    }
    if (mPicture) {
        av_frame_free(&mPicture);
        //av_free(mPicture);
        mPicture = NULL;
    }
    if (mOutpic) {
        av_frame_free(&mOutpic);
        //av_free(mOutpic);
        mOutpic = NULL;
    }
    if (mVideoBuffer) {
        free(mVideoBuffer);
        mVideoBuffer = NULL;
    }
    isVideoDecoderActive = false;
    //pthread_mutex_unlock(&gDecoderMutex);
}

unsigned char *JAMedia::GetVideoBuffer() {
    return mVideoBuffer;
}

FILE *fp = NULL;

int JAMedia::InitAudioDecoder(enum AVCodecID codec, int pSample, int pChannel, int bit,
                              int nb_samples) {
    if (mAudioDecoderAvaliable) {
        //        CloseAudioDecoder();
        return 1;
    }
    int rt = 0;
    AVCodec *lcodec = NULL;
    lcodec = avcodec_find_decoder(codec);
    if (!lcodec) {
        printf("[JAMedia] -InitAudioDecoder- Fail to find audio decoder. Decoder id -> %d\n",
               codec);
        return -1;
    }

    mAudioCodecCtx = avcodec_alloc_context3(lcodec);

    if (!mAudioCodecCtx) {
        printf("[JAMedia] -InitAudioDecoder- Fail to alloc audio decoder context.\n");
        av_free(lcodec);
        lcodec = NULL;
        return -2;
    }


    //    mAudioCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    //    mAudioCodecCtx->channels = pChannel;
    //    mAudioCodecCtx->sample_rate = pSample;
    //    mAudioCodecCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;
    //    mAudioCodecCtx->frame_size = nb_samples;

    mAudioCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    mAudioCodecCtx->sample_rate = 8000;
    mAudioCodecCtx->channels = 1;
    mAudioCodecCtx->bit_rate = 16;
    mAudioCodecCtx->channel_layout = AV_CH_LAYOUT_MONO;
    mAudioCodecCtx->frame_size = nb_samples;
    mAudioCodecCtx->sample_fmt = AV_SAMPLE_FMT_FLTP;


    rt = avcodec_open2(mAudioCodecCtx, lcodec, NULL);
    if (rt < 0) {
        avcodec_free_context(&mAudioCodecCtx);
        mAudioCodecCtx = NULL;
        return rt;
    }

    mAudioFrame = av_frame_alloc();
    mAudioSampleRate = pSample;
    mAudioChannels = pChannel;

    printf(">>>>>>>>> audioDecoder samplerate->%d, channels->%d\n", mAudioCodecCtx->sample_rate,
           mAudioCodecCtx->channels);

    mAudioDecoderAvaliable = true;
    return 0;
}

void JAMedia::DecodeAudio(unsigned char *inbuf, int insize, unsigned char *outbuf, int *outsize) {
    if (!mAudioDecoderAvaliable) {
        printf("[JAMedia] -DecodeAudio- Invalid audio decoder.\n");
        return;
    }

    if (fp)
        fwrite(inbuf, insize, 1, fp);

    //LOGD("audio frame.........%d",insize);
    int got_pic = 0;
    int rt = 0;
    AVPacket lpacket;
    av_init_packet(&lpacket);

    lpacket.size = insize;
    lpacket.data = inbuf;

    rt = avcodec_decode_audio4(mAudioCodecCtx, mAudioFrame, &got_pic, &lpacket);
    if (rt < 0) {
        return;
    }

    uint8_t *outbuffer = NULL;
    int out_linesize = 0;
    int data_size = av_samples_get_buffer_size(&out_linesize, mAudioCodecCtx->channels,
                                               mAudioFrame->nb_samples, mAudioCodecCtx->sample_fmt,
                                               1);
    *outsize = out_linesize;

    outbuffer = (uint8_t *) malloc(data_size);
    memset((void *) outbuffer, 0, data_size);

    struct SwrContext *swr_ctx = NULL;
    swr_ctx = swr_alloc();
    swr_ctx = swr_alloc_set_opts(swr_ctx, AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, 8000,
                                 mAudioCodecCtx->channel_layout, mAudioCodecCtx->sample_fmt,
                                 mAudioCodecCtx->sample_rate, 0, NULL);
    swr_init(swr_ctx);
    swr_convert(swr_ctx, &outbuffer, out_linesize, (const uint8_t **) mAudioFrame->data,
                mAudioFrame->nb_samples);

    memcpy((void *) outbuf, outbuffer, data_size);

    swr_free(&swr_ctx);
    swr_ctx = NULL;

    free(outbuffer);
    outbuffer = NULL;
    av_free_packet(&lpacket);
}

void JAMedia::CloseAudioDecoder() {
    if (!mAudioDecoderAvaliable)
        return;

    avcodec_free_context(&mAudioCodecCtx);
    mAudioCodecCtx = NULL;

    av_frame_free(&mAudioFrame);
    mAudioFrame = NULL;

    mAudioDecoderAvaliable = false;
}


int JAMedia::AudioResample(AVCodecContext *audio_dec_ctx, AVFrame *pAudioDecodeFrame,
                           int out_sample_fmt, int out_channels, int out_sample_rate, char *buf) {
    return 0;
}


void JAMedia::EnableCrop(bool value) {
    mCropFishEye = value;
}

void JAMedia::CaptureImage(char *filename, int type, void *mJAConnect) {
    pJAConnect = mJAConnect;
    CaptureImageType = type;

    LOGV("strcpy filename len:%d", strlen(filename));
    strcpy(mCapFileName, filename);
    AtomicInc(&mCaptureImage);
    LOGV("strcpy filename mCaptureImage:%d", mCaptureImage);
}

void JAMedia::CaptureCurImage(char *filename, void *mJAConnect) {
    WritePNG(mOutpic, filename, VideoWidth(), VideoHeight());
}

int JAMedia::StartRecord(char *filename, bool h265, int fps, int bitrate, void *mJAConnect) {
    pJAConnect = mJAConnect;
    if (mRecord)
        return false;
    mRecordfmt = av_guess_format(NULL, filename, NULL);

    if (!mRecordfmt) {
        //LOGD("Could not deduce output format from file extension: using MP4.......................");
        mRecordfmt = av_guess_format("mp4", NULL, NULL);
    }
    if (!mRecordfmt) {
        //LOGD("Could not find suitable output format..........................");

        return false;
    }
    //LOGD(".....................................jjjjjjjjjjjjjjjjjjjj %x",mRecordfmt);
    /* allocate the output media context */
    mRecordContext = avformat_alloc_context();
    if (!mRecordContext) {
        fprintf(stderr, "Memory error\n");
        return false;
    }
    mRecordContext->oformat = mRecordfmt;
    snprintf(mRecordContext->filename, sizeof(mRecordContext->filename), "%s", filename);

    /* add the audio and video streams using the default format codecs
    and initialize the codecs */
    mRecordContext->oformat->video_codec = h265 ? AV_CODEC_ID_HEVC : AV_CODEC_ID_H264;
    mRecordContext->oformat->audio_codec = AV_CODEC_ID_PCM_ALAW;

    AVStream *vst = avformat_new_stream(mRecordContext, NULL);
    if (!vst) {
        return false;
        //   return AVERROR(ENOMEM);
    }

    vst->codec->codec_type = AVMEDIA_TYPE_VIDEO;

    vst->codec->codec_id = h265 ? AV_CODEC_ID_HEVC : AV_CODEC_ID_H264;

    vst->codec->gop_size = 12;
    //这里就简单的直接赋值为420p


    vst->time_base.num = 1;
    vst->time_base.den = fps;


    vst->codec->width = mVideoWidth;
    vst->codec->height = mVideoHeight;
    vst->codec->pix_fmt = AV_PIX_FMT_YUV420P;

    vst->codec->bit_rate = bitrate;

    if (mExtraSize) {
        vst->codec->extradata = (uint8_t *) av_malloc(mExtraSize);
        memcpy(vst->codec->extradata, mExtraData, mExtraSize);
        vst->codec->extradata_size = mExtraSize;
    }

#ifdef __ANDROID__
    //AVStream *ast = avformat_new_stream(mRecordContext, NULL);
    //if (!ast)
    //    return AVERROR(ENOMEM);
    //AVCodecContext *c = ast->codec;
    //c->codec_id = AV_CODEC_ID_PCM_ALAW;
    //c->codec_type = AVMEDIA_TYPE_AUDIO;
    //c->bit_rate = 64000;
    //c->sample_rate = 8000;
    //c->channels = 1;
    //c->bits_per_raw_sample=16;
#else
    AVStream *ast = avformat_new_stream(mRecordContext, NULL);
    if (!ast)
        return AVERROR(ENOMEM);
    AVCodecContext *c = ast->codec;
    c->codec_id = AV_CODEC_ID_PCM_ALAW;
    c->codec_type = AVMEDIA_TYPE_AUDIO;
    c->bit_rate = 64000;
    c->sample_rate = 8000;
    c->channels = 1;
    c->bits_per_raw_sample = 16;
#endif

    //if (av_set_parameters(mRecordContext, NULL) < 0) {
    //    fprintf(stderr, "Invalid output format parameters\n");
    //    return -1;
    //}

    av_dump_format(mRecordContext, 0, filename, 1);

    if (!(mRecordfmt->flags & AVFMT_NOFILE)) {
        printf("network files!\n");
        if (avio_open(&(mRecordContext->pb), filename, AVIO_FLAG_WRITE) < 0) {
            fprintf(stderr, "Could not open '%s'\n", filename);
            return false;
        }
    }
    printf("Create file %s completed", filename);
    avformat_write_header(mRecordContext, NULL);
    mPts = 0;
#ifdef __TESTRECORD__
    testfp = fopen("test.h264", "wb");
#endif
    AtomicInc(&mRecord);
    return true;
}

void JAMedia::StopRecord() {
    if (!mRecord)
        return;
#ifdef __TESTRECORD__
    fclose(testfp);
#endif
    mWriteRecord = false;
    AtomicDec(&mRecord);
    av_write_trailer(mRecordContext);
    av_free(mRecordContext);
    mPts = 0;
}

int JAMedia::VideoWidth() {
//	if (mCropFishEye)
//		return mCropWidth;
//	else
    return mVideoWidth;
}

void JAMedia::GetFisheyeParameter(float *centerx, float *centery, float *radius) {
    *centerx = mCenterX;
    *centery = mCenterY;
    *radius = mRadius;
}

int JAMedia::VideoHeight() {
//	if (mCropFishEye) {
//		if (!mCropCut)
//			return mCropHeight;
//		else
//			return mCropWidth;
//	}
//	else
    return mVideoHeight;
}

int JAMedia::GetVideoBufferSize() {
    return mVideoBufferSize;
}

bool JAMedia::GetCrop() {
    return mCropFishEye;
}

unsigned char *JAMedia::GetOSDBuffer() {
    return mOSDBuffer;
}

int JAMedia::GetOSDBufferSize() {
    return mOSDBufferSize;
}

int JAMedia::OSDWidth() {
    return mOSDWidth;
}

int JAMedia::OSDHeight() {
    return mOSDHeight;
}

int JAMedia::WallMode() {
    return mWallMode;
}

int JAMedia::WritePNG(AVFrame *pFrame, char *filename, int w, int h) {
    //return 0;
    //INFO("save file start..............................");
    AVCodecContext *pOCodecCtx;
    AVCodec *pOCodec;
    uint8_t *Buffer;
    int BufSizActual;
    enum AVPixelFormat ImgFmt = AV_PIX_FMT_RGB24; //for the newer ffmpeg version, this int to pixelformat
    FILE *JPEGFile;
    struct SwsContext *img_convert_ctx;
    AVPacket pkt;
    AVFrame *tmppic = av_frame_alloc();
    AVFrame *tmppic1 = av_frame_alloc();
    pFrame->width = w;
    pFrame->height = h;

    int got = 0;

    //BufSiz = avpicture_get_size (ImgFmt,pCodecCtx->width,pCodecCtx->height);

    //LOGD("bufsize: .....................................%d",BufSiz);


    pOCodec = avcodec_find_encoder(AV_CODEC_ID_PNG);
    if (!pOCodec) {
        //        free ( Buffer );
        //INFO("Error avcodec_find_encoder........................");
        if (OnCaptureImage) {
            OnCaptureImage(false, filename, pJAConnect);
        }
        return (-1);
    }

    pOCodecCtx = avcodec_alloc_context3(pOCodec);
    if (!pOCodecCtx) {
        //INFO("Error avcodec_alloc_context3........................");
        //        free ( Buffer );
        if (OnCaptureImage) {
            OnCaptureImage(false, filename, pJAConnect);
        }
        return (-1);
    }
    //return 0;

    pOCodecCtx->width = w;
    pOCodecCtx->height = h;
    pOCodecCtx->pix_fmt = ImgFmt;
    pOCodecCtx->compression_level = 0;
    pOCodecCtx->time_base = AV_TIME_BASE_Q;
    pOCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;

    avpicture_alloc((AVPicture *) tmppic, ImgFmt, w, h);
    tmppic->width = w;
    tmppic->height = h;

    img_convert_ctx = sws_getContext(pOCodecCtx->width, pOCodecCtx->height, mDecoderFmt,
                                     pOCodecCtx->width, pOCodecCtx->height, ImgFmt, SWS_BICUBIC,
                                     NULL, NULL, NULL);
    sws_scale(img_convert_ctx, (const unsigned char *const *) pFrame->data, pFrame->linesize, 0,
              pOCodecCtx->height, tmppic->data, tmppic->linesize);
    sws_freeContext(img_convert_ctx);
    img_convert_ctx = NULL;

    //    avpicture_alloc((AVPicture *)tmppic1,AV_PIX_FMT_RGB32,w,h);
    //    tmppic1->width = w;
    //    tmppic1->height = h;
    //
    //    img_convert_ctx = sws_getContext(pOCodecCtx->width, pOCodecCtx->height, mDecoderFmt, pOCodecCtx->width, pOCodecCtx->height, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
    //    sws_scale(img_convert_ctx, (const unsigned char *const *) pFrame->data, pFrame->linesize, 0, pOCodecCtx->height, tmppic1->data, tmppic1->linesize);
    //    sws_freeContext(img_convert_ctx);

    //    FILE *fp = fopen(filename,"wb");
    //    fwrite(pFrame->data[0],1,w*h*4,fp);
    //    fclose(fp);
    //    return 0;
    //
    //    char lfn[1024];
    //    sprintf(lfn,"%s_y",filename);
    //    fp = fopen(lfn,"wb");
    //    fwrite(pFrame->data[0],1,w*h,fp);
    //    fclose(fp);
    //    sprintf(lfn,"%s_u",filename);
    //    fp = fopen(lfn,"wb");
    //    fwrite(pFrame->data[1],1,w*h/4,fp);
    //    fclose(fp);
    //    sprintf(lfn,"%s_v",filename);
    //    fp = fopen(lfn,"wb");
    //    fwrite(pFrame->data[2],1,w*h/4,fp);
    //    fclose(fp);
    //    return 0;

    //         pOCodecCtx->time_base.num = pCodecCtx->time_base.num;
    //         pOCodecCtx->time_base.den = pCodecCtx->time_base.den;

    //INFO("...................ssssssssssss........................");
    int tmp = avcodec_open2(pOCodecCtx, pOCodec, NULL);
    LOGD("WritePNG---->tmp:%d", tmp);
    if (tmp < 0) {
        //free ( Buffer );
        //INFO("Error avcodec_open2........................");
        if (OnCaptureImage) {
            OnCaptureImage(false, filename, pJAConnect);
        }
        return (-1);
    }
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    BufSizActual = avcodec_encode_video2(pOCodecCtx, &pkt, tmppic, &got);

    //LOGD("BufsizeActual..............................%d %d",BufSizActual,got);

    JPEGFile = fopen(filename, "wb");
    fwrite(pkt.data, 1, pkt.size, JPEGFile);
    fclose(JPEGFile);
    avcodec_close(pOCodecCtx);
    av_free_packet(&pkt);
    av_free(pOCodecCtx);
    avpicture_free((AVPicture *) tmppic);
    //INFO("save file end..............................");
    if (OnCaptureImage) {
        OnCaptureImage(true, filename, pJAConnect);
    }
    return 0;
}

int JAMedia::WritePNG(AVFrame *pFrame, char *filename, int srcW, int srcH, int dstW, int dstH) {
    //return 0;
    //INFO("save file start..............................");
    AVCodecContext *pOCodecCtx;
    AVCodec *pOCodec;
    uint8_t *Buffer;
    int BufSizActual;
    enum AVPixelFormat ImgFmt = AV_PIX_FMT_RGB24; //for the newer ffmpeg version, this int to pixelformat
    FILE *JPEGFile;
    static struct SwsContext *img_convert_ctx;
    AVPacket pkt;
    AVFrame *tmppic = av_frame_alloc();
    AVFrame *tmppic1 = av_frame_alloc();

    int got = 0;

    //BufSiz = avpicture_get_size (ImgFmt,pCodecCtx->width,pCodecCtx->height);

    //LOGD("bufsize: .....................................%d",BufSiz);


    pOCodec = avcodec_find_encoder(AV_CODEC_ID_PNG);
    if (!pOCodec) {
        //        free ( Buffer );
        //INFO("Error avcodec_find_encoder........................");
        if (OnCaptureImage) {
            OnCaptureImage(false, filename, pJAConnect);
        }
        return (-1);
    }

    pOCodecCtx = avcodec_alloc_context3(pOCodec);
    if (!pOCodecCtx) {
        //INFO("Error avcodec_alloc_context3........................");
        //        free ( Buffer );
        if (OnCaptureImage) {
            OnCaptureImage(false, filename, pJAConnect);
        }
        return (-1);
    }
    //return 0;

    pOCodecCtx->width = dstW;
    pOCodecCtx->height = dstH;
    pOCodecCtx->pix_fmt = ImgFmt;
    pOCodecCtx->compression_level = 0;

    avpicture_alloc((AVPicture *) tmppic, ImgFmt, dstW, dstH);
    tmppic->width = dstW;
    tmppic->height = dstH;

    img_convert_ctx = sws_getContext(srcW, srcH, mDecoderFmt, dstW, dstH, ImgFmt, SWS_BICUBIC, NULL,
                                     NULL, NULL);
    sws_scale(img_convert_ctx, (const unsigned char *const *) pFrame->data, pFrame->linesize, 0,
              srcH, tmppic->data, tmppic->linesize);
    sws_freeContext(img_convert_ctx);


    //    avpicture_alloc((AVPicture *)tmppic1,AV_PIX_FMT_RGB32,w,h);
    //    tmppic1->width = w;
    //    tmppic1->height = h;
    //
    //    img_convert_ctx = sws_getContext(pOCodecCtx->width, pOCodecCtx->height, mDecoderFmt, pOCodecCtx->width, pOCodecCtx->height, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);
    //    sws_scale(img_convert_ctx, (const unsigned char *const *) pFrame->data, pFrame->linesize, 0, pOCodecCtx->height, tmppic1->data, tmppic1->linesize);
    //    sws_freeContext(img_convert_ctx);

    //    FILE *fp = fopen(filename,"wb");
    //    fwrite(pFrame->data[0],1,w*h*4,fp);
    //    fclose(fp);
    //    return 0;
    //
    //    char lfn[1024];
    //    sprintf(lfn,"%s_y",filename);
    //    fp = fopen(lfn,"wb");
    //    fwrite(pFrame->data[0],1,w*h,fp);
    //    fclose(fp);
    //    sprintf(lfn,"%s_u",filename);
    //    fp = fopen(lfn,"wb");
    //    fwrite(pFrame->data[1],1,w*h/4,fp);
    //    fclose(fp);
    //    sprintf(lfn,"%s_v",filename);
    //    fp = fopen(lfn,"wb");
    //    fwrite(pFrame->data[2],1,w*h/4,fp);
    //    fclose(fp);
    //    return 0;

    //         pOCodecCtx->time_base.num = pCodecCtx->time_base.num;
    //         pOCodecCtx->time_base.den = pCodecCtx->time_base.den;

    //INFO("...................ssssssssssss........................");
    if (avcodec_open2(pOCodecCtx, pOCodec, NULL) < 0) {
        //free ( Buffer );
        //INFO("Error avcodec_open2........................");
        if (OnCaptureImage) {
            OnCaptureImage(false, filename, pJAConnect);
        }
        return (-1);
    }
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;
    BufSizActual = avcodec_encode_video2(pOCodecCtx, &pkt, tmppic, &got);

    //LOGD("BufsizeActual..............................%d %d",BufSizActual,got);

    JPEGFile = fopen(filename, "wb");
    fwrite(pkt.data, 1, pkt.size, JPEGFile);
    fclose(JPEGFile);
    avcodec_close(pOCodecCtx);
    av_free_packet(&pkt);
    av_free(pOCodecCtx);
    avpicture_free((AVPicture *) tmppic);
    //INFO("save file end..............................");
    if (OnCaptureImage) {
        OnCaptureImage(true, filename, pJAConnect);
    }
    return 0;
}

void JAMedia::WriteFrame(uint8_t *buf, int buflen, int buf_type, int duration) {
    LOGV("JAMedia::WriteFrame---->buf_type:%d", buf_type);
    if (!mRecord)
        return;
    //if (!buf_type)
    //	return;
    if (mRecord && buf_type == 1)
        mWriteRecord = true;
    if (mWriteRecord) {
#ifdef __TESTRECORD__
        fwrite(buf, 1, buflen, testfp);
#endif
        AVPacket pkt;
        av_init_packet(&pkt);
        //pkt.pts = *timestamp;
        if (buf_type == 1) {
            pkt.flags |= AV_PKT_FLAG_KEY;
            printf("Key frame found!\n");

        }
        pkt.stream_index = buf_type ? 0 : 1;
        pkt.data = buf_type ? buf : buf + 36;
        pkt.size = buf_type ? buflen : buflen - 36;

        if (buf_type) {
            pkt.pts = mPts;// *12;// av_rescale_q(mPts, AV_TIME_BASE_Q, mRecordContext->streams[0]->time_base);;
            pkt.dts = mPts;// *12;
            //pkt.pts = av_rescale_q(mPts, mRecordContext->streams[0]->codec->time_base, mRecordContext->streams[0]->time_base);;
            //printf("pts:%d duration:%d\n",pkt.pts,duration);
#ifdef _MSC_VER
            mPts += 1;
#else
            mPts += 900 * (duration / 40);// duration;
#endif
        }
        /* write the compressed frame in the media file */
        int ret = av_interleaved_write_frame(mRecordContext, &pkt);
        //LOGV1("write frame data...........%d %d ----- %x %x %x %x %x %x",duration,ret,buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6]);
        av_free_packet(&pkt);
    }
}

bool JAMedia::IsRecord() {
    return mRecord > 0;
}

int JAMedia::GetOSDBufferSizeYUV(int yuv) {
    if (yuv < 0 || yuv > 2)
        return 0;
    switch (yuv) {
        case 0:
            return VideoWidth() * VideoHeight();
            break;
        case 1:
            return VideoWidth() * VideoHeight() / 4;
            break;
        case 2:
            return VideoWidth() * VideoHeight() / 4;
            break;
    }
    return 0;
}

unsigned char *JAMedia::GetVideoBufferYUV(int yuv) {
    if (yuv < 0 || yuv > 2)
        return NULL;
    switch (yuv) {
        case 0:
            return mVideoBuffer;
            break;
        case 1:
            return mVideoBuffer + VideoWidth() * VideoHeight();
            break;
        case 2:
            return mVideoBuffer + VideoWidth() * VideoHeight() + VideoWidth() * VideoHeight() / 4;
            break;
    }
    return NULL;
}

unsigned char *JAMedia::GetOSDBufferYUV(int yuv) {
    if (yuv < 0 || yuv > 2)
        return NULL;
    switch (yuv) {
        case 0:
            return mOSDBuffer;
            break;
        case 1:
            return mOSDBuffer + mOSDWidth * mOSDHeight;
            break;
        case 2:
            return mOSDBuffer + mOSDWidth * mOSDHeight + mOSDWidth * mOSDHeight / 4;
            break;
    }
    return NULL;
}

void JAMedia::UpdateCircleParameter(int radius, int centerx, int centery) {
    mRadius = radius;
    mCenterY = centery;
    mCenterX = centerx;
}

void JAMedia::UpdateCircleParameter(int radius, int centerx, int centery, int wallmode) {
    UpdateCircleParameter(radius, centerx, centery);
    //switch (wallmode)
    //{
    //	case eP2P_IMAGE_FISHEYE_FIX_MODE_WALL:
    //		mWallMode = true;
    //		break;
    //	case eP2P_IMAGE_FISHEYE_FIX_MODE_CEIL:
    //		mWallMode = false;
    //		break;
    //	case eP2P_IMAGE_FISHEYE_FIX_MODE_TABLE:
    //		break;
    //	case eP2P_IMAGE_FISHEYE_FIX_MODE_NONE:
    //		break;
    //}
}

void JAMedia::ResetData() {
    mVideoHeight = 0;
    mVideoWidth = 0;
    mCropWidth = 0;
    mCropHeight = 0;
    mCropLeft = 0;
    mCropTop = 0;
    mCropRight = 0;
    mCropBottom = 0;
    mRadius = 0;
    mCenterX = 0;
    mCenterY = 0;
    mIsP360ev = false;
    mISP720Dev = false;
}


bool JAMedia::WriteTextOSDDirectTextureFrame(void *buffer) {
#ifdef __ANDROID__
    if (isDirectTexture) {
        if (!mGraphicBufferOSD)
            mGraphicBufferOSD = new GraphicBuffer(480, 24, PIXEL_FORMAT_RGBA_8888,
                                                  GraphicBuffer::USAGE_SW_READ_OFTEN);
        if (!mGraphicBuffer->IsSupportDirectTexture()) {
            isDirectTexture = false;
            delete mGraphicBufferOSD;
        }
    }
    if (isDirectTexture) {
        void *writePtr;
        void *readPtr;
        mGraphicBufferOSD->lock(GraphicBuffer::USAGE_SW_READ_OFTEN, &writePtr);

        readPtr = buffer;
        int stride = mGraphicBufferOSD->getStride();
        for (int i = 0; i < 24; i++) {
            memcpy(writePtr, readPtr, 480 * 4);
            writePtr = (void *) (int(writePtr) + stride * 4);
            readPtr = (void *) (int(readPtr) + 480 * 4);
        }
        mGraphicBufferOSD->unlock();
    }
#endif
    return false;
}

void JAMedia::SetIsP720Dev(bool pisp720) {
    mISP720Dev = pisp720;
}

bool JAMedia::getIsP720Dev() {
    return mISP720Dev;
}

void JAMedia::SetIsP360Dev(bool pisp360, float centerx, float centery, float radius, int wallmode) {
    mIsP360ev = pisp360;
    mCenterX = centerx;
    mCenterY = centery;
    mRadius = radius;
    if (wallmode >= 0)
        mWallMode = wallmode;
}

bool JAMedia::getIsP360Dev() {
    return mIsP360ev || mRadius != 0;
}


/* Added by Jason */
//void JAMedia::AddBuf2PacketQueue(void *pframe, int plen, int ptype, int pw, int ph, int psampleRate, int psampleWidth, int psampleChn, uint64_t pts, int gentime) {
//    AVPacket packet_;
//    FrameHead head;
//    
//    av_new_packet(&packet_, plen);
//    memcpy(packet_.data, pframe, plen);
//    
//    head.codec = ptype;
//    head.width = pw;
//    head.height = ph;
//    head.dataSize = plen;
//    head.sysTime_ms = pts;
//    head.coderStamp_ms = gentime;
//    head.sampleRate = psampleRate;
//    head.sampleWidth = psampleWidth;
//    
//    if (ptype == 0) {
//        /* audio */
//        mAudio_pkt_queue->packet_queue_put(&packet_, &head);
//    } else {
//        mVideo_pkt_queue->packet_queue_put(&packet_, &head);
//    }
//}
//
//int JAMedia::GetBufFromPacketQueue(void *frame, unsigned int *len, int type, int *width, int *height, int *psampleRate, int *sampleWidth, int *sampleChn, uint64_t *ts, int *gentime) {
//    AVPacket packet_;
//    FrameHead head;
//    if (type == 0) {
//        /* Audio */
//        if (mAudio_pkt_queue->packet_queue_get(&packet_, &head, 0) == 1) {
//            /* Get packet success */
//            frame = malloc(head.dataSize);
//            if (!frame)
//                return -3;
//            memcpy(frame, packet_.data, head.dataSize);
//            *len = head.dataSize;
//            *psampleRate = head.sampleRate;
//            *sampleWidth = head.sampleWidth;
//            *sampleChn = 1;
//            *ts = head.sysTime_ms;
////            *gentime = 0;
//            av_free_packet(&packet_);
//            return 0;
//        } else
//            return -2;
//    } else {
//        /* Video */
//        if (mVideo_pkt_queue->packet_queue_get(&packet_, &head, 0) == 1) {
//            /* Get packet success */
//            frame = malloc(head.dataSize);
//            if (!frame)
//                return -3;
//            memcpy(frame, packet_.data, head.dataSize);
//            *len = head.dataSize;
//            *width = head.width;
//            *height = head.height;
//            *ts = head.sysTime_ms;
////            *gentime = 0;
//            av_free_packet(&packet_);
//            return 0;
//        } else
//            return -2;
//    }
//}
//
//int JAMedia::GetPacketQueued(int type) {
//    if (type == 0)
//        return mAudio_pkt_queue->packet_get_nb_pkt_in_queue();
//    else
//        return mVideo_pkt_queue->packet_get_nb_pkt_in_queue();
//}
//
//void JAMedia::ClearPacketQueue() {
//    mVideo_pkt_queue->packet_queue_flush();
//    mAudio_pkt_queue->packet_queue_flush();
//}
