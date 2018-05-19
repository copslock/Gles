
//
//	Create by MiQi on 16/10/11
//
//

//#include "list.h"
#include "../../Network/ConnectManager.h"
#include <string.h>
#include <jni.h>
#include "handler/exception_handler.h"
#include "handler/minidump_descriptor.h"


google_breakpad::ExceptionHandler *exceptionHandler;

#define LOG_TAG "JAVideo"
#define LOGD(...)__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGV(...)__android_log_print(ANDROID_LOG_DEBUG,"Lee",__VA_ARGS__)
#define INFO(...) {__android_log_write(ANDROID_LOG_INFO,"videoconnect",__VA_ARGS__);}
#define LOGE(...) {__android_log_print(ANDROID_LOG_ERROR, "videoconnect", __VA_ARGS__);}
#define LOGV1(...) {__android_log_print(ANDROID_LOG_DEBUG,"testhardwaredecoder",__VA_ARGS__);}


#define MAX_CONN 36

JavaVM *mJavaVMc;
jobject mJavaObjc;

bool isForce;  //是否强制
bool isForceWallMode;  //是否强制壁挂

typedef struct connect_info {
    char deviceIdOrIp[80];
    char verify[80];
    char user[20];
    char password[20];
    int port;
    bool isConnect;
} CONNECTINFO, *PCONNECTINFO;

List<ConnectManager *> ConnectManagerList;
List<CONNECTINFO *> ConnectInfoList;

#ifdef __cplusplus
extern "C" {
#endif


void OnGsensorData(JAConnect *con, uint64_t timeStamp, double x, double y, double z, void *ctx) {
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);

    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnGSensorData", "(JDDD)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, timeStamp, x, y, z);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}


void ParseConnectString(char *connstr, char *verifystr, CONNECTINFO *connectinfo) {
    memset(connectinfo->deviceIdOrIp, 0, 20);
    memset(connectinfo->verify, 0, 80);
    memset(connectinfo->user, 0, 20);
    memset(connectinfo->password, 0, 20);
    connectinfo->port = 0;

    if (!strchr(connstr, ':'))
        strcpy(connectinfo->deviceIdOrIp, connstr);
    else {
        char *lip = strtok(connstr, ":");
        strcpy(connectinfo->deviceIdOrIp, lip);
        lip = strtok(NULL, ":");
        connectinfo->port = atoi(lip);
    }
    if (!strchr(verifystr, ':'))
        strcpy(connectinfo->verify, verifystr);
    else {
        char *luser = strtok(verifystr, ":");
        strcpy(connectinfo->user, luser);
        luser = strtok(NULL, ":");
        if (luser)
            strcpy(connectinfo->password, luser);
    }
}

/**
* char *转jstring
*/
jstring CharToString(JNIEnv *env, char *cstring) {
    jstring str;
    jbyteArray strArray;
    int length;
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");

    length = strlen(cstring);
    strArray = env->NewByteArray(length);
    env->SetByteArrayRegion(strArray, 0, length, (jbyte *) cstring);
    str = env->NewStringUTF("utf-8");
    str = (jstring) env->NewObject(strClass, ctorID, strArray, str);
    /*env->DeleteLocalRef(strClass);
    env->DeleteLocalRef(strArray);
    env->DeleteLocalRef(str);*/
    return str;
}

/**
* jstring转char *
*/
char *jstringTostrings(JNIEnv *env, jstring jstr) {
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
    /*env->ReleaseByteArrayElements(barr, ba, 0);
    env->DeleteLocalRef(strencode);
    env->DeleteLocalRef(clsstring);*/
    return rtn;
}

/*
	传递设备信息判断是否P720
*/
void OnDeviceInfo_GLVideoConnect(JAConnect *con, bool isP720, void *ctx) {
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);
    int i;
    if (java_class != NULL) {
        if (isP720)
            i = 0;
        else
            i = 1;
        java_fun_id = env->GetMethodID(java_class, "OnDeviceInfo", "(I)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, i);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}

void OnOOBFrameAvailable_GLVideoConnect(JAConnect *con, int installMode, int scene, void *ctx) {
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);
    int i;
    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnOOBFrameAvailable", "(II)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, installMode, scene);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}


void
OnDownloadList_GLVideoConnect(char *item_fp, int item_size, char *suffix, int item_type, int flag) {
    LOGV("GLVideoConnect::OnDownloadList_GLVideoConnect");
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);
    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnDownloadList",
                                       "(Ljava/lang/String;ILjava/lang/String;II)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, CharToString(env, item_fp), item_size,
                            CharToString(env, suffix), item_type, flag);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}

void OnDownloading_GLVideoConnect(int flag, int fdsize, long pos, int end) {
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);
    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnDownloading", "(IIJI)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, flag, fdsize, (jlong) pos, end);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}

/**
 *
 * 有效贴图回调函数
 *
 */
