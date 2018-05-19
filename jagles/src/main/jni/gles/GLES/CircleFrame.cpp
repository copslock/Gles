//
// Created by LiHong on 16/8/16.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include "CircleFrame.h"

const char* CircleFrameSeg = "uniform mat4 projection;\n"
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
const char* CircleFrameFrag = "precision mediump float;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "uniform vec4 vColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor=vColor;\n"
        "}";

CircleFrame::CircleFrame(float aspect)
{
    m_aspect = aspect;
    mLineCoords = NULL;

    mSelectedColor[0] = 1.0f;
    mSelectedColor[1] = .0f;
    mSelectedColor[2] = .0f;
    mSelectedColor[3] = 1.0f;

    mNormalColor[0] = .5f;
    mNormalColor[1] = 1.0f;
    mNormalColor[2] = .5f;
    mNormalColor[3] = 1.0f;
    mIsUpdateDiameter = false;
    mDiameter = 1.0f;
    mLineBuffer = 0;

//	ParametricInterval interval = { 1, vec2(1,1), vec2(2, 2) };
//	SetInterval(interval);
    setupProgram(CircleFrameSeg, CircleFrameFrag);
    setupProjection();
    SetupBuffer();
    _Zoom = -1.75;
}

CircleFrame::~CircleFrame()
{
    if (mLineCoords)
        free(mLineCoords);
    glDeleteBuffers(1, &mLineBuffer);
    LOGDEBUG("8---glDeleteBuffers:%d %p",mLineBuffer,this);

}

void CircleFrame::DrawSelf(int scnindex)
{
    glGetError();
    glUseProgram(_programHandle);
    if(mIsUpdateDiameter)
    {
        glDeleteBuffers(1, &mLineBuffer);
        SetupBuffer();
        mIsUpdateDiameter = false;
    }
    updateSurfaceTransform();
    mColorHandle = glGetUniformLocation(_programHandle, "vColor");
//	if (scnindex == 0xffffffff)
    glUniform4fv(mColorHandle, 1, mSelectedColor);
//	else
//		glUniform4fv(mColorHandle, 1, mNormalColor);
    glBindBuffer(GL_ARRAY_BUFFER, mLineBuffer);
    glVertexAttribPointer(_positionSlot, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(_positionSlot);
    glLineWidth(2);
    //glDrawArrays(GL_TRIANGLES, 0, mLinebufferSize / 3);
    glDrawArrays(GL_LINES, 0, mLinebufferSize / 3);
    glDisableVertexAttribArray(_positionSlot);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //printf("CircleFrame drawself.....%x\n", glGetError());
}

void CircleFrame::SetupBuffer()
{
    if (mLineCoords)
        free(mLineCoords);
    int lsize = 120;
    mLinebufferSize = lsize * 6;

    mLineCoords = (float *)malloc(mLinebufferSize * sizeof(float));
    for (int i = 0; i < lsize; i++)
    {
        mLineCoords[i * 6] = cos((i * 3) / 180.0f*Pi) * mDiameter;
        mLineCoords[i * 6 + 1] = sin((i * 3) / 180.0f*Pi) * mDiameter;
        mLineCoords[i * 6 + 2] =0;
        mLineCoords[i * 6 + 3] =  cos(((i + 1) * 3) / 180.0f*Pi) * mDiameter;
        mLineCoords[i * 6 + 4] = sin(((i + 1) * 3) / 180.0f*Pi) * mDiameter;
        mLineCoords[i * 6 + 5] =0;
    }

    if(mLineBuffer)
        glDeleteBuffers(1,&mLineBuffer);
    glGenBuffers(1, &mLineBuffer);
    LOGDEBUG("8--glGenBuffers:%d",mLineBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, mLineBuffer);
    glBufferData(GL_ARRAY_BUFFER, mLinebufferSize * sizeof(GLfloat), mLineCoords, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void CircleFrame::UpdateDiameter(float diameter)
{
    mDiameter = diameter;
    mIsUpdateDiameter = true;
}
