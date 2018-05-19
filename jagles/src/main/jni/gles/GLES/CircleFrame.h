//
// Created by LiHong on 16/8/16.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef OSX_CIRCLEFRAME_H
#define OSX_CIRCLEFRAME_H

#include "ParametricSurface.h"

class CircleFrame: public ParametricSurface {
public:
    CircleFrame(float aspect);
    ~CircleFrame();
    void DrawSelf(int scnindex);
    void SetupBuffer();
    vec3 Evaluate(const vec2& domain) {return vec3(0,0,0);};
    vec2 EvaluateCoord(const vec2& domain) {return vec2(0,0);};
    void StartAnimation(int scnindex) {};
    void StopAnimation(int scnindex) {};
    void LoadOSDTexture(int w, int h, GLenum type, GLvoid *pixels) {};
    void LoadOSDTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels) {};
    void PutInfo(GLuint texid, int scnindex) {};
    void ShowOSD(bool value) {};

    int GetFrameType() { return FRAME_TYPE_HEMISPHERE;}

    void UpdateDiameter(float diameter);
    virtual void ShowRecordStatu(bool value) {};
    virtual void LoadRecordStatuTexture(GLuint texid, int scnindex) {};
    virtual void* GetLoading(int index) {return 0;};
    void UpdateAspect(float aspect) {m_aspect = aspect;};
    virtual void SetIsAnimation(int index, bool animation) {};
private:
    float   *mLineCoords;
    GLuint mLineBuffer;
    GLuint mLinebufferSize;
    GLfloat mSelectedColor[4];
    GLfloat mNormalColor[4];
    GLuint  mColorHandle;
    bool 	mIsUpdateDiameter;
    float   mDiameter;
};


#endif //OSX_CIRCLEFRAME_H