void OnTextureAvaible(
        int w, int h, void *frame, int frame_len, int frame_type, int timestamp, int index,
        void *ctx) {
    jclass javaClass;
    JNIEnv *env = NULL;
    jmethodID callBackid;
    mJavaVMc->AttachCurrentThread(&env, NULL);
    if (!env) {
        return;
    }


    javaClass = env->GetObjectClass(mJavaObjc);
    if (javaClass == NULL) {
        mJavaVMc->DetachCurrentThread();
        return;
    }


    if (!frame_len) {
        env->DeleteLocalRef(javaClass);
        mJavaVMc->DetachCurrentThread();
        return;

    }


    if (!frame) {
        env->DeleteLocalRef(javaClass);
        mJavaVMc->DetachCurrentThread();
        return;
    }

//	LOGD("OnTextureAvaible----->%d", frame_len);
//    LOGD("-------------width:%d,height:%d",w,h);
    jbyteArray nativePixels = env->NewByteArray(frame_len);
    if (nativePixels == NULL) {
        env->DeleteLocalRef(javaClass);
        mJavaVMc->DetachCurrentThread();
        return;
    }
    //if (!nativePixels)
    //return;

    env->SetByteArrayRegion(nativePixels, 0, frame_len, (jbyte *) frame);
    if (javaClass != NULL) {
        callBackid = env->GetMethodID(javaClass, "OnTextureAvaible", "(II[BIIIIJ)V");
        env->CallVoidMethod(mJavaObjc, callBackid, w, h, nativePixels, frame_len, frame_type,
                            timestamp, index, (jlong) ctx);
    }

    env->DeleteLocalRef(javaClass);
    env->DeleteLocalRef(nativePixels);
    mJavaVMc->DetachCurrentThread();

}

/**
 * 截图的结果回调函数
 */
void OnCaptureImage(
        bool success, int index, char *path) {
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);
    int i;
    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnCaptureImage", "(II)V");
        if (success) {
            i = 1;
        } else {
            i = 0;
        }
        env->CallVoidMethod(mJavaObjc, java_fun_id, i, index);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}

/**
 * OSD有效贴图数据的回调函数
 */
void OnOSDTextureAvaible(
        JAConnect *con, void *frame, int frame_len, void *ctx, int64_t utctime) {

    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);
    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnOSDTextureAvaible", "(IIJIJ)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, 480, 24, (jlong) frame, frame_len, utctime);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}

/**
 * 连接状态的回调函数
 */
void OnStatus(
        JAConnect *con, void *ctx, int status_code, int content_code, int index) {
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;
    char *msg = (char *) ctx;
    LOGE("this msg is .....%s", msg);
    jstring messsage;
    jbyteArray msgArray;
    int length;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);

    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");

    length = strlen(msg);
    msgArray = env->NewByteArray(length);
    env->SetByteArrayRegion(msgArray, 0, length, (jbyte *) msg);
    messsage = env->NewStringUTF("utf-8");
    messsage = (jstring) env->NewObject(strClass, ctorID, msgArray, messsage);
//    LOGV("GLVideoConnect::--------->>>%d", status_code);
    if (java_class != NULL) {
//        LOGV("GLVideoConnect::--------->%d", status_code);
        java_fun_id = env->GetMethodID(java_class, "OnStatus", "(JIIILjava/lang/String;)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, (jlong) ctx, status_code, content_code, index,
                            messsage);
    }
    env->DeleteLocalRef(java_class);
    env->DeleteLocalRef(msgArray);
    mJavaVMc->DetachCurrentThread();
}

/**
 * 回放搜索结果的回调函数
 */
void OnSearchRecData(
        JAConnect *con, int starttime, int endtime, int rectype, int index, bool theend,
        void *ctx) {
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);

    if (java_class != NULL) {
        int end;
        java_fun_id = env->GetMethodID(java_class, "OnSearchRecData", "(IIIII)V");

        if (theend)
            end = 1;
        else
            end = 0;

        env->CallVoidMethod(mJavaObjc, java_fun_id, starttime, endtime, rectype, index, end);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}

/**
 *  回放的时间戳的回调函数
 */
void OnPlaybackUpdateTime(
        JAConnect *con, int time, int index, void *ctx) {
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);

    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnPlaybackUpdateTime", "(II)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, time, index);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}

/**
 * 虚拟通道的回调函数
 */
