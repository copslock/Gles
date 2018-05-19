//
// Created by LiHong on 16/1/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "Hemisphere.h"

const char *HemisphereSeg = "uniform mat4 projection;\n"
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
const char *HemisphereFrag = "precision mediump float;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "uniform sampler2D sTexture;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor=texture2D(sTexture,vTextureCoord);\n"
        "}";

const char *YUV_HemisphereFrag = "precision mediump float;\n"
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


void Hemisphere::setupProjection() {
    ParametricSurface::setupProjection();
}


Hemisphere::Hemisphere(float radius,float aspect,void *mgn) : m_radius(radius)
{
    m_aspect = aspect;
    _Manager = mgn;
    ParametricInterval interval = { 6, vec2(96,6), vec2(276, 186) };
    SetInterval(interval);
#ifdef __USE_RGB32__
    setupProgram(HemisphereSeg,HemisphereFrag);
#else
    setupProgram(HemisphereSeg,YUV_HemisphereFrag,true);
#endif
    setupProjection();
    SetupBuffer();
    mScreenMode = SCRN_HEMISPHERE;
}

void Hemisphere::ResetPosition() {
    ParametricSurface::ResetPosition();
    _Zoom = -3;
}


void Hemisphere::SetScale(vec3 scale, bool texture, int scnindex)
{
	ParametricSurface::SetScale(scale, texture, scnindex);
	_Zoom = scale.x*20 - 23;
	if (_Zoom > 0)
		_Zoom = 0;
	if (_Zoom < -3)
		_Zoom = -3;
}

vec3 Hemisphere::Evaluate(const vec2& domain)
{
    float u = domain.x, v = domain.y;
    float x = m_radius * cosf(u/180.0f*Pi) * cosf(v/180.0f*Pi);
    float y = m_radius * sinf(u/180.0f*Pi);
    float z = m_radius * cosf(u/180.0f*Pi) * sinf(v/180.0f*Pi);
    return vec3(x, y, z);
}

vec2 Hemisphere::EvaluateCoord(const vec2& domain)
{
    vec3 npos = Evaluate(domain);
    npos.Normalize();
    vec2 px = vec2(npos.x,npos.y);
    //return px;
    float sinthita=px.Length();
    px.Normalize();
    //printf("px.x:%f,px.y:%f\n",px.x,px.y);
    if(sinthita>=1.0f)
        sinthita=1.0f;
    float thita=asinf(sinthita);
    //printf("thita:%f sinthita:%f\n",thita,sinthita);
    float py=2.0f*sinf(thita*0.5f);
    vec2 vTextureCoord=px*py*(float)((1.0/powf(2.0,0.5))*0.5);
    vTextureCoord.x= vTextureCoord.x+0.5f;
    vTextureCoord.y = vTextureCoord.y + 0.5f;
    vTextureCoord.y = 1.0f-vTextureCoord.y;
    //printf("x:%f,y:%f\n",vTextureCoord.x,vTextureCoord.y);
    return vTextureCoord;
}

void Hemisphere::TurnLeftRight(GLfloat angle,int scnindex)
{
    ksRotate(&_rotationMatrix, angle*90.0f, 0, 0, 1);
}
void Hemisphere::TurnUpDown(GLfloat angle,int scnindex)
{
    _udCurAngle += angle;
    KSMatrix4 rotate;
    ksMatrixLoadIdentity(&rotate);

    ksRotate(&rotate, angle*90.0f, 1, 0, 0);

    ksMatrixMultiply(&_rotationMatrix, &_rotationMatrix, &rotate);
}

void Hemisphere::setZoom(GLfloat multi,int scnindex)
{
    _Zoom = multi-4;
}
