//
// Created by Hong Li on 2016/12/11.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "SphereStitch2.h"

#include <stdio.h>

#include <string.h>

#define LOG_TAG "JAVideo"
#define LOGD(...)__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGV(...)__android_log_print(ANDROID_LOG_DEBUG,"Lee",__VA_ARGS__)
#define INFO(...) {__android_log_write(ANDROID_LOG_INFO,"videoconnect",__VA_ARGS__);}
#define LOGE(...) {__android_log_print(ANDROID_LOG_ERROR, "videoconnect", __VA_ARGS__);}
#ifdef GLESV2
#include "ParametricManagerV2.h"
#else
#include "ParametricManager.h"
typedef ParametricManager ParametricManagerV2;
#endif


const char *SphereStitch2Seg = "uniform mat4 projection;\n"
"uniform mat4 modelView;\n"
"attribute vec4 vPosition;\n"
"attribute vec2 aTexCoor;\n"
"uniform float aAspect;\n"
"uniform int vMode;\n"
"uniform float vBlending;\n"
"varying float vAlpha;\n"
"varying float fMode;\n"
"\n"
"varying vec2 vTextureCoord;\n"
"\n"
"void main(void)\n"
"{\n"
"    vec4 lpos;\n"
"    float lz=vPosition.z;\n"
"    if(lz>0.0)\n"
"       vAlpha=(vBlending-lz)/vBlending;\n"
"    else\n"
"       vAlpha=1.0;\n"
"    if(vMode==0)\n"
"    {\n"
"       vTextureCoord = vec2(1.0-aTexCoor.x,aTexCoor.y);\n"
"       fMode=-1.0;\n"
"       lpos=vec4(-vPosition.x,vPosition.y,-lz,1.0);"
"    }\n"
"    else\n"
"    {\n"
"       vTextureCoord = vec2(aTexCoor.x,aTexCoor.y);\n"
"		fMode=1.0;\n"
"       lpos=vec4(-vPosition.x,vPosition.y,lz,1.0);"
"    }\n"
"    gl_Position = projection * modelView * lpos;\n"
"}";
const char *SphereStitch2Frag = "precision mediump float;\n"
"\n"
"varying vec2 vTextureCoord;\n"
"uniform sampler2D sTexture;\n"
"uniform vec2 vCenter1;\n"
"uniform float vRadius1;\n"
"uniform vec2 vCenter2;\n"
"uniform float vRadius2;\n"
"uniform float vTexAspect;\n"
"uniform int vSingle;\n"
"varying float vAlpha;\n"
"varying float fMode;\n"
"\n"
"void main()\n"
"{\n"
"	 vec2 ltc;\n"
"    vec4 lcolor;\n"
"	 if(fMode>0.0)\n"
"    {\n"
"			ltc = vec2((vTextureCoord.x-0.5)*vRadius1/vTexAspect+(vCenter1.x+vTexAspect)/(vTexAspect*2.0),(vTextureCoord.y-0.5)*vRadius1+(1.0-vCenter1.y)/2.0);"
"    }\n"
"    else\n"
"    {\n"
"       if(vSingle==0)\n"
"       {\n"
"			ltc = vec2((vTextureCoord.x-0.5)*vRadius2/vTexAspect+(vCenter2.x+vTexAspect)/(vTexAspect*2.0),(vTextureCoord.y-0.5)*vRadius2+(1.0-vCenter2.y)/2.0);"
"       }\n"
"       else\n"
"           ltc = vec2((vTextureCoord.x-0.5)*vRadius1+vCenter1.x/2.0+0.5,(vTextureCoord.y-0.5)*vRadius1+0.5-vCenter1.y/2.0);"
"    }\n"
"	 gl_FragColor=texture2D(sTexture,ltc);\n"
"    if(vSingle==0)\n"
"    {\n"
"			gl_FragColor=vec4(gl_FragColor.r,gl_FragColor.g,gl_FragColor.b,vAlpha);\n"
"    }\n"
"    else\n"
"		gl_FragColor=vec4(gl_FragColor.r,gl_FragColor.g,gl_FragColor.b,vAlpha);\n"
"}";

const char *YUV_SphereStitch2Frag = "precision mediump float;\n"
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