void OnVconData(
        JAConnect *con, char *data, int len, int index, void *ctx) {
    LOGV("GLVideoConnect::OnVconData  len = %d", len);
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    jboolean isCopy;
    jbyte *vocnJbyte;
    jbyteArray vconJbyteArray;
    void *voidData;

    jstring dev;
    jbyteArray devArray;
    int length;

    if (*data != '{')
        return;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);

    vocnJbyte = (jbyte *) data;
    vconJbyteArray = env->NewByteArray(len);
    voidData = env->GetPrimitiveArrayCritical((jarray) vconJbyteArray, &isCopy);
    memcpy(voidData, vocnJbyte, len);
    env->ReleasePrimitiveArrayCritical((jarray) vconJbyteArray, voidData, 0);

    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");

    if (strcmp(con->GetDevId(), "") != 0) {
        length = strlen(con->GetDevId());
        devArray = env->NewByteArray(length);
        env->SetByteArrayRegion(devArray, 0, length, (jbyte *) con->GetDevId());
        dev = env->NewStringUTF("utf-8");
        dev = (jstring) env->NewObject(strClass, ctorID, devArray, dev);
    } else {
        length = strlen(con->GetIp());
        devArray = env->NewByteArray(length);
        env->SetByteArrayRegion(devArray, 0, length, (jbyte *) con->GetIp());
        dev = env->NewStringUTF("utf-8");
        dev = (jstring) env->NewObject(strClass, ctorID, devArray, dev);
    }

    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnVconData", "(I[BLjava/lang/String;)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, index, vconJbyteArray, dev);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}

/**
 * 直播时间戳的回调函数
 */
void OnOSDTxtTime(
        JAConnect *con, unsigned int OSDTime, int index, void *ctx) {
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);
    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnOSDTxtTime", "(II)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, OSDTime, index);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}

/**
 * 局域网设备的搜索回调函数
 */
void OnSearchDevice(JAConnect *con, SearchDeviceResult *pResult, void *ctx) {
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    jint port;
    jint max_ch;

    jstring eseeId;
    jstring ip;
    jstring devModel;

    jbyteArray eseeIdArray;
    jbyteArray ipArray;
    jbyteArray devModelArray;

    jboolean isCopy;

    void *data;

    int length;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);

    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");

    length = strlen(pResult->esee_id);
    eseeIdArray = env->NewByteArray(length);
    env->SetByteArrayRegion(eseeIdArray, 0, length, (jbyte *) pResult->esee_id);
    eseeId = env->NewStringUTF("utf-8");
    eseeId = (jstring) env->NewObject(strClass, ctorID, eseeIdArray, eseeId);

    length = strlen(pResult->ip);
    ipArray = env->NewByteArray(length);
    env->SetByteArrayRegion(ipArray, 0, length, (jbyte *) pResult->ip);
    ip = env->NewStringUTF("utf-8");
    ip = (jstring) env->NewObject(strClass, ctorID, ipArray, ip);

    port = pResult->port;
    max_ch = pResult->max_ch;

    length = strlen(pResult->dev_model);
    devModelArray = env->NewByteArray(length);
    env->SetByteArrayRegion(devModelArray, 0, length, (jbyte *) pResult->dev_model);
    devModel = env->NewStringUTF("utf-8");
    devModel = (jstring) env->NewObject(strClass, ctorID, devModelArray, devModel);

    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnSearchDevice",
                                       "(Ljava/lang/String;Ljava/lang/String;IILjava/lang/String;)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, eseeId, ip, port, max_ch, devModel);
    }

    env->DeleteLocalRef(java_class);
}

void OnSearchDeviceResults(struct SearchDeviceResult *pResult, void *ctx) {
    OnSearchDevice(NULL, pResult, ctx);
}

/**
 * 报警消息的回调函数
 */
void OnPullAlarmmsg(
        JAConnect *con, struct P2PAlarmMsgData *msgData, void *ctx) {
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);

    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");

    int type;
    int channel;
    time_t ticket;
    /*char src[64];
    char enc[8];*/
    int fps;
    int width, height;
    int samplerate, samplewidth, channelAudio;
    float compress_ratio;
    int totalLen, curLen;
    void *data;

    jbyteArray srcArray, encArray;
    jstring src, enc;
    int length;

    jboolean isCopy;
    jbyte *dataJbyte;
    jbyteArray dataJbyteArray;
    void *voidData;

    type = msgData->type;
    channel = msgData->chn;
    ticket = msgData->ticket;

    totalLen = msgData->totalLen;
    curLen = msgData->curLen;

    dataJbyte = (jbyte *) msgData->data;
    dataJbyteArray = env->NewByteArray(totalLen);
    voidData = env->GetPrimitiveArrayCritical((jarray) dataJbyteArray, &isCopy);
    memcpy(voidData, dataJbyte, totalLen);
    env->ReleasePrimitiveArrayCritical((jarray) dataJbyteArray, voidData, 0);

    switch (type) {
        case 0:
            break;
        case 1:
            break;
        case 2:
            length = strlen(msgData->i.src);
            srcArray = env->NewByteArray(length);
            env->SetByteArrayRegion(srcArray, 0, length, (jbyte *) msgData->i.src);
            src = env->NewStringUTF("utf-8");
            src = (jstring) env->NewObject(strClass, ctorID, srcArray, src);

            length = strlen(msgData->i.enc);
            encArray = env->NewByteArray(length);
            env->SetByteArrayRegion(encArray, 0, length, (jbyte *) msgData->i.enc);
            enc = env->NewStringUTF("utf-8");
            enc = (jstring) env->NewObject(strClass, ctorID, encArray, enc);

            width = msgData->i.width;
            height = msgData->i.height;
            fps = msgData->i.fps;
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
    }

    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnPullAlarmmsgData",
                                       "(IIJLjava/lang/String;Ljava/lang/String;IIIIIIFII[B)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id,
                            type,
                            channel,
                            (jlong) ticket,
                            src,
                            enc,
                            fps,
                            width,
                            height,
                            samplerate,
                            samplewidth,
                            channelAudio,
                            compress_ratio,
                            totalLen,
                            curLen,
                            dataJbyteArray);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}

