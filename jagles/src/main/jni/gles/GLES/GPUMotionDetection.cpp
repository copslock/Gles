//
// Created by LiHong on 16/6/9.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "GPUMotionDetection.h"
#include "GLESMath.h"


const char *GPUMotionDetectionSeg = "uniform mat4 projection;\n"
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
	const char *GPUMotionDetectionFrag = "precision mediump float;\n"
	"\n"
	"varying vec2 vTextureCoord;\n"
	"uniform sampler2D sTexture;\n"
	"uniform sampler2D sPrevTexture;\n"
	"uniform vec2 blocksize;\n"
	"\n"
	"void main()\n"
	"{\n"
	"	float dx=0.02;\n"
	"   float dy=0.02;\n"
	"   vec2 uv=vTextureCoord.xy;\n"
	"   vec2 coord = vec2(dx*floor(uv.x/dx),dy*floor(uv.y / dy));\n"
	"   vec4 lcolor = texture2D(sTexture, coord);\n"
	"   float lgray = lcolor.r*0.299+lcolor.g*0.587+lcolor.b*0.114;\n"
	"   vec4 lcolor1 = texture2D(sPrevTexture, coord);\n"
	"   float lgray1 = lcolor1.r*0.299+lcolor1.g*0.587+lcolor1.b*0.114;\n"
	"   float lgray2 = abs(lgray-lgray1);\n"
	"   vec4 lcolor2 = texture2D(sTexture,vTextureCoord);"
	"   if(lgray2>0.07)\n"
	"      lgray2=1.0;\n"
	"   else\n"
	"      lgray2=0.0;\n"
	"   gl_FragColor=vec4(lgray2,lgray2,lgray2,1.0);"
//			"   if(lgray2>0.07)\n"
//			"      gl_FragColor=vec4(1.0,.0,.0,1.0);\n"
//			"   else\n"
//			"      gl_FragColor=lcolor2;\n"
	"}";

const char *YUV_GPUMotionDetectionFrag = "precision mediump float;\n"
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


GPUMotionDetection::GPUMotionDetection(float radius, float aspect,void *mgn) : m_radius(radius)
{
	_Manager = mgn;
	m_aspect = aspect;
	ParametricInterval interval = { 1, vec2(1,1), vec2(2, 2) };
	SetInterval(interval);
#ifdef __USE_RGB32__
	setupProgram(GPUMotionDetectionSeg, GPUMotionDetectionFrag);
#else
	setupProgram(GPUMotionDetectionSeg, YUV_GPUMotionDetectionFrag, true);
#endif
	setupProjection();
	SetupBuffer();
	_Zoom = -2.74;
	mLastTexID = 0;
	mCurrentTexID = 0;
	mScreenMode = SCRN_MOTIONDETECTION;
}

vec3 GPUMotionDetection::Evaluate(const vec2& domain)
{
	float u = domain.x, v = domain.y;
	float x = (m_radius*u - m_radius / 2 )*m_aspect;
	float y = m_radius*v - m_radius / 2 ;
	float z = 1;
	return vec3(x, y, z);
}

vec2 GPUMotionDetection::EvaluateCoord(const vec2& domain)
{
	float u = domain.x, v = domain.y;
	float x = u;
	float y = 1 - v;
	return vec2(x, y);
}

void GPUMotionDetection::setupProgram(const char *segsrc, const char *fragsrc, bool yuv)
{
	ParametricSurface::setupProgram(segsrc, fragsrc, yuv);
}

void GPUMotionDetection::setupProgram(const char *segsrc, const char *fragsrc)
{
	ParametricSurface::setupProgram(segsrc, fragsrc);
	_Samplers[0] = glGetUniformLocation(_programHandle, "sTexture");
	_Samplers[1] = glGetUniformLocation(_programHandle, "sPrevTexture");
}

void GPUMotionDetection::DrawSelf(int scnindex)
{
	mNeedDraw = false;
	if (!Visible)
		return;

	//printf("NeedDraw.....%d\n",mNeedDraw);
	glUseProgram(_programHandle);

	updateSurfaceTransform();

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(_positionSlot, 3, GL_FLOAT, GL_FALSE, vertexSize * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(_positionSlot);

	//printf("glEnableVertexAttribArray:%d\n",glGetError());

	glBindBuffer(GL_ARRAY_BUFFER, TextureCoordBuffer);
	glVertexAttribPointer(_coordSlot, 2, GL_FLOAT, GL_FALSE, TextureCoordSize * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(_coordSlot);


	if (mIsYUV)
	{
		for (int i = 0; i < 2; ++i) {
			//			printf("texture......%d\n",_yuvtexid[i]);
			glActiveTexture(GL_TEXTURE0 + i);
			if (_yuvtexid[0])
				glBindTexture(GL_TEXTURE_2D, _yuvtexid[i]);
			else
				glBindTexture(GL_TEXTURE_2D, _yuvNoVideoTexID[i]);
			glUniform1i(_uniformSamplers[i], i);
		}
	}
	else {
		
		if (mCurrentTexID&&mLastTexID)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mCurrentTexID);
			glUniform1i(_Samplers[0], 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mLastTexID);
			glUniform1i(_Samplers[1], 1);
		}
		else
		{
			glDisableVertexAttribArray(_positionSlot);
			glDisableVertexAttribArray(_coordSlot);
			return;
		}

	}
	glDrawArrays(GL_TRIANGLES, 0, triangleIndexCount);
	//	glDrawArrays(GL_LINES, 0, triangleIndexCount);


	glBindTexture(GL_TEXTURE_2D, 0);
	//glDrawArrays(GL_LINES, 0, triangleIndexCount);
	glDisableVertexAttribArray(_positionSlot);
	glDisableVertexAttribArray(_coordSlot);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool GPUMotionDetection::LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex)
{
	if (mLastTexID)
		glDeleteTextures(1, &mLastTexID);
	if (mCurrentTexID)
		mLastTexID = mCurrentTexID;
	_texwidth = w;
	_texheight = h;
	GLuint texture_object_id = 0;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture_object_id);
//	LOGDEBUG("0--glGenTextures:%d",texture_object_id);
	//assert(texture_object_id != 0);

	glBindTexture(GL_TEXTURE_2D, texture_object_id);
	glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, type, GL_UNSIGNED_BYTE, pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	mCurrentTexID = texture_object_id;
		//printf("textid...........:%d %d\n",_texid,scnindex);
	mNeedDraw = true;
	return true;
}
