#ifndef EMSCRIPTEN
#include "GPUMotionTracking.h"

#ifdef __ANDROID__
#include <unistd.h>
#endif

#ifdef __LINUXNVR__

#define AtomicInc(a)  __sync_add_and_fetch (a, 1);
#define AtomicDec(a)  __sync_sub_and_fetch (a, 1);
#define AtomicSwap(a, b, c) __sync_val_compare_and_swap(b,c,a);
#endif


void *MotionTrackingThread(void *arg)
{
	GPUMotionTracking *gmt = (GPUMotionTracking *)arg;
	return gmt->DoThread();
}


bool GPUMotionTracking::testEGLError(const char* functionLastCalled)
{
	EGLint lastError = eglGetError();
	if (lastError != EGL_SUCCESS)
	{
		char stringBuffer[256];
		sprintf(stringBuffer, "%s failed (%x).\n", functionLastCalled, lastError);
		printf("%s",stringBuffer);
		return false;
	}

	return true;
}

bool GPUMotionTracking::testGLError(const char* functionLastCalled)
{
	GLenum lastError = glGetError();
	if (lastError != GL_NO_ERROR)
	{
		char stringBuffer[256];
		sprintf(stringBuffer, "%s failed (%x).\n", functionLastCalled, lastError);
		printf("%s",stringBuffer);
		return false;
	}
	return true;
}

TrackingFrame *GPUMotionTracking::GetFrame(int x,int y)
{
	int l,t,r,b;
	l=x-5;
	for(int i=x-5;i<x+5;i++)
	{
		bool lfound=false;
		for(int j=y;j<((y+10)>50?50:y+10);j++)
			if(mPixelsBuffer[(j)*50+i]==0xffffffff)
			{
				lfound=true;
				l=i;
				break;
			}
		if(lfound)
			break;
	}
	r=x+4;
	for(int i=x+4;i>=x-5;i--)
	{
		bool lfound=false;
		for(int j=y;j<((y+10)>50?50:y+10);j++)
			if(mPixelsBuffer[(j)*50+i]==0xffffffff)
			{
				lfound=true;
				r=i;
				break;
			}
		if(lfound)
			break;
	}
	t=y;
	for(int i=y;i<((y+10)>50?50:y+10);i++)
	{
		bool lfound=false;
		for(int j=x-5;j<x+5;j++)
			if(mPixelsBuffer[(i)*50+j]==0xffffffff)
			{
				lfound=true;
				t=i;
				break;
			}
		if(lfound)
			break;
	}
	b=(y+10)>49?49:y+10;
	for(int i=((y+9)>49?49:y+9);i>=y;i--)
	{
		bool lfound=false;
		for(int j=x-5;j<x+5;j++)
			if(mPixelsBuffer[(i)*50+j]==0xffffffff)
			{
				lfound=true;
				b=i;
				break;
			}
		if(lfound)
			break;
	}
	_temptf = new TrackingFrame();
	_temptf->_l = l;
	_temptf->_t = t;
	_temptf->_r = r;
	_temptf->_b = b;
	_temptf->_area=GetFrameArea(_temptf);
	_temptf->_update = true;
	return _temptf;
}

bool GPUMotionTracking::InFrame(int x,int y,List <TrackingFrame *> *lst)
{
	for(int i=0;i<lst->length();i++)
		if(lst->at(i)->contain(x,y))
			return true;
	return false;
}

int GPUMotionTracking::intersectFrame(TrackingFrame *tf,List <TrackingFrame *> *lst)
{
	for(int i=0;i<lst->length();i++)
		if(lst->at(i)->intersect(tf))
			return i;
	return -1;
}

int GPUMotionTracking::GetFrameArea(TrackingFrame *tf)
{
	int lcount=0;
	for(int x=tf->_l;x<=tf->_r;x++)
		for(int y=tf->_t;y<=tf->_b;y++)
			if(mPixelsBuffer[(y)*50+x]==0xffffffff)
				lcount++;
	return lcount;
}




