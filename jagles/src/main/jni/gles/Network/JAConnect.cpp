//
// Created by LiHong on 16/2/6.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
#include <windows.h>
#else

#include <unistd.h>

#endif

#define __USE_NEWP2P__
//#define __MULTI_CONNECTOR__

#include "JAConnect.h"
#include "../Utils/mediabuffer.h"
#include "../Media/h265wh.h"
#include "../Media/h264wh.h"
//#include "SphereStitch2.h"
//#include "libjnnat.h"
#include "JAP2PConnector.h"
#include "JAP2PConnector2.h"
#include "JAP2PManager.h"
#include "ConnectManager.h"

#define LOG_TAG1 "testfirstframe"
#ifdef __linux__

#include <unistd.h>
#include <sys/prctl.h>

#define set_thread_name(name) \
{   \
    prctl(PR_SET_NAME, name); \
}
#elif defined(__APPLE__)
#define set_thread_name(name) \
{   \
    pthread_setname_np(name); \
}
#else
#define set_thread_name(name)
#endif

#define LOG_TAG "JAVideo"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGD1(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG1, __VA_ARGS__)
#define LOGD2(...) __android_log_print(ANDROID_LOG_DEBUG, "honglee_0801", __VA_ARGS__)
#define LOGV(...) __android_log_print(ANDROID_LOG_DEBUG, "Lee", __VA_ARGS__)
#define LOGV2(...) __android_log_print(ANDROID_LOG_DEBUG, "honglee_0902", __VA_ARGS__)
#define LOGV3(...) __android_log_print(ANDROID_LOG_DEBUG, "honglee_1019", __VA_ARGS__)
#define LOGV4(...) __android_log_print(ANDROID_LOG_DEBUG, "honglee_1024", __VA_ARGS__)
#define LOGV5(...) __android_log_print(ANDROID_LOG_DEBUG, "honglee_1102", __VA_ARGS__)
#define LOGV6(...) __android_log_print(ANDROID_LOG_DEBUG, "KP2P", __VA_ARGS__)
#define LOGV7(...) __android_log_print(ANDROID_LOG_DEBUG, "honglee_1110", __VA_ARGS__)
#define LOGV8(...) __android_log_print(ANDROID_LOG_DEBUG, "honglee_1111", __VA_ARGS__)
//#define LOGD(...) NULL
//#define LOGD1(...) NULL
//#define LOGD2(...) NULL
//#define LOGV(...) NULL
//#define LOGV2(...) NULL
//#define LOGV3(...) NULL
//#define LOGV4(...) NULL
//#define LOGV5(...) NULL
//#define LOGV6(...) NULL
//#define LOGV7(...) NULL
//#define LOGV8(...) NULL


#define INFO(...)                                                           \
    {                                                                       \
        __android_log_write(ANDROID_LOG_INFO, "videoconnect", __VA_ARGS__); \
    }
#define LOGE(...)                                                            \
    {                                                                        \
        __android_log_print(ANDROID_LOG_ERROR, "videoconnect", __VA_ARGS__); \
    }

#ifdef __ANDROID__

#include <android/log.h>

#define JACONNECT_INFO(...)                                         \
    {                                                               \
        __android_log_print(ANDROID_LOG_INFO, "0716", __VA_ARGS__); \
    }
#endif

#ifdef __APPLE__
#define JACONNECT_INFO(...)           \
    {                                 \
        fprintf(stderr, __VA_ARGS__); \
    }
#endif

#ifdef WIN32
#define JACONNECT_INFO(...)           \
    {                                 \
        fprintf(stderr, __VA_ARGS__); \
    }
#define strcasecmp stricmp
#endif

#define ONE_MILLION 1000000L

#include <time.h>
#include <h264wh.h>
#include <h265wh.h>

#define ThreadStackSize (512 * 1024)

extern List<JAP2PManager *> g_ListManager;
extern pthread_mutex_t g_listLock;


