#include "SphereStitch.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "ParametricManager.h"

const char *SphereStitchSeg = 
"uniform mat4 projection;\n"
"uniform mat4 modelView;\n"
"attribute vec4 vPosition;\n"
"uniform int aMode;\n"
"attribute vec2 aTexCoor;\n"
"void main(void)\n"
"{\n"
"    gl_Position = vPosition*projection*modelView;\n"
"}";

const char *SphereStitchFrag =
"precision mediump float;\n"
"#define PI 3.1415926535897932384626433832795\n"
"#define DTOR            0.0174532925\n"
"#define RTOD            57.2957795\n"
"#define TWOPI           6.283185307179586476925287\n"
"#define PID2            1.570796326794896619231322\n"
"#define XTILT 0 \n"
"#define YROLL 1 \n"
"#define ZPAN  2 \n"
"struct TRANSFORM {\n"
"    int axis;\n"
"    float value;\n"
"    float cvalue;\n"
"    float svalue;\n"
"};\n"
"\n"
"struct FISHEYE {\n"
"    float width;\n"
"    float height;\n"
"    float centerx;\n"
"    float centery;\n"
"    float radius;\n"
"    TRANSFORM transform[3];\n"
"    int ntransform;\n"
"};\n"
"\n"
//"uniform float distortion[115];\n"
// half_height;\n"
"uniform FISHEYE fish[2];\n"
"uniform vec2 uSolution;\n"
"uniform float blending;\n"
"uniform int Stitching;\n"
"uniform sampler2D sTexture;\n"
"\n"
//"float getRadius(float angle)\n"
//"{\n"
//"    int i;\n"
//"    float llower;\n"
//"    float lunit;\n"
//"    for(i=0;i<114;i++)\n"
//"    {\n"
//"        llower = distortion[i];\n"
//"        lunit = distortion[i+1]-distortion[i];\n"
//"        if(angle>=float(i)&&angle<=float(i+1))\n"
//"            return (llower+(angle-float(i))*lunit)/half_height;\n"
//"    }\n"
//"    return 0.0;\n"
//"}\n"
"vec2 FindFishPixel(int n,float latitude,float longitude)\n"
"{\n"
"    int k,index;\n"
"    vec3 p,q;\n"
"    float theta,phi,r,lradius,lcenterx,lcentery,lwidth,lheight;\n"
"    float u,v;\n"
"\n"
"    if (n == 1)\n"
"        longitude += PI;\n"
"\n"
"    p.x = cos(latitude) * sin(longitude);\n"
"    p.y = cos(latitude) * cos(longitude);\n"
"    p.z = sin(latitude);\n"
"\n"
"    for (k=0;k<3;k++) {\n"
"        if(fish[n].transform[k].axis==XTILT) {\n"
"            q.x =  p.x;\n"
"            q.y =  p.y * fish[n].transform[k].cvalue + p.z * fish[n].transform[k].svalue;\n"
"            q.z = -p.y * fish[n].transform[k].svalue + p.z * fish[n].transform[k].cvalue;\n"
"        }\n"
"        else if(fish[n].transform[k].axis==YROLL) {\n"
"           q.x =  p.x * fish[n].transform[k].cvalue + p.z * fish[n].transform[k].svalue;\n"
"           q.y =  p.y;\n"
"           q.z = -p.x * fish[n].transform[k].svalue + p.z * fish[n].transform[k].cvalue;\n"
"       }\n"
"      else if(fish[n].transform[k].axis==ZPAN) {\n"
"           q.x =  p.x * fish[n].transform[k].cvalue + p.y * fish[n].transform[k].svalue;\n"
"           q.y = -p.x * fish[n].transform[k].svalue + p.y * fish[n].transform[k].cvalue;\n"
"           q.z =  p.z;\n"
"       }\n"
"       else\n"
"          {\n"
"            q=p;\n"
"          }\n"
"        p = q;\n"
"   }\n"
"\n"
"   theta = atan(p.z,p.x);\n"
"   phi = atan(sqrt(p.x*p.x+p.z*p.z),p.y);\n"
"   \n"
"   lradius = fish[n].radius*fish[n].height/2.0;\n"
"   lcenterx = fish[n].centerx*fish[n].height/2.0+fish[n].width/2.0;\n"
"   lcentery = -fish[n].centery*fish[n].height/2.0+fish[n].height/2.0;\n"
"   lwidth = float(fish[n].width);\n"
"   lheight = float(fish[n].height);\n"
"\n"
//"     r = lradius * getRadius(phi/PI*180.0);\n"
"     r = lradius * phi / (195.1* DTOR/2.0);\n"
"\n"
"   u = lcenterx + r * cos(theta);\n"
"   if (u < 0.0 || u > lwidth)\n"
"      u = 0.0;\n"
"\n"
"   v = lcentery - r * sin(theta);\n"
"   if (v < 0.0 || v > lheight)\n"
"       v = 0.0;\n"
"   \n"
"   float dr = sqrt((u-lcenterx)*(u-lcenterx)+(v-lcentery)*(v-lcentery));\n"
"   if(dr > lradius)\n"
"   {\n"
"      u = 0.0;\n"
"      v = 0.0;\n"
"   }\n"
"   u = u / lwidth;\n"
"   v = v / lheight;\n"
"   return vec2(u,v);\n"
"}\n"
"\n"
"void main(void)\n"
"{\n"
"    int n;\n"
"    float blendwidth = DTOR*blending;\n"
"    vec4 color1=vec4(0.0,0.0,0.0,0.0);\n"
"    vec4 color2=vec4(0.0,0.0,0.0,0.0);\n"
"    vec2 Coord = vec2(0.0,0.0);\n"
"    vec2 ltmp = gl_FragCoord.xy/uSolution;\n"
"    ltmp.y = 1.0-ltmp.y;\n"
#if defined(__APPLE__) && !defined(__JA_MAC__)
"    vec2 xy = vec2(TWOPI*ltmp.x-PI,PI*ltmp.y-PI-PID2);\n"
#else
"    vec2 xy = vec2(TWOPI*ltmp.x-PI,PID2-PI*ltmp.y);\n"
#endif
"    float blend = (PID2 + 0.5*blendwidth - abs(xy.x)) / blendwidth;\n"
"    if(blend<0.0)\n"
"       blend = 0.0;\n"
"    if(blend>1.0)\n"
"       blend = 1.0;\n"
"    float oneminusblend = 1.0 - blend;\n"
"    for (n=0;n<2;n++) \n"
"    {\n"
"        if (n == 0) \n"
"        {\n"
"            if (xy.x > (PID2+blendwidth) || xy.x < (-PID2 - blendwidth))\n"
"                continue;\n"
"        } \n"
"        else \n"
"        {\n"
"            if (xy.x > (-PID2 + blendwidth) && xy.x < (PID2 - blendwidth))\n"
"                continue;\n"
"        }\n"
"        Coord = FindFishPixel(n,xy.y,xy.x);\n"
"        if(Coord.x>0.0&&Coord.y>0.0)\n"
"        {\n"
"        \tif(n==0)\n"
"                color1 = texture2D(sTexture,Coord);\n"
"        \telse\n"
"                color2 = texture2D(sTexture,Coord);\n"
"        }\n"
"    }\n"
"    if(Stitching==0)\n"
"       gl_FragColor=color1*blend+color2*oneminusblend;\n"
"    else\n"
"    {\n"
"       if(ltmp.x<(0.25-10.0/uSolution.x)||(ltmp.x>(0.25+10.0/uSolution.x)&&ltmp.x<(0.75-10.0/uSolution.x))||ltmp.x>(0.75+10.0/uSolution.x))\n"
"       {\n"
"          if(Stitching==1)\n"
"             gl_FragColor=color1*blend+color2*oneminusblend;\n"
"          else\n"
"             gl_FragColor=vec4(0.0,0.0,0.0,1.0);\n"
"       }\n"
"       else\n"
"          gl_FragColor=vec4(color1.r-color2.r,color1.g-color2.g,color1.b-color2.b,1.0);\n"
"    }\n"
"}";





