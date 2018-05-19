//
// Created by LiHong on 16/1/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include <stdio.h>
#include "Cylinder.h"

const char *CylinderSeg = "#define PI 3.1415926535897932384626433832795\n"
        "\n"
        "uniform mat4 projection;\n"
        "uniform mat4 modelView;\n"
        "attribute vec3 vPosition;\n"
        "attribute vec3 vReference;\n"
        "attribute vec2 aTexCoor;\n"
        "varying vec2 vTextureCoord;\n"
        "uniform float aTextureOffset;\n"
        "uniform float aDiameterChange;\n"
        "float aDiameterChange1;\n"
        "float thita;\n"
        "\n"
        "void main(){\n"
        "    vec3 lposition = vPosition.xyz;\n"
        "    float aDiameter1 = length(lposition.xz);\n"
        "    if(aDiameterChange<aDiameter1)\n"
        "        aDiameterChange1 = aDiameter1;\n"
        "    else\n"
        "        aDiameterChange1 = aDiameterChange;\n"
        "    float thita1=asin(abs(lposition.z/aDiameter1));\n"
        "    float thita2=PI/2.0;\n"
        "    float lx = lposition.x;\n"
        "    float lz = lposition.z;\n"
        "    if(lz<0.0)\n"
        "        thita = (thita1+thita2)*aDiameter1/(aDiameterChange1);\n"
        "    else\n"
        "        thita = (thita2-thita1)*aDiameter1/(aDiameterChange1);\n"
        "\n"
        "    if(thita<PI/2.0)\n"
        "    {\n"
        "        if(vReference.x>0.1)\n"
        "            lposition.x = sin(thita)*aDiameterChange1;\n"
        "        else if(vReference.x<-0.1)\n"
        "            lposition.x = -sin(thita)*aDiameterChange1;\n"
        "        else if(lx<0.0)\n"
        "            lposition.x = -sin(thita)*aDiameterChange1;\n"
        "        else if(lx>0.0)\n"
        "            lposition.x = sin(thita)*aDiameterChange1;\n"
        "        lposition.z = -cos(thita)*aDiameterChange1;\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        if(vReference.x>0.1)\n"
        "            lposition.x = cos(thita-PI/2.0)*aDiameterChange1;\n"
        "        else if(vReference.x<-0.1)\n"
        "            lposition.x = -cos(thita-PI/2.0)*aDiameterChange1;\n"
        "        else if(lx<0.0)\n"
        "            lposition.x = -cos(thita-PI/2.0)*aDiameterChange1;\n"
        "        else if(lx>0.0)\n"
        "            lposition.x = cos(thita-PI/2.0)*aDiameterChange1;\n"
        "        lposition.z = sin(thita-PI/2.0)*aDiameterChange1;\n"
        "    }\n"
        "\n"
        "    lposition.z = lposition.z+(aDiameterChange1-aDiameter1);\n"
        "    //lposition.x = sign(lposition.x) * sqrt(length(lposition.xz)*length(lposition.xz)-(lposition.z*lposition.z));\n"
        "    gl_Position=projection*modelView*vec4(lposition,1.0);\n"
        "    vec2 cp = aTexCoor;\n"
        "    cp.x = cp.x-0.5;\n"
        "    cp.y = cp.y-0.5;\n"
        "    float c = length(cp);\n"
        "    float la = asin(abs(cp.y)/c);\n"
        "    lx=cp.x;\n"
        "    float ly=cp.y;\n"
        "    if(lx<0.0)\n"
        "    {\n"
        "        if(ly<0.0)\n"
        "            la=PI-la;\n"
        "        else\n"
        "            la=la+PI;\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        if(ly>0.0)\n"
        "            la=2.0*PI-la;\n"
        "    }\n"
		"    cp.x =1.0 - (c * sin(aTextureOffset+la)+0.5);\n"
        "    cp.y = c * cos(aTextureOffset+la)+0.5;\n"
        "    vTextureCoord=cp;\n"
        "\n"
        "    //vTextureCoord.x = c * cos(aDiameter);\n"
        "    //vTextureCoord.y = c * sin(aDiameter);\n"
        "}";
