//
// Created by LiHong on 16/2/9.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef FISHEYE_JAMEDIA_H
#define FISHEYE_JAMEDIA_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../../ffmpeg/include/libavcodec/avcodec.h"
#include "../../ffmpeg/include/libavformat/avformat.h"
#include "../../ffmpeg/include/libswscale/swscale.h"
#include "../../ffmpeg/include/libavutil/avutil.h"
#include "../../ffmpeg/include/libavutil/dict.h"
#include "../../ffmpeg/include/libavutil/opt.h"
#include "../../ffmpeg/include/libswresample/swresample.h"

#ifdef __cplusplus
}
#endif

#ifdef __ANDROID__
#include "../GLES/GraphicBuffer.h"


#endif

//#include "PacketQueue.hpp"

class JAMedia {
private:
	AVCodec *mCodec;
	AVCodecContext *mContext;
	AVFrame *mPicture;
	AVFrame *mCropPic;
	AVFrame *mOSDPic;
	AVFrame *mOSDOutPic;
	AVFrame *mOutpic;
	unsigned char  *mVideoBuffer;
	int mVideoBufferSize;
	enum AVCodecID mCodecID;
	AVPixelFormat mDecoderFmt;
	bool mCropFishEye;
	bool mCropCut;
	int mWallMode;
	int mCropLeft;
	int mCropTop;
	int mCropRight;
	int mCropBottom;
	int mCropWidth;
	int mCropHeight;
	int mVideoWidth;
	int mVideoHeight;
	int mOSDWidth;
	int mOSDHeight;
	unsigned char *mOSDBuffer;
	int mOSDBufferSize;

    enum AVCodecID mCurrentCodec;

	int mRadius;
	int mCenterX;
	int mCenterY;

	bool isRecord;
	bool isVideoDecoderActive;

	//测试录像
	FILE *testfp;

	//截图

#ifdef WIN32
	unsigned int mCaptureImage;
#else
	int mCaptureImage;
#endif
	char mCapFileName[1024];
	int CaptureImageType;
	//录像
#ifdef WIN32
	unsigned int mRecord;
#else
	int mRecord;
#endif
	bool mWriteRecord;
	char mRecordFileName[1024];
	AVOutputFormat *mRecordfmt;
	AVFormatContext *mRecordContext;
	int mPts;
	uint8_t mExtraData[1024];
	int     mExtraSize;


	void DetectCircleData(int *left, int *right, int *top, int *bottom, int height, int width);
	void TestCircRound(AVFrame *f, int *left, int *right, int *top, int *bottom, unsigned char opt);
	int WritePNG(AVFrame *pFrame, char *filename, int w, int h);
	int WritePNG(AVFrame *pFrame, char *filename, int srcW, int srcH, int dstW, int dstH);

    /* G711 --> AAC */
    SwrContext *mSwrCtx;
	AVCodecContext *mAudioEncCtx;
    AVCodec *audio_codec;
    AVStream *audio_st;
	AVFrame *mAudioFrame;
	AVPacket mAudioPkt;
    bool mAudioEncReady;
    int apts;
    int vpts;
    int mfps;
    
    bool EncodeAACAudioBuf(uint8_t *inbuf, AVPacket *outbuf, int buflen);
    
    
    /* aac decoder */
    AVCodecContext *mAudioCodecCtx;
    AVFrame *mAucioFrame;
    bool mAudioDecoderAvaliable;
    int mAudioChannels;
    int mAudioSampleRate;
	uint8_t *mAudioOutputData;
	
	/* Added by Jason */
	//  PacketQueue *mAudio_pkt_queue; /* packet queue 回放专用 */
	//  PacketQueue *mVideo_pkt_queue;

	bool mISP720Dev;
	bool mIsP360ev;

public:
	void(*OnCaptureImage)(bool success, char* path, void* pJAConnect);
	void(*OnRecordVideo)(bool success, char* path, void * pJAConnect);
#ifdef __ANDROID__ 
	void(*OnNotSupportDirectTexture)(bool success, void * pJAConnect);
#endif

	JAMedia();
	~JAMedia();

	int InitVideoDecoder(enum AVCodecID codec, unsigned char  *header, int headersize, AVPixelFormat fmt);
	int AnalystHeader(enum AVCodecID codec,unsigned char *header,int headersize,AVPixelFormat fmt);
	int DecodeVideo(unsigned char  *inbuf, int insize);
	void CloseVideoDecoder();

    int InitAudioDecoder(enum AVCodecID codec,int pSample,int pChannel,int bit, int nb_samples);
    void DecodeAudio(unsigned char  *inbuf,int insize, unsigned char *outbuf, int *outsize);
    void CloseAudioDecoder();

	int AudioResample(AVCodecContext * audio_dec_ctx, AVFrame * pAudioDecodeFrame, int out_sample_fmt, int out_channels, int out_sample_rate, char *buf);

	void EnableCrop(bool value);
	bool GetCrop();
	void CaptureImage(char *filename, int type, void* mJAConnect); /* capture next frame. */
	void CaptureCurImage(char *filename, void *mJAConnect);   /* capture current frame. */
	int StartRecord(char *filename, bool h265, int fps, int bitrate, void* mJAConnect);
	void WriteFrame(uint8_t *buf, int buflen, int buf_type, int duration);
	void StopRecord();

	unsigned char  *GetVideoBuffer();
	int       GetVideoBufferSize();

	unsigned char *GetVideoBufferYUV(int yuv);
	int       GetOSDBufferSizeYUV(int yuv);

	unsigned char  *GetOSDBuffer();
	int       GetOSDBufferSize();

	unsigned char *GetOSDBufferYUV(int yuv);
	bool WriteTextOSDDirectTextureFrame(void* buffer);


	int VideoWidth();
	int VideoHeight();
	int OSDWidth();
	int OSDHeight();
	int WallMode();
	bool IsRecord();

	void * pJAConnect;
	bool isDirectTexture;
	int directTextureWidth;
	int directTextureHieght;

	void SetIsP720Dev(bool pisp720);
	bool getIsP720Dev();

	void SetIsP360Dev(bool pisp360,float centerx,float centery,float radius,int wallmode);
	bool getIsP360Dev();

	void GetFisheyeParameter(float *centerx,float *centery,float *radius);


#ifdef __ANDROID__
	GraphicBuffer* mGraphicBuffer;
	GraphicBuffer* mGraphicBufferOSD;
#endif // __ANDROID__

	void UpdateCircleParameter(int radius, int centerx, int centery);
	void UpdateCircleParameter(int radius, int centerx, int centery, int wallmode);
	void ResetData();
	/* Added by Jason */
	void AddBuf2PacketQueue(void *pframe,
		int plen,
		int ptype,
		int pw,
		int ph,
		int psampleRate,
		int psampleWidth,
		int psampleChn,
		uint64_t pts,
		int gentime);
	int GetBufFromPacketQueue(void *frame,
		unsigned int *len,
		int type,
		int *width,
		int *height,
		int *psampleRate,
		int *sampleWidth,
		int *sampleChn,
		uint64_t *ts,
		int *gentime);
	int GetPacketQueued(int type);  /* 获取PacketQueue中的buffer数 0音频、1视频 */
	void ClearPacketQueue();
    struct SwsContext *img_convert_ctx;
};


#endif //FISHEYE_JAMEDIA_H