void SphereStitch::StartAnimation(int scnindex)
{
	BaseScreen::StartAnimation(scnindex);
}

void SphereStitch::StopAnimation(int scnindex)
{
	BaseScreen::StopAnimation(scnindex);
}

SphereStitch::~SphereStitch() {
}

SphereStitch::SphereStitch(float radius,float aspect,void *mgn)
{
	m_aspect = aspect;
	_Manager = mgn;
	ParametricInterval interval = { 6, vec2(6,6), vec2(366,186) };
	SetInterval(interval);
#ifdef __USE_RGB32__
	setupProgram(SphereStitchSeg, SphereStitchFrag);
#else
	setupProgram(SphereStitchSeg, SphereStitchFrag);
	//setupProgram(SphereStitchSeg, YUV_SphereStitchFrag, true);
#endif
	//eTB_GLSL__print_uniforms(_programHandle);
	InitUniform();
	memset(mFish, 0, sizeof(mFish));
    mBlending = 0.0f;
    mStitching = 0;
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 3; j++)
			mFish[i].transform[j].axis = -1;
	mFish[0].aperture = 90*DTOR;
	mFish[1].aperture = 90*DTOR;
	this->setupProjection();
	SetupBuffer();
	cxFBO = 0;
	cyFBO = 0;
	_frameBufferID = 0;
	_colorRenderBuffer = 0;
	_depthRenderBuffer = 0;
	_fboTextureID = 0;

    eTB_GLSL__print_uniforms(_programHandle);
	BaseScreen::setupLineBuffer();
}

