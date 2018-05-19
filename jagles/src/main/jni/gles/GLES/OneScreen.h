//
// Created by LiHong on 16/1/29.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef FISHEYE_ONESCREEN_H
#define FISHEYE_ONESCREEN_H
#include "ParametricSurface.h"
#include "BaseScreen.h"

class OneScreen : public BaseScreen {
public:
    OneScreen(float radiusW,float radiusH,float aspect,float width,float left,float right,float start,void *mgn) ;
    vec3 Evaluate(const vec2& domain);
    vec2 EvaluateCoord(const vec2& domain);
    void TurnLeftRight(GLfloat angle,int scnindex);
    void TurnUpDown(GLfloat angle,int scnindex);
    void setupProgram(const char *segsrc,const char *fragsrc);
    void setupProgram(const char *segsrc,const char *fragsrc,bool yuv);
    void DrawSelf(int scnindex);
    void ResetPosition();

private:
    float m_radiusW,m_radiusH;
    GLfloat mTextureOffset;
    GLint mTextureOffsetHandle;
    GLfloat mWidth;
    GLfloat mLeft;
    GLfloat mTop;
    GLfloat mStart;
};

#endif //FISHEYE_ONESCREEN_H
