//
// Created by Hong Li on 2016/12/11.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef OSX_SPHERESTITCH2_H
#define OSX_SPHERESTITCH2_H

#include "BaseScreen.h"


////////////////�����ṹ��////////////////////
// typedef struct DistTortion {
// 	float angle;
// 	float half_height;
// }TDISTORITION_NUM, *PDISTORTION_NUM;
//////////////////////////////////////////////

typedef struct Dist_list {
    PDISTORTION_NUM mdistortion;
    int	  mdistortion_count;
    float mdistortion_half_height;
    GLuint vertexBuffer;
    int vertexSize;
    GLuint TextureCoordBuffer;
    int TextureCoordSize;
	int triangleIndexCount;
	float blending;
} DISTORTIONLIST,*PDISTORTIONLIST;

class SphereStitch2: public BaseScreen {
public:
    SphereStitch2(float radius,float aspect,void *mgn);
	~SphereStitch2();
    vec3 Evaluate(const vec2& domain);
    vec2 EvaluateCoord(const vec2& domain);
    void TurnLeftRight(GLfloat angle,int scnindex);
    void TurnUpDown(GLfloat angle,int scnindex);
    void setZoom(GLfloat multi,int scnindex);
    void setupProjection();
    void ResetPosition();
    void updateSurfaceTransform();
    void DrawSelf(int index);
    void updateAspect(float aspect){m_aspect = aspect;};
	void SetupBuffer();
	void GenerateVertices(float * vertices, float *texturecorrd);
	void SetDistortion(PDISTORTION_NUM pdist,int dist_count,float half);
	void SetFishEyeParameter(float centerx, float centery, float radius, float anglex, float angley, float anglez,int index);
	void setupProgram(const char *segsrc, const char *fragsrc);
	void setupProgram(const char *segsrc, const char *fragsrc, bool yuv);
    void SetTextureAspect(float aspect){mTexAspect = aspect;};
	void SetSingleMode(bool single);
private:
    float m_radius;
    GLuint mFragRadius1Handle;
	GLuint mFragCenter1Handle;
	GLuint mFragRadius2Handle;
	GLuint mFragCenter2Handle;
	GLuint mFragTexAspectHandle;
	GLuint mSegBlendingHandle;
	GLuint mFragSingleHandle;
    GLuint mSegModeHandle;
	vec2 mCenter[2];
	vec3 mRotateAngle[2];
    float mTexAspect;
	float mCircle_radius[2];
    PDISTORTION_NUM mdistortion;
    int	  mdistortion_count;
    float mdistortion_half_height;
	float mBlending;
    List <PDISTORTIONLIST> mDistList;
	bool mSingle;
	int mMode;
private:
    int GetCurrentDistortion(PDISTORTION_NUM dist);
};


#endif //OSX_SPHERESTITCH2_H
