#pragma once
#ifdef _MSC_VER
#include <windows.h>
#include <tchar.h>
#endif
#define GL_GLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdint.h>
#include "GPUMotionDetection.h"
#include <pthread.h>
#include "../Utils/mediabuffer.h"
#include <string.h>
#include "../Utils/list.h"


class TrackingFrame {
public:
	int _l;
	int _t;
	int _r;
	int _b;
	int _area;
	bool _update;
	int _updatecount;
	int _skipupdatecount;
	int _index;
	TrackingFrame(){
		_l = 0; _t = 0; _r = 0; _b = 0; _area = 0; _update = true; _updatecount = 0; _skipupdatecount = 0; _index = -1;
	};
	bool intersect(TrackingFrame *tf)
	{
		return contain(tf->_l,tf->_t) || contain(tf->_l,tf->_b) || contain(tf->_r,tf->_t) || contain(tf->_r,tf->_b);
	};
	void combine(TrackingFrame *tf)
	{
		TrackingFrame *rt=new TrackingFrame();
		_l=tf->_l<_l ? tf->_l : _l;
		_r=tf->_r>_r ? tf->_r : _r;
		_t=tf->_t<_t ? tf->_t : _t;
		_b=tf->_b>_b ? tf->_b : _b;
		_update = true;
		_updatecount = (tf->_updatecount > _updatecount ? tf->_updatecount : _updatecount)+1;
		_skipupdatecount = 0;
	};
	void updatefrom(TrackingFrame *tf) {
		_l=tf->_l;
		_t=tf->_t;
		_r=tf->_r;
		_b=tf->_b;
		_update=true;
		_updatecount++;
		_skipupdatecount=0;
		_area = tf->_area;
	};
	bool contain(int x,int y)
	{
		return x>=_l&&x<=_r&&y>=_t&&y<=_b;
	};
	void resizeFrame()
	{
		int cx = (_l + _r) / 2;
		int cy = (_t + _b) / 2;
		_l = (cx - 5) > 0 ? cx - 5 : 0;
		_r = (cx + 5) > 49 ? 49 : cx + 5;
		_t = (cy - 5) > 0 ? cy - 5 : 0;
		_b = (cy + 5) > 49 ? 49 : cy + 5;
	}
};

class GPUMotionTracking
{
private:
	int InitGLES();
	bool testEGLError(const char* functionLastCalled);
	bool testGLError(const char* functionLastCalled);
	void DetectTracking(int scrnindex);
	TrackingFrame *GetFrame(int x,int y);
	bool InFrame(int x,int y,List <TrackingFrame *> *lst);
	int intersectFrame(TrackingFrame *tf,List <TrackingFrame *> *lst);
	int GetFrameArea(TrackingFrame *tf);

public:
	GPUMotionTracking(bool FBO);
	virtual ~GPUMotionTracking();
	int render();
	void Clear();
	void setCurrent();
	int (*OnMotionTracking)(float l,float t,float r,float b,float w,float h,int index,int scrnindex,void *ctx, bool del);
	void *DoThread();
	void LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex);
	void LoadTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels, int scnindex);

//FBO implementation
	bool InitFBO();
	bool RenderFBO();
	void DestroyFBO();

	bool IsFBO(){return mFBO;};
public:
	void *mCtx;
	int mObjectSize;
	int mDropSecond;

private:
	EGLDisplay			eglDisplay;
	EGLConfig			eglConfig;
	EGLSurface			eglSurface;
	EGLContext			eglContext;
    GLuint defaultFramebuffer, colorRenderbuffer, depthRenderbuffer;
    
	GPUMotionDetection  *mMotion[4];
	pthread_t                       mTrackingThread;
	pthread_mutex_t                 mTrackingLock;
#ifdef __JA_MAC__
	volatile
#else
	#ifdef	__ANDROID__
	volatile
#else
		unsigned
#endif
#endif
	int mThreadRun;
#ifdef __USE_RGB32__
	void *mPixels[4];
#else
	void *mY_pixels[4];
	void *mCbcr_pixels[4];
#endif
	int mTexWidth[4];
	int mTexHeight[4];
#ifdef __JA_MAC__
	volatile
#else
#ifdef	__ANDROID__
	volatile
#else
		unsigned
#endif
#endif
	int mHaveTexture[4];
	unsigned int mPixelsBuffer[50 * 50];
	List <TrackingFrame *> mTrackingFrame;
	TrackingFrame *_temptf;

//FBO implementation
	GLsizei cxFBO;
	GLsizei cyFBO;
	GLuint _frameBufferID;
	GLuint _colorRenderBuffer;
	GLuint _depthRenderBuffer;
	GLuint _fboTextureID;

	bool   mFBO;
};
