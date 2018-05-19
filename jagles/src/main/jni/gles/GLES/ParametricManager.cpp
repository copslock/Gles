//
// Created by LiHong on 16/2/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include <stdlib.h>

#ifndef _MSC_VER

#include <unistd.h>

#endif

#include <string.h>
#include <stdio.h>
#include "ParametricManager.h"
//#include "Cylinder.h"
#include "NormalPlay.h"
#include "NormalScreen.h"
//#include "Hemisphere.h"
//#include "Expand.h"
#include "UpDownScreen.h"
//#include "FourScreen.h"
//#include "VR.h"
#include "sphere.h"
//#include "ThreeScreen.h"
//#include "sixscreen.h"
#include "../Utils/mediabuffer.h"
#include "VertexObjectImpl.h"
#include "../GLES/CircleDetection.h"
#include "ImageLoading.h"
#include "OpenALManager.h"
//#include "VR_sphere.h"
//#include "SphereStitch2.h"
#if defined(__USE_NETWORK__)

#include "SphereStitch.h"
#include "JAFileTexture.h"

#if !defined(__ANDROID__)
#include "GPUMotionDetection.h"
#endif
#ifdef __ANDROID__
#define GL_GLEXT_PROTOTYPES

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <JAP2PConnector.h>

#define LOGD(...)__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGV(...)__android_log_print(ANDROID_LOG_DEBUG,"Lee",__VA_ARGS__)
#define INFO(...) {__android_log_write(ANDROID_LOG_INFO,"videoconnect",__VA_ARGS__);}
#define LOGE(...) {__android_log_print(ANDROID_LOG_ERROR, "videoconnect", __VA_ARGS__);}
#endif

#include "VertexObject.h"

#endif

#ifdef __LINUXNVR__
#define ANI_STEP 30
#else
#define ANI_STEP 100
#endif

#define __NEW__

#define SCREENMODE_COUNT 18


#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
extern "C" {
JA_OpenAL *mAudioPlayer = NULL;
}
#endif

void OnEventObjectHidden(ParametricManager *mgn, ParametricSurface *obj, OnAnimationEnd pOnEnd,
                         void *ctx, int index) {
    if (mgn && mgn->OnObjectHidden)
        mgn->OnObjectHidden(mgn, obj, pOnEnd, ctx, index);
}

void OnEventObjectWillVisible(ParametricManager *mgn, ParametricSurface *obj, vec3 pos, vec3 scale,
                              vec3 rotate) {
    if (mgn && mgn->OnObjectWillVisible)
        mgn->OnObjectWillVisible(mgn, obj, pos, scale, rotate);
}


void *AnimationLoop(void *arg) {
    ParametricManager *mgn = (ParametricManager *) arg;
    if (mgn) {
        mgn->DoAnimationThread();
    }

    pthread_exit(NULL);
    return NULL;
}

/*
 * 动画操作
*/
void ParametricManager::AnimationOperator(void *arg) {
    int ltime = clock_ms();
    for (int i = _Ani.length() - 1; i >= 0; i--) {
        PJA_Animation ani = _Ani.at(i);
        int lsteptime;
        if (ani->_isLoop)
            lsteptime = ani->duration;
        else
            lsteptime = ani->duration / ani->_step;
        //if(!(lcount % lsteptime))
        {
            if (!ani->Inertia)
                ani->_value = ani->_value + ani->_unit;
            else {
                float a = Pi / 180;
                float bx = sinf(a * ani->_stepcount * ((float) 90 / ani->_step)) *
                           (ani->_end.x - ani->_start.x);
                float by = sinf(a * ani->_stepcount * ((float) 90 / ani->_step)) *
                           (ani->_end.y - ani->_start.y);
                float bz = sinf(a * ani->_stepcount * ((float) 90 / ani->_step)) *
                           (ani->_end.z - ani->_start.z);
                ani->_value = ani->_start + vec3(bx, by, bz);
            }
            switch (ani->_type) {
                case JA_ANI_TYPE_POSITION:
                    ani->_obj->SetPosition(ani->_value, ani->_istexture, ani->index);
                    break;
                case JA_ANI_TYPE_ROTATE:
                    ani->_obj->setRotate(ani->_value, ani->_istexture, ani->index);
                    break;
                case JA_ANI_TYPE_SCALE:
                    ani->_obj->SetScale(ani->_value, ani->_istexture, ani->index);
                    break;
            }
            ani->_stepcount++;
            if (ani->_stepcount >= ani->_step && !ani->_isLoop) {
                switch (ani->_type) {
                    case JA_ANI_TYPE_POSITION:
                        ani->_obj->SetPosition(ani->_end, ani->_istexture, ani->index);
                        break;
                    case JA_ANI_TYPE_ROTATE:
                        ani->_obj->setRotate(ani->_end, ani->_istexture, ani->index);
                        break;
                    case JA_ANI_TYPE_SCALE:
                        ani->_obj->SetScale(ani->_end, ani->_istexture, ani->index);
                        break;
                }
                if (ani->OnEnd) {
                    pthread_mutex_unlock(&_AniLock);
                    ani->OnEnd(this, (void *) ani);
                    pthread_mutex_lock(&_AniLock);
                }
                ani->_obj->SetIsAnimation(ani->index, false);
                _Ani.remove(i);
                free(ani);
            }
        }
    }
}


/*
 * 开启动画线程
 */
void ParametricManager::DoAnimationThread() {
    unsigned long lcount = 0;
    while (mAniRun) {
        if (_current->NeedDraw())
            if (OnDrawRequest)
                OnDrawRequest(_current, _ctx);
        pthread_mutex_lock(&_AniLock);
        AnimationOperator(NULL);
        //printf("handle time:%d\n", clock_ms() - ltime);
        pthread_mutex_unlock(&_AniLock);
        lcount++;
        //usleep(1000);
#ifdef _MSC_VER
        Sleep(10);
#else
#ifdef __LINUXNVR__
        usleep(40000);
#else
        usleep(10000);
#endif
#endif
    }
}

ParametricManager::ParametricManager(float aspect, void *ctx, char *apppath) {
    //    ParametricList.insert(-1,new OneScreen(1,1,aspect,92.0f,0,0,0,this));
    _aspect = aspect;
    hemisphereSpeel = 1.5;
    hemisphereInertia = 2.8;
    adjustableScale = 1.6;
    lastx = -1;
    lasty = -1;
    startx = -1;
    starty = -1;
    limit_left = 0;
    limit_right = 0;
    limit_top = 0;
    limit_bottom = 0;
    mTexLogoID = 0;
    mLogo = NULL;
    mViewAngle = 60.0;
    mLogoLoading = false;
    isDestroying = false;
    _isUseDirectTexture = false;
    _current_mode = SCRN_NORMAL;
    OnDrawRequest = NULL;
    OnObjectHidden = NULL;
    OnObjectWillVisible = NULL;
#if defined(__USE_NETWORK__)
    mShowCircle = false;
    mPlayaudio = true;
    mAudioPlayer = JA_OpenAL::SharedInstance(NULL, MONO16);
//	mAudioPlayer = new JA_OpenAL(NULL, MONO16);
#endif
    OnTextureAvaible = NULL;
    OnOSDTextureAvaible = NULL;
#if !defined(__NEW__)
    ParametricList.insert(-1, new NormalScreen(aspect, this));
    ParametricList.insert(-1, new Hemisphere(1.5f, aspect, this));
    ParametricList.insert(-1, new Cylinder(0.7f, aspect, this));
    ParametricList.insert(-1, new Expand(2.3f, aspect, this));
    ParametricList.insert(-1, new UpDownScreen(aspect, this));
    ParametricList.insert(-1, new FourScreen(aspect, this));
    //ParametricList.insert(-1,new ToursLoading(0.1f,0.07f,aspect));
    ParametricList.insert(-1, new VR(aspect, this));
    ParametricList.insert(-1, new sphere(1.5f, aspect, this));
    ParametricList.insert(-1, new ThreeScreen(aspect, this));
    ParametricList.insert(-1, new SixScreen(aspect, this));
    //#define MOTIONTRACKINGDEBUG
#ifdef MOTIONTRACKINGDEBUG
    ParametricList.insert(-1, new GPUMotionDetection(2, aspect, this));
#endif
    ParametricList.insert(-1, new VertexObject(1.5f, aspect, this));
#else
    LOGDEBUG("ParametricManager Start Create Parametric------------------");
    ParametricList.insert(-1, new NormalScreen(aspect, this));
    ParametricList.insert(-1, new VertexObjectImpl(aspect, this));
//	ParametricList.insert(-1, new sphere(1.5f, aspect, this));
#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
//	ParametricList.insert(-1, new SphereStitch(1.2f, aspect, this));
#endif
    //ParametricList.insert(-1, new VR_sphere(aspect, this));
    //ParametricList.insert(-1, new SphereStitch2(2.5f, aspect, this));
#endif
    _current = ParametricList.at(0);
    NormalScreen *obj = (NormalScreen *) _current;
    obj->ObjectHidden = OnEventObjectHidden;
    obj->ObjectWillVisible = OnEventObjectWillVisible;
    InitNoVideoTexture();
#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
    mCircle[0] = new CircleFrame(2);
    mDiameter[0] = 1.0;
    mCircle[0]->SetPosition(vec3(-1, 0, 0), false, 0);
    mCircle[1] = new CircleFrame(2);
    mCircle[1]->SetPosition(vec3(1, 0, 0), false, 0);
    mDiameter[1] = 1.0;
#endif
    _texid = 0;
    _texheight = 0;
    _texwidth = 0;
    _screenlock = 0;
    _ctx = ctx;
    pthread_mutex_init(&_AniLock, NULL);
    mAniRun = true;
    int iret = pthread_create(&_AniLoop, NULL, AnimationLoop, (void *) this);
    _audioIndex = 0;
#if defined(__USE_NETWORK__)
    if (apppath) {
        printf("%s\n", apppath);
        strcpy(AppPath, apppath);
    }
    _Textput = new JAText();
    _Textput->LoadFont(AppPath);
#endif
}

