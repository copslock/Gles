#include "ThreeScreen.h"
#include "ExpandFrameDraw.h"

ThreeScreen::ThreeScreen(float aspect, void *mgn)
{
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	mNormal = new OneScreen(2, 2, aspect, 360.0f, 0, -0.45, 180.0f, mgn);
	mNormal->SetPosition(vec3(0, 0.52, 0), false, 0);
	mVideo[0] = new Hemisphere(1.5f, aspect, mgn);
	mVideo[0]->SetPosition(vec3(0, 0, 3), false, 0);
	mVideo[0]->setRotate(vec3(60, 0,225), false, 0);

	mVideo[1] = new Hemisphere(1.5f, aspect, mgn);
	mVideo[1]->SetPosition(vec3(0, 0, 3), false, 0);
	mVideo[1]->setRotate(vec3(60, 0, 45), false, 0);

	mVideo[0]->SetSelected(true, 0);
	mSelected = 0;
	m_aspect = aspect;

	mleftangle=225+60*aspect/2;
	mrightangle=mleftangle-360;
	mNormal->AddFrame(new ExpandFrameDraw(1.5,0 , 60, 0,60*aspect,aspect));
	mNormal->ShowFrame(true);
}


ThreeScreen::~ThreeScreen()
{
	delete mNormal;
	for (int i = 0; i < 2; i++) {
		delete mVideo[i];
	}
}

void ThreeScreen::DrawSelf(int scnindex)
{
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	int w = vp[2] / 2;
	int h = vp[3] / 3;
	for (int i = 2; i < 4; i++)
	{
		glViewport((i % 2)*w,(i / 2)*h, w, h*2);
		if(mVideo[i-2]->GetSelected(0))
			mVideo[i-2]->DrawSelf(0x7fffffff);
		else
			mVideo[i-2]->DrawSelf(0);
	}
	glViewport(0, 0,  w * 2, h);
	mNormal->DrawSelf(0);
}

int ThreeScreen::GetScreenCount()
{
	return 3;
}

void ThreeScreen::ResetPosition()
{
//	mNormal->ResetPosition();
//	mVideo[0]->ResetPosition();
//	mVideo[1]->ResetPosition();
}

int ThreeScreen::GetScreenMode()
{
	return SCRN_THREE;
	//return -1;
}

void ThreeScreen::SetPosition(vec3 pos, bool texture, int scnindex)
{
	if (scnindex > 2 || scnindex < 0)
		return;
	if (scnindex == 2)
		mNormal->SetPosition(pos, texture, 0);
	else
		mVideo[scnindex]->SetPosition(pos, texture, 0);
}

void ThreeScreen::SetScale(vec3 scale, bool texture, int scnindex)
{
	if (scnindex > 2 || scnindex < 0)
		return;
	if (scnindex == 2)
		mNormal->SetScale(scale, texture, 0);
	else
		mVideo[scnindex]->SetScale(scale, texture, 0);
}

void ThreeScreen::setRotate(vec3 rotate, bool texture, int scnindex)
{
	if (scnindex > 2 || scnindex < 0)
		return;
	if (scnindex == 2)
		mNormal->setRotate(rotate, texture, 0);
	else
		mVideo[scnindex]->setRotate(rotate, texture, 0);
}

vec3 ThreeScreen::GetPosition(bool texture, int scnindex)
{
	if (scnindex > 2 || scnindex < 0)
		return vec3(0,0,0);
	if (scnindex == 2)
		return mNormal->GetPosition(texture, 0);
	else
		return mVideo[scnindex]->GetPosition(texture,0);
}

vec3 ThreeScreen::GetScale(bool texture, int scnindex)
{
	if (scnindex > 2 || scnindex < 0)
		return vec3(0, 0, 0);
	if (scnindex == 2)
		return mNormal->GetScale(texture, 0);
	else
		return mVideo[scnindex]->GetScale(texture, 0);
}

vec3 ThreeScreen::GetRotate(bool texture, int scnindex)
{
	if (scnindex > 2 || scnindex < 0)
		return vec3(0, 0, 0);
	if (scnindex == 2)
		return mNormal->GetRotate(texture, 0);
	else
		return mVideo[scnindex]->GetRotate(texture, 0);
}