void GPUMotionTracking::DetectTracking(int scrnindex)
{
	glReadPixels(0, 0, 50, 50, GL_RGBA, GL_UNSIGNED_BYTE, (void *)mPixelsBuffer);
//    	for (int i = 0; i < 50 * 50; i++)
//    	{
//    		if (!(i % 50))
//    			printf("\n");
//    		if (mPixelsBuffer[i] == 0xffffffff)
//    			printf("%d", 1);
//    		else
//    			printf("%d",0);
//    	}
//    	printf("\n\n");
    //return;
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	int y=0;
	int x=5;
	for(int i=mTrackingFrame.length()-1;i>=0;i--)
		mTrackingFrame.at(i)->_update=false;
	List <TrackingFrame *> lframelist;
	for(y=0;y<50;y++) {
		for(x=5;x<45;x++) {
			if(mPixelsBuffer[x+(y)*50]==0xffffffff)
			{
				if(InFrame(x,y,&lframelist)) {
					continue;
				}
				TrackingFrame *tf=GetFrame(x,y);
				int lindex=intersectFrame(tf,&lframelist);
				if(lindex>=0) {
					lframelist.at(lindex)->combine(tf);
					lframelist.at(lindex)->_area = GetFrameArea(lframelist.at(lindex));
					delete tf;
				}
				else
				{
					lframelist.insert(-1, tf);
				}
			}
		}
	}

	for(int i=lframelist.length()-1;i>=0;i--)
	{
		lframelist.at(i)->resizeFrame();
		if (lframelist.at(i)->_area >= mObjectSize)
		{
			int lindex = intersectFrame(lframelist.at(i), &mTrackingFrame);
			if (lindex >= 0) {
				//printf("update Frame\n");
				mTrackingFrame.at(lindex)->updatefrom(lframelist.at(i));
				mTrackingFrame.at(lindex)->_area = GetFrameArea(mTrackingFrame.at(lindex));
			}
			else
			{
				TrackingFrame *tf = new TrackingFrame();
				printf("new Frame\n");
				tf->updatefrom(lframelist.at(i));
				tf->_area = GetFrameArea(tf);
				mTrackingFrame.insert(-1, tf);
			}
		}
		TrackingFrame *lframe = lframelist.at(i);
		lframelist.remove(i);
		delete lframe;
	}
	for(int i=mTrackingFrame.length()-1;i>=0;i--)
	{
		if (!mTrackingFrame.at(i)->_update)
		{
			mTrackingFrame.at(i)->_skipupdatecount = mTrackingFrame.at(i)->_skipupdatecount + 1;
			if(mTrackingFrame.at(i)->_skipupdatecount>mDropSecond)
			{
				printf("delete frame\n");
				mTrackingFrame.at(i)->_index = OnMotionTracking(mTrackingFrame.at(i)->_l, mTrackingFrame.at(i)->_t, mTrackingFrame.at(i)->_r, mTrackingFrame.at(i)->_b, 50, 50, mTrackingFrame.at(i)->_index, scrnindex, mCtx, true);
				TrackingFrame *lframe = mTrackingFrame.at(i);
				mTrackingFrame.remove(i);
				delete lframe;
			}
		}
		else
		{
			if (OnMotionTracking)
			{
				mTrackingFrame.at(i)->_index = OnMotionTracking(mTrackingFrame.at(i)->_l, mTrackingFrame.at(i)->_t, mTrackingFrame.at(i)->_r, mTrackingFrame.at(i)->_b, 50, 50, mTrackingFrame.at(i)->_index, scrnindex, mCtx, false);
				if (mTrackingFrame.at(i)->_index < 0)
				{
					TrackingFrame *lframe = mTrackingFrame.at(i);
					mTrackingFrame.remove(i);
					delete lframe;
				}
			}
		}
	}
}

GPUMotionTracking::GPUMotionTracking(bool FBO) {
	eglDisplay = NULL;
	eglConfig = NULL;
	eglSurface = NULL;
	eglContext = NULL;
	mThreadRun = 1;
	mCtx = NULL;
	OnMotionTracking = NULL;
	mObjectSize = 10;
	mDropSecond = 85;
	mFBO = FBO;
    memset((void *)mPixelsBuffer,0,50*50*4);
	memset((void *)mHaveTexture, 0, sizeof(int)*4);

	_fboTextureID = 0;
	_colorRenderBuffer  = 0;
	_depthRenderBuffer = 0;
	_frameBufferID = 0;

	if(!mFBO) {
		pthread_mutex_init(&mTrackingLock, NULL);
		pthread_create(&mTrackingThread, NULL, MotionTrackingThread, this);
	}
}