const char *CylinderFrag = "precision mediump float;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        "uniform sampler2D sTexture;\n"
        "\n"
        "void main(){\n"
        "    gl_FragColor=texture2D(sTexture,vTextureCoord);\n"
        "    //gl_FragColor=vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}";

const char *YUV_CylinderFrag = "precision mediump float;\n"
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


Cylinder::Cylinder(float radius,float aspect,void *mgn) : m_radius(radius)
{
    m_aspect = aspect;
    _Manager = mgn;
    //mTextureOffset = 0.0f;
    _Zoom = -6;
    mExpandDiameter = radius;
    mReferenceBuffer = 0;
    ParametricInterval interval = { 3.0f, vec2(-87,-9), vec2(273, 27) };
    SetInterval(interval);
#ifdef __USE_RGB32__
    setupProgram(CylinderSeg,CylinderFrag);
#else
    setupProgram(CylinderSeg,YUV_CylinderFrag,true);
#endif
    setupProjection();
    CylinderSetupBuffer();
    //ksRotate(&_rotationMatrix, 30.0f, 1, 0, 0);
    mScreenMode = SCRN_CYLINDER;
    setRotate(vec3(30.0f,0,0),false,0);
    SetPosition(vec3(0,0,-6),false,0);
}

vec3 Cylinder::Evaluate(const vec2& domain)
{
    float u = domain.x, v = domain.y;
    float x = m_radius * cos(u/180.0f*Pi);
    float y = m_radius * tan((v-6)/180.0f*Pi)*m_aspect*1.7;
    float z = m_radius * sin(u/180.0f*Pi);
    //printf("x:%f,y:%f,z:%f\n",x,y,z);
    if(mxStart)
    {
        vec3 range = vec3(1,1,1);
        fReferencebufadd = range.Write(fReferencebufadd);
    }
    else if(mxEnd)
    {
        vec3 range = vec3(-1,-1,-1);
        fReferencebufadd = range.Write(fReferencebufadd);
    }
    else
    {
        vec3 range = vec3(0,0,0);
        fReferencebufadd = range.Write(fReferencebufadd);
    }
    return vec3(x, y, z);
}

vec2 Cylinder::EvaluateCoord(const vec2& domain)
{
    float W = (360.0f/anglespan);
    float H = (45.0f/anglespan);
    float cx =(domain.x/anglespan+W/4);
    float cy =(domain.y/anglespan+H/2);

    float temp = ( 2 * Pi )/W;
    float theta =(( 2 * Pi ) - cx * temp);
    //theta = theta * PI / 180;
    int r = cy;
    float temp1 = (float) ( r * cos( theta ) );
    float temp2 = (float) ( r * sin( theta ) );
    //printf("   %d", r );
    return vec2((temp1 + H)/(H*2),((H*2 - ( temp2 + H ))/(H*2)));
}

