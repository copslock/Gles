#include "framedraw.h"
#include <cstdlib>
#include <stdio.h>

const char* SixFrameDrawSeg = "uniform mat4 projection;\n"
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
const char* SixFrameDrawFrag = "precision mediump float;\n"
							"\n"
							"varying vec2 vTextureCoord;\n"
							"uniform vec4 vColor;\n"
							"\n"
							"void main()\n"
							"{\n"
							"    gl_FragColor=vColor;\n"
							"}";

SixFrameDraw::SixFrameDraw(float radius,float tbstart,float tbend,float lrstart,float lrend,float aspect)
{
	mLeft = lrstart;
	mRigth = lrend;
	mTop = tbstart;
	mBottom = tbend;
	m_aspect = aspect;
	m_radius = radius;
	mLineCoords = NULL;
	mLineBuffer = 0;

	mSelectedColor[0] = 1.0f;
	mSelectedColor[1] = .0f;
	mSelectedColor[2] = .0f;
	mSelectedColor[3] = 1.0f;

	mNormalColor[0] = .5f;
	mNormalColor[1] = 1.0f;
	mNormalColor[2] = .5f;
	mNormalColor[3] = 1.0f;
	mIsUpdatePosition = false;

//	ParametricInterval interval = { 1, vec2(1,1), vec2(2, 2) };
//	SetInterval(interval);
	setupProgram(SixFrameDrawSeg, SixFrameDrawFrag);
	setupProjection();
	SetupBuffer();
	_Zoom = -3;
}

SixFrameDraw::~SixFrameDraw()
{
	if (mLineCoords)
		free(mLineCoords);
	glDeleteBuffers(1, &mLineBuffer);
	LOGDEBUG("1---glDeleteBuffers:%d %p",mLineBuffer,this);
}

void SixFrameDraw::DrawSelf(int scnindex)
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
	//printf("SixFrameDraw drawself.....%x\n", glGetError());
}

