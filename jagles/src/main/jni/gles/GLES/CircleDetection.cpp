//
// Created by LiHong on 16/7/24.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include "CircleDetection.h"


//const char *CircleDetectionSeg = "uniform mat4 projection;\n"
//        "uniform mat4 modelView;\n"
//        "attribute vec4 vPosition;\n"
//        "attribute vec2 aTexCoor;\n"
//        "uniform float aLeftOffset;\n"
//        "uniform float aTopOffset;\n"
//        "uniform float aZoom;\n"
//        "\n"
//        "varying vec2 vTextureCoord;\n"
//        "\n"
//        "void main(void)\n"
//        "{\n"
//        "    gl_Position = projection * modelView * vPosition;\n"
//        "    vTextureCoord = aTexCoor;\n"
//        "}";

const char *CircleDetectionSeg=
        "uniform mat4 projection;\n"
        "uniform mat4 modelView;\n"
        "attribute vec4 vPosition;\n"
        "attribute vec2 aTexCoor;\n"
        " \n"
        " uniform float texelWidth;\n"
        " uniform float texelHeight; \n"
        " \n"
        " varying vec2 vTextureCoord;\n"
        " varying vec2 leftTextureCoordinate;\n"
        " varying vec2 rightTextureCoordinate;\n"
        " \n"
        " varying vec2 topTextureCoordinate;\n"
        " varying vec2 topLeftTextureCoordinate;\n"
        " varying vec2 topRightTextureCoordinate;\n"
        " \n"
        " varying vec2 bottomTextureCoordinate;\n"
        " varying vec2 bottomLeftTextureCoordinate;\n"
        " varying vec2 bottomRightTextureCoordinate;\n"
        " \n"
        " void main()\n"
        " {\n"
        "     gl_Position = projection * modelView * vPosition;\n"
        "     \n"
        "     vec2 widthStep = vec2(texelWidth, 0.0);\n"
        "     vec2 heightStep = vec2(0.0, texelHeight);\n"
        "     vec2 widthHeightStep = vec2(texelWidth, texelHeight);\n"
        "     vec2 widthNegativeHeightStep = vec2(texelWidth, -texelHeight);\n"
        "     \n"
        "     vTextureCoord = aTexCoor.xy;\n"
        "     leftTextureCoordinate = aTexCoor.xy - widthStep;\n"
        "     rightTextureCoordinate = aTexCoor.xy + widthStep;\n"
        "     \n"
        "     topTextureCoordinate = aTexCoor.xy - heightStep;\n"
        "     topLeftTextureCoordinate = aTexCoor.xy - widthHeightStep;\n"
        "     topRightTextureCoordinate = aTexCoor.xy + widthNegativeHeightStep;\n"
        "     \n"
        "     bottomTextureCoordinate = aTexCoor.xy + heightStep;\n"
        "     bottomLeftTextureCoordinate = aTexCoor.xy - widthNegativeHeightStep;\n"
        "     bottomRightTextureCoordinate = aTexCoor.xy + widthHeightStep;\n"
        " }";

//const char *CircleDetectionFrag = "precision mediump float;\n"
//        "\n"
//        "varying vec2 vTextureCoord;\n"
//        "uniform sampler2D sTexture;\n"
//        "\n"
//        "void main()\n"
//        "{\n"
//        "    gl_FragColor=texture2D(sTexture,vTextureCoord);\n"
//        "}";

const char *CircleDetectionFrag = "precision mediump float;\n"
        "\n"
        " varying vec2 vTextureCoord;\n"
        " varying vec2 leftTextureCoordinate;\n"
        " varying vec2 rightTextureCoordinate;\n"
        " \n"
        " varying vec2 topTextureCoordinate;\n"
        " varying vec2 topLeftTextureCoordinate;\n"
        " varying vec2 topRightTextureCoordinate;\n"
        " \n"
        " varying vec2 bottomTextureCoordinate;\n"
        " varying vec2 bottomLeftTextureCoordinate;\n"
        " varying vec2 bottomRightTextureCoordinate;\n"
        "\n"
        " uniform sampler2D sTexture;\n"
