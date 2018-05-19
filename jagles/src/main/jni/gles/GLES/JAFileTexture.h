#pragma once
#ifdef __cplusplus 
extern "C" {
#endif

#include "../../ffmpeg/include/libavcodec/avcodec.h"
#include "../../ffmpeg/include/libswscale/swscale.h"
#include "../../ffmpeg/include/libavformat/avformat.h"
#include "../../ffmpeg/include/libavutil/avutil.h"
#include "../../ffmpeg/include/libavutil/dict.h"
#include "../../ffmpeg/include/libavutil/opt.h"
#include "../../ffmpeg/include/libswresample/swresample.h"

#ifdef __cplusplus
}
#endif
#include "GLESMath.h"

GLuint LoadFileTexture(char *fn, int *w, int *h);