#ifdef __cplusplus
extern "C" {
#endif
//libjnnat Callback function

pthread_mutex_t gDecoderMutex;

#ifdef __ANDROID__
void OnNotSupportDirectTexture(bool success, void *pJAConnect) {
    JAConnect *con = (JAConnect *) pJAConnect;
    if (con)
        con->DoNotSupportDirectTexture(success);
}
#endif

void
OnOpenChannel(JAP2PManager *mgn, int nChannelId, int nStreamId, const char *camDes, void *ctx) {
    LOGV6("[JASON] ---JAConnect--- OnOpenChannel. --- %s", camDes);
    JAConnect *con = (JAConnect *) ctx;
    if (con) {
        LOGV6("open channel......");
        con->DoOnOpenChannel(nChannelId, nStreamId, camDes);
    }
}

//报警推送消息回调接口
void OnRecvAlarmMsg(JAP2PManager *mgn, struct P2PAlarmMsgData *msgData, void *ctx) {
    JAConnect *con = (JAConnect *) ctx;
    if (con) {
        if (!con->mVconbuffer) {
            con->mVconbuffer = (uint8_t *) malloc(msgData->totalLen);
            memset(con->mVconbuffer, 0, msgData->totalLen);
            con->mVconRecvLength = 0;
        }
        memcpy(con->mVconbuffer + con->mVconRecvLength, msgData->data, msgData->curLen);
        con->mVconRecvLength += msgData->curLen;
        if (con->mVconRecvLength == msgData->totalLen) {
            //todo
            msgData->data = (void *) malloc(msgData->totalLen);
            memset(msgData->data, 0, msgData->totalLen);
            memcpy(msgData->data, con->mVconbuffer, msgData->totalLen);
            con->DoPullAlarmmsg(msgData);
            free(con->mVconbuffer);
            con->mVconbuffer = NULL;
        }
    }
}

void OnConnect(JAP2PManager *mgn, const char *conn_type_msg, void *ctx) {
    JACONNECT_INFO("[JACONNECT]----------------------------- connect success! %s\n", conn_type_msg);
}

void OnDisconnect(JAP2PManager *mgn, int ret, void *ctx) {
    if (ret == JA_RET_CLOSEBYPEER || ret == KP2P_ERR_CLOSE_BY_PEER) {
        JAConnect *con = (JAConnect *) ctx;
        if (!con) {
            JACONNECT_INFO(
                    "----------------------------------------------NOT FOUND HANDLE-----------------------------------");
            return;
        }
        if (con->getConnectNowStates() == NET_STATUS_AUTHOFAIL)            //密码错误的不去重连
            return;
        if (con) {
//            LOGV7("OnStatus------>------ ondisconnect ---- %d --%s\n", ret, con->connectStr);
//            LOGV5("JAConnect.cpp-->OnDisconnect--->%d",ret);
            //con->DoStatus(NET_STATUS_DISCONNECTED, ret);
            //con->ReConnect();
//            if(con->GetAreadyConnected()) {
//                LOGV7("reconnect ---------> %s",con->connectStr);
//                con->Connect(con->connectStr, con->verifyStr);                    //对端断开重连
//            }

        }
    }

//    LOGD("")
    if (ret == JA_RET_CLOSE || ret == KP2P_ERR_CLOSE_BY_SELF || ret == JA_RET_FAILED) {
        JAConnect *con = (JAConnect *) ctx;
        if (con)
            con->DoStatus(NET_STATUS_ACTIVECLOSED, ret);
    }
    if (ret == JA_RET_CLOSEBYPEER || ret == JA_RET_TIMEOUT ||
        ret == KP2P_ERR_CLOSE_BY_PEER || ret == KP2P_ERR_TIMEOUT) {
        JAConnect *con = (JAConnect *) ctx;
        if (con && con->GetAreadyConnected()) {
            con->Connect(con->connectStr, con->verifyStr);

        }
        //对端断开重连//
    }
}

static bool g_firstiframe = false;
static int g_framecount = false;
uint64_t mLastTimeStampMs = 0;

//支持h265的视频直播回调
void OnRecvFrameEx(
        JAP2PManager *mgn,
        unsigned short frame_type,
        void *frame,
        int frame_len,
        JAP2P_UINT64 uiTimeStampMs,
        unsigned int uiWidth,
        unsigned int uiHeight,
        unsigned int uiSamplerate,
        unsigned int uiSamplewidth,
        unsigned int uiChannels,
        char *avEnc, void *ctx) {
    JACONNECT_INFO(
            "KP2PL recvFrameEx channel:%d frametype:%d, frame_len:%d, width:%d, height:%d, enc:%s,uiTimeStampMs:%llu",
            uiChannels, frame_type, frame_len, uiWidth, uiHeight, avEnc, uiTimeStampMs);

    JAConnect *con = (JAConnect *) ctx;

    if (con)
        if (con->mAddBuffer/* && con->GetChannel()==uiChannels*/ ) {
            //JACONNECT_INFO("[JACONNECT]----------frame_type:%d", frame_type);
            if (frame_type == JA_FT_OOB) {
                int installMode = eP2P_IMAGE_FISHEYE_FIX_MODE_NONE;
                int scene = 0;

                //g_firstiframe = false;
                g_framecount = 0;
                LP_BYPASS_FRAME lfishframe = (LP_BYPASS_FRAME) frame;
                if (lfishframe->frameType == P2P_BYPASS_FRAME_TYPE_FISHEYE_PARAM) {
                    //con->doOnDeviceInfo(false);
                    /*switch(lparam->version)
                    {
                        case 0x01000002:
                            con->setHowBirate(3);
                            con->setIsInstallModeCome(true);
                            break;
                    }*/
                    uint8_t *signedframe = (uint8_t *) calloc(1, frame_len);
                    memcpy(signedframe, frame, frame_len);

                    LP_FISHEYE_PARAM lparam = (LP_FISHEYE_PARAM) ((uint8_t *) signedframe + 8);


                    if (sizeof(ST_FISHEYE_PARAM) == 60) {
                        switch (lparam->version) {
                            case 0x01000003://1.0.0.3  带64码流(特殊帧)
                                con->setHowBirate(3);
                                break;

                            default:
                                break;
                        }

                        installMode = lparam->fixMode;
                        scene = 0;
                        if (installMode == eP2P_IMAGE_FISHEYE_FIX_MODE_WALL) {
                            scene = 180;
                        } else if (installMode == eP2P_IMAGE_FISHEYE_FIX_MODE_CEIL) {
                            scene = 360;
                        }

                        con->setIsInstallModeCome(lparam->fixMode);
                        if (lparam->param[0].Radius) {

                            con->GetDecoder()->UpdateCircleParameter(lparam->param[0].Radius,
                                                                     lparam->param[0].CenterCoordinateX,
                                                                     lparam->param[0].CenterCoordinateY);
//                        con->setFishEyeParameter360((float)lparam->param[0].Radius/(float)uiHeight,(float)lparam->param[0].CenterCoordinateX/(float)uiHeight-1.0,(float)lparam->param[0].CenterCoordinateY/(float)uiHeight-(float)uiWidth/(float)uiHeight);

                            con->setFishEyeParameter360(lparam->param[0].CenterCoordinateX,
                                                        lparam->param[0].CenterCoordinateY,
                                                        lparam->param[0].Radius);
                            con->GetDecoder()->SetIsP360Dev(true,
                                                            lparam->param[0].CenterCoordinateX,
                                                            lparam->param[0].CenterCoordinateY,
                                                            lparam->param[0].Radius,
                                                            lparam->fixMode ? 0 : 1);
                        } else {
                            con->GetDecoder()->SetIsP360Dev(false, 0, 0, 0, 0);
                        }

                        //if (48 == lfishframe->dataSize) {
                        //	//JACONNECT_INFO("[JACONNECT]----------------------lparam->fixMode:%d", lparam->fixMode);
                        //	con->GetDecoder()->UpdateCircleParameter(lparam->param[0].Radius, lparam->param[0].CenterCoordinateX, lparam->param[0].CenterCoordinateY);
                        //}
                        //else
                        //{
                        //	con->setHowBirate(3);
                        //	con->setIsInstallModeCome(true);
                        //	con->GetDecoder()->UpdateCircleParameter(lparam->param[0].Radius, lparam->param[0].CenterCoordinateX, lparam->param[0].CenterCoordinateY, lparam->fixMode);
                        //	JACONNECT_INFO("[JACONNECT]----------------------lparam->fixMode:%d", lparam->fixMode);
                        //}
                        LOGD1("[JACONNECT]----------------------lparam->param[0].Radius:%d, lparam->param[0].CenterCoordinateX:%d, lparam->param[0].CenterCoordinateY:%d, lparam->fixMode:%d",
                              lparam->param[0].Radius, lparam->param[0].CenterCoordinateX,
                              lparam->param[0].CenterCoordinateY, lparam->fixMode);
                    }
                    free(signedframe);

                } else if (lfishframe->frameType == P2P_BYPASS_FRAME_TYPE_LENS_PARAM) {
                    uint8_t *signedframe = (uint8_t *) calloc(1, frame_len);
                    memcpy(signedframe, frame, frame_len);
                    LP_FISHEYE_PARAM2 lparam2 = (LP_FISHEYE_PARAM2) ((uint8_t *) signedframe + 8);

                    scene = 720;
                    installMode = lparam2->fixMode;

                    con->setHowBirate(3);
                    con->setIsInstallModeCome(lparam2->fixMode);
                    con->setFishEyeParameter(lparam2->param[0].CenterCoordinateX,
                                             lparam2->param[0].CenterCoordinateY,
                                             lparam2->param[0].Radius, lparam2->param[0].AngleX,
                                             lparam2->param[0].AngleY,
                                             lparam2->param[0].AngleZ, 0);
                    con->setFishEyeParameter(lparam2->param[1].CenterCoordinateX,
                                             lparam2->param[1].CenterCoordinateY,
                                             lparam2->param[1].Radius, lparam2->param[1].AngleX,
                                             lparam2->param[1].AngleY,
                                             lparam2->param[1].AngleZ, 1);

                    void *angleData = (void *) ((uint8_t *) signedframe + 8 +
                                                sizeof(ST_FISHEYE_PARAM2));
                    con->setDistTortion(angleData,
                                        (int) (frame_len - 8 - sizeof(ST_FISHEYE_PARAM2)));

                    //con->doOnDeviceInfo(true);
                    free(signedframe);
                } else if (lfishframe->frameType == P2P_BYPASS_FRAME_TYPE_GSENSER_PARAM) {
                    LOGD("gsensor LP_GSENSOR_ANGLE uiTimeStampMs = %llu", mLastTimeStampMs);
                    LP_GSENSOR_ANGLE gsensorframe = (LP_GSENSOR_ANGLE) ((uint8_t *) frame + 8);
                    con->DoGsensorData(mLastTimeStampMs, gsensorframe->angle_x,
                                       gsensorframe->angle_y, gsensorframe->angle_z);
                    return;

                }

                mLastTimeStampMs = 0;
                con->doOnOOBFrameAvailable(installMode, scene);
                return;

            } else if (frame_type == JA_FT_IFRAME || frame_type == JA_FT_PFRAME) {
//                if (mLastTimeStampMs == 0) {
//                    if ((uiWidth == 1280 && uiHeight == 720) ||
//                        (uiWidth == 1920 && uiHeight == 1080)) {
//                        con->doOnOOBFrameAvailable(eP2P_IMAGE_FISHEYE_FIX_MODE_WALL, 180);
//                    } else if ((uiWidth == 1280 && uiHeight == 960) ||
//                               (uiWidth == 1536 && uiHeight == 1536) ||
//                               (uiWidth == 1944 && uiHeight == 1944)) {
//                        con->doOnOOBFrameAvailable(eP2P_IMAGE_FISHEYE_FIX_MODE_CEIL, 360);
//                    }
//                }
                mLastTimeStampMs = uiTimeStampMs;
            }


            int rt = -1;
//            if (frame_type == JA_FT_AUDIO) {
//            LOGV("JAConnect::avEnc = %s",avEnc);
            //	JACONNECT_INFO("have audio date come................");
            //con->GetDecoder()->WriteFrame((uint8_t*)frame, frame_len, 0, 0);
//			con->DoOnAudioData((unsigned char *)frame + sizeof(AUDIO_BUF_ATTR_t), frame_len - sizeof(AUDIO_BUF_ATTR_t));
//                con->DoOnAudioData2((unsigned char *) frame, frame_len,
//                                    strcasecmp(avEnc, "AAC") == 0 || strcasecmp(avEnc, "AAC1") == 0,
//                                    false, strcasecmp(avEnc, "AAC1") != 0);
//                rt = 0;
//            } else {
//#ifdef __ANDROID__
//			unsigned int timeStampM = uiTimeStampMs;
//			rt = con->AddBuffer((unsigned char *)frame, frame_len, frame_type, timeStampM, 0, avEnc);
//#else

            rt = con->AddBuffer((unsigned char *) frame, frame_len, frame_type, uiTimeStampMs,
                                0,
                                avEnc);
            rt = 0;
            //#endif
//            }
            g_framecount++;
        } else {
            //con->CleanBuffer();
        }
}
#pragma mark -
//旧的视频直播回调
void OnRecvFrame(
        JAP2PManager *mgn,
        unsigned short media_type,
        unsigned short frame_type,
        void *frame,
        int frame_len, void *ctx) {
    //JACONNECT_INFO("[JACONNECT]----------------------------- receive frame,len:%d,frame_type:%d\n",frame_len,frame_type);
    JAConnect *con = (JAConnect *) ctx;
    if (con && con->mAddBuffer)
        con->AddBuffer((unsigned char *) frame, frame_len, frame_type, 0, 0, NULL);
}

//回放的数据
void OnRecvRecFrame(void *frame,
                    int len,
                    int type,
                    int vWidth,
                    int vHeight,
                    const char *avencode,
                    int aSampleRate,
                    int aSampleWidth,
                    int aSampleChn,
                    int chn,
                    JAP2P_UINT64 timestamp,
                    unsigned int gentime,
                    void *ctx) {
//    LOGV("JAConnect::OnRecvRecFrame  receive frame,len:%d,frame_type:%d,timestamp:%d %s\n", len, type,timestamp,avencode);
    JAConnect *con = (JAConnect *) ctx;
    //JAMedia *decoder = con->GetDecoder();
    static JAP2P_UINT64 lLastVideoTS = 0;
    if (con && con->mAddBuffer) {
        int rt = -1;

        if (type == JA_FT_OOB) {
            LP_BYPASS_FRAME lfishframe = (LP_BYPASS_FRAME) frame;
            if (lfishframe && lfishframe->frameType == P2P_BYPASS_FRAME_TYPE_GSENSER_PARAM) {
                uint8_t *sensorframe = (uint8_t *) calloc(1, 48);
                memcpy(sensorframe, ((uint8_t *) frame + 8), 48);
                LP_GSENSOR_ANGLE gsensorframe = (LP_GSENSOR_ANGLE) sensorframe;
                if (gsensorframe) {
                    con->DoGsensorData(mLastTimeStampMs, gsensorframe->angle_x,
                                       gsensorframe->angle_y, gsensorframe->angle_z);
                    LOGD("gsensor OnRecvRecFrame DoGsensorData timestamp = %llu", timestamp);
                }
                free(sensorframe);
            }

        } else {
            if (type == JA_FT_IFRAME || type == JA_FT_PFRAME) {
                mLastTimeStampMs = timestamp;
            }
            rt = con->AddBuffer((unsigned char *) frame, len, type, timestamp, gentime,
                                (char *) avencode);
            if (rt) {
                printf("add buffer faile");
            }
        }


    }
}

void OnRecvRecFrame2(JAP2PManager *p2p, void *frame,
                     int len,
                     int type,
                     int vWidth,
                     int vHeight,
                     const char *avencode,
                     int aSampleRate,
                     int aSampleWidth,
                     int aSampleChn,
                     int chn,
                     JAP2P_UINT64 timestamp,
                     unsigned int gentime,
                     void *ctx) {
    //LOGV("JAConnect::OnRecvRecFrame  receive frame,len:%d,frame_type:%d,timestamp:%d\n", len, type,timestamp);
    JAConnect *con = (JAConnect *) ctx;
    //JAMedia *decoder = con->GetDecoder();
    static JAP2P_UINT64 lLastVideoTS = 0;
    if (con && con->mAddBuffer) {
        int rt = -1;

        if (type == JA_FT_OOB) {
            LP_BYPASS_FRAME lfishframe = (LP_BYPASS_FRAME) frame;
            if (lfishframe && lfishframe->frameType == P2P_BYPASS_FRAME_TYPE_GSENSER_PARAM) {
                LP_GSENSOR_ANGLE gsensorframe = (LP_GSENSOR_ANGLE) ((uint8_t *) frame + 8);
                con->DoGsensorData(mLastTimeStampMs, gsensorframe->angle_x, gsensorframe->angle_y,
                                   gsensorframe->angle_z);
                LOGD("gsensor OnRecvRecFrame2 DoGsensorData timestamp = %llu", timestamp);

            }
        } else {
            if (type == JA_FT_IFRAME || type == JA_FT_PFRAME) {
                mLastTimeStampMs = timestamp;
            }
            rt = con->AddBuffer((unsigned char *) frame, len, type, timestamp, gentime,
                                (char *) avencode);
        }
        if (rt) {
            printf("add buffer faile");
        }
    }
}

void JAConnect::DoGsensorData(uint64_t timeStamp, double x, double y, double z) {
    if (mCallback && mCallback->OnGsensorData)
        mCallback->OnGsensorData(this, timeStamp, x, y, z, mCallback->ctx);
}


void OnImageCapture(bool success, char *path, void *pJAConnect) {
    JAConnect *con = (JAConnect *) pJAConnect;
    if (con)
        con->DoCaptureImage(success, path);
}

void OnVideoRecord(bool success, char *path, void *pJAConnect) {
    JAConnect *con = (JAConnect *) pJAConnect;
    if (con)
        con->DoRecordVideo(success, path);
}

void OnSearchDeviceResult(struct SearchDeviceResult *pResult, void *ctx) {
    JAConnect *con = (JAConnect *) ctx;
    if (con)
        con->DoSearchDevice(pResult);
}

void OnLoginResult(JAP2PManager *mgn, int ret, void *ctx) {
}

void OnPtzResult(JAP2PManager *mgn, int ret, void *ctx) {

}

void OnVconRecv(JAP2PManager *mgn, void *buf, int len, void *ctx) {
    JAConnect *con = (JAConnect *) ctx;
    if (con)
        con->DoOnVconData((char *) buf, len);
}

void OnVconEvent(JAP2PManager *mgn, char *event, void *ctx) {

}

void OnP2PError(JAP2PManager *mgn, char *error_msg, int len, void *ctx) {

}

void OnVconRecv2(JAP2PManager *mgn, VCON_HANDLE *vcon_handle, void *buf, int len, void *ctx) {
    //LOGV("JAConnect::OnVconRecv2 ");
    JAConnect *con = (JAConnect *) ctx;
    if (con) {
        if (vcon_handle) {
            VCON_HANDLE *dlv = con->getDownLoadVconHandle();
            if (!dlv) {
                con->DoOnVconData((char *) buf, len);
                return;
            }

            if (dlv->vcon_id == vcon_handle->vcon_id) {
                con->doOnVconFileOperate(buf, len);
            } else {
                con->DoOnVconData((char *) buf, len);
            }
        } else
            con->DoOnVconData((char *) buf, len);
    }
}

void
OnConnectReq(JAP2PManager *mgn, int proto_type, int action, unsigned int ip, unsigned short port,
             unsigned int random, void *ctx) {

}

//初始化JAConnect跟P2P
void JAConnect_InitLib(const char *bundleid) {
    if (gIsInitNetwork)
        return;
    memset(&gJACallbackInit, 0, sizeof(gJACallbackInit));
    gJACallbackInit.OnConnect = OnConnect;
    gJACallbackInit.OnConnectReq = OnConnectReq;
    gJACallbackInit.OnDisconnect = OnDisconnect;
    gJACallbackInit.OnLoginResult = OnLoginResult;
    gJACallbackInit.OnP2PError = OnP2PError;
    gJACallbackInit.OnPtzResult = OnPtzResult;
    //   gJACallbackInit.OnRecvFrame   = OnRecvFrame;
    gJACallbackInit.OnRecvFrameEx = OnRecvFrameEx;
    gJACallbackInit.OnRecvRecFrame = OnRecvRecFrame2;
    gJACallbackInit.OnVconEvent = OnVconEvent;
    gJACallbackInit.OnVconRecv = OnVconRecv;
    gJACallbackInit.OnVconRecv2 = OnVconRecv2; //XM下载专属
    gJACallbackInit.OnRecvAlarmMsg = OnRecvAlarmMsg;
    gJACallbackInit.OnOpenChannel = OnOpenChannel;
    pthread_mutex_init(&gDecoderMutex, NULL);
    //ja_p2p_set_debug_level(0);
    //ja_p2p_init(&gJACallbackInit);
    gIsInitNetwork = true;
    JACONNECT_INFO("this app bundleId is :%s", bundleid);
    //ja_p2p_set_clientinfo_bundleid(bundleid);

}

/*
*	初始化转发服务器的地址
*/
int JAConnect_InitAddr(const char *addr, unsigned short port) {
    return 0; //ja_p2p_set_tcpt_server(addr, port);
}

//impl JAConnect class

void *JAConnect_Task(void *arg) {
    JAConnect *con = (JAConnect *) arg;
    set_thread_name("conn_task");
    con->HandleTaskThread();
    return NULL;
}

void *JAConnect_Audio(void *arg) {
    JAConnect *con = (JAConnect *) arg;
    return NULL;
}

void *JAConnect_Decode(void *arg) {
    JAConnect *con = (JAConnect *) arg;
    set_thread_name("conn_decode");
    con->HandleDecodeThread();
    return NULL;
}

void *JAConnect_DisConnect(void *arg) {
    JAConnect *con = (JAConnect *) arg;
    set_thread_name("conn_dis");
    con->DoDisconnectThread();
    return NULL;
}

#ifdef __cplusplus
}
#endif

JAConnect *FindConnector(JAP2P_HANDLE p_handle) {
    for (int i = 0; i < gConnector.length(); i++) {
        if (gConnector.at(i) != NULL) {
            if (gConnector.at(i)->getHandle() == p_handle) {
                return gConnector.at(i);
            }
        }
    }
    return NULL;
}

JAP2P_HANDLE JAConnect::getHandle() {
    return mHandle;
}

void JAConnect::CleanBuffer() {
    printf("====media buffer reset\n");
    ResetMediaBuffer(mBuffer);
}

