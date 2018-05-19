#ifndef __MEDIABUFFER__
#define __MEDIABUFFER__

#define MB_SUCCESS 0          //读写成功
#define MB_INVALID_HANDLE -1  //非法缓冲池句柄
#define MB_NO_BUFFER -2       //没有更多的帧可以读
#define MB_TOO_SMALL -3       //读取时传入的缓冲太小，不能容纳需要读取的帧
#define MB_INVALID_INDEX -4   //非法读句柄

//帧类型
#define MB_FT_AUDIO		0
#define MB_FT_IFRAME	1
#define MB_FT_PFRAME	2
#ifdef __ANDROID__
#include <sys/atomics.h>
#include <android/log.h>
#define MB_INFO(...) {__android_log_print(ANDROID_LOG_INFO, "videoconnect", __VA_ARGS__);}
#define AtomicInc(a)  __atomic_inc(a);
#define AtomicDec(a)  __atomic_dec(a);
#define AtomicSwap(a,b,c) __atomic_swap(a,b);
#endif

#ifdef __APPLE__
#include <libkern/OSAtomic.h>
#define MB_INFO(...) {fprintf(stderr, __VA_ARGS__);}
#define AtomicInc(a)  OSAtomicIncrement32(a);
#define AtomicDec(a)  OSAtomicDecrement32(a);
#define AtomicSwap(a,b,c) OSAtomicCompareAndSwap32(c,a,b);
#include <unistd.h>
#include "osxtime.h"
#endif

#ifdef WIN32
#include <windows.h>
#define MB_INFO(...) {fprintf(stderr, __VA_ARGS__);}
#define AtomicInc(a)  InterlockedIncrement(a);
#define AtomicDec(a)  InterlockedDecrement(a);
#define AtomicSwap(a,b,c) InterlockedExchange(b,a);
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <stdint.h>
#endif
#include "wintime.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *获取当前时间毫秒
 *@return       返回毫秒
 */
uint64_t clock_ms();

/*!
 *重置读写指针
 *@param        pHandle 缓冲池句柄，由InitMediaBuffer返回
 */

void ResetMediaBuffer(unsigned long pHandle);

/*!
 *初始化 MediaBuffer
 *@param        pSize 初始化缓冲池大小
 *@return       返回缓冲池句柄
 */
unsigned long InitMediaBuffer(int pSize);

/*!
 *获取读句柄
 *@param        pHandle 缓冲池句柄，由InitMediaBuffer返回
 *@return       返回读句柄
 */
int GetReadIndex(unsigned long pHandle);

/*!
 *释放读句柄
 *@param        pHandle 缓冲池句柄，由InitMediaBuffer返回
 *@param        index 读句柄
 */
void ReleaseReader(unsigned long pHandle, int index);

/*!
 *读取缓冲帧
 *@param        pHandle 缓冲池句柄，由InitMediaBuffer返回
 *@param        buf 需要把帧读取出来写入的缓冲
 *@param        pReadsize 读取的帧的大小
 *@param        pType 读取帧的类型，参考MB_FT_*
 *@param        index 读句柄
 *@return       返回值参考MB_*
 */
//int ReadBuffer(unsigned long pHandle, unsigned char **buf, unsigned int *pReadsize, int *pType, uint64_t *pTimestamp, unsigned int *pPlaytime, int index, char* _enc);
int ReadBuffer( unsigned long pHandle,
        unsigned char **buf,
        unsigned int *pReadsize,
        int *pType,
//#ifdef __ANDROID__
//        uint32_t *pTimestamp,
//#else
        uint64_t *pTimestamp,
//#endif
        unsigned int *pPlaytime,
        int index, char* _enc);
/*!
 *写帧入缓冲池
 *@param        pHandle 缓冲池句柄，由InitMediaBuffer返回
 *@param        pBuf 需要写入帧数据
 *@param        pSize 写入帧的大小
 *@param        pType 写入帧的类型，参考MB_FT_*
 *@return       返回值参考MB_*
 */
//int WrtieBuffer(unsigned long pHandle, unsigned char *pBuf, unsigned int pSize, int pType, uint64_t pTimeStamp, unsigned int pPlaytime, const char* _enc);
int WrtieBuffer(unsigned long pHandle,
        unsigned char *pBuf,
        unsigned int pSize,
        int pType,
//#ifdef __ANDROID__
//        uint32_t pTimestamp,
//#else
        uint64_t pTimestamp,
//#endif
        unsigned int pPlaytime,
        const char* _enc);
/*!
 *解锁read缓冲，在每次读取完后必须调用
 *@param        pHandle 缓冲池句柄，由InitMediaBuffer返回
 *@param        index 读句柄
 *@return       返回值参考MB_*
 */
int unlockRead(unsigned long pHandle, int index);


/*!
 *释放缓冲池
 *@param        pHandle 缓冲池句柄，由InitMediaBuffer返回
 */
void ReleaseBuffer(unsigned long pHandle);

/*!
 *获取总共写入缓冲池的帧数
 *@param        pHandle 缓冲池句柄，由InitMediaBuffer返回
 *@return       返回目前总共写入缓冲池的帧数
 */
unsigned int GetWriteCount(unsigned long pHandle);

/*!
 *获取总共读出缓冲池的帧数
 *@param        pHandle 缓冲池句柄，由InitMediaBuffer返回
 *@param        index 读句柄
 *@return       返回目前总共写入缓冲池的帧数
 */
unsigned int GetReadCount(unsigned long pHandle, int index);

/*!
 *获取当前码流的帧率
 *@param        pHandle 缓冲池句柄，由InitMediaBuffer返回
 *@return       返回目前的实时帧率
 */

int GetCurrentFPS(unsigned long pHandle);

/*!
 *获取当前的码流
 *@param        pHandle 缓冲池句柄，由InitMediaBuffer返回
 *@return       返回目前的实时码流
 */

int GetCurrentBitrate(unsigned long pHandle);


#ifdef __cplusplus
}
#endif

#endif
