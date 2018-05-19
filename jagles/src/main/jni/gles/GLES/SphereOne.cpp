//
// Created by LiHong on 16/7/27.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "SphereOne.h"


const char *SphereOneSeg=
        "uniform mat4 projection;\n"
                "uniform mat4 modelView;\n"
                "attribute vec4 vPosition;\n"
                "uniform float aRotate;\n"
                "attribute vec2 aTexCoor;\n"
                "\n"
                "varying vec2 vTextureCoord;\n"
                "\n"
                "vec2 GetCoord(float pRotate)\n"
                "{\n"
                "    float sin_factor = sin(-pRotate);\n"
                "    float cos_factor = cos(-pRotate);\n"
                "    vec2 tmpcoor = vec2(aTexCoor.x - 0.5, aTexCoor.y - 0.5) * mat2(cos_factor, sin_factor, -sin_factor, cos_factor);\n"
                "    return vec2(tmpcoor.x+0.5,tmpcoor.y+0.5);\n"
                "}\n"
                "void main(void)\n"
                "{\n"
                "    gl_Position = projection * modelView * vPosition;\n"
                "    vTextureCoord = GetCoord(aRotate);\n"
                "}";

const char *SphereOneFrag ="precision mediump float;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "uniform sampler2D sTexture;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor=texture2D(sTexture,vTextureCoord);\n"
        "}";

const char *YUV_SphereOneFrag ="precision mediump float;\n"
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

SphereOne::SphereOne(float radius,float aspect,void *mgn) : m_radius(radius)
{
    _Manager = mgn;
    m_aspect = aspect;
    mIsSelected = false;
    ParametricInterval interval = { 6, vec2(6,186), vec2(366, 276) };
    SetInterval(interval);
#ifdef __USE_RGB32__
    setupProgram(SphereOneSeg,SphereOneFrag);
#else
    setupProgram(SphereOneSeg,YUV_SphereOneFrag,true);
#endif
    setupProjection();
    SetupBuffer();
    _Zoom = -3;
    mTexHeight = 0;
    mTexWidth = 0;
    mCenterX = 0.5;
    mCenterY = 0.5;
    mDiameter = 1.0;
    mScreenMode = SCRN_STITCHONE;
}

vec3 SphereOne::Evaluate(const vec2& domain)
{
//    float u = domain.x, v = domain.y;
//    float x =m_radius * cos(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
//    float y =m_radius * sin(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
//    float z =m_radius * sin((v - 180) / 180.0f * Pi);
//    return vec3(x,y,z);
    float u = domain.x, v = domain.y;
    float x=0;
    float y=0;
    float z=0;
    float rx,ry;
    x = 2 * cos(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
    y = 2 * sin(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
    z = 2 * sin((v - 180) / 180.0f * Pi);
    //return vec3(x,y,z);
    if (z == 0.0)
        z = 0.0000001;
    rx = 2 * atanf(x / z) / Pi ;
    ry = -sinf(u / 180.0f * Pi) * sinf((270 - v) / 180.0 * Pi);
    x = rx;
    y = ry;
    z = 1.265;
    return vec3(x, y, z);
}

vec2 SphereOne::EvaluateCoord(const vec2& domain)
{
    float u = domain.x, v = domain.y;
    float xs = 90.0f;
    xs = xs/90.0f;
    vec2 vTextureCoord = vec2(0,0);
    float x = m_radius * cos(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
    float y = m_radius * sin(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
    float z = m_radius * sin(v / 180.0f * Pi);
    vec3 npos = vec3(x, y, z);
    npos.Normalize();
    vec2 px = vec2(npos.x, npos.y);
    //return px;
    float sinthita = px.Length();
    px.Normalize();
    if (sinthita >= 1.0f)
        sinthita = 1.0f;
    float thita = asinf(sinthita);
    float py = 2.0f * sinf(thita * 0.5f);
    vTextureCoord = px * py * (float) ((1.0 / powf(2.0, 0.5)) * 0.5)/xs;
    vTextureCoord.x = vTextureCoord.x + 0.5f;
    vTextureCoord.y = vTextureCoord.y + 0.5f;
    vTextureCoord.y = (1.0f - vTextureCoord.y);
    return vTextureCoord;
}


void SphereOne::setupProgram(const char *segsrc, const char *fragsrc) {
    ParametricSurface::setupProgram(segsrc, fragsrc);
    mRotateHandle = glGetUniformLocation(_programHandle,"aRotate");
}

void SphereOne::setupProgram(const char *segsrc, const char *fragsrc, bool yuv) {
    ParametricSurface::setupProgram(segsrc, fragsrc, yuv);
    mRotateHandle = glGetUniformLocation(_programHandle,"aRotate");
}

void SphereOne::DrawSelf(int scnindex) {
    glUseProgram(_programHandle);
    glUniform1f(mRotateHandle,mAngle);

    //printf("-----------------------x:%f y:%f diameter:%f\n",mCenterX,mCenterY,mDiameter);
    BaseScreen::DrawSelf(scnindex);
}

bool SphereOne::LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex) {
//    if(mDiameter==0)
//    {
//        printf("....................................\n");
//        mDiameter = 1.0;
//        mCenterX = 0.5;
//        mCenterY = 0.5;
//    }
    mTexWidth = w;
    mTexHeight = h;
    return ParametricSurface::LoadTexture(w, h, type, pixels, scnindex);
}

bool SphereOne::LoadTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels, int scnindex) {
//    if(mDiameter==0)
//    {
//        mDiameter = 1.0;
//        mCenterX = 0.5;
//        mCenterY = 0.5;
//    }
    mTexWidth = w;
    mTexHeight = h;
    return ParametricSurface::LoadTexture(w, h, y_pixels, cbcr_pixels, scnindex);
}