int SphereStitch::GetScreenMode() {
    return SCRN_STITCH;
}

int SphereStitch::GetScreenCount() {
    return 2;
}

void SphereStitch::DrawSelf(int scnindex)
{
	glUseProgram(_programHandle);
	UpdateUniform();
	BaseScreen::DrawSelf(0);
}


void SphereStitch::ResetPosition() {
	BaseScreen::ResetPosition();
}

void SphereStitch::SetPosition(vec3 pos, bool texture,int scnindex)
{
	BaseScreen::SetPosition(pos, texture, scnindex);
}

void SphereStitch::SetScale(vec3 scale, bool texture,int scnindex)
{
	BaseScreen::SetScale(scale, texture, scnindex);
}

void SphereStitch::setRotate(vec3 rotate, bool texture,int scnindex)
{
	BaseScreen::SetPosition(rotate, texture, scnindex);
}

vec3 SphereStitch::GetPosition(bool texture,int scnindex)
{
	return BaseScreen::GetPosition(texture, scnindex);
}

vec3 SphereStitch::GetScale(bool texture,int scnindex)
{
	return BaseScreen::GetScale(texture, scnindex);
}

vec3 SphereStitch::GetRotate(bool texture,int scnindex)
{
	return BaseScreen::GetRotate(texture, scnindex);
}


bool SphereStitch::LoadTexture(int w,int h,GLenum type,GLvoid *pixels,int scnindex)
{
    //uint8_t *lbuf;
    //uint8_t *src=(uint8_t *)pixels;
    //float lx,ly,ld,la;
    //int lw,lh,ll,lt;
    //mFront->GetFishParameter(&lx,&ly,&ld,&la);
    //ll = w*lx/2.0f-(ld/2.0f)*h;
    //lt = h*ly-(ld/2.0f)*h;
    //lw = ld*h;
    //lh = ld*h;
    //if(lt<0)
    //    lt=0;
    //if(ll<0)
    //    ll=0;
    //if(ll+lw>w)
    //    lw=w-ll;
    //if(lt+lh>h)
    //    lh=h-lt;
    //lbuf=(uint8_t *)malloc(lw*lh*4);
    //for(int i=lt;i<lh+lt;i++)
    //    memcpy(lbuf+(i-lt)*lw*4,src+i*w*4+ll*4,lw*4);
    //mBack->LoadTexture(lw,lh,type,lbuf,0);
    //mFront->LoadTexture(lw,lh,type,lbuf,0);
    //free(lbuf);
    //mMiddle->GetFishParameter(&lx,&ly,&ld,&la);
    //ll = w*lx/2.0f-(ld/2.0f)*h;
    //lt = h*ly-(ld/2.0f)*h;
    //lw = ld*h;
    //lh = ld*h;
    //if(lt<0)
    //    lt=0;
    //if(ll<0)
    //    ll=0;
    //if(ll+lw>w)
    //    lw=w-ll;
    //if(lt+lh>h)
    //    lh=h-lt;
    //lbuf=(uint8_t *)malloc(lw*lh*4);
    //for(int i=lt;i<lh+lt;i++)
    //    memcpy(lbuf+(i-lt)*lw*4,src+i*w*4+ll*4,lw*4);
    //mMiddle->LoadTexture(lw,lh,type,lbuf,0);
    //free(lbuf);
	mFish[0].width = w;
	mFish[1].width = w;
	mFish[0].height = h;
	mFish[1].height = h;
	ParametricSurface::LoadTexture(w, h, type, pixels, scnindex);
    return true;
}

