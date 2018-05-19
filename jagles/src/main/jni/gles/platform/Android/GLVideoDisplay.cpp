//
// Created by LiHong on 16/3/15，
// follow-up development by MiQi 
//

//#include <NormalScreen.h>
#include <jni.h>
//#include "com_app_jagles_video_GLVideoRender.h"
//#include "ParametricManager.h"
//#include "ParametricSurface.h"
//#include "JAPlayer.h"
//#include "mediabuffer.h"
//#include "GPUMotionTracking.h"
//#include "ConnectManager.h"

//#include "apportable_openal_funcs.h"
#include "../../GLES/ParametricManager.h"
#include "../../Media/JAPlayer.h"
#include "../../GLES/GPUMotionTracking.h"
#include "../../../3rdparty/OpenAL-soft-android/Alc/apportable_openal_funcs.h"
#include "../../GLES/NormalScreen.h"

#define LOG_TAG "JAVideo"
#define LOGV(...)__android_log_print(ANDROID_LOG_DEBUG,"Lee",__VA_ARGS__)
#define LOGD(...)__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define INFO(...) {__android_log_write(ANDROID_LOG_INFO,"videoconnect",__VA_ARGS__);}
#define LOGE(...) {__android_log_print(ANDROID_LOG_ERROR, "videoconnect", __VA_ARGS__);}
#define LOGV1(...)__android_log_print(ANDROID_LOG_DEBUG,"testsetcrop",__VA_ARGS__)
#define LOGV2(...)__android_log_print(ANDROID_LOG_DEBUG,"testinfo",__VA_ARGS__)
#define LOGV3(...)__android_log_print(ANDROID_LOG_DEBUG,"zoom",__VA_ARGS__)
#define LOGV4(...)__android_log_print(ANDROID_LOG_DEBUG,"honglee_20170823",__VA_ARGS__)
#define LOGV5(...)__android_log_print(ANDROID_LOG_DEBUG,"honglee_1110",__VA_ARGS__)

#define TRUE 1
#define FALSE 0

typedef int BOOL;

BOOL DEBUG = TRUE;

JavaVM *javaVM;
JavaVM *mJavaVM;
jobject mJavaObj;

extern jobject mJavaObjc;
extern JavaVM *mJavaVMc;

float limitX;

typedef struct parametric_manager {
    //JavaVM * mJavaVM;
    ParametricManager *_Manager;        //整个底层库的接口类出口
    JAPlayer *_Player;         //播放器类
    float startx;           //记录手势操作的开始的x坐标
    float starty;           //y坐标
    float lastx;            //记录上一次的x坐标
    float lasty;            //y坐标
    int starttime;        //记录手势的间隔时间
    int window_width, window_height;   //记录渲染opengl控件的大小
    float limit_left, limit_top, limit_right, limit_bottom;     //手势操作的限制
    float aspect;           //屏幕的宽高比

    bool isOnePlay;
    bool isForceWall;
    bool isForce;

    GPUMotionTracking *mTracking;
    bool mTracingIndex[5];
    int mMotionTracking[4];
    int mFramecount[4];
} JAMANAGER;

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/jni.h"

/**
 * java字符串转为c的字符串
 *
*/
char *jstringTostring(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    env->DeleteLocalRef(strencode);
    env->DeleteLocalRef(clsstring);
    return rtn;
}

/**
 * 同上
 *
*/
char *jstringTostring2(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("GBK");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);

    env->DeleteLocalRef(strencode);
    env->DeleteLocalRef(clsstring);
    return rtn;
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    mJavaVM = vm;
    mJavaVMc = vm;
    javaVM = vm;
    //  JAConnect_InitLib();
    av_jni_set_java_vm(vm, NULL);
    LOGE("加载jni层的库");
    if (apportableOpenALFuncs.alc_android_set_java_vm) {
        apportableOpenALFuncs.alc_android_set_java_vm(vm);
    }

    return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {

    LOGD("JNI_OnUnload");
}

/*
 * Method:  callback function
 */
void OnDrawRequest(ISurface *obj, void *ctx) {
    JAMANAGER *mgn = (JAMANAGER *) ctx;
}

/**
*
* 动画结束的回调
*/
void DoAniEnd(int msgid) {
    jclass javaClass;
    JNIEnv *env;
    jmethodID callBackid;
    mJavaVM->AttachCurrentThread(&env, NULL);
    javaClass = env->GetObjectClass(mJavaObj);
    if (javaClass != NULL) {
        callBackid = env->GetMethodID(javaClass, "OnAnimationEnd", "(I)V");
        env->CallVoidMethod(mJavaObj, callBackid, msgid);
    }
    env->DeleteLocalRef(javaClass);
    mJavaVM->DetachCurrentThread();
}

void OnAniEnd(ParametricManager *mgn, void *ani) {
    PJA_Animation lani = (PJA_Animation) ani;
    if ((int) lani->Ctx < 0)
        return;
    DoAniEnd((int) lani->Ctx);
}

int GetTrackingCanUse(JAMANAGER *mgn) {
    for (int i = 0; i < 5; i++)
        if (!mgn->mTracingIndex[i]) {
            mgn->mTracingIndex[i] = true;
            return i;
        }
    return -1;
}

int OnMotionTracking(float l, float t, float r, float b, float w, float h, int index, int scrnindex,
                     void *ctx, bool del) {
    JAMANAGER *mgn = (JAMANAGER *) ctx;
    if (!mgn || !mgn->_Manager || !mgn->_Manager->CurrentParametric()) {
        return -1;
    }
    if (del) {
        mgn->mTracingIndex[index] = false;
        return -1;
    }
    int lindex;
    if (index == -1) {
        lindex = GetTrackingCanUse(mgn);
        if (lindex < 0)
            return -1;
    } else
        lindex = index;
    bool canupdate = false;
    if (scrnindex == 0) {
        if (mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_HEMISPHERE && lindex == 0) {
            vec3 lpos = mgn->_Manager->CurrentParametric()->GetPosition(false, 0);
            //if (lscale.x < 1.12)
            //mgn->manager->CurrentParametric()->SetScale(vec3(1.12, 1.12, 1.12), false, 0);
            if (lpos.z < 3) {
                mgn->_Manager->CurrentParametric()->SetPosition(vec3(0, 0, 3), false, 0);
            }
            canupdate = true;
        }
        if (mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_FOUR && lindex >= 0 &&
            lindex < 4)
            canupdate = true;
        if (mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_THREE && lindex >= 0 &&
            lindex < 2)
            canupdate = true;
        if (mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_SIX && lindex >= 0 &&
            lindex < 5)
            canupdate = true;
        if (mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_FOUREX && lindex >= 0 &&
            lindex <= 1)
            canupdate = true;
        if (canupdate)
            mgn->_Manager->CurrentParametric()->SetScale(vec3(1, 1, 1), false, lindex);
    }
    if (canupdate) {
        mgn->_Manager->ClearAnimation(mgn->_Manager->CurrentParametric()->GetSelected(0));
        float mLeft = (float) (r / 2) / 50 * mgn->window_width;
        float mTop = (float) (50.0 - b) / 50 * mgn->window_height;
        float mWidth = mgn->window_width;
        float mHeight = mgn->window_height;
        float BoolValue = true;
        float BoolValue1 = true;
        float index = lindex;
        jclass javaClass;
        JNIEnv *env;
        jmethodID callBackid;
        mJavaVM->AttachCurrentThread(&env, NULL);
        javaClass = env->GetObjectClass(mJavaObj);
        if (javaClass != NULL) {
            callBackid = env->GetMethodID(javaClass, "DoUpdateFrame", "(FFFFIZZ)V");
            env->CallVoidMethod(mJavaObj, callBackid, mLeft, mWidth, mTop, mHeight, index,
                                BoolValue, BoolValue1);
        }
    } else {
        //env->DeleteLocalRef(javaClass);
        return -1;
    }
    //env->DeleteLocalRef(javaClass);
    return lindex;
}

