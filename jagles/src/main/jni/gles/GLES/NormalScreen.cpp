//
// Created by LiHong on 16/2/6.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "NormalScreen.h"
#include <stdio.h>
#include "ParametricManager.h"

#define STATIC_SCREEN_COUNT 36

const SPLIT_POSITION SPLIT_1[1] = {{0, 0, 12, 12}};
const SPLIT_POSITION SPLIT_2[4] = {{0, 0, 6, 6},
                                   {6, 0, 6, 6},
                                   {0, 6, 6, 6},
                                   {6, 6, 6, 6}};
const SPLIT_POSITION SPLIT_3[6] = {{0, 0, 8, 8},
                                   {8, 0, 4, 4},
                                   {8, 4, 4, 4},
                                   {0, 8, 4, 4},
                                   {4, 8, 4, 4},
                                   {8, 8, 4, 4}};
const SPLIT_POSITION SPLIT_4[8] = {{0, 0, 9, 9},
                                   {9, 0, 3, 3},
                                   {9, 3, 3, 3},
                                   {9, 6, 3, 3},
                                   {0, 9, 3, 3},
                                   {3, 9, 3, 3},
                                   {6, 9, 3, 3},
                                   {9, 9, 3, 3}};
const SPLIT_POSITION SPLIT_5[9] = {{0, 0, 4, 4},
                                   {4, 0, 4, 4},
                                   {8, 0, 4, 4},
                                   {0, 4, 4, 4},
                                   {4, 4, 4, 4},
                                   {8, 4, 4, 4},
                                   {0, 8, 4, 4},
                                   {4, 8, 4, 4},
                                   {8, 8, 4, 4}};
const SPLIT_POSITION SPLIT_6[13] = {{0, 0, 3, 3},
                                    {3, 0, 3, 3},
                                    {6, 0, 3, 3},
                                    {9, 0, 3, 3},
                                    {0, 3, 3, 3},
                                    {3, 3, 6, 6},
                                    {9, 3, 3, 3},
                                    {0, 6, 3, 3},
                                    {9, 6, 3, 3},
                                    {0, 9, 3, 3},
                                    {3, 9, 3, 3},
                                    {6, 9, 3, 3},
                                    {9, 9, 3, 3}};
const SPLIT_POSITION SPLIT_7[16] = {{0, 0, 3, 3},
                                    {3, 0, 3, 3},
                                    {6, 0, 3, 3},
                                    {9, 0, 3, 3},
                                    {0, 3, 3, 3},
                                    {3, 3, 3, 3},
                                    {6, 3, 3, 3},
                                    {9, 3, 3, 3},
                                    {0, 6, 3, 3},
                                    {3, 6, 3, 3},
                                    {6, 6, 3, 3},
                                    {9, 6, 3, 3},
                                    {0, 9, 3, 3},
                                    {3, 9, 3, 3},
                                    {6, 9, 3, 3},
                                    {9, 9, 3, 3}};

//录像图标坐标
const SPLIT_POSITION SPLITE_RECORD_1[1] = {{0, 0, 12, 12}};
const SPLIT_POSITION SPLITE_RECORD_2[4] = {{0, 0, 6,  12},
                                           {0, 0, 12, 12},
                                           {0, 0, 6,  6},
                                           {0, 0, 12, 6}};
const SPLIT_POSITION SPLITE_RECORD_3[6] = {{0, 0, 8,  12},
                                           {0, 0, 12, 12},
                                           {0, 0, 12, 8},
                                           {0, 0, 4,  4},
                                           {0, 0, 8,  4},
                                           {0, 0, 12, 4}};
const SPLIT_POSITION SPLITE_RECORD_4[8] = {{0, 0, 9,  12},
                                           {0, 0, 12, 12},
                                           {0, 0, 12, 9},
                                           {0, 0, 12, 6},
                                           {0, 0, 3,  3},
                                           {0, 0, 6,  3},
                                           {0, 0, 9,  3},
                                           {0, 0, 12, 3}};
