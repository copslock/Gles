#ifndef SPHERESTITCHIMP_H
#define SPHERESTITCHIMP_H

#include "ParametricSurface.h"
#include "SphereStitch.h"
#include "SphereStitch2.h"
#include "sphere.h"

#define STITCH_MODE_CRYSTALBALL 0
#define STITCH_MODE_WORLDMAP    1
#define STITCH_MODE_WORLDMAP_EXT    2



class SphereStitchImp : public ISurface {
public:
	SphereStitchImp(float aspect, void *mgn);
	~SphereStitchImp();
	vec3 Evaluate(const vec2& domain);
	vec2 EvaluateCoord(const vec2& domain);
	void ResetPosition();
	void DrawSelf(int index);

	int  GetScreenCount();
	int  GetScreenMode();
	void SetPosition(vec3 pos, bool texture, int scnindex);
	void SetScale(vec3 scale, bool texture, int scnindex);
	void setRotate(vec3 rotate, bool texture, int scnindex);
	vec3 GetPosition(bool texture, int scnindex);
	vec3 GetScale(bool texture, int scnindex);
	vec3 GetRotate(bool texture, int scnindex);
	void CleanTexture(int scnindex);
	bool LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex);
	void SetTexture(GLuint texid, int scnindex);
	GLuint GetTexture(int scnindex);
	bool NeedDraw();
	void StartAnimation(int scnindex);
	void StopAnimation(int scnindex);
	void SetNoVideoTexture(GLuint texid);
	void LoadOSDTexture(int w, int h, GLenum type, GLvoid *pixels);
	void PutInfo(GLuint texid, int scnindex);
	bool LoadTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels, int scnindex);
	void LoadOSDTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels);
	void SetTexture(GLuint y_texid, GLuint cbcr_texid, int scnindex);
	GLuint GetTexture(int scnindex, int yuv);
	void SetNoVideoTexture(GLuint y_texid, GLuint u_texid, GLuint v_texid);
	void ShowOSD(bool value);
	void AddFrame(IFrameDraw *frame);
	void UpdateFrame(float tbstart, float tbend, float lrstart, float lrend, int index, bool update, bool ani);
	void ShowFrame(bool value);
	void SetBorderColor(GLuint selecttex, GLuint normaltex);
	void SetSelected(bool value, int scnindex);
	int GetSelected(int scnindex);
	void ShowInfo(bool value);
	void ShowRecordStatu(bool value, int index);
	void LoadRecordStatuTexture(GLuint texid, int scnindex);
	void* GetLoading(int index);
	void SetIsAnimation(int index, bool animation);
	//add on 2016-11-21
	void SetOSDTexture(GLuint texid);

	void updateAspect(float aspect);
	void SetDistortion(PDISTORTION_NUM pdist, int dist_count, float half);
	void SetFishEyeParameter(float centerx, float centery, float radius, float anglex, float angley, float anglez, int index);
	void SetTextureAspect(float aspect);
	void SetSingleMode(bool single);
	void SetFactoryMode(int factory);
	void SetMode(int mode, int vertex_mode);
	void RenderFBO();
	GLuint FBOTexture(){return mWorldmap->FBOTexture();};
	void SetFBOTexture(GLint texid){mWorldmap->SetFBOTexture(texid); };


public:
	void *mManager;
private:
	int			  mStitchMode;
	SphereStitch *mWorldmap;
	SphereStitch2 *mCrystalBall;
	sphere		  *mAllInOne;
};

#endif
