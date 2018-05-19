#pragma once
#include "NormalPlay.h"
#include "Hemisphere.h"
class SixScreen :
	public ISurface
{
public:
	SixScreen(float aspect, void *mgn);
	virtual ~SixScreen();
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
	virtual void LoadOSDTexture(int w, int h, GLenum type, GLvoid *pixels) {};
	virtual void LoadOSDTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels) {};
	virtual void TurnLeftRight(GLfloat angle, int scnindex) {};
	virtual void TurnUpDown(GLfloat angle, int scnindex) {};
	void PutInfo(GLuint texid, int scnindex) { mNormal->PutInfo(texid, scnindex); }

	bool LoadTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels, int scnindex);
	void SetTexture(GLuint y_texid, GLuint cbcr_texid, int scnindex);
	GLuint GetTexture(int scnindex, int yuv);
	void SetNoVideoTexture(GLuint y_texid, GLuint u_texid, GLuint v_texid);
	void ShowOSD(bool value) {};

	void AddFrame(IFrameDraw *frame) {};
	void UpdateFrame(float tbstart, float tbend, float lrstart, float lrend,int index,bool update,bool ani);
	void ShowFrame(bool value) {};
	void SetBorderColor(GLuint selecttex, GLuint normaltex);
	void SetSelected(bool value, int scnindex);
	int GetSelected(int scnindex);
	void ShowInfo(bool value) {};
	void LoadRecordStatuTexture(GLuint texid, int scnindex) {};
	virtual void* GetLoading(int index) {return 0;};
	virtual void SetIsAnimation(int index, bool animation) {};
private:
	Hemisphere *mNormal;
	Hemisphere *mVideo[5];
	int		   mSelected;
	float		m_aspect;
	void		*mManager;
};

