//
// Created by LiHong on 16/1/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include <stdio.h>
#include "NormalPlay.h"
#include "GLESMath.h"
#define LOGV(...)__android_log_print(ANDROID_LOG_INFO,"honglee_0420",__VA_ARGS__)

//
// Created by LiHong on 16/1/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

const char *NormalPlaySeg = "uniform mat4 projection;\n"
        "uniform mat4 modelView;\n"
        "attribute vec4 vPosition;\n"
        "attribute vec2 aTexCoor;\n"
        "uniform float aLeftOffset;\n"
        "uniform float aTopOffset;\n"
        "uniform float aZoom;\n"
        "uniform float aKeepAspect;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "varying float vKeepAspect;\n"
        "\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = projection * modelView * vPosition;\n"
        "    vTextureCoord = aTexCoor;\n"
        "    vTextureCoord = vTextureCoord-0.5;\n"
        "    vTextureCoord = vTextureCoord*aZoom;\n"
        "    vTextureCoord = vTextureCoord+0.5;\n"
        "    vTextureCoord.x = vTextureCoord.x+aLeftOffset;\n"
        "    vTextureCoord.y = vTextureCoord.y+aTopOffset;\n"
        "    vKeepAspect = aKeepAspect;\n"
        "}";
const char *NormalPlayFrag = "precision mediump float;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "varying float vKeepAspect;\n"
        "uniform sampler2D sTexture;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec2 lcoord;\n"
        "    if(vKeepAspect>1.0)\n"
        "    {\n"
        "       lcoord=vec2(vTextureCoord.x,(vTextureCoord.y-0.5)*vKeepAspect+0.5);\n"
        "       if(lcoord.y<0.0||lcoord.y>1.0)\n"
        "           gl_FragColor=vec4(0.0,0.0,0.0,1.0);\n"
        "       else\n"
        "           gl_FragColor=texture2D(sTexture,lcoord);\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "       lcoord=vec2((vTextureCoord.x-0.5)/vKeepAspect+0.5,vTextureCoord.y);\n"
        "       if(lcoord.x<0.0||lcoord.x>1.0)\n"
        "           gl_FragColor=vec4(0.0,0.0,0.0,1.0);\n"
        "       else\n"
        "           gl_FragColor=texture2D(sTexture,lcoord);\n"
        "    }\n"
        "}";

const char *YUV_NormalPlayFrag = "precision mediump float;\n"
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


NormalPlay::NormalPlay(float radius,float aspect,float left,float top,void *mgn) : m_radius(radius),mLeft(left),mTop(top)
{
    _Manager = mgn;
    m_aspect = aspect;
    mLeftOffset = 0.0f;
    mTopOffset = 0.0f;
    mKeepAspect = 1.0f;
    mIsSelected = false;
    ParametricInterval interval = { 1, vec2(1,1), vec2(2, 2) };
    SetInterval(interval);
#ifdef __USE_RGB32__
    setupProgram(NormalPlaySeg,NormalPlayFrag);
#else
    setupProgram(NormalPlaySeg,YUV_NormalPlayFrag,true);
#endif
    setupProjection();
    SetupBuffer();
    mZoom = 1.0;
    _Zoom = -2.74;
    DebugIndex = 0xff;
    mScreenMode = SCRN_ONENORMAL;
}

vec3 NormalPlay::Evaluate(const vec2& domain)
{
    float u = domain.x, v = domain.y;
    float x = (m_radius*u-m_radius/2+mLeft)*m_aspect;
//    float y = (m_radius*v-m_radius/2)*9.0/16.0+mTop;
    float y = m_radius*v-m_radius/2+mTop;
    float z = 1;
    return vec3(x, y, z);
}

vec2 NormalPlay::EvaluateCoord(const vec2& domain)
{
    float u = domain.x, v = domain.y;
    float x = u;
    float y = 1-v;
    return vec2(x, y);
}

void NormalPlay::setupProgram(const char *segsrc,const char *fragsrc,bool yuv)
{
    ParametricSurface::setupProgram(segsrc,fragsrc,yuv);
    mZoomHandle = glGetUniformLocation(_programHandle,"aZoom");
    //printf("value:%d,mZoomHandle:%d\n",mZoomHandle,glGetError());
    mLeftOffsetHandle = glGetUniformLocation(_programHandle,"aLeftOffset");
    //printf("value:%d,mLeftOffsetHandle:%d\n",mLeftOffsetHandle,glGetError());
    mTopOffsetHandle = glGetUniformLocation(_programHandle,"aTopOffset");
    mKeepAspectHandle = glGetUniformLocation(_programHandle,"aKeepAspect");
    //printf("value:%d,mTopOffsetHandle:%d\n",mTopOffsetHandle,glGetError());
}

void NormalPlay::setupProgram(const char *segsrc,const char *fragsrc)
{
    ParametricSurface::setupProgram(segsrc,fragsrc);
    mZoomHandle = glGetUniformLocation(_programHandle,"aZoom");
    //printf("value:%d,mZoomHandle:%d\n",mZoomHandle,glGetError());
    mLeftOffsetHandle = glGetUniformLocation(_programHandle,"aLeftOffset");
    //printf("value:%d,mLeftOffsetHandle:%d\n",mLeftOffsetHandle,glGetError());
    mTopOffsetHandle = glGetUniformLocation(_programHandle,"aTopOffset");
    mKeepAspectHandle = glGetUniformLocation(_programHandle,"aKeepAspect");
    //printf("value:%d,mTopOffsetHandle:%d\n",mTopOffsetHandle,glGetError());
}

