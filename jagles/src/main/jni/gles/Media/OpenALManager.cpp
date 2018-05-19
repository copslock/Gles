//
//  OpenALManager.cpp
//  FFmpeg_Player
//
//  Created by JasonChan on 5/5/16.
//  Copyright © 2016 JasonChan. All rights reserved.
//

#include "OpenALManager.h"
#ifdef __ANDROID__
#include <android/log.h>
#define JACONNECT_INFO(...) {__android_log_print(ANDROID_LOG_INFO, "videoconnect", __VA_ARGS__);}
#endif

/* 单例 */
JA_OpenAL* JA_OpenAL::SharedInstance(void *ctx, kAudioType audioType) {
    if (sharedOpenALInstance == NULL) {
        sharedOpenALInstance = new JA_OpenAL;
        sharedOpenALInstance->initOpenAL(NULL);
    }
    sharedOpenALInstance->setAudioType(audioType);
    sharedOpenALInstance->SetCtx(ctx);
//    sharedOpenALInstance->stopSound();  /* 清空队列 */
//    sharedOpenALInstance->playSound();  /* 重新生成新的队列, 准备播放 */
    return sharedOpenALInstance;
}

JA_OpenAL::JA_OpenAL(void *ctx, kAudioType audioType)
{
	initOpenAL(NULL);
	setAudioType(audioType);
	SetCtx(ctx);
}

JA_OpenAL::JA_OpenAL() {}

JA_OpenAL::~JA_OpenAL() {}

void JA_OpenAL::SetCtx(void *ctx) {
    if (ctx)
        mCtx = ctx;
}

int JA_OpenAL::GetQueued() {
    return mQueued;
}

int JA_OpenAL::GetProcessed() {
    return mProcessed;
}

bool JA_OpenAL::initOpenAL(void *ctx){
    m_Device = NULL;
    m_Context = NULL;
    m_sourceID = 0;
    m_debug = false;
    OnOpenALinfoCallback = NULL;

    if (m_Device == NULL)
    {
		
        m_Device = alcOpenDevice(NULL);                      //参数为NULL , 让ALC 使用默认设备
    }

    if (m_Device==NULL)
    {
        printf("[OpenAL] Fail to find device.\n");
		JACONNECT_INFO("m_Device is null......................");
        return false;
    }
    if (m_Context==NULL)
    {
        if (m_Device)
        {
            m_Context =alcCreateContext(m_Device, NULL);      //与初始化device是同样的道理
            alcMakeContextCurrent(m_Context);
			if (m_Context == NULL)
			{
				JACONNECT_INFO("m_Context is null............");
			}
        }
    }

//    alGenSources(1, &m_sourceID);    //初始化音源ID
//    alSourcei(m_sourceID, AL_LOOPING, AL_FALSE);     // 设置音频播放是否为循环播放，AL_FALSE是不循环
//    alSourcef(m_sourceID, AL_SOURCE_TYPE, AL_STREAMING);  // 设置声音数据为流试，（openAL 针对PCM格式数据流）
//    alSourcef(m_sourceID, AL_GAIN, 1.0f);  //设置音量大小，1.0f表示最大音量。openAL动态调节音量大小就用这个方法
//    alDopplerVelocity(1.0);      //多普勒效应，这属于高级范畴，不是做游戏开发，对音质没有苛刻要求的话，一般无需设置
//    alDopplerFactor(1.0);    //同上
    alSpeedOfSound(1.0);   //设置声音的播放速度

//    mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&mutex, NULL);
    if (m_Context==NULL)
    {
        printf("[OpenAL] Fail to init OpenAL.\n");
        return false;
    }
    mCtx = ctx;
    return true;
}

void JA_OpenAL::setDebugMode(bool debug) {
    m_debug = debug;
}

bool JA_OpenAL::updataQueueBuffer() {
    ALint  state;
    int processed ,queued;

    alGetSourcei(m_sourceID, AL_SOURCE_STATE, &state);
    if (state !=AL_PLAYING)
    {
        playSound();
        return false;
    }

    alGetSourcei(m_sourceID, AL_BUFFERS_PROCESSED, &processed);
    alGetSourcei(m_sourceID, AL_BUFFERS_QUEUED, &queued);

    mQueued = queued;
    mProcessed = processed;

    if (m_debug) {
        JACONNECT_INFO("Processed = %d\n", processed);
        JACONNECT_INFO("Queued = %d\n", queued);
    }

    
    if (OnOpenALinfoCallback)
        OnOpenALinfoCallback(queued, processed, mCtx);
    
    while (processed--) /* 释放队列中已经播放的buf */
    {
        ALuint  buffer;
        alSourceUnqueueBuffers(m_sourceID, 1, &buffer);
        alDeleteBuffers(1, &buffer);
    }
    
    return true;
}