const SPLIT_POSITION SPLITE_RECORD_5[9] = {{0, 0, 4,  12},
                                           {0, 0, 8,  12},
                                           {0, 0, 12, 12},
                                           {0, 0, 4,  8},
                                           {0, 0, 8,  8},
                                           {0, 0, 12, 8},
                                           {0, 0, 4,  4},
                                           {0, 0, 8,  4},
                                           {0, 0, 12, 4}};
const SPLIT_POSITION SPLITE_RECORD_6[13] = {{0, 0, 3,  12},
                                            {0, 0, 6,  12},
                                            {0, 0, 9,  12},
                                            {0, 0, 12, 12},
                                            {0, 0, 3,  9},
                                            {0, 0, 9,  9},
                                            {0, 0, 12, 9},
                                            {0, 0, 3,  6},
                                            {0, 0, 12, 6},
                                            {0, 0, 3,  3},
                                            {0, 0, 6,  3},
                                            {0, 0, 9,  3},
                                            {0, 0, 12, 3}};
const SPLIT_POSITION SPLITE_RECORD_7[16] = {{0, 0, 3,  12},
                                            {0, 0, 6,  12},
                                            {0, 0, 9,  12},
                                            {0, 0, 12, 12},
                                            {0, 0, 3,  9},
                                            {0, 0, 6,  9},
                                            {0, 0, 9,  9},
                                            {0, 0, 12, 9},
                                            {0, 0, 3,  6},
                                            {0, 0, 6,  6},
                                            {0, 0, 9,  6},
                                            {0, 0, 12, 6},
                                            {0, 0, 3,  3},
                                            {0, 0, 6,  3},
                                            {0, 0, 9,  3},
                                            {0, 0, 12, 3}};

const SPLIT_MODE SplitMode[7] = {{1,  (PSPLIT_POSITION) SPLIT_1},
                                 {4,  (PSPLIT_POSITION) SPLIT_2},
                                 {6,  (PSPLIT_POSITION) SPLIT_3},
                                 {8,  (PSPLIT_POSITION) SPLIT_4},
                                 {9,  (PSPLIT_POSITION) SPLIT_5},
                                 {13, (PSPLIT_POSITION) SPLIT_6},
                                 {16, (PSPLIT_POSITION) SPLIT_7}};

//录像图标坐标集合
const SPLIT_MODE SplitRecordMode[7] = {{1,  (PSPLIT_POSITION) SPLITE_RECORD_1},
                                       {4,  (PSPLIT_POSITION) SPLITE_RECORD_2},
                                       {6,  (PSPLIT_POSITION) SPLITE_RECORD_3},
                                       {8,  (PSPLIT_POSITION) SPLITE_RECORD_4},
                                       {9,  (PSPLIT_POSITION) SPLITE_RECORD_5},
                                       {13, (PSPLIT_POSITION) SPLITE_RECORD_6},
                                       {16, (PSPLIT_POSITION) SPLITE_RECORD_7}};

void OnPageAniEnd(ParametricManager *mgn, void *ani) {
    PJA_Animation lani = (PJA_Animation) ani;
    ParametricSurface *lobj = (ParametricSurface *) lani->_obj;
    lobj->Visible = false;
    NormalScreen *lobj1 = (NormalScreen *) lani->Ctx;
    if (lani->index == 0)
        lobj1->SetSplitPosition(true);
}

NormalScreen::~NormalScreen() {
    for (int i = 0; i < allPage; i++) {

        delete mVideo[i];
        mVideo[i] = NULL;

        delete mRecordState[i];
        mRecordState[i] = NULL;
    }
}

