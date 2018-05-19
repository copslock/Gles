//
// Created by LiHong on 16/5/31.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef OSX_EXPANDFRAMEDRAW_H
#define OSX_EXPANDFRAMEDRAW_H

#include "ParametricSurface.h"


class ExpandFrameDraw : public ParametricSurface ,public IFrameDraw {
public:
    ExpandFrameDraw(float radius, float tbstart, float tbend, float lrstart, float lrend, float aspect);
    ~ExpandFrameDraw();
    void DrawSelf(int scnindex);
    void SetupBuffer();
    vec3 Evaluate(const vec2& domain) {return vec3(0,0,0);};
    vec2 EvaluateCoord(const vec2& domain) {return vec2(0,0);};
    void StartAnimation(int scnindex) {};
    void StopAnimation(int scnindex) {};
    void LoadOSDTexture(int w, int h, GLenum type, GLvoid *pixels) {};
    void LoadOSDTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels) {};
    void PutInfo(GLuint texid, int scnindex) {};
    void ShowOSD(bool value) {};

    int GetFrameType() {return FRAME_TYPE_PANORAMA;}

    void UpdatePosition(float tbstart, float tbend, float lrstart, float lrend);
	void ShowRecordStatu(bool value) {};
	void LoadRecordStatuTexture(GLuint texid, int scnindex) {};
	virtual void* GetLoading(int index) {return 0;};
	virtual void SetIsAnimation(int index, bool animation) {};
	void UpdateAspect(float aspect) {m_aspect = aspect;};
private:
    float	mLeft, mRigth, mTop, mBottom, m_radius;
    float   *mLineCoords;
    GLuint mLineBuffer;
    GLuint mLinebufferSize;
	bool 	mIsUpdatePosition;
};


#endif //OSX_EXPANDFRAMEDRAW_H
