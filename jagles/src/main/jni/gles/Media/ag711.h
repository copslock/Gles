//
//  ag711.h
//  EseeNet
//
//  Created by Wynton on 15/8/27.
//  Copyright (c) 2015年 CORSEE Intelligent Technology. All rights reserved.
//
#ifndef EseeNet_ag711_h
#define EseeNet_ag711_h

#ifdef __cplusplus
extern "C" {
#endif


void batch_linear2ulaw(unsigned char *dst, const short *src, int len);

void batch_ulaw2linear(short *dst, const unsigned char *src, int len);

void batch_linear2alaw(unsigned char *dst, const short *src, int len);

void batch_alaw2linear(short *dst, const unsigned char *src, int len);


#ifdef __cplusplus
}
#endif

#endif