void OnFilePlayProgress(int curtime, int duration, bool wallmode) {
    jclass javaClass;
    JNIEnv *env;
    jmethodID callBackid;
    mJavaVM->AttachCurrentThread(&env, NULL);
    javaClass = env->GetObjectClass(mJavaObj);
    if (javaClass != NULL) {
        callBackid = env->GetMethodID(javaClass, "OnPlayfileProgress", "(II)V");
        env->CallVoidMethod(mJavaObj, callBackid, curtime, duration);
    }
    mJavaVM->DetachCurrentThread();
}

void OnFileTextureAvaible(
        int w, int h, void *frame, int frame_len, int frame_type, int timestamp, int index,
        void *ctx) {
    jclass javaClass;
    JNIEnv *env = NULL;
    jmethodID callBackid;
    mJavaVMc->AttachCurrentThread(&env, NULL);
    if (!env)
        return;
    javaClass = env->GetObjectClass(mJavaObj);
    if (!frame_len)
        return;
    if (!frame)
        return;

//	LOGD("OnTextureAvaible----->%d", frame_len);
//    LOGD("-------------width:%d,height:%d",w,h);
    jbyteArray nativePixels = env->NewByteArray(frame_len);
    if (nativePixels == NULL) {
        return;
    }
    //if (!nativePixels)
    //return;

    env->SetByteArrayRegion(nativePixels, 0, frame_len, (jbyte *) frame);
    if (javaClass != NULL) {
        callBackid = env->GetMethodID(javaClass, "DoTextureAvaible", "(II[BIIIIJ)V");
        env->CallVoidMethod(mJavaObj, callBackid, w, h, nativePixels, frame_len, frame_type,
                            timestamp, index, (jlong) ctx);
    }

    env->DeleteLocalRef(javaClass);
    env->DeleteLocalRef(nativePixels);
    mJavaVMc->DetachCurrentThread();

}


/*
 * Class:     com_app_jagles_video_GLVideoRender
 * Method:    InitManager   初始化Manager
 * Signature: (FLjava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_app_jagles_video_GLVideoRender_InitManager
        (JNIEnv *env, jobject zthis, jfloat aspect, jint window_width, jint window_height,
         jstring fontpath) {
    //return 0;
    JAMANAGER *mgn = (JAMANAGER *) malloc(sizeof(JAMANAGER));
    mgn->_Manager = new ParametricManager(aspect, (void *) mgn, jstringTostring(env, fontpath));
    mgn->_Manager->OnDrawRequest = OnDrawRequest;
    mgn->_Manager->InitBorderTexture(0xffff00ff, 0xffffffff);
    mgn->_Manager->OnTextureAvaible = OnFileTextureAvaible;
    mgn->_Player = new JAPlayer((void *) mgn->_Manager);
    LOGD("fontpath:%s %f", jstringTostring(env, fontpath), aspect);
    printf("error open file ");
    mgn->window_width = window_width;
    mgn->window_height = window_height;
    mgn->aspect = aspect;
    mgn->isForceWall = false;
    mgn->isForce = false;
    mJavaObj = env->NewGlobalRef(zthis);
    mgn->_Manager->SetTextureWidthHeight(512, 512);
    LOGE("InitManager native ....................%lu", mgn);

    mgn->mTracking = new GPUMotionTracking(true);
    mgn->mTracking->OnMotionTracking = OnMotionTracking;
    mgn->mTracking->mCtx = (void *) mgn;
    mgn->mTracking->InitFBO();
    for (int i = 0; i < 5; i++) {
        mgn->mTracingIndex[i] = false;
    }
    for (int i = 0; i < 4; i++) {
        mgn->mMotionTracking[i] = 0;
    }
    return (jlong) mgn;
}

/*
 * Class:     com_app_jagles_video_GLVideoRender
 * Method:    SetMode  设置当前的显示模式
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_SetMode
        (JNIEnv *env, jobject zthis, jlong handle, jint mode) {
    LOGV("Java_com_app_jagles_video_GLVideoRender_SetMode  mode = %d", mode);
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    if (mode < 0)
        return;
    mgn->_Manager->SetOutsideChanged(true);
    mgn->_Manager->ClearAnimation();
    mgn->_Manager->SwitchMode(mode);
}

/*
 * Class:     com_app_jagles_video_GLVideoRender
 * Method:    DrawRequest
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_DrawRequest
        (JNIEnv *, jobject) {

}

/*
 * Class:     com_app_jagles_video_GLVideoRender
 * Method:    StartAnimation
 * Signature: (J[FIIIZIZI)I
 */
JNIEXPORT jlong JNICALL Java_com_app_jagles_video_GLVideoRender_StartAnimation
        (JNIEnv *env, jobject zthis, jlong handle, jfloatArray pend, jint step, jint duration,
         jboolean isloop, jint type, jboolean texture, jint srcIndex, jboolean Inertia,
         jint endmsg) {
    LOGV("Java_com_app_jagles_video_GLVideoRender_StartAnimation");
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return 0;
    }
    vec3 end;
    jfloat *v;
    v = env->GetFloatArrayElements(pend, NULL);
    end.x = v[0];
    end.y = v[1];
    end.z = v[2];
    LOGD("...GLVideoRender_StartAnimation.........%f,%f,%f.............", end.x, end.y, end.z);
    env->ReleaseFloatArrayElements(pend, v, 0);
    return mgn->_Manager->StartAnimation(mgn->_Manager->CurrentParametric(), end, step, duration,
                                         isloop, type, texture, srcIndex, Inertia, OnAniEnd,
                                         (void *) endmsg);
}

