#include <assert.h>
#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>

#include "media/NdkMediaCodec.h"
#include "media/NdkMediaExtractor.h"
#include <media/NdkMediaMuxer.h>

#include <android/log.h>
#define TAG "NativeCodec"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

// for native window JNI
#include <android/native_window_jni.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>


class HWCodec {
private:
    ANativeWindow* _window;
    AMediaCodec *_codec;
    AMediaFormat *_fmt;
    AMediaMuxer *_muxer;
};