ParametricManager::~ParametricManager() {
    isDestroying = true;
    mAniRun = false;
    pthread_join(_AniLoop, NULL);

    delete mCircle[0];
    delete mCircle[1];
    _current = NULL;
    for (int i = 0; i < ParametricList.length(); i++) {
        ISurface *sur = ParametricList.at(0);

//        NormalScreen *obj = (NormalScreen *) sur;
//        obj->ObjectHidden = NULL;
//        obj->ObjectWillVisible = NULL;

        delete sur;
        sur = NULL;
        ParametricList.remove(0);
    }

#if defined(__USE_NETWORK__)
    if (_Textput)
        delete _Textput;
#endif
    glDeleteTextures(1, &_NoVideotexID);
    printf("ParametricManager Destroy............\n");
}

#pragma mark -

long ParametricManager::StartAnimation(ISurface *obj, vec3 pend, int step, int duration, int isloop,
                                       int anitype, bool texture, int scrIndex, bool Inertia,
                                       OnAnimationEnd ponend, void *ctx) {
    if (_current == ParametricList.at(1)) {
        VertexObjectImpl *voi = (VertexObjectImpl *) _current;
        if (voi->GetSwitchAnimation())
            return 0;
    }
    obj->SetIsAnimation(scrIndex, true);
    PJA_Animation ani = (PJA_Animation) malloc(sizeof(JA_Animation));
    memset(ani, 0, sizeof(JA_Animation));
    switch (anitype) {
        case JA_ANI_TYPE_POSITION:
            ani->_start = obj->GetPosition(texture, scrIndex);
            ani->_value = obj->GetPosition(texture, scrIndex);
            break;
        case JA_ANI_TYPE_SCALE:
            ani->_start = obj->GetScale(texture, scrIndex);
            ani->_value = obj->GetScale(texture, scrIndex);
            break;
        case JA_ANI_TYPE_ROTATE:
            ani->_start = obj->GetRotate(texture, scrIndex);
            ani->_value = obj->GetRotate(texture, scrIndex);
            break;
    }
    ani->_end = pend;
    ani->_isLoop = isloop;
    ani->_istexture = texture;
    ani->_obj = obj;
    ani->_type = anitype;
    ani->_step = step;
    ani->_stepcount = 0;
    ani->duration = duration;
    ani->index = scrIndex;
    ani->OnEnd = ponend;
    ani->Inertia = Inertia;
    ani->Ctx = ctx;
    if (ani->_isLoop)
        ani->_unit = ani->_end;
    else
        ani->_unit = vec3((pend.x - ani->_start.x) / step, (pend.y - ani->_start.y) / step,
                          (pend.z - ani->_start.z) / step);
    pthread_mutex_lock(&_AniLock);
    _Ani.insert(-1, ani);
    pthread_mutex_unlock(&_AniLock);
    return (long) ani;
}

void ParametricManager::StopAnimation(long pHandle) {
    PJA_Animation ani = (PJA_Animation) pHandle;
    if (ani) {
        pthread_mutex_lock(&_AniLock);
        int index = _Ani.search(ani);
        if (index >= 0) {
            ani->_obj->SetIsAnimation(ani->index, false);
            _Ani.remove(index);
            free(ani);
        }
        pthread_mutex_unlock(&_AniLock);
    }
}


void ParametricManager::ClearAnimation() {
    pthread_mutex_lock(&_AniLock);
    for (int i = _Ani.length() - 1; i >= 0; i--) {
        bool value = true;
        PJA_Animation ani = _Ani.at(i);
#ifndef EMSCRIPTEN
        ISurface *tempObj;
        for (int j = 0; j < 36; j++) {
            tempObj = (ISurface *) CurrentParametric()->GetLoading(j);
            if (tempObj == ani->_obj) {
                value = false;
                break;
            }
        }
#else
        value = true;
#endif
        if (value) {
            ani->_obj->SetIsAnimation(ani->index, false);
            _Ani.remove(i);
            free(ani);
        }
    }
    pthread_mutex_unlock(&_AniLock);
}


void ParametricManager::ClearAnimation(int index) {
    pthread_mutex_lock(&_AniLock);
    for (int i = _Ani.length() - 1; i >= 0; i--) {
        PJA_Animation ani = _Ani.at(i);
        if (ani->index == index) {
            ani->_obj->SetIsAnimation(ani->index, false);
            _Ani.remove(i);
            free(ani);
        }
    }
    pthread_mutex_unlock(&_AniLock);
}

ISurface *ParametricManager::CurrentParametric() {
    return _current;
}

ISurface *ParametricManager::GetScreen(int index) {
    if (index < 0 || index > SCREENMODE_COUNT)
        return NULL;
    return ParametricList.at(index);
}


void *ParametricManager::GetCtx() {
    return _ctx;
}

//获取选中的video的下标，在正常模式下
int ParametricManager::getVideoIndex() {
    if (_current->GetScreenMode() != SCRN_NORMAL)
        return 0;
    NormalScreen *nor = (NormalScreen *) _current;
    return nor->getVideoIndex();

}

GLuint ParametricManager::GenColorTexture(uint32_t color) {
    unsigned char *pixels;
    GLuint texture_object_id = 0;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture_object_id);
    //assert(texture_object_id != 0);
//    LOGDEBUG("3--glGenTextures:%d",texture_object_id);

    pixels = (unsigned char *) malloc(10 * 10 * 4);

    uint32_t *tmp = (uint32_t *) pixels;
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++) {
            *tmp = color;
            tmp++;
        }

    glBindTexture(GL_TEXTURE_2D, texture_object_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 10, 10, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    free(pixels);
    return texture_object_id;
}

void ParametricManager::InitBorderTexture(uint32_t scolor, uint32_t ncolor) {
    //    NormalScreen *scn = (NormalScreen *) ParametricList.at(0);
    //    scn->SetBorderColor(GenColorTexture(scolor),GenColorTexture(ncolor));
    GLuint scolor1 = GenColorTexture(scolor);
    GLuint ncolor1 = GenColorTexture(ncolor);
    for (int i = 0; i < ParametricList.length(); i++)
        ParametricList.at(i)->SetBorderColor(scolor1, ncolor1);
}

void ParametricManager::InitNoVideoTexture() {
#ifdef EMSCRIPTEN
    return;
#endif
    unsigned char *pixels;
    GLuint texture_object_id = 0;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture_object_id);
//    LOGDEBUG("4--glGenTextures:%d",texture_object_id);
    //assert(texture_object_id != 0);

    pixels = (unsigned char *) malloc(512 * 512 * 4);

    unsigned char *tmp = pixels;
    for (int i = 0; i < 511; i++)
        for (int j = 0; j < 511; j++) {
            unsigned char a = 255 - abs(255 - i);
            *tmp = 0;//255-abs(255-i);
            tmp++;
            *tmp = 0;//255-abs(255-i);
            tmp++;
            *tmp = abs(255 - i);// > 128 ? abs(255-i) : a;
            tmp++;
            *tmp = 0xff;
            tmp++;
        }

    glBindTexture(GL_TEXTURE_2D, texture_object_id);
#ifdef __ANDROID__
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 511, 511, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);

    _NoVideotexID = texture_object_id;

    glBindTexture(GL_TEXTURE_2D, 0);
    free(pixels);
    for (int i = 0; i < ParametricList.length(); ++i) {
        ParametricList.at(i)->SetNoVideoTexture(_NoVideotexID);
    }
}

void ParametricManager::SwitchMode(int mode) {
    LOGV("当前的安装模式是  mode = %d", mode);
    if (mode < 0 || mode > SCREENMODE_COUNT)
        return;
    _current_mode = mode;                            //记录保存当前的mode
    pthread_mutex_lock(&_AniLock);
#if defined(__NEW__)
    if (mode > 0) {
        _current = ParametricList.at(1);
        VertexObjectImpl *voi = (VertexObjectImpl *) _current;
#ifdef EMSCRIPTEN
        voi->SwitchMode(mode, false);
#else
        voi->SwitchMode(mode, !voi->GetSwitchAnimation());
#endif
    } else {
        _current = ParametricList.at(0);
    }

#else
    _current = ParametricList.at(mode);
#endif
    //_current->ResetPosition();
    pthread_mutex_unlock(&_AniLock);
}

void ParametricManager::SetTextMask(char *txt) {
#if defined(__USE_NETWORK__)
    if (!txt)
        return;
    void *pixels = (void *) _Textput->GenTextbuffer(txt, 0xffffffff, 48, 24);
    GLuint texture_object_id = 0;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture_object_id);
//    LOGDEBUG("5--glGenTextures:%d",texture_object_id);
    //assert(texture_object_id != 0);

    glBindTexture(GL_TEXTURE_2D, texture_object_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 48, 24, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    VertexObjectImpl *voi = (VertexObjectImpl *) ParametricList.at(1);
    voi->SetTextTexture(texture_object_id);
    glBindTexture(GL_TEXTURE_2D, 0);
    free(pixels);
#endif
}