/*
 * Class:     com_app_jagles_video_GLVideoRender
 * Method:    StopAnimation
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_StopAnimation
        (JNIEnv *env, jobject zthis, jlong handle, jlong ani) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    mgn->_Manager->StopAnimation(ani);
}

/*
 * Class:     com_app_jagles_video_GLVideoRender
 * Method:    TextureAvaible
 * Signature: (JJIIII)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_TextureAvaible
        (JNIEnv *env, jobject zthis, jlong handle, jint w, jint h, jbyteArray texture,
         jint frame_len, jint frame_type, jint timestamp, jint index, jlong ctx) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    JAConnect *con = (JAConnect *) ctx;
    if (!mgn || !mgn->_Manager || !mgn->_Manager->CurrentParametric()) {
        return;
    }
    void *data = (void *) env->GetByteArrayElements(texture, 0);

    uint32_t ltime = clock_ms();
    if (index > 0 && mgn->_Manager->CurrentParametric()->GetScreenMode() != SCRN_NORMAL)
        return;
    mgn->_Manager->SetTextureWidthHeight(w, h);
//    LOGE("w:%d,h:%d................",w,h);
    if (mgn->_Manager->CurrentParametric()->GetScreenMode() != SCRN_NORMAL) {
        for (int i = 0; i < 1; i++) {
            if (mgn->mMotionTracking[index]) {
                if (!(mgn->mFramecount[i] % 3)) {
                    mgn->mTracking->LoadTexture(w, h, GL_RGBA, (void *) data, index);
                    mgn->mTracking->RenderFBO();
                }
            }
            mgn->mFramecount[i]++;
        }
    }
    if (mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_SIX ||
        mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_THREE ||
        mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_FOUREX) {
        mgn->_Manager->CurrentParametric()->UpdateFrame(0, 0, 0, 0, 0, false, false);
    }
//    if(mgn->_Manager->CurrentParametric()->GetScreenMode()==SCRN_NORMAL|| mgn->_Manager->GetDualMode()){
//        con->GetDecoder()->EnableCrop(false);
//        mgn->_Manager->LoadTexture(w,h,GL_RGBA,(void *)data,index);
//        //LOGV("-----------------------normal LoadTexture %d %d",w,h);
//    }
//    else{
//        if (!con->GetDecoder()->GetCrop())
//        {
//            con->GetDecoder()->EnableCrop(true);
//            env->ReleaseByteArrayElements(texture, (jbyte*)data, 0);
//            return;
//        }


    //  mgn->_Manager->CurrentParametric()->ShowOSD(true);
    mgn->_Manager->LoadTexture(w, h, GL_RGBA, (void *) data, index);
    //LOGV("-----------------------normal LoadTexture %d %d",w,h);
//    }
    //   LOGE("load texture:%d",clock_ms()-ltime);
    env->ReleaseByteArrayElements(texture, (jbyte *) data, 0);

    if (!con)
        return;
    if (w > 0 && h > 0 && !con->GetHasSetP720() && con->GetDecoder()->getIsP720Dev()) {

        LOGV("GLVideoDisPlay::if (w > 0 && h > 0 && !con->GetHasSetP720())");

        float centerX = 0, centerY = 0, radius = 0, angleX = 0, angleY = 0, angleZ = 0;

        con->GetFishEyeParameter(&centerX, &centerY, &radius, &angleX, &angleY, &angleZ, 0);
        mgn->_Manager->SetFishEyeParameters(centerX, centerY, radius, angleX, angleY, angleZ, 0,
                                            true);

        con->GetFishEyeParameter(&centerX, &centerY, &radius, &angleX, &angleY, &angleZ, 1);
        mgn->_Manager->SetFishEyeParameters(centerX, centerY, radius, angleX, angleY, angleZ, 1,
                                            true);

        int lDist_len = 0;
        void *lDist = con->GetDistTortion(&lDist_len);
        mgn->_Manager->SetDistortion(lDist, lDist_len);

        con->SetHasSetP720(true);

        mgn->_Manager->SetStitch2TextureAspect((float) w / (float) h);
    } else if (w > 0 && h > 0 && !con->GetHasSetP360() && con->GetDecoder()->getIsP360Dev()) {
        float centerX = 0, centerY = 0, radius = 0;
        con->GetFishEyeParameter360(&centerX, &centerY, &radius);
        LOGV("(w > 0 && h > 0 && !con->GetHasSetP360() && con->GetDecoder()->getIsP360Dev())  CenterX---%f,CenterY:%f,Radius:%f width:%d,height:%d",
             centerX, centerY, radius, w, h);
//        if(radius>((float)h/2.0))
//            mgn->_Manager->SetFishEyeParameters(centerX/((float)h/2)-(float)w/(float)h,1.0-centerY/((float)h/2),radius/(float)h,0,0,0,0,false);
//        else
        if (centerY - radius < 0) {
            //radius = centerY;
        }
        if (centerY + radius > h) {
            //radius = h - centerY;
            //centerY = h/2;//不矫正半径，改矫正圆心
        }
        if (radius <= 0) {
            centerX = w / 2;
            centerY = h / 2;
            radius = centerY;
        }
        mgn->_Manager->SetFishEyeParameters(centerX / (float) w - 0.5,
                                            centerY / (float) h - 0.5,
                                            radius / (float) (w / 2.0), 0, 0, 0, 0, false);
        con->SetHasSetP360(true);
    }
}

JNIEXPORT void JNICALL Java_com_juanvision_video_GLVideoRender_OSDTextureAvaible2
        (JNIEnv *env, jobject zthis, jlong handle, jstring texture) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager || !mgn->_Manager->CurrentParametric()) {
        if (DEBUG) {
            LOGE("OSD faile, the case is the handle is invaile");
        }
        return;
    }
    mgn->_Manager->CurrentParametric()->LoadOSDTexture(480, 24, GL_RGBA,
                                                       (void *) jstringTostring(env, texture));
}


/*
 * Class:     com_app_jagles_video_GLVideoRender
 * Method:    OSDTextureAvaible  左上角时间绘制
 * Signature: (JJIIII)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_OSDTextureAvaible
        (JNIEnv *env, jobject zthis, jlong handle, jlong texture, jint lenght, jint width,
         jint height) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager || !mgn->_Manager->CurrentParametric()) {
        if (DEBUG) {
            LOGE("OSD faile, the case is the handle is invaile");
        }
        return;
    }

    //LOGV("Java_com_app_jagles_video_GLVideoRender_OSDTextureAvaible mgn->_Manager->GetMode() = %d  width = %d  height = %d", mgn->_Manager->GetMode(),width,height);
    /////////修改方法//////////////
//	if (mgn->_Manager->GetDualMode()) {
//
//		unsigned char*txttex = mgn->_Manager->_Textput->GenTextbuffer((char *)texture, 0xff000000, 480, 24);
//		mgn->_Manager->CurrentParametric()->LoadOSDTexture(480, 24, GL_RGBA, (void*)txttex);
//		free(txttex);
//	}
//	else {
//		mgn->_Manager->CurrentParametric()->LoadOSDTexture(width, height, GL_RGBA, (void*)texture);
//	}
    mgn->_Manager->CurrentParametric()->LoadOSDTexture(width, height, GL_RGBA, (void *) texture);

}

/*
 * Class:     com_app_jagles_video_GLVideoRender
 * Method:    DestroyManager
 * Signature: ()V
 */
JNIEXPORT jboolean JNICALL Java_com_app_jagles_video_GLVideoRender_DestroyManager
        (JNIEnv *env, jobject zthis, jlong handle) {
    LOGV("GLVideoDisplay::Java_com_app_jagles_video_GLVideoRender_DestroyManager");
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn)
        return false;
    LOGD("destroyManager ....................................");
    delete mgn->_Manager;
    delete mgn->mTracking;
    free(mgn);
    if (apportableOpenALFuncs.alc_android_set_java_vm) {
        apportableOpenALFuncs.alc_android_set_java_vm(NULL);
    }
    return true;
}