void SphereStitch::SetTexture(GLuint texid,int scnindex)
{
	BaseScreen::SetTexture(texid, scnindex);
}

GLuint SphereStitch::GetTexture(int scnindex)
{
	return BaseScreen::GetTexture(scnindex);
}

bool SphereStitch::NeedDraw()
{
	return BaseScreen::NeedDraw();
}

void SphereStitch::SetNoVideoTexture(GLuint texid)
{
	BaseScreen::SetNoVideoTexture(texid);
}

void SphereStitch::LoadOSDTexture(int w,int h,GLenum type,GLvoid *pixels)
{
	BaseScreen::LoadOSDTexture(w, h, type, pixels);
}

void SphereStitch::LoadOSDTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels)
{
	BaseScreen::LoadOSDTexture(w, h, y_pixels, cbcr_pixels);
}

bool SphereStitch::LoadTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels,int scnindex)
{
	BaseScreen::LoadTexture(w, h, y_pixels, cbcr_pixels, scnindex);
    return true;
}

void SphereStitch::SetTexture(GLuint y_texid,GLuint cbcr_texid,int scnindex)
{
	BaseScreen::SetTexture(y_texid, cbcr_texid, scnindex);
}

GLuint SphereStitch::GetTexture(int scnindex,int yuv)
{
    return 0;
}

void SphereStitch::SetNoVideoTexture(GLuint y_texid,GLuint u_texid,GLuint v_texid)
{

}

void SphereStitch::SetStitchOptions(float centerx, float centery, float radius,float aperture, float anglex, float angley, float anglez,int width,int height, int index)
{
	if (index < 0 || index>1)
		return;
	mFish[index].width = width;
	mFish[index].height = height;
	mFish[index].centerx = centerx;
	mFish[index].centery = centery;
	mFish[index].radius = radius;
	mFish[index].aperture = aperture;
	if (anglex != 0.000f)
	{
		mFish[index].transform[0].axis = XTILT;
		mFish[index].transform[0].value = anglex * DTOR;
		mFish[index].transform[0].cvalue = cosf(mFish[index].transform[0].value);
		mFish[index].transform[0].svalue = sinf(mFish[index].transform[0].value);
	}
	if (angley != 0.000f)
	{
		mFish[index].transform[1].axis = YROLL;
		mFish[index].transform[1].value = angley * DTOR;
		mFish[index].transform[1].cvalue = cosf(mFish[index].transform[1].value);
		mFish[index].transform[1].svalue = sinf(mFish[index].transform[1].value);
	}
	if (anglez != 0.000f)
	{
		mFish[index].transform[2].axis = ZPAN;
		mFish[index].transform[2].value = anglez * DTOR;
		mFish[index].transform[2].cvalue = cosf(mFish[index].transform[2].value);
		mFish[index].transform[2].svalue = sinf(mFish[index].transform[2].value);
	}
}



bool SphereStitch::InitFBO(int w,int h) {
#ifndef __ANDROID__1
	if (_frameBufferID != 0)
	{
		glDeleteRenderbuffers(1, &_depthRenderBuffer);
		glDeleteRenderbuffers(1, &_colorRenderBuffer);
		glDeleteFramebuffers(1, &_frameBufferID);
	}

	cxFBO = w;
	cyFBO = h;

//    cxFBO = 375;
//    cyFBO = 375;
    
    
	GLint maxTextureSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

	if ((cxFBO > maxTextureSize) ||
        (cyFBO > maxTextureSize) || cxFBO==0 || cyFBO == 0)
	{
		return false;
	}

	glGenFramebuffers(1, &_frameBufferID);
	glGenRenderbuffers(1, &_depthRenderBuffer);
	glGenTextures(1, &_fboTextureID);
//	LOGDEBUG("0--glGenTextures:%d",texture_object_id);


	// texture
#ifdef __ANDROID__1
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, _fboTextureID);

    // Can't do mipmapping with camera source
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER,
            GL_NEAREST);
    glTexParameterf(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER,
            GL_LINEAR);
    // Clamp to edge is the only option
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_S,
            GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_WRAP_T,
            GL_CLAMP_TO_EDGE);
#else
	glBindTexture(GL_TEXTURE_2D, _fboTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cxFBO, cyFBO, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif
	// depth buffer
	glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, cxFBO, cyFBO);

	// frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferID);


	// attachments
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTextureID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);

	//glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferID);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer is not complete\n");
		return false;
	}

	mRenderBuffer = (void *)malloc(cxFBO*cyFBO * 4);