void ParametricManager::PutInfo(char *txt, int index) {
//    return;
#if defined(__USE_NETWORK__)

    LOGD("ParametricManager::PutInfo,txt--->%s , index---->%d", txt, index);
    if (!txt)
        return;
    void *pixels = (void *) _Textput->GenTextbuffer(txt, 0xffffffff, 360, 24);
    if (!pixels)
        return;
    GLuint texture_object_id = 0;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture_object_id);
//    LOGDEBUG("6--glGenTextures:%d",texture_object_id);
    //assert(texture_object_id != 0);

    glBindTexture(GL_TEXTURE_2D, texture_object_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 360, 24, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    this->CurrentParametric()->ShowInfo(true);
    if (CurrentParametric()->GetScreenMode() == SCRN_NORMAL) {
        NormalScreen *mNormalScreen = (NormalScreen *) CurrentParametric();
        mNormalScreen->ShowInfo(true, index);
    }
    this->CurrentParametric()->PutInfo(texture_object_id, index);
    glBindTexture(GL_TEXTURE_2D, 0);
    free(pixels);
#endif
}

void ParametricManager::SetSplit(int index, bool ani) {
    /*if (_screenlock)
        return;*/
    if (_current->GetScreenMode() != SCRN_NORMAL)
        return;
    NormalScreen *nor = (NormalScreen *) _current;
    nor->SetSplit(index, ani);
}

void ParametricManager::SetPage(int index, bool ani) {
    /*if (_screenlock)
        return;*/
    if (_current->GetScreenMode() != SCRN_NORMAL)
        return;
    NormalScreen *nor = (NormalScreen *) _current;
    nor->SetPage(index, ani);
}

int ParametricManager::GetPage() {
    if (_current->GetScreenMode() != SCRN_NORMAL)
        return 0;
    NormalScreen *nor = (NormalScreen *) _current;
    return nor->GetPage();
}

int ParametricManager::GetAllPage() {
    if (_current->GetScreenMode() != SCRN_NORMAL)
        return 0;
    NormalScreen *nor = (NormalScreen *) _current;
    return nor->GetAllPage();
}

void ParametricManager::SetSelected(int x, int y, int w, int h) {
    if (CurrentParametric()->GetScreenMode() == SCRN_NORMAL) {
        NormalScreen *scn = (NormalScreen *) CurrentParametric();
        scn->SetSelected(x, y, w, h);
    }
}

void ParametricManager::SetSelected(int index) {
    if (CurrentParametric()->GetScreenMode() == SCRN_NORMAL) {
        NormalScreen *scn = (NormalScreen *) CurrentParametric();
        scn->SetSelected(true, index);
    }
}

int ParametricManager::GetScreenCount() {
    if (CurrentParametric()->GetScreenMode() == SCRN_NORMAL) {
        NormalScreen *scn = (NormalScreen *) CurrentParametric();
        return scn->GetScreenCount();
    }
    return 1;
}

void ParametricManager::RenderFBO() {
    VertexObjectImpl *voi = (VertexObjectImpl *) ParametricList.at(1);
    voi->RenderFBO();
}

int ParametricManager::GetDualMode() {
    if (ParametricList.length() >= 2) {
        VertexObjectImpl *voi = (VertexObjectImpl *) ParametricList.at(1);
        return voi->DualMode();
    }
    return 0;
}

int ParametricManager::GetMode() {
    return _current_mode;
    //	return CurrentParametric()->GetScreenMode();
}

//切换为单屏
void ParametricManager::SetSingVideo(int index, bool ani) {
    if (CurrentParametric()->GetScreenMode() == SCRN_NORMAL) {
        NormalScreen *scn = (NormalScreen *) CurrentParametric();
        scn->SetSplit(SCRN_SPLIT_ONE, ani);
        scn->SetPage(index, ani);
    }
}

int ParametricManager::GetSplitMode() {
    if (CurrentParametric()->GetScreenMode() == SCRN_NORMAL) {
        NormalScreen *scn = (NormalScreen *) CurrentParametric();
        return scn->GetSplitMode();
    }
    return -1;
}

bool ParametricManager::LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex) {
#ifdef __ANDROID__
    if (CurrentParametric()->GetScreenMode() == SCRN_NORMAL && !GetDualMode()) {
        NormalScreen *mNormalScreen = (NormalScreen *) CurrentParametric();
        mNormalScreen->ShowInfo(false, scnindex);
        mNormalScreen->releaseInfo(scnindex);
        //LOGV("---------------------------------");
        return mNormalScreen->LoadTexture(w, h, type, pixels, scnindex);
    }
#endif

    if (_texid)
        glDeleteTextures(1, &_texid);
    _texid = 0;
    _texwidth = w;
    _texheight = h;
    GLuint texture_object_id = 0;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture_object_id);
//    LOGDEBUG("7--glGenTextures:%d",texture_object_id);
    //assert(texture_object_id != 0);

    glBindTexture(GL_TEXTURE_2D, texture_object_id);
    if (type == GL_RGBA) {
        glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, type, GL_UNSIGNED_BYTE, pixels);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, type, GL_UNSIGNED_SHORT_5_6_5, pixels);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
    _texid = texture_object_id;
    for (int i = 0; i < ParametricList.length(); i++) {
        ParametricList.at(i)->SetTexture(_texid, scnindex);
        ParametricList.at(i)->ShowInfo(false);
    }
    return _texid;
}

void ParametricManager::cleanTexture(int index) {
    for (int i = 0; i < ParametricList.length(); i++)
        ParametricList.at(i)->SetTexture(0, index);
}

void ParametricManager::LoadRecordStatuTexture(GLint recordStatusTextID) {
    //	this->CurrentParametric()->ShowRecordStatu(true);
    this->CurrentParametric()->LoadRecordStatuTexture(recordStatusTextID, 0);
}

bool ParametricManager::GetVisibility(int index) {
    if (CurrentParametric()->GetScreenMode() == SCRN_NORMAL) {
        NormalScreen *scn = (NormalScreen *) CurrentParametric();
        return scn->GetVisibility(index);
    }
    return false;
}

void ParametricManager::ReSizeSplite(float aspect) {
    if (CurrentParametric()->GetScreenMode() == SCRN_NORMAL) {
        NormalScreen *scn = (NormalScreen *) CurrentParametric();
        scn->ReSizeSplite(aspect);
    }
}


//统一手势代码、
int convertWindowIndex(int windowIndex) {
    int index = 0;
    switch (windowIndex) {
        case 0:
            index = 2;
            break;
        case 1:
            index = 3;
            break;
        case 2:
            index = 0;
            break;
        case 3:
            index = 1;
            break;
    }
    return index;
}


void ParametricManager::DoDoubleTap(int wallmode, int scene, int scrnFourIndex, int scindex) {

    int index = scindex;
    int screenMode = this->CurrentParametric()->GetScreenMode();
    if (screenMode == SCRN_FOUR) {
        windowIndex = convertWindowIndex(scrnFourIndex);
        index = windowIndex;
    }

    vec3 position = this->CurrentParametric()->GetPosition(false, index);
    vec3 rotation = this->CurrentParametric()->GetRotate(false, index);
    vec3 scale = this->CurrentParametric()->GetScale(false, index);

    switch (screenMode) {
        case SCRN_HEMISPHERE: {
            //双击切换成四分屏
            if (!outsideChanged) {
                this->ClearAnimation();
                this->SwitchMode(SCRN_FOUR);
                this->CurrentParametric()->SetPosition(position, false, windowIndex);
                this->CurrentParametric()->setRotate(rotation, false, windowIndex);
                this->CurrentParametric()->SetScale(scale, false, windowIndex);
                break;
            }

            //模掉多余的圈数
            if (rotation.z > 360 || rotation.z < -360) {
                while (true) {
                    if (rotation.z > 360) {
                        rotation.z -= 360;
                    } else if (rotation.z < -360) {
                        rotation.z += 360;
                    } else {
                        break;
                    }
                }
                this->CurrentParametric()->setRotate(rotation, false, scindex);
            }

            vec3 initVec = vec3(0, 0, 0);
            if (position == initVec) {
                //等于初始位置，放大
                if (scene == 180) {
                    initVec.z = 1.75;
                } else {
                    initVec.z = 3;
                }
                this->StartAnimation(this->CurrentParametric(), initVec, 100, 500,
                                     0, JA_ANI_TYPE_POSITION, false, 0, true, NULL, NULL);

                if (scene == 180 || scene == 360 || scene == 720) {
                    vec3 rotateAni = vec3(0, 0, 0);
                    if (scene == 360 && !wallmode) {
                        rotateAni.x = 45;
                        rotateAni.z = rotation.z + 90;
                    }
                    this->StartAnimation(this->CurrentParametric(),
                                         rotateAni, 100, 500, 0,
                                         JA_ANI_TYPE_ROTATE,
                                         false, 0, true, NULL, NULL);
                }
            } else {
                //不等于初始位置，缩小
                this->StartAnimation(this->CurrentParametric(), initVec, 30, 250, 0,
                                     JA_ANI_TYPE_POSITION, false, 0, true, NULL, NULL);
                if (scene == 180 || scene == 360 || scene == 720) {
                    vec3 rotateAni = vec3(0, 0, 0);
                    if (scene == 360 && !wallmode) {
                        rotateAni.z = rotation.z - 90;
                    }
                    this->StartAnimation(this->CurrentParametric(),
                                         rotateAni, 30, 250, 0,
                                         JA_ANI_TYPE_ROTATE,
                                         false, 0, true, NULL, NULL);
                }
                this->CurrentParametric()->SetScale(vec3(1, 1, 1), false, 0);
            }
            break;
        }
        case SCRN_FOUR:
            //四分屏，双击切换到某一个分屏
            outsideChanged = false;
            this->ClearAnimation();
            this->SwitchMode(SCRN_HEMISPHERE);
            this->CurrentParametric()->SetPosition(position, false, scindex);
            this->CurrentParametric()->setRotate(rotation, false, scindex);
            this->CurrentParametric()->SetScale(scale, false, scindex);
            break;
        default:
            break;
    }
}


