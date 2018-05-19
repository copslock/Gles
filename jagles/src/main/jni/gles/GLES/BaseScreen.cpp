//
// Created by LiHong on 16/2/21.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include <stdio.h>
#include "BaseScreen.h"
#include "ParametricManager.h"
#include "NormalPlay.h"
#include "ImageLoading.h"

#ifdef __ANDROID_API__

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#endif


#define LOG_TAG "JAVideo"
#define LOGV(...)__android_log_print(ANDROID_LOG_DEBUG,"Lee",__VA_ARGS__)
#define LOGD(...)__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define INFO(...) {__android_log_write(ANDROID_LOG_INFO,"videoconnect",__VA_ARGS__);}
#define LOGE(...) {__android_log_print(ANDROID_LOG_ERROR, "videoconnect", __VA_ARGS__);}


BaseScreen::BaseScreen() {
    isUseDirectTexture = false;
    directTextureBuffer = NULL;
    direct_width = 0;
    direct_height = 0;
    _LoadingAnimation = 0;
    _loading = NULL;
    _OSD = NULL;
    _InfoTexID = 0;
    _Info = NULL;
    mShowOSD = true;
    mShowframe = false;
    mSelectedTex = 0;
    mNormalTex = 0;
    mIsSelected = 0;
    mShowInfo = false;
    mShowRecordStatue = false;
    mIsOwnAnimation = false;
    mLineBuffer = 0;
}

bool BaseScreen::NeedDraw() {
    if (_loading)
        return _loading->NeedDraw() | ParametricSurface::NeedDraw();
    else
        return ParametricSurface::NeedDraw();
}

BaseScreen::~BaseScreen() {
    if (_loading)
        delete _loading;
    if (_OSD)
        delete _OSD;
    if (_Info)
        delete _Info;
    if (_InfoTexID)
        glDeleteTextures(1, &_InfoTexID);
    if (mLineBuffer != 0) {
        glDeleteBuffers(1, &mLineBuffer);
        LOGDEBUG("7---glDeleteBuffers:%d %p", mLineBuffer, this);

    }
}

void BaseScreen::StartAnimation(int scnindex) {
    ParametricManager *mgn = (ParametricManager *) _Manager;
    if (!_loading) {
        if (mgn->GetLogoLoading()) {
            int logow, logoh, winw, winh;
            GLuint logoid;
            mgn->GetLogo(&logoid, &logow, &logoh);
            mgn->GetWindowWH(&winw, &winh);
            ImageLoading *il = new ImageLoading((float) logow / (float) logoh, mgn);
            il->SetTexture(logoid, 0);
            il->SetTextureWH(logow, winw / 2 * 0.9);
            _loading = il;
        } else {
            _temploading = new ToursLoading(0.15, 0.10, m_aspect);
            _loading = _temploading;
        }
        vec3 lpos = GetPosition(false, 0);
        lpos = vec3(lpos.x, lpos.y, 0);
        _loading->SetPosition(lpos, false, 0);
        _Info = new NormalPlay(0.05, m_aspect, 0, 0, _Manager);
        _Info->SetScale(vec3(14, 1.3, 1), false, 0);
    }
    if (!mgn->GetLogoLoading() && !_LoadingAnimation)
        _LoadingAnimation = mgn->StartAnimation(_loading, vec3(0, 0, 5), 5, 10, 1,
                                                JA_ANI_TYPE_ROTATE, false, 0, false, NULL, NULL);
    else
        _loading->StartAnimation(0);
}

void BaseScreen::StopAnimation(int scnindex) {
    ParametricManager *mgn = (ParametricManager *) _Manager;
    if (mgn->GetLogoLoading()) {
        if (_loading)
            _loading->StopAnimation(0);
    } else {
        if (_LoadingAnimation) {
            mgn->StopAnimation(_LoadingAnimation);
            _LoadingAnimation = 0;
        }
    }
}

void BaseScreen::DrawSelf(int scnindex) {
    if (!Visible)
        return;
#ifdef __ANDROID__
    if ((_texwidth != direct_width || _texheight != direct_height) && isUseDirectTexture) {
        if (_texid)
            glDeleteTextures(1, &_texid);
        _texid = 0;
        _texwidth = direct_width;
        _texheight = direct_height;
        //	JACONNECT_INFO("normal should not come to here.................1");
    }
    if (!_texid && isUseDirectTexture) {
        CreateDirectTextureFrame();
    }
#endif //__ANDROID__

    ParametricSurface::DrawSelf(scnindex);
    ParametricManager *mgn = (ParametricManager *) _Manager;
    if (_loading && !mIsOwnAnimation)
        if (mgn->GetLogoLoading())
            _loading->DrawSelf(0);
        else if (_LoadingAnimation)
            _loading->DrawSelf(0);
    if (GetScreenMode() != SCRN_NORMAL && GetScreenMode() != SCRN_SPHERE)
        if (_OSD && mShowOSD)
            _OSD->DrawSelf(0);
    if (_InfoTexID && _Info && GetScreenMode() == SCRN_ONENORMAL && !mIsOwnAnimation) {
        if (mShowInfo) {
            _Info->DrawSelf(0);
        }
    }
    if (mShowframe)
        for (int i = 0; i < mFrameList.length(); i++)
            if (scnindex == i)
                mFrameList.at(i)->DrawSelf(0xffffffff);
            else
                mFrameList.at(i)->DrawSelf(0);
    if (scnindex != 0x7fffffff)
        return;
    DrawBorder();
}

