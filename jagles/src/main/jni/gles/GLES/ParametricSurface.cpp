#include "ParametricSurface.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

ParametricSurface::~ParametricSurface()
{
	Cleanup();
}

ParametricSurface::ParametricSurface() {
	mScreenCount = 1;
	mPosition = vec3(0,0,0);
	mRotate = vec3(0,0,0);
	mScale = vec3(1,1,1);
	mTexPosition = vec3(0,0,0);
	mTexRotate = vec3(0,0,0);
	mTexScale = vec3(1,1,1);
	_texid = 0;
	_texwidth = 0;
	_texheight = 0;
	_NoVideoTexID = 0;
	mNeedDraw = false;
	lvb = 0;
	Visible = true;
	DebugIndex = 0xff;
	_scnindex = -1;
	vertexBuffer = 0;
	TextureCoordBuffer = 0;
	memset(_yuvtexid,0,sizeof(_yuvtexid));
	memset(_yuvNoVideoTexID,0,sizeof(_yuvNoVideoTexID));
	mIsYUV = false;
}

void ParametricSurface::ResetPosition() {
	mPosition = vec3(0,0,0);
	mRotate = vec3(0,0,0);
	mScale = vec3(1,1,1);
	mTexPosition = vec3(0,0,0);
	mTexRotate = vec3(0,0,0);
	mTexScale = vec3(1,1,1);
	mNeedDraw = true;
}

int ParametricSurface::GetScreenCount() {
	return mScreenCount;
}

int ParametricSurface::GetScreenMode() {
	return mScreenMode;
}

void ParametricSurface::SetInterval(const ParametricInterval& interval)
{
    anglespan = interval.angleSpan;
    m_startangle = interval.StartAngle;
    m_endangle = interval.EndAngle;
    //m_slices = m_divisions - ivec2(1, 1);
}

void ParametricSurface::Cleanup() {
	if(_texid) {
		glDeleteTextures(1, &_texid);
		_texid = 0;
	}
	if (vertexBuffer != 0) {
		glDeleteBuffers(1, &vertexBuffer);
//		LOGDEBUG("glDeletebuffers vertexBuffer:%d,%p",vertexBuffer,this);
        LOGDEBUG("5---glDeleteBuffers:%d %p",vertexBuffer,this);
        vertexBuffer = 0;

    }
	if (TextureCoordBuffer != 0) {
		glDeleteBuffers(1, &TextureCoordBuffer);
//		LOGDEBUG("glDeletebuffers TextureCoordBuffer:%d,%p",TextureCoordBuffer,this);
        LOGDEBUG("6---glDeleteBuffers:%d %p",TextureCoordBuffer,this);

        TextureCoordBuffer = 0;
	}
	if(_programHandle)
		glDeleteProgram(_programHandle);
	if (lvb != 0);
		glDeleteBuffers(1, &lvb);
	printf("ParametricSurface Destroy........\n");
}

void ParametricSurface::setupProjection() {
	_Zoom = -3;
	glUseProgram(_programHandle);
	ksMatrixLoadIdentity((KSMatrix4 *)&_projectionMatrix);

	ksPerspective((KSMatrix4 *)&_projectionMatrix, 60.0, m_aspect, 0.0001f, 1200.0f);
	//ksOrtho(&_projectionMatrix,-m_aspect,m_aspect,-1,1,1,10);
	//ksLookAt()

	// Load projection matrix
	glUniformMatrix4fv(_projectionSlot, 1, GL_FALSE, (GLfloat *) &_projectionMatrix.m[0][0]);
	//printf("glUniformMatrix4fv0:%d\n",glGetError());
	ksMatrixLoadIdentity(&_rotationMatrix);
}