void ParametricManager::DoTapOrMouseDown(int x, int y) {
    int px, py;
#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
    if (mShowCircle) {
        mCurrentCircle = -1;
        float lcx, lcy, lcdiameter;
        vec3 lcpos = mCircle[0]->GetPosition(false, 0);
        lcx = (x - (float) window_width / 2) / ((float) window_height / 2) - lcpos.x;
        lcy = (y - (float) window_height / 2) / ((float) window_height / 2) - lcpos.y;
        lcdiameter = sqrtf(lcx * lcx + lcy * lcy);
        if (lcdiameter < mDiameter[0])
            mCurrentCircle = 0;
        if (mShowCircle > 1 && mCurrentCircle < 0) {
            lcpos = mCircle[1]->GetPosition(false, 0);
            lcx = (float) (x - (float) window_width / 2) / ((float) window_height / 2) - lcpos.x;
            lcy = (float) (y - (float) window_height / 2) / ((float) window_height / 2) - lcpos.y;
            lcdiameter = sqrtf(lcx * lcx + lcy * lcy);
            if (lcdiameter < mDiameter[1])
                mCurrentCircle = 1;
        }
    } else
#endif
        switch (this->CurrentParametric()->GetScreenMode()) {
            case SCRN_FOUREX:
                if (y > window_height / 3 * 2)break;
            case SCRN_FOUR:
            case SCRN_THREE:
            case SCRN_SIX:
                mIndex = GetScreenIndex(x, y, this->CurrentParametric()->GetScreenMode());
                if (mIndex >= 0) {
                    this->ClearAnimation(mIndex);
                    this->CurrentParametric()->SetSelected(true, mIndex);
                    this->CurrentParametric()->UpdateFrame(0, 0, 0, 0, mIndex, false, false);
                } else {
                    this->ClearAnimation(this->CurrentParametric()->GetSelected(0));
                    CurrentParametric()->UpdateFrame(x, window_width, y, window_height, mIndex,
                                                     true, false);
                    //this->CurrentParametric()->UpdateFrame(x, window_width,y, window_height, 0,true,false);
                }
                break;
            default:
                if (CurrentParametric()->GetScreenMode() != SCRN_NORMAL)
                    this->ClearAnimation();
                break;
        }
    lastx = x;
    lasty = y;
    startx = x;
    starty = y;
    starttime = clock_ms();
    ////printf("mouse down ......x:%d,y:%d\n", x, y);
}

void ParametricManager::DoTapOrMouseMove(int x, int y, int wallmode, int scindex) {
    int lwallmode = wallmode;
    if (startx < 0)
        return;
    vec3 pos, lafter, lscale, lrotate, ltscale, ltpos;
    int px, py, scale_value;
    static int speed = 0;
    static int start_x = startx;
    static int time_point = clock_ms();

    if (clock_ms() - time_point >= 10) {
        speed = x - start_x;
        time_point = clock_ms();
        start_x = x;
    }
    mSpeed = speed;
#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
    if (mShowCircle) {
        if (mCurrentCircle >= 0) {
            float lcx, lcy, lcdiameter;
            vec3 lcpos = mCircle[mCurrentCircle]->GetPosition(false, 0);
            lcx = (float) (x - lastx) / ((float) window_height / 2);
            lcy = (float) (y - lasty) / ((float) window_height / 2);
            lcpos.x = lcpos.x + lcx;
            lcpos.y = lcpos.y - lcy;
            mCircle[mCurrentCircle]->SetPosition(lcpos, false, 0);
        }
    } else
#endif
        switch (this->CurrentParametric()->GetScreenMode()) {
            case SCRN_NORMAL:
                px = x - lastx;
                py = y - lasty;
                lscale = CurrentParametric()->GetScale(true, scindex);
                scale_value = lscale.x;
                px /= scale_value;
                py /= scale_value;
                pos = this->CurrentParametric()->GetPosition(true, scindex);
                pos.x -= (float) px / window_width;
                pos.y -= (float) py / window_height;
                this->CurrentParametric()->SetPosition(pos, true, scindex);
                break;
            case SCRN_HEMISPHERE:
            case SCRN_VECTOROBJECT:
            case SCRN_CUP:
                ////printf("mouse move ......x:%d,y:%d\n", x, y);
                if (this->GetDualMode()) {
                    lafter = this->CurrentParametric()->GetRotate(false, 0);
                    lafter.x = lafter.x + (float) (y - lasty) / window_height * 90.0;
                    lafter.y = lafter.y + (float) (x - lastx) / window_width * 90.0;
                    this->CurrentParametric()->setRotate(lafter, false, 0);
                } else {
                    pos = this->CurrentParametric()->GetScale(false, 0);
                    lafter = this->CurrentParametric()->GetRotate(false, 0);
                    if (lwallmode) {
                        limit_left = 60 - (60 / pos.y / 2);
                        limit_right = -limit_left;
                        limit_top = 30 - (60 / pos.x / 2);
                        limit_bottom = -limit_top;
                        lafter.x = lafter.x + (float) (y - lasty) / window_height * 90.0;
                        lafter.y = lafter.y + (float) (x - lastx) / window_width * 90.0;
                        if (lafter.y > limit_left + 20)
                            lafter.y = limit_left + 20;
                        if (lafter.y < limit_right - 20)
                            lafter.y = limit_right - 20;
                        if (lafter.x > limit_top + 10)
                            lafter.x = limit_top + 10;
                        if (lafter.x < limit_bottom - 10)
                            lafter.x = limit_bottom - 10;
                        this->CurrentParametric()->setRotate(lafter, false, 0);
                    } else {
                        limit_top = 90 - (mViewAngle / pos.y / 2);
                        limit_bottom = 0;
                        lafter.x = lafter.x + (float) (y - lasty) / window_height * 90.0;
                        lafter.z = lafter.z + (float) (x - lastx) / window_width * 90.0;
                        if (this->CurrentParametric()->GetScreenMode() != SCRN_CUP) {
                            if (lafter.x > limit_top + 10)
                                lafter.x = limit_top + 10;
                            if (lafter.x < limit_bottom - 10)
                                lafter.x = limit_bottom - 10;
                        }
                        this->CurrentParametric()->setRotate(lafter, false, 0);
                    }
                }
                break;
            case SCRN_CYLINDER:
                lscale = this->CurrentParametric()->GetScale(false, 0);
                lrotate = this->CurrentParametric()->GetRotate(false, 0);
                ltscale = this->CurrentParametric()->GetScale(true, 0);
                ltpos = this->CurrentParametric()->GetPosition(true, 0);
                if (abs(x - lastx) < 20 && abs(y - starty) > 20) {
                    ltscale.x += (float) (lasty - y) * 10 / window_height;
                    if (ltscale.x < 1)
                        ltscale.x = 1.0;
                    if (ltscale.x > 6)
                        ltscale.x = 6;
                    if (this->GetDualMode())
                        lrotate.x = -30.0 + (ltscale.x - 1) * 6.0;
                    else
                        lrotate.x = 30.0 - (ltscale.x - 1) * 6.0;
                    lscale = vec3(1.0 + (ltscale.x - 1) * 0.1, 1.0 + (ltscale.x - 1) * 0.1,
                                  1.0 + (ltscale.x - 1) * 0.1);
                    this->CurrentParametric()->SetScale(ltscale, true, 0);
                    this->CurrentParametric()->setRotate(lrotate, false, 0);
                    this->CurrentParametric()->SetScale(lscale, false, 0);
                }
                if (GetDualMode()) {
                    ltpos.x -= (float) (x - lastx) / (float) window_width / Pi;
                    ltpos.x = ltpos.x - floorf(ltpos.x);
//				while(ltpos.x>1.0)
//					ltpos.x -= 1.0;
//				while(ltpos.x<0.0)
//					ltpos.x +=1.0;
                } else
                    ltpos.x += (float) (x - lastx) / window_width;
                this->CurrentParametric()->SetPosition(ltpos, true, 0);
                break;
            case SCRN_UPDOWN:
                ltpos = this->CurrentParametric()->GetPosition(true, 0);
                ltpos.x -= 2 * (float) (x - lastx) / window_width;
                this->CurrentParametric()->SetPosition(ltpos, true, 0);
                ltpos = this->CurrentParametric()->GetPosition(true, 1);
                ltpos.x -= 2 * (float) (x - lastx) / window_width;
                this->CurrentParametric()->SetPosition(ltpos, true, 1);
                break;
            case SCRN_FOUREX:
                if (y > window_height / 3 * 2)break;
            case SCRN_FOUR:
            case SCRN_THREE:
            case SCRN_SIX:
                //index = GetScreenIndex(x, y, mgn, this->CurrentParametric()->GetScreenMode());
                if (this->GetDualMode()) {
                    lafter = this->CurrentParametric()->GetRotate(false, mIndex);
                    lafter.x = lafter.x + (float) (y - lasty) / window_height * 90.0;
                    lafter.y = lafter.y + (float) (x - lastx) / window_width * 90.0;
                    if (this->CurrentParametric()->GetScreenMode() != SCRN_FOUREX ||
                        (this->CurrentParametric()->GetScreenMode() == SCRN_FOUREX &&
                         this->CurrentParametric()->GetSelected(2) != 2))
                        this->CurrentParametric()->setRotate(lafter, false, mIndex);
                    if (this->CurrentParametric()->GetScreenMode() == SCRN_FOUREX &&
                        this->CurrentParametric()->GetSelected(2) == 2) {
                        ltpos = this->CurrentParametric()->GetPosition(true, mIndex);
                        ltpos.x -= 2 * (float) (x - lastx) / window_width;
                        this->CurrentParametric()->SetPosition(ltpos, true, mIndex);
                    }
                } else {
                    if (mIndex >= 0) {
                        lafter = this->CurrentParametric()->GetRotate(false, mIndex);
                        pos = this->CurrentParametric()->GetScale(false, mIndex);
                        //			lafter.z = lafter.z + (float)(x -lastx) /window_width*90.0;
                        limit_top = 90 - (mViewAngle - (100 * (pos.y - 1) * 8)) / 2;
                        limit_bottom = 0;
                        lafter.x = lafter.x + (float) (y - lasty) / window_height * 90.0;
                        lafter.z = lafter.z + (float) (x - lastx) / window_width * 90.0;
                        if (lafter.x > limit_top + 10)
                            lafter.x = limit_top + 10;
                        if (lafter.x < limit_bottom - 10)
                            lafter.x = limit_bottom - 10;
                        this->CurrentParametric()->setRotate(lafter, false, mIndex);
                    } else {
                        this->ClearAnimation(this->CurrentParametric()->GetSelected(0));
                        this->CurrentParametric()->UpdateFrame(x, window_width, y, window_height,
                                                               mIndex, true, false);
                    }
                }
                break;
            case SCRN_SPHERE:
            case SCRN_STITCH2:
                //            case SCRN_STITCH:
                lafter = this->CurrentParametric()->GetRotate(false, 0);
                lafter.x = lafter.x + (float) (y - lasty) / window_height * 90.0;
                lafter.y = lafter.y + (float) (x - lastx) / window_width * 90.0;
                this->CurrentParametric()->setRotate(lafter, false, 0);
                break;
            case SCRN_PANORAMA:
                ltpos = this->CurrentParametric()->GetPosition(true, 0);
                ltpos.x -= (float) (x - lastx) / window_width;
                this->CurrentParametric()->SetPosition(ltpos, true, 0);
                break;
        }
    lastx = x;
    lasty = y;
}

