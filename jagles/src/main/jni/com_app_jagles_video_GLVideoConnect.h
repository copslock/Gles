/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_app_jagles_video_GLVideoConnect */

#ifndef _Included_com_app_jagles_video_GLVideoConnect
#define _Included_com_app_jagles_video_GLVideoConnect
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    InitManager
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_InitManager(
        JNIEnv *env, jobject zthis, jstring bundleid);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    InitAddr
 * Signature: (Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_InitAddr(
        JNIEnv *env, jobject zthis, jstring addr, jint port);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    DestroyManager
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_DestroyManager(
        JNIEnv *env, jobject zthis);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    GetConnectInstance
 * Signature: (Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_app_jagles_video_GLVideoConnect_GetConnectInstance
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    Connect1
 * Signature: (JLjava/lang/String;Ljava/lang/String;II)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_Connect1
  (JNIEnv *, jobject, jlong, jstring, jstring, jint, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    Connect
 * Signature: (JLjava/lang/String;Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_Connect
  (JNIEnv *, jobject, jlong, jstring, jstring, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    DisConnect
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_DisConnect
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    OpenChannel
 * Signature: (JIII)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_OpenChannel
  (JNIEnv *, jobject, jlong, jint, jint, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    CloseChannel
 * Signature: (JIII)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_CloseChannel
  (JNIEnv *, jobject, jlong, jint, jint, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    SendAudioPacket
 * Signature: (J[BIJLjava/lang/String;IIIFI)I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_SendAudioPacket
  (JNIEnv *, jobject, jlong, jbyteArray, jint, jlong, jstring, jint, jint, jint, jfloat, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    Call
 * Signature: (JII)I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_Call
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    HangUp
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_HangUp
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    SendData
 * Signature: (J[BII)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_SendData
  (JNIEnv *, jobject, jlong, jbyteArray, jint, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    PausePlayback
 * Signature: (JII)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_PausePlayback
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    ResumePlayback
 * Signature: (JII)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_ResumePlayback
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    GetBitrate
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_GetBitrate
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    PtzCtrl
 * Signature: (JIII)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_PtzCtrl
  (JNIEnv *, jobject, jlong, jint, jint, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    StopCtrl
 * Signature: (JII)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_StopCtrl
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    SearchRec
 * Signature: (JIIII)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_SearchRec
  (JNIEnv *, jobject, jlong, jint, jint, jint, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    StartPlayback
 * Signature: (JIII)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_StartPlayback
  (JNIEnv *, jobject, jlong, jint, jint, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    StopPlayback
 * Signature: (JII)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_StopPlayback
  (JNIEnv *, jobject, jlong, jint, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    CaptureImage
 * Signature: (JLjava/lang/String;II)Z
 */
JNIEXPORT jboolean JNICALL Java_com_app_jagles_video_GLVideoConnect_CaptureImage
  (JNIEnv *, jobject, jlong, jstring, jint, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    CaptureThumbnailImage
 * Signature: (JLjava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_app_jagles_video_GLVideoConnect_CaptureThumbnailImage
  (JNIEnv *, jobject, jlong, jstring, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    StartRecord
 * Signature: (JLjava/lang/String;I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_app_jagles_video_GLVideoConnect_StartRecord
  (JNIEnv *, jobject, jlong, jstring, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    StopRecord
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_StopRecord
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    GetNetWorkSpeed
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_GetNetWorkSpeed
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    GetAllNetWorkSpeed
 * Signature: (J)I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_GetAllNetWorkSpeed
  (JNIEnv *, jobject, jlong);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    GetChannel
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_GetChannel
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    GetRecordState
 * Signature: (JI)Z
 */
JNIEXPORT jboolean JNICALL Java_com_app_jagles_video_GLVideoConnect_GetRecordState
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    SearchDevice
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_SearchDevice
  (JNIEnv *, jobject);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    PullAlarmmsg
 * Signature: (JIIJI)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_PullAlarmmsg
  (JNIEnv *, jobject, jlong, jint, jint, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    UseDirectTexture
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_UseDirectTexture
  (JNIEnv *, jobject, jlong, jboolean);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    DecoderPause
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_DecoderPause
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    DecoderResume
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_DecoderResume
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    PlayAudioIndex
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_PlayAudioIndex
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    GetWallModelType
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_GetWallModelType
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    IsForceWallMode
 * Signature: (Z)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_IsForceWallMode
  (JNIEnv *, jobject, jboolean);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    EnableCrop
 * Signature: (JZ)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_EnableCrop
  (JNIEnv *, jobject, jlong, jboolean);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    DoDisConnect
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_DoDisConnect
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    GetFPS
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_GetFPS
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    GetHowBitrate
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_GetHowBitrate
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    isNeedOpen64Stream
 * Signature: (JI)Z
 */
JNIEXPORT jboolean JNICALL Java_com_app_jagles_video_GLVideoConnect_isNeedOpen64Stream
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    getIsInstallModeCome
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_getIsInstallModeCome
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    ResetForceWallMode
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_ResetForceWallMode
  (JNIEnv *, jobject);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    setOnlyDecoderIFrame
 * Signature: (JZI)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_setOnlyDecoderIFrame
  (JNIEnv *, jobject, jlong, jboolean, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    getFileDownloadList
 * Signature: (JI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_app_jagles_video_GLVideoConnect_getFileDownloadList
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    doDownloadFile
 * Signature: (JIILjava/lang/String;Ljava/lang/String;I)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_doDownloadFile
  (JNIEnv *, jobject, jlong, jint, jint, jstring, jstring, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    doCheckTutkOnline
 * Signature: (JLjava/lang/String;ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_doCheckTutkOnline
  (JNIEnv *, jobject, jlong, jstring, jint, jstring);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    SetHardwareDecoder
 * Signature: (JIZII)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_SetHardwareDecoder
  (JNIEnv *, jobject, jlong, jint, jboolean, jint, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    FinishDeviceFileTransfer
 * Signature: (JI)I
 */
JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_FinishDeviceFileTransfer
  (JNIEnv *, jobject, jlong, jint);

/*
 * Class:     com_app_jagles_video_GLVideoConnect
 * Method:    SetTimeZone
 * Signature: (JFI)V
 */
JNIEXPORT void JNICALL Java_com_app_jagles_video_GLVideoConnect_SetTimeZone
  (JNIEnv *, jobject, jlong, jfloat, jint);

JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_nativeInit
        (JNIEnv *, jobject, jstring, jstring);

JNIEXPORT jint JNICALL
Java_com_app_jagles_video_GLVideoConnect_GetRealState(JNIEnv *, jobject, jlong);


JNIEXPORT void  JNICALL
Java_com_app_jagles_video_GLVideoConnect_ResetTransfer(JNIEnv *, jobject, jlong, jint );


JNIEXPORT jint JNICALL Java_com_app_jagles_video_GLVideoConnect_DestroyConnector(
        JNIEnv *, jobject , jlong ) ;


#ifdef __cplusplus
}
#endif
#endif
