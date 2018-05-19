//
// Created by LiHong on 16/1/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include <stdio.h>
#include "Expand.h"


const char *ExpandSeg = "uniform mat4 projection;\n"
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
const char *ExpandFrag = "precision mediump float;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "uniform sampler2D sTexture;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor=texture2D(sTexture,vTextureCoord);\n"
        "}";

const char *YUV_ExpandFrag = "precision mediump float;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        " uniform sampler2D s_texture_y;\n"
        " uniform sampler2D s_texture_cbcr;\n"
        "\n"
        "void main()\n"
        "{\n"
        "     highp float y = texture2D(s_texture_y, vTextureCoord).r;\n"
        "     highp float u = texture2D(s_texture_cbcr, vTextureCoord).a - 0.5;\n"
        "     highp float v = texture2D(s_texture_cbcr, vTextureCoord).r - 0.5;\n"
        "     \n"
        "     highp float r = y +             1.402 * v;\n"
        "     highp float g = y - 0.344 * u - 0.714 * v;\n"
        "     highp float b = y + 1.772 * u;\n"
        "     \n"
        "     gl_FragColor = vec4(r,g,b,1.0);     \n"
        "}";

Expand::Expand(float radius,float aspect,void *mgn) : m_radius(radius)
{
    m_aspect = aspect;
    _Manager = mgn;
    ParametricInterval interval = { 3, vec2(-42,3), vec2(48, 183) };
    SetInterval(interval);
#ifdef __USE_RGB32__
    setupProgram(ExpandSeg,ExpandFrag);
#else
    setupProgram(ExpandSeg,YUV_ExpandFrag,true);
#endif
    setupProjection();
    SetupBuffer();
    //ksRotate(&_rotationMatrix, -90.0f, 0, 0, 1);
    setRotate(vec3(0,0,-90),false,0);
    mZoom = 1.0f;
    mScreenMode  = SCRN_EXPAND;
}

vec3 Expand::Evaluate(const vec2& domain)
{
    float u = domain.x, v = domain.y;
    float x = m_radius * (u/180.0f)*2;
    float y = m_radius * ((v-90.0f)/180.0f) * m_aspect;
    float z = 1;
    return vec3(x, y, z);
}

vec2 Expand::EvaluateCoord(const vec2& domain)
{
    float u = domain.x, v = domain.y;
    float x = m_radius * cos(u/180.0f*Pi) * cos(v/180.0f*Pi);
    float y = m_radius * sin(u/180.0f*Pi);
    float z = m_radius * cos(u/180.0f*Pi) * sin(v/180.0f*Pi);
    vec3 npos = vec3(x,y,z);
    npos.Normalize();
    vec2 px = vec2(npos.x,npos.y);
    //return px;
    float sinthita=px.Length();
    px.Normalize();
    float thita=asin(sinthita);
    float py=2.0*sin(thita*0.5);
    vec2 vTextureCoord=px*py*(float)((1.0/pow(2.0,0.5))*0.5);
    vTextureCoord.x= (vTextureCoord.x+0.5);
    vTextureCoord.y = vTextureCoord.y + 0.5;
    vTextureCoord.y = (1.0-vTextureCoord.y);
    return vTextureCoord;
}

void Expand::ResetPosition()
{
    ParametricSurface::ResetPosition();
    _Zoom = -3;
    setRotate(vec3(0,0,-90),false,0);
    mZoom = 1.0f;
}

void Expand::TurnLeftRight(GLfloat angle,int scnindex)
{
    //ksRotate(&_rotationMatrix, angle, 0, 0, 1);
    ksTranslate(&_rotationMatrix,0,angle,0);
}
void Expand::TurnUpDown(GLfloat angle,int scnindex)
{
    ksTranslate(&_rotationMatrix,angle,0,0);
//    _udCurAngle += angle;
//    KSMatrix4 rotate;
//    ksMatrixLoadIdentity(&rotate);
//
//    ksRotate(&rotate, angle, 1, 0, 0);
//
//    ksMatrixMultiply(&_rotationMatrix, &_rotationMatrix, &rotate);
}

void Expand::setZoom(GLfloat multi,int scnindex)
{
    //_Zoom = multi-4;
    float lzoom = (multi - mZoom)/mZoom;
    mZoom = multi;
    ksScale(&_rotationMatrix,1+lzoom,1+lzoom,1);
    printf("mZoom:%f\n",mZoom);
}

