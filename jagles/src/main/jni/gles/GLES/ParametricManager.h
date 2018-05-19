//
// Created by LiHong on 16/2/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef IOS_PARAMETRICANIMATION_H
#define IOS_PARAMETRICANIMATION_H

#include "ParametricSurface.h"
#include <stdint.h>
#include <pthread.h>
#include "../Utils/list.h"
#ifndef EMSCRIPTEN
#include "../Media/OpenALManager.h"
#include "../Network/ConnectManager.h"
#endif

#if defined(__USE_NETWORK__)
#include "../Network/JAConnect.h"
#include "../Media/JAText.h"
#include "../GLES/CircleFrame.h"
#endif
#ifdef __ANDROID__
#include <unistd.h>
#else
#ifndef EMSCRIPTEN

#endif
#endif


#define JA_ANI_LOOP 0
#define JA_ANI_STEP 1

#define JA_ANI_TYPE_POSITION 0
#define JA_ANI_TYPE_SCALE 1
#define JA_ANI_TYPE_ROTATE 2
#define JA_ANI_TYPE_TRANSPARENT 3
#define JA_ANI_LOOKAT	4

#define ACTION_MOVE_SPEED 0
#define ACTION_MOVE_INERTIA 1
#define ACTION_MOVE_SCALE 2

class ParametricManager;

typedef void(*OnAnimationEnd)(ParametricManager *, void *);

typedef struct JAAnimation {
	ISurface *_obj;
	vec3 _start;
	vec3 _value;
	vec3 _unit;
	int _step;
	int _stepcount;
	vec3 _end;
	int  _isLoop;
	int  _istexture;
	int  _type;
	int  duration;
	int  index;
	bool Inertia;
	OnAnimationEnd OnEnd;
	void  *Ctx;
} JA_Animation, *PJA_Animation;

class ParametricManager {
public:
	void(*OnDrawRequest)(ISurface *obj, void *ctx);
	void(*OnTextureAvaible)(int w, int h, void *frame, int frame_len, int frame_type, int timestamp, int index, void *ctx);
	void(*OnObjectHidden)(ParametricManager *mgn, ParametricSurface *obj, OnAnimationEnd pOnEnd, void *ctx, int index);
	void(*OnObjectWillVisible)(ParametricManager *mgn, ParametricSurface *obj, vec3 pos, vec3 scale, vec3 rotate);
	void(*OnOSDTextureAvaible)(void *con, void *frame, int frame_len, void *ctx);

	ParametricManager(float aspect, void *ctx, char *apppath);
	~ParametricManager();
	/**
	* 显示连接视频的加载动画
	*/
	void ConnectVideoLoading(int index);
	/**
	* 关闭视频后清理动画资源
	*/
	void CloseVideoLoading(int index);
	/**
	* 音频相关，因为播放文件时也需要播放音频，暂时在此处理
	*/
	void PlayAudioData(uint8_t *buf, int len);
	void EnableAudio(bool value) { mPlayaudio = value; };
	void PauseAudioPlaying();       /* 暂停音频播放(Will clear buf queue in OpenAL) */
	void OpenAudioPlaying();

	/**
	 * 选中某个视频
	 *
	 */
	void SetSelected(int x, int y, int w, int h);
	void SetSelected(int index);

	int getVideoIndex();

	int GetScreenCount();
	bool GetVisibility(int index);
	void SetVisibility(bool value, int index);
	void ReSizeSplite(float aspect);

    void SetOutsideChanged(bool changed) { outsideChanged = changed; }
#pragma mark -compareLaseDay
	/*!
	*StartAnimation 创建一个动画
	 * param       obj,一个ISurface对象,可以是全景\圆柱\展开\前后\四分屏等方式
	 * param       pend,物体动画的的结束值,可以是放大缩小倍数\位置\旋转角度
	 * param       step,分多少步执行完毕
	 * param       duration,动画执行时间
	 * param       isloop,是否是循环动画,假如是循环动画,duration为每次执行的时间间隔
	 * param       anitype,动画类型,可以为JA_ANI_TYPE_POSITION\JA_ANI_TYPE_SCALE\JA_ANI_TYPE_ROTATE
	 * param       texture,是物体动画,还是物体里的贴图动画
	 * param       scrindex,屏幕索引,只对SCRN_NORMAL模式有效
	 * param       Inertia,是否采用惯性方式动画,目前是正玄曲线,即是开始快后来慢
	 * param       ponend,动画结束时执行的回调
	 * return      返回动画句柄,可以给StopAnimation调用来终止动画.
	*/
	long StartAnimation(ISurface *obj, vec3 pend, int step, int duration, int isloop, int anitype, bool texture, int scrIndex, bool Inertia, OnAnimationEnd ponend, void *ctx);
	void StopAnimation(long pHandle);
	void ClearAnimation();
	void ClearAnimation(int index);
	ISurface *CurrentParametric(); //获取当前激活的物体
	ISurface *GetScreen(int index);
	void DoAnimationThread();
	void *GetCtx();
	void SwitchMode(int mode);
	int GetMode();
    int GetDualMode();
    void RenderFBO();
	GLuint FBOTexture();
	void SetFBOTexture(GLint texid);
	void SetSingVideo(int index, bool ani);

	bool LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex);

	void cleanTexture(int index);

	int GetTextureWidth() { return _texwidth; };
	int GetTextureHeight() { return _texheight; };
	void LoadRecordStatuTexture(GLint recordStatusTextID);
	void SetTextureWidthHeight(int w, int h) { _texwidth = w; _texheight = h; };
	void PutInfo(char *txt, int index);
	void SetSplit(int index, bool ani);
	void SetPage(int index, bool ani);
	int  GetPage();
	int  GetAllPage();
	void SetAllPage(int allPage);
	void InitBorderTexture(uint32_t scolor, uint32_t ncolor);
	int GetSplitMode();
	int			_audioIndex;

	void AnimationOperator(void *arg);

	bool    isDestroying;