void SphereStitch2::DrawSelf(int index)
{
    int lindex = GetCurrentDistortion(mdistortion);
	//LOGV("---------------------------SphereStitch2::DrawSelf:lindex:%d", lindex);
    if(lindex<0)
        return;
	glUseProgram(_programHandle);
    PDISTORTIONLIST dlist = mDistList.at(lindex);
    if(dlist->TextureCoordBuffer==0) {
		//LOGV("---------------------------SphereStitch2::DrawSelf->setupbuffer");
        SetupBuffer();
        dlist->TextureCoordBuffer = TextureCoordBuffer;
        dlist->TextureCoordSize = TextureCoordSize;
        dlist->vertexBuffer = vertexBuffer;
        dlist->vertexSize = vertexSize;
		dlist->triangleIndexCount = triangleIndexCount;
    }
	else
	{
		TextureCoordBuffer = dlist->TextureCoordBuffer;
		//TextureCoordSize = dlist->TextureCoordSize;
		vertexBuffer = dlist->vertexBuffer;
		//vertexSize = dlist->vertexSize;
		triangleIndexCount = dlist->triangleIndexCount;
	}
    glUniform1f(mFragRadius1Handle, mCircle_radius[0]);
    glUniform2f(mFragCenter1Handle, mCenter[0].x, mCenter[0].y);
	glUniform1f(mFragRadius2Handle, mCircle_radius[1]);
	glUniform2f(mFragCenter2Handle, mCenter[1].x, mCenter[1].y);
	glUniform1f(mFragTexAspectHandle, mTexAspect);
	glUniform1f(mSegBlendingHandle, mBlending);
	if (mSingle)
	{
		glUniform1i(mFragSingleHandle, 1);
		glUniform1i(mSegModeHandle, 0);
		mMode = 0;
	}
	else
	{
		glUniform1i(mFragSingleHandle, 0);
		glUniform1i(mSegModeHandle, index);
		mMode = index;
	}
    BaseScreen::DrawSelf(0);
}