void BaseScreen::SetPosition(vec3 pos, bool texture, int scnindex) {
    if (!texture && GetScreenMode() != SCRN_CYLINDER) {
        vec3 lpos;
        if (_loading) {
            lpos = vec3(pos.x, pos.y, 0.0);
            _loading->SetPosition(lpos, false, 0);
        }
        if (_Info) {
            if (GetScreenMode() != SCRN_HEMISPHERE) {
//                lpos = GetPosition(false,0);
                if (m_aspect > 1) {
                    lpos = GetPosition(false, 0);
                } else {
                    lpos = vec3(pos.x, pos.y, 0.0);
                }
                _Info->SetPosition(vec3(lpos.x, lpos.y - GetScale(false, 0).y / 2 + 0.1, 0), false,
                                   0);
            }
        }

    }
    ParametricSurface::SetPosition(pos, texture, scnindex);
}

void BaseScreen::LoadOSDTexture(int w, int h, GLenum type, GLvoid *pixels) {
    if (!_OSD) {
        _OSD = new NormalPlay(0.1, m_aspect, 0, 0, _Manager);
        _OSD->SetScale(vec3(12, 1, 1), false, 0);
        vec3 lpos = _OSD->GetPosition(false, 0);
        _OSD->SetPosition(vec3(-0.4 / m_aspect, 0.95, lpos.z), false, 0);
    }
    _OSD->LoadTexture(w, h, type, pixels, 0);
}

void BaseScreen::LoadOSDTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels) {
    if (!_OSD) {
        _OSD = new NormalPlay(0.1, m_aspect, 0, 0, _Manager);
        _OSD->SetScale(vec3(12, 1, 1), false, 0);
        vec3 lpos = _OSD->GetPosition(false, 0);
        _OSD->SetPosition(vec3(-0.4 / m_aspect, 0.95, 0), false, 0);
    }
    _OSD->LoadTexture(w, h, y_pixels, cbcr_pixels, 0);
}

void BaseScreen::PutInfo(GLuint texid, int scnindex) {
    if (_Info) {
        vec3 lpos = _Info->GetPosition(false, 0);

        if (GetScreenMode() != SCRN_ONENORMAL) {

            _Info->SetPosition(vec3(-0.7 / m_aspect, -0.95, 0), false, 0);
        } else {

            NormalPlay *lc = (NormalPlay *) this;
            float lw = lc->getHeight();
            vec3 lpos1 = GetPosition(false, 0);
            LOGD("BaseScreen::PutInfo,scnindex:%d \tlpos1.x:%f \t lpos1.y:%f \t lpos1.z:%f \t GetScale(false, 0).y:%f",
                 scnindex, lpos1.x, lpos1.y, lpos1.z, GetScale(false, 0).y);
            _Info->SetPosition(vec3(lpos1.x, lpos1.y - GetScale(false, 0).y / 2 + 0.10, lpos1.z),
                               false, 0);
        }
        _InfoTexID = texid;
        _Info->SetTexture(texid, 0);
    }
}

void BaseScreen::ShowOSD(bool value) {
    mShowOSD = value;
}

void BaseScreen::releaseInfo() {
    if (_Info)
        free(_Info);
    _Info = NULL;
}

//add recording Statue 
void BaseScreen::LoadRecordStatuTexture(GLuint texid, int scnindex) {

}

//show recording Statue 
void BaseScreen::ShowRecordStatu(bool value, int index) {

}

void BaseScreen::AddFrame(IFrameDraw *frame) {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    mFrameList.insert(-1, frame);
}

void BaseScreen::UpdateFrame(float tbstart, float tbend, float lrstart, float lrend, int index,
                             bool update, bool ani) {
    if (index < 0 || index >= mFrameList.length())
        return;
    mFrameList.at(index)->UpdatePosition(tbstart, tbend, lrstart, lrend);
}

void BaseScreen::ShowFrame(bool value) {
    mShowframe = value;
}

void BaseScreen::SetBorderColor(GLuint selecttex, GLuint normaltex) {
    mSelectedTex = selecttex;
    mNormalTex = normaltex;
}


void BaseScreen::SetupBuffer() {
    ParametricSurface::SetupBuffer();
    this->setupLineBuffer();
}