bool ThreeScreen::LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex)
{
	bool ret = mNormal->LoadTexture(w, h, type, pixels, 0);
	for (int i = 0; i < 2; i++)
		mVideo[i]->LoadTexture(w, h, type, pixels, 0);
	return ret;
}

bool ThreeScreen::LoadTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels, int scnindex)
{
	bool ret = mNormal->LoadTexture(w, h, y_pixels,cbcr_pixels, 0);
	for (int i = 0; i < 2; i++)
		mVideo[i]->LoadTexture(w, h, y_pixels, cbcr_pixels, 0);
	return ret;
}

void ThreeScreen::SetTexture(GLuint texid, int scnindex)
{
	mNormal->SetTexture(texid, 0);
	mVideo[0]->SetTexture(texid, 0);
	mVideo[1]->SetTexture(texid, 0);
}

void ThreeScreen::SetTexture(GLuint y_texid, GLuint cbcr_texid, int scnindex)
{
	mNormal->SetTexture(y_texid, cbcr_texid, 0);
	mVideo[0]->SetTexture(y_texid, cbcr_texid, 0);
	mVideo[1]->SetTexture(y_texid, cbcr_texid, 0);
}

GLuint ThreeScreen::GetTexture(int scnindex)
{
	if (scnindex > 2 || scnindex < 0)
		return 0;
	if (scnindex == 2)
		return mNormal->GetTexture(0);
	else
		return mVideo[scnindex]->GetTexture(0);
}

GLuint ThreeScreen::GetTexture(int scnindex, int yuv)
{
	return 0;
}

void ThreeScreen::SetNoVideoTexture(GLuint texid)
{
	mNormal->SetNoVideoTexture(texid);
	for (int i = 0; i < 2; i++)
		mVideo[i]->SetNoVideoTexture(texid);
}

void ThreeScreen::SetNoVideoTexture(GLuint y_texid, GLuint u_texid, GLuint v_texid)
{

}

void ThreeScreen::SetBorderColor(GLuint selecttex, GLuint normaltex)
{
	for (int i = 0; i < 2; i++)
		mVideo[i]->SetBorderColor(selecttex, normaltex);
}

void ThreeScreen::SetSelected(bool value, int scnindex)
{
	if (scnindex < 0 || scnindex>1)
		return;
	for (int i = 0; i < 2; i++)
		mVideo[i]->SetSelected(false, 0);
	mSelected = scnindex;
	mVideo[scnindex]->SetSelected(value, 0);
}

int ThreeScreen::GetSelected(int scnindex)
{
//	if (scnindex < 0 || scnindex>1)
//		return false;
//	return mVideo[scnindex]->GetSelected(0);
	return mSelected;
}


bool ThreeScreen::NeedDraw()
{
	return mNormal->NeedDraw() || mVideo[0]->NeedDraw() || mVideo[1]->NeedDraw();
}

void ThreeScreen::StartAnimation(int scnindex)
{

}

void ThreeScreen::StopAnimation(int scnindex)
{

}

void ThreeScreen::UpdateFrame(float tbstart, float tbend, float lrstart, float lrend,int index,bool update,bool ani)
{
	vec3 lscale = mVideo[mSelected]->GetScale(false, 0);
	vec3 lrotate = mVideo[mSelected]->GetRotate(false, 0);
	float langle = 60 - (100 * (lscale.y - 1) * 8)/55.0;
	if(update)
	{
		lrotate.z = mleftangle-tbstart/tbend*360.0f;
		if(lrotate.z<mrightangle)
			lrotate.z =mrightangle;
		if(lrotate.z>mleftangle)
			lrotate.z = mleftangle;
		lrotate.x = 90-(lrstart-lrend/3*2)/(lrend/3)*70;
		mVideo[mSelected]->setRotate(lrotate,false,0);
		//printf("[SIXSCREEN]-----------------------------lrotate.x:%f z:%f\n", lrotate.x,lrotate.z);
	}
	//printf("[THREESCREEN]---------------lrotate.x:%f z:%f langle:%f\n", lrotate.x, lrotate.z,langle);
	float lstartud = 70-(lrotate.x-20);
	float lstartlr = mleftangle-lrotate.z;
	mNormal->UpdateFrame(lstartud-langle/2, lstartud+langle/2, lstartlr-langle*m_aspect/2, lstartlr + langle*m_aspect/2, 0, false,false);
}
