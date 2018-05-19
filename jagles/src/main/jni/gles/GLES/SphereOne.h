//
// Created by LiHong on 16/8/15.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef OSX_SPHEREONE_H
#define OSX_SPHEREONE_H

#include "BaseScreen.h"


class SphereOne: public BaseScreen {
public:
    SphereOne(float radius,float aspect,void *mgn);
    vec3 Evaluate(const vec2& domain);
    vec2 EvaluateCoord(const vec2& domain);
    void setupProgram(const char *segsrc,const char *fragsrc);
    void setupProgram(const char *segsrc,const char *fragsrc,bool yuv);
    void DrawSelf(int scnindex);
    bool LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex);
    bool LoadTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels, int scnindex);
    void SetFishParameter(float cx,float cy,float diameter,float angle){mCenterX=cx;mCenterY=cy;mDiameter=diameter;mAngle=angle;};
    void GetFishParameter(float *cx,float *cy,float *diameter,float *angle){*cx=mCenterX;*cy=mCenterY;*diameter=mDiameter;*angle=mAngle;};
private:
    float m_radius;
    float mTexWidth,mTexHeight,mDiameter,mCenterX,mCenterY,mAngle;
    GLuint mRotateHandle;
};


#endif //OSX_SPHEREONE_H
