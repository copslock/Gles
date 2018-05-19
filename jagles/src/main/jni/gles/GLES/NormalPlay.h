//
// Created by LiHong on 16/1/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef FISHEYE_NORMALPLAY_H
#define FISHEYE_NORMALPLAY_H
#include "ParametricSurface.h"
#include "BaseScreen.h"


class NormalPlay : public BaseScreen {
public:
	NormalPlay(float radius,float aspect,float left,float top,void *mgn);
	vec3 Evaluate(const vec2& domain);
    vec2 EvaluateCoord(const vec2& domain);
    void TurnLeftRight(GLfloat angle,int scnindex);
    void TurnUpDown(GLfloat angle,int scnindex);
    void setZoom(GLfloat multi,int scnindex);
    void setupProgram(const char *segsrc,const char *fragsrc);
    void setupProgram(const char *segsrc,const char *fragsrc,bool yuv);
    void DrawSelf(int scnindex);
    void ResetPosition();
    float getHeight(){return m_radius;};
	void SetPosition(vec3 pos, bool texture, int scnindex);
	void SetScale(vec3 scale, bool texture, int scnindex);
	void ReSetBuffer(float aspect);

	void SetVisible(bool value) { Visible = value; };
    //NormalPlay 私有方法

	float GetKeepAspect() { return mKeepAspect;};
	void  SetKeepAspect(float aspect){ mKeepAspect = aspect;};
private:
    float m_radius;
    float mLeft;
    float mTop;
    GLint mZoomHandle;
    GLint mLeftOffsetHandle;
    GLint mTopOffsetHandle;
	GLint mKeepAspectHandle;
    GLfloat mZoom;
    GLfloat mLeftOffset;
    GLfloat mTopOffset;
	GLfloat mKeepAspect;
    int posx;
    int posy;
    int posw;
};

#endif //FISHEYE_NORMALPLAY_H
