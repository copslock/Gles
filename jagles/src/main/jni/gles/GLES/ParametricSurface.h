#ifndef __PARAMETRICSURFACE__
#define __PARAMETRICSURFACE__

#include "Vector.h"
#include "GLESMath.h"

#define SCRN_NORMAL 	0
#define SCRN_HEMISPHERE 1
#define SCRN_CYLINDER	2
#define SCRN_EXPAND		3
#define SCRN_UPDOWN		4
#define SCRN_FOUR		5
#define SCRN_VR			6
#define SCRN_SPHERE		7
#define SCRN_THREE		8
#define SCRN_SIX		9
#define SCRN_MOTIONDETECTION 10
#define SCRN_PANORAMA	11
#define SCRN_CIRCLEDETECTION 12
#define SCRN_STITCH		13
#define SCRN_CUP		14
#define SCRN_FOUREX     15
#define SCRN_CONE       16
#define SCRN_VR_SPHERE  17
#define SCRN_STITCH2	18
#define SCRN_OLDHEMISPHERE 19
#define SCRN_ONESCREEN	0x101
#define SCRN_ONENORMAL	0x102
#define SCRN_VECTOROBJECT 0x103
#define SCRN_STITCHONE  0x104


#define VERTEX_TYPE_HEMISPHERE 	0
#define VERTEX_TYPE_PANORAMA	1
#define VERTEX_TYPE_CYLINDER   	2
#define VERTEX_TYPE_EXPAND		3
#define VERTEX_TYPE_NORMAL		4
#define VERTEX_TYPE_SPHERE		5
#define VERTEX_TYPE_CONE		6

#define VERTEX_TYPE_COUNT 7


#define SCRN_SPLIT_ONE	0
#define SCRN_SPLIT_FOUR 1
#define SCRN_SPLIT_SIX	2
#define SCRN_SPLIT_EIGHT 3
#define SCRN_SPLIT_NINE 4
#define SCRN_SPLIT_THIRTEEN 5
#define SCRN_SPLIT_SIXTEEN 6


#define TRANSFORM_POSITION 0
#define TRANSFORM_SCALE    1
#define TRANSFORM_ROTATE   2


#define FRAME_TYPE_PANORAMA 0
#define FRAME_TYPE_HEMISPHERE 1

#ifdef __ANDROID__
#include <android/log.h>
#define GL_INFO(...) {__android_log_print(ANDROID_LOG_INFO, "GLES", __VA_ARGS__);}
#else
#define GL_INFO(...)
#endif

typedef struct distortion_num {
   float angle;
   float half_height;
} DISTORTION_NUM,*PDISTORTION_NUM;


class ParametricManager;


struct IFrameDraw {
	virtual void UpdatePosition(float tbstart, float tbend, float lrstart, float lrend) = 0;
	virtual void DrawSelf(int scnindex) = 0;
	virtual int GetFrameType() = 0;
	virtual void UpdateAspect(float aspect) = 0;
//	virtual void SetFrameColor(unsigned int SelectedColor, unsigned int NormalColor) = 0;
};

struct ISurface
{
	virtual ~ISurface() {}

	virtual void DrawSelf(int scnindex) = 0;
	virtual void ResetPosition() = 0;
	virtual int  GetScreenCount() = 0; 
	virtual int  GetScreenMode() = 0;  
	virtual void SetPosition(vec3 pos, bool texture,int scnindex) = 0;
	virtual void SetScale(vec3 scale, bool texture,int scnindex) = 0;  
	virtual void setRotate(vec3 rotate, bool texture,int scnindex) = 0; 
	virtual vec3 GetPosition(bool texture,int scnindex) = 0;
	virtual vec3 GetScale(bool texture,int scnindex) = 0;
	virtual vec3 GetRotate(bool texture,int scnindex) = 0;
	virtual bool LoadTexture(int w,int h,GLenum type,GLvoid *pixels,int scnindex) = 0;
	virtual void SetTexture(GLuint texid,int scnindex) = 0;
	virtual GLuint GetTexture(int scnindex) = 0;
	virtual bool NeedDraw() = 0;
	virtual void StartAnimation(int scnindex) = 0; 
	virtual void StopAnimation(int scnindex) = 0;  
	virtual void SetNoVideoTexture(GLuint texid) = 0; 
	virtual void LoadOSDTexture(int w,int h,GLenum type,GLvoid *pixels) = 0;
	virtual void PutInfo(GLuint texid,int scnindex) = 0; 
	virtual bool LoadTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels,int scnindex)=0;
	virtual void LoadOSDTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels) = 0;
	virtual void SetTexture(GLuint y_texid,GLuint cbcr_texid,int scnindex) = 0;
	virtual GLuint GetTexture(int scnindex,int yuv) = 0;
	virtual void SetNoVideoTexture(GLuint y_texid,GLuint u_texid,GLuint v_texid)=0;
	virtual void ShowOSD(bool value) = 0;
	virtual void AddFrame(IFrameDraw *frame) = 0;
	virtual void UpdateFrame(float tbstart, float tbend, float lrstart, float lrend, int index,bool update,bool ani) = 0;
	virtual void ShowFrame(bool value) = 0;
	virtual void SetBorderColor(GLuint selecttex, GLuint normaltex)=0;
	virtual void SetSelected(bool value,int scnindex) = 0;
	virtual int GetSelected(int scnindex) = 0;
	virtual void ShowInfo(bool value) = 0;
	virtual void ShowRecordStatu(bool value, int index) = 0;
	virtual void LoadRecordStatuTexture(GLuint texid, int scnindex) = 0;
	virtual void* GetLoading(int index) = 0;
	virtual void SetIsAnimation(int index, bool animation) = 0;
	virtual void CleanTexture(int scnindex) = 0;
	//add on 2016-11-21
	virtual void SetOSDTexture(GLuint texid) = 0;
};