NormalScreen::NormalScreen(float aspect, void *mgn) {
    allPage = STATIC_SCREEN_COUNT;
    mCurrentMode = SCRN_SPLIT_ONE;
    mCurrentIndex = 0;
    mCurrentPage = 0;
    LOGDEBUG("NormalScreen Create---------------------------");
    for (int i = 0; i < allPage; ++i) {
        mVideo[i] = new NormalPlay(1, aspect, 0, 0, mgn);
        mVideo[i]->DebugIndex = i;
        if (i > SplitMode[mCurrentMode].count - 1)
            mVideo[i]->Visible = false;
    }
    for (int i = 0; i < allPage; i++) {
        mRecordState[i] = new NormalPlay(1, aspect, 0, 0, mgn);
        mRecordState[i]->Visible = false;
        mRecordState[i]->SetScale(vec3(1.0, 1.0, 1.0), false, 0);
    }
    _Manager = mgn;
    m_aspect = aspect;
    SetSplitPosition(false);
    mVideo[0]->SetSelected(true, 0);
    mSelectIndex = 0;
}

int NormalScreen::GetPage() {
    return mCurrentPage;
}

void NormalScreen::SetSplitPosition(bool ani) {
    isSpliting = true;
    int pc = allPage / SplitMode[mCurrentMode].count;
    if (allPage % SplitMode[mCurrentMode].count)
        pc++;
    float lstart = -2 * mCurrentPage;
    float lposunit = 1.0f / 6.0f;
    ParametricManager *mgn = (ParametricManager *) _Manager;
    //for(int i=0;i<pc;i++)
    for (int j = 0; j < SplitMode[mCurrentMode].count; j++) {
        if (mCurrentPage * SplitMode[mCurrentMode].count + j > 35)
            break;
        //mVideo[mCurrentPage*SplitMode[mCurrentMode].count+j]->ResetPosition();
        mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j]->Visible = true;
        vec3 lpos = mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j]->GetPosition(false, 0);
        lpos.x = -lposunit * SplitMode[mCurrentMode].pos[j].w / 2;
        lpos.y = lposunit * SplitMode[mCurrentMode].pos[j].h / 2;
        float absx = -1 + SplitMode[mCurrentMode].pos[j].x * lposunit;
        float absy = 1 - SplitMode[mCurrentMode].pos[j].y * lposunit;
        lpos.x = (absx - lpos.x) * m_aspect;
        lpos.y = absy - lpos.y;
        //printf("1............%f,%f\n",lpos.x,lpos.y);
        vec3 lscale = vec3((float) SplitMode[mCurrentMode].pos[j].w / 6.0f,
                           (float) SplitMode[mCurrentMode].pos[j].h / 6.0f, 1);
        if (ani) {
            if (ObjectWillVisible)
                ObjectWillVisible(mgn, mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j],
                                  lpos, lscale, vec3(0, 0, 0));
            else {
                mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j]->SetScale(lscale, false,
                                                                                   0);
                mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j]->SetPosition(lpos, false,
                                                                                      0);
            }
        } else {
            mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j]->SetScale(lscale, false, 0);
            mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j]->SetPosition(lpos, false, 0);
        }
    }
    isSpliting = false;
}


void NormalScreen::SetSplit(int index, bool ani) {
    //   if(mCurrentMode == index)
    //       return;
    for (int j = 0; j < SplitMode[mCurrentMode].count; j++) {
        if (mCurrentPage * SplitMode[mCurrentMode].count + j > 35)
            break;
        if (ani) {
            if (ObjectHidden)
                ObjectHidden((ParametricManager *) _Manager,
                             mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j], OnPageAniEnd,
                             this, j);
            else
                mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j]->Visible = false;
        } else
            mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j]->Visible = false;
    }
    mCurrentMode = index;
    if (!ani) {
        int tempSelect = mSelectIndex;
        int page = tempSelect / SplitMode[mCurrentMode].count;
        mCurrentPage = page;
        SetPage(mCurrentPage, false);
    }
    //	SetSplitPosition(ani);
}