/**
 * 硬件加速的Buffer写入，第一次有效
 */
void OnDirectTextureFrameUpdata(
        int w, int h, void *directBuffer, int index, void *ctx) {
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;
    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);
    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnDirectTextureFrameUpdata", "(IIJI)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, w, h, (jlong) directBuffer, index);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}

/**
 * 音频数据的回调
 */
void OnAudioData(JAConnect *con, void *data, int len, void *ctx) {
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);

    jbyteArray nativePixels = env->NewByteArray(len);
    env->SetByteArrayRegion(nativePixels, 0, len, (signed char *) data);

    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnAudioData", "([B)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, nativePixels);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}

void OnOpenChanneledSuccess(JAConnect *con, int nChannelId, int nStreamId, const char *camDes,
                            void *ctx) {
    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);

    if (java_class != NULL) {
        //LOGV("camDes:%s", camDes);
        java_fun_id = env->GetMethodID(java_class, "OnOpenChanneledSuccess",
                                       "(IILjava/lang/String;)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, nChannelId, nStreamId,
                            CharToString(env, (char *) camDes));

    }
    //env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();


}


//视频第一帧
void OnPlayedFirstFrame_GLVideoConnect(int w, int h, void *frame, int frame_len, int frame_type,
                                       int timestamp, int index, void *ctx) {

    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;
    int is180;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);


    //180度
    float aspect = w * 1.0 / h;
    LOGD("OnPlayedFirstFrame w = %d, h = %d, aspect = %0.2f", w, h, aspect);
    if (aspect > 1.33) {
        is180 = 1;
    } else if (aspect < 1.33) {
        is180 = 0;
    }

    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnPlayedFirstFrame", "(I)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, is180);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();

}

//检测TUTK是否在线回调
void OnCheckTUTKDevOnline(int result, char *msg) {
    LOGV("GLVideoConnect::OnCheckTUTKDevOnline  result = %d", result);

    JNIEnv *env;
    jclass java_class;
    jmethodID java_fun_id;
    LOGE("OnCheckTUTKDevOnline  this msg is .....%s", msg);
    jstring messsage;
    jbyteArray msgArray;
    int length;

    mJavaVMc->AttachCurrentThread(&env, NULL);
    java_class = env->GetObjectClass(mJavaObjc);

    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");

    length = strlen(msg);
    msgArray = env->NewByteArray(length);
    env->SetByteArrayRegion(msgArray, 0, length, (jbyte *) msg);
    messsage = env->NewStringUTF("utf-8");
    messsage = (jstring) env->NewObject(strClass, ctorID, msgArray, messsage);

    if (java_class != NULL) {
        java_fun_id = env->GetMethodID(java_class, "OnCheckTUTKDevOnline",
                                       "(ILjava/lang/String;)V");
        env->CallVoidMethod(mJavaObjc, java_fun_id, result, messsage);
    }
    env->DeleteLocalRef(java_class);
    mJavaVMc->DetachCurrentThread();
}

/**
 * 初始化底层的网络库
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_InitManager(
        JNIEnv *env, jobject zthis, jstring bundleid) {
    LOGV("JNI Version 0.0.7  解决了多通道设备连接闪退问题，XM不适用该库(OnVconRecv)");
    JAConnect_InitLib(jstringTostrings(env, bundleid));
    LOGV("Java_com_app_jagles_video_GLVideoConnect_InitManager");
    isForceWallMode = false;
    isForce = false;
//
//    char *a;
//    *a = 0;
}

/**
*	初始化转发服务器的地址
*/
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_InitAddr(
        JNIEnv *env, jobject zthis, jstring addr, jint port) {
    return JAConnect_InitAddr(jstringTostrings(env, addr), port);
}

/**
 * 获取一个链接实例
 * 将ConnectManager放进List中
 */
