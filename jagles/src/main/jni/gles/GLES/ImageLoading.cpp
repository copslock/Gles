//
// Created by LiHong on 9/6/16.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef _MSC_VER
#include <unistd.h>
#endif
#include "ImageLoading.h"

const char *ImageLoadingSeg = "uniform mat4 projection;\n"
        "uniform mat4 modelView;\n"
        "attribute vec4 vPosition;\n"
        "attribute vec2 aTexCoor;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = projection * modelView * vPosition;\n"
        "    vTextureCoord = aTexCoor;\n"
        "}";

const char *ImageLoadingFrag = "precision mediump float;\n"
		"\n"
		"varying vec2 vTextureCoord;\n"
		"uniform sampler2D sTexture;\n"
		"uniform float texwidth;\n"
		"uniform float lightpos;\n"
		"uniform float texleft;\n"
        "\n"
        "void main()\n"
        "{\n"
        "	vec4 cl = texture2D(sTexture,vTextureCoord);\n"
        //"	float clw = (gl_FragCoord.x/texwidth*0.5-lightpos)*.9;\n"
		"	float clw = 1.0-abs((gl_FragCoord.x-texleft)/texwidth-lightpos);\n"
        "	vec4 cl1 = vec4(clw,clw,clw,cl.a);"
        "   gl_FragColor=cl1+cl;\n"
        "}";


void *ImageLoading_AnimationThread(void *arg)
{
    ImageLoading *il = (ImageLoading *)arg;
    il->HandleThread();
    pthread_exit(NULL);
	return NULL;
}

ImageLoading::ImageLoading(float aspect,void *mgn) {
    _Manager = mgn;
    m_aspect = aspect;
    ParametricInterval interval = { 1, vec2(1,1), vec2(2, 2) };
    SetInterval(interval);
#ifdef __USE_RGB32__
    setupProgram(ImageLoadingSeg,ImageLoadingFrag);
#else
    setupProgram(ImageLoadingSeg,ImageLoadingFrag,true);
#endif
    setupProjection();
    SetupBuffer();
    _Zoom = -2.74;
    pthread_mutex_init(&mAnimationLock,NULL);
    pthread_cond_init(&mAnimationCond,NULL);
    mStatus = 0;
    mRunning = true;
    Visible = false;
    mLightPos = -2.0;
    pthread_create(&mAnimationThread,NULL,ImageLoading_AnimationThread,(void *)this);
}

ImageLoading::~ImageLoading() {
	mRunning = false;
	ThreadResume();
	pthread_join(mAnimationThread, NULL);
}

void ImageLoading::DrawSelf(int scnindex) {
    glUseProgram(_programHandle);
    glUniform1f(mTexWidthHandle, _texwidth);
    glUniform1f(mLightPosHandle, mLightPos);
	glUniform1f(mTexLeftWidthHandle, mTexLeft);
    ParametricSurface::DrawSelf(scnindex);
}

void ImageLoading::SetupBuffer() {
    ParametricSurface::SetupBuffer();
}

vec3 ImageLoading::Evaluate(const vec2 &domain) {
    float u = domain.x, v = domain.y;
    float x = (0.2*u-0.1)*m_aspect;
    float y = 0.2*v-0.1;
    //float x = (2*u-1)*m_aspect;
    //float y = 2*v-1;
    float z = 1;
    return vec3(x, y, z);
}

vec2 ImageLoading::EvaluateCoord(const vec2 &domain) {
    float u = domain.x, v = domain.y;
    float x = u;
    float y = 1-v;
    return vec2(x, y);
}

void ImageLoading::setupProgram(const char *segsrc, const char *fragsrc)
{
    ParametricSurface::setupProgram(segsrc, fragsrc);
    mTexWidthHandle = glGetUniformLocation(_programHandle, "texwidth");
    mLightPosHandle = glGetUniformLocation(_programHandle, "lightpos");
	mTexLeftWidthHandle = glGetUniformLocation(_programHandle, "texleft");
}
void ImageLoading::setupProgram(const char *segsrc, const char *fragsrc, bool yuv)
{
    ParametricSurface::setupProgram(segsrc, fragsrc,yuv);
    mTexWidthHandle = glGetUniformLocation(_programHandle, "texwidth");
    mLightPosHandle = glGetUniformLocation(_programHandle, "lightpos");
	mTexLeftWidthHandle = glGetUniformLocation(_programHandle, "texleft");
}

void ImageLoading::HandleThread() {
    while(mRunning)
    {
        pthread_mutex_lock(&mAnimationLock);
        while(!mStatus)
            pthread_cond_wait(&mAnimationCond,&mAnimationLock);
        mLightPos+=0.01;
        if(mLightPos>2.0)
            mLightPos = -2.0;
        mNeedDraw = true;
        pthread_mutex_unlock(&mAnimationLock);
#ifdef _MSC_VER
        Sleep(10);
#else
        usleep(10000);
#endif
    }
}

void ImageLoading::ThreadPause() {
    if(mStatus)
    {
        pthread_mutex_lock(&mAnimationLock);
        mStatus = 0;
        pthread_mutex_unlock(&mAnimationLock);
    }
}

void ImageLoading::ThreadResume() {
    if(!mStatus) {
        pthread_mutex_lock(&mAnimationLock);
        mStatus = 1;
        pthread_cond_signal(&mAnimationCond);
        pthread_mutex_unlock(&mAnimationLock);
    }
}

void ImageLoading::StartAnimation(int scnindex) {
    Visible = true;
    ThreadResume();
}

void ImageLoading::StopAnimation(int scnindex) {
    ThreadPause();
    Visible = false;
}

