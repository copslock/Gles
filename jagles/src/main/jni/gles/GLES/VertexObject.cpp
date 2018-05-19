//
// Created by LiHong on 16/6/15.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "VertexObject.h"
#include "ParametricManager.h"
#include "../Utils/mediabuffer.h"
#include "SelectedBox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __ANDROID_API__
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#define LOGV(...)__android_log_print(ANDROID_LOG_DEBUG,"Lee",__VA_ARGS__)
#endif


const char *VertexObjectSeg = "#define PI 3.1415926535897932384626433832795\n"
        "#define VERTEX_TYPE_HEMISPHERE 0\n"
        "#define VERTEX_TYPE_PANORAMA 1\n"
        "#define VERTEX_TYPE_CYLINDER 2\n"
        "#define VERTEX_TYPE_EXPAND 3\n"
        "#define VERTEX_TYPE_NORMAL 4\n"
        "#define VERTEX_TYPE_SPHERE 5\n"
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
        "varying float vtexwidth;\n"
        "varying float vtexheight;\n"
//        "varying vec2 vTextCoord;\n"
        "\n"
        "vec4 GetPosition(mat4 proj,mat4 mv,vec4 pos,int mode,float diameter)\n"
        "{\n"
        "    vec4 aPos=pos;\n"
        "    if(mode==VERTEX_TYPE_CYLINDER)\n"
        "    {\n"
        "       float y;"
        "       float x=cos((aPos.x)/diameter-0.5*PI+(diameter-1.0)/diameter*PI)*0.8*diameter;\n"
        "       if(aPos.y>250.0)\n"
        "           y=-35.0/180.0*PI*1.1;\n"
        "       else\n"
        "           y=(215.0-aPos.y)/180.0*PI*1.1;\n"
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
        "    float sin_factor = sin(-pRotate);\n"
        "    float cos_factor = cos(-pRotate);\n"
        "    vec2 tmpcoor = vec2(aTexCoor.x - 0.5, aTexCoor.y - 0.5) * mat2(cos_factor, sin_factor, -sin_factor, cos_factor);\n"
//        "    if(texheight>0.0)\n"
//        "    {\n"
//        "       float vcut=texwidth/texheight;\n"
//        "       if(vcut<1.33)\n"
//        "          vcut=1.0;\n"
//        "       return vec2(tmpcoor.x+0.5,tmpcoor.y*vcut+0.5);\n"
//        "    }\n"
//        "    else\n"
        "       return vec2(tmpcoor.x+0.5,tmpcoor.y+0.5);\n"
        "}\n"
        "void main(void)\n"
        "{\n"
        "    vec4 aPos = GetPosition(projection,modelView,vPosition,aMode,aDiameter);\n"
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
        "    gl_Position = aPos;\n"
        "    vtexwidth=texwidth;\n"
        "    vtexheight = texheight;\n"
        "}";
const char *VertexObjectFrag = "precision mediump float;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "varying vec2 vTextCoord;\n"
        "uniform sampler2D sTexture;\n"
        "uniform float centerx;\n"
        "uniform float centery;\n"
        "uniform float radius;\n"
        "varying float vtexwidth;\n"
        "varying float vtexheight;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec2 ltc = vec2((vTextureCoord.x-0.5)*radius+centerx+0.5,(vTextureCoord.y-0.5)*radius*(vtexwidth/vtexheight)+0.5+centery);"
        "    if(ltc.y>1.0||ltc.y<0.0)\n"
        "       gl_FragColor=vec4(0.0,0.0,0.0,0.0);\n"
        "    else\n"
        "    {\n"
        "       vec4 acolor=texture2D(sTexture,ltc);\n"
        "           gl_FragColor=acolor;\n"
        "    }\n"
        "}";

const char *YUV_VertexObjectFrag = "precision mediump float;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "varying vec2 vTextCoord;\n"
        " uniform sampler2D s_texture_y;\n"
        " uniform sampler2D s_texture_cbcr;\n"
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




VertexObject::VertexObject(float radius,float aspect,void *mgn) : m_radius(radius)
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
    ParametricInterval interval = { 6, vec2(6,186), vec2(366, 276) };
    SetInterval(interval);