void ParametricManager::DoTapOrMouseUp(int x, int y, int wallmode) {
    if (startx < 0)
        return;
    int lwallmode = wallmode;
    vec3 lafter, lscale, lrotate, ltscale, ltpos;
    int ltime, px, py;
#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
    if (mShowCircle) {
        mCurrentCircle = -1;
    } else
#endif
        switch (this->CurrentParametric()->GetScreenMode()) {
            case SCRN_NORMAL:
                lafter = this->CurrentParametric()->GetPosition(true, 0);
                ltime = clock_ms() - starttime;
                if (ltime < 1500 && ltime > 100 && abs(x - startx) > 30) {
                    ////printf("z...............:%f %f %d\n", lafter.z, (float)ltime / 1000, x -startx);
                    lafter.x -= ((float) (x - startx) / window_width / ((float) ltime / 1000));
                    lafter.y -= ((float) (y - starty) / window_height / ((float) ltime / 1000));
                    this->StartAnimation(this->CurrentParametric(), lafter, ANI_STEP, 500, 0,
                                         JA_ANI_TYPE_ROTATE, true, 0, true, NULL, NULL);
                }
                break;
            case SCRN_HEMISPHERE:
            case SCRN_VECTOROBJECT:
            case SCRN_CUP:
                if (this->GetDualMode()) {
                    lafter = this->CurrentParametric()->GetRotate(false, 0);
                    ltime = clock_ms() - starttime;
                    if (ltime < 800 && ltime > 100 && abs(x - startx) > 30) {
                        lafter.y += 180.0 *
                                    ((float) (x - startx) / window_width / ((float) ltime / 1000));
                        this->StartAnimation(this->CurrentParametric(), lafter, ANI_STEP, 500, 0,
                                             JA_ANI_TYPE_ROTATE, false, 0, true, NULL, NULL);
                    }
                } else {
                    lafter = this->CurrentParametric()->GetRotate(false, 0);
                    if (lwallmode) {
                        if (lafter.y > limit_left)
                            lafter.y = limit_left;
                        if (lafter.y < limit_right)
                            lafter.y = limit_right;
                        if (lafter.x > limit_top)
                            lafter.x = limit_top;
                        if (lafter.x < limit_bottom)
                            lafter.x = limit_bottom;
//				this->StartAnimation(this->CurrentParametric(), lafter, ANI_STEP, 500, 0, JA_ANI_TYPE_ROTATE, false, 0, true, NULL, NULL);
                    } else {
                        ////printf("limit is:%f,%f,current x rotate:%f\n",limit_top,limit_bottom,lafter.x);
                        if (this->CurrentParametric()->GetScreenMode() != SCRN_CUP) {
                            if (lafter.x > limit_top)
                                lafter.x = limit_top;
                            if (lafter.x < limit_bottom)
                                lafter.x = limit_bottom;
                        }
                        //				int ltime = clock_ms() - starttime;
                        //                if (ltime < 1500 && ltime>30&&abs(x-startx)>30)
                        //                {
                        //                    printf("m_speed : %d\n", mSpeed);
                        //                    lafter.z += 180.0  *((float)(x -startx) /window_width / ((float)ltime / 1000)) * (((float)abs(x-startx)/(float)ltime) < 1 ? ((float)abs(x-startx)/(float)ltime)/4 : 1);
                        //                    this->StartAnimation(this->CurrentParametric(), lafter, ANI_STEP, 500, 0, JA_ANI_TYPE_ROTATE, false, 0, true, NULL, NULL);
                        //                }
                        if (abs(mSpeed) > 3 && abs(x - startx) > 30) {
                            lafter.z += mSpeed * hemisphereInertia;
                            this->StartAnimation(this->CurrentParametric(), lafter, 75, 150, 0,
                                                 JA_ANI_TYPE_ROTATE, false, 0, true, NULL, NULL);
                        }
                    }
                }
                break;
            case SCRN_CYLINDER:
                lafter = this->CurrentParametric()->GetPosition(true, 0);
                lscale = this->CurrentParametric()->GetScale(false, 0);
                lrotate = this->CurrentParametric()->GetRotate(false, 0);
                ltscale = this->CurrentParametric()->GetScale(true, 0);
                ltpos = this->CurrentParametric()->GetPosition(true, 0);
                ltime = clock_ms() - starttime;
                if (ltime < 1500 && ltime > 0) {
                    if (GetDualMode())
                        lafter.x -=
                                2 * ((float) (x - startx) / window_width / ((float) ltime / 1000));
                    else
                        lafter.x +=
                                2 * ((float) (x - startx) / window_width / ((float) ltime / 1000));
                }
                this->StartAnimation(this->CurrentParametric(), lafter, ANI_STEP, 500, 0,
                                     JA_ANI_TYPE_POSITION, true, 0, true, NULL, NULL);
                if (!(ltscale.x == 1.0) && !(ltscale.x == 6)) {
                    if (ltscale.x < 1.3) {
                        ltscale.x = 1.0;
                        if (this->GetDualMode())
                            lrotate.x = -30.0;
                        else
                            lrotate.x = 30.0;
                        lscale = vec3(1, 1, 1);
                    } else {
                        ltscale.x = 6.0;
                        lrotate.x = 0;
                        lscale = vec3(1.5, 1.5, 1.5);
                    }
                    this->StartAnimation(this->CurrentParametric(), ltscale, ANI_STEP / 3, 100, 0,
                                         JA_ANI_TYPE_SCALE, true, 0, true, NULL, NULL);
                    this->StartAnimation(this->CurrentParametric(), lrotate, ANI_STEP / 3, 100, 0,
                                         JA_ANI_TYPE_ROTATE, false, 0, true, NULL, NULL);
                    this->StartAnimation(this->CurrentParametric(), lscale, ANI_STEP / 3, 100, 0,
                                         JA_ANI_TYPE_SCALE, false, 0, true, NULL, NULL);
                }
                break;
            case SCRN_UPDOWN:
                lafter = this->CurrentParametric()->GetPosition(true, 0);
                ltime = clock_ms() - starttime;
                if (ltime < 1500 && ltime > 0) {
                    lafter.x -= 2 * ((float) (x - startx) / window_width / ((float) ltime / 1000));
                }
                this->StartAnimation(this->CurrentParametric(), lafter, ANI_STEP, 500, 0,
                                     JA_ANI_TYPE_POSITION, true, 0, true, NULL, NULL);
                lafter = this->CurrentParametric()->GetPosition(true, 1);
                ltime = clock_ms() - starttime;
                if (ltime < 1500 && ltime > 0) {
                    lafter.x -= 2 * ((float) (x - startx) / window_width / ((float) ltime / 1000));
                }
                this->StartAnimation(this->CurrentParametric(), lafter, ANI_STEP, 500, 0,
                                     JA_ANI_TYPE_POSITION, true, 1, true, NULL, NULL);
                break;
            case SCRN_FOUR:
            case SCRN_THREE:
            case SCRN_FOUREX:
            case SCRN_SIX:
                //mIndex = GetScreenIndex(x, y, mgn, this->CurrentParametric()->GetScreenMode());
                if (this->GetDualMode()) {
                    if (mIndex >= 0) {
                        lafter = this->CurrentParametric()->GetRotate(false, 0);
                        ltime = clock_ms() - starttime;
                        if (this->CurrentParametric()->GetScreenMode() != SCRN_FOUREX ||
                            (this->CurrentParametric()->GetScreenMode() == SCRN_FOUREX &&
                             this->CurrentParametric()->GetSelected(2) != 2)) {
                            if (ltime < 800 && ltime > 100 && abs(x - startx) > 30) {
                                lafter.y += 180.0 * ((float) (x - startx) / window_width /
                                                     ((float) ltime / 1000));
                                this->StartAnimation(this->CurrentParametric(), lafter, ANI_STEP,
                                                     500, 0, JA_ANI_TYPE_ROTATE, false, mIndex,
                                                     true, NULL, NULL);
                            }
                        }
                        if (this->CurrentParametric()->GetScreenMode() == SCRN_FOUREX &&
                            this->CurrentParametric()->GetSelected(2) == 2) {
                            lafter = this->CurrentParametric()->GetPosition(true, mIndex);
                            ltime = clock_ms() - starttime;
                            if (ltime < 1500 && ltime > 0) {
                                lafter.x -= 2 * ((float) (x - startx) / window_width /
                                                 ((float) ltime / 1000));
                            }
                            this->StartAnimation(this->CurrentParametric(), lafter, ANI_STEP, 500,
                                                 0, JA_ANI_TYPE_POSITION, true, mIndex, true, NULL,
                                                 NULL);
                        }
                    }
                } else {
                    if (mIndex >= 0) {
                        lafter = this->CurrentParametric()->GetRotate(false, mIndex);

                        if (lafter.x > limit_top)
                            lafter.x = limit_top;
                        if (lafter.x < limit_bottom)
                            lafter.x = limit_bottom;
                        ltime = clock_ms() - starttime;

                        if (ltime < 1500 && ltime > 0 && abs(x - startx) > 30) {
                            ////printf("z...............:%f %f %d\n", lafter.z, (float)ltime / 1000, x -startx);
                            lafter.z += 180.0 * ((float) (x - startx) / window_width /
                                                 ((float) ltime / 1000));
                            this->StartAnimation(this->CurrentParametric(), lafter, ANI_STEP, 500,
                                                 0, JA_ANI_TYPE_ROTATE, false, mIndex, true, NULL,
                                                 NULL);
                        }
                    }
                }
                break;
            case SCRN_SPHERE:
            case SCRN_STITCH2:
                //            case SCRN_STITCH:
                lafter = this->CurrentParametric()->GetRotate(false, 0);
                ltime = clock_ms() - starttime;
                if (ltime < 800 && ltime > 100 && abs(x - startx) > 30) {
                    ////printf("z...............:%f %f %d\n", lafter.z, (float)ltime / 1000, x -startx);
                    lafter.y +=
                            180.0 * ((float) (x - startx) / window_width / ((float) ltime / 1000));
                    this->StartAnimation(this->CurrentParametric(), lafter, ANI_STEP, 500, 0,
                                         JA_ANI_TYPE_ROTATE, false, 0, true, NULL, NULL);
                }
                break;
            case SCRN_PANORAMA:
                lafter = this->CurrentParametric()->GetPosition(true, 0);
                ltime = clock_ms() - starttime;
                if (ltime < 1500 && ltime > 0) {
                    lafter.x -= 2 * ((float) (x - startx) / window_width / ((float) ltime / 1000));
                }
                this->StartAnimation(this->CurrentParametric(), lafter, ANI_STEP, 500, 0,
                                     JA_ANI_TYPE_POSITION, true, 0, true, NULL, NULL);
                break;
        }
    lastx = -1;
    lasty = -1;
    startx = -1;
    starty = -1;
    starttime = 0;
    mIndex = -1;
}