void NormalScreen::SetPage(int index, bool ani) {
    //  if(index==mCurrentPage)
    //       return;
    int oldpage = mCurrentPage;
    int pc = allPage / SplitMode[mCurrentMode].count;
    if (allPage % SplitMode[mCurrentMode].count)
        pc++;
    if (index < 0 || index >= pc)
        return;
    ParametricManager *mgn = (ParametricManager *) _Manager;
    for (int j = 0; j < SplitMode[mCurrentMode].count; j++) {
        if (mCurrentPage * SplitMode[mCurrentMode].count + j > 35)
            break;
        vec3 lpos = mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j]->GetPosition(false, 0);
//        if(index>oldpage) {
//            lpos.x = 3;
//            lpos.y = 3;
//        }
//        else
//        {
//            lpos.x=-3;
//            lpos.y=-3;
//        }
        if (ani) {
            if (ObjectHidden)
                ObjectHidden(mgn, mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j],
                             OnPageAniEnd, this, j);
            else
                mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j]->Visible = false;
        } else
            mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j]->Visible = false;
    }
    mCurrentPage = index;
//    for(int j=0;j<SplitMode[mCurrentMode].count;j++)
//    {
//        if(mCurrentPage*SplitMode[mCurrentMode].count+j>35)
//            break;
//        vec3 lpos = mVideo[mCurrentPage*SplitMode[mCurrentMode].count+j]->GetPosition(false,0);
//        if(index>oldpage) {
//            lpos.x = -3;
//            lpos.y = -3;
//        }
//        else
//        {
//            lpos.x=3;
//            lpos.y=3;
//        }
//        mVideo[mCurrentPage * SplitMode[mCurrentMode].count + j]->SetPosition(lpos, false, 0);
//    }
    if (!ani)
        SetSplitPosition(ani);
}


void NormalScreen::ResetPosition() {
    for (int i = 0; i < allPage; i++)
        mVideo[i]->ResetPosition();
    SetSplit(mCurrentMode, false);
    for (int i = 0; i < allPage; i++)
        mVideo[i]->setRotate(vec3(0, 0, 0), false, 0);
}


int NormalScreen::GetScreenMode() {
    return SCRN_NORMAL;
}

void NormalScreen::TurnLeftRight(GLfloat angle, int scnindex) {

}

void NormalScreen::TurnUpDown(GLfloat angle, int scnindex) {

}

void NormalScreen::setZoom(GLfloat multi, int scnindex) {

}

void NormalScreen::DrawSelf(int scnindex) {
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    float w = (float) vp[2] / 12;
    float h = (float) vp[3] / 12;
//	glViewport(0, 0, vp[2], vp[3]);
    if (isSpliting)
        return;
    int index = -1;
    for (int i = 0; i < allPage; i++) {
        if (!mVideo[i]) {
            return;
        }
        if (mVideo[i]->GetSelected(0))
            index = i;
        else
            mVideo[i]->DrawSelf(0x7fffffff);
        if (GetSplitMode() > 2) {
            mVideo[i]->ShowInfo(false);
        } else {
            mVideo[i]->ShowInfo(true);
        }
    }
    if (index >= 0)
        mVideo[index]->DrawSelf(0x7fffffff);

    bool temp = false;
    for (int i = mCurrentPage * SplitRecordMode[mCurrentMode].count; i < allPage; i++) {
        if (mVideo[i]->mIsOwnAnimation) {
            temp = true;
        }
    }

//    for (int i = mCurrentPage * SplitRecordMode[mCurrentMode].count; i < allPage; i++) {
//        if (!temp) {
//            if (i < SplitRecordMode[mCurrentMode].count)
//                glViewport(SplitRecordMode[mCurrentMode].pos[i].w * w - 150,
//                           SplitRecordMode[mCurrentMode].pos[i].h * h - 70, 150, 80);
//            else
//                glViewport(SplitRecordMode[mCurrentMode].pos[i %
//                                                             SplitRecordMode[mCurrentMode].count].w *
//                           w - 150, SplitRecordMode[mCurrentMode].pos[i %
//                                                                      SplitRecordMode[mCurrentMode].count].h *
//                                    h - 70, 150, 80);
//            if (mRecordState[i]->Visible) {
//                mRecordState[i]->DrawSelf(0);
//            }
//        }
//    }

    glViewport(0, 0, vp[2], vp[3]);
}

void NormalScreen::PutInfo(GLuint texid, int scnindex) {
    //for(int i=0;i<STATIC_SCREEN_COUNT;i++)
    mVideo[scnindex]->PutInfo(texid, 0);
}

