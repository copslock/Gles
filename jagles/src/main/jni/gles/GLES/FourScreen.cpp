//
// Created by LiHong on 16/1/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "FourScreen.h"

void FourScreen::SetNoVideoTexture(GLuint texid)
{
    mNormal->SetNoVideoTexture(texid);
    for(int i=0;i<4;i++)
        mVideo[i]->SetNoVideoTexture(texid);
}


void FourScreen::SetBorderColor(GLuint selecttex, GLuint normaltex)
{
	for (int i = 0; i < 4; i++)
		mVideo[i]->SetBorderColor(selecttex,normaltex);
}


void FourScreen::SetSelected(bool value, int scnindex)
{
	if (scnindex < 0 || scnindex>3)
		return;
	for (int i = 0; i < 4; i++)
		mVideo[i]->SetSelected(false, 0);
	mSelected = scnindex;
	mVideo[scnindex]->SetSelected(value, 0);
}


int FourScreen::GetSelected(int scnindex)
{
//	if (scnindex < 0 || scnindex>3)
//		return false;
//	return mVideo[scnindex]->GetSelected(0);
	return mSelected;
}

FourScreen::~FourScreen() {
    delete mNormal;
    for (int i = 0; i < 4; i++) {
        delete mVideo[i];
    }
}

FourScreen::FourScreen(float aspect,void *mgn)
{
    mNormal = new Hemisphere(1.5f,aspect,mgn);
    for(int i=0;i<4;i++) {
        mVideo[i] = new Hemisphere(1.5f, aspect, mgn);
		mVideo[i]->SetPosition(vec3(0, 0, 3), false, 0);
        mVideo[i]->setRotate(vec3(60, 0,i * 90), false, 0);
    }
	mVideo[2]->SetSelected(true, 0);
	mSelected = 2;
}

int FourScreen::GetScreenMode() {
    return SCRN_FOUR;
}

int FourScreen::GetScreenCount() {
    return 4;
}

void FourScreen::TurnLeftRight(GLfloat angle,int scnindex)
{
}

void FourScreen::TurnUpDown(GLfloat angle,int scnindex)
{

}

void FourScreen::setZoom(GLfloat multi,int scnindex)
{

}

void FourScreen::DrawSelf(int scnindex)
{
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT,vp);
    int w = vp[2]/2;
    int h = vp[3]/2;
    for(int i=0;i<4;i++)
    {
        glViewport((i%2)*w,(i/2)*h,w,h);
		if (mSelected==i)
			mVideo[i]->DrawSelf(0x7fffffff);
		else
			mVideo[i]->DrawSelf(0);
    }
//    glViewport(w*3/4,h*3/4,w/2,h/2);
//    mNormal->DrawSelf(0);
}

void FourScreen::ResetPosition() {

}

void FourScreen::SetPosition(vec3 pos, bool texture,int scnindex)
{
    if(scnindex<0||scnindex>3)
        return;
    mVideo[scnindex]->SetPosition(pos,texture,0);
}

void FourScreen::SetScale(vec3 scale, bool texture,int scnindex)
{
    if(scnindex<0||scnindex>3)
        return;
    mVideo[scnindex]->SetScale(scale,texture,0);
}

void FourScreen::setRotate(vec3 rotate, bool texture,int scnindex)
{
    if(scnindex<0||scnindex>3)
        return;
    mVideo[scnindex]->setRotate(rotate,texture,0);
}

vec3 FourScreen::GetPosition(bool texture,int scnindex)
{
    if(scnindex<0||scnindex>3)
        return vec3(0,0,0);
    return mVideo[scnindex]->GetPosition(texture,0);
}

vec3 FourScreen::GetScale(bool texture,int scnindex)
{
    if(scnindex<0||scnindex>3)
        return vec3(0,0,0);
    return mVideo[scnindex]->GetScale(texture,0);
}

vec3 FourScreen::GetRotate(bool texture,int scnindex)
{
    if(scnindex<0||scnindex>3)
        return vec3(0,0,0);
    return mVideo[scnindex]->GetRotate(texture,0);
}


bool FourScreen::LoadTexture(int w,int h,GLenum type,GLvoid *pixels,int scnindex)
{
    bool ret = mNormal->LoadTexture(w,h,type,pixels,0);
    for(int i=0;i<4;i++)
        mVideo[i]->LoadTexture(w,h,type,pixels,0);
    return ret;
}

void FourScreen::SetTexture(GLuint texid,int scnindex)
{
    mNormal->SetTexture(texid,0);
    for (int i = 0; i < 4; i++) {
        mVideo[i]->SetTexture(texid,0);
    }
}

GLuint FourScreen::GetTexture(int scnindex)
{
    if(scnindex<0||scnindex>3)
        return 0;
    return mNormal->GetTexture(0);
}

bool FourScreen::NeedDraw()
{
    return mNormal->NeedDraw() | mVideo[0]->NeedDraw() | mVideo[1]->NeedDraw() | mVideo[2]->NeedDraw() | mVideo[3]->NeedDraw();
}

void FourScreen::StartAnimation(int scnindex)
{
    mNormal->StartAnimation(scnindex);
}

void FourScreen::StopAnimation(int scnindex)
{
    mNormal->StopAnimation(scnindex);
}

bool FourScreen::LoadTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels,int scnindex)
{
    for(int i=0;i<4;i++)
        mVideo[i]->LoadTexture(w,h,y_pixels,cbcr_pixels,0);
    return true;
}

void FourScreen::SetTexture(GLuint y_texid,GLuint cbcr_texid,int scnindex)
{
    for(int i=0;i<4;i++)
        mVideo[i]->SetTexture(y_texid,cbcr_texid,0);
}

GLuint FourScreen::GetTexture(int scnindex,int yuv)
{
    return 0;
}

void FourScreen::SetNoVideoTexture(GLuint y_texid,GLuint u_texid,GLuint v_texid)
{

}