void JA_OpenAL::openAudio(unsigned char *pBuffer, unsigned int pLength, int sample_rate) {
//    ALint  state;
//    alGetSourcei(m_sourceID, AL_SOURCE_STATE, &state);
//    if (state == AL_STOPPED) {
//        return;
//    }
    if (m_sourceID == 0)
        return;
    
    pthread_mutex_lock(&this->mutex);
    
//    playSound();

    ALenum  error =AL_NO_ERROR;
    if ((error =alGetError())!=AL_NO_ERROR) {
        pthread_mutex_unlock(&this->mutex);
        return;
    }

    if (pBuffer ==NULL)
    {
        return ;
    }

    updataQueueBuffer();

    if ((error =alGetError())!=AL_NO_ERROR) {
        pthread_mutex_unlock(&this->mutex);
        return;
    }

    ALuint    bufferID =0;
    alGenBuffers(1, &bufferID);

    if ((error = alGetError())!=AL_NO_ERROR)
    {
        printf("Create buffer failed\n");
        pthread_mutex_unlock(&this->mutex);
        return;
    }

    if (sample_rate == 0)
        sample_rate = 44100;

    alBufferData(bufferID, mAudioType, pBuffer , (ALsizei)pLength, sample_rate);
    if ((error =alGetError())!=AL_NO_ERROR)
    {
        pthread_mutex_unlock(&this->mutex);
        return;
    }

    alSourceQueueBuffers(this->m_sourceID, 1, &bufferID);

    if ((error =alGetError())!=AL_NO_ERROR)
    {
        pthread_mutex_unlock(&this->mutex);
        return;
    }
    if ((error=alGetError())!=AL_NO_ERROR)
    {
        alDeleteBuffers(1, &bufferID);
        pthread_mutex_unlock(&this->mutex);
        return;
    }

    pthread_mutex_unlock(&this->mutex);
}


void JA_OpenAL::playSound() {
    if (m_sourceID == 0)
        rebuildQueue();
    ALint  state;
    alGetSourcei(m_sourceID, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING)
    {
        alSourcePlay(m_sourceID);
    }
}

void JA_OpenAL::stopSound() {
    ALint  state;
    alGetSourcei(m_sourceID, AL_SOURCE_STATE, &state);
    if (state != AL_STOPPED)
    {
        alSourceStop(m_sourceID);
    }
    clearQueue(); /* 清空队列 */
}

void JA_OpenAL::clearOpenAL() {
    pthread_mutex_lock(&mutex);
    alDeleteSources(1, &m_sourceID);
    if (m_Context != NULL)
    {
        alcDestroyContext(m_Context);
        m_Context=NULL;
    }
    if (m_Device !=NULL)
    {
        alcCloseDevice(m_Device);
        m_Device=NULL;
    }
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
}

void JA_OpenAL::clearQueue() {
    pthread_mutex_lock(&mutex);
//    alDeleteSources(1, &m_sourceID);
//    m_sourceID = 0;
    int queued = 0;
    ALuint buffer;
    alGetSourcei(m_sourceID, AL_BUFFERS_QUEUED, &queued);
    printf("%d buffer to delete.\n", queued);
    while (queued > 0) {
        alSourceUnqueueBuffers(m_sourceID, 1, &buffer);
        alDeleteBuffers(1, &buffer);
        queued--;
    }
    printf("Clear Completed.\n");
    alDeleteSources(1, &m_sourceID);
    m_sourceID = 0;
    pthread_mutex_unlock(&mutex);
}

void JA_OpenAL::rebuildQueue() {
    alGenSources(1, &m_sourceID);
    alSourcei(m_sourceID, AL_LOOPING, AL_FALSE);
    alSourcef(m_sourceID, AL_SOURCE_TYPE, AL_STREAMING);
    alSourcef(m_sourceID, AL_GAIN, 1.0f);
    if (OnOpenALinfoCallback)
        OnOpenALinfoCallback(0, 0, mCtx);
}