#if defined(__USE_NETWORK__)
	JAText      *_Textput;
#endif

	//统一手势代码
	void SetWindowWidthHeight(int w, int h) { window_width = w;window_height = h; };

	void DoDoubleTap(int wallmode, int scene, int scrnFourIndex, int scindex);

	void DoTapOrMouseDown(int x, int y);

	void DoTapOrMouseMove(int x, int y, int wallmode, int scindex);

	void DoTapOrMouseUp(int x, int y, int wallmode);

	void DoTapOrMouseWheel(int intra, int x, int y, int scindex, int wallMode);

	int GetScreenIndex(int x, int y, int screenmode);

	void DoUpdateFrame(float tbstart, float tbend, float lrstart, float lrend, int index, bool update, bool ani);

	//解决转屏椭圆问题

	void UpdateAspect(float aspect);

	//添加文字效果

	void SetTextMask(char *txt);

	//设置没有显示模式的位置、放大缩小、旋转

	void TransformVertex(int mode, vec3 vect, bool texture, int type, int scrnindex);
	bool	_isUseDirectTexture;

	void EnableFactoryMode(int value) {
#if !defined(__LINUXNVR__)  && !defined(EMSCRIPTEN)
		mShowCircle = value;
#endif
	};
	bool Get_FM_CircleOptions(float *centerx, float *centery, float *diameter, int index);
	void Set_FM_CircleOptions(float centerx, float centery, float diameter, int index);
	void RenderCircle();

	// View of Angle
	float GetViewAngle() {
		if(_current_mode==SCRN_CUP)
			return 120.0;
		else
			return mViewAngle;
	};
	void SetViewAngle(float angle);

	// overlay LOGO
	void LoadLogo(char *fn);
	void RenderLogo();
	void GetLogo(GLuint *texid, int *logowidth, int *logoheight) { *texid = mTexLogoID, *logowidth = mLogoWidth; *logoheight = mLogoHeight; };
	void SetLogoLoading(bool value) { mLogoLoading = value; };
	bool GetLogoLoading() { return mLogoLoading; };
	void GetWindowWH(int *ww, int *wh) { *ww = window_width; *wh = window_height; };
	//显示格子
	void EnableGrid(bool value);

	void LoadOSDTexture(int w, int h, GLenum type, GLvoid *pixels);
#ifndef EMSCRIPTEN
	void DoDirectTextureFrameUpdata(ConnectManager *conn, int w, int h, void * directBuffer, int index);
	void DoDirectTextureOSDFrameUpdata(ConnectManager *conn, int w, int h, void * textBuffer, int index);
#endif
	void AdjustActionExperience(int type, int actionType, float value);
	/*void SetDistortion(PDISTORTION_NUM pdist, int dist_count, float half)
	{
		mdistortion = pdist;
		mdistortion_count = dist_count;
		mdistortion_half_height = half;
	};
	void SetFishEyeParameter(float centerx, float centery, float radius, float anglex, float angley, float anglez, int index);*/

    void SetFishEyeParameters(float centerX, float centerY, float radius, float angleX, float angleY, float angleZ, int index,bool v720);
    void SetDistortion(void *pdist, int dist_size);

    void SetStitch2TextureAspect(float pAspect);

	void SetKeepAspect(float aspect,int scnindex);
	float GetKeepAspect(int scnindex);

private:
	//统一手势代码
	int lastx;
	int lasty;
	int startx;
	int starty;
	int mSpeed;  /* 手指瞬间速度 */
	int starttime;
	int mIndex;
	int mQueued;        /* OpenAL队列中的buf数 */
	int mProcessed;     /* OpenAL队列中播放完成的buf数 */
	float limit_left, limit_top, limit_right, limit_bottom;
	int                         window_width;
	int 						window_height;

	float hemisphereSpeel;
	float hemisphereInertia;
	float adjustableScale;

	List <PJA_Animation> _Ani;
	List <ISurface *> ParametricList;
	pthread_t _AniLoop;
	pthread_mutex_t _AniLock;

	int         _current_screen;
	int         _current_mode;
	ISurface    *_current;
	bool        mAniRun;
	void        *_ctx;
	GLuint       _NoVideotexID;
	char        AppPath[1024];
	void InitNoVideoTexture();
	GLuint GenColorTexture(uint32_t color);
	int 	_texwidth;
	int 	_texheight;
	GLuint  _texid;
	int     _screenlock;
	bool                            mPlayaudio;
	float _aspect;
	//Factory Mode
#if !defined(__LINUXNVR__)  && !defined(EMSCRIPTEN)
	CircleFrame					*mCircle[2];
	int 						mShowCircle;
	float						mDiameter[2];
	int 						mCurrentCircle;
#endif
	//View of Angle
	float						mViewAngle;
	//overlay LOGO
	GLuint						mTexLogoID;
	ISurface					*mLogo;
	int							mLogoWidth, mLogoHeight;
	bool						mLogoLoading;

	vec2 mCenter[2];
	vec3 mRotateAngle[2];
	float mTexAspect;
	float mCircle_radius[2];

    //四分屏双击需要
    //双击窗口的index
    int windowIndex;
    //是否外部改变显示模式
    bool outsideChanged;

	//PDISTORTION_NUM mdistortion;
	/*int	  mdistortion_count;
	float mdistortion_half_height;*/
};


#endif //IOS_PARAMETRICANIMATION_H
