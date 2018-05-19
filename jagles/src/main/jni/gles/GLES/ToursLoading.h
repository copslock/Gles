//
// Created by LiHong on 16/2/21.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef IOS_TOURSLOADING_H
#define IOS_TOURSLOADING_H


#include "ParametricSurface.h"

class ToursLoading : public ParametricSurface{
public:
    ToursLoading(float majorRadius,float minorRadius,float aspect);
    vec3 Evaluate(const vec2& domain);
    vec2 EvaluateCoord(const vec2& domain);
    void TurnLeftRight(GLfloat angle,int scnindex);
    void TurnUpDown(GLfloat angle,int scnindex);
    void setZoom(GLfloat multi,int scnindex);
    void ResetPosition();
    void StartAnimation(int scnindex);
    void StopAnimation(int scnindex);
    virtual void LoadOSDTexture(int w,int h,GLenum type,GLvoid *pixels) {};
    virtual void LoadOSDTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels){};
    void PutInfo(GLuint texid,int scnindex){};
	void ShowOSD(bool value) {};
	void LoadRecordStatuTexture(GLuint texid, int scnindex) {};
	void ReSizeSplite(float aspect);
	virtual void* GetLoading(int index) {return 0;};
	virtual void SetIsAnimation(int index,bool animation) {};
private:
    float m_majorRadius;
    float m_minorRadius;
};


#endif //IOS_TOURSLOADING_H
