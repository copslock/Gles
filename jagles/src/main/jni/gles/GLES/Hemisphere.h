//
// Created by LiHong on 16/1/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//


#ifndef FISHEYE_HEMISPHERE_H
#define FISHEYE_HEMISPHERE_H
#include "ParametricSurface.h"
#include "BaseScreen.h"


class Hemisphere : public BaseScreen {
public:
    Hemisphere(float radius,float aspect,void *mgn) ;
    vec3 Evaluate(const vec2& domain);
    vec2 EvaluateCoord(const vec2& domain);
    void TurnLeftRight(GLfloat angle,int scnindex);
    void TurnUpDown(GLfloat angle,int scnindex);
    void setZoom(GLfloat multi,int scnindex);
    void setupProjection();
    void ResetPosition();
	void SetScale(vec3 scale, bool texture, int scnindex);
private:
    float m_radius;
};

#endif //FISHEYE_HEMISPHERE_H
