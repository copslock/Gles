//
// Created by LiHong on 16/1/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef FISHEYE_EXPAND_H
#define FISHEYE_EXPAND_H
#include "ParametricSurface.h"
#include "BaseScreen.h"


class Expand : public BaseScreen {
public:
    Expand(float radius,float aspect,void *mgn) ;
    vec3 Evaluate(const vec2& domain);
    vec2 EvaluateCoord(const vec2& domain);
    void TurnLeftRight(GLfloat angle,int scnindex);
    void TurnUpDown(GLfloat angle,int scnindex);
    void ResetPosition();
    void setZoom(GLfloat multi,int scnindex);

private:
    float m_radius;
    float mZoom;
};


#endif //FISHEYE_EXPAND_H