void ParametricManager::DoTapOrMouseWheel(int intra, int x, int y, int scindex, int wallMode) {
    vec3 lscale, lrotate, lposition;
    int index;
    float lbs = mViewAngle / 60.0;

#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
    if (mShowCircle) {
        mCurrentCircle = -1;
        float lcx, lcy, lcdiameter;
        vec3 lcpos = mCircle[0]->GetPosition(false, 0);
        lcx = (x - (float) window_width / 2) / ((float) window_height / 2) - lcpos.x;
        lcy = (y - (float) window_height / 2) / ((float) window_height / 2) - lcpos.y;
        lcdiameter = sqrtf(lcx * lcx + lcy * lcy);
        if (lcdiameter < mDiameter[0])
            mCurrentCircle = 0;
        if (mShowCircle > 1 && mCurrentCircle < 0) {
            lcpos = mCircle[1]->GetPosition(false, 0);
            lcx = (float) (x - (float) window_width / 2) / ((float) window_height / 2) - lcpos.x;
            lcy = (float) (y - (float) window_height / 2) / ((float) window_height / 2) - lcpos.y;
            lcdiameter = sqrtf(lcx * lcx + lcy * lcy);
            if (lcdiameter < mDiameter[1])
                mCurrentCircle = 1;
        }
        if (mCurrentCircle >= 0) {
            float incdec = 0.01;
            if (intra < 0)
                incdec = -0.01;
            mDiameter[mCurrentCircle] += incdec;
            if (mDiameter[mCurrentCircle] < 0.5)
                mDiameter[mCurrentCircle] = 0.5;
            if (mDiameter[mCurrentCircle] > 1.8)
                mDiameter[mCurrentCircle] = 1.8;
            mCircle[mCurrentCircle]->UpdateDiameter(mDiameter[mCurrentCircle]);
        }
        mCurrentCircle = -1;
    } else
#endif
        switch (this->CurrentParametric()->GetScreenMode()) {
            case SCRN_NORMAL:
                lscale = this->CurrentParametric()->GetScale(true, scindex);
                if (intra > 0)
                    lscale = lscale + vec3(0.1, 0.1, 0.1);
                else
                    lscale = lscale - vec3(0.1, 0.1, 0.1f);
                if (lscale.x < 1)
                    lscale = vec3(1, 1, 1);
                if (lscale.x > 8)
                    lscale = vec3(8, 8, 8);
                if (intra == -100)
                    lscale = vec3(1, 1, 1);
                this->CurrentParametric()->SetScale(lscale, true, scindex);
                ////printf("lscale.x:%f\n", lscale.x);
                break;
            case SCRN_CYLINDER:
                break;
            case SCRN_HEMISPHERE:
            case SCRN_VECTOROBJECT:
            case SCRN_CUP:
                //            case SCRN_STITCH:
                this->ClearAnimation();
                lscale = this->CurrentParametric()->GetScale(false, 0);
                lrotate = this->CurrentParametric()->GetRotate(false, 0);
                lposition = this->CurrentParametric()->GetPosition(false, 0);
                //			if (lposition.z > 2.8)
                //			{
                //				lposition.z = 0;
                //				this->CurrentParametric()->SetPosition(lposition, false, 0);
                //			}
                if (intra < 0) {
                    if (this->CurrentParametric()->GetScreenMode() == SCRN_CUP) {
//					if (lscale.x == 0.85f)
//					{
//						if (lposition.z > 0)
//							lposition.z -= 0.085;
//						if (lposition.z < 0)
//							lposition.z = 0;
//						//							lrotate.x = lrotate.x - 3;
//						//							if (lrotate.x < 0)
//						//								lrotate.x = 0;
//					}
//					else
                        {
                            lscale = lscale - vec3(0.02 * lbs, 0.02 * lbs, 0.02f * lbs);
                            if (lscale.x < 0.85f)
                                lscale = vec3(0.85f, 0.85f, 0.85f);
                        }
                    } else {
                        if (lscale.x == 1.0f) {
                            if (lposition.z > 0)
                                lposition.z -= 0.05;
//						if (this->CurrentParametric()->GetScreenMode() == SCRN_HEMISPHERE && wallMode)
//						{
//							if (lposition.z < adjustableScale)
//							{
//								lposition.z = adjustableScale;
//							}
//						}
//						else
                            {
                                if (lposition.z < 0) {
                                    lposition.z = 0;
                                }
                            }
                            lrotate.x = lrotate.x - 3;
                            if (lrotate.x < 0)
                                lrotate.x = 0;
                        } else {
                            lscale = lscale - vec3(0.02 * lbs, 0.02 * lbs, 0.02f * lbs);
                            if (lscale.x < 1.0f)
                                lscale = vec3(1.0f, 1.0f, 1.0f);
                        }
                    }
                } else {
                    if (lposition.z < 3) {
                        lposition.z += 0.05;
                        if (lposition.z > 3.0)
                            lposition.z = 3;
                    } else {
                        lscale = lscale + vec3(0.02 * lbs, 0.02 * lbs, 0.02 * lbs);
                        if (lscale.x > 3.0 * lbs)
                            lscale = vec3(3.0 * lbs, 3.0 * lbs, 3.0 * lbs);
                    }
                }
                ////printf("%f,%f,%f\n",lscale.x,lscale.y,lscale.z);
                this->CurrentParametric()->SetScale(lscale, false, 0);
                this->CurrentParametric()->setRotate(lrotate, false, 0);
                this->CurrentParametric()->SetPosition(lposition, false, 0);
                break;
            case SCRN_FOUR:
            case SCRN_THREE:
            case SCRN_FOUREX:
            case SCRN_SIX:
                index = GetScreenIndex(x, y, this->CurrentParametric()->GetScreenMode());
                if (index >= 0) {
                    lscale = this->CurrentParametric()->GetScale(false, index);
                    if (intra > 0)
                        lscale = lscale + vec3(0.02 * lbs, 0.02 * lbs, 0.02 * lbs);
                    else
                        lscale = lscale - vec3(0.02 * lbs, 0.02 * lbs, 0.02f * lbs);
                    if (lscale.x < 1)
                        lscale = vec3(1, 1, 1);
                    if (lscale.x > 4.0 * lbs)
                        lscale = vec3(4.0 * lbs, 4.0 * lbs, 4.0 * lbs);
                    //printf("scale:%f\n", lscale.y);
                    this->ClearAnimation(index);
                    this->CurrentParametric()->SetScale(lscale, false, index);
                }
                break;
            case SCRN_SPHERE:
            case SCRN_STITCH2:
                this->ClearAnimation();
                lscale = this->CurrentParametric()->GetScale(false, 0);
                lposition = this->CurrentParametric()->GetPosition(false, 0);
                if (intra > 0) {
                    if (lposition.z < 0) {
                        lposition.z += 0.05;
                        if (lposition.z > 0)
                            lposition.z = 0;
                    } else {
                        lscale = lscale + vec3(0.006, 0.006, 0.006);
                        if (lscale.x > 3)
                            lscale = vec3(3, 3, 3);
                    }
                } else {
                    if (lscale.x > 1) {
                        lscale = lscale - vec3(0.006, 0.006, 0.006f);
                        if (lscale.x < 1)
                            lscale = vec3(1, 1, 1);
                    } else {
                        lposition.z -= 0.05;
                        if (lposition.z < -2.5)
                            lposition.z = -2.5;
                    }
                }
                this->CurrentParametric()->SetScale(lscale, false, 0);
                this->CurrentParametric()->SetPosition(lposition, false, 0);
                break;
            case SCRN_PANORAMA:
                lscale = this->CurrentParametric()->GetScale(false, scindex);
                if (intra > 0)
                    lscale = vec3(lscale.x * 1.05, lscale.y * 1.05, lscale.z * 1.05);
                else
                    lscale = vec3(lscale.x * 0.9523, lscale.y * 0.9523, lscale.z * 0.9523f);
                if (lscale.y < 0.5)
                    lscale = vec3(1, 0.5, 1);
                if (lscale.y > 1)
                    lscale = vec3(2, 1, 2);
                this->CurrentParametric()->SetScale(lscale, false, scindex);
                ////printf("lscale.x:%f\n", lscale.x);
                break;
        }
}

