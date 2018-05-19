//
// Created by LiHong on 16/1/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef FISHEYE_CYLINDER_H
#define FISHEYE_CYLINDER_H
#include "ParametricSurface.h"
#include "BaseScreen.h"

class Cylinder : public BaseScreen {
public:
    Cylinder(float radius,float aspect,void *mgn);
    vec3 Evaluate(const vec2& domain);
    vec2 EvaluateCoord(const vec2& domain);
    void setupProgram(const char *segsrc,const char *fragsrc);
    void setupProgram(const char *segsrc,const char *fragsrc,bool yuv);
    void DrawSelf(int scnindex);
    void TurnLeftRight(GLfloat angle,int scnindex);
    void TurnUpDown(GLfloat angle,int scnindex);
    void updateSurfaceTransform();
    void setupProjection();
    void CylinderSetupBuffer();
    void setZoom(GLfloat multi,int scnindex);
    void ResetPosition();
private:
    float m_radius;
    GLint mTextureOffsetHandle;
    GLint mDiameterChangeHandle;
    GLint mReferenceHandle;
    GLfloat mExpandDiameter;
    GLfloat mTextureOffset;
    GLuint mReferenceBuffer;
    GLfloat *fReferencebuf;
    GLfloat *fReferencebufadd;
};


#endif //FISHEYE_CYLINDER_H