int NormalScreen::GetScreenCount() {
    return allPage;
}

int NormalScreen::GetAllPage() {
    int pc = allPage / SplitMode[mCurrentMode].count;
    if (allPage % SplitMode[mCurrentMode].count)
        pc++;
    return pc;
}

void NormalScreen::SetPosition(vec3 pos, bool texture, int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return;
    mVideo[scnindex]->SetPosition(pos, texture, 0);
}

void NormalScreen::SetScale(vec3 scale, bool texture, int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return;
    mVideo[scnindex]->SetScale(scale, texture, 0);
}

void NormalScreen::setRotate(vec3 rotate, bool texture, int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return;
    mVideo[scnindex]->setRotate(rotate, texture, 0);
}

vec3 NormalScreen::GetPosition(bool texture, int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return vec3(0, 0, 0);
    return mVideo[scnindex]->GetPosition(texture, 0);
}

vec3 NormalScreen::GetScale(bool texture, int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return vec3(0, 0, 0);
    return mVideo[scnindex]->GetScale(texture, 0);
}

vec3 NormalScreen::GetRotate(bool texture, int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return vec3(0, 0, 0);
    return mVideo[scnindex]->GetRotate(texture, 0);
}


void NormalScreen::CleanTexture(int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return;
    return mVideo[scnindex]->CleanTexture(scnindex);
}

bool NormalScreen::LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex) {
    if (mVideo[scnindex]->mIsOwnAnimation) {
        return false;
    }
    if (scnindex < 0 || scnindex > 35)
        return false;
    //printf("loadtexture:%d,%d,%d\n",w,h,scnindex);
    mVideo[scnindex]->SetIsUseDirectTexture(false);
    return mVideo[scnindex]->LoadTexture(w, h, type, pixels, scnindex);
}

bool NormalScreen::LoadTexture(int w, int h, void *buffer, int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return false;
    mVideo[scnindex]->SetIsUseDirectTexture(false);
    return mVideo[scnindex]->LoadTexture(w, h, buffer, 0);
}

void NormalScreen::SetTexture(GLuint texid, int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return;
    mVideo[scnindex]->SetTexture(texid, 0);
    mVideo[scnindex]->SetIsUseDirectTexture(false);
}

GLuint NormalScreen::GetTexture(int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return 0;
    return mVideo[scnindex]->GetTexture(0);
}

bool NormalScreen::NeedDraw() {
//    return mVideo[0]->NeedDraw();
    bool lneed = false;
    for (int i = 0; i < allPage; i++)
        lneed = lneed | mVideo[i]->NeedDraw();
    //printf("...........Needdraw:%d\n",mVideo[0]->NeedDraw());
    return lneed;
}

void NormalScreen::StartAnimation(int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return;
    mVideo[scnindex]->StartAnimation(0);
}

void NormalScreen::StopAnimation(int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return;
    mVideo[scnindex]->StopAnimation(0);
}

void NormalScreen::SetNoVideoTexture(GLuint texid) {
    for (int i = 0; i < allPage; i++)
        mVideo[i]->SetNoVideoTexture(texid);
}

void NormalScreen::SetSelected(int x, int y, int w, int h) {
    int wunit = w / 12;
    int hunit = h / 12;
    int px = x / wunit;
    int py = y / hunit;
    for (int i = 0; i < allPage; i++)
        mVideo[i]->SetSelected(false, 0);
    for (int i = 0; i < SplitMode[mCurrentMode].count; i++)
        if (px >= SplitMode[mCurrentMode].pos[i].x &&
            px <= SplitMode[mCurrentMode].pos[i].x + SplitMode[mCurrentMode].pos[i].w &&
            py >= SplitMode[mCurrentMode].pos[i].y &&
            py <= SplitMode[mCurrentMode].pos[i].y + SplitMode[mCurrentMode].pos[i].h) {
            mVideo[mCurrentPage * SplitMode[mCurrentPage].count + i]->SetSelected(true, 0);
            mSelectIndex =
                    mCurrentPage * SplitMode[mCurrentPage].count + i;            //保存已经选中的video的下标
            break;
        }
}