void ParametricManager::AdjustActionExperience(int type, int actionType, float value) {
    switch (actionType) {
        case ACTION_MOVE_SPEED:
            hemisphereSpeel = value;
            break;
        case ACTION_MOVE_INERTIA:
            hemisphereInertia = value;
            break;
        case ACTION_MOVE_SCALE:
            adjustableScale = value;
    }
}

int ParametricManager::GetScreenIndex(int x, int y, int screenmode) {
    int px, py, index;
    switch (screenmode) {
        case SCRN_FOUR:
            px = x / (window_width / 2);
            py = y / (window_height / 2);
            index = px + (1 - py) * 2;
            break;
        case SCRN_THREE:
            index = -1;
            if (x > 0 && x < window_width / 2 && y > 0 && y < 2 * window_height / 3)
                index = 0;
            if (x > window_width / 2 && x < window_width && y > 0 && y < 2 * window_height / 3)
                index = 1;
            break;
        case SCRN_SIX:
            index = -1;
            if (x > 2 * window_width / 3 && x < window_width && y > 0 && y < window_height / 3)
                index = 0;
            if (x > 2 * window_width / 3 && x < window_width && y > window_height / 3 &&
                y < 2 * window_height / 3)
                index = 1;
            if (y > 2 * window_height / 3) {
                px = x / (window_width / 3);
                index = px + 2;
            }
            break;
        case SCRN_FOUREX:
            index = -1;
            if (x > window_width / 3 * 2 && y < window_height / 3)
                index = 0;
            if (x > window_width / 3 * 2 && y > window_height / 3 && y < window_height / 3 * 2)
                index = 1;
            break;
    }
    return index;
}

void
ParametricManager::DoUpdateFrame(float tbstart, float tbend, float lrstart, float lrend, int index,
                                 bool update, bool ani) {
    //index = mIndex;
    vec3 lscale = this->CurrentParametric()->GetScale(false, index);
    vec3 lrotate = this->CurrentParametric()->GetRotate(false, index);
    //  printf("lrotate.x:%f,y:%f,z:%f\n",lrotate.x,lrotate.y,lrotate.z);
    float langle = mViewAngle - (100 * (lscale.y - 1) * 8);
    tbend = tbend / 3 * 2;
    lrend = lrend / 3 * 2;
    lrstart = lrstart - lrend / 2;
    tbstart = tbstart - tbend / 2;
    float lrangle = atanf(lrstart / tbstart) / Pi * 180.0f;
    if (tbstart > 0 && lrstart < 0)
        lrangle = -lrangle;
    if (tbstart > 0 && lrstart > 0)
        lrangle = 360 - lrangle;
    if (tbstart < 0)
        lrangle = 180 - lrangle;
    float tbangle;
    lrotate.z = lrangle - 90;
    float px = tbstart;
    float py = lrstart;
    float actr = sqrtf(px * px + py * py);
    tbangle = 90 - acosf(actr / (lrend / 2)) / Pi * 180.0f;
    if (tbangle > 90 - langle / 2 || actr > lrend / 2)
        tbangle = 90 - langle / 2;
    else if (tbangle < langle / 2);// tbangle = langle / 2;
    else
        tbangle = tbangle + 12.0;
    //printf("tbstart:%f,lrstart:%f,actr:%f,R:%f,tbangle:%f,lrotate.x:%f,angle:%f\n", tbstart, lrstart,actr,lrend/2, tbangle,lrotate.x, acosf(actr / (lrend / 2)) / Pi*180.0f);
    lrotate.x = tbangle;
    if (ani) {
        //printf("doUpdateFrame......%d\n");
        printf("lrotate.x:%f,y:%f,z:%f\n", lrotate.x, lrotate.y, lrotate.z);
        this->StartAnimation(this->CurrentParametric(), lrotate, 50, 30, 0, JA_ANI_TYPE_ROTATE,
                             false, index, true, NULL, NULL);
    } else
        this->CurrentParametric()->setRotate(lrotate, false, index);
}


void ParametricManager::UpdateAspect(float aspect) {
#ifdef __NEW__
    pthread_mutex_lock(&_AniLock);
    _aspect = aspect;

    VertexObjectImpl *voi = (VertexObjectImpl *) ParametricList.at(1);
    //    ParametricList.remove(1);
    //    ParametricList.insert(-1,new VertexObjectImpl(aspect,this));
    //    if(_current_mode!=0)
    //        _current = ParametricList.at(1);
    //    delete voi;
    voi->UpdateAspect(aspect);
#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
    mCircle[0]->UpdateAspect(aspect);
    mCircle[1]->UpdateAspect(aspect);
#endif
    pthread_mutex_unlock(&_AniLock);
#endif
}

void ParametricManager::SetAllPage(int allPage) {
    if (_current->GetScreenMode() != SCRN_NORMAL)
        return;
    NormalScreen *nor = (NormalScreen *) _current;
    nor->SetAllPage(allPage);
}

void
ParametricManager::TransformVertex(int mode, vec3 vect, bool texture, int type, int scrnindex) {
    VertexObjectImpl *voi = (VertexObjectImpl *) ParametricList.at(1);
    voi->TransformVertex(mode, vect, texture, type, scrnindex);
}

void ParametricManager::LoadOSDTexture(int w, int h, GLenum type, GLvoid *pixels) {
#ifdef __ANDROID__
    if (_isUseDirectTexture) {
        LOGV("_isUseDirectTexture");
    } else {
        CurrentParametric()->LoadOSDTexture(w, h, type, pixels);
        LOGV("CurrentParametric()->LoadOSDTexture(w, h, type, pixels);");
    }
#endif
}

bool ParametricManager::Get_FM_CircleOptions(float *centerx, float *centery, float *diameter,
                                             int index) {
#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
    if (index < 0 || index > 1)
        return false;
    *diameter = mDiameter[index];
    vec3 pos = mCircle[index]->GetPosition(false, 0);
    *centerx = pos.x;
    *centery = pos.y;
#endif
    return true;
}

void
ParametricManager::Set_FM_CircleOptions(float centerx, float centery, float diameter, int index) {
#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
    if (index < 0 || index > 1)
        return;
    mDiameter[index] = diameter;
    vec3 pos = vec3(centerx, centery, 0);
    mCircle[index]->SetPosition(pos, false, 0);
    mCircle[index]->UpdateDiameter(diameter);
#endif
}

void ParametricManager::SetVisibility(bool value, int index) {
    if (index < 0 || index > 35)
        return;
    if (CurrentParametric()->GetScreenMode() == SCRN_NORMAL) {
        NormalPlay *mNormalPlay = (NormalPlay *) CurrentParametric();
        mNormalPlay->SetVisible(value);
    }
}

void ParametricManager::RenderCircle() {
#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
//    mShowCircle = true;
    if (mShowCircle) {
        mCircle[0]->DrawSelf(0);
        if (mShowCircle > 1)
            mCircle[1]->DrawSelf(0);
    }
#endif
}

void ParametricManager::SetViewAngle(float angle) {
    VertexObjectImpl *voi = (VertexObjectImpl *) ParametricList.at(1);
    mViewAngle = angle;
    voi->SetViewAngle(angle);
}

