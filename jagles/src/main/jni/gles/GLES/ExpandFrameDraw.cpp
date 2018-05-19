//
// Created by LiHong on 16/5/31.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "ExpandFrameDraw.h"

#include "GLESMath.h"

#include <cstdlib>
#include <stdio.h>
#include <cstring>

const char* ExpandFrameDrawSeg = "uniform mat4 projection;\n"
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
const char* ExpandFrameDrawFrag = "precision mediump float;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "uniform sampler2D sTexture;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor=vec4(1.0,.0,.0,1.0);\n"
        "}";

ExpandFrameDraw::ExpandFrameDraw(float radius,float tbstart,float tbend,float lrstart,float lrend,float aspect)
{
    mLeft = lrstart;
    mRigth = lrend;
    mTop = tbstart;
    mBottom = tbend;
    m_aspect = aspect;
    m_radius = radius;
    mLineBuffer = 0;
    mLineCoords = NULL;
    mIsUpdatePosition = false;
//	ParametricInterval interval = { 1, vec2(1,1), vec2(2, 2) };
//	SetInterval(interval);
    setupProgram(ExpandFrameDrawSeg, ExpandFrameDrawFrag);
    setupProjection();
    SetupBuffer();
    _Zoom = -2.74;
}

ExpandFrameDraw::~ExpandFrameDraw()
{
    if (mLineCoords)
        free(mLineCoords);
    LOGDEBUG("0---glDeleteBuffers:%d %p",mLineBuffer,this);
    glDeleteBuffers(1, &mLineBuffer);
}

void ExpandFrameDraw::DrawSelf(int scnindex)
{
    glGetError();
    glUseProgram(_programHandle);
    if(mIsUpdatePosition)
    {
        glDeleteBuffers(1, &mLineBuffer);
        SetupBuffer();
        mIsUpdatePosition = false;
    }
    updateSurfaceTransform();
    glBindBuffer(GL_ARRAY_BUFFER, mLineBuffer);
    glVertexAttribPointer(_positionSlot, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(_positionSlot);
    glLineWidth(2);
    glDrawArrays(GL_LINES, 0, mLinebufferSize / 3);
    glDisableVertexAttribArray(_positionSlot);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //printf("ExpandFrameDraw drawself.....%x\n", glGetError());
}

void ExpandFrameDraw::SetupBuffer()
{
    if (mLineCoords)
        free(mLineCoords);
    
    mLineCoords = (float *)malloc(24*sizeof(float));

	float lineCoords[] = {
            (mLeft-180.0f)/180.f*m_aspect,(mTop-35.0f)/35.0f,1.0f,
            (mRigth-180.0f)/180.f*m_aspect,(mTop-35.0f)/35.0f,1.0f,
            (mRigth-180.0f)/180.f*m_aspect,(mTop-35.0f)/35.0f,1.0f,
            (mRigth-180.0f)/180.f*m_aspect,(mBottom-35.0f)/35.0f,1.0f,
            (mLeft-180.0f)/180.f*m_aspect,(mBottom-35.0f)/35.0f,1.0f,
            (mRigth-180.0f)/180.f*m_aspect,(mBottom-35.0f)/35.0f,1.0f,
            (mLeft-180.0f)/180.f*m_aspect,(mBottom-35.0f)/35.0f,1.0f,
            (mLeft-180.0f)/180.f*m_aspect,(mTop-35.0f)/35.0f,1.0f,
	};
	

    memcpy(mLineCoords, lineCoords, 24 * sizeof(float));
    mLinebufferSize = 24;

    glGenBuffers(1, &mLineBuffer);
    LOGDEBUG("0--glGenBuffers:%d",mLineBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mLineBuffer);
    glBufferData(GL_ARRAY_BUFFER, mLinebufferSize * sizeof(GLfloat), mLineCoords, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void ExpandFrameDraw::UpdatePosition(float tbstart, float tbend, float lrstart, float lrend)
{
    mLeft = (int)lrstart % 360;
    mRigth = (int)lrend % 360;
    if(mRigth<mLeft) {
        if(mLeft<0&&mRigth<0)
        {
            mLeft = 0;
            mRigth = 360+mRigth;
        }
        else
          mRigth = 360;
    }
    else if(mLeft<0 && mRigth<0)
    {
        mLeft=360+mLeft;
        mRigth = 360 + mRigth;
    }
    if(mLeft<0)
        mLeft = 0;
    mTop = 70-tbstart;
    mBottom = 70-tbend;
    mIsUpdatePosition = true;
    //printf("-------mtop:%f,mbottom:%f\n",mTop,mBottom);
}