int NormalScreen::getVideoIndex() {
    return mSelectIndex;
}

void NormalScreen::SetSelected(bool value, int index) {
    for (int i = 0; i < allPage; i++) {
        mVideo[i]->SetSelected(false, 0);
        if (mCurrentMode == 0)
            mVideo[i]->Visible = false;
    }
    mVideo[index]->SetSelected(value, 0);
    if (mCurrentMode == 0)
        mVideo[index]->Visible = true;
    mSelectIndex = index;
}


int NormalScreen::GetSelected(int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return false;
    return mVideo[scnindex]->GetSelected(0);
}

void NormalScreen::SetBorderColor(GLuint selecttex, GLuint normaltex) {
    for (int i = 0; i < allPage; i++)
        mVideo[i]->SetBorderColor(selecttex, normaltex);
}


bool NormalScreen::LoadTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels, int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return false;
    mVideo[scnindex]->LoadTexture(w, h, y_pixels, cbcr_pixels, 0);
    mVideo[scnindex]->SetIsUseDirectTexture(false);
    return true;
}

void NormalScreen::SetTexture(GLuint y_texid, GLuint cbcr_texid, int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return;
    mVideo[scnindex]->SetTexture(y_texid, cbcr_texid, 0);
    mVideo[scnindex]->SetIsUseDirectTexture(false);
    return;
}

GLuint NormalScreen::GetTexture(int scnindex, int yuv) {
    return 0;
}

void NormalScreen::SetNoVideoTexture(GLuint y_texid, GLuint u_texid, GLuint v_texid) {

}

int NormalScreen::GetSplitMode() {
    return mCurrentMode;
}

bool NormalScreen::GetVisibility(int index) {
    return mVideo[index]->Visible;
}

void NormalScreen::ReSizeSplite(float aspect) {
    m_aspect = aspect;
    for (int i = 0; i < allPage; i++) {
        mVideo[i]->ReSetBuffer(aspect);
        mVideo[i]->ResetSizeAnimation(aspect);
    }
    SetSplitPosition(false);
}

void *NormalScreen::GetLoading(int index) {
    if (index < 0 || index > 35)
        return NULL;
    return mVideo[index]->GetLoading(index);
}

void NormalScreen::SetAllPage(int allPage) {
    this->allPage = allPage;
}

void NormalScreen::SetIsAnimation(int index, bool animation) {
    if (index < 0 || index > 35)
        return;
    mVideo[index]->SetIsAnimation(index, animation);
}

void NormalScreen::DirectTextureFrameUpdata(int w, int h, void *directBuffer, int scnindex) {
    mVideo[scnindex]->DirectTextureFrameUpdata(w, h, directBuffer, 0);
}

void NormalScreen::LoadRecordStatuTexture(GLuint texid, int scnindex) {
    /*for (int i = 0; i < 36; i++)
        mVideo[i]->LoadRecordStatuTexture(texid, 0);*/
//	for (int i = 0; i < 16; i++)
//		mRecordState[i]->SetTexture(texid, 0);
}

void NormalScreen::ShowRecordStatu(bool value, int index) {
//	mVideo[index]->ShowRecordStatu(value, index);
//	if (index < 16)
//	mRecordState[index]->Visible = value;
    /*else
        mRecordState[index % 16]->Visible = value;*/
}

void NormalScreen::ShowInfo(bool value, int index) {
    mVideo[index]->ShowInfo(value);
}

void NormalScreen::releaseInfo(int index) {
    mVideo[index]->releaseInfo();
}


void NormalScreen::SetKeepAspect(float aspect, int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return;
    if (aspect < 1.0)
        return;
    if (mVideo[scnindex]) {
        mVideo[scnindex]->SetKeepAspect(aspect);
    }

}

float NormalScreen::GetKeepAspect(int scnindex) {
    if (scnindex < 0 || scnindex > 35)
        return 1.0;
    return mVideo[scnindex]->GetKeepAspect();
}
