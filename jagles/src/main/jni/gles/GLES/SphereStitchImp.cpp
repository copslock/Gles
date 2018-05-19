#include "SphereStitchImp.h"
#include "ParametricManager.h"
#include "NormalPlay.h"
#define LOGV(...)__android_log_print(ANDROID_LOG_DEBUG,"Lee",__VA_ARGS__)

SphereStitchImp::SphereStitchImp(float aspect, void* mgn)
{
	mStitchMode = STITCH_MODE_CRYSTALBALL;
	mCrystalBall = new SphereStitch2(1.5, aspect, mgn);
	mWorldmap = new SphereStitch(1.5, aspect, mgn);
	mAllInOne = new sphere(1.5, aspect, mgn);
//	mAllInOne = new NormalPlay(1.0, aspect, 0, 0, mgn);
	mManager = mgn;
}

SphereStitchImp::~SphereStitchImp()
{
	delete mCrystalBall;
	delete mWorldmap;
	delete mAllInOne;
}

vec3 SphereStitchImp::Evaluate(const vec2& domain)
{
	return vec3(0, 0, 0);
}

vec2 SphereStitchImp::EvaluateCoord(const vec2& domain)
{
	return vec2(0, 0);
}

void SphereStitchImp::ResetPosition()
{
	//mCrystalBall->ResetPosition();
	//mWorldmap->ResetPosition();
}

void SphereStitchImp::DrawSelf(int index)
{
//	mCrystalBall->_Manager = mManager;
//	mWorldmap->_Manager = mManager;
//	mAllInOne->_Manager = mManager;
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT,&vp[0]);
	switch(mStitchMode)
	{
	case STITCH_MODE_CRYSTALBALL:
		mCrystalBall->DrawSelf(index);
        //LOGV("hello,world.................dual mode drawself............");
		break;
	case STITCH_MODE_WORLDMAP_EXT:
	case STITCH_MODE_WORLDMAP:
		//if (!index)
		//{
		//	if (mWorldmap->IsInitFBO())
		//	{
		//		mWorldmap->RenderFBO();
		//		glViewport(vp[0], vp[1], vp[2], vp[3]);
		//		glClearColor(0, 0, 0, 1);
		//		glClear(GL_COLOR_BUFFER_BIT);

		//		mAllInOne->SetTexture(mWorldmap->FBOTexture(), 0);
		//	}
		//}
		mAllInOne->SetTexture(mWorldmap->FBOTexture(), 0);
		mAllInOne->DrawSelf(0);
//        mWorldmap->DrawSelf(index);
		break;
	//case STITCH_MODE_WORLDMAP:
	//	if (!index)
	//	{
	//		mWorldmap->DrawSelf(0);
	//	}
	//	break;
	}
}

void SphereStitchImp::updateAspect(float aspect)
{
	mCrystalBall->updateAspect(aspect);
	mAllInOne->m_aspect = aspect;
}

void SphereStitchImp::SetDistortion(PDISTORTION_NUM pdist, int dist_count, float half)
{
	mCrystalBall->SetDistortion(pdist, dist_count, half);
	mWorldmap->SetDistortion(pdist, dist_count, half);
}

void SphereStitchImp::SetFishEyeParameter(float centerx, float centery, float radius, float anglex, float angley, float anglez, int index)
{
    mCrystalBall->SetFishEyeParameter(centerx, centery, radius, anglex, angley, anglez, index);
	ParametricManager *mgn = (ParametricManager *)mManager;
	int width, height;
	if(mgn)
	{
		width = mgn->GetTextureWidth();
		height = mgn->GetTextureHeight();
	}
	//todo 转换还不对
	LOGV("-----------------width:%d,height:%d",width,height);
	if(!mWorldmap->IsInitFBO())
		mWorldmap->InitFBO(width, height);
	float lz;
	if (index == 0)
		lz = 90 + (fabs(anglez) - 90.0);
	else
		lz = -90 - (fabs(anglez) - 90.0);
	mWorldmap->SetStitchOptions(centerx, centery , radius, 195.0* DTOR/2,  anglex, lz, angley, width,height, index);
	mWorldmap->SetBlending(8);
}

void SphereStitchImp::SetTextureAspect(float aspect)
{
	mCrystalBall->SetTextureAspect(aspect);
}

void SphereStitchImp::SetSingleMode(bool single)
{
	mCrystalBall->SetSingleMode(single);
}

void SphereStitchImp::SetFactoryMode(int factory)
{
	//mCrystalBall->SetFactoryMode(factory);
}

void SphereStitchImp::SetMode(int mode, int vertex_mode)
{
	mStitchMode = mode;
	mAllInOne->SwitchMode(vertex_mode);
}

void SphereStitchImp::RenderFBO()
{
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, &vp[0]);
        mWorldmap->RenderFBO();
