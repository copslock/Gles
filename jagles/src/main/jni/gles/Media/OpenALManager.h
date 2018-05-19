//
//  OpenALManager.hpp
//  FFmpeg_Player
//
//  Created by JasonChan on 5/5/16.
//  Copyright © 2016 JasonChan. All rights reserved.
//

/* Last Edit by Jason on 09/21/2016 */

#ifndef OpenALManager_hpp
#define OpenALManager_hpp

#include <stdio.h>

#define JA_OPENAL_INTERFACE

#ifdef __cplusplus
extern "C" {
#endif

#include "al.h"
#include "alc.h"
#include <pthread.h>

#ifdef __cplusplus
}
#endif

typedef enum {
    MONO8 = 0,  /* 单声道 8位 */
    MONO16,     /* 单声道 16位 */
    STEREO8,    /* 双声道 8位 */
    STEREO16,   /* 双声道 16位 */
}kAudioType;


class JA_OpenAL;
static JA_OpenAL *sharedOpenALInstance = NULL;

class JA_OpenAL {
public:

	JA_OpenAL(void *ctx, kAudioType audioType);
	JA_OpenAL();
	~JA_OpenAL();

JA_OPENAL_INTERFACE  static JA_OpenAL *SharedInstance(void *ctx, kAudioType audioType);
JA_OPENAL_INTERFACE void SetCtx(void *ctx);
JA_OPENAL_INTERFACE int GetQueued();
JA_OPENAL_INTERFACE int GetProcessed();

    /**
     *  print the OpenAL state if debug = true
     *
     *  @param debug
     */
JA_OPENAL_INTERFACE void setDebugMode(bool debug);

    /**
     *  Open Audio
     */
JA_OPENAL_INTERFACE void playSound();

    /**
     *  Close Audio
     */
JA_OPENAL_INTERFACE void stopSound();

    /**
     *  Put Audio Buffer in OpenAL Queue.
     *
     *  @param pBuffer     Audio Buffer
     *  @param pLength     Buffer Size
     *  @param sample_rate 音频采样率
     */
JA_OPENAL_INTERFACE void openAudio(unsigned char *pBuffer, unsigned int pLength, int sample_rate);

    /**
     初始化OpenAL
     
     - returns: true:success || false:fail
     */
    bool initOpenAL(void *ctx);
    
    /**
     *  Release OpenAL
     */
    void clearOpenAL();
    
    void clearQueue();    /* 销毁队列中所有音频帧 */
    void rebuildQueue();  /* 重建队列 */

    /**
     *  CallBack to return OpenAL queue state.
     */
    void (*OnOpenALinfoCallback) (int queued, int processed, void *ctx);   /* OpenAL队列状态回调, queue size = 1024 */
    
    void setAudioType(kAudioType audioType) {
        switch (audioType) {
            case MONO8:mAudioType=AL_FORMAT_MONO8;break;
            case MONO16:mAudioType=AL_FORMAT_MONO16;break;
            case STEREO8:mAudioType=AL_FORMAT_STEREO8;break;
            case STEREO16:mAudioType=AL_FORMAT_STEREO16;break;
            default:mAudioType = AL_FORMAT_STEREO16;break;
        }
    }

private:
    ALCcontext  *m_Context;
    ALCdevice   *m_Device;
    ALuint      m_sourceID;
    ALenum      mAudioType;
    int mQueued;
    int mProcessed;
    /**
     *  Play
     *
     *  @return true | false
     */
    pthread_mutex_t mutex;
    bool updataQueueBuffer();
    bool m_debug;

    void *mCtx;
};

#endif /* OpenALManager_hpp */