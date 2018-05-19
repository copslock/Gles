//
// Created by LiHong on 16/2/21.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef IOS_BASESCREEN_H
#define IOS_BASESCREEN_H


#include "ParametricSurface.h"
#include "ToursLoading.h"
#include "framedraw.h"
#include "../Utils/list.h"
#if defined(ANDROID) || defined(__ANDROID__)
#include <EGL/egl.h>
#include <EGL/eglext.h>
#endif
class BaseScreen: public ParametricSurface {
public:
    BaseScreen();
    virtual ~BaseScreen();
    virtual void TurnLeftRight(GLfloat angle,int scnindex){};
    virtual void TurnUpDown(GLfloat angle,int scnindex){};
    bool NeedDraw();
    void DrawSelf(int scnindex);
    void StartAnimation(int scnindex);
    void StopAnimation(int scnindex);
    void SetPosition(vec3 pos, bool texture,int scnindex);
    void LoadOSDTexture(int w,int h,GLenum type,GLvoid *pixels);
    void LoadOSDTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels);
    void PutInfo(GLuint texid,int scnindex);
	void releaseInfo();
	void ShowOSD(bool value);
	void AddFrame(IFrameDraw *frame);
	void UpdateFrame(float tbstart, float tbend, float lrstart, float lrend,int index,bool update,bool ani);
	void ShowFrame(bool value);
	void setupLineBuffer();
	void DrawBorder();
	void SetBorderColor(GLuint selecttex, GLuint normaltex);
	void SetupBuffer();
	void SetSelected(bool value, int scnindex);
	int GetSelected(int scnindex);
	void ShowInfo(bool value);

	void ShowRecordStatu(bool value, int index);
	void LoadRecordStatuTexture(GLuint texid, int scnindex);
	void ResetSizeAnimation(float aspect);
    void* GetLoading(int index);
	void SetIsAnimation(int index, bool animation);
	void CreateDirectTextureFrame();
	void DirectTextureFrameUpdata(int w, int h, void *directBuffer, int scnindex);
	void SetIsUseDirectTexture(bool value) { isUseDirectTexture = value; directTextureBuffer = NULL; /*_texheight = 0; _texwidth = 0;*/ };
	bool mIsOwnAnimation;
protected:
    virtual vec3 Evaluate(const vec2& domain) = 0;
    virtual vec2 EvaluateCoord(const vec2& domain) = 0;
	bool	mShowOSD;
	int mIsSelected;

private:
    ISurface *_loading;
    ISurface *_OSD;
    ISurface *_Info;
    long _LoadingAnimation;
    GLuint _InfoTexID;
	List<IFrameDraw *> mFrameList;
	bool	mShowframe;
	GLuint mSelectedTex;
	GLuint mNormalTex;
	GLuint mLineBuffer;
	GLuint mLinebufferSize;
	bool mShowInfo;
	bool mShowRecordStatue;
	ToursLoading *_temploading;
	
	int direct_height;
	int direct_width;
	void * directTextureBuffer;
	bool isUseDirectTexture;
};

#endif //IOS_BASESCREEN_H