int JAConnect::AddBuffer(unsigned char *buf,
                         int buf_len,
                         int frame_type,
//#ifdef __ANDROID__
//	unsigned int pTimestamp,
//#else
                         uint64_t pTimestamp,
//#endif
                         unsigned int pPlaytime,
                         const char *_enc) {
    return WrtieBuffer(mBuffer, buf, buf_len, frame_type, pTimestamp, pPlaytime, _enc);
}

void JAConnect::GetSPSVideoWH(void *buf, int buflen, int *w, int *h) {

}

void JAConnect::preRender(uint64_t presentationTimeUsec) {
    if (mPrevMonoUsec == 0) {
        // Latch current values, then return immediately.
        mPrevMonoUsec = clock_ms() * 1000;
        mPrevPresentUsec = presentationTimeUsec;
    } else {
        // Compute the desired time delta between the previous frame and this frame.
        long frameDelta;
        if (mFixedFrameDurationUsec != 0) {
            // Caller requested a fixed frame rate.  Ignore PTS.
            frameDelta = mFixedFrameDurationUsec;
        } else {
            frameDelta = presentationTimeUsec - mPrevPresentUsec;
        }
        if (frameDelta < 0) {
            //Log.w("honglee", "Weird, video times went backward");
            frameDelta = 0;
        } else if (frameDelta == 0) {
            // This suggests a possible bug in movie generation.
            //Log.i("honglee", "Warning: current frame and previous frame had same timestamp");
        } else if (frameDelta > 10 * ONE_MILLION) {
            // Inter-frame times could be arbitrarily long.  For this player, we want
            // to alert the developer that their movie might have issues (maybe they
            // accidentally output timestamps in nsec rather than usec).
//            Log.i("honglee", "Inter-frame pause was " + (frameDelta / ONE_MILLION) +
//                             "sec, capping at 5 sec");
            frameDelta = 5 * ONE_MILLION;
        }

        long desiredUsec = mPrevMonoUsec + frameDelta;  // when we want to wake up
        long nowUsec = clock_ms() * 1000;
        while (nowUsec < (desiredUsec - 100) /*&& mState == RUNNING*/) {
            // Sleep until it's time to wake up.  To be responsive to "stop" commands
            // we're going to wake up every half a second even if the sleep is supposed
            // to be longer (which should be rare).  The alternative would be
            // to interrupt the thread, but that requires more work.
            //
            // The precision of the sleep call varies widely from one device to another;
            // we may wake early or late.  Different devices will have a minimum possible
            // sleep time. If we're within 100us of the target time, we'll probably
            // overshoot if we try to sleep, so just go ahead and continue on.
            long sleepTimeUsec = desiredUsec - nowUsec;
            if (sleepTimeUsec > 500000) {
                sleepTimeUsec = 500000;
            }
            usleep(sleepTimeUsec);
            nowUsec = clock_ms() * 1000;
        }

        // Advance times using calculated time values, not the post-sleep monotonic
        // clock time, to avoid drifting.
        mPrevMonoUsec += frameDelta;
        mPrevPresentUsec += frameDelta;
    }
}

