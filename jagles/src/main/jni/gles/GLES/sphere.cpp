//
// Created by LiHong on 16/4/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "sphere.h"
#ifdef GLESV2
#include "ParametricManagerV2.h"
#else
#include "ParametricManager.h"
typedef ParametricManager ParametricManagerV2;
#endif
#include "../Utils/mediabuffer.h"
#include "../GLES/SelectedBox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#ifdef __ANDROID_API__
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
//#endif


const char *sphereSeg = "#define PI 3.1415926535897932384626433832795\n"
"#define VERTEX_TYPE_HEMISPHERE 0\n"
"#define VERTEX_TYPE_PANORAMA 1\n"
"#define VERTEX_TYPE_CYLINDER 2\n"
"#define VERTEX_TYPE_EXPAND 3\n"
"#define VERTEX_TYPE_NORMAL 4\n"
"#define VERTEX_TYPE_SPHERE 5\n"
"#define VERTEX_TYPE_CONE 6\n"
"uniform mat4 projection;\n"
"uniform mat4 modelView;\n"
"uniform mat4 changeprojection;\n"
"uniform mat4 changemodelView;\n"
"attribute vec4 vPosition;\n"
"attribute vec4 vChangePosition;\n"
"attribute vec2 aTexCoor;\n"
"uniform int aMode;\n"
"uniform int aChangeMode;\n"
"uniform float texwidth;\n"
"uniform float texheight;\n"
"uniform float aRotate;\n"
"uniform float aDiameter;\n"
"uniform float aAspect;\n"
"uniform float aViewportAspect;\n"
"uniform float aChangeDiameter;"
"uniform float aChangeRotate;"
"uniform int aChangeAnimation;\n"
"uniform float aChangeStep;\n"
"uniform float aChangeStepCount;\n"
"\n"
"varying vec2 vTextureCoord;\n"
"varying float FRotate;\n"
//        "varying vec2 vTextCoord;\n"
"\n"
"vec4 GetPosition(mat4 proj,mat4 mv,vec4 pos,int mode,float diameter)\n"
"{\n"
"    vec4 aPos=pos;\n"
"    if(mode==VERTEX_TYPE_CYLINDER||mode==VERTEX_TYPE_CONE)\n"
"    {\n"
"       float y;"
"       float x=cos((aPos.x)/diameter-0.5*PI+(diameter-1.0)/diameter*PI)*0.8*diameter;\n"
"       if(aPos.y>250.0)\n"
"           y=-25.0/180.0*PI*1.6;\n"
"       else if(aPos.y<200.0)\n"
"           y=25.0/180.0*PI*1.6;\n"
"       else\n"
"            y=(225.0-aPos.y)/180.0*PI*1.6;\n"
"       float z=sin((aPos.x)/diameter-0.5*PI+(diameter-1.0)/diameter*PI)*0.8*diameter-(sqrt(diameter)-1.0/diameter);\n"
"       aPos = vec4(x,y,z,1.0);\n"
"    }\n"
"    else\n"
"    if(mode!=VERTEX_TYPE_HEMISPHERE&&mode!=VERTEX_TYPE_SPHERE)\n"
"    {\n"
"       float vcut=texwidth/texheight;\n"
"       if(mode==VERTEX_TYPE_EXPAND)\n"
"           aPos=vec4(aPos.x*aAspect,aPos.y*vcut,aPos.z,1.0);\n"
"       else\n"
"           aPos=vec4(aPos.x*aAspect,aPos.y,aPos.z,1.0);\n"
"    }\n"
"    else\n"
"           aPos=vec4(aPos.x,aPos.y,aPos.z,1.0);\n"
"    return proj * mv * aPos;\n"
"}\n"
"vec2 GetCoord(float pRotate)\n"
"{\n"
//"    float sin_factor = sin(-pRotate);\n"
//"    float cos_factor = cos(-pRotate);\n"
//"    vec2 tmpcoor = vec2(aTexCoor.x - 0.5, aTexCoor.y - 0.5) * mat2(cos_factor, sin_factor, -sin_factor, cos_factor);\n"
//"    vec2 tmpcoor=vec2(aTexCoor.x+pRotate/PI,aTexCoor.y-0.5);\n"
"    vec2 tmpcoor=vec2(aTexCoor.x-0.5,aTexCoor.y-0.5);\n"
//"    while(tmpcoor.x>1.0)\n"
//"      tmpcoor.x=tmpcoor.x-1.0;\n"
//"    tmpcoor.x=tmpcoor.x-0.5;\n"
"    if(texheight>0.0)\n"
"    {\n"
"       float vcut=texwidth/texheight;\n"
"       if(vcut<1.29||aMode==VERTEX_TYPE_NORMAL||aMode==VERTEX_TYPE_CYLINDER)\n"
"          vcut=1.0;\n"
"       if(aMode==VERTEX_TYPE_PANORAMA)\n"
"           return vec2(tmpcoor.x+0.5,tmpcoor.y*5.0/9.0+0.5);\n"
"       return vec2(tmpcoor.x+0.5,tmpcoor.y*vcut+0.5);\n"
"    }\n"
"    else\n"
"       return vec2(tmpcoor.x+0.5,tmpcoor.y+0.5);\n"
"}\n"
"void main(void)\n"
"{\n"
"    vec4 aPos = GetPosition(projection,modelView,vPosition,aMode,aDiameter);\n"
"    FRotate = aRotate;\n"
"    if(aChangeAnimation==1)\n"
"    {\n"
"       vec4 bPos=GetPosition(changeprojection,changemodelView,vChangePosition,aChangeMode,aChangeDiameter)-aPos;\n"
"       bPos=bPos/aChangeStepCount;\n"
"       aPos=aPos+bPos*aChangeStep;\n"
"       float tmp1 = aChangeRotate - aRotate;\n"
"       tmp1=aRotate+tmp1/aChangeStepCount*aChangeStep;\n"
"       vTextureCoord = GetCoord(tmp1);\n"
"    }\n"
"    else\n"
"       vTextureCoord = GetCoord(aRotate);\n"
"    if(aMode==VERTEX_TYPE_CYLINDER)\n"
"       vTextureCoord = vec2(1.0-vTextureCoord.x,vTextureCoord.y);\n"
"	 else if(aMode==VERTEX_TYPE_EXPAND||aMode==VERTEX_TYPE_PANORAMA)" 
"       vTextureCoord = vec2(vTextureCoord.x,1.0-vTextureCoord.y);\n"
"    gl_Position = aPos;\n"
"}";
const char *sphereFrag =
"#extension GL_OES_EGL_image_external : require\n"
"#define VERTEX_TYPE_HEMISPHERE 0\n"
"#define VERTEX_TYPE_PANORAMA 1\n"
"#define VERTEX_TYPE_CYLINDER 2\n"
"#define VERTEX_TYPE_EXPAND 3\n"
"#define VERTEX_TYPE_NORMAL 4\n"
"#define VERTEX_TYPE_SPHERE 5\n"
"#define VERTEX_TYPE_CONE 6\n"
"precision mediump float;\n"
"\n"
"#define PI 3.1415926535897932384626433832795\n"
"varying vec2 vTextureCoord;\n"
"uniform int vMode;\n"
"varying float FRotate;"
"uniform vec2 uSolution;\n"
"uniform sampler2D sTexture;\n"
" struct MASKFRAME {\n"
"   float left;\n"
"   float right;\n"
"   float top;\n"
"   float bottom;\n"
" };\n"
" uniform MASKFRAME maskframe[8];\n"
"\n"
"void main()\n"
"{\n"
"    int i;\n"
"    int found=0;\n"
"    if(vTextureCoord.y>1.0||vTextureCoord.y<0.0)\n"
"       gl_FragColor=vec4(0.0,0.0,0.0,0.0);\n"
"    else\n"
"    {\n"
"       vec2 coor;\n"
"       coor=vec2(vTextureCoord.x+FRotate,1.0-vTextureCoord.y);\n"
"       coor.x=coor.x-floor(coor.x);\n"
"       vec4 acolor=texture2D(sTexture,coor);\n"
"           gl_FragColor=acolor;\n"
"    }\n"
"}";

