//
// Created by LiHong on 16/1/28.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "UpDownScreen.h"

void UpDownScreen::StartAnimation(int scnindex)
{
    mUp->StartAnimation(scnindex);
}

void UpDownScreen::StopAnimation(int scnindex)
{
    mUp->StopAnimation(scnindex);
}

UpDownScreen::~UpDownScreen() {
    delete mUp;
    delete mDown;
}

UpDownScreen::UpDownScreen(float aspect,void *mgn)
{
    mUp = new OneScreen(2,1.01,aspect,180.0f,0,0.55,0,mgn);
    mDown = new OneScreen(2,1.01,aspect,180.0f,0,-0.45,180.0f,mgn);
}

int UpDownScreen::GetScreenMode() {
    return SCRN_UPDOWN;
}

int UpDownScreen::GetScreenCount() {
    return 2;
}

void UpDownScreen::TurnLeftRight(GLfloat angle,int scnindex)
{
    if(scnindex)
        mDown->TurnLeftRight(angle*180,0);
    else
        mUp->TurnLeftRight(angle*180,0);
}

void UpDownScreen::TurnUpDown(GLfloat angle,int scnindex)
{

}

void UpDownScreen::DrawSelf(int scnindex)
{
    mDown->DrawSelf(0);
    mUp->DrawSelf(0);
}

void UpDownScreen::setZoom(GLfloat multi,int scnindex)
{

}

void UpDownScreen::ResetPosition() {
    mUp->ResetPosition();
    mDown->ResetPosition();
}

void UpDownScreen::SetPosition(vec3 pos, bool texture,int scnindex)
{
    if(scnindex)
        mDown->SetPosition(pos,texture,0);
    else
        mUp->SetPosition(pos,texture,0);
}

void UpDownScreen::SetScale(vec3 scale, bool texture,int scnindex)
{
    if(scnindex)
        mDown->SetScale(scale,texture,0);
    else
        mUp->SetScale(scale,texture,0);
}

void UpDownScreen::setRotate(vec3 rotate, bool texture,int scnindex)
{
    if(scnindex)
        mDown->setRotate(rotate,texture,0);
    else
        mUp->setRotate(rotate,texture,0);
}

vec3 UpDownScreen::GetPosition(bool texture,int scnindex)
{
    if(scnindex)
        return mDown->GetPosition(texture,0);
    else
        return mUp->GetPosition(texture,0);
}

vec3 UpDownScreen::GetScale(bool texture,int scnindex)
{
    if(scnindex)
        return mDown->GetScale(texture,0);
    else
        return mUp->GetScale(texture,0);
}

vec3 UpDownScreen::GetRotate(bool texture,int scnindex)
{
    if(scnindex)
        return mDown->GetRotate(texture,0);
    else
        return mUp->GetRotate(texture,0);
}


bool UpDownScreen::LoadTexture(int w,int h,GLenum type,GLvoid *pixels,int scnindex)
{
    bool ret = mDown->LoadTexture(w,h,type,pixels,0);
    mUp->SetTexture(mDown->GetTexture(0),0);
    return ret;
}

void UpDownScreen::SetTexture(GLuint texid,int scnindex)
{
    mDown->SetTexture(texid,0);
    mUp->SetTexture(texid,0);
}

GLuint UpDownScreen::GetTexture(int scnindex)
{
    if(scnindex)
        return mDown->GetTexture(0);
    else
        return mUp->GetTexture(0);
}

bool UpDownScreen::NeedDraw()
{
    return mDown->NeedDraw() | mUp->NeedDraw();
}

void UpDownScreen::SetNoVideoTexture(GLuint texid)
{
    mDown->SetNoVideoTexture(texid);
    mUp->SetNoVideoTexture(texid);
}

void UpDownScreen::LoadOSDTexture(int w,int h,GLenum type,GLvoid *pixels)
{
    mUp->LoadOSDTexture(w,h,type,pixels);
}

void UpDownScreen::LoadOSDTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels)
{
    mUp->LoadOSDTexture(w,h,y_pixels,cbcr_pixels);
}

bool UpDownScreen::LoadTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels,int scnindex)
{
    mUp->LoadTexture(w,h,y_pixels,cbcr_pixels,0);
    mDown->LoadTexture(w,h,y_pixels,cbcr_pixels,0);
    return true;
}

void UpDownScreen::SetTexture(GLuint y_texid,GLuint cbcr_texid,int scnindex)
{
    mUp->SetTexture(y_texid,cbcr_texid,0);
    mDown->SetTexture(y_texid,cbcr_texid,0);
}

GLuint UpDownScreen::GetTexture(int scnindex,int yuv)
{
    return 0;
}

void UpDownScreen::SetNoVideoTexture(GLuint y_texid,GLuint u_texid,GLuint v_texid)
{

}

