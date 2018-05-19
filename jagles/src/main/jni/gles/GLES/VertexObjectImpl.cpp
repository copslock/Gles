//
// Created by LiHong on 16/6/23.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef _MSC_VER
#include <unistd.h>
#endif
#include "VertexObjectImpl.h"
#include "ParametricManager.h"
#include "NormalPlay.h"
#include "ExpandFrameDraw.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ImageLoading.h"
#define LOGV(...)__android_log_print(ANDROID_LOG_DEBUG,"Lee",__VA_ARGS__)

#define VMODE_COUNT 12

const vec3 _DP = vec3(0,0,0);
const vec3 _DS = vec3(1,1,1);
const vec3 _DR = vec3(0,0,0);

const vec3 _DR1 = vec3(60,0,0);
const vec3 _DP1 = vec3(0,0,3);
const vec3 _DS1 = vec3(1.0,1.0,1.0);
const vec3 _DP2 = vec3(0,0,2);

const VO_MATRIX _MATRIX = {_DP,_DS,_DR,_DP,_DS,_DR};

//VO_MODEITEM VMODE1[1] = {{{0,0,12,12},_MATRIX,_MATRIX,VERTEX_TYPE_NORMAL,true,false}};
VO_MODEITEM VMODE1[1] = {{{0,0,12,12},_MATRIX,_MATRIX,VERTEX_TYPE_HEMISPHERE,true,false}};
VO_MODEITEM VMODE2[1] = {{{0,0,12,12},{vec3(0,0,-6),_DS,vec3(30,180,0),_DP,_DS,_DR},{vec3(0,0,-6),_DS,vec3(30,180,0),_DP,_DS,_DR},VERTEX_TYPE_CYLINDER,true,false}};
VO_MODEITEM VMODE3[1] = {{{0,0,12,12},_MATRIX,_MATRIX,VERTEX_TYPE_EXPAND,true,false}};
VO_MODEITEM VMODE4[2] = {{{-12,6,24,6},_MATRIX,_MATRIX,VERTEX_TYPE_PANORAMA,true,false},{{0,0,24,6},_MATRIX,_MATRIX,VERTEX_TYPE_PANORAMA,true,false}};
VO_MODEITEM VMODE5[4] = {{{0,0,6,6},{_DP1,_DS,_DR1,_DP,_DS,_DR},{_DP1,_DS,_DR1,_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{6,0,6,6},{_DP1,_DS,_DR1+vec3(0,0,-90),_DP,_DS,_DR},{_DP1,_DS,_DR1+vec3(0,0,-90),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{0,6,6,6},{_DP1,_DS,_DR1+vec3(0,0,-180),_DP,_DS,_DR},{_DP1,_DS,_DR1+vec3(0,0,-180),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{6,6,6,6},{_DP1,_DS,_DR1+vec3(0,0,-270),_DP,_DS,_DR},{_DP1,_DS,_DR1+vec3(0,0,-270),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false}};
VO_MODEITEM VMODE6[3] = {{{0,4,6,8},{_DP1,_DS,_DR1,_DP,_DS,_DR},{_DP1,_DS,_DR1,_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{6,4,6,8},{_DP1,_DS,_DR1+vec3(0,0,90),_DP,_DS,_DR},{_DP1,_DS,_DR1+vec3(0,0,90),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{0,0,12,4},_MATRIX,_MATRIX,VERTEX_TYPE_PANORAMA,false,true}};
VO_MODEITEM VMODE7[6] = {{{8,8,4,4},{_DP1,_DS,_DR1+vec3(0,0,60),_DP,_DS,_DR},{_DP1,_DS,_DR1+vec3(0,0,60),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{8,4,4,4},{_DP1,_DS,_DR1+vec3(0,0,120),_DP,_DS,_DR},{_DP1,_DS,_DR1+vec3(0,0,120),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{0,0,4,4},{_DP1,_DS,_DR1+vec3(0,0,180),_DP,_DS,_DR},{_DP1,_DS,_DR1+vec3(0,0,180),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{4,0,4,4},{_DP1,_DS,_DR1+vec3(0,0,240),_DP,_DS,_DR},{_DP1,_DS,_DR1+vec3(0,0,240),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{8,0,4,4},{_DP1,_DS,_DR1+vec3(0,0,300),_DP,_DS,_DR},{_DP1,_DS,_DR1+vec3(0,0,300),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{0,4,8,8},_MATRIX,_MATRIX,VERTEX_TYPE_HEMISPHERE,true,true}};
VO_MODEITEM VMODE8[2] = {{{0,0,6,12},{_DP1,_DS,_DR1,_DP,_DS,_DR},{_DP1,_DS,_DR1,_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{6,0,6,12},{_DP1,_DS,_DR1,_DP,_DS,_DR},{_DP1,_DS,_DR1,_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false}};
VO_MODEITEM VMODE9[1] = {{{0,0,12,12},{_DP,vec3(1,0.5,1),_DR,_DP,_DS,_DR},{_DP,vec3(1,0.5,1),_DR,_DP,_DS,_DR},VERTEX_TYPE_PANORAMA,true,false}};
VO_MODEITEM VMODE10[1] = {{{0,0,12,12},{_DP,_DS,_DR,_DP,_DS,_DR},{_DP,_DS,_DR,_DP,_DS,_DR},VERTEX_TYPE_SPHERE,true,false}};
VO_MODEITEM VMODE11[1] = { { { 0,0,12,12 },{ _DP,vec3(0.85,0.85,0.85),vec3(60,0,0),_DP,_DS,_DR },{ _DP,vec3(0.85,0.85,0.85),vec3(60,0,0),_DP,_DS,_DR },VERTEX_TYPE_HEMISPHERE,true,false } };
VO_MODEITEM VMODE12[4] = { {{8,8,4,4},{ _DP1,_DS,_DR1 + vec3(0,0,90),_DP,_DS,_DR } ,{ _DP1,_DS,_DR1 + vec3(0,0,90),_DP,_DS,_DR } ,VERTEX_TYPE_HEMISPHERE,false,false },{{8,4,4,4},{ _DP1,_DS,_DR1 + vec3(0,0,180),_DP,_DS,_DR } ,{ _DP1,_DS,_DR1 + vec3(0,0,180),_DP,_DS,_DR } ,VERTEX_TYPE_HEMISPHERE,false,false},{{0,0,12,4},_MATRIX,_MATRIX,VERTEX_TYPE_PANORAMA,false,true },{{0,4,8,8},_MATRIX, _MATRIX, VERTEX_TYPE_HEMISPHERE,true,true } };
//VO_MODEITEM VMODE12[3] = { { { 0,4,6,8 },{ _DP1,_DS,_DR1,_DP,_DS,_DR },{ _DP1,_DS,_DR1,_DP,_DS,_DR },VERTEX_TYPE_HEMISPHERE,false,false },{ { 6,4,6,8 },{ _DP1,_DS,_DR1 + vec3(0,0,90),_DP,_DS,_DR },{ _DP1,_DS,_DR1 + vec3(0,0,90),_DP,_DS,_DR },VERTEX_TYPE_HEMISPHERE,false,false },{ { 0,0,12,4 },_MATRIX,_MATRIX,VERTEX_TYPE_PANORAMA,false,true } };
VO_MODEITEM DUAL_VMODE1[1] = {{{0,0,12,12},_MATRIX,_MATRIX,VERTEX_TYPE_HEMISPHERE,true,false}};
VO_MODEITEM DUAL_VMODE2[1] = {{{0,0,12,12},{vec3(0,0,-6),_DS,vec3(-30,0,0),_DP,_DS,_DR},{vec3(0,0,-6),_DS,vec3(-30,0,0),_DP,_DS,_DR},VERTEX_TYPE_CYLINDER,true,false}};
VO_MODEITEM DUAL_VMODE3[1] = {{{0,0,12,12},_MATRIX,_MATRIX,VERTEX_TYPE_EXPAND,true,false}};
VO_MODEITEM DUAL_VMODE4[2] = {{{-12,6,24,6},_MATRIX,_MATRIX,VERTEX_TYPE_PANORAMA,true,false},{{0,0,24,6},_MATRIX,_MATRIX,VERTEX_TYPE_PANORAMA,true,false}};
VO_MODEITEM DUAL_VMODE5[4] = {{{0,0,6,6},{_DP2,_DS,_DR,_DP,_DS,_DR},{_DP2,_DS,_DR,_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{6,0,6,6},{_DP2,_DS,_DR+vec3(0,-90,0),_DP,_DS,_DR},{_DP2,_DS,_DR+vec3(0,-90,0),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{0,6,6,6},{_DP2,_DS,_DR+vec3(0,-180,0),_DP,_DS,_DR},{_DP2,_DS,_DR+vec3(0,-180,0),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{6,6,6,6},{_DP2,_DS,_DR+vec3(0,-270,0),_DP,_DS,_DR},{_DP2,_DS,_DR+vec3(0,-270,0),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false}};
VO_MODEITEM DUAL_VMODE6[3] = {{{0,4,6,8},{_DP2,_DS,_DR,_DP,_DS,_DR},{_DP2,_DS,_DR,_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{6,4,6,8},{_DP2,_DS,_DR+vec3(0,90,0),_DP,_DS,_DR},{_DP2,_DS,_DR+vec3(0,90,0),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{0,0,12,4},_MATRIX,_MATRIX,VERTEX_TYPE_PANORAMA,false,true}};
VO_MODEITEM DUAL_VMODE7[6] = {{{8,8,4,4},{_DP2,_DS,_DR+vec3(0,60,0),_DP,_DS,_DR},{_DP2,_DS,_DR+vec3(0,60,0),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{8,4,4,4},{_DP2,_DS,_DR+vec3(0,120,0),_DP,_DS,_DR},{_DP2,_DS,_DR+vec3(0,120,0),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{0,0,4,4},{_DP2,_DS,_DR+vec3(0,180,0),_DP,_DS,_DR},{_DP2,_DS,_DR+vec3(0,180,0),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{4,0,4,4},{_DP2,_DS,_DR+vec3(0,240,0),_DP,_DS,_DR},{_DP2,_DS,_DR+vec3(0,240,0),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{8,0,4,4},{_DP2,_DS,_DR+vec3(0,300,0),_DP,_DS,_DR},{_DP2,_DS,_DR+vec3(0,300,0),_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{0,4,8,8},_MATRIX,_MATRIX,VERTEX_TYPE_HEMISPHERE,true,true}};
VO_MODEITEM DUAL_VMODE8[2] = {{{0,0,6,12},{_DP1,_DS,_DR1,_DP,_DS,_DR},{_DP1,_DS,_DR1,_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false},{{6,0,6,12},{_DP1,_DS,_DR1,_DP,_DS,_DR},{_DP1,_DS,_DR1,_DP,_DS,_DR},VERTEX_TYPE_HEMISPHERE,false,false}};
VO_MODEITEM DUAL_VMODE9[1] = {{{0,0,12,12},{_DP,vec3(1,0.5,1),_DR,_DP,_DS,_DR},{_DP,vec3(1,0.5,1),_DR,_DP,_DS,_DR},VERTEX_TYPE_PANORAMA,true,false}};
VO_MODEITEM DUAL_VMODE10[1] = {{{0,0,12,12},{_DP,_DS,_DR,_DP,_DS,_DR},{_DP,_DS,_DR,_DP,_DS,_DR},VERTEX_TYPE_SPHERE,true,false}};
VO_MODEITEM DUAL_VMODE11[1] = { { { 0,0,12,12 },{ _DP2,_DS,vec3(-90,0,0),_DP,_DS,_DR },{ _DP2,_DS,vec3(-90,0,0),_DP,_DS,_DR },VERTEX_TYPE_HEMISPHERE,true,false } };
VO_MODEITEM DUAL_VMODE12[4] = { {{8,8,4,4},{ _DP1,_DS,_DR1 + vec3(0,0,90),_DP,_DS,_DR } ,{ _DP1,_DS,_DR1 + vec3(0,0,90),_DP,_DS,_DR } ,VERTEX_TYPE_HEMISPHERE,false,false },{{8,4,4,4},{ _DP1,_DS,_DR1 + vec3(0,0,180),_DP,_DS,_DR } ,{ _DP1,_DS,_DR1 + vec3(0,0,180),_DP,_DS,_DR } ,VERTEX_TYPE_HEMISPHERE,false,false},{{0,0,12,4},_MATRIX,_MATRIX,VERTEX_TYPE_PANORAMA,false,true },{{0,4,8,8},_MATRIX, _MATRIX, VERTEX_TYPE_HEMISPHERE,true,true } };
VO_MODE _Dual_VMode[VMODE_COUNT] = { {DUAL_VMODE1,SCRN_HEMISPHERE,1,false},{DUAL_VMODE2,SCRN_CYLINDER,1,false},{DUAL_VMODE3,SCRN_EXPAND,1,false},{DUAL_VMODE4,SCRN_UPDOWN,2,false},{DUAL_VMODE5,SCRN_FOUR,4,true},{DUAL_VMODE6,SCRN_THREE,3,true},{DUAL_VMODE7,SCRN_SIX,6,true},{DUAL_VMODE8,SCRN_VR,2,true},{DUAL_VMODE9,SCRN_PANORAMA,1,false},{DUAL_VMODE10,SCRN_STITCH,1,false},{DUAL_VMODE11,SCRN_CUP,1,false},{DUAL_VMODE12,SCRN_FOUREX,4,true} };


VO_MODE _VMode[VMODE_COUNT] = { {VMODE1,SCRN_HEMISPHERE,1,false},{VMODE2,SCRN_CYLINDER,1,false},{VMODE3,SCRN_EXPAND,1,false},{VMODE4,SCRN_UPDOWN,2,false},{VMODE5,SCRN_FOUR,4,true},{VMODE6,SCRN_THREE,3,true},{VMODE7,SCRN_SIX,6,true},{VMODE8,SCRN_VR,2,true},{VMODE9,SCRN_PANORAMA,1,false},{VMODE10,SCRN_STITCH,1,false},{VMODE11,SCRN_CUP,1,false},{VMODE12,SCRN_FOUREX,4,true} };

void *ChangeThread(void *arg)
{
    VertexObjectImpl *voi=(VertexObjectImpl*)arg;
    voi->HandleChangeThread();
    return NULL;
}


VertexObjectImpl::VertexObjectImpl(float aspect, void *mgn)
{
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    m_aspect = aspect;
    mVertex = new VertexObject(1.7,aspect,mgn);
	mSphere = new SphereStitchImp(aspect, mgn);
	mDualMode = 0;
	for (int i = 0; i < VMODE_COUNT; i++)
	{
		PVO_MODE vm = (PVO_MODE)malloc(sizeof(VO_MODE));
		vm->_item = (VO_MODEITEM *)malloc(sizeof(VO_MODEITEM)*_VMode[i]._count);
		memcpy(vm->_item, _VMode[i]._item, sizeof(VO_MODEITEM)*_VMode[i]._count);
		vm->_count = _VMode[i]._count;
		vm->_mode = _VMode[i]._mode;
		vm->_showselected = _VMode[i]._showselected;
		mVertexList[0].insert(-1, vm);
	}
    for (int i = 0; i < VMODE_COUNT; i++)
    {
        PVO_MODE vm = (PVO_MODE)malloc(sizeof(VO_MODE));
        vm->_item = (VO_MODEITEM *)malloc(sizeof(VO_MODEITEM)*_Dual_VMode[i]._count);
        memcpy(vm->_item, _Dual_VMode[i]._item, sizeof(VO_MODEITEM)*_Dual_VMode[i]._count);
        vm->_count = _Dual_VMode[i]._count;
        vm->_mode = _Dual_VMode[i]._mode;
        vm->_showselected = _Dual_VMode[i]._showselected;
        mVertexList[1].insert(-1, vm);
    }
    mCurrentMode = SCRN_HEMISPHERE;
	ParametricManager *mgn1 = (ParametricManager *)mgn;
	float viewangle;
	if (mgn)
		viewangle = mgn1->GetViewAngle();
	else
		viewangle = 60;
    mFrameList.insert(-1,new SixFrameDraw(1.7,0,30,40,40+viewangle*aspect,aspect));
    mFrameList.insert(-1,new ExpandFrameDraw(1.5,0 , 60, 0, viewangle*aspect,aspect));
    //mVertex->ShowFrame(true);
    mManager = mgn;
    mSelected = 0;
    _NeedDraw = false;
    _LoadingAnimation = 0;
    _loading = NULL;
    _OSD = NULL;
    _InfoTexID = 0;
    _Info = NULL;
	_RecordState = NULL;
	_RecordStateTexID = false;
	mShowRecordState = false;
	_RecordStateTexID = 0;
    mShowOSD = true;
    mShowframe = false;
    mShowInfo = false;
    mleftangle=360-(225+60*aspect/2);
    mrightangle=mleftangle+360;
    mRunThread = 1;
    pthread_mutex_init(&mChangeLock,NULL);
    pthread_create(&mChangeThread,NULL,ChangeThread,this);
}


VertexObjectImpl::~VertexObjectImpl()
{
	mRunThread = false;
	pthread_join(mChangeThread, NULL);
	delete mVertex;
	delete mSphere;
    for (int i = 0; i < mFrameList.length(); ++i) {
        IFrameDraw *fd = mFrameList.at(0);
        fd = NULL;
        mFrameList.remove(0);
    }
    if(_loading)
        delete _loading;
    if(_OSD)
        delete _OSD;
    if(_Info)
        delete _Info;
    if(_InfoTexID)
        glDeleteTextures(1,&_InfoTexID);
	if (_RecordState)
		delete _RecordState;
}

void VertexObjectImpl::DrawSelf(int scnindex)
{
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    float w = (float)vp[2] / 12;
    float h = (float)vp[3] / 12;
	if (scnindex && !mDualMode)
	{
		glViewport(0, 0, vp[2], vp[3]);
		if (_LoadingAnimation&&_loading) {
			_loading->DrawSelf(0);
		}
	}
	else
	{
		PVO_MODE vm = getCurrent(mCurrentMode);
		if (vm)
		{
            //LOGV("width-------%d,height:------------%d,w:%d,h:%d",(int)vm->_item[0]._pos.w*(int)w, vm->_item[0]._pos.h*(int)h,(int)w,(int)h);
			if(mDualMode&&mCurrentMode!=SCRN_NORMAL)
			{
			for (int i = 0;i < vm->_count;i++)
                    {
                        glViewport(vm->_item[i]._pos.x*w + vp[0], vm->_item[i]._pos.y*h + vp[1], vm->_item[i]._pos.w*w, vm->_item[i]._pos.h*h);
                        //LOGV("width-------%d,height:------------%d,w:%d,h:%d",(int)vm->_item[i]._pos.w*(int)w, vm->_item[i]._pos.h*(int)h,(int)w,(int)h);
                        mSphere->SetPosition(vm->_item[i]._matrix._pos, false, 0);
                        mSphere->SetScale(vm->_item[i]._matrix._scale, false, 0);
                        mSphere->setRotate(vm->_item[i]._matrix._rotate, false, 0);
                        mSphere->SetPosition(vm->_item[i]._matrix._texpos, true, 0);
                        mSphere->SetScale(vm->_item[i]._matrix._texscale, true, 0);
                        mSphere->setRotate(vm->_item[i]._matrix._texrotate, true, 0);
                        if (vm->_item[i]._mode == VERTEX_TYPE_PANORAMA || vm->_item[i]._mode == VERTEX_TYPE_CYLINDER)
                            mSphere->SetMode(STITCH_MODE_WORLDMAP_EXT,vm->_item[i]._mode);
                        else if (vm->_item[i]._mode == VERTEX_TYPE_EXPAND)
                            mSphere->SetMode(STITCH_MODE_WORLDMAP, vm->_item[i]._mode);
                        else
                            mSphere->SetMode(STITCH_MODE_CRYSTALBALL, vm->_item[i]._mode);
                        mSphere->DrawSelf(scnindex);
                    }
			}
			else
			{
				for (int i = 0; i < vm->_count; i++)
				{
					glViewport(vm->_item[i]._pos.x*w + vp[0], vm->_item[i]._pos.y*h + vp[1], vm->_item[i]._pos.w*w, vm->_item[i]._pos.h*h);
				mVertex->SetPosition(vm->_item[i]._matrix._pos, false, 0);
				mVertex->SetScale(vm->_item[i]._matrix._scale, false, 0);
				mVertex->setRotate(vm->_item[i]._matrix._rotate, false, 0);
				mVertex->SetPosition(vm->_item[i]._matrix._texpos, true, 0);
				mVertex->SetScale(vm->_item[i]._matrix._texscale, true, 0);
				mVertex->setRotate(vm->_item[i]._matrix._texrotate, true, 0);
				mVertex->SwitchMode(vm->_item[i]._mode);
				mVertex->UpdateViewportAspect((float)vm->_item[i]._pos.w / (float)vm->_item[i]._pos.h);
				mVertex->m_aspect = m_aspect *(float)vm->_item[i]._pos.w / (float)vm->_item[i]._pos.h;
				if (mChangeAnimation)
				{
					pthread_mutex_lock(&mChangeLock);
					PVO_MODE vm1 = getCurrent(mChangeMode);
					mVertex->SetChangePSR(vm1->_item[i]._matrix._pos, vm1->_item[i]._matrix._scale, vm1->_item[i]._matrix._rotate, vm1->_item[i]._matrix._texpos, vm1->_item[i]._matrix._texscale, vm1->_item[i]._matrix._texrotate);
					mVertex->SetChangeAnimation(mChangeAnimation, mChangeStep, mChangeStepCount, vm1->_item[i]._mode);
					pthread_mutex_unlock(&mChangeLock);
				}
				else
					mVertex->SetChangeAnimation(mChangeAnimation, 0, 100, 0);
				if (i == mSelected&&vm->_showselected)
					mVertex->DrawSelf(0);
				else
					mVertex->DrawSelf(0);
				if (vm->_item[i]._showframe)
				{
					IFrameDraw *fd;
					switch (vm->_item[i]._mode)
					{
					case VERTEX_TYPE_PANORAMA:
						fd = GetFrameDraw(FRAME_TYPE_PANORAMA);
						break;
					case VERTEX_TYPE_HEMISPHERE:
						fd = GetFrameDraw(FRAME_TYPE_HEMISPHERE);
						break;
					}
					if (fd)
						fd->DrawSelf(0xffffffff);
                    }
				}
			}
		}
		glViewport(0, 0, vp[2], vp[3]);
		if (_loading)
		{
			ParametricManager *mgn = (ParametricManager *)mManager;
			if (mgn->GetLogoLoading())
				_loading->DrawSelf(0);
			else if (_LoadingAnimation)
				_loading->DrawSelf(0);
		}
		if (GetScreenMode() != SCRN_NORMAL&& !mDualMode)
			if (!_OSD && mVertex->isUseDirectTexture) {		//android ʹ��directextureʱ����ͼ
				_OSD = new NormalPlay(0.1, m_aspect, 0, 0, mManager);
				_OSD->SetScale(vec3(12, 1, 1), false, 0);
				vec3 lpos = _OSD->GetPosition(false, 0);
			 float x =((1 - m_aspect) / 2.3) - 0.4;
                _OSD->SetPosition(vec3(x, 0.95, lpos.z), false, 0);
			}
			if (_OSD&&mShowOSD && _OSD->GetTexture(0))
				_OSD->DrawSelf(0);
		if (_InfoTexID&&_Info&&mShowInfo)
		{
			_Info->DrawSelf(0);
			//printf("putinfo....................\n");
		}
		if (_RecordState&&mShowRecordState)
		{
			_RecordState->DrawSelf(0);
		}
		//    if (mShowframe)
		//        for (int i = 0; i < mFrameList.length(); i++)
		//            if (scnindex == i)
		//                mFrameList.at(i)->DrawSelf(0xffffffff);
		//            else
		//                mFrameList.at(i)->DrawSelf(0);
	}
    _NeedDraw = false;
}

int VertexObjectImpl::GetScreenCount()
{
    PVO_MODE vm=getCurrent(mCurrentMode);
    if(vm)
        return vm->_count;
    else
        return 0;
}

void VertexObjectImpl::ResetPosition()
{
    PVO_MODE vm = getCurrent(mCurrentMode);
    if(vm)
    {
        for(int i=0;i<vm->_count;i++)
        {
			if(!mDualMode)
            mVertex->SetPosition(vm->_item[i]._default._pos,false,0);
			else
			{
				mSphere->SetPosition(vm->_item[i]._default._pos, false, 0);
			}
            vm->_item[i]._matrix._pos = vm->_item[i]._default._pos;
			if(!mDualMode)
            mVertex->SetScale(vm->_item[i]._default._scale,false,0);
			else
			{
				mSphere->SetScale(vm->_item[i]._default._scale, false, 0);
			}
            vm->_item[i]._matrix._scale = vm->_item[i]._default._scale;
			if(!mDualMode)
            mVertex->setRotate(vm->_item[i]._default._rotate,false,0);
			else
			{
				mSphere->setRotate(vm->_item[i]._default._rotate, false, 0);
			}
            vm->_item[i]._matrix._rotate = vm->_item[i]._default._rotate;
			if(!mDualMode)
            mVertex->SetPosition(vm->_item[i]._default._texpos,true,0);
			else
			{
				mSphere->SetPosition(vm->_item[i]._default._texpos, true, 0);
			}
            vm->_item[i]._matrix._texpos = vm->_item[i]._default._texpos;
			if(!mDualMode)
            mVertex->SetScale(vm->_item[i]._default._texscale,true,0);
			else
			{
				mSphere->SetScale(vm->_item[i]._default._texscale, true, 0);
			}
            vm->_item[i]._matrix._texscale = vm->_item[i]._default._texscale;
			if(!mDualMode)
            mVertex->setRotate(vm->_item[i]._default._texrotate,true,0);
			else
			{
				mSphere->setRotate(vm->_item[i]._default._texrotate, true, 0);
			}
            vm->_item[i]._matrix._texrotate = vm->_item[i]._default._texrotate;
        }
    }
    _NeedDraw = true;
}

int VertexObjectImpl::GetScreenMode()
{
    return mCurrentMode;
}

void VertexObjectImpl::SetPosition(vec3 pos, bool texture, int scnindex)
{
    PVO_MODE vm=getCurrent(mCurrentMode);
    if(!vm)
        return;
    if (scnindex > vm->_count-1 || scnindex < 0)
        return;
    if(texture)
        vm->_item[scnindex]._matrix._texpos = pos;
    else
        vm->_item[scnindex]._matrix._pos=pos;
	if(mDualMode)
		vm->_item[scnindex]._matrix._texpos.x = vm->_item[scnindex]._matrix._texpos.x-floorf(vm->_item[scnindex]._matrix._texpos.x);
	else
		vm->_item[scnindex]._matrix._texpos.x = vm->_item[scnindex]._matrix._texpos.x;
    _NeedDraw = true;
}

void VertexObjectImpl::SetScale(vec3 scale, bool texture, int scnindex)
{
    PVO_MODE vm=getCurrent(mCurrentMode);
    if(!vm)
        return;
    if (scnindex > vm->_count-1 || scnindex < 0)
        return;
    if(texture)
        vm->_item[scnindex]._matrix._texscale = scale;
    else
        vm->_item[scnindex]._matrix._scale=scale;
    _NeedDraw = true;
}

void VertexObjectImpl::setRotate(vec3 rotate, bool texture, int scnindex)
{
    PVO_MODE vm=getCurrent(mCurrentMode);
    if(!vm)
        return;
    if (scnindex > vm->_count-1 || scnindex < 0)
        return;
    if(texture)
        vm->_item[scnindex]._matrix._texrotate = rotate;
    else
        vm->_item[scnindex]._matrix._rotate=rotate;
    _NeedDraw = true;
}

vec3 VertexObjectImpl::GetPosition(bool texture, int scnindex)
{
    PVO_MODE vm=getCurrent(mCurrentMode);
    if(!vm)
        return vec3(0,0,0);
    if (scnindex > vm->_count-1 || scnindex < 0)
        return vec3(0,0,0);
    if(texture)
        return vm->_item[scnindex]._matrix._texpos;
    else
        return vm->_item[scnindex]._matrix._pos;
}

vec3 VertexObjectImpl::GetScale(bool texture, int scnindex)
{
    PVO_MODE vm=getCurrent(mCurrentMode);
    if(!vm)
        return vec3(0,0,0);
    if (scnindex > vm->_count-1 || scnindex < 0)
        return vec3(0,0,0);
    if(texture)
        return vm->_item[scnindex]._matrix._texscale;
    else
        return vm->_item[scnindex]._matrix._scale;
}

vec3 VertexObjectImpl::GetRotate(bool texture, int scnindex)
{
    PVO_MODE vm=getCurrent(mCurrentMode);
    if(!vm)
        return vec3(0,0,0);
    if (scnindex > vm->_count-1 || scnindex < 0)
        return vec3(0,0,0);
    if(texture)
        return vm->_item[scnindex]._matrix._texrotate;
    else
        return vm->_item[scnindex]._matrix._rotate;
}


void VertexObjectImpl::CleanTexture(int scnindex) {

}

bool VertexObjectImpl::LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex)
{
	mVertex->SetIsUseDirectTexture(false);
    bool ret = mVertex->LoadTexture(w, h, type, pixels, 0);
	mSphere->LoadTexture(w, h, type, pixels, scnindex);
    _NeedDraw = true;
    return ret;
}

bool VertexObjectImpl::LoadTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels, int scnindex)
{
	mVertex->SetIsUseDirectTexture(false);
    bool ret = mVertex->LoadTexture(w, h, y_pixels, cbcr_pixels, 0);
	mSphere->LoadTexture(w, h, y_pixels, cbcr_pixels, 0);
    _NeedDraw = true;
    return ret;
}

void VertexObjectImpl::SetTexture(GLuint texid, int scnindex)
{
    _NeedDraw = true;
    mVertex->SetTexture(texid, 0);
	mSphere->SetTexture(texid, 0);
	mVertex->SetIsUseDirectTexture(false);
}

void VertexObjectImpl::SetTexture(GLuint y_texid, GLuint cbcr_texid, int scnindex)
{
    _NeedDraw = true;
    mVertex->SetTexture(y_texid, cbcr_texid, 0);
	mSphere->SetTexture(y_texid, cbcr_texid, 0);
	mVertex->SetIsUseDirectTexture(false);
}

GLuint VertexObjectImpl::GetTexture(int scnindex)
{
    PVO_MODE vm=getCurrent(mCurrentMode);
    if(!vm)
        return 0;
    if (scnindex > vm->_count-1 || scnindex < 0)
        return 0;
    return mVertex->GetTexture(scnindex);
}

GLuint VertexObjectImpl::GetTexture(int scnindex, int yuv)
{
    return 0;
}

void VertexObjectImpl::SetNoVideoTexture(GLuint texid)
{
    mVertex->SetNoVideoTexture(texid);
	mSphere->SetNoVideoTexture(texid);
}

void VertexObjectImpl::SetNoVideoTexture(GLuint y_texid, GLuint u_texid, GLuint v_texid)
{
    mVertex->SetTexture(y_texid,u_texid,v_texid);
	mSphere->SetTexture(y_texid, u_texid, v_texid);
}

void VertexObjectImpl::UpdateFrame(float tbstart, float tbend, float lrstart, float lrend, int index,bool update,bool ani)
{
	int i = 0;
	PVO_MODEITEM vmi = this->getFrameTypeVO(&i);
	if(update&&GetScreenMode()==SCRN_FOUREX)
	{
		ParametricManager *mgn = (ParametricManager *)mManager;
		int w, h;
		mgn->GetWindowWH(&w, &h);
		if (lrstart > h / 3 * 2)
		{
			UpdateFrameHemisphere(tbstart, tbend, lrstart, lrend, index, update, ani);
			UpdateFramePanorama(tbstart, tbend, lrstart, lrend, index, update, ani);
		}
		else
		{
			UpdateFramePanorama(tbstart, tbend, lrstart, lrend, index, update, ani);
			UpdateFrameHemisphere(tbstart, tbend, lrstart, lrend, index, update, ani);
		}
		_NeedDraw = true;
	}
	else
	while (NULL != vmi)
	{
		switch (vmi->_mode)
		{
		case VERTEX_TYPE_HEMISPHERE:
			UpdateFrameHemisphere(tbstart, tbend, lrstart, lrend, index, update, ani);
			break;
		case VERTEX_TYPE_PANORAMA:
			UpdateFramePanorama(tbstart, tbend, lrstart, lrend, index, update, ani);
			break;
		}
		_NeedDraw = true;
		vmi = this->getFrameTypeVO(&i);
	}
	/*PVO_MODEITEM vmi = this->getFrameTypeVO();
	if(vmi)
	{
		switch(vmi->_mode)
		{
			case VERTEX_TYPE_HEMISPHERE:
				UpdateFrameHemisphere(tbstart,tbend,lrstart,lrend,index,update,ani);
				break;
			case VERTEX_TYPE_PANORAMA:
				UpdateFramePanorama(tbstart,tbend,lrstart,lrend,index,update,ani);
				break;
		}
		_NeedDraw = true;
    }*/
}

void VertexObjectImpl::SetBorderColor(GLuint selecttex, GLuint normaltex)
{
    mVertex->SetBorderColor(selecttex,normaltex);
	mSphere->SetBorderColor(selecttex, normaltex);
}

void VertexObjectImpl::SetSelected(bool value, int scnindex)
{
    mSelected = scnindex;
}

int VertexObjectImpl::GetSelected(int scnindex)
{
    return mSelected;
}

bool VertexObjectImpl::NeedDraw()
{
    if(_loading)
        return _loading->NeedDraw() | _NeedDraw;
    return _NeedDraw;
}

void VertexObjectImpl::StartAnimation(int scnindex)
{
    ParametricManager *mgn = (ParametricManager *)mManager;
    if(!_loading)
		if (mgn->GetLogoLoading())
		{
			int logow, logoh, winw, winh;
			GLuint logoid;
			mgn->GetLogo(&logoid, &logow, &logoh);
			mgn->GetWindowWH(&winw, &winh);
			ImageLoading *il = new ImageLoading((float)logow / (float)logoh, mgn);
			il->SetTexture(logoid, 0);
			il->SetTextureWH(logow, winw / 2 * 0.9);
			_loading = il;
		}
		else
		{
			_loading = new ToursLoading(0.15, 0.10, m_aspect);
		}
	if (!mgn->GetLogoLoading())
		_LoadingAnimation = mgn->StartAnimation(_loading, vec3(0, 0, 5), 5, 10, 1, JA_ANI_TYPE_ROTATE, false, 0, false, NULL, NULL);
	else
		_loading->StartAnimation(0);
}

void VertexObjectImpl::StopAnimation(int scnindex)
{
	ParametricManager *mgn = (ParametricManager *)mManager;
	if (mgn->GetLogoLoading())
	{
		if (_loading)
		{
			_loading->StopAnimation(0);
		}
	}
	else
	{
		if (_LoadingAnimation) {
			mgn->StopAnimation(_LoadingAnimation);
			_LoadingAnimation = 0;
		}
	}
}

PVO_MODE VertexObjectImpl::getCurrent(int mode) {
    for(int i=0;i<mVertexList[mDualMode].length();i++)
        if(mVertexList[mDualMode].at(i)->_mode==mode)
            return mVertexList[mDualMode].at(i);
    return NULL;
}

/////////OSD����/////////////
void VertexObjectImpl::LoadOSDTexture(int w,int h,GLenum type,GLvoid *pixels)
{
	if (!pixels)		//���û��OSD�Ĳ಻��ʾOSD
	{
		ShowOSD(false);
		return;
	}
	ShowOSD(true);
    if(!_OSD) {
        _OSD = new NormalPlay(0.1, m_aspect, 0, 0, mManager);
        _OSD->SetScale(vec3(12,1,1),false,0);
        vec3 lpos = _OSD->GetPosition(false,0);

        float x =((1 - m_aspect) / 2.3) - 0.4;
        _OSD->SetPosition(vec3(x,0.95,lpos.z),false,0);
    }
	ParametricManager* mM = (ParametricManager*)mManager;
	unsigned char*txttex = mM->_Textput->GenTextbuffer((char *)pixels, 0xff000000, 480, 24);
    _OSD->LoadTexture(w,h,type, txttex, 0);
	NormalPlay * mOSD = (NormalPlay *)_OSD;
	mOSD->SetIsUseDirectTexture(false);
	free(txttex);
}

void VertexObjectImpl::LoadOSDTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels)
{
    if(!_OSD) {
        _OSD = new NormalPlay(0.1, m_aspect, 0, 0, mManager);
        _OSD->SetScale(vec3(12,1,1),false,0);
        vec3 lpos = _OSD->GetPosition(false,0);
        //-0.4/m_aspect

        float x = ((1 - m_aspect) / 2.3) - 0.4;
        _OSD->SetPosition(vec3(x,0.95,0),false,0);
    }
    _OSD->LoadTexture(w,h,y_pixels,cbcr_pixels,0);
}

void VertexObjectImpl::PutInfo(GLuint texid,int scnindex)
{
    if(!_Info)
    {
        _Info = new NormalPlay(0.05, m_aspect, 0, 0, mManager);
        _Info->SetScale(vec3(12,1,1),false,0);
        vec3 lpos = _Info->GetPosition(false,0);
        if(GetScreenMode()!=SCRN_ONENORMAL)
            _Info->SetPosition(vec3(-0.7/m_aspect,-0.95,0),false,0);
        else
        {
            NormalPlay *lc =(NormalPlay *)this;
            float lw = lc->getHeight();
            vec3 lpos1 = GetPosition(false,0);
            _Info->SetPosition(vec3(lpos1.x,lpos1.y-GetScale(false,0).y/2+0.03,lpos1.z),false,0);
        }
    }
    _InfoTexID = texid;
    _Info->SetTexture(texid,0);
    _NeedDraw=true;
}

void VertexObjectImpl::ShowOSD(bool value)
{
    mShowOSD = value;
}

void VertexObjectImpl::AddFrame(IFrameDraw *frame)
{
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    mFrameList.insert(-1, frame);
}


void VertexObjectImpl::ShowFrame(bool value)
{
    mShowframe = value;
}


void VertexObjectImpl::ShowInfo(bool value)
{
    mShowInfo = value;
}


void VertexObjectImpl::SwitchMode(int mode,bool Ani) {
    mSelected = 0;
    if(!Ani||mDualMode)
    {
        mChangeAnimation = false;
        mCurrentMode = mode;
    }
    else
    {
        pthread_mutex_lock(&mChangeLock);
        PVO_MODE vm1 = getCurrent(mCurrentMode);
        PVO_MODE vm2 = getCurrent(mode);
        if(vm1&&vm2) {
            if(vm1->_count==vm2->_count&&vm1!=vm2) {
                mChangeMode = mode;
                mChangeAnimation = true;
                mChangeStepCount = 60;
                mChangeStep = 0;
            }
            else
            {
                mChangeAnimation = false;
                mCurrentMode = mode;
            }
        }
        else {
            mChangeAnimation = false;
            mCurrentMode = mode;
        }
        pthread_mutex_unlock(&mChangeLock);
    }
}

PVO_MODEITEM VertexObjectImpl::getFrameTypeVO(int* index)
{
	PVO_MODE vm = getCurrent(mCurrentMode);
	if (!vm)
		return NULL;
	PVO_MODEITEM vmi = NULL;
	for (int i = *index;i < vm->_count;i++)
		if (vm->_item[i]._showframe)
		{
			vmi = &vm->_item[i];
			*index = i + 1;
			return vmi;
		}
	return NULL;
}

PVO_MODEITEM VertexObjectImpl::getFrameTypeVO() {
    PVO_MODE vm = getCurrent(mCurrentMode);
    if(!vm)
        return NULL;
    int lcount=0;
    PVO_MODEITEM vmi=NULL;
    for(int i=0;i<vm->_count;i++)
        if(vm->_item[i]._showframe)
        {
            vmi=&vm->_item[i];
            lcount++;
        }
    if(lcount>0&&lcount<2)
        return vmi;
    else
        return NULL;
}

void VertexObjectImpl::UpdateFrameHemisphere(float tbstart, float tbend, float lrstart, float lrend,int index,bool update,bool ani)
{
    PVO_MODE vm=getCurrent(mCurrentMode);
    if(!vm)
        return;
    if (index < 0)
        index = mSelected;
	ParametricManager *mgn = (ParametricManager *)mManager;
	float viewangle = mgn->GetViewAngle();
    vec3 lscale = vm->_item[mSelected]._matrix._scale;
    vec3 lrotate = vm->_item[mSelected]._matrix._rotate;
    float langle = viewangle - (100 * (lscale.y - 1) * 8)/55.0;
    //printf("[SIXSCREEN]----------------%f,%f,%f,%f,lrotate.x:%f z:%f\n", tbstart, lrstart, tbend, lrend, lrotate.x, lrotate.z);
    if(update)
    {
        if (index != mSelected)
        {
            lscale = vm->_item[index]._matrix._scale;
            lrotate = vm->_item[index]._matrix._rotate;
            langle = viewangle - (100 * (lscale.y - 1) * 8);
        }
        tbend = tbend / 3 * 2;
        lrend = lrend / 3 * 2;
        lrstart = lrstart - lrend / 2;
        tbstart = tbstart - tbend / 2;
		if (tbstart == 0.00)
			return;
        float lrangle = atanf(lrstart / tbstart)/Pi*180.0f;
        if (tbstart > 0 && lrstart < 0)
            lrangle = -lrangle;
        if (tbstart > 0 && lrstart > 0)
            lrangle = 360 - lrangle;
        if (tbstart < 0)
            lrangle = 180 - lrangle;
        float tbangle;
        lrotate.z = lrangle - 90;
        float px = tbstart;
        float py = lrstart;
        float actr = sqrtf(px*px + py*py);
		if (actr / (lrend / 2)>1)
			tbangle = 90;
		else
			tbangle = 90-acosf(actr / (lrend / 2)) / Pi*180.0f;
        if (tbangle > 90 - langle / 2 || actr > lrend / 2)
            tbangle = 90 - langle / 2;
        else
        if (tbangle < langle / 2)
            ;// tbangle = langle / 2;
        else
            tbangle =tbangle + 12.0;
        //printf("tbstart:%f,lrstart:%f,actr:%f,R:%f,tbangle:%f,lrotate.x:%f,angle:%f\n", tbstart, lrstart,actr,lrend/2, tbangle,lrotate.x, acosf(actr / (lrend / 2)) / Pi*180.0f);
        lrotate.x = tbangle;
        if (ani)
        {
            ParametricManager *mgn = (ParametricManager*)mManager;
            mgn->StartAnimation(this, lrotate, 30, 30, 0, JA_ANI_TYPE_ROTATE, false, index, true, NULL, NULL);
        }
        else
            vm->_item[index]._matrix._rotate = lrotate;
        if (index != mSelected)
            return;
        //printf("[SIXSCREEN]-----------------------------lrotate.x:%f z:%f\n", lrotate.x,lrotate.z);
    }
    float lstartud = (90 - lrotate.x) - langle / 2;
    float lstartlr = lrotate.z + (90 - langle*m_aspect / 2);
    IFrameDraw *fd = GetFrameDraw(FRAME_TYPE_HEMISPHERE);
    if(fd)
        fd->UpdatePosition(lstartud, lstartud + langle, lstartlr, lstartlr + langle*m_aspect);
    //mVertex->UpdateFrame(lstartud, lstartud + langle, lstartlr, lstartlr + langle*m_aspect, index, false,false);
}

void VertexObjectImpl::UpdateFramePanorama(float tbstart, float tbend, float lrstart, float lrend,int index,bool update,bool ani)
{
    PVO_MODE vm=getCurrent(mCurrentMode);
    if(!vm)
        return;
    if (index < 0)
        index = mSelected;
	ParametricManager *mgn = (ParametricManager *)mManager;
	float viewangle = mgn->GetViewAngle();
	vec3 lscale = vm->_item[mSelected]._matrix._scale;
    vec3 lrotate = vm->_item[mSelected]._matrix._rotate;
    float langle = viewangle - (100 * (lscale.y - 1) * 8)/55.0;
    if(update)
    {
        lrotate.z =mleftangle-tbstart/tbend*360.0f;
        //if(lrotate.z<mrightangle)
        //    lrotate.z =mrightangle;
        //if(lrotate.z>mleftangle)
        //    lrotate.z = mleftangle;
        lrotate.x = 100-(lrstart-lrend/3*2)/(lrend/3)*90;
        vm->_item[index]._matrix._rotate = lrotate;
        //printf("[SIXSCREEN]-----------------------------lrotate.x:%f z:%f\n", lrotate.x,lrotate.z);
    }
    //printf("[THREESCREEN]---------------lrotate.x:%f z:%f langle:%f\n", lrotate.x, lrotate.z,langle);
    float lstartud = 90-lrotate.x;
    float lstartlr = mleftangle-lrotate.z;
    IFrameDraw *fd = GetFrameDraw(FRAME_TYPE_PANORAMA);
    if(fd)
        fd->UpdatePosition(lstartud-langle/2, lstartud+langle/2, lstartlr-langle*m_aspect/2, lstartlr + langle*m_aspect/2);
    //mNormal->UpdateFrame(lstartud-langle/2, lstartud+langle/2, lstartlr-langle*m_aspect/2, lstartlr + langle*m_aspect/2, 0, false,false);
}

IFrameDraw *VertexObjectImpl::GetFrameDraw(int framedrawtype) {
    for(int i=0;i<mFrameList.length();i++)
        if(mFrameList.at(i)->GetFrameType()==framedrawtype)
            return mFrameList.at(i);
    return NULL;
}

void* VertexObjectImpl::GetLoading(int index)
{
	return _loading;
}

void VertexObjectImpl::UpdateAspect(float aspect) {
    m_aspect = aspect;
    if(_Info)
    {
        ParametricSurface *ps=(ParametricSurface *)_Info;
        ps->m_aspect = aspect;
    }
    if(_OSD)
    {
        ParametricSurface *ps=(ParametricSurface *)_OSD;
        ps->m_aspect = aspect;
    }
	if (_loading)
	{
		ParametricSurface *ps = (ParametricSurface *)_loading;
		ps->m_aspect = aspect;
	}
    mVertex->m_aspect = aspect;
	if (mCurrentMode == SCRN_VR)
		mSphere->updateAspect(aspect/2);
	else
		mSphere->updateAspect(aspect);
    mVertex->setupLineBuffer();
    for (int i = 0; i < mFrameList.length(); i++) {
        mFrameList.at(i)->UpdateAspect(aspect);
    }
}

void VertexObjectImpl::HandleChangeThread() {
    while(mRunThread)
    {
        if(mChangeAnimation)
        {
            pthread_mutex_lock(&mChangeLock);
            mChangeStep++;
            if(mChangeStep>=mChangeStepCount)
            {
                mChangeAnimation = false;
                mCurrentMode = mChangeMode;
            }
            _NeedDraw = true;
            pthread_mutex_unlock(&mChangeLock);
        }
#ifdef _MSC_VER
		Sleep(10);
#else
        usleep(10000);
#endif
    }
	pthread_exit(NULL);
}

void VertexObjectImpl::TransformVertex(int mode,vec3 vect,bool texture,int type,int scrnindex)
{
    PVO_MODE vm = getCurrent(mode);
    if(vm)
    {
        if(scrnindex>=vm->_count)
            return;
        switch(type)
        {
            case TRANSFORM_POSITION:
                if(!texture)
                    vm->_item[scrnindex]._matrix._pos = vect;
                else
                    vm->_item[scrnindex]._matrix._texpos = vect;
                break;
            case TRANSFORM_SCALE:
                if(!texture)
                    vm->_item[scrnindex]._matrix._scale = vect;
                else
                    vm->_item[scrnindex]._matrix._texscale = vect;
                break;
            case TRANSFORM_ROTATE:
                if(!texture)
                    vm->_item[scrnindex]._matrix._rotate = vect;
                else
                    vm->_item[scrnindex]._matrix._texrotate = vect;
                break;
        }
    }
}


void VertexObjectImpl::SetViewAngle(float angle)
{
    if(mDualMode)
        return;
	for(int i=0;i<mVertexList[mDualMode].length();i++)
	{
		PVO_MODE pvo = mVertexList[mDualMode].at(i);
		PVO_MODEITEM voi = pvo->_item;
		for(int j=0;j<pvo->_count;j++)
		{
			if (voi[j]._mode == VERTEX_TYPE_HEMISPHERE&&!voi[j]._ismain)
			{
				voi[j]._default._rotate = vec3(90 - angle / 2, voi[j]._default._rotate.y, voi[j]._default._rotate.z);
				voi[j]._matrix._rotate = voi[j]._default._rotate;
			}
		}
	}
}

void VertexObjectImpl::DirectTextureFrameUpdata(int w, int h, void * directBuffer, int index)
{
	mVertex->DirectTextureFrameUpdata(w, h, directBuffer, index);
	ShowInfo(false);
}

void VertexObjectImpl::DirectTextureOSDFrameUpdata(int w, int h, void * directBuffer, int index)
{
	if (_OSD) {
		NormalPlay* _mOSD = (NormalPlay*)_OSD;
		_mOSD->DirectTextureFrameUpdata(w, h, directBuffer, index);
	}
}

void VertexObjectImpl::LoadRecordStatuTexture(GLuint texid, int scnindex)
{
	if (!_RecordState)
	{
		_RecordState = new NormalPlay(0.1, m_aspect, 0, 0, mManager);
		_RecordState->SetScale(vec3(3, 2, 1), false, 0);
		vec3 lpos = _RecordState->GetPosition(false, 0);
		_RecordState->SetPosition(vec3(1.0 / m_aspect, 0.75, lpos.z), false, 0);
	}
	_RecordState->SetTexture(texid, 0);
}
void VertexObjectImpl::SetDistortion(PDISTORTION_NUM pdist, int dist_count, float half)
{
	mSphere->SetDistortion(pdist, dist_count, half);
}
void VertexObjectImpl::SetFishEyeParameter(float centerx, float centery, float radius, float anglex, float angley, float anglez, int index,bool v720)
{
	mSphere->mManager = mManager;
    if(v720)
	    mSphere->SetFishEyeParameter(centerx, centery, radius, anglex, angley, anglez, index);
    else
        mVertex->SetFishEyeParameter(centerx,centery,radius);
}
void VertexObjectImpl::SetTextureAspect(float aspect)
{
	mSphere->SetTextureAspect(aspect);
}
void VertexObjectImpl::SetSingleMode(bool single)
{
	mSphere->SetSingleMode(single);
}
void VertexObjectImpl::SetFactoryMode(int factory)
{
	mSphere->SetFactoryMode(factory);
}
void VertexObjectImpl::RenderFBO()
{
	mSphere->RenderFBO();
}
