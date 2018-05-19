//
// Created by LiHong on 16/1/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "ToursLoading.h"

const char *ToursLoadingSeg = "uniform mat4 projection;\n"
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
const char *ToursLoadingFrag = "precision mediump float;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "uniform sampler2D sTexture;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor=vec4(vTextureCoord.x,vTextureCoord.x,vTextureCoord.x,1.0);\n"
//        "    gl_FragColor=texture2D(sTexture,vTextureCoord);\n"
        "}";

ToursLoading::ToursLoading(float majorRadius,float minorRadius,float aspect) : m_majorRadius(majorRadius),m_minorRadius(minorRadius)
{
    m_aspect = aspect;
    ParametricInterval interval = { 3, vec2(3,3), vec2(363, 30) };
    SetInterval(interval);
    setupProgram(ToursLoadingSeg,ToursLoadingFrag);
    setupProjection();
    SetupBuffer();
    _Zoom = -2.73;
    mScreenMode = SCRN_HEMISPHERE;
}

void ToursLoading::ResetPosition() {
    ParametricSurface::ResetPosition();
    _Zoom = -2.73;
}

vec3 ToursLoading::Evaluate(const vec2& domain)
{
    const float major = m_majorRadius;
    const float minor = m_minorRadius;
    const float value = m_majorRadius-m_minorRadius;
    float u = domain.x, v = domain.y;
    float x = (value*(v/30) + minor) * cos(u/180.f*Pi);
    float y = (value*(v/30) + minor) * sin(u/180.f*Pi);
    float z = 1;
    return vec3(x, y, z);
}

vec2 ToursLoading::EvaluateCoord(const vec2& domain)
{
    return vec2((360-domain.x)/360.f,0);
}

void ToursLoading::TurnLeftRight(GLfloat angle,int scnindex)
{
    ksRotate(&_rotationMatrix, angle*90.0f, 0, 0, 1);
}
void ToursLoading::TurnUpDown(GLfloat angle,int scnindex)
{
    _udCurAngle += angle;
    KSMatrix4 rotate;
    ksMatrixLoadIdentity(&rotate);

    ksRotate(&rotate, angle*90.0f, 1, 0, 0);

    ksMatrixMultiply(&_rotationMatrix, &_rotationMatrix, &rotate);
}

void ToursLoading::setZoom(GLfloat multi,int scnindex)
{
    _Zoom = multi-4;
}

void ToursLoading::StartAnimation(int scnindex)
{

}

void ToursLoading::StopAnimation(int scnindex)
{

}

void ToursLoading::ReSizeSplite(float aspect)
{
	m_aspect = aspect;
	setupProjection();
	_Zoom = -2.73;
}