#ifdef __USE_RGB32__
    setupProgram(VertexObjectSeg,VertexObjectFrag);
#else
    setupProgram(VertexObjectSeg,YUV_VertexObjectFrag,true);
#endif
	
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

VertexObject::~VertexObject() {
    for(int i=0;i<VERTEX_TYPE_COUNT;i++)
    {
        if(mVertexVBOBuffer[i]!=0) {
            glDeleteBuffers(1, &mVertexVBOBuffer[i]);
            LOGDEBUG("2---glDeleteBuffers:%d %p",mVertexVBOBuffer[i],this);
            mVertexVBOBuffer[i] = 0;
        }
        if(mTextureCoordBuffer[i]!=0) {
            glDeleteBuffers(1, &mTextureCoordBuffer[i]);
            LOGDEBUG("3---glDeleteBuffers:%d %p",mTextureCoordBuffer[i],this);

            mTextureCoordBuffer[i] = 0;
        }
    }
    delete mDrawSelected;
}

void VertexObject::ResetPosition() {
    ParametricSurface::ResetPosition();
    //setRotate(vec3(-30,0,0),false,0);
    _Zoom = -3;
}


void VertexObject::SetupBuffer() {

    for (int i = 0; i < VERTEX_TYPE_COUNT; ++i) {
        mVertexType = i;
        int lvs = GetVertexSize();
        int vBufSize = GetVertexCount() * lvs;
        int vCount = GetVertexCount();
        GLfloat *vbuf = (GLfloat *)malloc(vBufSize*sizeof(GLfloat));//new GLfloat[vBufSize];

        int CoordSize = GetCoordSize();
        int vCoorBufSize = GetCoordCount() * CoordSize;
        GLfloat *vcoordbuf = (GLfloat *)malloc(vCoorBufSize*sizeof(GLfloat));//new GLfloat[vCoorBufSize];

        GenerateVertices(vbuf, vcoordbuf);

        GLuint lvb = 0;
        //printf("\n\n");
        glGenBuffers(1, &lvb);
        LOGDEBUG("2--glGenBuffers:%d",lvb);

        //printf("gl error number:%d\n\n",glGetError());
        glBindBuffer(GL_ARRAY_BUFFER, lvb);
        glBufferData(GL_ARRAY_BUFFER, vBufSize * sizeof(GLfloat), vbuf, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //printf("set vexture buffer:%d\n",glGetError());
        GLuint CoordBuffer = 0;
        glGenBuffers(1, &CoordBuffer);
        LOGDEBUG("3--glGenBuffers:%d",CoordBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, CoordBuffer);
        glBufferData(GL_ARRAY_BUFFER, vCoorBufSize * sizeof(GLfloat), vcoordbuf, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        printf("set Coord buffer:%d i:%d\n",glGetError(),i);

        //delete[] vbuf;
        //delete[] vcoordbuf;
        free(vcoordbuf);
        free(vbuf);

        if(mVertexVBOBuffer[i])
            glDeleteBuffers(1,&mVertexVBOBuffer[i]);
        mVertexVBOBuffer[i] = lvb;
        mVertexSize[i] = lvs;
        mTriangleIndexCount[i] = vCount;

        mTextureCoordSize[i] = CoordSize;
        if(mTextureCoordBuffer[i])
            glDeleteBuffers(1,&mTextureCoordBuffer[i]);
        mTextureCoordBuffer[i] = CoordBuffer;

//        mVertexVBOBuffer[i] = lvb;
//        mVertexSize[i] = lvs;
//        mTriangleIndexCount[i] = vCount;
//        mTextureCoordSize[i] = CoordSize;
//        mTextureCoordBuffer[i] = CoordBuffer;
    }
    mVertexType = VERTEX_TYPE_PANORAMA;
    //printf("vCount:%d\n",vCount);
}


void VertexObject::setupProjection() {
	ParametricManager *mgn = (ParametricManager *)_Manager;
	float ViewAngle = mgn->GetViewAngle();
    ksMatrixLoadIdentity((KSMatrix4 *)&_projectionMatrix);
    if(mVertexType!=VERTEX_TYPE_CYLINDER) {
        _Zoom = -3;
        ksPerspective((KSMatrix4 *) &_projectionMatrix, ViewAngle, m_aspect, 0.0001f, 1200.0f);
        //printf("aaaaaaa\n");
    }
    else {
        _Zoom = 1;
        //ksPerspective((KSMatrix4 *)&_projectionMatrix, 30.0, m_aspect, 0.0001f, 11.0f);
        ksOrtho(&_projectionMatrix,-m_aspect,m_aspect,-1,1,0.0001f,1200);
        //printf("bbbbbbbb");
    }

    glUseProgram(_programHandle);
    // Load projection matrix
    glUniformMatrix4fv(_projectionSlot, 1, GL_FALSE, (GLfloat *) &_projectionMatrix.m[0][0]);
    //printf("glUniformMatrix4fv0:%d\n",glGetError());
    ksMatrixLoadIdentity(&_rotationMatrix);
    //ksRotate(&_rotationMatrix,180.0f,0,1,0);
    //glEnable(GL_LINE_SMOOTH);
}


vec3 VertexObject::Evaluate(const vec2& domain)
{
    float u = domain.x, v = domain.y;
    float x=0;
    float y=0;
    float z=0;
    float rx,ry;

    switch(mVertexType) {
        case VERTEX_TYPE_HEMISPHERE:
            //全景贴图模式
            x = m_radius * cos(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
            y = m_radius * sin(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
            z = m_radius * sin(v / 180.0f * Pi);
            break;
        case VERTEX_TYPE_PANORAMA:
            // 全景展开模式
            x = 1.0*((180.0f-u) / 180.0f);
            if(v>250)
                y=(-35.0/180.0f)*6;
            else
                y = ((215 - v) / 180.0f)*6;
            z = 1;//* sin(v/180.0f*Pi);
            break;
        case VERTEX_TYPE_CYLINDER:
            // 圆柱体模式
            x = (u / 180.0f*Pi);
            y = v;
            z = 1;
//            x = cosf((u) / 180.0 * Pi)*.8;
//            if(v>240.0)
//                y = -30.0/180.0f * Pi * 1.2;
//            else
//                y = (210 - v) / 180.0f * Pi*1.2;
//            z = sinf((u) / 180.0 * Pi)*.8;
            break;
        case VERTEX_TYPE_EXPAND:
            //四角展开模式
            x = 2 * cos(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
            y = 2 * sin(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
            z = 2 * sin((v - 180) / 180.0f * Pi);
            //return vec3(x,y,z);
			if (z == 0.0)
				z = 0.0000001;
            rx = 2 * atanf(x / z) / Pi ;
            ry = -sinf(u / 180.0f * Pi) * sinf((270 - v) / 180.0 * Pi);
            x = rx;
            y = ry;
            z = 1.265;
            break;
        case VERTEX_TYPE_NORMAL:
            //普通模式
            x = (u-180.0f)/360.0f * 2;
            y = (v-225.0f)/90.0f*2;
            z = 1.265f;
            break;
        case VERTEX_TYPE_SPHERE:
            x = cos(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
            y = sin(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
            z = sin((v - 180) / 180.0f * Pi);
            //return vec3(x,y,z);
			if (z == 0.0)
				z = 0.0000001;
			rx = 2*atanf(x / z) / Pi ;
            ry = -sinf(u / 180.0f * Pi) * sinf((270 - v) / 180.0 * Pi);
            x = rx;
            y = ry;
            z = 1.265;
            break;
    }
    return vec3(x, y, z);
}

vec2 VertexObject::EvaluateCoord(const vec2& domain)
{
    float u = domain.x, v = domain.y;
    vec2 vTextureCoord = vec2(0,0);
    if(mVertexType==VERTEX_TYPE_NORMAL)
    {
        vTextureCoord = vec2(u/360.0f,(270.0f-v)/90.0f);
    }
    else {
        float x = m_radius * cos(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
        float y = m_radius * sin(u / 180.0f * Pi) * cos(v / 180.0f * Pi);
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
        vTextureCoord = px * py * (float) ((1.0 / powf(2.0, 0.5)) * 0.5);
        //    vTextureCoord.y = vTextureCoord.y*1.35;
        vTextureCoord.x = vTextureCoord.x + 0.5f;
        vTextureCoord.y = vTextureCoord.y + 0.5f;
        vTextureCoord.y = (1.0f - vTextureCoord.y);
        //printf("x:%f,y:%f\n",vTextureCoord.x,vTextureCoord.y);
    }
    return vTextureCoord;
}

void VertexObject::DrawSelf(int scnindex)
{
#ifdef __ANDROID__
	if ((_texwidth != direct_width || _texheight != direct_height) && isUseDirectTexture){
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
    //LOGV("---------------------------------------------------DrawSelf-------------------------------VertexObject::Drawself  -----> %f -- %f -- %f -   mVisible---->%d",mRadius,mCenterX,mCenterY,Visible);
    if(!Visible)
        return ;
    ParametricManager *mgn = (ParametricManager *)_Manager;
    float tw=mgn->GetTextureWidth();
    float th=mgn->GetTextureHeight();

    if(mRadius<=0.0) {
        mCenterX = 0;
        mCenterY = 0;
        if(th==720.0)
        {
            mRadius = 0.9;
        }
        else if(th==1088.0||th==1080.0)
        {
            mRadius = 0.85;
        }
        else
        {
            mRadius = 0.95;
        }
    }
    //printf("NeedDraw.....%d\n",mNeedDraw);
    glUseProgram(_programHandle);

    glUniform1f(mSegTexWidthHandle,tw);
    glUniform1f(mSegTexHeightHandle,th);

	float viewangle = mgn->GetViewAngle();
	if (mVertexType == VERTEX_TYPE_HEMISPHERE || mVertexType == VERTEX_TYPE_SPHERE)
		updateSurfaceTransform(&_projectionMatrix, &_modelViewMatrix, &_rotationMatrix, _Zoom*tanf(30.0/180.0f*Pi)/tanf(viewangle/2/180.0f*Pi), vec3(mPosition.x,mPosition.y,mPosition.z*tanf(30.0/180.0*Pi) / tanf(viewangle/ 2 / 180.0f*Pi)), mScale, mRotate, _projectionSlot, _modelViewSlot, mVertexType);
	else
		updateSurfaceTransform(&_projectionMatrix,&_modelViewMatrix,&_rotationMatrix,_Zoom,mPosition,mScale,mRotate,_projectionSlot,_modelViewSlot,mVertexType);

    glUniform1i(mSegModeHandle,mVertexType);

    glUniform1f(mSegRotateHandle,mTexPosition.x);
    glUniform1f(mSegDiameterHandle,mTexScale.x);
    glUniform1f(mSegAspectHandle,m_aspect);
    glUniform1f(mSegViewportAspectHandle,mViewportAspect);
    glUniform1f(mFragCenterXHandle,mCenterX);
    glUniform1f(mFragCenterYHandle,mCenterY);
    glUniform1f(mFragRadiusHandle,mRadius);

    glUniform1i(mSegChangeAnimationHandle,mChangeAnimation?1:0);
    if(mChangeAnimation)
    {
        glUniform1f(mSegChangeRotateHandle,nTexPosition.x);
        glUniform1f(mSegChangeDiameterHandle,nTexScale.x);
        glUniform1f(mSegChangeStepCountHandle,mChangeStepCount);
        glUniform1f(mSegChangeStepHandle,mChangeStep);
        glUniform1i(mSegChangeModeHandle,mChangeType);
        float lzoom;
        if(mChangeType!=VERTEX_TYPE_CYLINDER) {
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
        updateSurfaceTransform(&mChangeprojectionMatrix,&mChangemodelViewMatrix,&mChangeRoateMatrix,lzoom,nPosition,nScale,nRotate,mSegChangeProjHandle,mSegChangeModelHandle,mChangeType);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexVBOBuffer[mChangeType]);
        glVertexAttribPointer(mSegChangePositionHandle, 3, GL_FLOAT, GL_FALSE, mVertexSize[mChangeType] * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(mSegChangePositionHandle);
    }

    //MB_INFO("texwidth:%f texheight:%f\n",tw,th);

    glBindBuffer(GL_ARRAY_BUFFER, mVertexVBOBuffer[mVertexType]);
    glVertexAttribPointer(_positionSlot, 3, GL_FLOAT, GL_FALSE, mVertexSize[mVertexType] * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(_positionSlot);

    //printf("glEnableVertexAttribArray:%d\n",glGetError());

    glBindBuffer(GL_ARRAY_BUFFER,mTextureCoordBuffer[mVertexType]);
    glVertexAttribPointer(_coordSlot, 2, GL_FLOAT, GL_FALSE, mTextureCoordSize[mVertexType] * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(_coordSlot);


    if(mIsYUV)
    {
        for (int i = 0; i < 2; ++i) {
//			printf("texture......%d\n",_yuvtexid[i]);
            glActiveTexture(GL_TEXTURE0 + i);
            if(_yuvtexid[0])
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
	//glDrawArrays(GL_LINES, 0, triangleIndexCount);


    glBindTexture(GL_TEXTURE_2D, 0);
	if (mShowGrid) {
#ifdef __ANDROID__
		glLineWidth(3.0f);
#endif // __ANDROID__
		glDrawArrays(GL_LINES, 0, mTriangleIndexCount[mVertexType]);
	}
    glDisableVertexAttribArray(_positionSlot);
    glDisableVertexAttribArray(_coordSlot);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    if(scnindex==0x7fffffff) {
        mDrawSelected->DrawSelf(0);
    }
    mNeedDraw = false;
}

void VertexObject::SwitchMode(int mode) {
    if(mVertexType!=mode) {
        //printf("Switchmode .................... start\n");
        mVertexType = mode;
        this->setupProjection();
        //printf("Switchmode ....................\n");
    }
}

void VertexObject::setupProgram(const char *segsrc,const char *fragsrc)
{
    ParametricSurface::setupProgram(segsrc,fragsrc);
	eTB_GLSL__print_uniforms(_programHandle);
    mSegModeHandle = glGetUniformLocation(_programHandle,"aMode");
    mSegRotateHandle = glGetUniformLocation(_programHandle,"aRotate");
    mSegDiameterHandle = glGetUniformLocation(_programHandle,"aDiameter");
    mSegTexWidthHandle = glGetUniformLocation(_programHandle,"texwidth");
    mSegTexHeightHandle = glGetUniformLocation(_programHandle,"texheight");
    mSegAspectHandle = glGetUniformLocation(_programHandle,"aAspect");
    mSegChangeAnimationHandle = glGetUniformLocation(_programHandle,"aChangeAnimation");
    mSegChangeStepHandle = glGetUniformLocation(_programHandle,"aChangeStep");
    mSegChangePositionHandle = glGetAttribLocation(_programHandle,"vChangePosition");
    mSegChangeStepCountHandle = glGetUniformLocation(_programHandle,"aChangeStepCount");
    mSegChangeProjHandle = glGetUniformLocation(_programHandle,"changeprojection");
    mSegChangeModelHandle = glGetUniformLocation(_programHandle,"changemodelView");
    mSegChangeModeHandle = glGetUniformLocation(_programHandle,"aChangeMode");
    mFragTexture = glGetUniformLocation(_programHandle,"sTexture");
    mFragTexText = glGetUniformLocation(_programHandle,"sTexText");
    mSegChangeDiameterHandle = glGetUniformLocation(_programHandle,"aChangeDiameter");
    mSegChangeRotateHandle = glGetUniformLocation(_programHandle,"aChangeRotate");
    mSegViewportAspectHandle = glGetUniformLocation(_programHandle,"aViewportAspect");
    mFragCenterXHandle = glGetUniformLocation(_programHandle,"centerx");
    mFragCenterYHandle = glGetUniformLocation(_programHandle,"centery");
    mFragRadiusHandle = glGetUniformLocation(_programHandle,"radius");
}

void VertexObject::setupProgram(const char *segsrc,const char *fragsrc,bool yuv)
{
    ParametricSurface::setupProgram(segsrc,fragsrc,yuv);
    mSegModeHandle = glGetUniformLocation(_programHandle,"aMode");
    mSegRotateHandle = glGetUniformLocation(_programHandle,"aRotate");
    mSegDiameterHandle = glGetUniformLocation(_programHandle,"aDiameter");
    mSegTexWidthHandle = glGetUniformLocation(_programHandle,"texwidth");
    mSegTexHeightHandle = glGetUniformLocation(_programHandle,"texheight");
    mSegAspectHandle = glGetUniformLocation(_programHandle,"aAspect");
    mSegChangeAnimationHandle = glGetUniformLocation(_programHandle,"aChangeAnimation");
    mSegChangeStepHandle = glGetUniformLocation(_programHandle,"aChangeStep");
    mSegChangePositionHandle = glGetAttribLocation(_programHandle,"vChangePosition");
    mSegChangeStepCountHandle = glGetUniformLocation(_programHandle,"aChangeStepCount");
    mSegChangeProjHandle = glGetUniformLocation(_programHandle,"changeprojection");
    mSegChangeModelHandle = glGetUniformLocation(_programHandle,"changemodelView");
    mSegChangeModeHandle = glGetUniformLocation(_programHandle,"aChangeMode");
    mSegChangeDiameterHandle = glGetUniformLocation(_programHandle,"aChangeDiameter");
    mSegChangeRotateHandle = glGetUniformLocation(_programHandle,"aChangeRotate");
    mSegViewportAspectHandle = glGetUniformLocation(_programHandle,"aViewportAspect");
    mFragCenterXHandle = glGetUniformLocation(_programHandle,"centerx");
    mFragCenterYHandle = glGetUniformLocation(_programHandle,"centery");
    mFragRadiusHandle = glGetUniformLocation(_programHandle,"radius");
}

void VertexObject::updateSurfaceTransform(KSMatrix4 *_proj, KSMatrix4 *_model,KSMatrix4 *_rotate,float zoom,vec3 pPos,vec3 pScale,vec3 pRotate,GLuint projslot,GLuint modelslot,int atype){
    ksMatrixLoadIdentity(_model);
    ksTranslate(_model, 0.0, 0.0, zoom);

    ksMatrixLoadIdentity(_rotate);
    KSMatrix4 lrotate,ltmprotate;
    ksMatrixLoadIdentity(&lrotate);
    if(pRotate.x) {
        ksRotate(&lrotate, pRotate.x, 1, 0, 0);
    }
    if(pRotate.y) {
        ksRotate(&lrotate, pRotate.y, 0, 1, 0);
    }
    if(pRotate.z) {
        ksRotate(&lrotate, pRotate.z, 0, 0, 1);
    }
    ksMatrixMultiply(_rotate,_rotate,&lrotate);

    ksTranslate(_model,pPos.x,pPos.y,pPos.z);
    //使用另外一种模式来放大
	ParametricManager *mgn = (ParametricManager *)_Manager;
	float ViewAngle = mgn->GetViewAngle();
	if(atype!=VERTEX_TYPE_CYLINDER) {
        if(atype==VERTEX_TYPE_HEMISPHERE||atype==VERTEX_TYPE_SPHERE) {
            ksMatrixLoadIdentity((KSMatrix4 *) _proj);
            ksPerspective((KSMatrix4 *) _proj, ViewAngle / pScale.x, m_aspect, 0.0001f, 1200.0f);
            glUniformMatrix4fv(projslot, 1, GL_FALSE, (GLfloat *) &_proj->m[0][0]);
        }
        else
        {
            ksMatrixLoadIdentity((KSMatrix4 *) _proj);
            ksOrtho(_proj,-m_aspect,m_aspect,-1,1,0.0001f,1200);
            glUniformMatrix4fv(projslot, 1, GL_FALSE, (GLfloat *) &_proj->m[0][0]);
            ksScale(_rotate, pScale.x, pScale.y, pScale.z);
        }
    }
    else {
        ksMatrixLoadIdentity((KSMatrix4 *) _proj);
        ksOrtho(_proj,-m_aspect,m_aspect,-1,1,0.0001f,1200);
        glUniformMatrix4fv(projslot, 1, GL_FALSE, (GLfloat *) &_proj->m[0][0]);
        ksScale(_rotate, pScale.x, pScale.y, pScale.z);
    }

    ///////////

    ksMatrixMultiply(_model, _rotate, _model);
    glUniformMatrix4fv(modelslot, 1, GL_FALSE, (GLfloat *) &_model->m[0][0]);
}

void VertexObject::updateSurfaceTransform() {
    ksMatrixLoadIdentity(&_modelViewMatrix);
    ksTranslate(&_modelViewMatrix, 0.0, 0.0, _Zoom);

    ksMatrixLoadIdentity(&_rotationMatrix);
    KSMatrix4 lrotate,ltmprotate;
    ksMatrixLoadIdentity(&lrotate);
    if(mRotate.x) {
        ksRotate(&lrotate, mRotate.x, 1, 0, 0);
    }
    if(mRotate.y) {
        ksRotate(&lrotate, mRotate.y, 0, 1, 0);
    }
    if(mRotate.z) {
        ksRotate(&lrotate, mRotate.z, 0, 0, 1);
    }
    ksMatrixMultiply(&_rotationMatrix,&_rotationMatrix,&lrotate);

    ksTranslate(&_modelViewMatrix,mPosition.x,mPosition.y,mPosition.z);
    //使用另外一种模式来放大
	ParametricManager *mgn = (ParametricManager *)_Manager;
	float ViewAngle = mgn->GetViewAngle();
	if(mVertexType!=VERTEX_TYPE_CYLINDER) {
        ksMatrixLoadIdentity((KSMatrix4 *) &_projectionMatrix);
        ksPerspective((KSMatrix4 *) &_projectionMatrix, ViewAngle / mScale.x, m_aspect, 0.0001f, 1200.0f);
        glUniformMatrix4fv(_projectionSlot, 1, GL_FALSE, (GLfloat *) &_projectionMatrix.m[0][0]);
    }
    else {
        ksMatrixLoadIdentity((KSMatrix4 *) &_projectionMatrix);
        ksOrtho(&_projectionMatrix,-m_aspect,m_aspect,-1,1,0.0001f,1200);
        glUniformMatrix4fv(_projectionSlot, 1, GL_FALSE, (GLfloat *) &_projectionMatrix.m[0][0]);
        ksScale(&_rotationMatrix, mScale.x, mScale.y, mScale.z);
    }


    ///////////

    ksMatrixMultiply(&_modelViewMatrix, &_rotationMatrix, &_modelViewMatrix);
    glUniformMatrix4fv(_modelViewSlot, 1, GL_FALSE, (GLfloat *) &_modelViewMatrix.m[0][0]);
    //printf("glUniformMatrix4fv:%d\n",glGetError());
}


void VertexObject::SetChangeAnimation(bool Start,int step,int stepcount,int nexttype)
{
    mChangeAnimation = Start;
    mChangeStep = step;
    mChangeType = nexttype;
    mChangeStepCount = stepcount;
}

void VertexObject::SetChangePSR(vec3 pos,vec3 scale,vec3 rotate,vec3 tpos,vec3 tscale,vec3 trotate)
{
    nPosition = pos;
    nScale = scale;
    nRotate = rotate;
    nTexPosition = tpos;
    nTexScale = tscale;
    nTexRotate = trotate;
}

void VertexObject::DirectTextureFrameUpdata(int w, int h, void * directBuffer, int index)
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

void VertexObject::CreateDirectTexture()
{
#ifdef __ANDROID__
	JACONNECT_INFO("the direct texture is come to???");
	if (!_texid) {
		GLuint texture_object_id = 0;
		glGenTextures(1, &texture_object_id);
//        LOGDEBUG("2--glGenTextures:%d",texture_object_id);

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


void VertexObject::SetDistortion(PDISTORTION_NUM pdist,int dist_count,float half)
{
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


int VertexObject::GetCurrentDistortion(PDISTORTION_NUM dist)
{
    for(int i=0;i<mDistList.length();i++)
        if(mDistList.at(i)->mdistortion==dist)
            return i;
    return -1;
}

void VertexObject::SetFishEyeParameter(float centerx, float centery, float radius)
{
    mCenterX = centerx;
    mCenterY = centery;
    mRadius = radius;
}
