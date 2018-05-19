//
// Created by LiHong on 16/7/27.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef OSX_SPHERESTITCH_H
#define OSX_SPHERESTITCH_H


#include "SphereOne.h"
#include "BaseScreen.h"


#define DTOR            0.0174532925
#define RTOD            57.2957795
#define TWOPI           6.283185307179586476925287
#define PI              3.141592653589793238462643
#define PID2            1.570796326794896619231322

#define XTILT 0 // x
#define YROLL 1 // y
#define ZPAN  2 // z


typedef struct {
	GLuint axis;
	GLuint value;
	GLuint cvalue;
	GLuint svalue;
} GLTRANSFORM;

typedef struct {
	GLuint width;
	GLuint height;
	GLuint centerx;
	GLuint centery;
	GLuint radius;
	GLuint aperture;
	GLTRANSFORM transform[3];
	GLuint ntransform;
} GLFISHEYE;


typedef struct {
	int axis;
	float value;
	float cvalue, svalue;
} TRANSFORM;

typedef struct {
	float width, height;
	float centerx, centery;
	float radius;
	float aperture;
	TRANSFORM transform[3];
	int ntransform;
} FISHEYE;


class SphereStitch : public BaseScreen {
public:
    SphereStitch(float radius,float aspect,void *mgn);
    ~SphereStitch();
	vec3 Evaluate(const vec2& domain);
	vec2 EvaluateCoord(const vec2& domain);
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
    void LoadOSDTexture(int w,int h,GLenum type,GLvoid *pixels);
    void LoadOSDTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels);
    void PutInfo(GLuint texid,int scnindex) {BaseScreen::PutInfo(texid,scnindex);}

    bool LoadTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels,int scnindex);
    void SetTexture(GLuint y_texid,GLuint cbcr_texid,int scnindex);
    GLuint GetTexture(int scnindex,int yuv);
    void SetNoVideoTexture(GLuint y_texid,GLuint u_texid,GLuint v_texid);
    void ShowOSD(bool value) {};
    virtual void AddFrame(IFrameDraw *frame) {};
    virtual void UpdateFrame(float tbstart, float tbend, float lrstart, float lrend,int index,bool update,bool ani) {};
    virtual void ShowFrame(bool value) {};
    void SetBorderColor(GLuint selecttex, GLuint normaltex) {};
    void SetSelected(bool value, int scnindex){};
    int GetSelected(int scnindex) { return false; };
    void ShowInfo(bool value) {};
    void ShowRecordStatu(bool value) {};
    void LoadRecordStatuTexture(GLuint texid, int scnindex) {};
    virtual void* GetLoading(int index) {return 0;};
    virtual void SetIsAnimation(int index, bool animation) {};

    void SetStitchOptions(float centerx, float centery, float radius,float aperture, float anglex, float angley, float anglez,int width,int height, int index);

    void SetBlending(float blending){mBlending = blending;};
    void SetStitching(int stitching){mStitching = stitching;};
	void SetDistortion(PDISTORTION_NUM pdist, int dist_count, float half);

	bool IsInitFBO(){return cxFBO!=0;};
    GLuint FBOTexture(){return _fboTextureID;};
	void SetFBOTexture(GLint texid){_fboTextureID = texid; };
	bool InitFBO(int w, int h);
	bool RenderFBO();
	void DestroyFBO();

private:
    float m_radius;
	float mDistortion[115];
	float mHalf_Height;
	FISHEYE mFish[2];
    float mBlending;
    int mStitching;
	GLFISHEYE mFishHandle[2];
	GLuint	mSolutionHandle;
	GLuint  mBlendingHandle;
    GLuint  mStitchingHandle;
//FBO implementation
    GLsizei cxFBO;
    GLsizei cyFBO;
    GLuint _frameBufferID;
    GLuint _colorRenderBuffer;
    GLuint _depthRenderBuffer;
    GLuint _fboTextureID;
	GLuint _distortionHandle[115];
	GLuint _halfheightHandle;
	void *mRenderBuffer;
	void InitUniform();
	void UpdateUniform();
};

#endif //OSX_SPHERESTITCH_H
