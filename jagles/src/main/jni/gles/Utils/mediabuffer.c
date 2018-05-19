#include "mediabuffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


#ifdef __cplusplus
extern "C" {
#endif


#ifdef ANDROID
#define LOG_TAG "JAVideo"
#define LOGD(...)__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGV(...)__android_log_print(ANDROID_LOG_DEBUG,"Lee",__VA_ARGS__)
#define INFO(...) {__android_log_write(ANDROID_LOG_INFO,"videoconnect",__VA_ARGS__);}
#define LOGE(...) {__android_log_print(ANDROID_LOG_ERROR, "videoconnect", __VA_ARGS__);}
#define LOGE1(...)
// {__android_log_print(ANDROID_LOG_ERROR, "honglee_0815", __VA_ARGS__);}
#else
#define LOGD(...)
#define LOGV(...)
#define INFO(...)
#define LOGE(...)
#define LOGE1(...)
#endif

typedef struct _mediabuffer {
    unsigned int _readpos[16];
    unsigned int _writepos;
    unsigned int _lastwritepos;
    unsigned int _readcount[16];
    unsigned int _writecount;
    int _readlock[16];
    int _writelock;
    unsigned char *_buf;
    unsigned int _bufsize;
    int _overwriteread[16];
    int _isuse[16];
    int _fps;
    int _bitrate;
    uint64_t _lasttime;
    uint64_t _starttime;
    int _lastwritecount;
    int _bitwritecount;
    pthread_mutex_t _mutex;
} _MEDIA_BUFFER, *PMEDIA_BUFFER;

typedef struct _bufhdr {
    unsigned int _flag;
    unsigned int _buftype;
    unsigned int _buflen;
//#ifdef __ANDROID__
//    uint32_t _timestamp;
//#else
	uint64_t _timestamp;
    unsigned int _reallen;
//#endif
	unsigned int _playtime;
	char     _enc[8];
    unsigned int _prevpos;
    unsigned int _nextpos;
} _BUFHDR, *PBUFHDR;

uint64_t clock_ms()
{
     uint64_t ts = 0;
#ifndef _MSC_VER
    struct timespec sys_clk;
#else
	struct timeval sys_clk;
#endif
//	clock_gettime(CLOCK_MONOTONIC, (struct timeval *) &sys_clk);
#ifdef __APPLE__
    clock_gettime(CLOCK_MONOTONIC, &sys_clk);
#else
    clock_gettime(CLOCK_MONOTONIC, (struct timeval *)&sys_clk);
#endif

    ts = sys_clk.tv_sec;
    ts *= 1000;
#ifdef _MSC_VER
		ts += sys_clk.tv_usec /1000;
#else
		ts += sys_clk.tv_nsec / 1000000;
#endif
		//printf("clock_ms return:%ld................%ld,%ld\n", ts,sys_clk.tv_sec,sys_clk.tv_usec);
    return ts;
}

unsigned long InitMediaBuffer(int size) {
    PMEDIA_BUFFER p = (PMEDIA_BUFFER) malloc(size);
    if (!p) {
        return 0;
    }
    memset((void *) p, 0, sizeof(_MEDIA_BUFFER));
    p->_buf = (unsigned char *) malloc(size);
    if (!p->_buf) {
        free((void *) p);
        return 0;
    }
    p->_lastwritepos = 0;
    p->_bufsize = size;
    pthread_mutex_init(&p->_mutex,NULL);
    return (unsigned long) p;
}
    
void ResetMediaBuffer(unsigned long pHandle)
{
    PMEDIA_BUFFER p = (PMEDIA_BUFFER) pHandle;
    if (!p) {
        return;
    }
#ifdef _MSC_VER
	if (!p->_mutex)
		return;
#endif
    pthread_mutex_lock(&p->_mutex);
	pthread_mutex_t tmpmutex = p->_mutex;
    unsigned char *tmpbuf=p->_buf;
    int tmpsize = p->_bufsize;
    int tmpisuse[16];
    int i;
    for (i=0; i<16; i++) {
        tmpisuse[i] = p->_isuse[i];
    }
    memset((void *)p,0,sizeof(_MEDIA_BUFFER));
    p->_bufsize = tmpsize;
    p->_buf = tmpbuf;
    for(i=0;i<16;i++)
        p->_isuse[i] = tmpisuse[i];
	p->_mutex = tmpmutex;
    pthread_mutex_unlock(&p->_mutex);
    LOGD("ResetMediaBuffer---><><>");
}

unsigned int GetWriteCount(unsigned long pHandle) {
    PMEDIA_BUFFER p = (PMEDIA_BUFFER) pHandle;
    if (!p) {
        return 0;
    }
    return p->_writecount;
}

unsigned int GetReadCount(unsigned long pHandle, int index) {
    PMEDIA_BUFFER p = (PMEDIA_BUFFER) pHandle;
    if (index < 0 || index > 16) {
        return 0;
    }
    if (!p) {
        return 0;
    }
    return p->_readcount[index];
}


int GetReadIndex(unsigned long pHandle) {
    PMEDIA_BUFFER p = (PMEDIA_BUFFER) pHandle;
    int i;
    if (!p) {
        return MB_INVALID_HANDLE;
    }
    for (i = 0; i < 16; i++) {
        if (!p->_isuse[i]) {
            p->_overwriteread[i] = 1;
            p->_isuse[i] = 1;
            return i;
        }
    }
    return MB_INVALID_HANDLE;
}

void ReleaseReader(unsigned long pHandle, int index) {
    if (index < 0 || index > 15)
        return;
    PMEDIA_BUFFER p = (PMEDIA_BUFFER) pHandle;
    if (!p) {
        return;
    }
    p->_isuse[index] = 0;
}

int unlockRead(unsigned long pHandle, int index) {
    PMEDIA_BUFFER p = (PMEDIA_BUFFER) pHandle;
    if (!p) {
        return MB_INVALID_HANDLE;
    }
    if (index < 0 || index > 15)
        return MB_INVALID_INDEX;
    AtomicSwap(0, &p->_readlock[index], p->_readlock[index]);//(&p->_readlock[index]);
    pthread_mutex_unlock(&p->_mutex);
    return MB_SUCCESS;
}

int ReadBuffer( unsigned long pHandle,
                unsigned char **buf,
                unsigned int *pReadsize,
                int *pType,
//#ifdef __ANDROID__
//                uint32_t *pTimestamp,
//#else
                uint64_t *pTimestamp,
//#endif
                unsigned int *pPlaytime,
                int index, char* _enc) {
    PMEDIA_BUFFER p = (PMEDIA_BUFFER) pHandle;
    if (!p) {
        return MB_INVALID_HANDLE;
    }
    if (!p->_buf)
    {
        LOGV("ReadBuffer----media buffer pbuf null----------------------------------");
        return MB_INVALID_HANDLE;
    }
    if (index < 0 || index > 15)
        return MB_INVALID_INDEX;
    if (p->_readpos[index] == p->_writepos) {
        return MB_NO_BUFFER;
    }
//    while (p->_writelock) {
//#ifdef _MSC_VER
//		Sleep(1);
//#else
//        usleep(10);
//#endif
//    }
    pthread_mutex_lock(&p->_mutex);
    int loverwrite = 0;
    AtomicInc(&p->_readlock[index]);
    int rt = MB_NO_BUFFER;
    while (1) {
        int lreadpos = p->_readpos[index];
        PBUFHDR lhdr = (PBUFHDR) (p->_buf + lreadpos);
        if (p->_overwriteread[index]) {
            lreadpos = p->_lastwritepos;
            if (lreadpos < 0) {
                lreadpos = 0;
            }
            int lfound = 0;
            lhdr = (PBUFHDR) (p->_buf + lreadpos);
            p->_readcount[index] = p->_writecount-1;
            while (lreadpos >=0 && lhdr->_prevpos >=0) {
                if (lhdr->_buftype == MB_FT_IFRAME && lhdr->_flag == 0xffefface) {
                    lfound = 1;
                    break;
                }
                if(lreadpos==0&&lhdr->_prevpos==0)
                    break;
                lreadpos = lhdr->_prevpos;
                lhdr = (PBUFHDR) (p->_buf + lreadpos);
                p->_readcount[index]--;
            }
            if (!lfound) {
                break;
            }
			MB_INFO(".........................haved overwrite, found key... %d\n", index);
            AtomicSwap(loverwrite, &p->_overwriteread[index], p->_overwriteread[index]);
            //p->_overwriteread[index] = 0;
        }
        //MB_INFO("........................... flag:%x %x",lhdr->_flag,lhdr);
        if (lhdr->_flag != 0xffefface) {
            lreadpos = 0;
            lhdr = (PBUFHDR) p->_buf;
            MB_INFO(".................Error Header! Loop %d\n", index);
        }
        if (*pReadsize < lhdr->_buflen) {
            rt = MB_TOO_SMALL;
            break;
        }
        //*pReadsize = lhdr->_buflen;
        *pReadsize = lhdr->_reallen;
        *pType = lhdr->_buftype;
        *pTimestamp = lhdr->_timestamp;
		*pPlaytime = lhdr->_playtime;
		if (lhdr->_enc != NULL)
		{
			strcpy(_enc, lhdr->_enc);
		}
        //memcpy(buf,p->_buf+lreadpos+sizeof(_BUFHDR),lhdr->_buflen);
        *buf = p->_buf + lreadpos + sizeof(_BUFHDR);
        lreadpos = lhdr->_nextpos;
        //if(*pTimestamp)
        AtomicInc(&p->_readcount[index]);
        AtomicSwap(lreadpos, &p->_readpos[index], p->_readpos[index]);
        //MB_INFO("........................... readpos:%d",p->_readpos);
        rt = MB_SUCCESS;
        break;
    }
    if(rt!=MB_SUCCESS);
       pthread_mutex_unlock(&p->_mutex);
    //AtomicDec(&p->_readlock[index]);
    return rt;
}

int WrtieBuffer(unsigned long pHandle,
                unsigned char *pBuf,
                unsigned int pSize,
                int pType,
//#ifdef __ANDROID__
//                uint32_t pTimestamp,
//#else
                uint64_t pTimestamp,
//#endif
                unsigned int pPlaytime,
                const char* _enc) {

    PMEDIA_BUFFER p = (PMEDIA_BUFFER) pHandle;
    int i;
    if (!p) {
        return MB_INVALID_HANDLE;
    }
    if (!p->_buf) {
        LOGV("writebuffer----media buffer pbuf null----------------------------------");
        return MB_INVALID_HANDLE;
    }

    pthread_mutex_lock(&p->_mutex);
    LOGE1("startwriting.............................. %llu,frame_type:%u",pTimestamp,pType);
    AtomicInc(&p->_writelock);
    if(!p->_starttime)
        p->_starttime = clock_ms();
    unsigned int lwritepos = p->_writepos;
    PBUFHDR phdr;
    int lfound = 0;
    for (i = 0; i < 16; i++) {
        if (lwritepos < p->_readpos[i] && lwritepos + sizeof(_BUFHDR) + pSize > p->_readpos[i]) {
//            if (p->_readlock[i]) {
//                phdr = (PBUFHDR) (p->_buf + p->_readpos[i]);
//                lwritepos = phdr->_nextpos;
//                lfound = 1;
//            }
            int loverwrite = 1;
			MB_INFO("...........................write overwrite %d\n", i);
            AtomicSwap(loverwrite, &p->_overwriteread[i], p->_overwriteread[i]);
        }
    }
//    if (lfound) {
//        phdr = (PBUFHDR) (p->_buf + p->_lastwritepos);
//        //AtomicSwap(lwritepos, &phdr->_nextpos, phdr->_nextpos);
//		phdr->_nextpos = lwritepos;
//        MB_INFO("read lock................");
//    }
    if (lwritepos + sizeof(_BUFHDR) > p->_bufsize || lwritepos + sizeof(_BUFHDR) + pSize > p->_bufsize) {
        lwritepos = 0;
        MB_INFO(".................. loop...\n");
    }
    //MB_INFO("........................... writepos1:%d %x",p->_writepos,p->_buf);
    PBUFHDR lhdr = (PBUFHDR) (p->_buf + lwritepos);
    lhdr->_flag = 0xffefface;
    lhdr->_buftype = pType;
	lhdr->_timestamp = pTimestamp;
	lhdr->_playtime = pPlaytime;
    lhdr->_prevpos = p->_lastwritepos;
	memset(lhdr->_enc, 0, 8);
	if (_enc != NULL) {
		strncpy(lhdr->_enc, _enc,5);
	}
    memcpy(p->_buf + lwritepos + sizeof(_BUFHDR), (void *) pBuf, pSize);
    lhdr->_reallen = pSize;
    if((pSize%4)!=0)
        pSize = (pSize/4+1)*4;
    lhdr->_buflen = pSize;
    p->_lastwritepos = lwritepos;
    //if(pType)
    AtomicInc(&p->_writecount);
    int lbitrate = p->_bitwritecount+pSize;
    AtomicSwap(lbitrate,&p->_bitwritecount,p->_bitwritecount);
    lwritepos = lwritepos + sizeof(_BUFHDR) + pSize;
    lhdr->_nextpos = lwritepos;
    AtomicSwap(lwritepos, &p->_writepos, p->_writepos);
    AtomicSwap(0,&p->_writelock,p->_writelock);
    pthread_mutex_unlock(&p->_mutex);
    LOGE1("endwriting..............................");
    if(!p->_lasttime) {
        p->_lasttime = clock_ms();
        p->_lastwritecount = p->_writecount;
    }
    else
    {
        uint32_t durationtime = clock_ms() - p->_starttime;
        if(durationtime>1000)
        {
            p->_lasttime = clock_ms();
            int lfps = (int)(p->_writecount/((float)durationtime/1000.0f));

            AtomicSwap(lfps,&p->_fps,p->_fps);
            p->_lastwritecount = p->_writecount;
            int lcount = (int)(p->_bitwritecount/((float)durationtime/1000.0f));
            AtomicSwap(lcount,&p->_bitrate,p->_bitrate);
            //p->_bitwritecount = 0;
            //printf("bitrate:%d fps:%d %d %d %d\n",p->_bitrate,p->_fps,p->_lasttime,p->_writecount);
        }
    }
    return MB_SUCCESS;
}


void ReleaseBuffer(unsigned long pHandle) {

    LOGV("Java_com_juanvision_video_GLVideoConnect_DestroyManager  ReleaseBuffer");
    PMEDIA_BUFFER p = (PMEDIA_BUFFER) pHandle;
    int i;
    if (!p) {
        return;
    }
//    int reson = 0;
//    LOGV("Java_com_juanvision_video_GLVideoConnect_DestroyManager  for (i = 0; i < 15; i++) 00000");
//    for (i = 0; i < 15; i++) {
//        if (p->_isuse[i]) {
//            reson = reson | p->_readlock[i];
//        }
//    }
//    LOGV("Java_com_juanvision_video_GLVideoConnect_DestroyManager  for (i = 0; i < 15; i++) 11111");
//    int timeout=0;
//    while (reson || p->_writelock) {
//        LOGV("Java_com_juanvision_video_GLVideoConnect_DestroyManager   p->_writelock = %d",p->_writelock);
//#ifdef _MSC_VER
//		Sleep(1);
//#else
//		usleep(1000);
//#endif
//		//usleep(10);
//        reson = 0;
//        LOGV("Java_com_juanvision_video_GLVideoConnect_DestroyManager   while (reson || p->_writelock) 00000");
//        for (i = 0; i < 15; i++) {
//            if (p->_isuse[i]) {
//                LOGV("Java_com_juanvision_video_GLVideoConnect_DestroyManager   reson = %d",reson);
//                reson = reson | p->_readlock[i];
//                LOGV("Java_com_juanvision_video_GLVideoConnect_DestroyManager   p->_readlock[i] = %d   reson = %d",p->_readlock[i],reson);
//            }
//        }
//        timeout++;
//        if(timeout>3000)
//            break;
//    }
//    LOGV("Java_com_juanvision_video_GLVideoConnect_DestroyManager   while (reson || p->_writelock) 11111");
    pthread_mutex_lock(&p->_mutex);
    free(p->_buf);
    p->_buf = NULL;
    pthread_mutex_unlock(&p->_mutex);
    pthread_mutex_destroy(&p->_mutex);
    free(p);
}


int GetCurrentFPS(unsigned long pHandle)
{
    PMEDIA_BUFFER p = (PMEDIA_BUFFER) pHandle;
    if (!p) {
        return MB_INVALID_HANDLE;
    }
    return p->_fps;
}

int GetCurrentBitrate(unsigned long pHandle)
{
    PMEDIA_BUFFER p = (PMEDIA_BUFFER) pHandle;
    if (!p) {
        return MB_INVALID_HANDLE;
    }
    return p->_bitrate;
}

#ifdef __cplusplus
}
#endif