void Cylinder::CylinderSetupBuffer()
{
    int lvs = GetVertexSize();
    int vBufSize = GetVertexCount() * lvs;
    fReferencebuf = new GLfloat[vBufSize];
    fReferencebufadd = fReferencebuf;
    SetupBuffer();
    glGenBuffers(1, &mReferenceBuffer);
    LOGDEBUG("13--glGenBuffers:%d",mReferenceBuffer);
    if(mReferenceBuffer)
        glDeleteBuffers(1,&mReferenceBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mReferenceBuffer);
    glBufferData(GL_ARRAY_BUFFER, vBufSize * sizeof(GLfloat), fReferencebuf, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void Cylinder::setupProjection() {
    _Zoom = 1;
    ksMatrixLoadIdentity((KSMatrix4 *)&_projectionMatrix);
    //ksPerspective((KSMatrix4 *)&_projectionMatrix, 30.0, m_aspect, 0.0001f, 11.0f);
    ksOrtho(&_projectionMatrix,-m_aspect,m_aspect,-1,1,3,1200);
    //ksLookAt()

    // Load projection matrix
    glUniformMatrix4fv(_projectionSlot, 1, GL_FALSE, (GLfloat *) &_projectionMatrix.m[0][0]);
    //printf("glUniformMatrix4fv0:%d\n",glGetError());
    ksMatrixLoadIdentity(&_rotationMatrix);
    //ksRotate(&_rotationMatrix,180.0f,0,1,0);
    //glEnable(GL_LINE_SMOOTH);
}

void Cylinder::setupProgram(const char *segsrc,const char *fragsrc)
{
    ParametricSurface::setupProgram(segsrc,fragsrc);
    mReferenceHandle = glGetAttribLocation(_programHandle,"vReference");
    //printf("value:%d,mReferenceHandle:%d\n",mReferenceHandle,glGetError());
    mDiameterChangeHandle = glGetUniformLocation(_programHandle,"aDiameterChange");
    //printf("value:%d,mDiameterChangeHandle:%d\n",mDiameterChangeHandle,glGetError());
    mTextureOffsetHandle = glGetUniformLocation(_programHandle,"aTextureOffset");
    //printf("value:%d,mTextureOffsetHandle:%d\n",mTextureOffsetHandle,glGetError());
}

void Cylinder::setupProgram(const char *segsrc,const char *fragsrc,bool yuv)
{
    ParametricSurface::setupProgram(segsrc,fragsrc,yuv);
    mReferenceHandle = glGetAttribLocation(_programHandle,"vReference");
    //printf("value:%d,mReferenceHandle:%d\n",mReferenceHandle,glGetError());
    mDiameterChangeHandle = glGetUniformLocation(_programHandle,"aDiameterChange");
    //printf("value:%d,mDiameterChangeHandle:%d\n",mDiameterChangeHandle,glGetError());
    mTextureOffsetHandle = glGetUniformLocation(_programHandle,"aTextureOffset");
    //printf("value:%d,mTextureOffsetHandle:%d\n",mTextureOffsetHandle,glGetError());
}


void Cylinder::updateSurfaceTransform() {
//    ksMatrixLoadIdentity(&_modelViewMatrix);
//
//    ksTranslate(&_modelViewMatrix, 0.0, 0.0, -6);
//    //ksScale(&_modelViewMatrix,_Zoom,_Zoom,_Zoom);
//    //ksRotate(&_modelViewMatrix,30.0f,0.0f,1.0f,0.0f);
//    ksMatrixMultiply(&_modelViewMatrix, &_rotationMatrix, &_modelViewMatrix);
//    glUniformMatrix4fv(_modelViewSlot, 1, GL_FALSE, (GLfloat *) &_modelViewMatrix.m[0][0]);
//    glGetError();

    ParametricSurface::updateSurfaceTransform();

    glBindBuffer(GL_ARRAY_BUFFER, mReferenceBuffer);
    glVertexAttribPointer(mReferenceHandle, 3, GL_FLOAT, GL_FALSE, vertexSize * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(mReferenceHandle);
    //printf("glEnableVertexAttribArray-vReference:%d\n",glGetError());
}

void Cylinder::DrawSelf(int scnindex)
{
    glUseProgram(_programHandle);
    glUniform1f(mTextureOffsetHandle, mTexPosition.x);
    glUniform1f(mDiameterChangeHandle,mTexScale.x*m_radius);
    BaseScreen::DrawSelf(0);
}


void Cylinder::TurnLeftRight(GLfloat angle,int scnindex)
{
    //ksRotate(&_rotationMatrix, angle, 0, 1, 0);
    mTextureOffset-=angle*6;
}
void Cylinder::TurnUpDown(GLfloat angle,int scnindex)
{
    //ksRotate(&_rotationMatrix, angle, 1, 0, 0);
    if(mExpandDiameter+angle*2<m_radius)
        mExpandDiameter = m_radius;
    else
        mExpandDiameter += angle*2;
    ksRotate(&_rotationMatrix, -angle*2*50, 1, 0, 0);
    _Zoom+=angle*3;
}

void Cylinder::ResetPosition()
{
    ParametricSurface::ResetPosition();
    mExpandDiameter = m_radius;
    mTextureOffset = 0;
    setRotate(vec3(27.0f,0,0),false,0);
    SetPosition(vec3(0,0,-6),false,0);
}

void Cylinder::setZoom(GLfloat multi,int scnindex)
{
    //_Zoom = multi-4;
}