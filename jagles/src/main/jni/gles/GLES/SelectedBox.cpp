//
// Created by LiHong on 16/7/29.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "SelectedBox.h"

#include "GLESMath.h"

#include <cstdlib>
#include <stdio.h>
#include <cstring>

const char* SelectedBoxSeg = "uniform mat4 projection;\n"
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
const char* SelectedBoxFrag = "precision mediump float;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "uniform sampler2D sTexture;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor=vec4(1.0,.0,.0,1.0);\n"
        "}";

SelectedBox::SelectedBox(float aspect)
{
    m_aspect = aspect;
    mIsUpdatePosition = false;
    setupProgram(SelectedBoxSeg, SelectedBoxFrag);
    setupProjection();
    SetupBuffer();
    _Zoom = -2.74;
}

SelectedBox::~SelectedBox()
{
    //if (mLineCoords)
    //    free(mLineCoords);
    glDeleteBuffers(1, &mLineBuffer);
    LOGDEBUG("4---glDeleteBuffers:%d %p",mLineBuffer,this);

}

void SelectedBox::DrawSelf(int scnindex)
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
}

void SelectedBox::SetupBuffer()
{
    if(mLineBuffer!=0)
        glDeleteBuffers(1,&mLineBuffer);
    float lineCoords[] = {
            -1.0f*m_aspect,-1.0f,1.0f,
            1.0f*m_aspect,-1.0f,1.0f,
            1.0f*m_aspect,-1.0f,1.0f,
            1.0f*m_aspect,1.0f,1.0f,
            -1.0f*m_aspect,1.0f,1.0f,
            1.0f*m_aspect,1.0f,1.0f,
            -1.0f*m_aspect,1.0f,1.0f,
            -1.0f*m_aspect,-1.0f,1.0f,
    };

    //memcpy(mLineCoords, lineCoords, 24 * sizeof(float));
    mLinebufferSize = 24;

    glGenBuffers(1, &mLineBuffer);
    LOGDEBUG("4--glGenBuffers:%d",mLineBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, mLineBuffer);
    glBufferData(GL_ARRAY_BUFFER, mLinebufferSize * sizeof(GLfloat), lineCoords, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

