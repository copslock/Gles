//
// Created by LiHong on 9/6/16.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef OSX_IMAGELOADING_H
#define OSX_IMAGELOADING_H

#include <pthread.h>
#include "ParametricSurface.h"

class ImageLoading:public ParametricSurface {
public:
    ImageLoading(float aspect,void *mgn);
    ~ImageLoading();
    void DrawSelf(int scnindex);
    void SetupBuffer();
    vec3 Evaluate(const vec2& domain);
    vec2 EvaluateCoord(const vec2& domain);
    void StartAnimation(int scnindex);
    void StopAnimation(int scnindex);
    void LoadOSDTexture(int w, int h, GLenum type, GLvoid *pixels) {};
    void LoadOSDTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels) {};
    void PutInfo(GLuint texid, int scnindex) {};
    void ShowOSD(bool value) {};
    void setupProgram(const char *segsrc, const char *fragsrc);
    void setupProgram(const char *segsrc, const char *fragsrc, bool yuv);

    int GetFrameType() { return FRAME_TYPE_HEMISPHERE;}

    void UpdateDiameter(float diameter);
    virtual void ShowRecordStatu(bool value) {};
    virtual void LoadRecordStatuTexture(GLuint texid, int scnindex) {};
    virtual void* GetLoading(int index) {return 0;};
    void UpdateAspect(float aspect) {m_aspect = aspect;};
    virtual void SetIsAnimation(int index, bool animation) {};
    void SetTextureWH(int w, int l) { _texwidth = w; mTexLeft = l; };
    void HandleThread();
private:
    void *_Manager;
    GLuint mTexWidthHandle;
	GLuint mTexLeftWidthHandle;
    GLuint mLightPosHandle;
    pthread_t                       mAnimationThread;
    pthread_mutex_t                 mAnimationLock;
    pthread_cond_t                  mAnimationCond;
    bool                            mRunning;
    int                             mStatus;
    void ThreadPause();
    void ThreadResume();
    float                           mLightPos;
	float						    mTexLeft;
};


#endif //OSX_IMAGELOADING_H