JNIEXPORT jlong JNICALL Java_com_app_jagles_video_GLVideoConnect_GetConnectInstance(
        JNIEnv *env, jobject zthis, jstring msg) {
    LOGV("Java_com_app_jagles_video_GLVideoConnect_GetConnectInstance");
    mJavaObjc = env->NewGlobalRef(zthis);
    ConnectManager *mConnectManager = new ConnectManager((void *) jstringTostrings(env, msg));
    mConnectManager->OnGsensorData = OnGsensorData;
    mConnectManager->OnTextureAvaible = OnTextureAvaible;
    mConnectManager->OnCaptureImage = OnCaptureImage;
    mConnectManager->OnOSDTextureAvaible = OnOSDTextureAvaible;
    mConnectManager->OnStatus = OnStatus;
    mConnectManager->OnSearchRecData = OnSearchRecData;
    mConnectManager->OnPlaybackUpdateTime = OnPlaybackUpdateTime;
    mConnectManager->OnVconData = OnVconData;
    mConnectManager->OnOSDTxtTime = OnOSDTxtTime;
    mConnectManager->OnPullAlarmmsg = OnPullAlarmmsg;
    mConnectManager->OnDirectTextureFrameUpdata = OnDirectTextureFrameUpdata;
    //   mConnectManager->OnSearchDevice = OnSearchDevice;
    mConnectManager->OnAudioData = OnAudioData;
    mConnectManager->OnOpenChanneledSuccess = OnOpenChanneledSuccess;
    mConnectManager->OnDeviceInfo = OnDeviceInfo_GLVideoConnect;
    mConnectManager->OnOOBFrameAvailable = OnOOBFrameAvailable_GLVideoConnect;
    mConnectManager->OnPlayedFirstFrame = OnPlayedFirstFrame_GLVideoConnect;
    mConnectManager->OnDownloadList = OnDownloadList_GLVideoConnect;
    mConnectManager->OnDownloading = OnDownloading_GLVideoConnect;
    mConnectManager->OnCheckTUTKDevOnline = OnCheckTUTKDevOnline;

    ConnectManagerList.insert(-1, mConnectManager);
    LOGE("getConnect...........%lu", mConnectManager);
    return (jlong) mConnectManager;
}