const char *YUV_sphereFrag = "precision mediump float;\n"
"\n"
"varying vec2 vTextureCoord;\n"
"varying vec2 vTextCoord;\n"
" uniform sampler2D s_texture_y;\n"
" uniform sampler2D s_texture_cbcr;\n"
" struct MASKFRAME {\n"
"   float left;\n"
"   float right;\n"
"   float top;\n"
"   float bottom;\n"
" };\n"
" uniform MASKFRAME maskframe[8];\n"
"\n"
"void main()\n"
"{\n"
"    if(vTextureCoord.y>1.0||vTextureCoord.y<0.0)\n"
"       gl_FragColor=vec4(0.0,0.0,0.0,0.0);\n"
"    else\n"
"    {\n"
"       highp float y = texture2D(s_texture_y, vTextureCoord).r;\n"
"       highp float u = texture2D(s_texture_cbcr, vTextureCoord).a - 0.5;\n"
"       highp float v = texture2D(s_texture_cbcr, vTextureCoord).r - 0.5;\n"
"       \n"
"       highp float r = y +             1.402 * v;\n"
"       highp float g = y - 0.344 * u - 0.714 * v;\n"
"       highp float b = y + 1.772 * u;\n"
"       \n"
"       gl_FragColor = vec4(r,g,b,1.0);     \n"
"    }\n"
"}";