GLuint ParametricSurface::LoadShader(GLenum type, const char *src) {
	GLuint shader = glCreateShader(type);
	if (shader == 0) {
		//NSLog(@"Error: failed to create shader.");
		return 0;
	}

	// Load the shader source
	glShaderSource(shader, 1, &src , NULL);

	// Compile the shader
	glCompileShader(shader);

	// Check the compile status
	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {
		GLint infoLen = 0;
		glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

		if (infoLen > 1) {
			char * infoLog =(char *) malloc(sizeof(char) * infoLen);
			glGetShaderInfoLog (shader, infoLen, NULL, infoLog);
			printf("Error compiling shader:\n%s\n", infoLog );

			free(infoLog);
		}

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

void ParametricSurface::SetupBuffer() {

	if(DebugIndex==0)
		LOGDEBUG("NormalScreen Setupbuffer------------");

	int lvs = GetVertexSize();
	int vBufSize = GetVertexCount() * lvs;
	int vCount = GetVertexCount();
	GLfloat *vbuf = new GLfloat[vBufSize];

	int CoordSize = GetCoordSize();
	int vCoorBufSize = GetCoordCount() * CoordSize;
	GLfloat *vcoordbuf = new GLfloat[vCoorBufSize];

	GenerateVertices(vbuf,vcoordbuf);
	lvb = vertexBuffer;
	if(lvb!=0);
		glDeleteBuffers(1, &lvb);
    //printf("\n\n");
	glGenBuffers(1, &lvb);
	LOGDEBUG("5--glGenBuffers:%d %p",lvb,this);

	//printf("gl error number:%d\n\n",glGetError());
	glBindBuffer(GL_ARRAY_BUFFER, lvb);
	glBufferData(GL_ARRAY_BUFFER, vBufSize * sizeof(GLfloat), vbuf, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER,0);
    //printf("set vexture buffer:%d\n",glGetError());
	GLuint CoordBuffer=TextureCoordBuffer;
	if(CoordBuffer!=0)
		glDeleteBuffers(1,&CoordBuffer);
	glGenBuffers(1, &CoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, CoordBuffer);
	LOGDEBUG("6--glGenBuffers:%d %p",CoordBuffer,this);

	glBufferData(GL_ARRAY_BUFFER, vCoorBufSize * sizeof(GLfloat), vcoordbuf, GL_DYNAMIC_DRAW);

    //printf("set Coord buffer:%d\n",glGetError());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] vbuf;
	delete[] vcoordbuf;

	vertexBuffer = lvb;
	vertexSize = lvs;
	triangleIndexCount = vCount;
	TextureCoordSize = CoordSize;
	TextureCoordBuffer = CoordBuffer;
	//printf("vCount:%d\n",vCount);
}

int ParametricSurface::GetCoordSize() {
	return 2;
}

int ParametricSurface::GetCoordCount() {
	return ((m_endangle.x-m_startangle.x)/anglespan)*((m_endangle.y-m_startangle.y)/anglespan)*6;
}

int ParametricSurface::GetVertexSize()
{
	int floatsPerVertex = 3;
	return floatsPerVertex;
}

int ParametricSurface::GetVertexCount()
{
    return ((m_endangle.x-m_startangle.x)/anglespan)*((m_endangle.y-m_startangle.y)/anglespan)*6;
}

void ParametricSurface::updateSurfaceTransform() {
	ksMatrixLoadIdentity(&_modelViewMatrix);

	ksMatrixLoadIdentity(&_rotationMatrix);
	KSMatrix4 lrotate,ltmprotate;
	ksMatrixLoadIdentity(&lrotate);
	if(mRotate.x) {
//		ksMatrixLoadIdentity(&ltmprotate);
		ksRotate(&lrotate, mRotate.x, 1, 0, 0);
//		ksMatrixMultiply(&lrotate, &lrotate, &ltmprotate);
	}
	if(mRotate.y) {
//		ksMatrixLoadIdentity(&ltmprotate);
		ksRotate(&lrotate, mRotate.y, 0, 1, 0);
//		ksMatrixMultiply(&lrotate, &lrotate, &ltmprotate);
	}
	if(mRotate.z) {
//		ksMatrixLoadIdentity(&ltmprotate);
		ksRotate(&lrotate, mRotate.z, 0, 0, 1);
//		ksMatrixMultiply(&lrotate, &lrotate, &ltmprotate);
	}
	ksMatrixMultiply(&_rotationMatrix,&_rotationMatrix,&lrotate);
    ksTranslate(&_modelViewMatrix, 0.0, 0.0, _Zoom);

	ksTranslate(&_modelViewMatrix,mPosition.x,mPosition.y,mPosition.z);
	//ksScale(&_rotationMatrix,mScale.x,mScale.y,mScale.z);
	//使用另外一种模式来放大
	if(GetScreenMode()!=SCRN_CYLINDER&&GetScreenMode()!=SCRN_ONENORMAL) {
		ksMatrixLoadIdentity((KSMatrix4 *) &_projectionMatrix);
		ksPerspective((KSMatrix4 *) &_projectionMatrix, 60.0 / mScale.x, m_aspect, 0.0001f, 1200.0f);
		glUniformMatrix4fv(_projectionSlot, 1, GL_FALSE, (GLfloat *) &_projectionMatrix.m[0][0]);
	}
	else
		ksScale(&_rotationMatrix,mScale.x,mScale.y,mScale.z);


	///////////

	ksMatrixMultiply(&_modelViewMatrix, &_rotationMatrix, &_modelViewMatrix);
	glUniformMatrix4fv(_modelViewSlot, 1, GL_FALSE, (GLfloat *) &_modelViewMatrix.m[0][0]);
	//printf("glUniformMatrix4fv:%d\n",glGetError());
}

void ParametricSurface::DrawSelf(int scnindex)
{
    mNeedDraw = false;
	if(!Visible)
		return ;

	//printf("NeedDraw.....%d\n",mNeedDraw);
	glUseProgram(_programHandle);

	updateSurfaceTransform();

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(_positionSlot, 3, GL_FLOAT, GL_FALSE, vertexSize * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(_positionSlot);

	//printf("glEnableVertexAttribArray:%d\n",glGetError());

    if(_coordSlot>=0)
    {
	glBindBuffer(GL_ARRAY_BUFFER,TextureCoordBuffer);
	glVertexAttribPointer(_coordSlot, 2, GL_FLOAT, GL_FALSE, TextureCoordSize * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(_coordSlot);
    }

	if(mIsYUV)
	{
		for (int i = 0; i < 2; ++i) {
//			printf("texture......%d\n",_yuvtexid[i]);
			glActiveTexture(GL_TEXTURE0 + i);
			if(_yuvtexid[0])
				glBindTexture(GL_TEXTURE_2D, _yuvtexid[i]);
			else
				glBindTexture(GL_TEXTURE_2D, _yuvNoVideoTexID[i]);
			glUniform1i(_uniformSamplers[i], i);
		}
	}
	else {
		glActiveTexture(GL_TEXTURE0);
		if (_texid)
			glBindTexture(GL_TEXTURE_2D, _texid);
		else
			glBindTexture(GL_TEXTURE_2D, _NoVideoTexID);
	}
	glDrawArrays(GL_TRIANGLES, 0, triangleIndexCount);
//	glDrawArrays(GL_LINES, 0, triangleIndexCount);


	glBindTexture(GL_TEXTURE_2D, 0);
    //glDrawArrays(GL_LINES, 0, triangleIndexCount);
	glDisableVertexAttribArray(_positionSlot);
    if(_coordSlot>=0)
	glDisableVertexAttribArray(_coordSlot);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParametricSurface::setZoom(GLfloat multi,int scnindex) {
	_Zoom = multi;
}

vec2 ParametricSurface::ComputeDomain(float x, float y)
{
    return vec2(x,y);
}

vec2 ParametricSurface::ComputeCoordDomain(float x, float y)
{
	return vec2(x,y);
}


void ParametricSurface::GenerateVertices(float * vertices,float *texturecorrd)
{
	float* attribute = vertices;
	float* attribute1 = texturecorrd;
	int lcount = 0;
	for (int j = m_startangle.y; j < m_endangle.y; j+=anglespan) {
		for (int i = m_startangle.x; i < m_endangle.x; i+=anglespan) {

			// Compute Position
			if(i==m_startangle.x)
				mxStart = 1;
			else
				mxStart = 0;
			if(i>=m_endangle.x-anglespan&&i<m_endangle.x)
				mxEnd = 1;
			else
				mxEnd = 0;
			vec2 domain = ComputeCoordDomain(i, j);
			vec3 range = Evaluate(domain);
			attribute = range.Write(attribute);

			domain = ComputeCoordDomain(i-anglespan, j);
			range = Evaluate(domain);
			attribute = range.Write(attribute);

			domain = ComputeCoordDomain(i, j-anglespan);
			range = Evaluate(domain);
			attribute = range.Write(attribute);

			domain = ComputeCoordDomain(i, j-anglespan);
			range = Evaluate(domain);
			attribute = range.Write(attribute);

			domain = ComputeCoordDomain(i-anglespan, j);
			range = Evaluate(domain);
			attribute = range.Write(attribute);

			domain = ComputeCoordDomain(i-anglespan, j-anglespan);
			range = Evaluate(domain);
			attribute = range.Write(attribute);

			lcount ++;

			//calc texturecoord

			domain = ComputeDomain(i, j);
			vec2 range1 = EvaluateCoord(domain);
			attribute1 = range1.Write(attribute1);

			domain = ComputeDomain(i-anglespan, j);
			range1 = EvaluateCoord(domain);
			attribute1 = range1.Write(attribute1);

			domain = ComputeDomain(i, j-anglespan);
			range1 = EvaluateCoord(domain);
			attribute1 = range1.Write(attribute1);

			domain = ComputeDomain(i, j-anglespan);
			range1 = EvaluateCoord(domain);
			attribute1 = range1.Write(attribute1);

			domain = ComputeDomain(i-anglespan, j);
			range1 = EvaluateCoord(domain);
			attribute1 = range1.Write(attribute1);

			domain = ComputeDomain(i-anglespan, j-anglespan);
			range1 = EvaluateCoord(domain);
			attribute1 = range1.Write(attribute1);

		}
	}
	//printf("all vertex piont:%d\n",lcount);
}

void ParametricSurface::setupProgram(const char *segsrc, const char *fragsrc) {
	GLuint vertexShader = LoadShader(GL_VERTEX_SHADER,segsrc);
	if (vertexShader == 0) {
		printf("error load segment shader:%d\n",glGetError());
		return;
	}

	GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER,fragsrc);
	if (fragmentShader == 0) {
		glDeleteShader(vertexShader);
		printf("error load fragment shader:%d\n",glGetError());
		return;
	}

	// Create the program object
	_programHandle = glCreateProgram();
	if (_programHandle == 0) {
		printf("error glCreateProgram shader:%d\n", glGetError());
		return;
	}
	glAttachShader(_programHandle, vertexShader);
	glAttachShader(_programHandle, fragmentShader);

	// Link the program
	glLinkProgram(_programHandle);

	// Check the link status
	GLint linked;
	glGetProgramiv(_programHandle, GL_LINK_STATUS, &linked);

	if (!linked) {
		GLint infoLen = 0;
		glGetProgramiv(_programHandle, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1){
			char * infoLog =(char *) malloc(sizeof(char) * infoLen);
			glGetProgramInfoLog(_programHandle, infoLen, NULL, infoLog);

			printf("Error linking program:\n%s\n", infoLog);

			free(infoLog);
		}

		glDeleteProgram(_programHandle );
		return;
	}

	// Free up no longer needed shader resources
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(_programHandle);

	//printf("value:%d,_programHandle:%d\n",_programHandle,glGetError());
	// Get the attribute position slot from program
	//
	_positionSlot = glGetAttribLocation(_programHandle, "vPosition");

	//printf("value:%d,_positionSlot:%d\n",_positionSlot,glGetError());

	// Get the attribute color slot from program
	//

	_coordSlot = glGetAttribLocation(_programHandle,"aTexCoor");
	//printf("value:%d,_coordSlot:%d\n",_coordSlot,glGetError());

	// Get the uniform model-view matrix slot from program
	//
	_modelViewSlot = glGetUniformLocation(_programHandle, "modelView");
	//printf("value:%d,_modelViewSlot:%d\n",_modelViewSlot,glGetError());

	_projectionSlot = glGetUniformLocation(_programHandle, "projection");
	//printf("value:%d,_projectionSlot:%d\n",_projectionSlot,glGetError());

	if(mIsYUV) {
		_uniformSamplers[0] = glGetUniformLocation(_programHandle, "s_texture_y");
		_uniformSamplers[1] = glGetUniformLocation(_programHandle, "s_texture_cbcr");
	}
}

void ParametricSurface::SetPosition(vec3 pos, bool texture,int scnindex)
{
	if(texture)
	{
		if(!(mTexPosition ==pos)) {
			mTexPosition = pos;
			mNeedDraw = true;
		}
	}
	else
	{
		if(!(mPosition ==pos))
		{
			mPosition = pos;
			mNeedDraw = true;
		}
	}
}

void ParametricSurface::SetScale(vec3 scale, bool texture,int scnindex)
{
	if(texture)
	{
		if(!(mTexScale ==scale))
		{
			mTexScale = scale;
			mNeedDraw = true;
		}
	}
	else
	{
		if(!(mScale == scale))
		{
			mScale = scale;
			mNeedDraw = true;
		}
	}
}

void ParametricSurface::setRotate(vec3 rotate, bool texture,int scnindex)
{
	if(texture)
	{
		if(!(mTexRotate == rotate))
		{
			mTexRotate = rotate;
			mNeedDraw = true;
		}
	}
	else
	{
		if(!(mRotate == rotate))
		{
			mRotate = rotate;
			mNeedDraw = true;
		}
	}
}

vec3 ParametricSurface::GetPosition(bool texture,int scnindex)
{
	if(texture)
		return mTexPosition;
	else
		return mPosition;
}

vec3 ParametricSurface::GetScale(bool texture,int scnindex)
{
	if(texture)
		return mTexScale;
	else
		return mScale;
}

vec3 ParametricSurface::GetRotate(bool texture,int scnindex)
{
	if(texture)
		return mTexRotate;
	else
		return mRotate;
}

void ParametricSurface::CleanTexture(int scnindex) {
    if (_texid) {
        glDeleteTextures(1, &_texid);
    }
    _texid = 0;

}

bool ParametricSurface::LoadTexture(int w,int h,GLenum type,GLvoid *pixels,int scnindex)
{
	_scnindex = scnindex;
    if (_texwidth != w || _texheight != h) {
        //printf("textid1...........:%d %d %lx\n",_texid,scnindex,(long)this);
		if(_texid) {
			glDeleteTextures(1, &_texid);
			LOGDEBUG("glDeleteTextures---->%d  %p texw:%d texh:%d w:%d  h:%d",_texid,this,_texwidth,_texheight,w,h);
		}
        _texid = 0;
    }

    if (!_texid) {
        _texwidth = w;
        _texheight = h;
        GLuint texture_object_id = 0;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &texture_object_id);
//		LOGDEBUG("10--%d --- glGenTextures:%d prog:%d posslot:%d coorslot:%d mvslot:%d projslot:%d mZoom:%f Tex.x:%f Tex.y:%f %p",DebugIndex, texture_object_id,_programHandle,_positionSlot,_coordSlot,_modelViewSlot,_projectionSlot,1/mTexScale.x,mTexPosition.x,mTexPosition.y,this);
//        LOGDEBUG("10--%d --- vertexBuffer:%d,TextureCoordBuffer:%d",DebugIndex,vertexBuffer,TextureCoordBuffer);
        //assert(texture_object_id != 0);

        glBindTexture(GL_TEXTURE_2D, texture_object_id);
		if (type == GL_RGBA) {
			glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, type, GL_UNSIGNED_BYTE, pixels);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, type, GL_UNSIGNED_SHORT_5_6_5, pixels);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        _texid = texture_object_id;
    }
    else {
//		if(DebugIndex==1)
//			LOGDEBUG("glTexSubImage2D--->%d  %p",_texid,this);
        glBindTexture(GL_TEXTURE_2D, _texid);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _texwidth, _texheight, type, GL_UNSIGNED_BYTE, pixels);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
	mNeedDraw = true;
    return _texid;
}

bool ParametricSurface::LoadTexture(int w, int h, void *buffer,int scnindex)
{
//	if (_texwidth != w || _texheight != h) {
//		//printf("textid1...........:%d %d %lx\n",_texid,scnindex,(long)this);
//		if (_texid)
//			glDeleteTextures(1, &_texid);
//		_texid = 0;
//	}
//	if (!_texid) {
//
//		_texwidth = w;
//		_texheight = h;
//		GLuint texture_object_id = 0;
//		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//		glGenTextures(1, &texture_object_id);
//		//assert(texture_object_id != 0);
//
//		glBindTexture(GL_TEXTURE_2D, texture_object_id);
//
//#ifdef __ANDROID__
//		GraphicBuffer * mGraphicBuffer = (GraphicBuffer *)buffer;
//		EGLImageKHR _imageKHR = eglCreateImageKHR(eglGetCurrentDisplay(),
//			NULL,
//			EGL_NATIVE_BUFFER_ANDROID,
//			(EGLClientBuffer)mGraphicBuffer->getNativeBuffer(),
//			NULL);
//		glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, (GLeglImageOES)_imageKHR);
//#endif
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		
//
//		glBindTexture(GL_TEXTURE_2D, 0);
//
//		_texid = texture_object_id;
//	//	JACONNECT_INFO("texture_object_id...%d",_texid);
//	}
////	else {
////		glBindTexture(GL_TEXTURE_2D, _texid);
////
////#ifdef __ANDROID__
////		EGLImageKHR _imageKHR = eglCreateImageKHR(eglGetCurrentDisplay(),
////			NULL,
////			EGL_NATIVE_BUFFER_ANDROID,
////			(EGLClientBuffer)buffer,
////			NULL);
////		glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, (GLeglImageOES)_imageKHR);
////#endif
////		glGenerateMipmap(GL_TEXTURE_2D);
////		glBindTexture(GL_TEXTURE_2D, 0);
////	}
//	mNeedDraw = true;
    return _texid;
}

void ParametricSurface::SetTexture(GLuint texid,int scnindex)
{
	_texid = texid;
	mNeedDraw = true;
}

GLuint ParametricSurface::GetTexture(int scnindex)
{
	return _texid;
}

bool ParametricSurface::NeedDraw() {
	//printf("************needdraw:%d,%d\n",mNeedDraw , Visible);
	return mNeedDraw & Visible;
}

void ParametricSurface::SetNoVideoTexture(GLuint texid)
{
	_NoVideoTexID = texid;
}


bool ParametricSurface::LoadTexture(int w,int h,GLvoid *y_pixels,GLvoid *cbcr_pixels,int scnindex)
{
	int widths[2] = { w,w/2};
	int heights[2] = {h,h/2};
	GLvoid *pixels[2] = {y_pixels,cbcr_pixels};
    if(_yuvtexid[0])
		glDeleteTextures(2,_yuvtexid);
    glGenTextures(2, _yuvtexid);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D,_yuvtexid[0]);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_LUMINANCE,
				 widths[0],
				 heights[0],
				 0,
				 GL_LUMINANCE,
				 GL_UNSIGNED_BYTE,
				 pixels[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	printf("glgeterror1------%d\n",glGetError());

	glBindTexture(GL_TEXTURE_2D,_yuvtexid[1]);
	glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_LUMINANCE_ALPHA,
			widths[1],
			heights[1],
			0,
			GL_LUMINANCE_ALPHA,
			GL_UNSIGNED_BYTE,
			pixels[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	printf("glgeterror2------%d\n",glGetError());

	mNeedDraw = true;
	return true;
}

void ParametricSurface::SetTexture(GLuint y_texid,GLuint cbcr_texid,int scnindex)
{
	_yuvtexid[0] = y_texid;
	_yuvtexid[1] = cbcr_texid;
}

GLuint ParametricSurface::GetTexture(int scnindex,int yuv)
{
	if(yuv<0||yuv>2)
		return 0;
	return _yuvtexid[yuv];
}

void ParametricSurface::SetNoVideoTexture(GLuint y_texid,GLuint u_texid,GLuint v_texid)
{
	_yuvNoVideoTexID[0] = y_texid;
	_yuvNoVideoTexID[1] = u_texid;
	_yuvNoVideoTexID[2] = v_texid;
}

void ParametricSurface::setupProgram(const char *segsrc,const char *fragsrc,bool yuv)
{
	mIsYUV = yuv;
	setupProgram(segsrc,fragsrc);
}