/*
 * Class:     com_app_jagles_video_GLVideoRender
 * Method:    DrawParametric
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_DrawParametric
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager || !mgn->_Manager->CurrentParametric()) {
        //LOGE("Can not DrawSelf , the JAMANAGER not init");
        return;
    }

    if (mgn->_Manager->GetDualMode() &&
        (mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_CYLINDER ||
         mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_EXPAND ||
         mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_UPDOWN ||
         mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_THREE ||
         mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_FOUREX)) {
        mgn->_Manager->RenderFBO();
//        glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
    }

    if (mgn->_Manager->GetDualMode()) {
        //LOGD("why...............");
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        mgn->_Manager->CurrentParametric()->DrawSelf(0);
        glCullFace(GL_BACK);
        mgn->_Manager->CurrentParametric()->DrawSelf(1);
    } else {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        mgn->_Manager->CurrentParametric()->DrawSelf(0);
        glCullFace(GL_FRONT);
        mgn->_Manager->CurrentParametric()->DrawSelf(0);

        if (!mgn || !mgn->_Manager || !mgn->_Manager->CurrentParametric()) {
            //LOGE("Can not DrawSelf , the JAMANAGER not init");
            return;
        }
        if (mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_CYLINDER) {
            mgn->_Manager->CurrentParametric()->DrawSelf(1);
        }

    }
    glDisable(GL_CULL_FACE);
    mgn->_Manager->RenderCircle();
    mgn->_Manager->RenderLogo();
}

JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_TransformObject
        (JNIEnv *env, jobject zthis, jlong handle, jfloatArray pos, jint type, jboolean texture,
         jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    jfloat *v;
    if (!mgn || !mgn->_Manager || !mgn->_Manager->CurrentParametric())
        return;
    v = env->GetFloatArrayElements(pos, NULL);
    vec3 end;
    end.x = v[0];
    end.y = v[1];
    end.z = v[2];
    switch (type) {
        case 0:
            mgn->_Manager->CurrentParametric()->SetPosition(end, texture, index);
            break;
        case 1:
            mgn->_Manager->CurrentParametric()->SetScale(end, texture, index);
            break;
        case 2:
            mgn->_Manager->CurrentParametric()->setRotate(end, texture, index);
            break;
    }
    env->ReleaseFloatArrayElements(pos, v, 0);
}

/**
* 播放文件
*/
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_Playfile
        (JNIEnv *env, jobject zthis, jlong handle, jstring filename, jboolean isimage,
         jboolean isfisheye) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Player) {
        return;
    }
    mgn->_Player->PlayFile(jstringTostring(env, filename), isimage, isfisheye, 0,
                           OnFilePlayProgress);
}

/**
* 停止播放文件
*/
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_StopPlay
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Player) {
        return;
    }
    mgn->_Player->StopPlay();
}

/**
* 获取贴图或者模型的位置
*/
JNIEXPORT jfloatArray JNICALL Java_com_app_jagles_video_GLVideoRender_GetObjectPosition
        (JNIEnv *env, jobject zthis, jlong handle, jint type, jboolean texture, jint index) {
    jfloat rt[3];
    jfloatArray rta = env->NewFloatArray(3);
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager || !mgn->_Manager->CurrentParametric()) {
        rt[0] = 0;
        rt[1] = 0;
        rt[2] = 0;
        env->SetFloatArrayRegion(rta, 0, 3, rt);//把rt里的值从0到3赋给rta
        LOGE("this mgn is null...............%ld", mgn);
        return rta;
    }
    vec3 pos;
    switch (type) {
        case 0:
            pos = mgn->_Manager->CurrentParametric()->GetPosition(texture, index);
            break;
        case 1:
            pos = mgn->_Manager->CurrentParametric()->GetScale(texture, index);
            break;
        case 2:
            pos = mgn->_Manager->CurrentParametric()->GetRotate(texture, index);
            break;
    }
    rt[0] = pos.x;
    rt[1] = pos.y;
    rt[2] = pos.z;
    env->SetFloatArrayRegion(rta, 0, 3, rt);
    return rta;
}

/**
*
* 正常模式下的分割
*/
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_SetSplit
        (JNIEnv *env, jobject zthis, jlong handle, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    mgn->_Manager->SetSplit(index, false);
}

/**
* 正常模式下设置跳转的页面
*/
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_SetScreenPage
        (JNIEnv *env, jobject zthis, jlong handle, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn)
        return;
    mgn->_Manager->SetPage(index, false);
}

/**
* 正常模式下选择某个正在播放的windows
*/
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_SetSelected
        (JNIEnv *env, jobject zthis, jlong handle, jint x, jint y, jint w, jint h) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    mgn->_Manager->SetSelected(x, y, w, h);
}

/**
* 正常模式下通过下标设置选中某个正在播放的windows
*/
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_SetSelectedByIndex
        (JNIEnv *env, jobject zthis, jlong handle, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("this handle is invaile");
        }
        return;
    }
    mgn->_Manager->SetSelected(index);
}

/**
* 获取当前页面的下标
*/
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoRender_GetPageIndex
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("this handle is invaile");
        }
        return -1;
    }
    return mgn->_Manager->GetPage();
}

/**
*
*/
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoRender_GetScreenCount
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("this handle is invaile");
        }
        return 0;
    }
    return mgn->_Manager->GetScreenCount();
}

/**
* 获取总共有多少个页面数
*/
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoRender_GetAllPage
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("this handle is invaile");
        }
        return 0;
    }
    return mgn->_Manager->GetAllPage();
}

/**
* 设置共有多少个页面数
*/
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_SetAllPage
        (JNIEnv *env, jobject zthis, jlong handle, jint allPage) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("this handle is invaile");
        }
        return;
    }
    mgn->_Manager->SetAllPage(allPage);
}

/**
* 获取当前选中的windows的下标
*/
JNIEXPORT int JNICALL Java_com_app_jagles_video_GLVideoRender_GetVideoIndex
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("GetVideoIndex faile, the case is the handle is invaile");
        }
        return -1;
    }
    return mgn->_Manager->getVideoIndex();
}


/**
* 获取当前切换的model
*/
JNIEXPORT int JNICALL Java_com_app_jagles_video_GLVideoRender_GetMode
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager || !mgn->_Manager->CurrentParametric()) {
        if (DEBUG) {
            LOGE("GetMode faile, the case is the handle is invaile");
        }
        return -1;
    }
    return mgn->_Manager->CurrentParametric()->GetScreenMode();
}

/**
* 切割为一个屏幕
*/
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_SetSingVideo
        (JNIEnv *env, jobject zthis, jlong handle, jint index, jboolean ani) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("SetSingVideo faile, the case is the handle is invaile");
        }
        return;
    }
    mgn->_Manager->SetSingVideo(index, ani);
}

/**
* 圆筒的展开
*/
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_CylinderUnwind
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("CylinderUnwind faile, the case is the handle is invaile");
        }
        return;
    }
    mgn->_Manager->StartAnimation(mgn->_Manager->CurrentParametric(), vec3(6, 0, 0), 30, 500, 0,
                                  JA_ANI_TYPE_SCALE, true, 0, true, NULL, NULL);
    mgn->_Manager->StartAnimation(mgn->_Manager->CurrentParametric(), vec3(0, 0, 0), 30, 500, 0,
                                  JA_ANI_TYPE_ROTATE, false, 0, true, NULL, NULL);
    mgn->_Manager->StartAnimation(mgn->_Manager->CurrentParametric(), vec3(1.5, 1.5, 1.5), 30, 500,
                                  0, JA_ANI_TYPE_SCALE, false, 0, true, NULL, NULL);
}

JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_CylinderWind
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("CylinderUnwind faile, the case is the handle is invaile");
        }
        return;
    }
    mgn->_Manager->StartAnimation(mgn->_Manager->CurrentParametric(), vec3(1, 0, 0), 30, 500, 0,
                                  JA_ANI_TYPE_SCALE, true, 0, true, NULL, NULL);
    mgn->_Manager->StartAnimation(mgn->_Manager->CurrentParametric(), vec3(-30, 0, 0), 30, 500, 0,
                                  JA_ANI_TYPE_ROTATE, false, 0, true, NULL, NULL);
    mgn->_Manager->StartAnimation(mgn->_Manager->CurrentParametric(), vec3(1, 1, 1), 30, 500, 0,
                                  JA_ANI_TYPE_SCALE, false, 0, true, NULL, NULL);
}