GPUMotionTracking::~GPUMotionTracking()
{
	if(!mFBO) {
		AtomicDec(&mThreadRun);
		pthread_join(mTrackingThread, NULL);
	}
	else
		DestroyFBO();
}

int GPUMotionTracking::InitGLES()
{
	eglDisplay = eglGetDisplay((EGLNativeDisplayType)EGL_DEFAULT_DISPLAY);
	if (eglDisplay == EGL_NO_DISPLAY)
	{
		printf("Failed to get an EGPUMotionTracking\n");
		return false;
	}

	EGLint eglMajorVersion, eglMinorVersion;
	if (!eglInitialize(eglDisplay, &eglMajorVersion, &eglMinorVersion))
	{
		printf("Failed to initialise the EGPUMotionTracking\n");
		return false;
	}

	const EGLint configurationAttributes[] =
	{
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,// very important!
		EGL_SURFACE_TYPE,EGL_PBUFFER_BIT,//EGL_WINDOW_BIT EGL_PBUFFER_BIT we will create a pixelbuffer surface
		EGL_RED_SIZE,   8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE,  8,
		EGL_ALPHA_SIZE, 8,// if you need the alpha channel
		EGL_DEPTH_SIZE, 8,// if you need the depth buffer
		EGL_STENCIL_SIZE,8,
		EGL_NONE
	};

	const EGLint surfaceAttr[] = {
		EGL_WIDTH,50,
		EGL_HEIGHT,50,
		EGL_NONE
	};


	EGLint configsReturned;
	if (!eglChooseConfig(eglDisplay, configurationAttributes, &eglConfig, 1, &configsReturned) || (configsReturned != 1))
	{
		GLenum lastError = glGetError();
		char stringBuffer[256];
		sprintf(stringBuffer, "%s failed (%x). %x\n", "eglChooseConfig", lastError, configsReturned);
		printf("%s",stringBuffer);
		//testGLError(nativeWindow, "eglChooseConfig");
		return false;
	}

	eglSurface = eglCreatePbufferSurface(eglDisplay, eglConfig, surfaceAttr);

	if (eglSurface == EGL_NO_SURFACE)
	{
		switch (eglGetError())
		{
		case EGL_BAD_ALLOC:
			// Not enough resources available. Handle and recover
			printf("Not enough resources available\n");
			break;
		case EGL_BAD_CONFIG:
			// Verify that provided EGLConfig is valid
			printf("provided EGLConfig is invalid\n");
			break;
		case EGL_BAD_PARAMETER:
			// Verify that the EGL_WIDTH and EGL_HEIGHT are
			// non-negative values
			printf("provided EGL_WIDTH and EGL_HEIGHT is invalid\n");
			break;
		case EGL_BAD_MATCH:
			// Check window and EGLConfig attributes to determine
			// compatibility and pbuffer-texture parameters
			printf("Check window and EGLConfig attributes\n");
			break;
		}
	}


	EGLint contextAttributes[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	// Create the context with the context attributes supplied
	eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, contextAttributes);
	if (!testEGLError("eglCreateContext")) { return false; }

	setCurrent();
	if (!testEGLError("eglMakeCurrent")) { return false; }

    
    eglBindAPI(EGL_OPENGL_ES_API);
    if (!testEGLError("eglBindAPI")) { return false; }
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        printf("Problem with OpenGL framebuffer after specifying color render buffer: %d\n", status);
    }

    
    return 0;
}


void GPUMotionTracking::setCurrent()
{
	eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
}

void * GPUMotionTracking::DoThread()
{
	InitGLES();
    Clear();
    render();
	for (int i = 0; i < 3; i++)
		mMotion[i] = new GPUMotionDetection(2, 1, NULL);
	while(mThreadRun)
	{
		for (int i = 0; i < 3; i++)
		{
			if (mHaveTexture[i])
			{
				Clear();
#ifdef __USE_RGB32__
				mMotion[i]->LoadTexture(mTexWidth[i], mTexHeight[i], GL_RGBA, mPixels[i], 0);
#else
				
#endif
				mMotion[i]->DrawSelf(0);
				render();
                glFlush();
				DetectTracking(i);
				AtomicDec(&mHaveTexture[i]);
			}
		}
#ifdef _MSC_VER
		Sleep(10);
#else
		usleep(10000);
#endif
	}
	return NULL;
}