void ParametricManager::LoadLogo(char *fn) {
#ifndef EMSCRIPTEN
    glGetError();
    mTexLogoID = LoadFileTexture(fn, &mLogoWidth, &mLogoHeight);
    printf("Create Logo Texture:%d %d\n", glGetError(), mTexLogoID);
    if (mTexLogoID) {
        printf("..........................%d %d\n", window_width, window_height);
        mLogo = new NormalPlay(2, (float) window_width / (float) window_height, 0, 0, this);
        //        mLogo = new ImageLoading((float)mLogoWidth/(float)mLogoHeight,this);
        mLogo->SetTexture(mTexLogoID, 0);
        mLogoLoading = true;
        //ImageLoading *fb = (ImageLoading *)mLogo;
        //fb->SetTextureWH(mLogoWidth,window_width/2*0.9);
        //      fb->StartAnimation(0);
    } else
        mLogoLoading = false;
#endif
}

void ParametricManager::RenderLogo() {
#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
    if (!mTexLogoID)
        return;
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    glViewport(vp[2] - mLogoWidth, 0, mLogoWidth, mLogoHeight);
    glGetError();
    mLogo->DrawSelf(0);
    glViewport(0, 0, vp[2], vp[3]);
#endif
}

void ParametricManager::EnableGrid(bool value) {
    VertexObjectImpl *voi = (VertexObjectImpl *) ParametricList.at(1);
    voi->EnableGrid(value);
}

void ParametricManager::PlayAudioData(uint8_t *buf, int len) {
    //printf("Audio buffer Size:%d\n", len);
#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
    if (mPlayaudio)
        mAudioPlayer->openAudio(buf, len, 8000);
#endif
}

void ParametricManager::OpenAudioPlaying() {
#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
    mAudioPlayer->playSound();
#endif
}

void ParametricManager::PauseAudioPlaying() {
#if !defined(__LINUXNVR__) && !defined(EMSCRIPTEN)
    mAudioPlayer->stopSound();
#endif
}

void ParametricManager::ConnectVideoLoading(int index) {
//	_current->SetTexture(0, index);
//	_current->ResetPosition();
    _current->StartAnimation(index);
//	vec3 pos = CurrentParametric()->GetPosition(true, index);
//	pos = CurrentParametric()->GetPosition(false, index);
    LOGV("ConnectVideoLoading,_aspect : %f", _aspect);
    ReSizeSplite(_aspect);
}

void ParametricManager::CloseVideoLoading(int index) {
    _current->StopAnimation(index);
//	_texwidth = 0;
//	_texheight = 0;
}

#ifndef EMSCRIPTEN

void ParametricManager::DoDirectTextureFrameUpdata(ConnectManager *conn, int w, int h,
                                                   void *directBuffer, int index) {
    CurrentParametric()->StopAnimation(index);
    CurrentParametric()->PutInfo(0, index);
    _texwidth = w;
    _texheight = h;
    if (CurrentParametric()->GetScreenMode() != SCRN_NORMAL && !GetDualMode()) {
        LOGV("if (CurrentParametric()->GetScreenMode() != SCRN_NORMAL&&CurrentParametric()->GetScreenMode() != SCRN_SPHERE&&CurrentParametric()->GetScreenMode() != SCRN_STITCH)");
        if (!conn->GetDecoder(index)->GetCrop()) {
            conn->GetDecoder(index)->EnableCrop(true);
            return;
        }
        //	CurrentParametric()->ShowOSD(true);
    } else {
        if (conn->GetDecoder(index)->GetCrop()) {
            conn->GetDecoder(index)->EnableCrop(false);
        }
    }

    if (CurrentParametric()->GetScreenMode() != SCRN_NORMAL && !GetDualMode()) {

        VertexObjectImpl *verObj = (VertexObjectImpl *) CurrentParametric();
        verObj->DirectTextureFrameUpdata(w, h, directBuffer, index);

    } else if (!GetDualMode()) {
        NormalScreen *mNormalScreen = (NormalScreen *) CurrentParametric();
        mNormalScreen->DirectTextureFrameUpdata(w, h, directBuffer, index);
        mNormalScreen->ShowInfo(false, index);
        mNormalScreen->releaseInfo(index);
        return;
    }
        // else if (CurrentParametric()->GetScreenMode() == SCRN_VR_SPHERE)
        // {
        // 	VR_sphere * vr = (VR_sphere*)CurrentParametric();
        // 	vr->DirectTextureFrameUpdata(w, h, directBuffer, 0);
        // 	vr->DirectTextureFrameUpdata(w, h, directBuffer, 1);
        // }
        // else if (CurrentParametric()->GetScreenMode() == SCRN_STITCH2)
        // {
        // 	SphereStitch2 *s2 = (SphereStitch2*)CurrentParametric();
        // 	s2->DirectTextureFrameUpdata(w,h,directBuffer,0);
        // 	s2->SetTextureAspect((float)w/(float)h);
        // 	s2->SetSingleMode(false);


        //}
    else {
        sphere *msphere = (sphere *) CurrentParametric();
        msphere->DirectTextureFrameUpdata(w, h, directBuffer, index);
    }
}

void ParametricManager::DoDirectTextureOSDFrameUpdata(ConnectManager *conn, int w, int h,
                                                      void *textBuffer, int index) {
    if (CurrentParametric()->GetScreenMode() != SCRN_NORMAL && !GetDualMode()) {
        unsigned char *txttex = _Textput->GenTextbuffer((char *) textBuffer, 0xff000000, 480, 24);
        //	con->mgn->OnOSDTextureAvaible(conn, txttex, 480 * 24 * 4, con->mgn->GetCtx());
        conn->GetDecoder(index)->WriteTextOSDDirectTextureFrame(txttex);
        free(txttex);
        VertexObjectImpl *verObj = (VertexObjectImpl *) CurrentParametric();
#ifdef __ANDROID__
        verObj->DirectTextureOSDFrameUpdata(480, 24, conn->GetDecoder(
                index)->mGraphicBufferOSD->getNativeBuffer(), 0);
#endif //__ANDROID__
    }
}

#endif

void
ParametricManager::SetFishEyeParameters(float centerX, float centerY, float radius, float angleX,
                                        float angleY, float angleZ, int index, bool v720) {
    if (index < 0 || index > 1)
//		return;
//	mCenter[index].x = centerx;
//	mCenter[index].y = centery;
//	mCircle_radius[index] = radius;
//	mRotateAngle[index].x = anglex;
//	mRotateAngle[index].y = angley;
//	mRotateAngle[index].z = anglez;
//}

        //if (CurrentParametric()->GetScreenMode() == SCRN_STITCH2)
        //{
        //	SphereStitch2 *s2 = (SphereStitch2*)ParametricList.at(5);
        //	if(_texwidth && _texheight)
        //		s2->SetTextureAspect((float)_texwidth/(float)_texheight);
        //	//s2->SetDistortion(mdistortion,mdistortion_count, mdistortion_half_height);
        //	//s2->SetFishEyeParameter(mCenter[0].x, mCenter[0].y, mCircle_radius[0], mRotateAngle[0].x, mRotateAngle[0].y, mRotateAngle[0].z, 0);
        //	//s2->SetFishEyeParameter(mCenter[1].x, mCenter[1].y, mCircle_radius[1], mRotateAngle[1].x, mRotateAngle[1].y, mRotateAngle[1].z, 1);
        //	//s2->SetSingleMode(false);
        //}



//////////////新增函数///////////////////////////
        return;

    if (ParametricList.length() >= 2) {
        VertexObjectImpl *voi = (VertexObjectImpl *) ParametricList.at(1);
        voi->SetFishEyeParameter(centerX, centerY, radius, angleX, angleY, angleZ, index, v720);
    }

}

void ParametricManager::SetDistortion(void *pdist, int dist_size) {
    int dist_cnt = 0;

    if (pdist == NULL)
        return;

    if (dist_size == 0) {
        printf("[ParametricManager] -SetDistortion- Invalid Distortion.");
        return;
    }
    dist_cnt = dist_size / (int) sizeof(DISTORTION_NUM);
    PDISTORTION_NUM dist = (PDISTORTION_NUM) pdist;
    DISTORTION_NUM last_dist = dist[dist_cnt - 1];
    for (int i = 0; i < dist_cnt; i++) {
        LOGV("--->{%f, %f}\n", dist[i].angle, dist[i].half_height);
    }
    VertexObjectImpl *voi = (VertexObjectImpl *) ParametricList.at(1);
    voi->SetDualMode(1);
    voi->SetDistortion(dist, dist_cnt, last_dist.half_height);
}

void ParametricManager::SetStitch2TextureAspect(float pAspect) {
    if (pAspect < 1 || pAspect > 2.5)
        return;

    VertexObjectImpl *voi = (VertexObjectImpl *) ParametricList.at(1);
    voi->SetTextureAspect(pAspect);
}


GLuint ParametricManager::FBOTexture() {
    VertexObjectImpl *voi = (VertexObjectImpl *) ParametricList.at(1);
    return voi->FBOTexture();
}

void ParametricManager::SetFBOTexture(GLint texid) {
    VertexObjectImpl *voi = (VertexObjectImpl *) ParametricList.at(1);
    voi->SetFBOTexture(texid);
};


void ParametricManager::SetKeepAspect(float aspect, int scnindex) {
    NormalScreen *ns = (NormalScreen *) ParametricList.at(0);
    if (ns)
        ns->SetKeepAspect(aspect, scnindex);
}

float ParametricManager::GetKeepAspect(int scnindex) {
    NormalScreen *ns = (NormalScreen *) ParametricList.at(0);
    if (ns)
        return ns->GetKeepAspect(scnindex);
    else
        return 1.0;
}
