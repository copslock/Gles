//
// Created by LiHong on 16/3/21.
// Copyright (c) 2016 LiHong. All rights reserved.
//
#pragma once
#ifndef IOS_JAPLAYER_H
#define IOS_JAPLAYER_H


#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include "../../ffmpeg/include/libavcodec/avcodec.h"
#include "../../ffmpeg/include/libswscale/swscale.h"
#include "../../ffmpeg/include/libavformat/avformat.h"
#include "../../ffmpeg/include/libavutil/avutil.h"
#include "../../ffmpeg/include/libavutil/dict.h"
#include "../../ffmpeg/include/libavutil/opt.h"
#include "../../ffmpeg/include/libavutil/time.h"
//#include <../ffmpeg/include/libswresample/swresample.h>
#if !defined(_MSC_VER) && !defined(__ANDROID__)
#include "OpenALManager.h"
#endif

#ifdef __cplusplus
}
#endif

typedef void(*OnPlayProgress)(int curtime,int duration,bool wallmode);

class JAPlayer {
public:
    JAPlayer(void *ctx);
    ~JAPlayer();

	bool PlayFile(char *filename, bool isimage, bool isfisheye, int64_t startime, OnPlayProgress callback);
    void StopPlay();
    void DoThread();
    int VideoWidth();
    int VideoHeight();
	int WallMode();
	void SeekFile(int mSec);

private:
    pthread_t mPlaythread;
	pthread_mutex_t mLock;
    AVFormatContext *pFormatCtx;
    int             i, videoindex,audioindex;
    AVCodecContext  *pCodecCtx,*pAudioCodecCtx;
    AVCodec         *pCodec,*pAudioCodec;
    AVFrame *pFrame, *pFrameRGB,*pFrameAudio;
    AVPacket *packet;
    struct SwsContext *img_convert_ctx;
    bool mRunthread;
    void *mCtx;
    uint8_t *mVideoBuffer;
    int     mVideoBufferSize;


    //截图
    int mCaptureImage;
    char mCapFileName[1024];

    //鱼眼
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
    AVFrame *mCropPic;

	OnPlayProgress mProgress;
	int64_t		   mStarttime;
	bool		   mIsImage;
	bool		   mSeek;
	int			   mSeekSecond;

    void TestCircRound(AVFrame *f,int *left,int *right,int *top,int *bottom,unsigned char opt);
	void DetectCircleData(int *left, int *right, int *top, int *bottom, int height, int width);
    void DoScale();
};

#endif //IOS_JAPLAYER_H
