//
// Created by LiHong on 16/6/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef OSX_VERTEXOBJECTIMPL_H
#define OSX_VERTEXOBJECTIMPL_H

#include "BaseScreen.h"
#include "VertexObject.h"
//#include "../../include/list.h"
#include "SphereStitchImp.h"
#include <pthread.h>
#include "GLESMath.h"


typedef struct vosplitposition {
    int x;
    int y;
    int w;
    int h;
}VOSPLIT_POSITION,*PVOSPLIT_POSITION;

typedef struct vomatrix {
    vec3    _pos;
    vec3    _scale;
    vec3    _rotate;
    vec3    _texpos;
    vec3    _texscale;
    vec3    _texrotate;
}VO_MATRIX,*PVO_MATRIX;

typedef struct vomode {
    VOSPLIT_POSITION _pos;
    VO_MATRIX        _default;
    VO_MATRIX        _matrix;
    int              _mode;
    bool             _ismain;
    bool             _showframe;
} VO_MODEITEM,*PVO_MODEITEM;


typedef struct vomodelist {
    PVO_MODEITEM _item;
    int         _mode;
    int         _count;
    bool        _showselected;
}VO_MODE,*PVO_MODE;


class VertexObjectImpl :
        public ISurface{
public:
    VertexObjectImpl(float aspect, void *mgn);
    virtual ~VertexObjectImpl();
    void DrawSelf(int scnindex);
    int GetScreenCount();
    void ResetPosition();
    int GetScreenMode();
    void SetPosition(vec3 pos, bool texture, int scnindex);
    void SetScale(vec3 scale, bool texture, int scnindex);
    void setRotate(vec3 rotate, bool texture, int scnindex);
    vec3 GetPosition(bool texture, int scnindex);
    vec3 GetScale(bool texture, int scnindex);
    vec3 GetRotate(bool texture, int scnindex);

    bool LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex);
    void SetTexture(GLuint texid, int scnindex);
    GLuint GetTexture(int scnindex);
    bool NeedDraw();
    void StartAnimation(int scnindex);
    void StopAnimation(int scnindex);

    void SetNoVideoTexture(GLuint texid);
	void CleanTexture(int scnindex);
    bool LoadTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels, int scnindex);
    void SetTexture(GLuint y_texid, GLuint cbcr_texid, int scnindex);
    GLuint GetTexture(int scnindex, int yuv);
    void SetNoVideoTexture(GLuint y_texid, GLuint u_texid, GLuint v_texid);
    void ShowOSD(bool value);

    void UpdateFrame(float tbstart, float tbend, float lrstart, float lrend,int index,bool update,bool ani);
    void SetBorderColor(GLuint selecttex, GLuint normaltex);
    void SetSelected(bool value, int scnindex);
    int GetSelected(int scnindex);
    void ShowInfo(bool value) ;
    void LoadOSDTexture(int w, int h, GLenum type, GLvoid *pixels);
    void LoadOSDTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels);
    void PutInfo(GLuint texid, int scnindex);
    void AddFrame(IFrameDraw *frame);
    void ShowFrame(bool value);
	void ShowRecordStatu(bool value, int index) { mShowRecordState = value; };
    void LoadRecordStatuTexture(GLuint texid, int scnindex);
    void* GetLoading(int index);

    void SwitchMode(int mode,bool Ani);

	bool GetSwitchAnimation() { return mChangeAnimation; };

    void UpdateAspect(float aspect);
    void HandleChangeThread();

    void SetTextTexture(GLuint texid){mVertex->SetTextTexture(texid);};
    void TransformVertex(int mode,vec3 vect,bool texture,int type,int scrnindex);
	virtual void SetIsAnimation(int index, bool animation) {};
	void SetViewAngle(float angle);
	void EnableGrid(bool value) { mVertex->EnableGrid(value); };
	void DirectTextureFrameUpdata(int w, int h, void * directBuffer, int index);
	void DirectTextureOSDFrameUpdata(int w, int h, void * directBuffer, int index);
	bool DualMode() { return mDualMode == 1; };
	void SetDualMode(int mode) { mDualMode = mode; };
	void SetDistortion(PDISTORTION_NUM pdist, int dist_count, float half);
	void SetFishEyeParameter(float centerx, float centery, float radius, float anglex, float angley, float anglez, int index,bool v720);
	void SetTextureAspect(float aspect);
	void SetSingleMode(bool single);
	void SetFactoryMode(int factory);
	void RenderFBO();
	void SetOSDTexture(GLuint texid) {};
	GLuint FBOTexture(){return mSphere->FBOTexture();};
	void SetFBOTexture(GLint texid){mSphere->SetFBOTexture(texid); };

private:
    VertexObject *mVertex;
    int		    mSelected;
    float		m_aspect;
    void		*mManager;
    int         mCurrentMode;
    List <PVO_MODE> mVertexList[2];
    bool        _NeedDraw;
	int			mDualMode;

    ISurface *_loading;
    ISurface *_OSD;
    ISurface *_Info;
	ISurface *_RecordState;
	GLuint _RecordStateTexID;
    long _LoadingAnimation;
    GLuint _InfoTexID;
    List<IFrameDraw *> mFrameList;
    bool	mShowframe;
    GLuint mLineBuffer;
    GLuint mLinebufferSize;
    bool mShowInfo;
    bool	mShowOSD;
	bool mShowRecordState;
    float 		mleftangle;
    float 		mrightangle;

    pthread_t   mChangeThread;
    pthread_mutex_t mChangeLock;
    bool   mChangeAnimation;
    int    mChangeMode;
    int    mChangeStep;
    int    mChangeStepCount;

    int    mRunThread;

	SphereStitchImp *mSphere;
private:
    PVO_MODE    getCurrent(int mode);
    PVO_MODEITEM    getFrameTypeVO();
	PVO_MODEITEM    getFrameTypeVO(int* index);
    void UpdateFrameHemisphere(float tbstart, float tbend, float lrstart, float lrend,int index,bool update,bool ani);
    void UpdateFramePanorama(float tbstart, float tbend, float lrstart, float lrend,int index,bool update,bool ani);
    IFrameDraw *GetFrameDraw(int framedrawtype);
};


#endif //OSX_VERTEXOBJECTIMPL_H