//#ifndef __ANDROID__
		glViewport(vp[0], vp[1], vp[2], vp[3]);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);
//#endif
		//mAllInOne->SetTexture(mWorldmap->FBOTexture(), 0);
}


int SphereStitchImp::GetScreenCount()
{
	return 1;
}

int SphereStitchImp::GetScreenMode()
{
	return SCRN_HEMISPHERE;
}

void SphereStitchImp::SetPosition(vec3 pos, bool texture, int scnindex)
{
	mCrystalBall->SetPosition(pos, texture, scnindex);
	mAllInOne->SetPosition(pos, texture, scnindex);
}

void SphereStitchImp::SetScale(vec3 scale, bool texture, int scnindex)
{
	mCrystalBall->SetScale(scale, texture, scnindex);
	mAllInOne->SetScale(scale, texture, scnindex);
}

void SphereStitchImp::setRotate(vec3 rotate, bool texture, int scnindex)
{
	mCrystalBall->setRotate(rotate, texture, scnindex);
	mAllInOne->setRotate(rotate, texture, scnindex);
}

vec3 SphereStitchImp::GetPosition(bool texture, int scnindex)
{
	return mCrystalBall->GetPosition(texture, scnindex);
}

vec3 SphereStitchImp::GetScale(bool texture, int scnindex)
{
	return mCrystalBall->GetScale(texture, scnindex);
}

vec3 SphereStitchImp::GetRotate(bool texture, int scnindex)
{
	return mCrystalBall->GetRotate(texture, scnindex);
}

void SphereStitchImp::CleanTexture(int scnindex) {
	
}

bool SphereStitchImp::LoadTexture(int w, int h, GLenum type, GLvoid* pixels, int scnindex)
{
	mCrystalBall->LoadTexture(w, h, type, pixels, scnindex);
	mWorldmap->LoadTexture(w, h, type, pixels, scnindex);
	return true;
}

void SphereStitchImp::SetTexture(GLuint texid, int scnindex)
{
	//LOGV("settexture ............................%d",texid);
	mCrystalBall->SetTexture(texid, scnindex);
	mWorldmap->SetTexture(texid, scnindex);
	//mAllInOne->SetTexture(texid, scnindex);
}

GLuint SphereStitchImp::GetTexture(int scnindex)
{
	return mCrystalBall->GetTexture(scnindex);
}

bool SphereStitchImp::NeedDraw()
{
	return mCrystalBall->NeedDraw() || mWorldmap->NeedDraw();
}

void SphereStitchImp::StartAnimation(int scnindex)
{
	//mCrystalBall->StartAnimation(scnindex);
}

void SphereStitchImp::StopAnimation(int scnindex)
{
}

void SphereStitchImp::SetNoVideoTexture(GLuint texid)
{
	mCrystalBall->SetNoVideoTexture(texid);
	mWorldmap->SetNoVideoTexture(texid);
}

void SphereStitchImp::LoadOSDTexture(int w, int h, GLenum type, GLvoid* pixels)
{
}

void SphereStitchImp::PutInfo(GLuint texid, int scnindex)
{
}

bool SphereStitchImp::LoadTexture(int w, int h, GLvoid* y_pixels, GLvoid* cbcr_pixels, int scnindex)
{
	return false;
}

void SphereStitchImp::LoadOSDTexture(int w, int h, GLvoid* y_pixels, GLvoid* cbcr_pixels)
{
}

void SphereStitchImp::SetTexture(GLuint y_texid, GLuint cbcr_texid, int scnindex)
{
}

GLuint SphereStitchImp::GetTexture(int scnindex, int yuv)
{
	return 0;
}

void SphereStitchImp::SetNoVideoTexture(GLuint y_texid, GLuint u_texid, GLuint v_texid)
{
}

void SphereStitchImp::ShowOSD(bool value)
{
	mCrystalBall->ShowOSD(value);
}

void SphereStitchImp::AddFrame(IFrameDraw* frame)
{
}

void SphereStitchImp::UpdateFrame(float tbstart, float tbend, float lrstart, float lrend, int index, bool update, bool ani)
{
}

void SphereStitchImp::ShowFrame(bool value)
{
}

void SphereStitchImp::SetBorderColor(GLuint selecttex, GLuint normaltex)
{
}

void SphereStitchImp::SetSelected(bool value, int scnindex)
{
}

int SphereStitchImp::GetSelected(int scnindex)
{
	return 0;
}

void SphereStitchImp::ShowInfo(bool value)
{
}

void SphereStitchImp::ShowRecordStatu(bool value, int index)
{
}

void SphereStitchImp::LoadRecordStatuTexture(GLuint texid, int scnindex)
{
}

void* SphereStitchImp::GetLoading(int index)
{
	return NULL;
}

void SphereStitchImp::SetIsAnimation(int index, bool animation)
{
}

void SphereStitchImp::SetOSDTexture(GLuint texid)
{
}
