#include "sixscreen.h"
#include "ParametricManager.h"
#include <cstdio>


SixScreen::SixScreen(float aspect, void *mgn)
{
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
//	mNormal = new NormalPlay(2,aspect,0,-0.5, mgn);
//	mNormal->SetPosition(vec3(0, 0.52, 0), false, 0);
	mNormal = new Hemisphere(1.7, aspect,mgn);
	//mNormal->SetScale(vec3(1.0, 1.1, 1.1), false, 0);
	for(int i=0;i<5;i++)
	{
		mVideo[i] = new Hemisphere(1.7f, aspect, mgn);
		mVideo[i]->SetPosition(vec3(0, 0, 3), false, 0);
		mVideo[i]->setRotate(vec3(60, 0, i*60), false, 0);
	}
//		mNormal->AddFrame(5, 5, vp[2] / 4 - 10, vp[3]-10);
	m_aspect = aspect;
	mVideo[0]->SetSelected(true, 0);
	mSelected = 0;
	mNormal->AddFrame(new SixFrameDraw(1.7,0,30,40,40+60*aspect,aspect));
	mNormal->ShowFrame(true);
	mManager = mgn;
}


SixScreen::~SixScreen()
{
	delete mNormal;
	for (int i = 0; i < 5; i++) {
		delete mVideo[i];
	}
}

void SixScreen::DrawSelf(int scnindex)
{
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	int w = vp[2] / 3;
	int h = vp[3] / 3;
	for (int i = 2; i < 5; i++)
	{
		glViewport((i-2)*w, 0, w, h);
		if (mVideo[i]->GetSelected(0))
			mVideo[i]->DrawSelf(0x7fffffff);
		else
			mVideo[i]->DrawSelf(0);
	}
	glViewport(w * 2, h * 2, w, h);
	if (mVideo[0]->GetSelected(0))
		mVideo[0]->DrawSelf(0x7fffffff);
	else
		mVideo[0]->DrawSelf(0);
	glViewport(w * 2, h, w, h);
	if (mVideo[1]->GetSelected(0))
		mVideo[1]->DrawSelf(0x7fffffff);
	else
		mVideo[1]->DrawSelf(0);
	glViewport(0, h, w * 2, h*2);
	mNormal->DrawSelf(mSelected);
}

int SixScreen::GetScreenCount()
{
	return 6;
}

void SixScreen::ResetPosition()
{
	//	mNormal->ResetPosition();
	//	mVideo[0]->ResetPosition();
	//	mVideo[1]->ResetPosition();
}

int SixScreen::GetScreenMode()
{
	return SCRN_SIX;
	//return -1;
}

void SixScreen::SetPosition(vec3 pos, bool texture, int scnindex)
{
	if (scnindex > 5 || scnindex < 0)
		return;
	if (scnindex == 5)
		mNormal->SetPosition(pos, texture, 0);
	else
		mVideo[scnindex]->SetPosition(pos, texture, 0);
}

void SixScreen::SetScale(vec3 scale, bool texture, int scnindex)
{
	if (scnindex > 5 || scnindex < 0)
		return;
	if (scnindex == 5)
		mNormal->SetScale(scale, texture, 0);
	else
		mVideo[scnindex]->SetScale(scale, texture, 0);
}

void SixScreen::setRotate(vec3 rotate, bool texture, int scnindex)
{
	if (scnindex > 5 || scnindex < 0)
		return;
	if (scnindex == 5)
		mNormal->setRotate(rotate, texture, 0);
	else
		mVideo[scnindex]->setRotate(rotate, texture, 0);
}

vec3 SixScreen::GetPosition(bool texture, int scnindex)
{
	if (scnindex > 5 || scnindex < 0)
		return vec3(0, 0, 0);
	if (scnindex == 5)
		return mNormal->GetPosition(texture, 0);
	else
		return mVideo[scnindex]->GetPosition(texture, 0);
}

vec3 SixScreen::GetScale(bool texture, int scnindex)
{
	if (scnindex > 5 || scnindex < 0)
		return vec3(0, 0, 0);
	if (scnindex == 5)
		return mNormal->GetScale(texture, 0);
	else
		return mVideo[scnindex]->GetScale(texture, 0);
}

vec3 SixScreen::GetRotate(bool texture, int scnindex)
{
	if (scnindex > 5 || scnindex < 0)
		return vec3(0, 0, 0);
	if (scnindex == 5)
		return mNormal->GetRotate(texture, 0);
	else
		return mVideo[scnindex]->GetRotate(texture, 0);
}