void SphereStitch2::SetupBuffer()
{
	int lvs = GetVertexSize();
	int vBufSize = (mdistortion_count-1) * 6 * lvs * 120;
	int vCount = (mdistortion_count-1) * 6 * 120;
	GLfloat *vbuf = new GLfloat[vBufSize];

	int CoordSize = GetCoordSize();
	int vCoorBufSize = (mdistortion_count-1) * 6 * CoordSize * 120;
	GLfloat *vcoordbuf = new GLfloat[vCoorBufSize];

	GenerateVertices(vbuf, vcoordbuf);

	//if (lvb != 0);
	//glDeleteBuffers(1, &lvb);
	//printf("\n\n");
	glGenBuffers(1, &lvb);
	LOGDEBUG("9--glGenBuffers:%d",lvb);

	//printf("gl error number:%d\n\n",glGetError());
	glBindBuffer(GL_ARRAY_BUFFER, lvb);
	glBufferData(GL_ARRAY_BUFFER, vBufSize * sizeof(GLfloat), vbuf, GL_DYNAMIC_DRAW);

	//printf("set vexture buffer:%d\n",glGetError());
	GLuint CoordBuffer = 0;
	glGenBuffers(1, &CoordBuffer);
	LOGDEBUG("10--glGenBuffers:%d",CoordBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, CoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, vCoorBufSize * sizeof(GLfloat), vcoordbuf, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//printf("set Coord buffer:%d\n",glGetError());

	delete[] vbuf;
	delete[] vcoordbuf;

    //if(vertexBuffer)
    //    glDeleteBuffers(1, &vertexBuffer);
    //if(TextureCoordBuffer)
    //    glDeleteBuffers(1, &TextureCoordBuffer);
	if(vertexBuffer)
		glDeleteBuffers(1,&vertexBuffer);
	vertexBuffer = lvb;
	vertexSize = lvs;
	triangleIndexCount = vCount;
	TextureCoordSize = CoordSize;
	if(TextureCoordBuffer)
		glDeleteBuffers(1,&TextureCoordBuffer);
	TextureCoordBuffer = CoordBuffer;
}

void SphereStitch2::GenerateVertices(float * vertices, float * texturecorrd)
{
	float* attribute = vertices;
	float* attribute1 = texturecorrd;
	int lcount = 0;
	for (int j = 1; j <mdistortion_count; j ++) {
		for (int i = 3; i < 363; i += 3) {

			vec2 domain = ComputeCoordDomain(i, j);
			vec3 range = Evaluate(domain);
			attribute = range.Write(attribute);

			domain = ComputeCoordDomain(i - 3, j);
			range = Evaluate(domain);
			attribute = range.Write(attribute);

			domain = ComputeCoordDomain(i, j - 1);
			range = Evaluate(domain);
			attribute = range.Write(attribute);

			domain = ComputeCoordDomain(i, j - 1);
			range = Evaluate(domain);
			attribute = range.Write(attribute);

			domain = ComputeCoordDomain(i - 3, j);
			range = Evaluate(domain);
			attribute = range.Write(attribute);

			domain = ComputeCoordDomain(i - 3, j - 1);
			range = Evaluate(domain);
			attribute = range.Write(attribute);

			lcount++;

			//calc texturecoord

			domain = ComputeDomain(i, j);
			vec2 range1 = EvaluateCoord(domain);
			attribute1 = range1.Write(attribute1);

			domain = ComputeDomain(i - 3, j);
			range1 = EvaluateCoord(domain);
			attribute1 = range1.Write(attribute1);

			domain = ComputeDomain(i, j - 1);
			range1 = EvaluateCoord(domain);
			attribute1 = range1.Write(attribute1);

			domain = ComputeDomain(i, j - 1);
			range1 = EvaluateCoord(domain);
			attribute1 = range1.Write(attribute1);

			domain = ComputeDomain(i - 3, j);
			range1 = EvaluateCoord(domain);
			attribute1 = range1.Write(attribute1);

			domain = ComputeDomain(i - 3, j - 1);
			range1 = EvaluateCoord(domain);
			attribute1 = range1.Write(attribute1);

		}
	}
}

void SphereStitch2::SetFishEyeParameter(float centerx, float centery, float radius, float anglex, float angley, float anglez, int index)
{
	LOGV("SphereStitch2::SetFishEyeParameter....");

	if (index < 0 || index>1)
		return;
	mCenter[index].x = centerx;
	mCenter[index].y = centery;
	mCircle_radius[index] = radius;
	vec3 tmpangle = vec3(anglex, angley, anglez);
	mRotateAngle[index].x = anglex;
	mRotateAngle[index].y = angley;
	mRotateAngle[index].z = anglez+180;
	LOGV("mRotateAngle[index].x : %f ,mRotateAngle[index].y : %f, mRotateAngle[index].z : %f", mRotateAngle[index].x, mRotateAngle[index].y, mRotateAngle[index].z);
}

void SphereStitch2::setupProgram(const char * segsrc, const char * fragsrc)
{
	BaseScreen::setupProgram(segsrc, fragsrc);
	mFragCenter1Handle = glGetUniformLocation(_programHandle, "vCenter1");
	mFragRadius1Handle = glGetUniformLocation(_programHandle, "vRadius1");
	mFragCenter2Handle = glGetUniformLocation(_programHandle, "vCenter2");
	mFragRadius2Handle = glGetUniformLocation(_programHandle, "vRadius2");
	mFragTexAspectHandle = glGetUniformLocation(_programHandle, "vTexAspect");
	mSegBlendingHandle = glGetUniformLocation(_programHandle, "vBlending");
	mFragSingleHandle = glGetUniformLocation(_programHandle, "vSingle");
    mSegModeHandle = glGetUniformLocation(_programHandle, "vMode");
}

void SphereStitch2::setupProgram(const char * segsrc, const char * fragsrc, bool yuv)
{
	BaseScreen::setupProgram(segsrc, fragsrc,yuv);
	mFragCenter1Handle = glGetUniformLocation(_programHandle, "vCenter1");
	mFragRadius1Handle = glGetUniformLocation(_programHandle, "vRadius1");
	mFragCenter2Handle = glGetUniformLocation(_programHandle, "vCenter2");
	mFragRadius2Handle = glGetUniformLocation(_programHandle, "vRadius2");
	mFragTexAspectHandle = glGetUniformLocation(_programHandle, "vTexAspect");
	mSegBlendingHandle = glGetUniformLocation(_programHandle, "vBlending");
	mFragSingleHandle = glGetUniformLocation(_programHandle, "vSingle");
	mSegModeHandle = glGetUniformLocation(_programHandle, "vMode");
}

void SphereStitch2::SetSingleMode(bool single)
{
	mSingle = single;
	if (mSingle)
		mScreenMode = SCRN_HEMISPHERE;
	else
		mScreenMode = SCRN_STITCH2;
}

void SphereStitch2::setupProjection() {
    BaseScreen::setupProjection();
}


SphereStitch2::SphereStitch2(float radius,float aspect,void *mgn) : m_radius(radius)
{
    m_aspect = aspect;
    _Manager = mgn;
	mCircle_radius[0] = 0.80;
	mCircle_radius[1] = 0.80;
	mCenter[0].x = -0.87;
	mCenter[0].y = 0.01;
	mCenter[1].x = 0.80;
	mCenter[1].y = 0.04;
    mTexAspect = 2.0;
    vertexBuffer = 0;
    TextureCoordBuffer = 0;
	mSingle = false;
	vertexBuffer = 0;
	TextureCoordBuffer = 0;
	mRotateAngle[0] = vec3(0.0, 0.0, 0.0);
	mRotateAngle[1] = vec3(0.0, 0.0, 0.0);
	memset(mRotateAngle,0, sizeof(mRotateAngle));
    //ParametricInterval interval = { 3, vec2(93,3), vec2(273, 363) };
    //SetInterval(interval);
	//SetDistortion(DistortionNum, LEN_DistortionCount,LEN_Image_Half_Height);
#ifdef __USE_RGB32__
    setupProgram(SphereStitch2Seg,SphereStitch2Frag);
#else
    setupProgram(SphereStitch2Seg,YUV_SphereStitch2Frag,true);
#endif
	eTB_GLSL__print_uniforms(_programHandle);
    setupProjection();
    //SetupBuffer();
    mScreenMode = SCRN_STITCH2;
}

void SphereStitch2::ResetPosition() {
    ParametricSurface::ResetPosition();
    _Zoom = -3;
}

vec3 SphereStitch2::Evaluate(const vec2& domain)
{
	int j = (int)domain.y;
    float u = domain.x, v = mdistortion[j].angle;
    float x = m_radius * cos(u/180.0f*Pi) * sin(v/180.0f*Pi);
    float y = m_radius * sin(u/180.0f*Pi) * sin(v/180.0f*Pi);
    float z = -m_radius /* cos(u/180.0f*Pi) */ * cos(v/180.0f*Pi);
    return vec3(x, y, z);
}

vec2 SphereStitch2::EvaluateCoord(const vec2& domain)
{
    int j = (int)domain.y;
    float u = domain.x, v = mdistortion[j].half_height/mdistortion_half_height;
    float x = cos(u/180.0f*Pi)/2*v+0.5;
    float y = 1.0-(sin(u/180.0f*Pi)/2*v+0.5);
    //printf("x:%f,y:%f\n",x,y);
    return vec2(x,y);
}

void SphereStitch2::TurnLeftRight(GLfloat angle,int scnindex)
{
    ksRotate(&_rotationMatrix, angle*90.0f, 0, 0, 1);
}
void SphereStitch2::TurnUpDown(GLfloat angle,int scnindex)
{
    _udCurAngle += angle;
    KSMatrix4 rotate;
    ksMatrixLoadIdentity(&rotate);

    ksRotate(&rotate, angle*90.0f, 1, 0, 0);

    ksMatrixMultiply(&_rotationMatrix, &_rotationMatrix, &rotate);
}

void SphereStitch2::setZoom(GLfloat multi,int scnindex)
{
    _Zoom = multi-4;
}

void SphereStitch2::updateSurfaceTransform() {
	ksMatrixLoadIdentity(&_modelViewMatrix);

	ksMatrixLoadIdentity(&_rotationMatrix);
	KSMatrix4 lrotate, ltmprotate;
	vec3 tmprotate = mRotate + mRotateAngle[mMode];
	ksMatrixLoadIdentity(&lrotate);
	if (tmprotate.x) {
		//		ksMatrixLoadIdentity(&ltmprotate);
		ksRotate(&lrotate, tmprotate.x, 1, 0, 0);
		//		ksMatrixMultiply(&lrotate, &lrotate, &ltmprotate);
	}
	if (tmprotate.y) {
		//		ksMatrixLoadIdentity(&ltmprotate);
		ksRotate(&lrotate, tmprotate.y, 0, 1, 0);
		//		ksMatrixMultiply(&lrotate, &lrotate, &ltmprotate);
	}
	if (tmprotate.z) {
		//		ksMatrixLoadIdentity(&ltmprotate);
		ksRotate(&lrotate, tmprotate.z, 0, 0, 1);
		//		ksMatrixMultiply(&lrotate, &lrotate, &ltmprotate);
	}
	ksMatrixMultiply(&_rotationMatrix, &_rotationMatrix, &lrotate);
	ksTranslate(&_modelViewMatrix, 0.0, 0.0, _Zoom);

	ksTranslate(&_modelViewMatrix, mPosition.x, mPosition.y, mPosition.z);
	//ksScale(&_rotationMatrix,mScale.x,mScale.y,mScale.z);
	//使用另外一种模式来放大
	if (GetScreenMode() != SCRN_CYLINDER&&GetScreenMode() != SCRN_ONENORMAL) {
		ksMatrixLoadIdentity((KSMatrix4 *)&_projectionMatrix);
		ksPerspective((KSMatrix4 *)&_projectionMatrix, 60.0 / mScale.x, m_aspect, 0.0001f, 1200.0f);
		glUniformMatrix4fv(_projectionSlot, 1, GL_FALSE, (GLfloat *)&_projectionMatrix.m[0][0]);
	}
	else
		ksScale(&_rotationMatrix, mScale.x, mScale.y, mScale.z);


	///////////

	ksMatrixMultiply(&_modelViewMatrix, &_rotationMatrix, &_modelViewMatrix);
	glUniformMatrix4fv(_modelViewSlot, 1, GL_FALSE, (GLfloat *)&_modelViewMatrix.m[0][0]);
	ksMatrixLoadIdentity((KSMatrix4 *) &_projectionMatrix);
	ParametricManagerV2 *mgn = (ParametricManagerV2 *)_Manager;
	float ViewAngle;
	if (mgn)
		ViewAngle = mgn->GetViewAngle();
	else
		ViewAngle = 60.0;
	ksPerspective((KSMatrix4 *) &_projectionMatrix, ViewAngle / mScale.x, m_aspect, 0.1f, 1200.0f);
    glUniformMatrix4fv(_projectionSlot, 1, GL_FALSE, (GLfloat *) &_projectionMatrix.m[0][0]);
}

void SphereStitch2::SetDistortion(PDISTORTION_NUM pdist, int dist_count, float half) {
    if(!pdist)
        return;
    mdistortion = pdist;
    mdistortion_count = dist_count;
    mdistortion_half_height = half;
	mBlending = -cosf(pdist[dist_count - 1].angle/180.0*Pi);
    int index = GetCurrentDistortion(pdist);
    if(index<0)
    {
        PDISTORTIONLIST dlist = (PDISTORTIONLIST) calloc(1, sizeof(DISTORTIONLIST));
        dlist->mdistortion=pdist;
        dlist->mdistortion_count=dist_count;
        dlist->mdistortion_half_height = half;
		dlist->blending = mBlending;
        mDistList.insert(-1, dlist);
    }
}

int SphereStitch2::GetCurrentDistortion(PDISTORTION_NUM dist) {
    for(int i=0;i<mDistList.length();i++)
        if(mDistList.at(i)->mdistortion==dist)
            return i;
    return -1;
}

SphereStitch2::~SphereStitch2() {
	if(_texid)
	{
		glDeleteTextures(1,&_texid);
		_texid = 0;
	}
	if(vertexBuffer)
	{
		glDeleteBuffers(1,&vertexBuffer);
		LOGDEBUG("9---glDeleteBuffers:%d %p",vertexBuffer,this);
		vertexBuffer = 0;
	}
	if(TextureCoordBuffer)
	{
		glDeleteBuffers(1,&TextureCoordBuffer);
		LOGDEBUG("10---glDeleteBuffers:%d %p",TextureCoordBuffer,this);
		TextureCoordBuffer = 0;

	}
}