/**
* vr模式下的根据陀螺仪进行变换的方法
*/
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_VRSensor
        (JNIEnv *env, jobject zthis, jlong handle, jfloatArray rate_j, jfloatArray gra_j,
         jint or_type, jint mode) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager || !mgn->_Manager->CurrentParametric()) {
        if (DEBUG) {
            LOGE("VRSensor faile, the case is the handle is invaile");
        }
        return;
    }
    double rate_x, rate_y, rate_z;
    double gra_x, gra_y, gra_z, graX_main, graY_main;

    float *rate = env->GetFloatArrayElements(rate_j, NULL);
    float *gra = env->GetFloatArrayElements(gra_j, NULL);

    rate_x = rate[0];
    rate_y = rate[1];
    rate_z = rate[2];

    gra_x = gra[0];
    gra_y = gra[1];
    gra_z = gra[2];

    graX_main = -gra_z;//new
    graY_main = -gra_x;//new

    if (or_type == 2) {
        double tmp = rate_x;
        rate_x = -rate_y;
        rate_y = tmp;
        //   gra_main = gra_x;
        //   graY_main = gra_x>0?gra_y:-gra_y;//new
        graY_main = gra_y;//new
    }
    //
    double tempZ = sqrt(rate_y * rate_y + rate_z * rate_z) * (rate_y > 0 ? 1 : -1);
    vec3 lrotate = mgn->_Manager->CurrentParametric()->GetRotate(false, 0);

    if (mode == 7) {
        lrotate.x = 9.0 * graX_main;//new
        lrotate.y += tempZ;
        lrotate.z = -9.0 * graY_main;
    } else {
        lrotate.x = 9.0 * graX_main + 90.0;//new
        lrotate.z += tempZ;
        lrotate.y = 9.0 * graY_main;
//        lrotate.x = -9.0 * gra_z;
//        lrotate.y += rate_x;
//        lrotate.z = -9.0 * gra_y;
    }

    mgn->_Manager->CurrentParametric()->setRotate(lrotate, false, 0);
    mgn->_Manager->CurrentParametric()->setRotate(lrotate, false, 1);

    env->ReleaseFloatArrayElements(rate_j, rate, 0);
    env->ReleaseFloatArrayElements(gra_j, gra, 0);
}

/**
* 获取正常模式下切割了多少个分屏的模式
*/
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoRender_GetSplitMode
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("GetSplitMode faile, the case is the handle is invaile");
        }
        return -1;
    }
    return mgn->_Manager->GetSplitMode();
}

/**
* 清理动画
*/
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_ClearAnimation
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("ClearAnimation faile, the case is the handle is invaile");
        }
        return;
    }
    mgn->_Manager->ClearAnimation();
}

/**
* 获取贴图或者模型放大的倍数
*/
JNIEXPORT jfloatArray JNICALL Java_com_app_jagles_video_GLVideoRender_GetScale
        (JNIEnv *env, jobject zthis, jlong handle, jboolean texture, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager || !mgn->_Manager->CurrentParametric()) {
        if (DEBUG) {
            LOGE("GetScale faile, the case is the handle is invaile");
        }
    }
    vec3 scale = mgn->_Manager->CurrentParametric()->GetScale(texture, index);
    jfloat rt[3];
    jfloatArray rta = env->NewFloatArray(3);
    rt[0] = scale.x;
    rt[1] = scale.y;
    rt[2] = scale.z;
    env->SetFloatArrayRegion(rta, 0, 3, rt);
    return rta;
}

/**
* 重置位置坐标
*/
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_ResetPosition
        (JNIEnv *env, jobject zthis, jlong handle, jboolean texture, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("ResetPositon faile ,the case is the handle is invaile");
        }
        return;
    }
    mgn->_Manager->CurrentParametric()->ResetPosition();
}

/**
* 在正常模式下获取已分割的屏幕的显示状态
*/
JNIEXPORT jboolean JNICALL Java_com_app_jagles_video_GLVideoRender_GetVisibility
        (JNIEnv *env, jobject zthis, jlong handle, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("GetVisibility faile, the case is the handle is invaile");
        }
        return false;
    }
    return mgn->_Manager->GetVisibility(index);
}

//状态文字的提示
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_DoStatus
        (JNIEnv *env, jobject zthis, jlong handle, jstring status_code, jint content_code,
         jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("DoStatus faile, the case is the handle is invaile");
        }
        return;
    }
    mgn->_Manager->PutInfo(jstringTostring2(env, status_code), index);
}

/**
* 屏幕大小变化后，重新计算设置所有模型和贴图的数据
*/
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_ReSizeSplite
        (JNIEnv *env, jobject zthis, jlong handle, jfloat aspect, jint width, jint height) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("ReSizeSplite faile, the case is the handle is invaile");
        }
        return;
    }
    mgn->window_width = width;
    mgn->window_height = height;
    mgn->_Manager->ReSizeSplite(aspect);
}

//加载录像标志的贴图(原因未知)
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_LoadRecordStatuTexture
        (JNIEnv *env, jobject zthis, jlong handle, jint textid) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn) {
        if (DEBUG) {
            LOGE("LoadRecordStateTexture faile, the case is the handle is invaile");
        }
        return;
    }
    //   mgn->_Manager->LoadRecordStatuTexture(textid);
}

//更新宽高比
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_UpdateAspect
        (JNIEnv *env, jobject zthis, jlong handle, jfloat aspect) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("UpdateAspect faile, the case is the handle is invaile");
        }
        return;
    }
    LOGE("ASPECT %f", aspect);
    mgn->_Manager->UpdateAspect(aspect);
}