//extern const char *uniformstr[4];

const int _IncludeMode[3] = { VERTEX_TYPE_CYLINDER,VERTEX_TYPE_PANORAMA };

#define SPHERE_MODE_COUNT 3;

#define VertexStep 3

sphere::sphere(float radius, float aspect, void *mgn) : m_radius(radius)
{
	isUseDirectTexture = false;
	direct_width = 0;
	direct_height = 0;
	m_aspect = aspect;
	_Manager = mgn;
	mViewportAspect = 1.0;
	mShowGrid = false;
	for (int i = 0; i < VERTEX_TYPE_COUNT; ++i) {
		mVertexVBOBuffer[i] = 0;
		mTextureCoordBuffer[i] = 0;
	}
	ParametricInterval interval = { VertexStep, vec2(VertexStep,180 + VertexStep), vec2(360 + VertexStep, 270 + VertexStep) };
	SetInterval(interval);
#ifdef __USE_RGB32__
	setupProgram(sphereSeg, sphereFrag);
#else
	setupProgram(sphereSeg, YUV_sphereFrag, true);
#endif
	//eTB_GLSL__print_uniforms(_programHandle);
	this->setupProjection();
	SetupBuffer();
	BaseScreen::setupLineBuffer();
	mChangeType = VERTEX_TYPE_HEMISPHERE;
	mChangeAnimation = false;
	mChangeStep = 0;
	mChangeStepCount = 100;
	directTextureBuffer = NULL;
	//setRotate(vec3(30,0,0),false,0);
	mScreenMode = SCRN_VECTOROBJECT;
	mDrawSelected = new SelectedBox(aspect);
}

sphere::~sphere() {
	for (int i = 0; i<VERTEX_TYPE_COUNT; i++)
	{
		if (mVertexVBOBuffer[i] != 0) {
			glDeleteBuffers(1, &mVertexVBOBuffer[i]);
			LOGDEBUG("11---glDeleteBuffers:%d %p", mVertexVBOBuffer[i], this);
			mVertexVBOBuffer[i] = 0;
		}
			if (mTextureCoordBuffer[i] != 0) {
				glDeleteBuffers(1, &mTextureCoordBuffer[i]);
				LOGDEBUG("12---glDeleteBuffers:%d %p",mTextureCoordBuffer[i],this);
				mTextureCoordBuffer[i] = 0;
			}
	}
	delete mDrawSelected;
}

void sphere::ResetPosition() {
	ParametricSurface::ResetPosition();
	//setRotate(vec3(-30,0,0),false,0);
	_Zoom = -3;
}


