//
// Created by LiHong on 16/7/24.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef OSX_CIRCLEDETECTION_H
#define OSX_CIRCLEDETECTION_H

#include "BaseScreen.h"

class CircleDetection : public BaseScreen {
public:
    CircleDetection(float radius,float aspect,void *mgn);
    vec3 Evaluate(const vec2& domain);
    vec2 EvaluateCoord(const vec2& domain);
    void setupProgram(const char *segsrc,const char *fragsrc);
    void setupProgram(const char *segsrc,const char *fragsrc,bool yuv);
    void DrawSelf(int scnindex);
    bool LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex);
    bool LoadTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels, int scnindex);
    virtual void SetIsAnimation(int index, bool animation) {};

private:
    float m_radius;
    GLint texelWidthUniform, texelHeightUniform;

    GLfloat texelWidth, texelHeight;
};

#endif //OSX_CIRCLEDETECTION_H