#ifdef __ANDROID__1
    glBindTexture(GL_TEXTURE_EXTERNAL_OES,0);
#else
	glBindTexture(GL_TEXTURE_2D, 0);
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
	return true;
}

bool SphereStitch::RenderFBO() {
#ifndef __ANDROID__1
	GLint sb[4];// , sb1[4];
	glGetError();
	glGetIntegerv(GL_SCISSOR_BOX, &sb[0]);
	glDisable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);
    ParametricManager *mgn = (ParametricManager *)_Manager;
    int width, height;
    if(mgn)
    {
        width = mgn->GetTextureWidth();
        height = mgn->GetTextureHeight();
    }
    //todo 转换还不对
    if(!IsInitFBO()||(cxFBO!=width&&cxFBO!=height))
        InitFBO(width,height);
    if(!IsInitFBO())
        return false;
    
    
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferID);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    glClearColor(1.0,0.0, 255.0/255.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glEnable(GL_DEPTH_TEST);
   
    
	glViewport(0, 0, cxFBO, cyFBO);
	//glGetIntegerv(GL_SCISSOR_BOX, &sb1[0]);
	mFish[0].width = cxFBO;
	mFish[0].height = cyFBO;
	mFish[1].width = cxFBO;
	mFish[1].height = cyFBO;
	//printf("BindFramebuffer2:%d\n", glGetError());
#endif
	this->DrawSelf(0);
#ifndef __ANDROID__1
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	glFlush();
	//glReadPixels(0, 0, cxFBO, cyFBO, GL_RGBA, GL_UNSIGNED_BYTE, mRenderBuffer);
#if 0
	FILE *fp = fopen("1.rgba", "wb");
	fwrite(mRenderBuffer, 1, cxFBO*cyFBO*4, fp);
	fclose(fp);
#endif
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if(sb[2]!=0&&sb[3]!=0)
    {
//        glEnable(GL_SCISSOR_TEST);
//        glScissor(sb[0], sb[1], sb[2], sb[3]);
    }
	//glBindTexture(GL_TEXTURE_2D, _fboTextureID);
	//glGenerateMipmap(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, 0);
#endif
    return true;
}

void SphereStitch::DestroyFBO() {
	if (_frameBufferID != 0)
	{
		glDeleteRenderbuffers(1, &_depthRenderBuffer);
		glDeleteRenderbuffers(1, &_colorRenderBuffer);
		glDeleteFramebuffers(1, &_frameBufferID);
	}
}