//        " uniform float edgeStrength;\n"
        " \n"
        " void main()\n"
        " {\n"
        "    float bottomLeftIntensity = texture2D(sTexture, bottomLeftTextureCoordinate).r;\n"
        "    float topRightIntensity = texture2D(sTexture, topRightTextureCoordinate).r;\n"
        "    float topLeftIntensity = texture2D(sTexture, topLeftTextureCoordinate).r;\n"
        "    float bottomRightIntensity = texture2D(sTexture, bottomRightTextureCoordinate).r;\n"
        "    float leftIntensity = texture2D(sTexture, leftTextureCoordinate).r;\n"
        "    float rightIntensity = texture2D(sTexture, rightTextureCoordinate).r;\n"
        "    float bottomIntensity = texture2D(sTexture, bottomTextureCoordinate).r;\n"
        "    float topIntensity = texture2D(sTexture, topTextureCoordinate).r;\n"
        "    float h = -topLeftIntensity - 2.0 * topIntensity - topRightIntensity + bottomLeftIntensity + 2.0 * bottomIntensity + bottomRightIntensity;\n"
        "    float v = -bottomLeftIntensity - 2.0 * leftIntensity - topLeftIntensity + bottomRightIntensity + 2.0 * rightIntensity + topRightIntensity;\n"
        "    \n"
        "    float mag = length(vec2(h, v)) * 0.4;\n"
        "    \n"
        "    gl_FragColor = vec4(vec3(mag), 1.0);\n"
        " }";

const char *YUV_CircleDetectionFrag = "precision mediump float;\n"
        "\n"
        "varying vec2 vTextureCoord;\n"
        " uniform sampler2D s_texture_y;\n"
        " uniform sampler2D s_texture_cbcr;\n"
        "\n"
        "void main()\n"
        "{\n"
        "     highp float y = texture2D(s_texture_y, vTextureCoord).r;\n"
        "     highp float u = texture2D(s_texture_cbcr, vTextureCoord).a - 0.5;\n"
        "     highp float v = texture2D(s_texture_cbcr, vTextureCoord).r - 0.5;\n"
        "     \n"
        "     highp float r = y +             1.402 * v;\n"
        "     highp float g = y - 0.344 * u - 0.714 * v;\n"
        "     highp float b = y + 1.772 * u;\n"
        "     \n"
        "     gl_FragColor = vec4(r,g,b,1.0);     \n"
        "}";


CircleDetection::CircleDetection(float radius,float aspect,void *mgn) : m_radius(radius)
{
    _Manager = mgn;
    m_aspect = aspect;
    mIsSelected = false;
    ParametricInterval interval = { 1, vec2(1,1), vec2(2, 2) };
    SetInterval(interval);
#ifdef __USE_RGB32__
    setupProgram(CircleDetectionSeg,CircleDetectionFrag);
#else
    setupProgram(CircleDetectionSeg,YUV_CircleDetectionFrag,true);
#endif
    setupProjection();
    SetupBuffer();
    _Zoom = -2.74;
    mScreenMode = SCRN_CIRCLEDETECTION;
}

vec3 CircleDetection::Evaluate(const vec2& domain)
{
    float u = domain.x, v = domain.y;
    float x = (m_radius*u-m_radius/2)*m_aspect;
    float y = m_radius*v-m_radius/2;
    float z = 1;
    return vec3(x, y, z);
}

vec2 CircleDetection::EvaluateCoord(const vec2& domain)
{
    float u = domain.x, v = domain.y;
    float x = u;
    float y = 1-v;
    return vec2(x, y);
}


void CircleDetection::setupProgram(const char *segsrc, const char *fragsrc) {
    ParametricSurface::setupProgram(segsrc, fragsrc);
    texelWidthUniform = glGetUniformLocation(_programHandle,"texelWidth");
    texelHeightUniform = glGetUniformLocation(_programHandle,"texelHeight");
}

void CircleDetection::setupProgram(const char *segsrc, const char *fragsrc, bool yuv) {
    ParametricSurface::setupProgram(segsrc, fragsrc, yuv);
    texelWidthUniform = glGetUniformLocation(_programHandle,"texelWidth");
    texelHeightUniform = glGetUniformLocation(_programHandle,"texelHeight");
}

void CircleDetection::DrawSelf(int scnindex) {
    glUseProgram(_programHandle);
    glUniform1f(texelWidthUniform,texelWidth);
    glUniform1f(texelHeightUniform,texelHeight);
    BaseScreen::DrawSelf(scnindex);
}

bool CircleDetection::LoadTexture(int w, int h, GLenum type, GLvoid *pixels, int scnindex) {
    texelWidth = 1.0f/(float)w;
    texelHeight = 1.0f/(float)h;
    return ParametricSurface::LoadTexture(w, h, type, pixels, scnindex);
}

bool CircleDetection::LoadTexture(int w, int h, GLvoid *y_pixels, GLvoid *cbcr_pixels, int scnindex) {
    texelWidth = 1.0f/(float)w;
    texelHeight = 1.0f/(float)h;
    return ParametricSurface::LoadTexture(w, h, y_pixels, cbcr_pixels, scnindex);
}