struct ParametricInterval
{
    GLfloat angleSpan;
    vec2 StartAngle;
    vec2 EndAngle;
};

class ParametricSurface : public ISurface
{
public:
	void SetVertexFlags(unsigned char flags = 0);

	ParametricSurface();
	virtual ~ParametricSurface();

	virtual void DrawSelf(int scnindex);
	virtual void SetupBuffer();
	//	virtual void TurnLeftRight(GLfloat angle,int scnindex)=0;
	//	virtual void TurnUpDown(GLfloat angle,int scnindex)=0;
	virtual void ResetPosition();
	void setZoom(GLfloat multi, int scnindex);
	int  GetScreenCount();


	virtual void SetPosition(vec3 pos, bool texture, int scnindex);
	virtual void SetScale(vec3 scale, bool texture, int scnindex);
	virtual void setRotate(vec3 rotate, bool texture, int scnindex);

	vec3 GetPosition(bool texture, int scnindex);
	vec3 GetScale(bool texture, int scnindex);
	vec3 GetRotate(bool texture, int scnindex);

	virtual void CleanTexture(int scnindex);
	virtual bool LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex);
	virtual bool LoadTexture(int w, int h, void* buffer, int scnindex);
	void SetTexture(GLuint texid, int scnindex);
	GLuint GetTexture(int scnindex);
	void SetNoVideoTexture(GLuint texid);

	virtual bool LoadTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels, int scnindex);
	void SetTexture(GLuint y_texid, GLuint cbcr_texid, int scnindex);
	GLuint GetTexture(int scnindex, int yuv);
	void SetNoVideoTexture(GLuint y_texid, GLuint u_texid, GLuint v_texid);

	virtual bool NeedDraw();

	void SetVisible(bool value);
	virtual void LoadOSDTexture(int w, int h, GLenum type, GLvoid *pixels) {};
	virtual void LoadOSDTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels) {};
	virtual void PutInfo(GLuint texid,int scnindex) {};
	virtual void ShowOSD(bool value) {};
	virtual void AddFrame(IFrameDraw *frame) {};
	virtual void UpdateFrame(float tbstart, float tbend, float lrstart, float lrend,int index,bool update,bool ani) {};
	virtual void ShowFrame(bool value){};
	virtual void SetSelected(bool value,int scnindex){};
	virtual int GetSelected(int scnindex) { return false;};
	virtual void ShowRecordStatu(bool value, int index) {};
	virtual void LoadRecordStatuTexture(GLuint texid, int scnindex){};
	virtual void* GetLoading(int index){ return 0;};
	virtual void SetIsAnimation(int index, bool animation){};
public:
	GLboolean Visible;
	int _scnindex;
protected:
	int GetVertexSize();
	int GetVertexCount();
	int GetCoordSize();
	int GetCoordCount();
	int  GetScreenMode();

	virtual void GenerateVertices(float * vertices,float *texturecorrd);
    void SetInterval(const ParametricInterval& interval);
	virtual void setupProgram(const char *segsrc,const char *fragsrc);
	virtual void setupProgram(const char *segsrc,const char *fragsrc,bool yuv);
    virtual bool InvertNormal(const vec2& domain) { return false; }
	virtual void setupProjection();
	virtual void updateSurfaceTransform();
	void Cleanup();
	vec2 ComputeDomain(float i, float j);
	vec2 ComputeCoordDomain(float i, float j);
	GLuint LoadShader(GLenum type, const char *src);
	virtual vec3 Evaluate(const vec2& domain) = 0;
	virtual vec2 EvaluateCoord(const vec2& domain) = 0;
	virtual void StartAnimation(int scnindex) {};
	virtual void StopAnimation(int scnindex) {};
	virtual void SetBorderColor(GLuint selecttex, GLuint normaltex) {};
	virtual void ShowInfo(bool value) {};
	virtual void SetOSDTexture(GLuint texid) {};
protected:
    //ivec2 m_slices;
    //ivec2 m_divisions;
	GLfloat anglespan;
    vec2 m_startangle;
	vec2 m_endangle;
	GLuint _programHandle;
	GLuint _positionSlot;
	GLint _coordSlot;
	GLuint _modelViewSlot;
	GLuint _projectionSlot;

	KSMatrix4 _modelViewMatrix;
	KSMatrix4 _projectionMatrix;
	KSMatrix4 _rotationMatrix;
	int triangleIndexCount;
	int mxStart;
	int mxEnd;
	GLuint
			vertexBuffer;
	int vertexSize;
	GLuint TextureCoordBuffer;
	int TextureCoordSize;

	int mScreenCount;
	int mScreenMode;

	vec3 mPosition;
	vec3 mScale;
	vec3 mRotate;
	vec3 mTexPosition;
	vec3 mTexScale;
	vec3 mTexRotate;
	GLboolean mNeedDraw;
	bool mIsYUV;
	GLuint lvb;
	bool mIsHardware;

public:
	float 	m_aspect;
	GLfloat _Zoom;
	GLfloat _udCurAngle;
	GLint 	_uniformSamplers[3];
	GLuint 	_yuvtexid[3];
	GLuint  _NoVideoTexID;
	GLuint  _yuvNoVideoTexID[3];
	int 	_texwidth;
	int 	_texheight;
	GLuint  _texid;
	void *_Manager;
public:
	int DebugIndex;
};

#endif