void SphereStitch::InitUniform()
{
	memset(mFishHandle, 0, sizeof(mFishHandle));
	mFishHandle[0].centerx = glGetUniformLocation(_programHandle, "fish[0].centerx");
	mFishHandle[0].centery = glGetUniformLocation(_programHandle, "fish[0].centery");
	mFishHandle[0].width = glGetUniformLocation(_programHandle, "fish[0].width");
	mFishHandle[0].height = glGetUniformLocation(_programHandle, "fish[0].height");
	mFishHandle[0].ntransform = glGetUniformLocation(_programHandle, "fish[0].ntransform");
	mFishHandle[0].radius = glGetUniformLocation(_programHandle, "fish[0].radius");
	mFishHandle[0].transform[0].axis = glGetUniformLocation(_programHandle, "fish[0].transform[0].axis");
	mFishHandle[0].transform[0].cvalue = glGetUniformLocation(_programHandle, "fish[0].transform[0].cvalue");
	mFishHandle[0].transform[0].svalue = glGetUniformLocation(_programHandle, "fish[0].transform[0].svalue");
	mFishHandle[0].transform[0].value = glGetUniformLocation(_programHandle, "fish[0].transform[0].value");

	mFishHandle[0].transform[1].axis = glGetUniformLocation(_programHandle, "fish[0].transform[1].axis");
	mFishHandle[0].transform[1].cvalue = glGetUniformLocation(_programHandle, "fish[0].transform[1].cvalue");
	mFishHandle[0].transform[1].svalue = glGetUniformLocation(_programHandle, "fish[0].transform[1].svalue");
	mFishHandle[0].transform[1].value = glGetUniformLocation(_programHandle, "fish[0].transform[1].value");

	mFishHandle[0].transform[2].axis = glGetUniformLocation(_programHandle, "fish[0].transform[2].axis");
	mFishHandle[0].transform[2].cvalue = glGetUniformLocation(_programHandle, "fish[0].transform[2].cvalue");
	mFishHandle[0].transform[2].svalue = glGetUniformLocation(_programHandle, "fish[0].transform[2].svalue");
	mFishHandle[0].transform[2].value = glGetUniformLocation(_programHandle, "fish[0].transform[2].value");

	mFishHandle[1].centerx = glGetUniformLocation(_programHandle, "fish[1].centerx");
	mFishHandle[1].centery = glGetUniformLocation(_programHandle, "fish[1].centery");
	mFishHandle[1].width = glGetUniformLocation(_programHandle, "fish[1].width");
	mFishHandle[1].height = glGetUniformLocation(_programHandle, "fish[1].height");
	mFishHandle[1].ntransform = glGetUniformLocation(_programHandle, "fish[1].ntransform");
	mFishHandle[1].radius = glGetUniformLocation(_programHandle, "fish[1].radius");
	mFishHandle[1].transform[0].axis = glGetUniformLocation(_programHandle, "fish[1].transform[0].axis");
	mFishHandle[1].transform[0].cvalue = glGetUniformLocation(_programHandle, "fish[1].transform[0].cvalue");
	mFishHandle[1].transform[0].svalue = glGetUniformLocation(_programHandle, "fish[1].transform[0].svalue");
	mFishHandle[1].transform[0].value = glGetUniformLocation(_programHandle, "fish[1].transform[0].value");

	mFishHandle[1].transform[1].axis = glGetUniformLocation(_programHandle, "fish[1].transform[1].axis");
	mFishHandle[1].transform[1].cvalue = glGetUniformLocation(_programHandle, "fish[1].transform[1].cvalue");
	mFishHandle[1].transform[1].svalue = glGetUniformLocation(_programHandle, "fish[1].transform[1].svalue");
	mFishHandle[1].transform[1].value = glGetUniformLocation(_programHandle, "fish[1].transform[1].value");

	mFishHandle[1].transform[2].axis = glGetUniformLocation(_programHandle, "fish[1].transform[2].axis");
	mFishHandle[1].transform[2].cvalue = glGetUniformLocation(_programHandle, "fish[1].transform[2].cvalue");
	mFishHandle[1].transform[2].svalue = glGetUniformLocation(_programHandle, "fish[1].transform[2].svalue");
	mFishHandle[1].transform[2].value = glGetUniformLocation(_programHandle, "fish[1].transform[2].value");
	mSolutionHandle = glGetUniformLocation(_programHandle, "uSolution");
    mBlendingHandle = glGetUniformLocation(_programHandle,"blending");
    mStitchingHandle = glGetUniformLocation(_programHandle,"Stitching");
//	_halfheightHandle = glGetUniformLocation(_programHandle, "half_height");
//	for (int i = 0; i < 115; i++)
//	{
//		char mstr[20];
//		sprintf(mstr, "distortion[%d]", i);
//		_distortionHandle[i] = glGetUniformLocation(_programHandle, mstr);
//	}
}