/**
 * 销毁底层的数据
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_DestroyManager(
        JNIEnv *env, jobject zthis) {
    LOGV("Java_com_app_jagles_video_GLVideoConnect_DestroyManager  ConnectManagerList.length() = %d",
         ConnectManagerList.length());
    for (int i = ConnectManagerList.length() - 1; i >= 0; i--) {
        LOGV("Java_com_app_jagles_video_GLVideoConnect_DestroyManager  delete前  i = %d", i);
        ConnectManager *test = ConnectManagerList.at(i);

        LOGV("Java_com_app_jagles_video_GLVideoConnect_DestroyManager  ConnectManagerList.at(i)");
        delete test;
        LOGV("Java_com_app_jagles_video_GLVideoConnect_DestroyManager  delete后  i = %d", i);
        ConnectManagerList.remove(i);
        LOGV("Java_com_app_jagles_video_GLVideoConnect_DestroyManager  remove后 i = %d", i);
    }
    LOGV("Java_com_app_jagles_video_GLVideoConnect_DestroyManager  end");
}

/**
 *
 *
 * @param env
 * @param zthis
 * @param handle
 * @return
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_DestroyConnector(
        JNIEnv *env, jobject zthis, jlong handle) {
    if (!handle) {
        return -1;
    }
    ConnectManager *conn = (ConnectManager *) handle;
    uint64_t conne = (uint64_t) conn;
    for (int i = ConnectManagerList.length() - 1; i >= 0; i--) {
        ConnectManager *test = ConnectManagerList.at(i);
        uint64_t testMan = (uint64_t) test;
        if (conne == testMan) {
            LOGD("Java_com_app_jagles_video_GLVideoConnect_DestroyConnector,conne:%llu", conne);
            delete test;
            ConnectManagerList.remove(i);
            return 0;
        }

    }
    return -1;

}


JNIEXPORT jint  JNICALL
Java_com_app_jagles_video_GLVideoConnect_GetRealState(JNIEnv *env, jobject zthis, jlong handle) {
    if (!handle) {
        return 0;
    }
    ConnectManager *conn = (ConnectManager *) handle;
    if (conn) {
        return conn->GetRealStatus(0);
    }
    return 0;
}


JNIEXPORT jlongArray JNICALL
Java_com_app_jagles_video_GLVideoConnect_GetAllConnectCtx(JNIEnv *env, jobject instance,
                                                          jlong handle) {
    jlong ctxs[MAX_CONN];
    jlongArray res = env->NewLongArray(MAX_CONN);

    memset(ctxs, 0, sizeof(jlong));
    env->SetLongArrayRegion(res, 0, MAX_CONN, ctxs);

    // TODO
    if (!handle)
        return res;
    ConnectManager *conn = (ConnectManager *) handle;
    for (int i = 0; i < MAX_CONN; i++) {
        ctxs[i] = conn->GetConnectCtx(i);
        //LOGD("GetAllConnectCtx clean con[%d] = %lld", i, ctxs[i]);
    }
    env->SetLongArrayRegion(res, 0, MAX_CONN, ctxs);
    return res;
}


JNIEXPORT void  JNICALL
Java_com_app_jagles_video_GLVideoConnect_ResetTransfer(JNIEnv *env, jobject zthis) {

    int ret = ja_p2p_reset_transfer();
    LOGD("ja_p2p_reset_transfer ret = 【【【【【%d】】】】】", ret);
}


/**
 * 连接设备的接口
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_Connect(
        JNIEnv *env, jobject zthis, jlong handle, jstring connectstr, jstring verifystr,
        jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->Connect(jstringTostrings(env, connectstr), jstringTostrings(env, verifystr), index);
    LOGD("Java_com_app_jagles_video_GLVideoConnect_Connect---->%d %lu", index, handle);
}

JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_Connect1(
        JNIEnv *env, jobject zthis, jlong handle, jstring connectstr, jstring verifystr, jint index,
        jint channel) {
    if (!handle)
        return;
    //return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->Connect(jstringTostrings(env, connectstr), jstringTostrings(env, verifystr), index,
                  channel);
    LOGD("Java_com_app_jagles_video_GLVideoConnect_Connect---->%d %lu", index, handle);
}

/**
 * 断开已经链接上的链接
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_DisConnect(
        JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return;
    //return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->Disconnect(index);
    LOGD("Java_com_app_jagles_video_GLVideoConnect_DisConnect---->%d %lu", index, handle);
    //  delete conn;
}

/**
 * 打开视频直播码流
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_OpenChannel(
        JNIEnv *env, jobject zthis, jlong handle, jint bitrate, jint channel, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->OpenChannel(bitrate, channel, index);
}

/**
 * 关闭直播码流
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_CloseChannel(
        JNIEnv *env, jobject zthis, jlong handle, jint bitrate, jint channel, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->CloseChannel(bitrate, channel, index);
}

/**
 * 发送音频数据包
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_SendAudioPacket(
        JNIEnv *env, jobject zthis, jlong handle, jbyteArray buffer, jint size, jlong tsMs,
        jstring enc,
        jint samplerate, jint samplewidth, jint channel, jfloat compressRatio, jint index) {
    if (!handle)
        return 0;
    ConnectManager *conn = (ConnectManager *) handle;
    void *data = (void *) env->GetByteArrayElements(buffer, 0);
    return conn->SendAudioPacket(data, size, tsMs, jstringTostrings(env, enc), samplerate,
                                 samplewidth, channel, compressRatio, index);
}

/**
 * 呼叫音频通话
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_Call(
        JNIEnv *env, jobject zthis, jlong handle, jint channelNo, jint index) {
    if (!handle)
        return 0;
    ConnectManager *conn = (ConnectManager *) handle;
    return conn->Call(channelNo, index);
}

/**
 * 挂断音频通话
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_HangUp(
        JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return 0;
    ConnectManager *conn = (ConnectManager *) handle;
    return conn->HangUp(index);
}

/**
 * 发送虚拟通道数据
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_SendData(
        JNIEnv *env, jobject zthis, jlong handle, jbyteArray buf, jint datasize, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    void *data = (void *) env->GetByteArrayElements(buf, 0);
    conn->SendData((char *) data, datasize, index);
}

/**
 * 暂停远程回放
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_PausePlayback(
        JNIEnv *env, jobject zthis, jlong handle, jint channel, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->PausePlayback(channel, index);
}

/**
 * 继续远程回放
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_ResumePlayback(
        JNIEnv *env, jobject zthis, jlong handle, jint channel, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->ResumePlayback(channel, index);
}

/**
 * 获取视频当前码流
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_GetBitrate(
        JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return 0;
    ConnectManager *conn = (ConnectManager *) handle;
    return conn->GetBitrate(index);
}

/**
 * 云台控制
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_PtzCtrl(
        JNIEnv *env, jobject zthis, jlong handle, jint action, jint channel, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->PtzCtrl(action, channel, index);
    LOGV("Java_com_app_jagles_video_GLVideoConnect_PtzCtrl channel = %d  action = %d", channel,
         action);
}

/**
 * 停止云台控制
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_StopCtrl(
        JNIEnv *env, jobject zthis, jlong handle, jint channel, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->StopPTZ(channel, index);
}

/**
 * 搜索远程回放的列表
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_SearchRec(
        JNIEnv *env, jobject zthis, jlong handle, jint startTime, jint endTime, jint channel,
        jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    LOGV("GLVideoConnect:------>%d,%d", startTime, endTime);
    conn->SearchRec(startTime, endTime, channel, index);
}

/**
 * 开始远程回放
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_StartPlayback(
        JNIEnv *env, jobject zthis, jlong handle, jint startTime, jint channel, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->StartPlayback(startTime, channel, index);
}

/**
 * 停止远程回放
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_StopPlayback(
        JNIEnv *env, jobject zthis, jlong handle, jint channel, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->StopPlayback(channel, index);
}

/**
 * 视频直播即时截图
 */