void NormalPlay::DrawSelf(int scnindex)
{
    if(!Visible)
        return;
    //LOGV("----------------------");
    glUseProgram(_programHandle);
    glUniform1f(mZoomHandle, 1/mTexScale.x);
    glUniform1f(mLeftOffsetHandle, mTexPosition.x);
    glUniform1f(mTopOffsetHandle, mTexPosition.y);
    if(mKeepAspect!=1.0)
        glUniform1f(mKeepAspectHandle,mKeepAspect/m_aspect);
    else
        glUniform1f(mKeepAspectHandle,1.0);
//    if(!DebugIndex)
//        LOGV("GLInfo-->texid:%d,slothandle:%d,bufferid:%d %p",_texid,_coordSlot,vertexBuffer,this);
    BaseScreen::DrawSelf(scnindex);
}



void NormalPlay::TurnLeftRight(GLfloat angle,int scnindex)
{
    if(mZoom>=1.0f)
        return;
    float lw = (1-mZoom)/2;
    if(angle+mLeftOffset<-lw||angle+mLeftOffset>lw)
        return;
    mLeftOffset += angle;
}

void NormalPlay::TurnUpDown(GLfloat angle,int scnindex)
{
    if(mZoom>=1.0f)
        return;
    float lw = (1-mZoom)/2;
    if(angle+mTopOffset<-lw||angle+mTopOffset>lw)
        return;
    mTopOffset += angle;
}

void NormalPlay::ResetPosition() {
    ParametricSurface::ResetPosition();
	SetIsAnimation(0,false);
    mLeftOffset = 0.0f;
    mTopOffset = 0.0f;
    mZoom = 1.0f;
}


void NormalPlay::SetPosition(vec3 pos, bool texture, int scnindex)
{
	if(!texture)
	{
		BaseScreen::SetPosition(pos, false, 0);
		return;
	}
	vec3 lscale = GetScale(true, 0);
    float lw;
    float lh;
    if(mKeepAspect!=1.0f) {
        if (m_aspect < mKeepAspect) {
            lw = (1 - 1 / lscale.x) / 2;
            lh = (m_aspect / mKeepAspect - 1.0 / lscale.x) / 2.0;
        } else {
            lh = (1 - 1 / lscale.x) / 2;
            lw = (mKeepAspect / m_aspect - 1.0 / lscale.x) / 2.0;
        }
    } else
    {
        lw = (1 - 1 / lscale.x) / 2;
        lh = (1 - 1 / lscale.x) / 2;
    }
	if (pos.y < -lh)
		pos.y = -lh;
	if (pos.y > lh)
		pos.y = lh;
    if(lh<0.0)
        pos.y = 0.0;
	if (pos.x < -lw)
		pos.x = -lw;
	if (pos.x > lw)
		pos.x = lw;
    if(lw<0.0)
        pos.x = 0.0;
	BaseScreen::SetPosition(pos,true, 0);
}
	
void NormalPlay::SetScale(vec3 scale, bool texture, int scnindex)
{
	if(!texture)
	{
		BaseScreen::SetScale(scale, false, 0);
		return;
	}
	if (scale.x < 1)
		return;
//	float lw = (1 - 1 / scale.x) / 2;
	vec3 pos = GetPosition(true, 0);
    float lw;
    float lh;
    if(mKeepAspect!=1.0f) {
        if (m_aspect < mKeepAspect) {
            lw = (1 - 1 / scale.x) / 2;
            lh = (m_aspect / mKeepAspect - 1.0 / scale.x) / 2.0;
        } else {
            lh = (1 - 1 / scale.x) / 2;
            lw = (mKeepAspect / m_aspect - 1.0 / scale.x) / 2.0;
        }
    } else
    {
        lw = (1 - 1 / scale.x) / 2;
        lh = (1 - 1 / scale.x) / 2;
    }
    if (pos.y < -lh)
        pos.y = -lh;
    if (pos.y > lh)
        pos.y = lh;
    if(lh<0.0)
        pos.y = 0.0;
	if (pos.x < -lw)
		pos.x = -lw;
	if (pos.x > lw)
		pos.x = lw;
    if(lw<0.0)
        pos.x = 0.0;
	BaseScreen::SetPosition(pos, true, 0);
	BaseScreen::SetScale(scale, true, 0);
}

void NormalPlay::setZoom(GLfloat multi, int scnindex)
{
    mZoom = 1/multi;
    float lw = (1-mZoom)/2;
    if(mTopOffset<-lw)
        mTopOffset = -lw;
    if(mTopOffset>lw)
        mTopOffset = lw;
    if(mLeftOffset<-lw)
        mLeftOffset = -lw;
    if(mLeftOffset>lw)
        mLeftOffset = lw;
}

void NormalPlay::ReSetBuffer(float aspect)
{
	m_aspect = aspect;
	setupProjection();
	SetupBuffer();
	_Zoom = -2.74;
}