void SixFrameDraw::SetupBuffer()
{
	if (mLineCoords)
		free(mLineCoords);
//	mRigth = 90 * 1.7;
//	mLeft = 35;
//	mTop = 0;
//	mBottom = 60;
//	m_radius = 1.7;
	int lsize = (mRigth - mLeft) / 3;
	mLinebufferSize = lsize * 12 + 12;

	mLineCoords = (float *)malloc(mLinebufferSize * sizeof(float));
	for (int i = 0; i < lsize; i++)
	{
		mLineCoords[i * 6] = cos((i * 3 + mLeft) / 180.0f*Pi) * m_radius*cos(mTop / 180.0f*Pi);
		mLineCoords[i * 6 + 1] = sin((i * 3 + mLeft) / 180.0f*Pi)*m_radius*cos(mTop / 180.0f*Pi);
		mLineCoords[i * 6 + 2] =-sin(mTop / 180.0f*Pi)*m_radius;
		mLineCoords[i * 6 + 3] =  cos(((i + 1) * 3 + mLeft) / 180.0f*Pi) * m_radius*cos(mTop / 180.0f*Pi);
		mLineCoords[i * 6 + 4] = sin(((i + 1) * 3 + mLeft) / 180.0f*Pi)*m_radius*cos(mTop / 180.0f*Pi);
		mLineCoords[i * 6 + 5] =-sin(mTop / 180.0f*Pi)*m_radius;
	}
	for (int i = 0; i < lsize; i++)
	{
		mLineCoords[lsize * 6 + i * 6] = cos((i * 3 + mLeft) / 180.0f*Pi) * m_radius*cos(mBottom / 180.0f*Pi);
		mLineCoords[lsize * 6 + i * 6 + 1] = sin((i * 3 + mLeft) / 180.0f*Pi)*m_radius*cos(mBottom / 180.0f*Pi);
		mLineCoords[lsize * 6 + i * 6 + 2] = -sin(mBottom / 180.0f*Pi)*m_radius;
		mLineCoords[lsize * 6 + i * 6 + 3] = cos(((i + 1) * 3 + mLeft) / 180.0f*Pi) * m_radius*cos(mBottom / 180.0f*Pi);
		mLineCoords[lsize * 6 + i * 6 + 4] = sin(((i + 1) * 3 + mLeft) / 180.0f*Pi)*m_radius*cos(mBottom / 180.0f*Pi);
		mLineCoords[lsize * 6 + i * 6 + 5] = -sin(mBottom / 180.0f*Pi)*m_radius;
	}
	//printf("mLeft:%f,mRight:%f,mTop:%f,mBottom:%f\n", mLeft, mRigth, mTop, mBottom);
	if (mBottom <= 90)
	{
		mLineCoords[lsize * 12] = cos(mLeft / 180.0f*Pi) * cos(mTop / 180.0f*Pi)*m_radius;
		mLineCoords[lsize * 12 + 1] = sin(mLeft / 180.0f*Pi)*m_radius* cos(mTop / 180.0f*Pi);
		mLineCoords[lsize * 12 + 2] = -sin(mTop / 180.0f*Pi)*m_radius;
		mLineCoords[lsize * 12 + 3] = cos(mLeft / 180.0f*Pi) * cos(mBottom / 180.0f*Pi)*m_radius;
		mLineCoords[lsize * 12 + 3 + 1] = sin(mLeft / 180.0f*Pi)*m_radius* cos(mBottom / 180.0f*Pi);
		mLineCoords[lsize * 12 + 3 + 2] = -sin(mBottom / 180.0f*Pi)*m_radius;

		mLineCoords[lsize * 12 + 3 + 3] = cos((lsize * 3 + mLeft) / 180.0f*Pi) * cos(mTop / 180.0f*Pi)*m_radius;
		mLineCoords[lsize * 12 + 3 + 4] = sin((lsize * 3 + mLeft) / 180.0f*Pi)*m_radius* cos(mTop / 180.0f*Pi);
		mLineCoords[lsize * 12 + 3 + 5] = -sin(mTop / 180.0f*Pi)*m_radius;
		mLineCoords[lsize * 12 + 6 + 3] = cos((lsize * 3 + mLeft) / 180.0f*Pi) * cos(mBottom / 180.0f*Pi)*m_radius;
		mLineCoords[lsize * 12 + 6 + 4] = sin((lsize * 3 + mLeft) / 180.0f*Pi)*m_radius* cos(mBottom / 180.0f*Pi);
		mLineCoords[lsize * 12 + 6 + 5] = -sin(mBottom / 180.0f*Pi)*m_radius;
	}
	else
	{
		mLineCoords[lsize * 12] = cos(mLeft / 180.0f*Pi) * cos(mTop / 180.0f*Pi)*m_radius;
		mLineCoords[lsize * 12 + 1] = sin(mLeft / 180.0f*Pi)*m_radius* cos(mTop / 180.0f*Pi);
		mLineCoords[lsize * 12 + 2] = -sin(mTop / 180.0f*Pi)*m_radius;
		mLineCoords[lsize * 12 + 3] = cos((lsize * 3 + mLeft) / 180.0f*Pi) * cos(mBottom / 180.0f*Pi)*m_radius;
		mLineCoords[lsize * 12 + 3 + 1] = sin((lsize * 3 + mLeft) / 180.0f*Pi)*m_radius* cos(mBottom / 180.0f*Pi);
		mLineCoords[lsize * 12 + 3 + 2] = -sin(mBottom / 180.0f*Pi)*m_radius;

		mLineCoords[lsize * 12 + 3 + 3] = cos((lsize * 3 + mLeft) / 180.0f*Pi) * cos(mTop / 180.0f*Pi)*m_radius;
		mLineCoords[lsize * 12 + 3 + 4] = sin((lsize * 3 + mLeft) / 180.0f*Pi)*m_radius* cos(mTop / 180.0f*Pi);
		mLineCoords[lsize * 12 + 3 + 5] = -sin(mTop / 180.0f*Pi)*m_radius;
		mLineCoords[lsize * 12 + 6 + 3] = cos(mLeft / 180.0f*Pi) * cos(mBottom / 180.0f*Pi)*m_radius;
		mLineCoords[lsize * 12 + 6 + 4] = sin(mLeft / 180.0f*Pi)*m_radius* cos(mBottom / 180.0f*Pi);
		mLineCoords[lsize * 12 + 6 + 5] = -sin(mBottom / 180.0f*Pi)*m_radius;
	}

//	float lineCoords[] = {
//		-0.5f*m_aspect,-0.5f,1.0f,
//		0.5f*m_aspect,-0.5f,1.0f,
//		0.5f*m_aspect,-0.5f,1.0f,
//		0.5f*m_aspect,0.5f,1.0f,
//		-0.5f*m_aspect,0.5f,1.0f,
//		0.5f*m_aspect,0.5f,1.0f,
//		-0.5f*m_aspect,0.5f,1.0f,
//		-0.5f*m_aspect,-0.5f,1.0f,
//	};
//	memcpy(mLineCoords, lineCoords, 24 * sizeof(float));

	if(mLineBuffer)
		glDeleteBuffers(1,&mLineBuffer);

	glGenBuffers(1, &mLineBuffer);
	LOGDEBUG("1--glGenBuffers:%d",mLineBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, mLineBuffer);
	glBufferData(GL_ARRAY_BUFFER, mLinebufferSize * sizeof(GLfloat), mLineCoords, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

void SixFrameDraw::UpdatePosition(float tbstart, float tbend, float lrstart, float lrend)
{
	mLeft = lrstart;
	mRigth = lrend;
	mTop = tbstart;
	mBottom = tbend;
	mIsUpdatePosition = true;
}