JNIEXPORT jboolean JNICALL Java_com_app_jagles_video_GLVideoConnect_CaptureImage(
        JNIEnv *env, jobject zthis, jlong handle, jstring filename, jint type, jint index) {
    if (!handle)
        return 0;
    ConnectManager *conn = (ConnectManager *) handle;
    LOGV("Java_com_app_jagles_video_GLVideoConnect_CaptureImage------>conn :%x", conn);
    return conn->CaptureImage(jstringTostrings(env, filename), type, index);
}

/**
* 视频直播即时截图
*/
JNIEXPORT jboolean JNICALL Java_com_app_jagles_video_GLVideoConnect_CaptureThumbnailImage(
        JNIEnv *env, jobject zthis, jlong handle, jstring filename, jint index) {
    if (!handle)
        return false;
    ConnectManager *conn = (ConnectManager *) handle;
    return conn->CaptureCurImage(jstringTostrings(env, filename), index);
}

/**
 * 开始直播的录像
 */
JNIEXPORT jboolean JNICALL Java_com_app_jagles_video_GLVideoConnect_StartRecord(
        JNIEnv *env, jobject zthis, jlong handle, jstring filename, jint index) {
    if (!handle)
        return false;
    ConnectManager *conn = (ConnectManager *) handle;
    return conn->StartRecord(jstringTostrings(env, filename), index);
}

/**
 * 停止直播的录像
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_StopRecord(
        JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->StopRecord(index);
}

/**
 * 拿到网络的网络数据速度
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_GetNetWorkSpeed(
        JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return 0;
    ConnectManager *conn = (ConnectManager *) handle;
    return conn->GetNetWorkSpeed(index);
}

/**
 * 拿到所有的网络的数据速度
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_GetAllNetWorkSpeed(
        JNIEnv *env, jobject zthis, jlong handle) {
    if (!handle)
        return 0;
    ConnectManager *conn = (ConnectManager *) handle;
    return conn->GetAllNetWorkSpeed();
}

/**
 * 根据下标拿到通道号
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_GetChannel(
        JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return 0;
    ConnectManager *conn = (ConnectManager *) handle;
    return conn->GetChannel(index);
}

/**
 * 根据下标拿到录像的状态
 */
JNIEXPORT jboolean JNICALL Java_com_app_jagles_video_GLVideoConnect_GetRecordState(
        JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return 0;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn) {
        LOGE("reture laji data?__1");
        return false;
    }
    bool b = conn->GetRecordState(index);
    LOGE("reture laji data?__2");
    return b;
}

/**
 * 获取局域网中所有的设备
 */
JNIEXPORT int JNICALL Java_com_app_jagles_video_GLVideoConnect_SearchDevice(
        JNIEnv *env, jobject zthis) {
    mJavaObjc = env->NewGlobalRef(zthis);
    return ja_p2p_search_device(OnSearchDeviceResults, NULL);
}

