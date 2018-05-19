//
// Created by LiHong on 16/4/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef IOS_VR_H
#define IOS_VR_H


#include "OneScreen.h"
#include "Hemisphere.h"
#include "NormalPlay.h"

class VR : public ISurface {
public:
    VR(float aspect,void *mgn);
    ~VR();
    void TurnLeftRight(GLfloat angle,int scnindex);
    void TurnUpDown(GLfloat angle,int scnindex);
    void setZoom(GLfloat multi,int scnindex);
    void DrawSelf(int scnindex);
    int GetScreenCount();
    void ResetPosition();
    int GetScreenMode();
    void SetPosition(vec3 pos, bool texture,int scnindex);
    void SetScale(vec3 scale, bool texture,int scnindex);
    void setRotate(vec3 rotate, bool texture,int scnindex);
    vec3 GetPosition(bool texture,int scnindex);
    vec3 GetScale(bool texture,int scnindex);
    vec3 GetRotate(bool texture,int scnindex);

    bool LoadTexture(int w,int h,GLenum type,GLvoid *pixels,int scnindex);
    void SetTexture(GLuint texid,int scnindex);
    GLuint GetTexture(int scnindex);
    bool NeedDraw();
    void StartAnimation(int scnindex);
    void StopAnimation(int scnindex);

    void SetNoVideoTexture(GLuint texid);
    virtual void LoadOSDTexture(int w,int h,GLenum type,GLvoid *pixels) {};
    virtual void LoadOSDTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels){};
    void PutInfo(GLuint texid,int scnindex) {mVideo[0]->PutInfo(texid,scnindex);}

    bool LoadTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels,int scnindex);
    void SetTexture(GLuint y_texid,GLuint cbcr_texid,int scnindex);
    GLuint GetTexture(int scnindex,int yuv);
    void SetNoVideoTexture(GLuint y_texid,GLuint u_texid,GLuint v_texid);
	void ShowOSD(bool value) {};
	virtual void AddFrame(IFrameDraw *frame) {};
	virtual void UpdateFrame(float tbstart, float tbend, float lrstart, float lrend,int index,bool update,bool ani) {};
	virtual void ShowFrame(bool value) {};
	void SetBorderColor(GLuint selecttex, GLuint normaltex) {};
	void SetSelected(bool value, int scnindex) {};
	int GetSelected(int scnindex) { return false; };
	void ShowInfo(bool value) {};
	void ShowRecordStatu(bool value) {};
	void LoadRecordStatuTexture(GLuint texid, int scnindex) {};
	virtual void* GetLoading(int index) {return 0;};
	virtual void SetIsAnimation(int index, bool animation){};
private:
    Hemisphere *mVideo[2];
};


#endif //IOS_VR_H



