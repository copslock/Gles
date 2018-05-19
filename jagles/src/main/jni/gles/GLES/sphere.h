//
// Created by LiHong on 16/4/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef IOS_SPHERE_H
#define IOS_SPHERE_H


#include "BaseScreen.h"

class sphere : public BaseScreen {
public:
    sphere(float radius,float aspect,void *mgn) ;
	~sphere();
	vec3 Evaluate(const vec2& domain);
	vec2 EvaluateCoord(const vec2& domain);
	void setupProjection();
	void ResetPosition();
	void DrawSelf(int scnindex);
	void SetupBuffer();
	void SwitchMode(int mode);
	void setupProgram(const char *segsrc, const char *fragsrc);
	void setupProgram(const char *segsrc, const char *fragsrc, bool yuv);
	void updateSurfaceTransform();
	void updateSurfaceTransform(KSMatrix4 *_proj, KSMatrix4 *_model, KSMatrix4 *_rotate, float zoom, vec3 pPos, vec3 pScale, vec3 pRotate, GLuint projslot, GLuint modelslot, int atype);
	void LoadRecordStatuTexture(GLuint texid, int scnindex) {};
	virtual void* GetLoading() { return 0; };
	void SetChangeAnimation(bool Start, int step, int stepcount, int nexttype);
	void SetChangePSR(vec3 pos, vec3 scale, vec3 rotate, vec3 tpos, vec3 tscale, vec3 trotate);
	void SetTextTexture(GLuint texid) { mTexText = texid; };
	void UpdateViewportAspect(float aspect) { mViewportAspect = aspect; };
	void EnableGrid(bool value) { mShowGrid = value; };
	void DirectTextureFrameUpdata(int w, int h, void * directBuffer, int index);
	void CreateDirectTexture();
	void SetIsUseDirectTexture(bool value) { isUseDirectTexture = value; directTextureBuffer = NULL; _texwidth = 0; _texheight = 0; };
	bool   isUseDirectTexture;
	void UpdateMaskFrame(float left, float right, float top, float bottom, int index);
	void ClearMaskFrame();
private:
	float m_radius;
	KSMatrix4 mChangemodelViewMatrix;
	KSMatrix4 mChangeprojectionMatrix;
	KSMatrix4 mChangeRoateMatrix;
	GLuint mVertexVBOBuffer[VERTEX_TYPE_COUNT];
	GLuint mVertexSize[VERTEX_TYPE_COUNT];
	GLuint mTriangleIndexCount[VERTEX_TYPE_COUNT];
	GLuint mTextureCoordSize[VERTEX_TYPE_COUNT];
	GLuint mTextureCoordBuffer[VERTEX_TYPE_COUNT];
	GLuint mSegModeHandle;
	GLuint mSegRotateHandle;
	GLuint mSegDiameterHandle;
	GLuint mSegTexWidthHandle;
	GLuint mSegTexHeightHandle;
	GLuint mSegAspectHandle;
	GLuint mSegChangeAnimationHandle;
	GLuint mSegChangeStepHandle;
	GLuint mSegChangePositionHandle;
	GLuint mSegChangeStepCountHandle;
	GLuint mSegChangeModeHandle;
	GLuint mSegChangeProjHandle;
	GLuint mSegChangeModelHandle;
	GLuint mSegChangeDiameterHandle;
	GLuint mSegChangeRotateHandle;
	GLuint mSegViewportAspectHandle;
	GLuint mFragSolutionHandle;
    GLuint mFragModeHandle;

	GLuint mTexText;
	GLuint mFragTexture;
	GLuint mFragTexText;
	vec3   nPosition;
	vec3   nScale;
	vec3   nRotate;
	vec3   nTexPosition;
	vec3   nTexScale;
	vec3   nTexRotate;
	float  mViewportAspect;

	bool   mChangeAnimation;
	int    mChangeType;
	int    mChangeStep;
	int    mChangeStepCount;
	int    mVertexType;
	bool   mShowGrid;
	ISurface *mDrawSelected;
	void * directTextureBuffer;
	int	   direct_width;
	int    direct_height;
};

#endif //IOS_SPHERE_H