/**
 * 获取报警消息接口
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_PullAlarmmsg(
        JNIEnv *env, jobject zthis, jlong handle, jint type, jint channel, jlong ticket,
        jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->PullAlarmmsg(type, channel, ticket, index);
}

/**
 *
 * 通知解码使用directTexture的buffer,解码后的数据写进这个buffer中去
 *
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_UseDirectTexture(
        JNIEnv *env, jobject zthis, jlong handle, jboolean isDirect) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->UseDirectTexture(isDirect);
}

/**
 *
 * 停止解码
 *
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_DecoderPause(
        JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->DecoderPause(index);
}

/**
 *
 * 继续解码
 *
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_DecoderResume(
        JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->DecoderResume(index);
}

/**
 * 获取当前的安装模式
	0 ： 360度
	1 ： 180度
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_GetWallModelType(
        JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return 0;
    ConnectManager *conn = (ConnectManager *) handle;
    int wallMode = 0;
    if (conn->GetConnector(index))
        if (conn->GetConnector(index)->GetDecoder())
            wallMode = conn->GetConnector(index)->GetDecoder()->WallMode();
    if (isForce) {
        if (isForceWallMode) {
            wallMode = 1;
        } else {
            wallMode = 0;
        }
    }
    return wallMode;
}

/**
 * 播放下标的音频
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_PlayAudioIndex(
        JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    conn->PlayAudio(index);
}

/**
 * 强制设置壁挂
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_IsForceWallMode(
        JNIEnv *env, jobject zthis, jboolean isWallMode) {
    isForce = true;
    isForceWallMode = isWallMode;
}

JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_ResetForceWallMode(
        JNIEnv *env, jobject zthis) {
    isForce = false;
}

/**
 * 开启鱼眼网格
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_EnableCrop
        (JNIEnv *env, jobject zthis, jlong handle, jboolean value) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn)
        return;
    if (conn->GetConnector(0)) {
        if (conn->GetConnector(0)->GetDecoder()) {
            conn->GetConnector(0)->GetDecoder()->EnableCrop(value);
        }
    }

//    conn->GetConnector(0)->GetDecoder()->EnableCrop(value);
}

//实验性方法
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_DoDisConnect
        (JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn)
        return;
    conn->DoDisconnect(index);
}

JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_GetFPS
        (JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return 0;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn)
        return 0;
    return conn->GetFps(index);
}

JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_GetHowBitrate
        (JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return 2;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn)
        return 2;
    return conn->getHowBitrate(index);
}

JNIEXPORT jboolean JNICALL Java_com_app_jagles_video_GLVideoConnect_isNeedOpen64Stream
        (JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return false;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn)
        return false;
    return conn->isNeedOpen64Stream(index);
}

JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_getIsInstallModeCome
        (JNIEnv *env, jobject zthis, jlong handle, jint index) {
    if (!handle)
        return 3;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn)
        return 3;
    return conn->getIsInstallModeCome(index);
}

JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_setOnlyDecoderIFrame
        (JNIEnv *env, jobject zthis, jlong handle, jboolean value, jint index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn)
        return;
    conn->setOnlyDecoderIFrame(value, index);
}

JNIEXPORT jstring JNICALL Java_com_app_jagles_video_GLVideoConnect_getFileDownloadList
        (JNIEnv *env, jobject zthis, jlong handle, int index) {
    if (!handle)
        return NULL;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn)
        return NULL;
    char *result = conn->GetFileDownloadList(index);
    return env->NewStringUTF(result);
}


JNIEXPORT int JNICALL Java_com_app_jagles_video_GLVideoConnect_FinishDeviceFileTransfer
        (JNIEnv *env, jobject zthis, jlong handle, int index) {

    if (!handle)
        return -1;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn)
        return -1;

    return conn->FinishDeviceFileTransfer(index);

}


JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_doStopDownload
        (JNIEnv *env, jobject zthis, jlong handle) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn)
        return;
    conn->StopDownloadThread(false);
    conn->FinishDeviceFileTransfer(0);
}

JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_doDownloadFile
        (JNIEnv *env, jobject zthis, jlong handle, int opType, int opFlag, jstring fp,
         jstring file_path, int index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn)
        return;
    conn->DoDownloadFile(opType, opFlag, jstringTostrings(env, fp),
                         jstringTostrings(env, file_path), index);
}


///////////////////////检测TUTK///////////////////////////
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_doCheckTutkOnline
        (JNIEnv *env, jobject zthis, jlong handle, jstring tutkid, jint timeout, jstring msg) {
    LOGV("Java_com_app_jagles_video_GLVideoConnect_doCheckTutkOnline handle = %d  tutkid = %s",
         handle, tutkid);
    if (!handle)
        return -1;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn)
        return -1;

    return conn->doCheckTutkOnline(jstringTostrings(env, tutkid), timeout,
                                   jstringTostrings(env, msg));
}


//设置时区时间
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_SetTimeZone
        (JNIEnv *env, jobject zthis, jlong handle, jfloat pTimeZone, int index) {
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn)
        return;
    conn->SetTimeZone(pTimeZone, index);

}


JNIEXPORT void JNICALL
Java_com_app_jagles_video_GLVideoConnect_SetHardwareDecoder(JNIEnv *env, jobject zthis,
                                                            jlong handle, jint index,
                                                            jboolean value, jint width,
                                                            jint height) {
    //LOGV("Java_com_app_jagles_video_GLVideoConnect_SetHardwareDecoder handle = %d index: %d value %d", handle,index,value);
    //LOGV1("SetHardwareDecoder---> handle:%lu index: %d  value: %d  width:%d height:%d",handle,index,value,width,height);
    if (!handle)
        return;
    ConnectManager *conn = (ConnectManager *) handle;
    if (!conn)
        return;
    conn->SetHardwareDecoder(value, index, width, height);
}

JNIEXPORT void JNICALL
JNICALL Java_com_app_jagles_video_DoCrash(JNIEnv *env, jobject zthis) {
    int *p = NULL;
    *p = 1;
}

bool DumpCallback(const google_breakpad::MinidumpDescriptor &descriptor,
                  void *context,
                  bool succeeded) {
    LOGE("DumpCallback succeeded %d %s", succeeded, descriptor.path());
    return succeeded;
}

JNIEXPORT jint JNICALL JNICALL Java_com_app_jagles_video_GLVideoConnect_nativeInit
        (JNIEnv *env, jobject obj, jstring crash_dump_path, jstring app_version) {
    const char *path = (char *) env->GetStringUTFChars(crash_dump_path, NULL);
    const char *version = (char *) env->GetStringUTFChars(app_version, NULL);

    LOGE("nativeInit path = %s, version = %s", path, version);

    google_breakpad::MinidumpDescriptor descriptor(path, version);
    exceptionHandler = new google_breakpad::ExceptionHandler(descriptor, NULL, DumpCallback, NULL,
                                                             true, -1);
    env->ReleaseStringUTFChars(crash_dump_path, path);
    env->ReleaseStringUTFChars(app_version, version);
    return 0;
}


#ifdef __cplusplus
}
#endif