void sphere::SetupBuffer() {

	for (int i = 0; i < VERTEX_TYPE_COUNT; ++i) {
		mVertexType = i;
		int lvs = GetVertexSize();
		int vBufSize = GetVertexCount() * lvs;
		int vCount = GetVertexCount();
		GLfloat *vbuf = (GLfloat *)malloc(vBufSize * sizeof(GLfloat));//new GLfloat[vBufSize];

		int CoordSize = GetCoordSize();
		int vCoorBufSize = GetCoordCount() * CoordSize;
		GLfloat *vcoordbuf = (GLfloat *)malloc(vCoorBufSize * sizeof(GLfloat));//new GLfloat[vCoorBufSize];

		GenerateVertices(vbuf, vcoordbuf);

		GLuint lvb = 0;
		//printf("\n\n");
		glGenBuffers(1, &lvb);
		LOGDEBUG("11--glGenBuffers:%d %p",lvb,this);

		//printf("gl error number:%d\n\n",glGetError());
		glBindBuffer(GL_ARRAY_BUFFER, lvb);
		glBufferData(GL_ARRAY_BUFFER, vBufSize * sizeof(GLfloat), vbuf, GL_STATIC_DRAW);
		//printf("####%d######%d\n",glGetError(),vBufSize);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//printf("set vexture buffer:%d\n",glGetError());
		GLuint CoordBuffer = 0;
		glGenBuffers(1, &CoordBuffer);
		LOGDEBUG("12--glGenBuffers:%d %p",CoordBuffer,this);

		glBindBuffer(GL_ARRAY_BUFFER, CoordBuffer);
		glBufferData(GL_ARRAY_BUFFER, vCoorBufSize * sizeof(GLfloat), vcoordbuf, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//printf("vBufSize-------------------%d %d %d %d\n",vBufSize,vCoorBufSize,lvb,CoordBuffer);
		//printf("set Coord buffer:%d i:%d\n",glGetError(),i);

		//delete[] vbuf;
		//delete[] vcoordbuf;
		free(vbuf);
		free(vcoordbuf);

		if(mVertexVBOBuffer[i])
			glDeleteBuffers(1,&mVertexVBOBuffer[i]);
		mVertexVBOBuffer[i] = lvb;
		mVertexSize[i] = lvs;
		mTriangleIndexCount[i] = vCount;

		mTextureCoordSize[i] = CoordSize;
		if(mTextureCoordBuffer[i])
			glDeleteBuffers(1,&mTextureCoordBuffer[i]);
		mTextureCoordBuffer[i] = CoordBuffer;
	}
	mVertexType = VERTEX_TYPE_CYLINDER;
	//printf("vCount:%d\n",vCount);
}


void sphere::setupProjection() {
	ParametricManagerV2 *mgn = (ParametricManagerV2 *)_Manager;
	float ViewAngle;
	if (mgn)
		ViewAngle = mgn->GetViewAngle();
	else
		ViewAngle = 60.0;
	ksMatrixLoadIdentity((KSMatrix4 *)&_projectionMatrix);
	if (mVertexType != VERTEX_TYPE_CYLINDER) {
		_Zoom = -3;
		ksPerspective((KSMatrix4 *)&_projectionMatrix, ViewAngle, m_aspect, 0.0001f, 1200.0f);
		//printf("aaaaaaa\n");
	}
	else {
		_Zoom = 1;
		//ksPerspective((KSMatrix4 *)&_projectionMatrix, 30.0, m_aspect, 0.0001f, 11.0f);
		ksOrtho(&_projectionMatrix, -m_aspect, m_aspect, -1, 1, 0.0001f, 1200);
		//printf("bbbbbbbb");
	}

	glUseProgram(_programHandle);
	// Load projection matrix
	glUniformMatrix4fv(_projectionSlot, 1, GL_FALSE, (GLfloat *)&_projectionMatrix.m[0][0]);
	//printf("glUniformMatrix4fv0:%d\n",glGetError());
	ksMatrixLoadIdentity(&_rotationMatrix);
	//ksRotate(&_rotationMatrix,180.0f,0,1,0);
	//glEnable(GL_LINE_SMOOTH);
}


vec3 sphere::Evaluate(const vec2& domain)
{
	float u = domain.x, v = domain.y;
	float x = 0;
	float y = 0;
	float z = 0;
	float rx, ry;

	switch (mVertexType) {
	case VERTEX_TYPE_HEMISPHERE:
		//ȫ����ͼģʽ
		x = m_radius * cos(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
		y = m_radius * sin(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
		z = m_radius * sin(v / 180.0f * Pi);
		break;
	case VERTEX_TYPE_PANORAMA:
		x = (u - 180.0f) / 360.0f * 2;
		y = (v - 225.0f) / 90.0f * 2;
		z = 1.265f;
		break;
	case VERTEX_TYPE_CYLINDER:
            
		x = (u / 180.0f*Pi);
		y = v;
		z = 1;
            break;
	case VERTEX_TYPE_EXPAND:
		//�Ľ�չ��ģʽ
		x = (u - 180.0f) / 360.0f * 2;
		y = (v - 225.0f) / 90.0f * 2;
		z = 1.265f;
		break;
	case VERTEX_TYPE_NORMAL:
		//��ͨģʽ
		break;
	case VERTEX_TYPE_SPHERE:
		x = cos(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
		y = sin(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
		z = sin((v - 180) / 180.0f * Pi);
		//return vec3(x,y,z);
		if (z == 0.0)
			z = 0.0000001;
		rx = 2 * atanf(x / z) / Pi;
		ry = -sinf(u / 180.0f * Pi) * sinf((270 - v) / 180.0 * Pi);
		x = rx;
		y = ry;
		z = 1.265;
		break;
	}
	return vec3(x, y, z);
}

vec2 sphere::EvaluateCoord(const vec2& domain)
{
	float u = domain.x, v = domain.y;
	vec2 vTextureCoord = vec2(0, 0);
	float x, y;
	if (mVertexType == VERTEX_TYPE_EXPAND||mVertexType == VERTEX_TYPE_CYLINDER|| mVertexType == VERTEX_TYPE_PANORAMA)
	{
		vTextureCoord = vec2(u / 360.0f, (270.0f - v) / 90.0f);
	}
	else
	{
        x = m_radius * cos(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
        y = m_radius * sin(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
		float z = m_radius * sin(v / 180.0f * Pi);
		vec3 npos = vec3(x, y, z);
		npos.Normalize();
		vec2 px = vec2(npos.x, npos.y);
		//return px;
		float sinthita = px.Length();
		px.Normalize();
		//printf("px.x:%f,px.y:%f\n",px.x,px.y);
		if (sinthita >= 1.0f)
			sinthita = 1.0f;
		float thita = asinf(sinthita);
		//printf("thita:%f sinthita:%f\n",thita,sinthita);
		float py = 2.0f * sinf(thita * 0.5f);
		vTextureCoord = px * py * (float)((1.0 / powf(2.0, 0.5)) * 0.5);
		//    vTextureCoord.y = vTextureCoord.y*1.35;
		vTextureCoord.x = vTextureCoord.x + 0.5f;
		vTextureCoord.y = vTextureCoord.y + 0.5f;
		vTextureCoord.y = (1.0f - vTextureCoord.y);
		//printf("x:%f,y:%f\n",vTextureCoord.x,vTextureCoord.y);
	}
	return vTextureCoord;
}

void sphere::DrawSelf(int scnindex)
{
#ifdef __ANDROID__
	if ((_texwidth != direct_width || _texheight != direct_height) && isUseDirectTexture) {
		if (_texid)
			glDeleteTextures(1, &_texid);
		_texid = 0;
		_texwidth = direct_width;
		_texheight = direct_height;
	}
	if (!_texid && isUseDirectTexture)
		CreateDirectTexture();
#endif //__ANDROID__
	mNeedDraw = false;
	if (!Visible)
		return;

	//printf("NeedDraw.....%d\n",mNeedDraw);
	glUseProgram(_programHandle);

	ParametricManagerV2 *mgn = (ParametricManagerV2 *)_Manager;
	float tw = mgn->GetTextureWidth();
	float th = mgn->GetTextureHeight();
	glUniform1f(mSegTexWidthHandle, tw);
	glUniform1f(mSegTexHeightHandle, th);

	float viewangle;
	if (mgn)
		viewangle = mgn->GetViewAngle();
	else
		viewangle = 60.0;
	if (mVertexType == VERTEX_TYPE_HEMISPHERE || mVertexType == VERTEX_TYPE_SPHERE)
		updateSurfaceTransform(&_projectionMatrix, &_modelViewMatrix, &_rotationMatrix, _Zoom*tanf(30.0 / 180.0f*Pi) / tanf(viewangle / 2 / 180.0f*Pi), vec3(mPosition.x, mPosition.y, mPosition.z*tanf(30.0 / 180.0*Pi) / tanf(viewangle / 2 / 180.0f*Pi)), mScale, mRotate, _projectionSlot, _modelViewSlot, mVertexType);
	else
		updateSurfaceTransform(&_projectionMatrix, &_modelViewMatrix, &_rotationMatrix, _Zoom, mPosition, mScale, mRotate, _projectionSlot, _modelViewSlot, mVertexType);

	glUniform1i(mSegModeHandle, mVertexType);
    glUniform1i(mFragModeHandle, mVertexType);

	glUniform1f(mSegRotateHandle, mTexPosition.x);
    //glUniform1f(mSegRotateHandle, 0.8);
	glUniform1f(mSegDiameterHandle, mTexScale.x);
	glUniform1f(mSegAspectHandle, m_aspect);
	glUniform1f(mSegViewportAspectHandle, mViewportAspect);
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT,vp);
    float vw = vp[2];
    float vh = vp[3];
    glUniform2f(mFragSolutionHandle,vw,vh);

	glUniform1i(mSegChangeAnimationHandle, mChangeAnimation ? 1 : 0);
	if (mChangeAnimation)
	{
		glUniform1f(mSegChangeRotateHandle, nTexPosition.x);
		glUniform1f(mSegChangeDiameterHandle, nTexScale.x);
		glUniform1f(mSegChangeStepCountHandle, mChangeStepCount);
		glUniform1f(mSegChangeStepHandle, mChangeStep);
		glUniform1i(mSegChangeModeHandle, mChangeType);
		float lzoom;
		if (mChangeType != VERTEX_TYPE_CYLINDER) {
			lzoom = -3;
			//            ksMatrixLoadIdentity((KSMatrix4 *) &mChangeprojectionMatrix);
			//            ksPerspective((KSMatrix4 *) &mChangeprojectionMatrix, 60.0 / mScale.x, m_aspect, 0.0001f, 1200.0f);
			//            glUniformMatrix4fv(mSegChangeProjHandle, 1, GL_FALSE, (GLfloat *) &mChangeprojectionMatrix.m[0][0]);
		}
		else {
			lzoom = 1;
			//            ksMatrixLoadIdentity((KSMatrix4 *) &mChangeprojectionMatrix);
			//            ksOrtho(&mChangeprojectionMatrix,-m_aspect,m_aspect,-1,1,0.0001f,1200);
			//            glUniformMatrix4fv(mSegChangeProjHandle, 1, GL_FALSE, (GLfloat *) &mChangeprojectionMatrix.m[0][0]);
		}
		//        ksMatrixLoadIdentity(&mChangemodelViewMatrix);
		//        ksTranslate(&mChangemodelViewMatrix, 0.0, 0.0, lzoom);
		//        glUniformMatrix4fv(mSegChangeModelHandle, 1, GL_FALSE, (GLfloat *) &mChangemodelViewMatrix.m[0][0]);
		updateSurfaceTransform(&mChangeprojectionMatrix, &mChangemodelViewMatrix, &mChangeRoateMatrix, lzoom, nPosition, nScale, nRotate, mSegChangeProjHandle, mSegChangeModelHandle, mChangeType);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexVBOBuffer[mChangeType]);
		glVertexAttribPointer(mSegChangePositionHandle, 3, GL_FLOAT, GL_FALSE, mVertexSize[mChangeType] * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(mSegChangePositionHandle);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	//for(int i=0;i<8;i++)
	//{
	//    for(int j=0;j<4;j++) {
	//        glUniform1f(mFragMaskFrameHandle[i].handle[j],mFragMaskFrame[i].value[j]);
	//    }
	//}
	//MB_INFO("texwidth:%f texheight:%f\n",tw,th);

	glBindBuffer(GL_ARRAY_BUFFER, mVertexVBOBuffer[mVertexType]);
	glVertexAttribPointer(_positionSlot, 3, GL_FLOAT, GL_FALSE, mVertexSize[mVertexType] * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(_positionSlot);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//printf("glEnableVertexAttribArray:%d\n",glGetError());

	glBindBuffer(GL_ARRAY_BUFFER, mTextureCoordBuffer[mVertexType]);
	glVertexAttribPointer(_coordSlot, 2, GL_FLOAT, GL_FALSE, mTextureCoordSize[mVertexType] * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(_coordSlot);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


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
		glActiveTexture(GL_TEXTURE0);
		if (_texid)
			glBindTexture(GL_TEXTURE_2D, _texid);
		else
			glBindTexture(GL_TEXTURE_2D, _NoVideoTexID);
		//glUniform1i(mFragTexture,0);
		//        glActiveTexture(GL_TEXTURE1);
		//        glBindTexture(GL_TEXTURE_2D, mTexText);
		//        glUniform1i(mFragTexText,1);
	}



	glDrawArrays(GL_TRIANGLES, 0, mTriangleIndexCount[mVertexType]);
	//printf("glDrawArrays............%d\n",glGetError());
	//printf("mTriangleIndexCount[%d] ---- %d\n",mVertexType,mTriangleIndexCount[mVertexType]);
	//glDrawArrays(GL_LINES, 0, triangleIndexCount);


	glBindTexture(GL_TEXTURE_2D, 0);
	if (mShowGrid)
		glDrawArrays(GL_LINES, 0, mTriangleIndexCount[mVertexType]);
	glDisableVertexAttribArray(_positionSlot);
	glDisableVertexAttribArray(_coordSlot);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (scnindex == 0x7fffffff) {
		mDrawSelected->DrawSelf(0);
	}
	mNeedDraw = false;
}

void sphere::SwitchMode(int mode) {
	if (mVertexType != mode) {
		//printf("Switchmode .................... start\n");
		mVertexType = mode;
		this->setupProjection();
		//printf("Switchmode ....................\n");
	}
}

void sphere::setupProgram(const char *segsrc, const char *fragsrc)
{
	ParametricSurface::setupProgram(segsrc, fragsrc);
	//eTB_GLSL__print_uniforms(_programHandle);
	mSegModeHandle = glGetUniformLocation(_programHandle, "aMode");
	mSegRotateHandle = glGetUniformLocation(_programHandle, "aRotate");
	mSegDiameterHandle = glGetUniformLocation(_programHandle, "aDiameter");
	mSegTexWidthHandle = glGetUniformLocation(_programHandle, "texwidth");
	mSegTexHeightHandle = glGetUniformLocation(_programHandle, "texheight");
	mSegAspectHandle = glGetUniformLocation(_programHandle, "aAspect");
	mSegChangeAnimationHandle = glGetUniformLocation(_programHandle, "aChangeAnimation");
	mSegChangeStepHandle = glGetUniformLocation(_programHandle, "aChangeStep");
	mSegChangePositionHandle = glGetAttribLocation(_programHandle, "vChangePosition");
	mSegChangeStepCountHandle = glGetUniformLocation(_programHandle, "aChangeStepCount");
	mSegChangeProjHandle = glGetUniformLocation(_programHandle, "changeprojection");
	mSegChangeModelHandle = glGetUniformLocation(_programHandle, "changemodelView");
	mSegChangeModeHandle = glGetUniformLocation(_programHandle, "aChangeMode");
	mFragTexture = glGetUniformLocation(_programHandle, "sTexture");
	mFragTexText = glGetUniformLocation(_programHandle, "sTexText");
	mSegChangeDiameterHandle = glGetUniformLocation(_programHandle, "aChangeDiameter");
	mSegChangeRotateHandle = glGetUniformLocation(_programHandle, "aChangeRotate");
	mSegViewportAspectHandle = glGetUniformLocation(_programHandle, "aViewportAspect");
    mFragSolutionHandle = glGetUniformLocation(_programHandle,"uSolution");
    mFragModeHandle = glGetUniformLocation(_programHandle, "vMode");
}

void sphere::setupProgram(const char *segsrc, const char *fragsrc, bool yuv)
{
	ParametricSurface::setupProgram(segsrc, fragsrc, yuv);
	mSegModeHandle = glGetUniformLocation(_programHandle, "aMode");
	mSegRotateHandle = glGetUniformLocation(_programHandle, "aRotate");
	mSegDiameterHandle = glGetUniformLocation(_programHandle, "aDiameter");
	mSegTexWidthHandle = glGetUniformLocation(_programHandle, "texwidth");
	mSegTexHeightHandle = glGetUniformLocation(_programHandle, "texheight");
	mSegAspectHandle = glGetUniformLocation(_programHandle, "aAspect");
	mSegChangeAnimationHandle = glGetUniformLocation(_programHandle, "aChangeAnimation");
	mSegChangeStepHandle = glGetUniformLocation(_programHandle, "aChangeStep");
	mSegChangePositionHandle = glGetAttribLocation(_programHandle, "vChangePosition");
	mSegChangeStepCountHandle = glGetUniformLocation(_programHandle, "aChangeStepCount");
	mSegChangeProjHandle = glGetUniformLocation(_programHandle, "changeprojection");
	mSegChangeModelHandle = glGetUniformLocation(_programHandle, "changemodelView");
	mSegChangeModeHandle = glGetUniformLocation(_programHandle, "aChangeMode");
	mSegChangeDiameterHandle = glGetUniformLocation(_programHandle, "aChangeDiameter");
	mSegChangeRotateHandle = glGetUniformLocation(_programHandle, "aChangeRotate");
	mSegViewportAspectHandle = glGetUniformLocation(_programHandle, "aViewportAspect");
    mFragSolutionHandle = glGetUniformLocation(_programHandle,"uSolution");
    mFragModeHandle = glGetUniformLocation(_programHandle, "vMode");
}

void sphere::updateSurfaceTransform(KSMatrix4 *_proj, KSMatrix4 *_model, KSMatrix4 *_rotate, float zoom, vec3 pPos, vec3 pScale, vec3 pRotate, GLuint projslot, GLuint modelslot, int atype) {
	ksMatrixLoadIdentity(_model);
	ksTranslate(_model, 0.0, 0.0, zoom);

	ksMatrixLoadIdentity(_rotate);
	KSMatrix4 lrotate, ltmprotate;
	ksMatrixLoadIdentity(&lrotate);
	if (pRotate.x) {
		ksRotate(&lrotate, pRotate.x, 1, 0, 0);
	}
	if (pRotate.y) {
		ksRotate(&lrotate, pRotate.y, 0, 1, 0);
	}
	if (pRotate.z) {
		ksRotate(&lrotate, pRotate.z, 0, 0, 1);
	}
	ksMatrixMultiply(_rotate, _rotate, &lrotate);

	ksTranslate(_model, pPos.x, pPos.y, pPos.z);
	//ʹ������һ��ģʽ���Ŵ�
	ParametricManagerV2 *mgn = (ParametricManagerV2 *)_Manager;
	float ViewAngle;
	if (mgn)
		ViewAngle = mgn->GetViewAngle();
	else
		ViewAngle = 60.0;
	if (atype != VERTEX_TYPE_CYLINDER) {
		if (atype == VERTEX_TYPE_HEMISPHERE || atype == VERTEX_TYPE_SPHERE) {
			ksMatrixLoadIdentity((KSMatrix4 *)_proj);
			ksPerspective((KSMatrix4 *)_proj, ViewAngle / pScale.x, m_aspect, 0.0001f, 1200.0f);
			glUniformMatrix4fv(projslot, 1, GL_FALSE, (GLfloat *)&_proj->m[0][0]);
		}
		else
		{
			ksMatrixLoadIdentity((KSMatrix4 *)_proj);
			ksOrtho(_proj, -m_aspect, m_aspect, -1, 1, 0.0001f, 1200);
			glUniformMatrix4fv(projslot, 1, GL_FALSE, (GLfloat *)&_proj->m[0][0]);
			ksScale(_rotate, pScale.x, pScale.y, 1.0);
		}
	}
	else {
		ksMatrixLoadIdentity((KSMatrix4 *)_proj);
		ksOrtho(_proj, -m_aspect, m_aspect, -1, 1, 0.0001f, 1200);
		glUniformMatrix4fv(projslot, 1, GL_FALSE, (GLfloat *)&_proj->m[0][0]);
		ksScale(_rotate, pScale.x, pScale.y, pScale.z);
	}

	///////////

	ksMatrixMultiply(_model, _rotate, _model);
	glUniformMatrix4fv(modelslot, 1, GL_FALSE, (GLfloat *)&_model->m[0][0]);
}

void sphere::updateSurfaceTransform() {
	ksMatrixLoadIdentity(&_modelViewMatrix);
	ksTranslate(&_modelViewMatrix, 0.0, 0.0, _Zoom);

	ksMatrixLoadIdentity(&_rotationMatrix);
	KSMatrix4 lrotate, ltmprotate;
	ksMatrixLoadIdentity(&lrotate);
	if (mRotate.x) {
		ksRotate(&lrotate, mRotate.x, 1, 0, 0);
	}
	if (mRotate.y) {
		ksRotate(&lrotate, mRotate.y, 0, 1, 0);
	}
	if (mRotate.z) {
		ksRotate(&lrotate, mRotate.z, 0, 0, 1);
	}
	ksMatrixMultiply(&_rotationMatrix, &_rotationMatrix, &lrotate);

	ksTranslate(&_modelViewMatrix, mPosition.x, mPosition.y, mPosition.z);
	//ʹ������һ��ģʽ���Ŵ�
	ParametricManagerV2 *mgn = (ParametricManagerV2 *)_Manager;
	float ViewAngle;
	if (mgn)
		ViewAngle = mgn->GetViewAngle();
	else
		ViewAngle = 60.0;
	if (mVertexType != VERTEX_TYPE_CYLINDER) {
		ksMatrixLoadIdentity((KSMatrix4 *)&_projectionMatrix);
		ksPerspective((KSMatrix4 *)&_projectionMatrix, ViewAngle / mScale.x, m_aspect, 0.0001f, 1200.0f);
		glUniformMatrix4fv(_projectionSlot, 1, GL_FALSE, (GLfloat *)&_projectionMatrix.m[0][0]);
	}
	else {
		ksMatrixLoadIdentity((KSMatrix4 *)&_projectionMatrix);
		ksOrtho(&_projectionMatrix, -m_aspect, m_aspect, -1, 1, 0.0001f, 1200);
		glUniformMatrix4fv(_projectionSlot, 1, GL_FALSE, (GLfloat *)&_projectionMatrix.m[0][0]);
		ksScale(&_rotationMatrix, mScale.x, mScale.y, mScale.z);
	}


	///////////

	ksMatrixMultiply(&_modelViewMatrix, &_rotationMatrix, &_modelViewMatrix);
	glUniformMatrix4fv(_modelViewSlot, 1, GL_FALSE, (GLfloat *)&_modelViewMatrix.m[0][0]);
	//printf("glUniformMatrix4fv:%d\n",glGetError());
}


void sphere::SetChangeAnimation(bool Start, int step, int stepcount, int nexttype)
{
	mChangeAnimation = Start;
	mChangeStep = step;
	mChangeType = nexttype;
	mChangeStepCount = stepcount;
}

void sphere::SetChangePSR(vec3 pos, vec3 scale, vec3 rotate, vec3 tpos, vec3 tscale, vec3 trotate)
{
	nPosition = pos;
	nScale = scale;
	nRotate = rotate;
	nTexPosition = tpos;
	nTexScale = tscale;
	nTexRotate = trotate;
}

void sphere::DirectTextureFrameUpdata(int w, int h, void * directBuffer, int index)
{
	isUseDirectTexture = true;
	if (direct_width != w || direct_height != h)
		directTextureBuffer = NULL;
	direct_width = w;
	direct_height = h;
	if (directTextureBuffer == NULL)
		directTextureBuffer = directBuffer;
	mNeedDraw = true;
}

void sphere::CreateDirectTexture()
{
#ifdef __ANDROID__
	JACONNECT_INFO("the direct texture is come to???");
	if (!_texid) {
		GLuint texture_object_id = 0;
		glGenTextures(1, &texture_object_id);
//		LOGDEBUG("8--glGenTextures:%d",texture_object_id);
		glBindTexture(GL_TEXTURE_2D, texture_object_id);
		EGLImageKHR _imageKHR = eglCreateImageKHR(eglGetCurrentDisplay(),
			NULL,
			EGL_NATIVE_BUFFER_ANDROID,
			(EGLClientBuffer)directTextureBuffer,
			NULL);
		glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, (GLeglImageOES)_imageKHR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
		_texid = texture_object_id;
	}
#endif //__ANDROID__
}

void sphere::UpdateMaskFrame(float left, float right, float top, float bottom, int index) {
	if (index<0 || index>7)
		return;
	float w = (right - left) / 4;
	float h = (bottom - top) / 4;
}

void sphere::ClearMaskFrame() {
}