void BaseScreen::setupLineBuffer() {
    if (mLineBuffer != 0) {
        glDeleteBuffers(1, &mLineBuffer);
        mLineBuffer = 0;
    }
    float lineCoords[] = {
            -0.5f * m_aspect, -0.5f, 1.0f,
            0.5f * m_aspect, -0.5f, 1.0f,
            0.5f * m_aspect, -0.5f, 1.0f,
            0.5f * m_aspect, 0.5f, 1.0f,
            -0.5f * m_aspect, 0.5f, 1.0f,
            0.5f * m_aspect, 0.5f, 1.0f,
            -0.5f * m_aspect, 0.5f, 1.0f,
            -0.5f * m_aspect, -0.5f, 1.0f,
    };
    mLinebufferSize = 24;

    if (mLineBuffer)
        glDeleteBuffers(1, &mLineBuffer);
    glGenBuffers(1, &mLineBuffer);
    LOGDEBUG("7--glGenBuffers:%d %p", mLineBuffer, this);

    glBindBuffer(GL_ARRAY_BUFFER, mLineBuffer);
    glBufferData(GL_ARRAY_BUFFER, mLinebufferSize * sizeof(GLfloat), lineCoords, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BaseScreen::DrawBorder() {
    if (mLineBuffer) {
        glUseProgram(_programHandle);
        float lzoom = _Zoom;
        vec3 lposition = mPosition;
        vec3 lrotate = mRotate;
        vec3 lscale = mScale;
        if (GetScreenMode() != SCRN_ONENORMAL) {
            _Zoom = -1.8725;
            mPosition = vec3(0, 0, 0);
            mRotate = vec3(0, 0, 0);
            mScale = vec3(1, 1, 1);
            ParametricSurface::updateSurfaceTransform();
        }
        glBindBuffer(GL_ARRAY_BUFFER, mLineBuffer);
        glVertexAttribPointer(_positionSlot, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(_positionSlot);
        glActiveTexture(GL_TEXTURE0);
        if (mIsSelected)
            glBindTexture(GL_TEXTURE_2D, mSelectedTex);
        else
            glBindTexture(GL_TEXTURE_2D, mNormalTex);
        glLineWidth(2);
        glDrawArrays(GL_LINES, 0, mLinebufferSize / 3);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisableVertexAttribArray(_positionSlot);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        _Zoom = lzoom;
        mPosition = lposition;
        mRotate = lrotate;
        mScale = lscale;
    }
}

void BaseScreen::SetSelected(bool value, int scnindex) {
    mIsSelected = value;
}

int BaseScreen::GetSelected(int scnindex) {
    return mIsSelected;
}

void BaseScreen::ShowInfo(bool value) {
    mShowInfo = value;
}

void *BaseScreen::GetLoading(int index) {
    return _loading;
}

void BaseScreen::ResetSizeAnimation(float aspect) {
//	if(_loading)
//		_loading->ReSizeSplite(aspect);
    if (_loading) {
        ParametricManager *mgn = (ParametricManager *) _Manager;
        if (!mgn->GetLogoLoading()) {
            ToursLoading *t = (ToursLoading *) _loading;
            t->ReSizeSplite(aspect);
        }
    }
    if (_Info) {
        vec3 lpos = _Info->GetPosition(false, 0);
        if (GetScreenMode() != SCRN_ONENORMAL)
            _Info->SetPosition(vec3(-0.7 / aspect, -0.95, 0), false, 0);
        else {
            NormalPlay *lc = (NormalPlay *) this;
            float lw = lc->getHeight();
            vec3 lpos1 = GetPosition(false, 0);
            _Info->SetPosition(vec3(lpos1.x, lpos1.y - GetScale(false, 0).y / 2 + 0.10, lpos1.z),
                               false, 0);
        }
    }

}

void BaseScreen::SetIsAnimation(int index, bool animation) {
    mIsOwnAnimation = animation;
}

void BaseScreen::CreateDirectTextureFrame() {
    //	JACONNECT_INFO("osd frame come.......................");
    if (!_texid) {
        _texwidth = direct_width;
        _texheight = direct_height;
        GLuint texture_object_id = 0;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &texture_object_id);
//		LOGDEBUG("9--glGenTextures:%d",texture_object_id);
        glBindTexture(GL_TEXTURE_2D, texture_object_id);
#ifdef __ANDROID__
        EGLImageKHR _imageKHR = eglCreateImageKHR(eglGetCurrentDisplay(),
                                                  NULL,
                                                  EGL_NATIVE_BUFFER_ANDROID,
                                                  (EGLClientBuffer) directTextureBuffer,
                                                  NULL);
        glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, (GLeglImageOES) _imageKHR);
#endif //__ANDROID__
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        _texid = texture_object_id;
    }
}

void BaseScreen::DirectTextureFrameUpdata(int w, int h, void *directBuffer, int scnindex) {
    isUseDirectTexture = true;
    if (direct_width != w || direct_height != h)
        directTextureBuffer = NULL;
    direct_width = w;
    direct_height = h;
    if (directTextureBuffer == NULL)
        directTextureBuffer = directBuffer;
    mNeedDraw = true;
}


