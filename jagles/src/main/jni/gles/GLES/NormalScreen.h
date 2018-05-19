//
// Created by LiHong on 16/2/6.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef FISHEYE_NORMALSCREEN_H
#define FISHEYE_NORMALSCREEN_H


#include "ParametricSurface.h"
#include "NormalPlay.h"
#include "ParametricManager.h"

typedef struct splitposition {
    int x;
    int y;
    int w;
    int h;
}SPLIT_POSITION,*PSPLIT_POSITION;

typedef struct splitmode {
    int count;
    PSPLIT_POSITION pos;
}SPLIT_MODE;


class NormalScreen : public ISurface {
public:
    NormalScreen(float aspect,void *mgn);
    ~NormalScreen();

    void DrawSelf(int scnindex);
    void TurnLeftRight(GLfloat angle,int scnindex);
    void TurnUpDown(GLfloat angle,int scnindex);
    void setZoom(GLfloat multi,int scnindex);
    void ResetPosition();
    int GetScreenCount();
    int GetScreenMode();

    void SetPosition(vec3 pos, bool texture,int scnindex);
    void SetScale(vec3 scale, bool texture,int scnindex);
    void setRotate(vec3 rotate, bool texture,int scnindex);
    vec3 GetPosition(bool texture,int scnindex);
    vec3 GetScale(bool texture,int scnindex);
    vec3 GetRotate(bool texture,int scnindex);
	void CleanTexture(int scnindex);
    bool LoadTexture(int w,int h,GLenum type,GLvoid *pixels,int scnindex);
	bool LoadTexture(int w, int h, void* buffer, int scnindex);
    void SetTexture(GLuint texid,int scnindex);
    GLuint GetTexture(int scnindex);
    bool NeedDraw();
    void StartAnimation(int scnindex);
    void StopAnimation(int scnindex);
    void SetNoVideoTexture(GLuint texid);
    virtual void LoadOSDTexture(int w,int h,GLenum type,GLvoid *pixels) {};
    void PutInfo(GLuint texid,int scnindex);

    bool LoadTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels,int scnindex);
    void LoadOSDTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels){};
    void SetTexture(GLuint y_texid,GLuint cbcr_texid,int scnindex);
    GLuint GetTexture(int scnindex,int yuv);
    void SetNoVideoTexture(GLuint y_texid,GLuint u_texid,GLuint v_texid);
    void SetSplit(int index,bool ani);
    void SetPage(int index,bool ani);
    int  GetPage();
	int  GetAllPage();
    void SetSelected(int x,int y,int w,int h);
    //void SetSelected(int index);
	void SetSelected(bool value, int scnindex);
	int GetSelected(int scnindex);
	void SetBorderColor(GLuint selecttex, GLuint normaltex);
    void SetSplitPosition(bool ani);
	int getVideoIndex();
    void (*ObjectHidden)(ParametricManager *mgn, ParametricSurface *obj,OnAnimationEnd pOnEnd,void *ctx,int index);
    void (*ObjectWillVisible)(ParametricManager *mgn,ParametricSurface *obj,vec3 pos,vec3 scale,vec3 rotate);

	void ShowOSD(bool value) {};
	virtual void AddFrame(IFrameDraw *frame) {};
	virtual void UpdateFrame(float tbstart, float tbend, float lrstart, float lrend,int index,bool update,bool ani) {};
	virtual void ShowFrame(bool value) {};
	void ShowInfo(bool value) {};
	void ShowInfo(bool value, int index);
	void releaseInfo(int index);
	int GetSplitMode();
	bool GetVisibility(int index);
	void ReSizeSplite(float aspect);
	void ShowRecordStatu(bool value, int index);
	void LoadRecordStatuTexture(GLuint texid, int scnindex);
	void* GetLoading(int index);
	void SetAllPage(int allPage);
	void SetIsAnimation(int index, bool animation);
	void DirectTextureFrameUpdata(int w,int h, void* directBuffer, int scnindex);
	void SetOSDTexture(GLuint texid) {};

	void SetKeepAspect(float aspect,int scnindex);
	float GetKeepAspect(int scnindex);

private:
    NormalPlay *mVideo[36];
	NormalPlay *mRecordState[36];
	SPLIT_POSITION mRecordLocalVerter[16];
    int         mCurrentIndex;
    int         mCurrentMode;
    int         mCurrentPage;
	int			mSelectIndex;
    float       m_aspect;
    void        *_Manager;
	int			allPage;
	bool        isSpliting;
};


#endif //FISHEYE_NORMALSCREEN_H
