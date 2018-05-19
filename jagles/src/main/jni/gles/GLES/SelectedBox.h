//
// Created by LiHong on 16/7/29.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef OSX_SELECTEDBOX_H
#define OSX_SELECTEDBOX_H


#include "ParametricSurface.h"


class SelectedBox : public ParametricSurface {
public:
    SelectedBox(float aspect);
    ~SelectedBox();
    void DrawSelf(int scnindex);
    void SetupBuffer();
    vec3 Evaluate(const vec2& domain) {return vec3(0,0,0);};
    vec2 EvaluateCoord(const vec2& domain) {return vec2(0,0);};
    void UpdateAspect(float aspect) {m_aspect = aspect;};
private:
    float   *mLineCoords;
    GLuint mLineBuffer;
    GLuint mLinebufferSize;
    bool 	mIsUpdatePosition;
};



#endif //OSX_SELECTEDBOX_H