void SphereStitch::UpdateUniform()
{
    
    ParametricManager *mgn = (ParametricManager *)_Manager;
    int width, height;
    if(mgn)
    {
        width = mgn->GetTextureWidth();
        height = mgn->GetTextureHeight();
        mFish[0].width = width;
        mFish[0].height = height;
        mFish[1].width = width;
        mFish[1].height = height;
    }
    
    
    glUniform1f(mFishHandle[0].centerx, mFish[0].centerx);
	glUniform1f(mFishHandle[0].centery, mFish[0].centery);
	glUniform1f(mFishHandle[0].width, mFish[0].width);
	glUniform1f(mFishHandle[0].height, mFish[0].height);
	glUniform1f(mFishHandle[0].radius, mFish[0].radius);
	glUniform1i(mFishHandle[0].ntransform, mFish[0].ntransform);
	glUniform1i(mFishHandle[0].transform[0].axis, mFish[0].transform[0].axis);
	glUniform1f(mFishHandle[0].transform[0].cvalue, mFish[0].transform[0].cvalue);
	glUniform1f(mFishHandle[0].transform[0].svalue, mFish[0].transform[0].svalue);
	glUniform1f(mFishHandle[0].transform[0].value, mFish[0].transform[0].value);

	glUniform1i(mFishHandle[0].transform[1].axis, mFish[0].transform[1].axis);
	glUniform1f(mFishHandle[0].transform[1].cvalue, mFish[0].transform[1].cvalue);
	glUniform1f(mFishHandle[0].transform[1].svalue, mFish[0].transform[1].svalue);
	glUniform1f(mFishHandle[0].transform[1].value, mFish[0].transform[1].value);

	glUniform1i(mFishHandle[0].transform[2].axis, mFish[0].transform[2].axis);
	glUniform1f(mFishHandle[0].transform[2].cvalue, mFish[0].transform[2].cvalue);
	glUniform1f(mFishHandle[0].transform[2].svalue, mFish[0].transform[2].svalue);
	glUniform1f(mFishHandle[0].transform[2].value, mFish[0].transform[2].value);


	glUniform1f(mFishHandle[1].centerx, mFish[1].centerx);
	glUniform1f(mFishHandle[1].centery, mFish[1].centery);
	glUniform1f(mFishHandle[1].width, mFish[1].width);
	glUniform1f(mFishHandle[1].height, mFish[1].height);
	glUniform1f(mFishHandle[1].radius, mFish[1].radius);
	glUniform1i(mFishHandle[1].ntransform, mFish[1].ntransform);
	glUniform1i(mFishHandle[1].transform[0].axis, mFish[1].transform[0].axis);
	glUniform1f(mFishHandle[1].transform[0].cvalue, mFish[1].transform[0].cvalue);
	glUniform1f(mFishHandle[1].transform[0].svalue, mFish[1].transform[0].svalue);
	glUniform1f(mFishHandle[1].transform[0].value, mFish[1].transform[0].value);

	glUniform1i(mFishHandle[1].transform[1].axis, mFish[1].transform[1].axis);
	glUniform1f(mFishHandle[1].transform[1].cvalue, mFish[1].transform[1].cvalue);
	glUniform1f(mFishHandle[1].transform[1].svalue, mFish[1].transform[1].svalue);
	glUniform1f(mFishHandle[1].transform[1].value, mFish[1].transform[1].value);

	glUniform1i(mFishHandle[1].transform[2].axis, mFish[1].transform[2].axis);
	glUniform1f(mFishHandle[1].transform[2].cvalue, mFish[1].transform[2].cvalue);
	glUniform1f(mFishHandle[1].transform[2].svalue, mFish[1].transform[2].svalue);
	glUniform1f(mFishHandle[1].transform[2].value, mFish[1].transform[2].value);
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	glUniform2f(mSolutionHandle, vp[2], vp[3]);
    glUniform1f(mBlendingHandle,mBlending);
    glUniform1i(mStitchingHandle,mStitching);
//	glUniform1f(_halfheightHandle, mHalf_Height);
//	for(int i=0;i<115;i++)
//	{
//		glUniform1f(_distortionHandle[i], mDistortion[i]);
//	}
}

void SphereStitch::SetDistortion(PDISTORTION_NUM pdist, int dist_count, float half)
{
	memset(mDistortion, 0, sizeof(float)*115);
	mDistortion[0] = 0.0;
	mHalf_Height = half;
	int angle = 1;
	for(int i=0;i<dist_count-1;i++)
	{
		while(angle>=pdist[i].angle&&angle<pdist[i+1].angle)
		{
			float llower = pdist[i].half_height;
			float lunit = (pdist[i + 1].half_height - pdist[i].half_height)/(pdist[i+1].angle-pdist[i].angle);
			mDistortion[angle] = llower+ lunit*(angle-pdist[i].angle);
			//printf("mDistortion[%d]:%f\n",angle,mDistortion[angle]);
			angle++;
		}
	}
	//printf("angle-----------------------------%d\n", angle);
}

vec3 SphereStitch::Evaluate(const vec2& domain)
{
	float u = domain.x, v = domain.y;
	float x, y, z;
	x = TWOPI * u / 360.0 - PI;
	y = PID2 - PI * v / 180.0f;
	z = -0.095f;
	return vec3(x, y, z);
//    float u = domain.x, v = domain.y;
//    float x = m_radius * cos(u/180.0f*Pi) * cos(v/180.0f*Pi);
//    float y = m_radius * sin(u/180.0f*Pi);
//    float z = m_radius * cos(u/180.0f*Pi) * sin(v/180.0f*Pi);
//    return vec3(x, y, z);
}

vec2 SphereStitch::EvaluateCoord(const vec2& domain)
{
	float x=0, y=0;
	return vec2(x, y);
}
