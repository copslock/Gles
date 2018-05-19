//
// Created by LiHong on 16/6/9.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef OSX_GPUMotionDetection_H
#define OSX_GPUMotionDetection_H

#include "ParametricSurface.h"
#include "BaseScreen.h"

class GPUMotionDetection : public ParametricSurface {
public:
    GPUMotionDetection(float radius, float aspect, void *mgn);
	vec3 Evaluate(const vec2& domain);
	vec2 EvaluateCoord(const vec2& domain);
	void setZoom(GLfloat multi, int scnindex);
	void setupProgram(const char *segsrc, const char *fragsrc);
	void setupProgram(const char *segsrc, const char *fragsrc, bool yuv);
	void DrawSelf(int scnindex);
	bool LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex);
	void ShowRecordStatu(bool value) {};
	void LoadRecordStatuTexture(GLuint texid, int scnindex) {};
	virtual void* GetLoading(int index) {return 0;};
	virtual void SetIsAnimation(int index, bool animation) {};

	//NormalPlay 私有方法
private:
	float m_radius;
	GLuint mLastTexID;
	GLuint mCurrentTexID;
	GLint 	_Samplers[2];
};


#endif //OSX_GPUMotionDetection_H