bool SixScreen::LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex)
{
	bool ret = mNormal->LoadTexture(w, h, type, pixels, 0);
	for (int i = 0; i < 5; i++)
		mVideo[i]->LoadTexture(w, h, type, pixels, 0);
	return ret;
}

bool SixScreen::LoadTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels, int scnindex)
{
	bool ret = mNormal->LoadTexture(w, h, y_pixels, cbcr_pixels, 0);
	for (int i = 0; i < 5; i++)
		mVideo[i]->LoadTexture(w, h, y_pixels, cbcr_pixels, 0);
	return ret;
}

void SixScreen::SetTexture(GLuint texid, int scnindex)
{
	mNormal->SetTexture(texid, 0);
	for(int i=0;i<5;i++)
		mVideo[i]->SetTexture(texid, 0);
}

void SixScreen::SetTexture(GLuint y_texid, GLuint cbcr_texid, int scnindex)
{
	mNormal->SetTexture(y_texid, cbcr_texid, 0);
	for (int i = 0; i < 5; i++)
		mVideo[i]->SetTexture(y_texid, cbcr_texid, 0);
}

GLuint SixScreen::GetTexture(int scnindex)
{
	if (scnindex > 5 || scnindex < 0)
		return 0;
	if (scnindex == 5)
		return mNormal->GetTexture(0);
	else
		return mVideo[scnindex]->GetTexture(0);
}

GLuint SixScreen::GetTexture(int scnindex, int yuv)
{
	return 0;
}

void SixScreen::SetNoVideoTexture(GLuint texid)
{
	mNormal->SetNoVideoTexture(texid);
	for (int i = 0; i < 5; i++)
		mVideo[i]->SetNoVideoTexture(texid);
}

void SixScreen::SetNoVideoTexture(GLuint y_texid, GLuint u_texid, GLuint v_texid)
{

}

void SixScreen::UpdateFrame(float tbstart, float tbend, float lrstart, float lrend, int index,bool update,bool ani)
{
	if (index < 0)
		index = mSelected;
	vec3 lscale = mVideo[mSelected]->GetScale(false, 0);
	vec3 lrotate = mVideo[mSelected]->GetRotate(false, 0);
	float langle = 60 - (100 * (lscale.y - 1) * 8)/55.0;
	//printf("[SIXSCREEN]----------------%f,%f,%f,%f,lrotate.x:%f z:%f\n", tbstart, lrstart, tbend, lrend, lrotate.x, lrotate.z);
	if(update)
	{
		if (index != mSelected)
		{
			lscale = mVideo[index]->GetScale(false, 0);
			lrotate = mVideo[index]->GetRotate(false, 0);
			langle = 60 - (100 * (lscale.y - 1) * 8);
		}
		tbend = tbend / 3 * 2;
		lrend = lrend / 3 * 2;
		lrstart = lrstart - lrend / 2;
		tbstart = tbstart - tbend / 2;
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
			mgn->StartAnimation(mVideo[index], lrotate, 30, 30, 0, JA_ANI_TYPE_ROTATE, false, 0, true, NULL, NULL);
		}
		else
			mVideo[index]->setRotate(lrotate, false, 0);
		if (index != mSelected)
			return;
		//printf("[SIXSCREEN]-----------------------------lrotate.x:%f z:%f\n", lrotate.x,lrotate.z);
	}
	float lstartud = (90 - lrotate.x) - langle / 2;
	float lstartlr = lrotate.z + (90 - langle*m_aspect / 2);
	mNormal->UpdateFrame(lstartud, lstartud + langle, lstartlr, lstartlr + langle*m_aspect, index, false,false);
}

void SixScreen::SetBorderColor(GLuint selecttex, GLuint normaltex)
{
	for (int i = 0; i < 5; i++)
		mVideo[i]->SetBorderColor(selecttex, normaltex);
}

void SixScreen::SetSelected(bool value, int scnindex)
{
	if (scnindex < 0 || scnindex>4)
		return;
	for (int i = 0; i < 5; i++)
		mVideo[i]->SetSelected(false, 0);
	mVideo[scnindex]->SetSelected(value, 0);
	mSelected = scnindex;
}

int SixScreen::GetSelected(int scnindex)
{
//	if (scnindex < 0 || scnindex>4)
//		return false;
//	return mVideo[scnindex]->GetSelected(0);
	return mSelected;
}

bool SixScreen::NeedDraw()
{
	return mNormal->NeedDraw() || mVideo[0]->NeedDraw() || mVideo[1]->NeedDraw() || mVideo[2]->NeedDraw() || mVideo[3]->NeedDraw() || mVideo[4]->NeedDraw();
}

void SixScreen::StartAnimation(int scnindex)
{

}

void SixScreen::StopAnimation(int scnindex)
{

}