void GPUMotionTracking::LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex)
{
#ifndef __LINUXNVR__
	if (scnindex < 0 || scnindex>3)
		return;
	mTexWidth[scnindex] = w;
	mTexHeight[scnindex] = h;
	if(mFBO) {
		mMotion[scnindex]->LoadTexture(mTexWidth[scnindex], mTexHeight[scnindex], GL_RGBA, pixels, 0);
		AtomicInc(&mHaveTexture[scnindex]);
	}
	else {
		mPixels[scnindex] = pixels;
		AtomicInc(&mHaveTexture[scnindex]);
		while (mHaveTexture[scnindex]) {
#ifdef _MSC_VER
			Sleep(10);
#else
			usleep(10000);
#endif
		}
	}
#endif
}

void GPUMotionTracking::LoadTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels, int scnindex)
{
#ifndef __USE_RGB32__
	if (scnindex < 0 || scnindex>3)
		return;
	mTexWidth[scnindex] = w;
	mTexHeight[scnindex] = h;
	mY_pixels[scnindex] = y_pixels;
	mCbcr_pixels[scnindex] = cbcr_pixels;
	AtomicInc(&mHaveTexture[scnindex]);
	while (mHaveTexture[scnindex])
	{
#ifdef _MSC_VER
		Sleep(10);
#else
		usleep(10000);
#endif
	}
#endif
}

void GPUMotionTracking::Clear()
{
	glViewport(0, 0, 50,50);
	glClearColor(0, 0, 0, 1);
	//printf("glClearColor:%d\n",glGetError());
	glClear(GL_COLOR_BUFFER_BIT);
	//printf("glClear:%d\n",glGetError());
}

int GPUMotionTracking::render()
{
	if (!eglSwapBuffers(eglDisplay, eglSurface))
	{
		//testEGLError(nativeWindow, "eglSwapBuffers");
	}
	return 0;
}


bool GPUMotionTracking::InitFBO() {
	if (_frameBufferID != 0)
	{
		glDeleteRenderbuffers(1, &_depthRenderBuffer);
		glDeleteRenderbuffers(1, &_colorRenderBuffer);
		glDeleteFramebuffers(1, &_frameBufferID);
		printf("Recreating FBO\n");
	}

	cxFBO = 50;
	cyFBO = 50;

	GLint maxTextureSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

	if ((cxFBO > maxTextureSize) ||
			(cyFBO > maxTextureSize))
	{
		printf("Requested size of frame buffer exceeds maximum\n");
		return false;
	}

	glGenFramebuffers(1, &_frameBufferID);
	glGenRenderbuffers(1, &_depthRenderBuffer);
	glGenTextures(1, &_fboTextureID);
//	LOGDEBUG("0--glGenTextures:%d",texture_object_id);


	// texture
	glBindTexture(GL_TEXTURE_2D, _fboTextureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cxFBO, cyFBO, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// depth buffer
	glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, cxFBO, cyFBO);

	// frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferID);


	// attachments
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fboTextureID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferID);
	GLenum status=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer is not complete\n");
		return false;
	}

	for (int i = 0; i < 3; i++)
		mMotion[i] = new GPUMotionDetection(2, 1, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	return true;
}

bool GPUMotionTracking::RenderFBO() {
	for (int i = 0; i < 3; i++)
	{
		if (mHaveTexture[i])
		{
			glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferID);
			glViewport(0,0, cxFBO, cyFBO);
			glClearColor(0, 0, 0, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			mMotion[i]->DrawSelf(0);
			glFlush();
			DetectTracking(i);
			//glBindFramebuffer(GL_FRAMEBUFFER,0);
			AtomicDec(&mHaveTexture[i]);
		}
	}
	return true;
}

void GPUMotionTracking::DestroyFBO() {
	if (_frameBufferID != 0)
	{
		glDeleteRenderbuffers(1, &_depthRenderBuffer);
		glDeleteRenderbuffers(1, &_colorRenderBuffer);
		glDeleteFramebuffers(1, &_frameBufferID);
	}
}


#endif
