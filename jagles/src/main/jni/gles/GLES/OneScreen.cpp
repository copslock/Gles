//
// Created by LiHong on 16/1/29.
// Copyright (c) 2016 LiHong. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include "OneScreen.h"


const char *OneScreenSeg = "#define PI 3.1415926535897932384626433832795\n"
        "\n"
        "uniform mat4 projection;\n"
        "uniform mat4 modelView;\n"
        "attribute vec4 vPosition;\n"
        "attribute vec2 aTexCoor;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "uniform float aTextureOffset;\n"
        "float aDiameterChange1;\n"
        "float thita;\n"
        "\n"
        "void main(){\n"
        "    vec3 lposition = vPosition.xyz;\n"
        "    gl_Position=projection*modelView*vec4(lposition,1.0);\n"
        "    vec2 cp = aTexCoor;\n"
        "    cp.x = cp.x-0.5;\n"
        "    cp.y = cp.y-0.5;\n"
        "    float c = length(cp);\n"
        "    float la = asin(abs(cp.y)/c);\n"
        "    float lx=cp.x;\n"
        "    float ly=cp.y;\n"
        "    if(lx<0.0)\n"
        "    {\n"
        "        if(ly<0.0)\n"
        "            la=PI-la;\n"
        "        else\n"
        "            la=la+PI;\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        if(ly>0.0)\n"
        "            la=2.0*PI-la;\n"
        "    }\n"
        "    cp.x = c * sin(aTextureOffset+la)+0.5;\n"
        "    cp.y = c * cos(aTextureOffset+la)+0.5;\n"
        "    vTextureCoord=cp;\n"
        "}";
const char *OneScreenFrag = "precision mediump float;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "uniform sampler2D sTexture;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor=texture2D(sTexture,vTextureCoord);\n"
        "}";

const char *YUV_OneScreenFrag = "precision mediump float;\n"
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

OneScreen::OneScreen(float radiusW,float radiusH,float aspect,float width,float left,
        float top,float start,void *mgn) : m_radiusW(radiusW),m_radiusH(radiusH)
{
    m_aspect = aspect;
    mWidth = width;
    _Manager = mgn;
    if(width>360)
        mWidth = 360.0f;
    if(width<90)
        mWidth = 90.0f;
    mLeft = left;
    mTop = top;
    mTextureOffset =0.0f;
    mStart = start/180.0f*Pi;
    ParametricInterval interval = { 4.0f, vec2(4,-8), vec2(mWidth+4,36) };
    SetInterval(interval);
#ifdef __USE_RGB32__
    setupProgram(OneScreenSeg,OneScreenFrag);
#else
    setupProgram(OneScreenSeg,YUV_OneScreenFrag,true);
#endif
    setupProjection();
    SetupBuffer();
    _Zoom = -2.73;
    mScreenMode = SCRN_ONESCREEN;
}

vec3 OneScreen::Evaluate(const vec2& domain)
{
    float u = domain.x , v = domain.y;
    float x = (m_radiusW*(u/90.0f)/(mWidth/90.0f)+mLeft-m_radiusW/2)*m_aspect;
    float y = (m_radiusH*(tan((v-12)/180.0f*Pi)*1.25)+mTop);
    float z = 1;
    return vec3(x, y, z);
}

vec2 OneScreen::EvaluateCoord(const vec2& domain)
{
 
//    int W =(int) (360.0f/anglespan);
//    int H =(int) (60.0f/anglespan);
//    int cx =(int)(domain.x/anglespan+W/4);
//    int cy =(int)(domain.y/anglespan+H/2);
//    
//    float temp = (float)( 2 * Pi )/W;
//    float theta =(float) (( 2 * Pi ) - cx * temp);
//    //theta = theta * PI / 180;
//    int r = cy;
//    float temp1 = (float) ( r * cos( theta ) );
//    float temp2 = (float) ( r * sin( theta ) );
//    //printf("   %d", r );
//    return vec2((temp1 + H)/(H*2),((H*2 - ( temp2 + H ))/(H*2)));
    
    int W =(int) (360.0f/anglespan);
    int H =(int) (60.0f/anglespan);
    int cx =(int)(domain.x/anglespan +W/4);
    int cy =(int)(domain.y/anglespan +H/2);

    float temp = (float)( 2 * Pi )/W;
    float theta =(float) (( 2 * Pi ) - cx * temp);
    //theta = theta * PI / 180;
    int r = cy;
    float temp1 = (float) ( r * cosf( theta ) );
    float temp2 = (float) ( r * sinf( theta ) );
    //printf("   %d", r );
    float x=(temp1 + H)/(H*2);
    float y=((H*2 - ( temp2 + H ))/(H*2));
    if(domain.y==21&&domain.x==3)
        printf("W:%d,H:%d,x:%f,y:%f,cx:%d,cy:%d,temp1:%f,temp2:%f\n",W,H,x,y,cx,cy,temp1,temp2);
//    if(x<0)
//        x=-x;
//    if(y<0)
//        y=-y;
//    if(x>1)
//        x=1;
//    if(y>1)
//        y=1;
    return vec2(x,y);
}

void OneScreen::setupProgram(const char *segsrc,const char *fragsrc)
{
    ParametricSurface::setupProgram(segsrc,fragsrc);
    mTextureOffsetHandle = glGetUniformLocation(_programHandle,"aTextureOffset");
}

void OneScreen::setupProgram(const char *segsrc,const char *fragsrc,bool yuv)
{
    ParametricSurface::setupProgram(segsrc,fragsrc,yuv);
    mTextureOffsetHandle = glGetUniformLocation(_programHandle,"aTextureOffset");
}


void OneScreen::DrawSelf(int scnindex)
{
    glUseProgram(_programHandle);
    glUniform1f(mTextureOffsetHandle, mTexPosition.x+mStart);
    BaseScreen::DrawSelf(0);
}
void OneScreen::TurnLeftRight(GLfloat angle,int scnindex)
{
    mTextureOffset-=angle/60;
}
void OneScreen::TurnUpDown(GLfloat angle,int scnindex)
{
    //ksRotate(&_rotationMatrix, angle, 1, 0, 0);
}

void OneScreen::ResetPosition() {
    ParametricSurface::ResetPosition();
    mTextureOffset =0.0f;
}
