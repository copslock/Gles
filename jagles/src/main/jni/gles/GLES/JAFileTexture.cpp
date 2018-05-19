#include <android/log.h>
#include "JAFileTexture.h"
#include "../../ffmpeg/include/libavcodec/avcodec.h"


GLuint LoadFileTexture(char *fn,int *w,int *h)
{
	AVFormatContext *pFormatCtx;
	int             i, videoindex, audioindex;
	AVCodecContext  *pCodecCtx, *pAudioCodecCtx;
	AVCodec         *pCodec, *pAudioCodec;
	AVFrame *pFrame, *pFrameRGB, *pFrameAudio;
	AVPacket *packet;
	struct SwsContext *img_convert_ctx;
	bool mRunthread;
	void *mCtx;
	uint8_t *mVideoBuffer;
	int     mVideoBufferSize;
	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();
	if (avformat_open_input(&pFormatCtx, fn, NULL, NULL) != 0) {
		printf("Couldn't open input stream.\n");
		return 0;
	}
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
		printf("Couldn't find stream information.\n");
		return 0;
	}
	videoindex = -1;
	audioindex = -1;
	for (i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoindex = i;
			//break;
		}
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			audioindex = i;
			//break;
		}
	}
	if (videoindex == -1) {
		printf("Didn't find a video stream.\n");
		return 0;
	}

	pCodec = avcodec_find_decoder(pFormatCtx->streams[videoindex]->codec->codec_id);
	if (pCodec == NULL) {
		printf("Codec not found.\n");
		return 0;
	}
	pCodecCtx = avcodec_alloc_context3(pCodec);

	pCodecCtx->width = pFormatCtx->streams[videoindex]->codec->width;
	pCodecCtx->height = pFormatCtx->streams[videoindex]->codec->height;
	pCodecCtx->pix_fmt = pFormatCtx->streams[videoindex]->codec->pix_fmt;
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		printf("Could not open codec.\n");
		return 0;
	}
	*w = pCodecCtx->width;
	*h = pCodecCtx->height;

	pFrame = av_frame_alloc();
	pFrameRGB = av_frame_alloc();

	mVideoBufferSize = avpicture_get_size(AV_PIX_FMT_BGR32, pCodecCtx->width, pCodecCtx->height);
	mVideoBuffer = (uint8_t *)malloc(mVideoBufferSize);
	avpicture_fill((AVPicture *)pFrameRGB, mVideoBuffer, AV_PIX_FMT_BGR32, pCodecCtx->width, pCodecCtx->height);

	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

#define FREEFRAME 		av_frame_free(&pFrame); \
						av_frame_free(&pFrameRGB); \
						free(mVideoBuffer);  \
						avformat_close_input(&pFormatCtx);
						

	if (av_read_frame(pFormatCtx, packet) < 0)
	{
		FREEFRAME;
		return 0;
	}
	int ret, got_picture;
	got_picture = 0;
	ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
	if (ret < 0) {
		printf("Decode Error.\n");
		FREEFRAME;
		return 0;
	}
	if (!got_picture)
	{
		printf("not got picture!\n");
		FREEFRAME;
		return 0;
	}
	img_convert_ctx = sws_getContext(*w, *h, pCodecCtx->pix_fmt, *w, *h, AV_PIX_FMT_BGR32, SWS_BICUBIC, NULL, NULL, NULL);
	sws_scale(img_convert_ctx, (const unsigned char *const *)pFrame->data, pFrame->linesize, 0, *h, pFrameRGB->data, pFrameRGB->linesize);
	sws_freeContext(img_convert_ctx);

	GLuint texture_object_id = 0;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture_object_id);
//	LOGDEBUG("1--glGenTextures:%d",texture_object_id);
	//assert(texture_object_id != 0);

	glBindTexture(GL_TEXTURE_2D, texture_object_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *w, *h, 0, GL_RGBA, GL_UNSIGNED_BYTE, mVideoBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	
	FREEFRAME;
	return texture_object_id;
}