void JAConnect::HandleDecodeThread() {
#ifdef _MSC_VER
    int64_t lasttime = clock_ms();
#else
    uint64_t lasttime = clock_ms();
#endif
    uint64_t curtime = clock_ms();
    while (mRunThread) {
        //printf("decode thread.........start\n");
        DecoderLock();
        while (!mDecodeStatus)
            pthread_cond_wait(&mDecoderCond, &mDecoderLock);
        DecoderUnLock();
        int fps = 0;
        int perFrametime = 40;
        if (mRealStatus == NET_PLAY_REAL) {
            fps = GetCurrentFPS(mBuffer);
            if (fps == 0)
                perFrametime = 40;
            else
                perFrametime = 1000 / fps;
        } else if (mRealStatus == NET_PLAY_RECORD) /* 回放中获取buf的速度 */
        {
            int bufoffset = GetWriteCount(mBuffer) - GetReadCount(mBuffer, mBufferReader);
            if (bufoffset > 150 && mPlaybackStatus == PLAYBACK_STATUS_GETDATA) {
                PlaybackPauseGetData(mChannelNo);
                mPlaybackStatus = PLAYBACK_STATUS_PAUSE;
            } else if (bufoffset < 50 && mPlaybackStatus == PLAYBACK_STATUS_PAUSE) {
                PlaybackContinueGetData(mChannelNo);
                mPlaybackStatus = PLAYBACK_STATUS_GETDATA;
            }
        }
        unsigned char *buf;
        unsigned int bufsize = 2048000;
        int ptype;
        //#ifdef __ANDROID__
        //		uint32_t ptimestamp = 0;
        //#else
        uint64_t ptimestamp = 0;
//#endif
        unsigned int playtime;
        char _enc[8];
        memset(_enc, 0, 8);
        int pwidth, pheight;

        int rt = ReadBuffer(mBuffer, &buf, &bufsize, &ptype, &ptimestamp, &playtime, mBufferReader,
                            _enc);
        if (rt != MB_SUCCESS) {
            //unlockRead(mBuffer, mBufferReader);
#ifdef _MSC_VER
            Sleep(1);
#else
            usleep(1000);
#endif
            //LOGD2("-----------------------HandleDecodeThread end...........................");
            continue;
        }

//        if(GetWriteCount(mBuffer) - GetReadCount(mBuffer,mBufferReader)>30)
//            onlyDecodeIFrame = true;
//
//        if(GetWriteCount(mBuffer) - GetReadCount(mBuffer,mBufferReader)<5)
//            onlyDecodeIFrame = false;

        //LOGV("writebuffer - readbuffer ========%d  %d",GetWriteCount(mBuffer) - GetReadCount(mBuffer,mBufferReader),onlyDecodeIFrame);

        //LOGD1("----------------------ReadBuffer Success........");

        if (onlyDecodeIFrame && ptype != JA_FT_IFRAME)            //超过8路同时播放的时候只解码I帧视频
        {
            //	JACONNECT_INFO("only decoder IFRAME..............");
            unlockRead(mBuffer, mBufferReader);
#ifdef _MSC_VER
            Sleep(1);
#else
            usleep(1000);
#endif
            continue;

        }
        //JACONNECT_INFO("KP2PL ReadBuffer-----type:%d  len:%d",ptype,bufsize);
        if (mRealStatus == NET_PLAY_RECORD) {
            perFrametime = ptimestamp - mLastTimestamp;
            if (abs(perFrametime) > 1000) {
                mPrevMonoUsec = 0;
//                LOGD2("honglee_0801-------");
            }
            if (perFrametime > 5000 || perFrametime < 0) {
                perFrametime = 40;
            }
            unsigned int ltime = ptimestamp / 1000;
            if (ltime != mLastUpdateTime && ptype != JA_FT_AUDIO) {
                LOGD2("---======>%d %d %d %d", ltime, playtime, ptype, bufsize);
                if (mCallback && mCallback->PlaybackTimeUpdate)
                    if (mRealStatus == NET_PLAY_RECORD)
                        mCallback->PlaybackTimeUpdate(this, ltime, mCallback->ctx);
                mLastUpdateTime = ltime;
            }
        } else {
            //#ifdef __ANDROID__
            //			unsigned int ltime = ptimestamp;
            //#else
            uint64_t ltime = ptimestamp;
            //#endif
            if (mRealStatus == NET_PLAY_RECORD)
                //	ltime += 28800;
                if (mCallback && mCallback->OnOSDTextTime && ltime > 946656000)
                    mCallback->OnOSDTextTime(this, ltime, mCallback->ctx);
        }
        if (rt == MB_SUCCESS) {
#ifdef _MSC_VER
            int64_t lgmtime;
#else
            long lgmtime;
#endif
            if (!mRecvIFrame && ptype == JA_FT_IFRAME)
                mRecvIFrame = true;

            if (!mRecvIFrame) {
#ifdef _MSC_VER
                Sleep(1);
#else
                usleep(1000);
#endif
                //LOGD2("-----------------------HandleDecodeThread end...........................");
                continue;
            }

            if (ptype == JA_FT_AUDIO) //音频数据的回调
            {
                // if (mCallback && mCallback->OnAudioData)
                // {
                // 	uint8_t *audiobuf;
                //     uint8_t *buf_ = (uint8_t *)malloc(360);
                // 	audiobuf = (unsigned char*)malloc(16000);
                // 	batch_alaw2linear((short *)audiobuf,
                //                       buf,
                //                       bufsize);				//G711音频解码
                //     mAudioPts = ptimestamp - (mQueued-mProcessed)*40; /* 当前播放的音频时间戳 */
                // 	mCallback->OnAudioData(this, (void *)audiobuf, bufsize*2, mCallback->ctx);
                // 	free(audiobuf);
                // }

                if (bufsize > 0) {
                    DoOnAudioData2(buf, bufsize,
                                   strcasecmp(_enc, "AAC") == 0 || strcasecmp(_enc, "AAC1") == 0,
                                   mRealStatus == NET_PLAY_RECORD, strcasecmp(_enc, "AAC") == 0);
                }
#ifdef _MSC_VER
                Sleep(1);
#else
                usleep(1000);
#endif
                //LOGD2("-----------------------HandleDecodeThread end...........................");
                continue;
            }

            if (strcasecmp(_enc, H265_Enc) == 0) {
                mHardwareDecoder = false;
//                mRecvIFrame = false;
            }

            if (strcasecmp(_enc, H265_Enc) == 0)
                mCurrentCodec = true;
            else
                mCurrentCodec = false;

            if (mHardwareDecoder && ptype == JA_FT_IFRAME) {
                int ret;
                if (strcasecmp(_enc, H265_Enc) == 0)
                    mDecoder->AnalystHeader(AV_CODEC_ID_HEVC, buf, bufsize, AV_PIX_FMT_BGR32);
                else
                    mDecoder->AnalystHeader(AV_CODEC_ID_H264, buf, bufsize, AV_PIX_FMT_BGR32);
            }

#ifdef __ANDROID__
            if (ptype) {
                mDecoder->WriteFrame(buf, bufsize, ptype, perFrametime);
            }
#else
                mDecoder->WriteFrame(buf, bufsize, ptype, perFrametime);
#endif
            //JACONNECT_INFO("...........................................................################## width:%d,height:%d,ret:%d\n",mVideoWidth,mVideoHeight,mHardwareDecoder);
            LOGD("JAConnect--------->%d----%d------%d", mMaxSupportWidth, mMaxSupportHeight,
                 mHardwareDecoder);
            if (mHardwareDecoder) {
                int w = 0, h = 0;
                if (ptype == JA_FT_IFRAME) {
                    if (strcasecmp(_enc, H265_Enc) == 0) {
                        if (!H265GetWidthHeight((char *) buf, bufsize, &w, &h)) {
                            usleep(1000);
                            continue;
                        }
                    } else {
                        if (!GetWidthHeight((char *) buf, bufsize, &w, &h)) {
                            usleep(1000);
                            continue;
                        }
                    }
                    if (w != mVideoWidth || h != mVideoHeight) {
                        mVideoWidth = w;
                        mVideoHeight = h;
                    }
                }
                LOGD("JAConnect--------->%d----%d------%d-------%d", w, h, mMaxSupportWidth,
                     mMaxSupportHeight);
                if (w > mMaxSupportWidth || h > mMaxSupportHeight ||
                    strcasecmp(_enc, H265_Enc) == 0) {
                    mHardwareDecoder = false;
                    mRecvIFrame = false;
                } else {
                    //JACONNECT_INFO("KP2PL OnFrame ..........");
                    //TODO: 4-17
                    struct timeval *tv_old_hard;
                    struct timeval *tv_new_hard;
                    tv_old_hard = (timeval *) malloc(sizeof(timeval));
                    tv_new_hard = (timeval *) malloc(sizeof(timeval));
                    gettimeofday(tv_old_hard, NULL);


                    if (mCallback && mCallback->OnFrame)
                        mCallback->OnFrame(this, buf, bufsize, FRAME_TYPE_RAW, mCallback->ctx);
                    if (abs(mFirstFrameTimeStimp - ptimestamp) > 2000) {
                        if (mCallback)
                            if (mCallback && mCallback->OnStatus)
                                mCallback->OnStatus(this, mCallback->ctx, NET_STATUS_FIRST_FRAME,
                                                    0);
                        mPrevMonoUsec = 0;
                    }
                    mFirstFrameTimeStimp = ptimestamp;
#ifdef _MSC_VER
                    int64_t lgmtime;
#else
                    long lgmtime;
#endif
                    unlockRead(mBuffer, mBufferReader);

                    //#ifdef __ANDROID__
                    //				long limit_time = 946656000L;
                    //#else
                    long long limit_time = 946656000LL;
                    //#endif
                    if (mRealStatus == NET_PLAY_RECORD) {
                        lgmtime = ptimestamp / 1000;
                    } else {
                        //#ifdef __ANDROID__
                        //					lgmtime = ptimestamp;
                        //#else
                        lgmtime = ptimestamp / 1000;
                        //#endif
                    }

                    char *txtbuf = (char *) malloc(40);
                    memset(txtbuf, 0, 40);
                    struct tm *t = localtime((const time_t *) &lgmtime); ////// 当地时间

                    if (abs(mTimeZone) > 12) {
                        sprintf(txtbuf, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", t->tm_year + 1900,
                                t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                    } else {
                        int lhour = (int) mTimeZone;
                        int lmin = (int) ((mTimeZone - lhour) * 100);
                        int lOffsetMs = (lhour * 3600 + lmin * 60);
                        lgmtime += lOffsetMs;
                        struct tm *utc_t = gmtime((const time_t *) &lgmtime); ////// GMT 时间
                        sprintf(txtbuf, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
                                utc_t->tm_year + 1900, utc_t->tm_mon + 1, utc_t->tm_mday,
                                utc_t->tm_hour, utc_t->tm_min, utc_t->tm_sec);
                        printf(">>>>>> 修正后的时间:%s\n", txtbuf);
                    }
                    //LOGV6("playback time--------%s %llu %llu",txtbuf,lgmtime,ptimestamp);
                    if (lgmtime > limit_time) {
                        mCallback->OnOSDFrame(this, (unsigned char *) txtbuf, 20, mCallback->ctx,
                                              ptimestamp);
                    } else {
                        mCallback->OnOSDFrame(this, NULL, 20, mCallback->ctx, ptimestamp);
                    }
                    free(txtbuf);
                    //usleep(1000);
                    if (clock_ms() - lasttime > 200)
                        mPrevMonoUsec = 0;
                    lasttime = clock_ms();
                    //TODO 4-17
//                    preRender(ptimestamp * 1000);
//                    mLastTimestamp = ptimestamp;
                    gettimeofday(tv_new_hard, NULL);
                    int delayTime_hard = (int) (tv_new_hard->tv_usec / 1000 -
                                                tv_old_hard->tv_usec / 1000);
//                    int frameDur = ptimestamp - mLastTimestamp;
                    int frameDur = ptimestamp - mLastTimestamp;
                    int fps_ = 0;
                    if (frameDur < 20) {
                        fps_ = GetFPS() > 0 ? GetFPS() : 20;
                        if (fps_ > 30) {
                            fps_ = 15;
                        };
                        frameDur = 1000 / fps_;
                    }

//                    if(frameDur<20){
//                        int fps_ = GetFPS()>0?GetFPS():25;
//                        frameDur = 1000/fps_;
//                    }
                    int sleepTime = frameDur - delayTime_hard - 5;

                    if (sleepTime < 0) {
                        sleepTime = 0;
                    } else if (sleepTime > 500) {
                        sleepTime = 60;
                    }
                    //preRender(ptimestamp * 1000);
                    usleep(sleepTime * 1000);

                    //LOGV("hardware sleepTime:%d delayTime_hard:%d frameDur:%d ptimestamp:%llu",sleepTime,delayTime_hard,frameDur,ptimestamp);
                    mLastTimestamp = ptimestamp;
                    free(tv_old_hard);
                    free(tv_new_hard);
                    continue;
                }
            }

            if (ptype == JA_FT_IFRAME) {
                if (mCallback && mCallback->OnFrame) {
                    if (strcasecmp(_enc, H265_Enc) == 0) {

                        mDecoder->InitVideoDecoder(AV_CODEC_ID_HEVC, buf, bufsize,
                                                   AV_PIX_FMT_BGR32);
                        mIsH265 = true;
                    } else {
                        mDecoder->InitVideoDecoder(AV_CODEC_ID_H264, buf, bufsize,
                                                   AV_PIX_FMT_BGR32);
                        mIsH265 = false;
                    }
                } else {
                    if (strcasecmp(_enc, H265_Enc) == 0) {
                        mDecoder->InitVideoDecoder(AV_CODEC_ID_HEVC, buf, bufsize,
                                                   AV_PIX_FMT_YUV420P);
                        mIsH265 = true;
                    } else {
                        mDecoder->InitVideoDecoder(AV_CODEC_ID_H264, buf, bufsize,
                                                   AV_PIX_FMT_YUV420P);
                        mIsH265 = false;
                    }
                }
                //JACONNECT_INFO("KP2PL SoftDecoder Init Decoder ..........");

            }
            rt = -1;
            LOGD("zasko0------ptype：%d", ptype);
            if (ptype) {
                struct timeval *tv_old;
                struct timeval *tv_new;
                tv_old = (timeval *) malloc(sizeof(timeval));
                tv_new = (timeval *) malloc(sizeof(timeval));
                gettimeofday(tv_old, NULL);
                rt = mDecoder->DecodeVideo(buf, bufsize);
                gettimeofday(tv_new, NULL);
                //	DecoderLock();
                delayTime = (int) (tv_new->tv_usec / 1000 - tv_old->tv_usec / 1000);
                //	DecoderUnLock();
                //	JACONNECT_INFO("delayTime:%d", delayTime);
                free(tv_old);
                free(tv_new);
                mVideoPts = ptimestamp; /* video pts */
            } else {
#ifdef _MSC_VER
                Sleep(1);
#else
                usleep(1000);
#endif
                continue;
            }
#ifdef __ANDROID__
            if (mDecoder->isDirectTexture) {
                if (!rt && mCallback->OnDirectTextureFrameUpdata)
                    mCallback->OnDirectTextureFrameUpdata(this,
                                                          mDecoder->mGraphicBuffer->getNativeBuffer(),
                                                          mCallback->ctx);
            }
#endif // __ANDROID__
            struct timeval *tv_old_frame;
            struct timeval *tv_new_frame;
            tv_old_frame = (timeval *) malloc(sizeof(timeval));
            tv_new_frame = (timeval *) malloc(sizeof(timeval));
            gettimeofday(tv_old_frame, NULL);
            if (!rt && mCallback->OnFrame && !mDecoder->isDirectTexture)
                //JACONNECT_INFO("KP2PL OnFrame ..........");
                mCallback->OnFrame(this, mDecoder->GetVideoBuffer(), mDecoder->GetVideoBufferSize(),
                                   ptype, mCallback->ctx);
//            JACONNECT_INFO(
//                    "-----------------decodevideo------------rt:%d,mCallback->OnFrame:%lu,isDirectTexture:%d",
//                    rt, mCallback->OnFrame, mDecoder->isDirectTexture);
            if (abs(mFirstFrameTimeStimp - ptimestamp) > 20) {
                mPrevMonoUsec = 0;
                if (mCallback && mCallback->OnStatus) {
                    LOGV("onDeviceDataCallBack NET_STATUS_FIRST_FRAME:%d", mChannelNo);
                    mCallback->OnStatus(this, mCallback->ctx, NET_STATUS_FIRST_FRAME, 0);
                }
            }
            mFirstFrameTimeStimp = ptimestamp;
            gettimeofday(tv_new_frame, NULL);
            delayTime1 = (int) (tv_new_frame->tv_usec / 1000 - tv_old_frame->tv_usec / 1000);
/*            DecoderLock();*/
            if (needOpen64 < (delayTime + delayTime1))
                needOpen64 = delayTime + delayTime1;
/*            DecoderUnLock();
            //	JACONNECT_INFO("needOpen64:%d", needOpen64);*/
            free(tv_old_frame);
            free(tv_new_frame);

            if (!rt && mCallback->OnYUVFrame)
                mCallback->OnYUVFrame(this, mDecoder->GetVideoBufferYUV(0),
                                      mDecoder->GetVideoBufferYUV(1),
                                      mDecoder->GetVideoBufferYUV(2),
                                      mDecoder->GetVideoBufferSize(), ptype, mCallback->ctx);

            if (!rt && mCallback->OnOSDFrame) {

#ifdef _MSC_VER
                int64_t lgmtime;
#else
                long lgmtime;
#endif

//#ifdef __ANDROID__
//				long limit_time = 946656000L;
//#else
                long long limit_time = 946656000LL;
//#endif
                if (mRealStatus == NET_PLAY_RECORD) {
                    lgmtime = playtime;
                } else {
//#ifdef __ANDROID__
//					lgmtime = ptimestamp;
//#else
                    lgmtime = ptimestamp / 1000;
//#endif
                }

                char *txtbuf = (char *) malloc(40);
                memset(txtbuf, 0, 40);
                struct tm *t = gmtime((const time_t *) &lgmtime);     ////// 当地时间

                if (abs(mTimeZone) > 12) {
                    sprintf(txtbuf, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", t->tm_year + 1900,
                            t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                } else {
                    int lhour = (int) mTimeZone;
                    int lmin = (int) ((mTimeZone - lhour) * 100);
                    int lOffsetMs = (lhour * 3600 + lmin * 60);
                    lgmtime += lOffsetMs;
                    struct tm *utc_t = gmtime((const time_t *) &lgmtime);    ////// GMT 时间
                    sprintf(txtbuf, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d", utc_t->tm_year + 1900,
                            utc_t->tm_mon + 1, utc_t->tm_mday, utc_t->tm_hour, utc_t->tm_min,
                            utc_t->tm_sec);
                    printf(">>>>>> 修正后的时间:%s\n", txtbuf);
                }

                if (lgmtime > limit_time) {
                    mCallback->OnOSDFrame(this, (unsigned char *) txtbuf, 20, mCallback->ctx,
                                          ptimestamp);
                } else {
                    mCallback->OnOSDFrame(this, NULL, 20, mCallback->ctx, ptimestamp);
                }
                free(txtbuf);
            }

            if (!rt && mCallback->OnYUVOSDFrame && !mDecoder->WallMode())
                mCallback->OnYUVOSDFrame(this, mDecoder->GetOSDBufferYUV(0),
                                         mDecoder->GetOSDBufferYUV(1), mDecoder->GetOSDBufferYUV(2),
                                         mDecoder->GetOSDBufferSize(), mCallback->ctx);

            unlockRead(mBuffer, mBufferReader);
            int delay_;
            unsigned int framedelay;
            if (lasttime == 0)
                framedelay = perFrametime;
            else
                framedelay = ptimestamp - lasttime;

            if (framedelay > perFrametime && mRealStatus != NET_PLAY_RECORD)
                framedelay = perFrametime;
            if (framedelay > 1000 && mRealStatus == NET_PLAY_RECORD)
                framedelay = perFrametime;
            lasttime = ptimestamp;
            uint64_t tmptime = clock_ms();
            int realydelay = framedelay - (tmptime - curtime) - 5;
            //printf("tmptime:%lu  realydelay:%d\n",tmptime,realydelay);
            if ((clock_ms() * 1000 - lasttime > 200) && mRealStatus != NET_PLAY_RECORD) {
                mPrevMonoUsec = 0;
                //JACONNECT_INFO("^^^^^^^^^^^^^^^^^^^^^^^^^^^!!!!!!!!!!!!!!");
            }
            lasttime = clock_ms();
            JACONNECT *lcon = (JACONNECT *) mCallback->ctx;
            //JACONNECT_INFO(
            //        "------------------------------------->%lld  index:%d current time:%ld sleep:%d perFrametime:%d",
            //        ptimestamp, lcon->index, clock_ms(), realydelay,perFrametime);
            //if(mLastTimestamp==ptimestamp)
//            {
//                if (realydelay > 0)
//#ifdef _MSC_VER
//		    if (mRealStatus == NET_PLAY_RECORD)
//				Sleep(realydelay);
//#else
//            if(mRealStatus==NET_PLAY_RECORD)
//              usleep(realydelay * 1000);
//#endif
//            }
//            else
//                preRender(ptimestamp);

            if ((clock_ms() * 1000 - lasttime > 200) && mRealStatus != NET_PLAY_RECORD)
                mPrevMonoUsec = 0;
            lasttime = clock_ms();
            //printf("decoderUnLock------%d\n",__LINE__);

            //printf("ptimestamp:%llu\n",ptimestamp);
            if (mRealStatus == NET_PLAY_RECORD) {
                if (realydelay > 0)
                    usleep(realydelay * 1000);
            } else
                preRender(ptimestamp/* * 1000*/);
            //usleep(60000);

            curtime = clock_ms();
            mLastTimestamp = ptimestamp;
        } else {
#ifdef _MSC_VER
            Sleep(1);
#else
            usleep(1000);
#endif
        }
        //LOGD2("-----------------------HandleDecodeThread end...........................");

    }
    pthread_exit(NULL);
    JACONNECT_INFO("decode thread.........EXIT....\n");
}

void JAConnect::HandleTaskThread() {
    JAConnect_Command cmd;
    int ret;
    int cmdcount;
    while (1) {
        memset(&cmd, 0, sizeof(cmd));
        cmd.commandNo = -1;
        TaskLock();
        if (mCommand.length() > 0) {
            cmd = mCommand.at(0);
            mCommand.remove(0);
        }
        cmdcount = mCommand.length();
        TaskUnLock();
        if (cmd.commandNo > 0) {
            switch (cmd.commandNo) {
                case CMD_CONNECT:
                    ret = DoConnect(&cmd);
                    break;
                case CMD_OPENCHANNEL:
                    ret = DoOpenChannel(&cmd, 1);
                    if (ret) {
                        ret = ReConnect();
//                        if(!ret)
//                            ret = DoOpenChannel(&cmd, 1);
                    }
                    break;
                case CMD_CLOSECHANNEL:
                    ret = DoOpenChannel(&cmd, 0);
                    break;
                case CMD_PTZSTART:
                    ret = DoStartPtzAction(&cmd);
                    break;
                case CMD_PTZSTOP:
                    ret = DoStopPtzAction(&cmd);
                    break;
                case CMD_SEARCHREC:
                    ret = DoSearchRec(&cmd);
                    break;
                case CMD_STARTPLAYBACK:
                    ret = DoStartPlayback(&cmd);
                    break;
                case CMD_STOPPLAYBACK:
                    ret = DoStopPlayback(&cmd);
                    break;
                case CMD_DISCONNECT:
                    DoDisconnect(&cmd);
                    break;
                case CMD_PLAYBACK_PAUSE:
                    DoPlaybackPauseGetData(&cmd);
                    break;
                case CMD_PLAYBACK_CONTINUE:
                    DoPlaybackContinueGetdata(&cmd);
                    break;
                case CMD_DESTROY_VCON:
                    //ja_p2p_vcon_destroy(mDataHandle);
//                    mP2p->vconDestroy((long) mDataHandle);
//                    mDataHandle = NULL;
                    break;
                case CMD_SENDDATA:
                    DoSendData(&cmd);
                    LOGV("JAConnect::DoOpenChannel--------DoSendData");
                    break;
                case CMD_CALL:
                    ret = DoCall(&cmd);
                    break;
                case CMD_HANG_UP:
                    ret = DoHangUp(&cmd);
                    break;
                case CMD_SEND_AUDIO:
                    ret = DoSendAudioPacket(&cmd);
                    break;
                case CMD_SEARCH_DEVICE:
                    ret = DoSearchDevice(&cmd);
                    break;
                case CMD_PULL_ALARMMSG:
                    ret = DoPullAlarmmsg(&cmd);
                    break;
            }
            JACONNECT_INFO("KP2PL [JACONNECT]----------------------------- command:%d,ret:%d\n",
                           cmd.commandNo, ret);
        }
        if (cmd.datasize > 0)
            free(cmd.data);
#ifdef _MSC_VER
        if (cmd.commandNo != CMD_SEND_AUDIO)
            Sleep(10);
#else
        if (cmd.commandNo != CMD_SEND_AUDIO)
            usleep(10000);
#endif
        //		if (cmd.datasize > 0)
        //			free(cmd.data);
        if (!mRunThread && !cmdcount)
            break;
    }
    int index = gConnector.search(this);

    printf("task thread exit.................................................%d\n", index);
    pthread_exit(NULL);

}

int JAConnect::DoOpenChannel(JAConnect_Command *cmd, int bOpen) {
    //    if(!mHandle)
    //        return -1;
    if (!mP2p)
        return -2;
    ResetMediaBuffer(mBuffer);
    if (bOpen) {
        mRecvIFrame = false;
        mAddBuffer = true;
        //LOGD2("-----DecoderResume----");
    } else {
        DecoderPause();
        mAddBuffer = false;
        //LOGD2("-----DecoderPause----");
        //	ResetMediaBuffer(mBuffer);
    }
    mChannelNo = cmd->channelNo;
    mStreamNo = cmd->streamNo;
    mHasSetP720 = 0;
    if (!mP2p) {
        return -1;
    }
    LOGV("JAConnect::DoOpenChannel,this,channelNo:%d,\tStreamID:%d,\tbOpen=%d", cmd->channelNo,
         mStreamNo,
         bOpen);
    int ret = -1;
    //if (mIsConnected)
    //ret = ja_p2p_open_channel(mHandle, cmd->channelNo, cmd->streamNo, bOpen);
    ret = mP2p->openChannel(cmd->channelNo, cmd->streamNo, bOpen, this);
    LOGD("JAConnect::DoOpenChannel: channelNo ----->%d,\tp2p--->%d,\tbOpen：---->%d",
         cmd->channelNo,
         ret, bOpen);
    if (!ret) {
        if (bOpen) {
            mRealStatus = NET_PLAY_REAL;
            DecoderResume();
        } else {
            mRealStatus = NET_PLAY_NONE;
        }
    }
    if (bOpen)
        DoStatus(NET_STATUS_OPEN_VIDEO, 0);
    else
        DoStatus(NET_STATUS_CLOSE_VIDEO, 0);
    return ret;
}

void JAConnect::DoStatus(int status_code, int content_code) {
    if (status_code == NET_STATUS_DISCONNECTED) {
        mIsConnected = false;
        mRealStatus = NET_PLAY_NONE;
    }
    if (status_code == NET_STATUS_DISCONNECTED) {
        DecoderPause();
        ResetMediaBuffer(mBuffer);
        GetDecoder()->ResetData();
        DoDisconnect();
        mAddBuffer = false;
        mIsConnected = false;
        mRealStatus = NET_PLAY_NONE;
//        mHandle = 0;
//        mPlaybackHandle = NULL;
    }
    if (mCallback && mCallback->OnStatus) {
        mCallback->OnStatus(this, mCallback->ctx, status_code, content_code);
    }

}


void JAConnect::DoOnVconData(char *buf, int len) {
    if (mCallback && mCallback->OnVconData)
        mCallback->OnVconData(this, buf, len, mCallback->ctx);
    DestroyVcon();
}

void JAConnect::DoSearchDevice(SearchDeviceResult *pResult) {
    if (mCallback && mCallback->OnSearchDevice)
        mCallback->OnSearchDevice(this, pResult, mCallback->ctx);
}

void JAConnect::DoDisconnectThread() {
    JAConnect_Command cmd;
    memset(&cmd, 0, sizeof(JAConnect_Command));
    DoDisconnect(&cmd);
    pthread_exit(NULL);

}

void JAConnect::DoDisconnect() {
    JACONNECT_INFO("the disconnect thread should run....");
    pthread_create(&mTaskDisconnectThread, NULL, JAConnect_DisConnect, (void *) this);
    //pthread_join(mTaskDisconnectThread,NULL);
}

void JAConnect::DoDisconnect(JAConnect_Command *cmd) {
    isRecyceConnect = 0;
    TaskLock();
    for (int i = mCommand.length() - 1; i >= 0; i--) {
        if (mCommand.at(i).commandNo != CMD_CONNECT)
            mCommand.remove(i);
    }
    TaskUnLock();
    //if (!mIsConnected) {
    //	DoStatus(NET_STATUS_CLOSED, 0);
    //	return;
    //}
    if (mDataHandle) {
        mP2p->vconDestroy(mDataHandle);
        mDataHandle = NULL;
    }

    if (mPlaybackHandle) {
        //ja_p2p_rec_PlaybackStop(mPlaybackHandle);
        mP2p->playBackStop((long) mPlaybackHandle);
        mPlaybackHandle = NULL;
    }
    if (mHandle && mP2p) {
        if (mP2p->GetP2PVersion() != UDX_P2P) {
            //ja_p2p_close(mHandle);
            //LOGV6("close Kp2p--------------------------^^^^^^^^^^^^^^^");
            //mP2p->closeP2P();
            DestroyConnector();
        } else {
            mP2p->closeP2P();
            delete mP2p;
            mP2p = NULL;
        }
    }
    if (mPlaybackHandle || mHandle)
        DecoderPause();
    GetDecoder()->ResetData();
    mAddBuffer = false;
    mIsConnected = false;
    mRealStatus = NET_PLAY_NONE;
    if (cmd)
        DoStatus(NET_STATUS_CLOSED, 0);

    if (GetDecoder()) {
//        GetDecoder()->SetIsP720Dev(false);
//        GetDecoder()->SetIsP360Dev(false, 0, 0, 0, 0);
    }

    mHandle = 0;
    mPlaybackHandle = NULL;
    if (mHasSetP720) {
        mHasSetP720 = 0;
        free(mDistTortion);
        mDistLen = 0;
    }
    LOGV("KP2P disconnect-----------------------");
}


int JAConnect::DoSearchRec(JAConnect_Command *cmd) {
    int ret = 0;
    //FileRecords *frecs;
    //PlaybackSearchTaskHandle		mSearchRecHandle;
    long frecs;
    long mSearchRecHandle;
    char chn[] = {(char) cmd->channelNo};
    for (int i = mSearchData.length() - 1; i >= 0; i--) {
        stFileRecord *rec = mSearchData.at(i);
        mSearchData.remove(i);
        delete rec;
    }
    if (!mIsConnected) {
        return -1;
    }
#ifdef __APPLE__
    //mSearchRecHandle = ja_p2p_rec_FindFileTaskStart(mHandle, chn, 1, cmd->StartTime-28800, cmd->EndTime-28800, 0xf);
    mSearchRecHandle = mP2p->findFileTaskStart(chn, 1, cmd->StartTime - 28800, cmd->EndTime - 28800, 0xf);
#else
    //mSearchRecHandle = ja_p2p_rec_FindFileTaskStart(mHandle, chn, 1, cmd->StartTime, cmd->EndTime, 0xf);
    mSearchRecHandle = mP2p->findFileTaskStart(chn, 1, cmd->StartTime, cmd->EndTime, 0xf);
#endif
    JACONNECT_INFO("KP2PL doSearchRec : %ld ", mSearchRecHandle);
    if (!mSearchRecHandle)
        return -1;
    while (true) {
        if (!mSearchRecHandle || !mP2p) {
            return -1;
        }
        pjap2p_rec_files recs;
        recs = mP2p->findFileNext(mSearchRecHandle);
        if (recs != NULL) {
            JACONNECT_INFO("KP2PL doSearchRec fileCount:%d", recs->cnt);
            if (recs->cnt == 0)
                break;
            for (int i = 0; i < recs->cnt; i++) {
                if (mCallback && mCallback->OnSearchData)
                    mCallback->OnSearchData(this, recs->records[i].channel, recs->records[i].type,
                                            recs->records[i].begin_time, recs->records[i].end_time,
                                            recs->records[i].quality, 0, mCallback->ctx);
            }
            //delete frecs;
        } else {
            JACONNECT_INFO("KP2PL doSearchRec fileCount:0")
            break;
        }
    }
    //ja_p2p_rec_FindFileTaskClose(&mSearchRecHandle);
    mP2p->findFileTaskClose(mSearchRecHandle);
    if (mCallback && mCallback->OnSearchData)
        mCallback->OnSearchData(this, 0, 0, 0, 0, 0, 1, mCallback->ctx);
    printf("allitems:%d\n", mSearchData.length());
    return ret;
}

int JAConnect::ReConnect() {
    int ret = 0;
    DoStatus(NET_STATUS_CONNECTING, 0);

    LOGV("JAConnect::ReConnect  start connect  ip = %s, port = %d, id = %s -------------", mIP,
         mPort, mDeviceID);
    moutCallback = gJACallbackInit;
    moutCallback.ctx = (void *) this;
    if (GetDecoder()) {
        setFishEyeParameter360(0, 0, 0);
        GetDecoder()->SetIsP360Dev(false, 0, 0, 0, 0);
    }
#ifdef __USE_NEWP2P__
#ifdef __MULTI_CONNECTOR__
    mP2p = new JAP2PConnector2(&moutCallback);
#else
    if (strlen(mDeviceID) > 15)
        mP2p = new JAP2PConnector(&moutCallback);
    else
        mP2p = CreateConnector();
#endif
#else
    mP2p = new JAP2PConnector(&moutCallback);
#endif
    mHandle = mP2p->createP2PHandle(this);
    if (strlen(mVerify) == 0) {
        ret = mP2p->connect3(mDeviceID, mIP, mPort);
        if ((ret == KP2P_ERR_RESOLVE_FAILED && mP2p->GetP2PVersion() == KCP_P2P) ||
            (ret < 0 && strlen(mIP) > 0)) {
#ifdef __USE_NEWP2P__
#ifdef __MULTI_CONNECTOR__
            delete mP2p;
            mP2p = NULL;
#else
            DestroyConnector();
#endif
            mP2p = new JAP2PConnector(&moutCallback);
            mHandle = mP2p->createP2PHandle(NULL);
            LOGV7("JAConnect::use libjnnat1 --->%s", mDeviceID);
            ret = mP2p->connect3(mDeviceID, mIP, mPort);
#endif
        }
    } else {
        ret = mP2p->connect2(mDeviceID, mIP, mPort, mVerify);
        LOGV7("new p2p connect result->%d mVerify=====%s mIP--->%s", ret, mVerify, mIP);
        if ((ret == KP2P_ERR_RESOLVE_FAILED && mP2p->GetP2PVersion() == KCP_P2P) ||
            (ret < 0 && strlen(mIP) > 0)) {

            DestroyConnector();

            mP2p = new JAP2PConnector(&moutCallback);
            mHandle = mP2p->createP2PHandle(NULL);
            LOGV7("JAConnect::use libjnnat1 --->%s  ret ---->%d", mDeviceID, ret);
            ret = mP2p->connect2(mDeviceID, mIP, mPort, mVerify);
            LOGV7("JAConnect::use libjnnat2 --->%s  ret ---->%d", mDeviceID, ret);

        }
    }

    if (strlen(mVerify) == 0 && ret == 0) {
        ret = mP2p->loginP2P(mUser, mPwd);
        LOGV7("login user-----%s %s %d", mUser, mPwd, ret);
    }

    LOGD("JAConnect::ReConnect,2018-04-14ret:%d,mDeviceID:%s", ret, mDeviceID);
    if (ret == 0) {//->成功
        mAddBuffer = true;
        mIsConnected = true;
        mAreadyConnected = true;
        mDataHandle = mP2p->vconCreate("RemoteSetup");
        setConnectNowStates(NET_STATUS_CONNECTED);
        DoStatus(NET_STATUS_OPENING, 0);
    } else if (ret == 1 || ret == KP2P_ERR_TIMEOUT) {//->超时
        DoStatus(NET_STATUS_TIMEOUT, 0);
        setConnectNowStates(NET_STATUS_TIMEOUT);
        DoDisconnect(NULL);
    } else if (ret == 2 || ret == KP2P_ERR_AUTH_FAILED ||
               ret == KP2P_ERR_AUTH2_FAILED) { //->账号/密码错误
        setConnectNowStates(NET_STATUS_AUTHOFAIL);
        DoStatus(NET_STATUS_AUTHOFAIL, 0);
        DoDisconnect(NULL);
    } else {
        DoStatus(NET_STATUS_CONNECTFAIL, 0);
        setConnectNowStates(NET_STATUS_CONNECTFAIL);
        DoDisconnect(NULL);
    }

    return ret;
}


JAP2PManager *JAConnect::CreateConnector() {
    JAP2PManager *lp2p;
    pthread_mutex_lock(&g_listLock);
    for (int i = 0; i < g_ListManager.length(); i++) {
        lp2p = g_ListManager.at(i);
        if (lp2p->GetP2PVersion() == UDX_P2P)
            continue;
        if (lp2p->CompareConnectStr(mDeviceID, mIP, mPort)) {
            lp2p->UpdateUse(true);
            pthread_mutex_unlock(&g_listLock);
            //LOGV8("one connector---------------------------------");
            return lp2p;
        }
    }
    lp2p = new JAP2PConnector2(&moutCallback);
    lp2p->UpdateUse(true);
    lp2p->UpdateProperty(mDeviceID, mIP, mPort);
    LOGV8("new JAP2PConnector2----------");
    pthread_mutex_unlock(&g_listLock);
    return (JAP2PManager *) lp2p;
}

void JAConnect::DestroyConnector() {
    //LOGV8("DestroyConnector............................");
    if (mP2p)
        if (mP2p->GetP2PVersion() == UDX_P2P) {
            mP2p->closeP2P();
            delete mP2p;
            mP2p = NULL;
        } else {
            if (mP2p->GetUse() > 1) {
                mP2p->UpdateUse(false);
                mP2p = NULL;
            } else {
                pthread_mutex_lock(&g_listLock);
                int index = g_ListManager.search(mP2p);
                if (index >= 0)
                    g_ListManager.remove(index);
                pthread_mutex_unlock(&g_listLock);
                mP2p->closeP2P();
                mP2p->UpdateUse(false);
                delete mP2p;
                mP2p = NULL;
                //LOGV8("real delete p2p ....................");
            }
        }
}

int JAConnect::DoConnect(JAConnect_Command *cmd) {
    isRecyceConnect = 1;
    int count = 0;
    int ret;
    if (getConnectNowStates() == NET_STATUS_CONNECTED) {
        DoDisconnect(NULL);
        setConnectNowStates(0);
    }
    ParseConnectString(cmd->ConnectStr, cmd->verifystr);
    ResetMediaBuffer(mBuffer);
    LOGD("JAConnect::DoConnect,2018-04-14,cmd->ConnectStr:%s, cmd->verifystr: %s", cmd->ConnectStr,
         cmd->verifystr);
    return ReConnect();
}

int JAConnect::DoStartPlayback(JAConnect_Command *cmd) {
    int ret = 0;
    mChannelNo = cmd->channelNo;
    char chn[] = {(char) cmd->channelNo};
    if (!mIsConnected || !mP2p) {
        return -1;
    }
    mPlaybackHandle = mP2p->playBackStart(chn, 1, cmd->StartTime, cmd->StartTime + 28800, 0xf,
                                          OnRecvRecFrame, (void *) this);
    LOGV2("JAConnect::DoStartPlayback  StartTime = %d", cmd->StartTime);
    if (mPlaybackHandle) {
        mRealStatus = NET_PLAY_RECORD;
        mRecvIFrame = false;
        mAddBuffer = true;
        DecoderResume();
    } else
        ret = -2;
    return ret;
}

int JAConnect::DoStopPlayback(JAConnect_Command *cmd) {
    int ret = 0;

    LOGD("DoStopPlayback: ----->回放关闭状态mPlaybackHandle：%ld", mPlaybackHandle);
    if (mPlaybackHandle && mP2p) {
        //ja_p2p_rec_PlaybackStop(mPlaybackHandle);


        int state = mP2p->playBackStop(mPlaybackHandle);
        LOGD("DoStopPlayback: ----->回放关闭状态state：%d", state);
        mPlaybackHandle = NULL;
        DecoderPause();
        ResetMediaBuffer(mBuffer);
    }
    DoStatus(NET_STATUS_CLOSE_VIDEO, 0);
    return ret;
}

int JAConnect::DoPlaybackContinueGetdata(JAConnect_Command *cmd) {
    int ret = -1;
    if (mPlaybackHandle) {
        //ret = ja_p2p_rec_PlaybackContinue(mPlaybackHandle);
        LOGD("DoPlaybackContinueGetdata,mp2p: %x , mPlaybackHandle:%ld", mP2p, mPlaybackHandle);
        ret = mP2p->playBackContinue(mPlaybackHandle);
    }

    return ret;
}


int JAConnect::DoPlaybackPauseGetData(JAConnect_Command *cmd) {
    int ret = -1;
    if (mPlaybackHandle) {
        LOGD("DoPlaybackPauseGetData,mp2p: %x , mPlaybackHandle:%ld", mP2p, mPlaybackHandle);
        ret = mP2p->playBackPause(mPlaybackHandle);
    }
    //ret = ja_p2p_rec_PlaybackPause(mPlaybackHandle);

    return ret;
}


int JAConnect::DoSendData(JAConnect_Command *cmd) {
    int ret = -1;
    if (!mIsConnected)
        return -1;
    LOGV("KP2PL DoSendData RemoteSetup-->%s", cmd->data);
    if (mP2p == NULL) {
        return -1;
    }
    ret = mP2p->vconSend(mDataHandle, cmd->data, cmd->datasize);
    LOGV("DoSendData-->%d", ret);
    return ret;
}

int JAConnect::DoStartPtzAction(JAConnect_Command *cmd) {
    int ret = -1;
    if (!mIsConnected)
        return -1;
    //ret = ja_p2p_ptz_ctrl(mHandle, cmd->channelNo, cmd->ptzCmd, 1, 1, 0);
    ret = mP2p->ptzCtrl(cmd->channelNo, cmd->ptzCmd, 1, 1, 0);
    return ret;
}

int JAConnect::DoStopPtzAction(JAConnect_Command *cmd) {
    int ret = -1;
    if (!mIsConnected)
        return -1;
    //ret = ja_p2p_ptz_ctrl(mHandle, cmd->channelNo, cmd->ptzCmd, 0, 1, 0);
    ret = mP2p->ptzCtrl(cmd->channelNo, cmd->ptzCmd, 0, 1, 0);
    return ret;
}

int JAConnect::DoCall(JAConnect_Command *cmd) {
    int ret = -1;
    if (!mIsConnected)
        return -1;
    //ret = ja_p2p_vop2p_call(mHandle, cmd->channelNo);
    ret = mP2p->call(cmd->channelNo);
    return ret;
}

int JAConnect::DoHangUp(JAConnect_Command *cmd) {
    int ret = -1;
    if (!mIsConnected)
        return -1;
    //ret = ja_p2p_vop2p_hangup(mHandle);
    ret = mP2p->hangup();
    return ret;
}

//音频对讲
int JAConnect::DoSendAudioPacket(JAConnect_Command *cmd) {
    int ret = -1;
    if (!mIsConnected/* || !mIsAudioCall*/)
        return -1;
    //	ret = ja_p2p_vop2p_talk_send(mHandle,
    ret = mP2p->talkSend(
            cmd->paudioPacket->voiceData,
            cmd->paudioPacket->dataSz,
            cmd->paudioPacket->tsMs,
            cmd->paudioPacket->enc,
            cmd->paudioPacket->samplerate,
            cmd->paudioPacket->samplewidth,
            cmd->paudioPacket->channels,
            cmd->paudioPacket->compressRatio);
    return ret;
}

//局域网搜索
int JAConnect::DoSearchDevice(JAConnect_Command *cmd) {
    int ret = -1;
    //ret = ja_p2p_search_device(cmd->OnSearchDeviceResult, (void *)this);
    //mP2p->searchDevice(cmd->OnSearchDeviceResult, (void *)this);
    //todo -----------------
    return ret;
}


void JAConnect::ParseConnectString(char *connstr, char *verifystr) {
    memset(mDeviceID, 0, 20);
    memset(mIP, 0, 80);
    memset(mPwd, 0, 40);
    memset(mVerify, 0, 200);

    mPort = 0;
    if (!strchr(connstr, ':'))
        strcpy(mDeviceID, connstr);
    else {
        char *lip = strtok(connstr, ":");
        strcpy(mIP, lip);
        lip = strtok(NULL, ":");
        mPort = atoi(lip);
    }
    if (!strchr(verifystr, ':'))
        strcpy(mVerify, verifystr);
    else {
        char *luser = strtok(verifystr, ":");
        strcpy(mUser, luser);
        luser = strtok(NULL, ":");
        if (luser)
            strcpy(mPwd, luser);
    }
}

JAConnect::JAConnect(PJAConnectInit pCallback) {
    connectNowStates = 0;
    onlyDecodeIFrame = false;
    needOpen64 = 0;
    isInstallModeCome = 3;
    howBirate = 2;
    mIsH265 = false;
    mCallback = pCallback;
    mLastTimestamp = 0;
    mFirstFrameTimeStimp = 0;
    mAreadyConnected = false;

    memset(mDeviceID, 0, sizeof(mDeviceID));
    memset(mUser, 0, sizeof(mUser));
    memset(mPwd, 0, sizeof(mPwd));
    memset(mIP, 0, sizeof(mIP));
    memset(mVerify, 0, sizeof(mVerify));
    mStreamNo = 1;
    mPort = 0;
    mHandle = 0;
    mDecodeStatus = 0;
    mVideoWidth = 0;
    mVideoHeight = 0;
    mHardwareDecoder = false;
    mRunThread = 1;
    mAddBuffer = false;
    mIsConnected = false;
    mIsAudioCall = false;
    mIsRecordStart = false;
    mIsRecord = false;
    mIsDisconnect = false;
    mRealStatus = NET_PLAY_NONE;
    mPlaybackStatus = PLAYBACK_STATUS_NONE;
    mPlaybackHandle = NULL;
    mDataHandle = NULL;
    mRecvIFrame = false;
    mBuffer = InitMediaBuffer(3096000);
    mBufferReader = GetReadIndex(mBuffer);
    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);
    pthread_attr_setstacksize(&attr1, ThreadStackSize);
    pthread_attr_setstacksize(&attr2, ThreadStackSize);
    pthread_mutex_init(&mTaskLock, NULL);
    pthread_mutex_init(&mDecoderLock, NULL);
    pthread_cond_init(&mDecoderCond, NULL);
    pthread_create(&mTaskThread, &attr1, JAConnect_Task, (void *) this);
    pthread_create(&mDecoderThread, &attr2, JAConnect_Decode, (void *) this);
    pthread_attr_destroy(&attr1);
    pthread_attr_destroy(&attr2);
//	pthread_create(&mTaskDisconnectThread, NULL, JAConnect_DisConnect,(void *)this);
    mDecoder = new JAMedia();
    mDecoder->pJAConnect = this;
#ifdef __ANDROID__
    mDecoder->OnNotSupportDirectTexture = OnNotSupportDirectTexture;
#endif
    gConnector.insert(-1, this);
    mVconbuffer = NULL;
    mVconRecvLength = 0;
    audiofp = NULL;
    mQueued = 0;
    mProcessed = 0;
    mDelay = 0;
    mVideoPts = 0;
    mAudioPts = 0;

    mCenterX[0] = 0;
    mCenterX[1] = 0;
    mCenterY[0] = 0;
    mCenterY[1] = 0;
    mRadius[0] = 0;
    mRadius[1] = 0;
    mAngleX[0] = 0;
    mAngleX[1] = 0;
    mAngleY[0] = 0;
    mAngleY[1] = 0;
    mAngleZ[0] = 0;
    mAngleZ[1] = 0;

    mDistLen = 0;
    mDistTortion = NULL;

    mHasSetP720 = 0;

    mDLHandle = NULL;
    gJACallbackInit.ctx = this;
    ////// 时区
    mP2p = NULL;
    mBufferStart = false;
    mTimeZone = 0;
    mFixedFrameDurationUsec = 0;
}

JAConnect::~JAConnect() {
    mRunThread = 0;
    this->DecoderResume();
    JACONNECT_INFO("start destory..............");
#ifndef __APPLE__
    pthread_join(mDecoderThread, NULL);
#endif
    JACONNECT_INFO("end destory..............");
    int index = gConnector.search(this);
    if (index >= 0)
        gConnector.remove(index);
    pthread_join(mTaskThread, NULL);
    ReleaseBuffer(mBuffer);

    /*if (pFishEyeParam)
        free(pFishEyeParam);
    if (pDistortion_num)
        free(pDistortion_num);*/
    delete mDecoder;

    if (mCallback) {
        mCallback->OnStatus = NULL;
        mCallback->OnSearchData = NULL;
        mCallback->OnOpenChannel = NULL;
        mCallback->PlaybackTimeUpdate = NULL;
    }
    free(mCallback);
    mCallback = NULL;
}

void JAConnect::AddCommand(int cmdno,
                           char *connstr,
                           char *verifystr,
                           long starttime,
                           long endtime,
                           int channelno,
                           int streamno,
                           int ptzcmd,
                           char *data,
                           int datasize,
                           int alarmmsg_type,
                           time_t ticket,
                           PJAConnect_Audio_Packet audio,
                           fOnSearchDeviceResult OnSearchDeviceResult) {
    JAConnect_Command cmd;
    memset(&cmd, 0, sizeof(cmd));
    TaskLock();
    cmd.commandNo = cmdno;
    if (connstr)
        strcpy(cmd.ConnectStr, connstr);
    if (verifystr)
        strcpy(cmd.verifystr, verifystr);
    cmd.StartTime = starttime;
    cmd.EndTime = endtime;
    cmd.channelNo = channelno;
    cmd.streamNo = streamno;
    cmd.ptzCmd = ptzcmd;
    cmd.alarmmsg_type = alarmmsg_type;
    cmd.ticket = ticket;
    if (datasize > 0) {
        cmd.data = (unsigned char *) malloc(datasize);
        memcpy(cmd.data, data, datasize);
        cmd.datasize = datasize;
    }
    if (audio != NULL) {
        cmd.paudioPacket = audio;
    }
    if (OnSearchDeviceResult != NULL) {
        cmd.OnSearchDeviceResult = OnSearchDeviceResult;
    }
    mCommand.insert(-1, cmd);
    TaskUnLock();

}

void JAConnect::Connect(char *connectstr, char *verifystr) {
//	if (mIsConnected)
//		Disconnect();
    Connect(connectstr, verifystr, mChannelNo);
}

void JAConnect::Connect(char *connectstr, char *verifystr, int channel) {
    LOGV("JAConnect::DoConnect   Connect ");
    mChannelNo = channel;
    if (strcmp(connectstr, connectStr) != 0) {
        memset(connectStr, 0, 80);
    }
    if (strcmp(verifystr, verifyStr) != 0) {
        memset(verifyStr, 0, 100);
    }
    strcpy(connectStr, connectstr);
    strcpy(verifyStr, verifystr);
    AddCommand(CMD_CONNECT, connectstr, verifystr, 0, 0, 0, 0, 0, NULL, 0, 0, 0, NULL, NULL);
}


void JAConnect::OpenChannel(int channelNo, int StreamNo) {
    if (mRealStatus == NET_PLAY_RECORD)
        StopPlayback(mChannelNo);
    mChannelNo = channelNo;
    mStreamNo = StreamNo;
    JACONNECT_INFO("this channelNo:%d", channelNo);
    AddCommand(CMD_OPENCHANNEL, NULL, NULL, 0, 0, channelNo, StreamNo, 0, NULL, 0, 0, 0, NULL,
               NULL);
}

void JAConnect::CloseChannel(int channelNo, int StreamNo) {
    mAddBuffer = false;
//    JAConnect_Command cmd;
//    memset(&cmd, 0, sizeof(cmd));
//    cmd.channelNo = channelNo;
//    cmd.streamNo = StreamNo;
//    DoOpenChannel(&cmd,0);
    AddCommand(CMD_CLOSECHANNEL, NULL, NULL, 0, 0, channelNo, StreamNo, 0, NULL, 0, 0, 0, NULL,
               NULL);

}

void JAConnect::OpenDataChannel() {
    AddCommand(CMD_OPENDATACHANNEL, NULL, NULL, 0, 0, 0, 0, 0, NULL, 0, 0, 0, NULL, NULL);
}

void JAConnect::CloseDataChannel() {
    AddCommand(CMD_CLOSEDATACHANNEL, NULL, NULL, 0, 0, 0, 0, 0, NULL, 0, 0, 0, NULL, NULL);
}

void JAConnect::SendData(char *data, int datasize) {
    AddCommand(CMD_SENDDATA, NULL, NULL, 0, 0, 0, 0, 0, (char *) data, datasize, 0, 0, NULL, NULL);
}

void JAConnect::SearchRec(long pStartTime, long pEndTime, int channelNo) {
    LOGV("JAConnect:--->%ld,%ld:", pStartTime, pEndTime);
    AddCommand(CMD_SEARCHREC, NULL, NULL, pStartTime, pEndTime, channelNo, 0, 0, NULL, 0, 0, 0,
               NULL, NULL);
}

void JAConnect::StartPlayback(long pStartTime, int channelNo) {
    if (!mIsConnected) {
        return;
    }
    if (mRealStatus == NET_PLAY_REAL)
        CloseChannel(mChannelNo, 0);
    if (mRealStatus == NET_PLAY_RECORD)
        StopPlayback(mChannelNo);
    mPlaybackStatus = PLAYBACK_STATUS_GETDATA;
    AddCommand(CMD_STARTPLAYBACK, NULL, NULL, pStartTime, 0, channelNo, 0, 0, NULL, 0, 0, 0, NULL,
               NULL);
}

void JAConnect::StopPlayback(int channelNo) {
    AddCommand(CMD_STOPPLAYBACK, NULL, NULL, 0, 0, channelNo, 0, 0, NULL, 0, 0, 0, NULL, NULL);
}


void JAConnect::PlaybackPauseGetData(int channelNo) {
    AddCommand(CMD_PLAYBACK_PAUSE, NULL, NULL, 0, 0, channelNo, 0, 0, NULL, 0, 0, 0, NULL, NULL);
}

void JAConnect::PlaybackContinueGetData(int channelNo) {
    AddCommand(CMD_PLAYBACK_CONTINUE, NULL, NULL, 0, 0, channelNo, 0, 0, NULL, 0, 0, 0, NULL, NULL);
}

void JAConnect::CtrlPTZ(int cmd, int channelNo) {
    AddCommand(CMD_PTZSTART, NULL, NULL, 0, 0, channelNo, 0, cmd, NULL, 0, 0, 0, NULL, NULL);
}

void JAConnect::StopPTZ(int channelNo) {
    AddCommand(CMD_PTZSTOP, NULL, NULL, 0, 0, channelNo, 0, JA_PTZ_CMD_STOP, NULL, 0, 0, 0, NULL,
               NULL);
}

void JAConnect::Disconnect() {
    AddCommand(CMD_DISCONNECT, NULL, NULL, 0, 0, 0, 0, 0, NULL, 0, 0, 0, NULL, NULL);
}

int JAConnect::Call(int channelNo) {
//	AddCommand(CMD_CALL, NULL, NULL, 0, 0, channelNo, 0, 0, NULL, 0, NULL,NULL);
    int ret;
    if (!mIsConnected)
        return -1;
    //ret = ja_p2p_vop2p_call(mHandle, channelNo);
    ret = mP2p->call(channelNo);
    return ret;
}

int JAConnect::HangUp() {
    //AddCommand(CMD_HANG_UP, NULL, NULL, 0, 0, 0, 0, 0,NULL, 0,NULL,NULL);
    int ret = -1;
    if (!mIsConnected)
        return -1;
    //ret = ja_p2p_vop2p_hangup(mHandle);
    ret = mP2p->hangup();
    return ret;

}

//语音对讲数据包发送
int JAConnect::SendAudioPacket(void *voiceData,
                               size_t dataSz,
                               JAP2P_UINT64 tsMs,
                               char *enc,
                               int samplerate,
                               int samplewidth,
                               int channels,
                               float compressRatio) {
//	PJAConnect_Audio_Packet paudio;
//	paudio = (PJAConnect_Audio_Packet)malloc(sizeof(JAConnect_Audio_Packet));
//	paudio->voiceData = voiceData;
//	paudio->dataSz = dataSz;
//	paudio->tsMs = tsMs;
//	memset(paudio->enc, 0, 7);
//	strcpy(paudio->enc, enc);
//	paudio->samplerate = samplerate;
//	paudio->samplewidth = samplewidth;
//	paudio->channels = channels;
//	paudio->compressRatio = compressRatio;

//	AddCommand(CMD_SEND_AUDIO, NULL, NULL, 0, 0, 0, 0, 0, NULL, 0, paudio,NULL);
    LOGV("dataSize----voice----->%d", dataSz);
    return mP2p->talkSend(voiceData, dataSz, tsMs, enc, samplerate, samplewidth, channels,
                          compressRatio);

}

void JAConnect::SearchDevice() {
    AddCommand(CMD_SEARCH_DEVICE, NULL, NULL, 0, 0, 0, 0, 0, NULL, 0, 0, 0, NULL,
               OnSearchDeviceResult);
}

//获取目前的码流
int JAConnect::getStreamNo() {
    return mStreamNo;
}

void JAConnect::TaskLock() {
    pthread_mutex_lock(&mTaskLock);
}

void JAConnect::TaskUnLock() {
    pthread_mutex_unlock(&mTaskLock);
}

int JAConnect::GetFPS() {
    return GetCurrentFPS(mBuffer);
}

int JAConnect::GetBitrate() {
    return GetCurrentBitrate(mBuffer);
}

int JAConnect::VideoWidth() {
    if (mHardwareDecoder)
        return mVideoWidth;
    if (mDecoder)
        return mDecoder->VideoWidth();
    else
        return 0;
}

int JAConnect::VideoHeight() {
    if (mHardwareDecoder)
        return mVideoHeight;
    if (mDecoder)
        return mDecoder->VideoHeight();
    else
        return 0;
}

int JAConnect::OSDWidth() {
    if (mDecoder)
        return mDecoder->OSDWidth();
    else
        return 0;
}

int JAConnect::OSDHeight() {
    if (mDecoder)
        return mDecoder->OSDHeight();
    else
        return 0;
}

void JAConnect::DecoderLock() {
    pthread_mutex_lock(&mDecoderLock);
}

void JAConnect::DecoderUnLock() {
    pthread_mutex_unlock(&mDecoderLock);
}

void JAConnect::DecoderPause() {
    if (mDecodeStatus) {
        DecoderLock();
        mDecodeStatus = 0;
        DecoderUnLock();
    }
    //   ResetMediaBuffer(mBuffer);
    //   DoStatus(NET_STATUS_CLOSED,0);
    printf("DecoderPause.....................................\n");
}

void JAConnect::DecoderResume() {
    if (!mDecodeStatus) {
        DecoderLock();
        mDecodeStatus = 1;
        pthread_cond_signal(&mDecoderCond);
        mFirstFrameTimeStimp = 0;
        DecoderUnLock();
    }
    printf("DecoderResume.....................................\n");
}


void JAConnect::DestroyVcon() {
    AddCommand(CMD_DESTROY_VCON, NULL, NULL, 0, 0, 0, 0, 0, NULL, 0, 0, 0, NULL, NULL);
}

JAMedia *JAConnect::GetDecoder() {
    return mDecoder;
}

bool JAConnect::CaptureImage(char *filename, int type) {
    if (!mAddBuffer)
        return false;
    if (!mDecoder)
        return false;
    mDecoder->OnCaptureImage = OnImageCapture;
    mDecoder->CaptureImage(filename, type, this);
    return true;
}

bool JAConnect::CaptureCurImage(char *filename) {
    if (!mAddBuffer)
        return false;
    if (!mDecoder)
        return false;

    //    mDecoder->OnCaptureImage = OnImageCapture;
    mDecoder->CaptureImage(filename, 1, this);
    return true;
}

bool JAConnect::RecordFile(char *filename) {
    if (!mAddBuffer)
        return false;
    if (!mDecoder)
        return false;
    if (!GetCurrentBitrate(mBuffer))
        return false;
    if (!GetCurrentFPS(mBuffer))
        return false;
    JACONNECT_INFO("why can not record 1");
    mIsRecord = true;
    mDecoder->OnRecordVideo = OnVideoRecord;
    int fps = GetCurrentFPS(mBuffer);
    return mDecoder->StartRecord(filename, mIsH265, 15/* fps ? fps : 15*/,
                                 GetCurrentBitrate(mBuffer),
                                 this);
}

void JAConnect::StopRecord() {
    if (!mDecoder)
        return;
    if (!mDecoder->IsRecord())
        return;
    mIsRecord = false;
    mDecoder->StopRecord();
}

int JAConnect::GetChannel() {
    return mChannelNo;
}

bool JAConnect::GetRecordState() {
    if (mIsRecord) {
        JACONNECT_INFO("return laji data 4");
    } else {
        JACONNECT_INFO("return laji data 5");
    }
    return mIsRecord;
}


char *JAConnect::GetDevId() {
    return mDeviceID;
}

char *JAConnect::GetIp() {
    return mIP;
}

void JAConnect::DoCaptureImage(bool success, char *path) {
    if (mCallback && mCallback->OnCaptureImage)
        mCallback->OnCaptureImage(this, success, path, mCallback->ctx);
}

void JAConnect::DoRecordVideo(bool success, char *path) {
    if (mCallback && mCallback->OnRecordVideo)
        mCallback->OnRecordVideo(this, success, path, mCallback->ctx);
}

void JAConnect::PullAlarmmsg(int type, int channel, time_t ticket) {
    AddCommand(CMD_PULL_ALARMMSG, NULL, NULL, 0, 0, channel, 0, 0, NULL, 0, type, ticket, NULL,
               NULL);
}

int JAConnect::DoPullAlarmmsg(JAConnect_Command *cmd) {
    int ret = -1;
    if (!mIsConnected)
        return ret;
    //ret = ja_p2p_pull_alarmmsg(mHandle, cmd->channelNo, cmd->alarmmsg_type, cmd->ticket);
    ret = mP2p->pullAlarm(cmd->alarmmsg_type, cmd->channelNo, cmd->ticket);
    return ret;
}

void JAConnect::DoPullAlarmmsg(struct P2PAlarmMsgData *msgData) {
    if (mCallback && mCallback->OnPullAlarmmsg)
        mCallback->OnPullAlarmmsg(this, msgData, mCallback->ctx);
}

bool JAConnect::GetIsConnected() {
    return mIsConnected;
}

void JAConnect::DoOnAudioData(uint8_t *buf, int buflen) {

    uint8_t *audiobuf;
    audiobuf = (unsigned char *) malloc(16000);
    batch_alaw2linear((short *) audiobuf, buf, buflen); //G711音频解码
    mCallback->OnAudioData(this, (void *) audiobuf, buflen * 2, mCallback->ctx);
    free(audiobuf);
}

//ADTS syncword (0xFFF)
bool checkIsAAC(uint8_t *buf) {
    if (buf == NULL) {
        return false;
    }
    uint8_t byte1 = buf[0];
    uint8_t byte2 = buf[1];
    //uint8_t byte3 = buf[2];
    //uint8_t byte4 = buf[3];

    uint32_t sync = (byte1 << 8) | byte2;

    uint8_t byte3 = buf[2];
    uint32_t sampling_frequency_index = (byte3 >> 2) & 0x0f;

    //uint8_t byte4 = buf[3];
    //uint32_t channel_configuration = ((byte3&0x01)<<2)|((byte4>>5)&0x03);
    if ((sync & 0xfff0) == 0xfff0 &&
        (sampling_frequency_index == 0xb || sampling_frequency_index == 0x8)
        // && ((channel_configuration&0x7)==2)
            ) {
        return true;
    }

    return false;
}

void
JAConnect::DoOnAudioData2(uint8_t *buf, int buflen, bool isAAC, bool isPlayback, bool haveheader) {
    // int sampleRate = 0;
    // int sampleWidth = 0;
    // int lsize = 0;
    // AUDIO_BUF_ATTR_t audioInfo;
    // memcpy((void *)&audioInfo, buf, sizeof(AUDIO_BUF_ATTR_t));
    // sampleRate = audioInfo.samplerate;
    // sampleWidth = audioInfo.samplewidth;
    // lsize = audioInfo.packsize;
    // //LOGV("audioInfo packSize:%d, encode:%s, samplerate:%d, samplewidth:%d  isAAC:%d\n", audioInfo.packsize, audioInfo.encode, audioInfo.samplerate, audioInfo.samplewidth, isAAC);
    // if (isAAC) {
    //     uint8_t *audiobuf = NULL;
    //     int outsize = 0;
    //     //LOGV(">>>>>> bufLen -> %d\n", buflen);
    //     audiobuf = (uint8_t *)malloc(16000);
    //     mDecoder->InitAudioDecoder(AV_CODEC_ID_AAC, sampleRate, 1, sampleWidth, lsize);
    //     mDecoder->DecodeAudio((unsigned char *)((long)buf+sizeof(AUDIO_BUF_ATTR_t)), buflen-sizeof(AUDIO_BUF_ATTR_t), audiobuf, &outsize);
    //     //LOGV(">>>>>> outsize -> %d\n", outsize);
    //     mCallback->OnAudioData(this, (void*)audiobuf, 2048, mCallback->ctx);
    //     free(audiobuf);
    //     audiobuf = NULL;
    // } else {
    //     DoOnAudioData(buf + sizeof(AUDIO_BUF_ATTR_t), buflen-sizeof(AUDIO_BUF_ATTR_t));
    // }

    if (!isPlayback) {
        int sampleRate = 0;
        int sampleWidth = 0;
        int lsize = 0;
        AUDIO_BUF_ATTR_t audioInfo;
        memcpy((void *) &audioInfo, buf, sizeof(AUDIO_BUF_ATTR_t));
        sampleRate = audioInfo.samplerate;
        sampleWidth = audioInfo.samplewidth;
        lsize = audioInfo.packsize;

        //    printf("audioInfo packSize:%d, encode:%s, samplerate:%d, samplewidth:%d\n", audioInfo.packsize, audioInfo.encode, audioInfo.samplerate, audioInfo.samplewidth);
        if (isAAC) {
            uint8_t *audiobuf = NULL;
            int outsize = 0;
            //        printf(">>>>>> bufLen -> %d\n", buflen);
            audiobuf = (uint8_t *) malloc(16000);
            mDecoder->InitAudioDecoder(AV_CODEC_ID_AAC, sampleRate, 1, sampleWidth, lsize);
            if (haveheader)
                mDecoder->DecodeAudio((unsigned char *) ((long) buf + sizeof(AUDIO_BUF_ATTR_t)),
                                      buflen - sizeof(AUDIO_BUF_ATTR_t), audiobuf, &outsize);
            else
                mDecoder->DecodeAudio((unsigned char *) buf,
                                      buflen, audiobuf, &outsize);
            //        printf(">>>>>> outsize -> %d\n", outsize);
            mCallback->OnAudioData(this, (void *) audiobuf, 2048, mCallback->ctx);
            free(audiobuf);
            audiobuf = NULL;
        } else {
            if (buflen == 320 || buflen == 160)
                DoOnAudioData(buf, buflen);
            else
                DoOnAudioData(buf + sizeof(AUDIO_BUF_ATTR_t), buflen - sizeof(AUDIO_BUF_ATTR_t));
        }
    } else {
        /* 绕，回放走转发服，AAC标识被修改为G711的问题
         * //128  //+sizeof(AUDIO_BUF_ATTR_t)=36 ===>320,356,160,196,128,174
         */
        if (!isAAC) {
            isAAC = checkIsAAC(buf);
        }

        if (isAAC) {
            uint8_t *audiobuf = NULL;
            int outsize = 0;
            //        printf(">>>>>> bufLen -> %d\n", buflen);
            audiobuf = (uint8_t *) malloc(16000);
            mDecoder->InitAudioDecoder(AV_CODEC_ID_AAC, 8000, 1, 16, buflen);
            mDecoder->DecodeAudio(buf, buflen, audiobuf, &outsize);
            //        printf(">>>>>> outsize -> %d\n", outsize);
            mCallback->OnAudioData(this, (void *) audiobuf, 2048, mCallback->ctx);
            free(audiobuf);
            audiobuf = NULL;
        } else {
            DoOnAudioData(buf, buflen);
        }
    }
}

#ifdef __ANDROID__

GraphicBuffer *JAConnect::GetGraphicBuffer() {
    return mDecoder->mGraphicBuffer;
}

void JAConnect::DoNotSupportDirectTexture(bool success) {
    if (mCallback && mCallback->OnNotSupportDirectTexture)
        mCallback->OnNotSupportDirectTexture(success, mCallback->ctx);
}

#endif

void JAConnect::UpdateCircleCenter(int radius, int centerx, int centery) {
    if (!mDecoder)
        return;
    DecoderPause();
    TaskLock();
    mDecoder->ResetData();
    mDecoder->UpdateCircleParameter(radius, centerx, centery);
    TaskUnLock();
    DecoderResume();
}

void JAConnect::SetOpanALQueueInfo(int pQueued, int pProcessed) {
    mQueued = pQueued;
    mProcessed = pProcessed;
}

void JAConnect::DoOnOpenChannel(int nChannelId, int nStreamId, const char *camDes) {
    JACONNECT_INFO("[JASON] --DoOnOpenChannel...camDes ： %s", camDes);
    if (mCallback && mCallback->OnOpenChannel)
        mCallback->OnOpenChannel(this, nChannelId, nStreamId, camDes, mCallback->ctx);
}

void JAConnect::doOnDeviceInfo(bool pIsP720) {
    JACONNECT_INFO("[JASON] --DoOnOpenChannel...camDes ： %s", pIsP720 ? "isP720" : "noP720");
    if (mCallback && mCallback->OnDeviceInfo) {
        mCallback->OnDeviceInfo(this, pIsP720, mCallback->ctx);
    }
}

void JAConnect::doOnOOBFrameAvailable(int installMode, int scene) {
    if (mCallback && mCallback->OnOOBFrameAvailable) {
        mCallback->OnOOBFrameAvailable(this, installMode, scene, mCallback->ctx);
    }
}

bool JAConnect::isNeedOpen64Stream() {
    if (!GetFPS())
        return false;
    return (needOpen64 + 40) - (GetFPS() / 1000) >= 0;
}

void JAConnect::CancelTask() {

}

////////////////新增函数///////////////////////////

//配置鱼眼参数
void JAConnect::setFishEyeParameter(float centerX, float centerY, float radius, float angleX,
                                    float angleY, float angleZ, int index) {

    if (index > 1 || index < 0)
        return;

    mCenterX[index] = centerX;
    mCenterY[index] = centerY;
    mRadius[index] = radius;
    mAngleX[index] = angleX;
    mAngleY[index] = angleY;
    mAngleZ[index] = angleZ;

    GetDecoder()->SetIsP720Dev(true);
}

void JAConnect::GetFishEyeParameter(float *centerX, float *centerY, float *radius, float *angleX,
                                    float *angleY, float *angleZ, int index) {

    if (index > 1 || index < 0)
        return;

    *centerX = mCenterX[index];
    *centerY = mCenterY[index];
    *radius = mRadius[index];
    *angleX = mAngleX[index];
    *angleY = mAngleY[index];
    *angleZ = mAngleZ[index];
}

void JAConnect::setDistTortion(void *pDist, int len) {
    if (len >= 8) {
        if (mDistTortion != NULL) {
            free(mDistTortion);
            mDistTortion = NULL;
        }

        mDistTortion = calloc(1, len);
        memcpy(mDistTortion, pDist, len);
        mDistLen = len;
    }
}

void *JAConnect::GetDistTortion(int *len) {
    *len = mDistLen;
    return mDistTortion;
}


bool JAConnect::GetHasSetP720() {
    return mHasSetP720 == DEVICE_TYPE_720 || (mDistLen == 0);
}


void JAConnect::SetHasSetP720(bool pHasSet) {
    if (pHasSet)
        mHasSetP720 = DEVICE_TYPE_720;
    else
        mHasSetP720 = 0;
}

VCON_HANDLE *JAConnect::getDownLoadVconHandle() {
    return (VCON_HANDLE *) mDLHandle;
}


//下载要用的：

int JAConnect::CreateDownLoadVcon() {
    if (mDLHandle) {
        return 0;
    }

    //mDLHandle = ja_p2p_vcon_create(mHandle, JA_DL_VCON_APPNAME);
    mDLHandle = mP2p->vconCreate(JA_DL_VCON_APPNAME);
    if (!mDLHandle) {
        //LOGV("[JAConnect] -CreateDownloadVcon- Fail to create vcon.");
        return -1;
    }
    LOGE("下载准备开始");
    return 0;
}

int JAConnect::OperateDeviceFile(int optype, int opflag, const char *fp) {
    int rt = 0;

    rt = CreateDownLoadVcon();
    //LOGV("JAConnect::OperateDeviceFile  CreateDownLoadVcon");
    if (rt < 0) {
        // error
        return -1;
    }

    int type = optype;
    int flag = opflag;

    LP_JADEVFILEOPERATE fop = (LP_JADEVFILEOPERATE) malloc(sizeof(ST_JADEVFILEOPERATE));
    memset(fop, 0, sizeof(ST_JADEVFILEOPERATE));

    type = htonl(optype);  //转到网络操作
    flag = htonl(opflag);

    fop->opera_type = (uint32_t) type;
    fop->opera_flag = (uint32_t) flag;
    //LOGV("JAConnect::OperateDeviceFile  转到网络操作");

    if (fp) {
        memcpy(fop->fp, fp, sizeof(fop->fp));
    }

    //int result = ja_p2p_vcon_send(mDLHandle, (int)(sizeof(ST_JADEVFILEOPERATE)), (void*)fop);
    int result = mP2p->vconSend(mDLHandle, (void *) fop, (int) (sizeof(ST_JADEVFILEOPERATE)));
    //LOGV("JAConnect::OperateDeviceFile  ja_p2p_vcon_send  result = %d", result);
    free(fop);
    fop = NULL;


    return 0;
}

int JAConnect::DestroyDownloadVcon() {
    if (!mDLHandle)
        return -1;

    //ja_p2p_vcon_destroy(mDLHandle);
    mP2p->vconDestroy(mDLHandle);
    mDLHandle = NULL;

    return 0;
}

void JAConnect::doOnVconFileOperate(void *buf, int len) {

    if (mCallback && mCallback->OnVconFileOperate) {
        mCallback->OnVconFileOperate(this, buf, len, mCallback->ctx);
    }
}

void JAConnect::SetTimeZone(float pTimeZone) {
    printf(">>>>>> [JAConnect] -SetTimeZone- %f\n", pTimeZone / 100);
    mTimeZone = pTimeZone / 100;
}

bool JAConnect::GetHasSetP360() {
    return mHasSetP720 == DEVICE_TYPE_360;
}

void JAConnect::SetHasSetP360(bool pSet) {
    if (pSet)
        mHasSetP720 = DEVICE_TYPE_360;
    else
        mHasSetP720 = 0;
}

void JAConnect::GetFishEyeParameter360(float *centerX, float *centerY, float *radius) {
    GetDecoder()->GetFisheyeParameter(centerX, centerY, radius);
//	*centerX = mCenterX[0];
//	*centerY = mCenterY[0];
//	*radius = mRadius[0];
}

void JAConnect::setFishEyeParameter360(float centerX, float centerY, float radius) {
    mCenterX[0] = centerX;
    mCenterY[0] = centerY;
    mRadius[0] = radius;
    GetDecoder()->SetIsP360Dev(true, centerX, centerY, radius, -1);
}
