//
// Created by LiHong on 16/4/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "VR.h"

void VR::SetNoVideoTexture(GLuint texid)
{
    for(int i=0;i<2;i++)
        mVideo[i]->SetNoVideoTexture(texid);
}

VR::~VR() {
    for (int i = 0; i < 2; i++) {
        delete mVideo[i];
    }
}

VR::VR(float aspect,void *mgn)
{
    for(int i=0;i<2;i++) {
        mVideo[i] = new Hemisphere(1.0f, aspect/2, mgn);
        mVideo[i]->SetScale(vec3(1.5, 1.5, 1.5), false, 0);
        mVideo[i]->setRotate(vec3(60,0,0), false, 0);
    }
}

int VR::GetScreenMode() {
    return SCRN_VR;
}

int VR::GetScreenCount() {
    return 2;
}

void VR::TurnLeftRight(GLfloat angle,int scnindex)
{
}

void VR::TurnUpDown(GLfloat angle,int scnindex)
{

}

void VR::setZoom(GLfloat multi,int scnindex)
{

}

void VR::DrawSelf(int scnindex)
{
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT,vp);
    int w = vp[2]/2;
    int h = vp[3];
    for(int i=0;i<2;i++)
    {
        glViewport((i%2)*w,(i/2)*h,w,h);
        mVideo[i]->DrawSelf(0);
    }
//    glViewport(w*3/4,h*3/4,w/2,h/2);
//    mNormal->DrawSelf(0);
}

void VR::ResetPosition() {

}

void VR::SetPosition(vec3 pos, bool texture,int scnindex)
{
    if(scnindex<0||scnindex>3)
        return;
    mVideo[scnindex]->SetPosition(pos,texture,0);
}

void VR::SetScale(vec3 scale, bool texture,int scnindex)
{
    if(scnindex<0||scnindex>3)
        return;
    mVideo[scnindex]->SetScale(scale,texture,0);
}

void VR::setRotate(vec3 rotate, bool texture,int scnindex)
{
    if(scnindex<0||scnindex>3)
        return;
    mVideo[scnindex]->setRotate(rotate,texture,0);
}

vec3 VR::GetPosition(bool texture,int scnindex)
{
    if(scnindex<0||scnindex>3)
        return vec3(0,0,0);
    return mVideo[scnindex]->GetPosition(texture,0);
}

vec3 VR::GetScale(bool texture,int scnindex)
{
    if(scnindex<0||scnindex>3)
        return vec3(0,0,0);
    return mVideo[scnindex]->GetScale(texture,0);
}

vec3 VR::GetRotate(bool texture,int scnindex)
{
    if(scnindex<0||scnindex>3)
        return vec3(0,0,0);
    return mVideo[scnindex]->GetRotate(texture,0);
}


bool VR::LoadTexture(int w,int h,GLenum type,GLvoid *pixels,int scnindex)
{
    bool ret = false;
    for(int i=0;i<2;i++)
        ret |= mVideo[i]->LoadTexture(w,h,type,pixels,0);
    return ret;
}

void VR::SetTexture(GLuint texid,int scnindex)
{
    for (int i = 0; i < 2; i++) {
        mVideo[i]->SetTexture(texid,0);
    }
}

GLuint VR::GetTexture(int scnindex)
{
    if(scnindex<0||scnindex>3)
        return 0;
    return mVideo[0]->GetTexture(0);
}

bool VR::NeedDraw()
{
    return mVideo[0]->NeedDraw() | mVideo[1]->NeedDraw();
}

void VR::StartAnimation(int scnindex)
{
    mVideo[0]->StartAnimation(scnindex);
}

void VR::StopAnimation(int scnindex)
{
    mVideo[0]->StopAnimation(scnindex);
}

bool VR::LoadTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels,int scnindex)
{
    for(int i=0;i<2;i++)
        mVideo[i]->LoadTexture(w,h,y_pixels,cbcr_pixels,0);
    return true;
}

void VR::SetTexture(GLuint y_texid,GLuint cbcr_texid,int scnindex)
{
    for(int i=0;i<2;i++)
        mVideo[i]->SetTexture(y_texid,cbcr_texid,0);
}

GLuint VR::GetTexture(int scnindex,int yuv)
{
    return 0;
}

void VR::SetNoVideoTexture(GLuint y_texid,GLuint u_texid,GLuint v_texid)
{

}