//*********************新手势开始******************************
/**
 * 设置屏幕的宽高
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_SetWindowWidthHeight
        (JNIEnv *env, jobject zthis, jlong handle, jint width, jint height) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("SetWindowWidthHeight faile,the case is the handle invaile");
        }
        return;
    }
    mgn->_Manager->SetWindowWidthHeight(width, height);
}

/**
 * 手势按下
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_DoTapOrMouseDown
        (JNIEnv *env, jobject zthis, jlong handle, jint x, jint y) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("DoTapOrMouseDown faile, the case is the handle invaile");
        }
        return;
    }
    mgn->_Manager->DoTapOrMouseDown(x, y);
}

/**
 * 手势移动
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_DoTapOrMouseMove
        (JNIEnv *env, jobject zthis, jlong handle, jint x, jint y, jint wallmode, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("DoTapOrMouseMove faile, the case is the handle invaile");
        }
        return;
    }
    mgn->_Manager->DoTapOrMouseMove(x, y, wallmode, index);
}

/**
 * 手势放松
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_DoTapOrMouseUp
        (JNIEnv *env, jobject zthis, jlong handle, jint x, jint y, jint wallmode) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("DoTapOrMouseUp faile, the case is the handle invaile");
        }
        return;
    }
    mgn->_Manager->DoTapOrMouseUp(x, y, wallmode);
}

/**
 * 放大手势
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_DoTapOrMouseWheel
        (JNIEnv *env, jobject zthis, jlong handle, jint intra, jint x, jint y, jint index,
         jint wallMode) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("DoTapOrMouseWheel faile, the case is the handle is invaile");
        }
        return;
    }
    mgn->_Manager->DoTapOrMouseWheel(intra, x, y, index, wallMode);
}

/**
 * 双击手势
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_DoDoubleTap
        (JNIEnv *env, jobject zthis, jlong handle, jint wallmode, jint scene,
         jint scrnFourIndex, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("DoDoubleTap faile, the case is the handle is invaile");
        }
        return;
    }

    mgn->_Manager->DoDoubleTap(wallmode, scene, scrnFourIndex, index);
}

//半圆收回
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_HemisphereUnwind
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager || !mgn->_Manager->CurrentParametric()) {
        if (DEBUG) {
            LOGE("DoDoubleTap faile, the case is the handle is invaile");
        }
        return;
    }
    vec3 position = mgn->_Manager->CurrentParametric()->GetPosition(false, 0);
    vec3 rotate = mgn->_Manager->CurrentParametric()->GetRotate(false, 0);
    vec3 initvec = vec3(0, 0, 0);
    if (position == initvec) {
        mgn->_Manager->StartAnimation(mgn->_Manager->CurrentParametric(), vec3(0, 0, 3), 100, 500,
                                      0, JA_ANI_TYPE_POSITION, false, 0, true, NULL, NULL);
        mgn->_Manager->StartAnimation(mgn->_Manager->CurrentParametric(),
                                      vec3(45, 0, rotate.z + 90), 100, 500, 0, JA_ANI_TYPE_ROTATE,
                                      false, 0, true, NULL, NULL);
    }
}

//半圆展开
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_HemisphereWind
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager || !mgn->_Manager->CurrentParametric()) {
        if (DEBUG) {
            LOGE("DoDoubleTap faile, the case is the handle is invaile");
        }
        return;
    }
    vec3 position = mgn->_Manager->CurrentParametric()->GetPosition(false, 0);
    vec3 rotate = mgn->_Manager->CurrentParametric()->GetRotate(false, 0);
    vec3 initvec = vec3(0, 0, 0);
    if (position == initvec) {
        return;
    } else {
        mgn->_Manager->StartAnimation(mgn->_Manager->CurrentParametric(), vec3(0, 0, 0), 30, 250, 0,
                                      JA_ANI_TYPE_POSITION, false, 0, true, NULL, NULL);
        mgn->_Manager->StartAnimation(mgn->_Manager->CurrentParametric(), vec3(0, 0, rotate.z - 90),
                                      30, 250, 0, JA_ANI_TYPE_ROTATE, false, 0, true, NULL, NULL);
        mgn->_Manager->CurrentParametric()->SetScale(vec3(1, 1, 1), false, 0);
    }
}

//********************************手势结束***************************
//设置模型或贴图的位置，缩放倍数，旋转角度
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_TransformVertex
        (JNIEnv *env, jobject zthis, jlong handle, jint mode, jfloatArray vect, jboolean texture,
         jint type, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("TransformVertex faile, the case is the handle is invaile");
        }
        return;
    }
    float *tempVect = env->GetFloatArrayElements(vect, NULL);
    vec3 vectt;
    vectt.x = tempVect[0];
    vectt.y = tempVect[1];
    vectt.z = tempVect[2];
    mgn->_Manager->TransformVertex(mode, vectt, texture, type, index);

    env->ReleaseFloatArrayElements(vect, tempVect, 0);
}

//是否开启音频
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_EnableAudio
        (JNIEnv *env, jobject zthis, jlong handle, jboolean enable) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("enable audio faile, the case is the handel is invaile");
        }
        return;
    }
    mgn->_Manager->EnableAudio(enable);
}

//强制设置壁挂，或者倒挂
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_IsForceWallMode
        (JNIEnv *env, jobject zthis, jlong handle, jboolean isWallMode) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn) {
        if (DEBUG) {
            LOGE("IsForceWallMode faile, the case is the handel is invaile");
        }
        return;
    }
    mgn->isForceWall = isWallMode;
    mgn->isForce = true;
}

//设置是否显示分屏
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_SetVisible
        (JNIEnv *env, jobject zthis, jlong handle, jboolean value, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("SetVisible faile, the case is the handle is invaile");
        }
        return;
    }
    mgn->_Manager->SetVisibility(value, index);
}

/**
 * 调整手势的惯性
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_AdjustActionExperience
        (JNIEnv *env, jobject zthis, jlong handle, jint type, jint actionType, jfloat value) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("AdjustActionExperience faile, the case is the handle is invaile");
        }
        return;
    }
    mgn->_Manager->AdjustActionExperience(type, actionType, value);
}

/**
 * 开启线条的画图
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_EnableGrid
        (JNIEnv *env, jobject zthis, jlong handle, jboolean value) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        if (DEBUG) {
            LOGE("EnableGrid faile, the case is the handle is invaile");
        }
        return;
    }
    mgn->_Manager->EnableGrid(value);
}

/**
 * 设置移动跟踪的参数
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_SetupMotionTrackingOptions
        (JNIEnv *env, jobject zthis, jlong handle, jint objsize, jint dropsecond) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn)
        return;
    if (mgn->mTracking) {
        mgn->mTracking->mDropSecond = dropsecond;
        mgn->mTracking->mObjectSize = objsize;
    }
}

/**
 *开始移动跟踪
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_StartMotionTracking
        (JNIEnv *env, jobject zthis, jlong handle, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn)
        return;
    // mgn->_Manager->mIsMotionTracking = true;
    AtomicSwap(1, &mgn->mMotionTracking[index], mgn->mMotionTracking[index]);
}

/**
 * 停止移动跟踪
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_StopMotionTracking
        (JNIEnv *env, jobject zthis, jlong handle, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn)
        return;
    // mgn->_Manager->mIsMotionTracking = false;
    AtomicSwap(0, &mgn->mMotionTracking[index], mgn->mMotionTracking[index]);
}

/**
 * 更新frame
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_DoUpdateFrame
        (JNIEnv *env, jobject zthis, jlong handle, jfloat tbstart, jfloat tbend, jfloat lrstart,
         jfloat lrend, jint index, jboolean update, jboolean ani) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    mgn->_Manager->DoUpdateFrame(tbstart, tbend, lrstart, lrend, index, update, ani);

}

/**
 * 硬件加速的贴图更新
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_DoDirectTextureFrameUpdata
        (JNIEnv *env, jobject zthis, jlong handle, jlong conn, jint w, jint h, jlong directBuffer,
         jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager || !conn) {
        return;
    }
    mgn->_Manager->DoDirectTextureFrameUpdata((ConnectManager *) conn, w, h, (void *) directBuffer,
                                              index);
}

/**
 * OSD的贴图更新
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_DoDirectTextureOSDFrameUpdata
        (JNIEnv *env, jobject zthis, jlong handle, jlong conn, jint w, jint h, jlong textBuffer,
         jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    mgn->_Manager->DoDirectTextureOSDFrameUpdata((ConnectManager *) conn, w, h, (void *) textBuffer,
                                                 index);
}

/**
 * 播放音频数据
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_PlayAudioData
        (JNIEnv *env, jobject zthis, jlong handle, jbyteArray audioData, jint length) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    void *data = (void *) env->GetByteArrayElements(audioData, 0);
    mgn->_Manager->PlayAudioData((uint8_t *) data, length);
    env->ReleaseByteArrayElements(audioData, (jbyte *) data, 0);
}

/**
 * 打开音频播放
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_OpenAudioPlaying
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn)
        return;
    mgn->_Manager->OpenAudioPlaying();
}

/**
 * 暂停音频的播放
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_PauseAudioPlaying
        (JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    mgn->_Manager->PauseAudioPlaying();
}

/**
 * 显示loading的图标动画
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_ShowVideoLoading
        (JNIEnv *env, jobject zthis, jlong handle, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    mgn->_Manager->ConnectVideoLoading(index);
}

/**
 * 隐藏loading的图标动画
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_HideVideoLoading
        (JNIEnv *env, jobject zthis, jlong handle, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    mgn->_Manager->CloseVideoLoading(index);
}

JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_SetBorderColor
        (JNIEnv *env, jobject zthis, jlong handle, jint value) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    mgn->_Manager->InitBorderTexture(value, 0xffffffff);
}

JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_SetViewAngle
        (JNIEnv *env, jobject zthis, jlong handle, jfloat value) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn)
        return;
    mgn->_Manager->SetViewAngle(value);
}

JNIEXPORT jboolean JNICALL
Java_com_app_jagles_video_GLVideoRender_GetDualMode(JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return false;
    }
    return mgn->_Manager->GetDualMode();
}

JNIEXPORT jboolean JNICALL
Java_com_app_jagles_video_GLVideoRender_GetDualTexture(JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return false;
    }
    return mgn->_Manager->FBOTexture();
}

JNIEXPORT void JNICALL
Java_com_app_jagles_video_GLVideoRender_CleanTexture(JNIEnv *env, jobject instance, jlong handle) {
    // TODO
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager ||  !mgn->_Manager->CurrentParametric()) {
        return;
    }
    mgn->_Manager->CurrentParametric()->CleanTexture(0);
}

JNIEXPORT void JNICALL
Java_com_app_jagles_video_GLVideoRender_SetHardwareTexture(JNIEnv *env, jobject zthis, jlong handle,
                                                           jint texture, jint w, jint h,
                                                           jlong conn) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager ||  !mgn->_Manager->CurrentParametric()) {
        return;
    }
    //mgn->_Manager->CurrentParametric()->SwitchDecoder(true, 0);
    mgn->_Manager->CurrentParametric()->SetTexture(texture, 0);
    mgn->_Manager->SetTextureWidthHeight(w, h);
    JAConnect *con = (JAConnect *) conn;
    if (!con)
        return;
    if (w > 0 && h > 0 && !con->GetHasSetP720() && con->GetDecoder()->getIsP720Dev()) {

        LOGV("GLVideoDisPlay::if (w > 0 && h > 0 && !con->GetHasSetP720())");

        float centerX = 0, centerY = 0, radius = 0, angleX = 0, angleY = 0, angleZ = 0;

        con->GetFishEyeParameter(&centerX, &centerY, &radius, &angleX, &angleY, &angleZ, 0);
        mgn->_Manager->SetFishEyeParameters(centerX, centerY, radius, angleX, angleY, angleZ, 0,
                                            true);

        con->GetFishEyeParameter(&centerX, &centerY, &radius, &angleX, &angleY, &angleZ, 1);
        mgn->_Manager->SetFishEyeParameters(centerX, centerY, radius, angleX, angleY, angleZ, 1,
                                            true);

        int lDist_len = 0;
        void *lDist = con->GetDistTortion(&lDist_len);
        mgn->_Manager->SetDistortion(lDist, lDist_len);

        con->SetHasSetP720(true);

        mgn->_Manager->SetStitch2TextureAspect((float) w / (float) h);
    } else if (w > 0 && h > 0 && !con->GetHasSetP360() && con->GetDecoder()->getIsP360Dev()) {
        float centerX = 0, centerY = 0, radius = 0;
        con->GetFishEyeParameter360(&centerX, &centerY, &radius);
        LOGV("(w > 0 && h > 0 && !con->GetHasSetP360() && con->GetDecoder()->getIsP360Dev())  CenterX---%f,CenterY:%f,Radius:%f",
             centerX, centerY, radius);
//        if (radius > ((float) h / 2.0))
//            mgn->_Manager->SetFishEyeParameters(centerX / (float) w - 0.5,
//                                                centerY / (float) h - 0.5,
//                                                radius / (float) (w / 2.0), 0, 0, 0, 0, false);
//        else {
        if (centerY - radius < 0) {
            //radius = centerY;
        }
        if (centerY + radius > h) {
            //radius = h - centerY;
            //centerY = h/2;//不矫正半径，改矫正圆心
        }
        if (radius <= 0) {
            centerX = w / 2;
            centerY = h / 2;
            radius = centerY;
        }
        mgn->_Manager->SetFishEyeParameters(centerX / (float) w - 0.5,
                                            centerY / (float) h - 0.5,
                                            radius / (float) (w / 2.0), 0, 0, 0, 0, false);
//        }
        con->SetHasSetP360(true);
    }
}

JNIEXPORT jboolean JNICALL
Java_com_app_jagles_video_GLVideoRender_GetHaveCropParmeter(JNIEnv *env, jobject zthis, jlong ctx) {
    JAConnect *con = (JAConnect *) ctx;
    if (!con || !con->GetDecoder())
        return false;
    //LOGV1("GetHaveCropParameter: %d",con->GetDecoder()->getIsP360Dev());
    return con->GetDecoder()->getIsP360Dev();
}

JNIEXPORT jintArray JNICALL
Java_com_app_jagles_video_GLVideoRender_GetCropParameter(JNIEnv *env, jobject zthis, jlong ctx,
                                                         jint w, jint h) {
    JAConnect *con = (JAConnect *) ctx;
    if (!con)
        return NULL;
    //LOGV1("GetHaveCropParameter: %d",con->GetDecoder()->getIsP360Dev());
    if (con->GetDecoder()->getIsP360Dev()) {
        float radius, centerx, centery;
        int r[4];
        con->GetFishEyeParameter360(&centerx, &centery, &radius);
        r[0] = centerx - radius;
        r[1] = centery - radius;
        r[2] = r[0] + radius * 2;
        r[3] = r[1] + radius * 2;
        if (r[1] < 0)
            r[1] = 0;
        if (r[3] > h)
            r[3] = h;
        jintArray rt = env->NewIntArray(4);
        env->SetIntArrayRegion(rt, 0, 4, r);
        return rt;
    }
    return NULL;
}

void GLES_V2_DetectCrop(void *y_pixels, int width, int height, int *left, int *top, int *right,
                        int *bottom, int value) {
    int minx = width - 1;
    int maxx = 0;
    int miny = height;
    int maxy = 0;
    int x, y;
    unsigned char *buf = (unsigned char *) y_pixels;
    unsigned char *dbuf = (unsigned char *) malloc(width * height);
    memset(dbuf, 0, width * height);
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            if (buf[y * width + x] > value) {
                dbuf[y * width + x] = 1;
            }
        }
    }
    //for (y = 0; y < height / 3; y++)
    //{
    //	for (x = 0; x < width / 3; x++)
    //		printf("%d", dbuf[y * 3 * width + x * 3]);
    //	printf("\n");
    //}
    for (y = height / 3; y < height * 2 / 3; y++) {
        for (x = 10; x < width; x++) {
            if (dbuf[y * width + x] == 1) {
                if (x < minx) {
                    minx = x;
                }
                break;
            }
        }
    }
    for (y = height / 3; y < height * 2 / 3; y++) {
        for (x = width - 1; x >= 0; x--) {
            if (dbuf[y * width + x] == 1) {
                if (x > maxx)
                    maxx = x;
                break;
            }
        }
    }
    for (x = width / 3; x < width * 2 / 3; x++) {
        for (y = 0; y < height; y++) {
            if (dbuf[y * width + x] == 1) {
                if (y < miny) {
                    miny = y;
                }
                break;
            }
        }
    }
    for (x = width / 3; x < width * 2 / 3; x++) {
        for (y = height - 1; y > 0; y--) {
            if (dbuf[y * width + x] == 1) {
                if (y > maxy)
                    maxy = y;
                break;
            }
        }
    }
    *left = minx;
    *right = maxx;
    *top = miny;
    *bottom = maxy;
    //printf("left:%d,top:%d,Right:%d,Bottom:%d\n",*left,*top,*right,*bottom);
    free(dbuf);
}

void GLES_V2_DetectCropRGBA(void *pixels, int width, int height, int *rect, int *ltrb, int value) {
    unsigned char *buf = (unsigned char *) pixels;
    int w = rect[2] - rect[0];
    int h = rect[3] - rect[1];
    if (w <= 0 || h <= 0)
        return;
    if (rect[0] < 0 || rect[1] < 0 || rect[2] > width || rect[3] > height || rect[2] < rect[0] ||
        rect[3] < rect[1])
        return;
    unsigned char *gray = (unsigned char *) malloc(w * h);
    for (int i = rect[1]; i < rect[3]; i++)
        for (int j = rect[0]; j < rect[2]; j++)
            gray[(i - rect[1]) * w + j - rect[0]] = (unsigned char) (
                    buf[(i * width + j) * 4] * 0.299 + buf[(i * width + j) * 4 + 1] * 0.587 +
                    buf[(i * width + j) * 4 + 2] * 0.114);
    GLES_V2_DetectCrop(gray, w, h, ltrb, &ltrb[1], &ltrb[2], &ltrb[3], value);
    ltrb[0] += rect[0];
    ltrb[2] += rect[0];
    ltrb[1] += rect[1];
    ltrb[3] += rect[1];
    free(gray);
}

void FixCircle(float *radius, float *centerx, float *centery, int w, int h) {
    if (h == 960 && w == 1280 && *radius < 960 / 3) {
        *centerx = 640;
        *centery = 480;
        *radius = 450;
    } else if ((h == 1088 || h == 1080) && w == 1920 && *radius < 1000 / 2) {
        *centerx = 960;
        *centery = 540;
        *radius = 780;
    } else if (h == 1536 && w == 1536 && *radius < 650) {
        *centerx = 768;
        *centery = 768;
        *radius = 730;
    }
}

JNIEXPORT jintArray JNICALL
Java_com_app_jagles_video_GLVideoRender_SetCropInfo(JNIEnv *env, jobject zthis, jlong handle,
                                                    jlong conn, jint w, jint h) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn)
        return NULL;
    JAConnect *con = (JAConnect *) conn;
    if (!con)
        return NULL;
    uint8_t *rgbbuffer = (uint8_t *) calloc(1, w * h * 4);
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, (void *) rgbbuffer);
    int rect[4] = {0, 0, w, h};
    int r[4];
    int mWallMode;
    GLES_V2_DetectCropRGBA(rgbbuffer, w, h, rect, r, 130);
    if (r[0] >= w - 1)
        r[0] = 0;
    if (r[2] == 0)
        r[2] = w;
    if (r[1] >= h)
        r[1] = 0;
    if (r[3] == 0)
        r[3] = h;
    float a1 = (float) (r[2] - r[0]) / (float) (r[3] - r[1]);
    //		JACONNECT_INFO("A1:%f",a1);
    if (a1 > 1.33 && a1 < 1.9) {
        mWallMode = 1;
    } else {
        mWallMode = 0;
    }
    if (w > 0 && h > 0 && !con->GetHasSetP360()) {
        float centerX = (float) (r[2] - r[0]) / 2.0 + r[0], centerY =
                (r[3] - r[1]) / 2 + r[1], radius =
                (r[2] - r[0]) / 2;
        LOGV1("(w > 0 && h > 0 && !con->GetHasSetP360() && con->GetDecoder()->getIsP360Dev())  CenterX---%f,CenterY:%f,Radius:%f",
              centerX, centerY, radius);
        if (centerY - radius < 0) {
            //radius = centerY;
        }
        if (centerY + radius > h) {
            //radius = h - centerY;
            //centerY = h/2;//不矫正半径，改矫正圆心
        }
        if (radius <= 0) {
            centerX = w / 2;
            centerY = h / 2;
            radius = centerY;
        }
        con->SetHasSetP360(true);
        con->GetDecoder()->SetIsP360Dev(true, centerX, centerY, radius, mWallMode);
    }

    free(rgbbuffer);
    jintArray rt = env->NewIntArray(4);
    env->SetIntArrayRegion(rt, 0, 4, r);
    return rt;
}

JNIEXPORT void JNICALL
Java_com_app_jagles_video_GLVideoRender_SetFBOTexture(JNIEnv *env, jobject zthis, jlong handle,
                                                      jint texid) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    mgn->_Manager->SetFBOTexture(texid);
}

JNIEXPORT void JNICALL
Java_com_app_jagles_video_GLVideoRender_RenderFBO(JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    mgn->_Manager->RenderFBO();
}

JNIEXPORT jboolean JNICALL
Java_com_app_jagles_video_GLVideoRender_GetIsHEVC(JNIEnv *env, jobject zthis, jlong conn) {
    JAConnect *con = (JAConnect *) conn;
    if (!con)
        return false;
    return con->GetIsHEVC();
}


JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoRender_SetFishEyeParameters
        (JNIEnv *env, jobject zthis, jlong handle, jfloat centerX, jfloat centerY, jfloat radius,
         jfloat angleX, jfloat angleY, jfloat angleZ, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    mgn->_Manager->SetFishEyeParameters(centerX, centerY, radius, angleX, angleY, angleZ, index,
                                        true);
    LOGV("Java_com_app_jagles_video_GLVideoRender_SetFishEyeParameters");
}

JNIEXPORT void JNICALL
Java_com_app_jagles_video_GLVideoRender_CloseInfo(JNIEnv *env, jobject zthis, jlong handle) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager || !mgn->_Manager->CurrentParametric()) {
        return;
    }


    //LOGV2("Close Info................");
    if (mgn->_Manager->CurrentParametric()->GetScreenMode() == SCRN_NORMAL) {
        NormalScreen *mNormalScreen = (NormalScreen *) mgn->_Manager->CurrentParametric();
        mNormalScreen->ShowInfo(false, 0);
    } else
        mgn->_Manager->CurrentParametric()->ShowInfo(false);
}

JNIEXPORT void JNICALL
Java_com_app_jagles_video_GLVideoRender_SetDistortion(JNIEnv *env, jobject zthis, jlong handle,
                                                      jfloatArray distortion, jfloatArray angle,
                                                      jint distlen) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    jfloat *d, *a;

    d = env->GetFloatArrayElements(distortion, NULL);
    a = env->GetFloatArrayElements(angle, NULL);
    PDISTORTION_NUM dist = (PDISTORTION_NUM) calloc(1, sizeof(DISTORTION_NUM) * distlen);
    PDISTORTION_NUM dist1 = dist;
    for (int i = 0; i < distlen; i++) {
        dist1->angle = a[i];
        dist1->half_height = d[i];
        dist1++;
    }
    //LOGV4("setDistortion...............Java_com_juanvision_video_GLVideoRender_SetDistortion.--->distlen:%d",
//          distlen);
    mgn->_Manager->SetDistortion(dist, sizeof(DISTORTION_NUM) * distlen);
    env->ReleaseFloatArrayElements(distortion, d, 0);
    env->ReleaseFloatArrayElements(angle, a, 0);
}


JNIEXPORT void JNICALL
Java_com_app_jagles_video_GLVideoRender_SetKeepAspect(JNIEnv *env, jobject zthis, jlong handle,
                                                      jfloat aspect, jint index) {
    JAMANAGER *mgn = (JAMANAGER *) handle;
    if (!mgn || !mgn->_Manager) {
        return;
    }
    mgn->_Manager->SetKeepAspect(aspect, index);
}


#ifdef __cplusplus
}
#endif

//MEDIA_FILES := $(SRC_ROOT)/Media/JAPlayer.cpp