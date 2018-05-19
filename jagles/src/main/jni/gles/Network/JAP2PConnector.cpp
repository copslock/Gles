//
//  JAP2PConnector.cpp
//  iOS
//
//  Created by Chan Jason on 2017/5/7.
//  Copyright © 2017年 LiHong. All rights reserved.
//


#include "JAP2PConnector.h"
#include "JAP2PManager.h"
#include <string.h>
#include <list.h>
//#include <HWCodec.h>

#ifdef __ANDROID__

#include <android/log.h>

#define LOGV(...) __android_log_print(ANDROID_LOG_DEBUG, "honglee_1111", __VA_ARGS__)
#endif

extern bool g_InitP2P_V1;

extern TJACallBack g_Callback_V1;

extern pthread_mutex_t g_listLock;
extern List<JAP2PManager *> g_ListManager;


/*连接过程中产生的成功连接事件*/
void OnOldP2pConnect(JAP2P_HANDLE p2p_handle, const char *conn_type_msg) {
    JAP2PManager *mgn = GetP2PConnector((long) p2p_handle);
    if (mgn)
        mgn->GetCallback()->OnConnect(mgn, conn_type_msg, mgn->GetCallback()->ctx);
}

/*断开连接事件,连接失败，及关闭连接的时候会有此事件
 **/
void OnOldP2pDisconnect(JAP2P_HANDLE p2p_handle, int ret) {
    JAP2PManager *mgn = GetP2PConnector((long) p2p_handle);
    if (mgn)
        mgn->GetCallback()->OnDisconnect(mgn, ret, mgn->GetCallback()->ctx);
}

/*
 *p2p会话收到音/视频帧事件，对应帧数据如下
 *音频帧：		AUDIO_BUF_ATTR_t的头 + raw_g711音频
 *视频帧：		h264的裸数据
 *p2p_handle:	对应p2p会话句柄
 *media_type:	0:音频;1:视频
 *frame_type:	0:音频帧;1:关键帧;2:P帧
 *frame:		视频帧数据
 *frame_len:	视频帧长度
 */
void OnOldP2pRecvFrame(
        JAP2P_HANDLE p2p_handle,
        unsigned short media_type,
        unsigned short frame_type,
        void *frame,
        int frame_len
) {
    JAP2PManager *mgn = GetP2PConnector((long) p2p_handle);
    if (mgn)
        mgn->GetCallback()->OnRecvFrame(mgn, media_type, frame_type, frame, frame_len,
                                        mgn->GetCallback()->ctx);
}

void OnOldP2pRecvFrameEx(
        JAP2P_HANDLE p2p_handle,
        unsigned short frame_type,
        void *frame,
        int frame_len,
        JAP2P_UINT64 uiTimeStampMs,
        unsigned int uiWidth,
        unsigned int uiHeight,
        unsigned int uiSamplerate,
        unsigned int uiSamplewidth,
        unsigned int uiChannels,
        char *avEnc
) {
    JAP2PManager *mgn = GetP2PConnector((long) p2p_handle);
    if (mgn)
        mgn->GetCallback()->OnRecvFrameEx(mgn, frame_type, frame, frame_len, uiTimeStampMs, uiWidth,
                                          uiHeight, uiSamplerate, uiSamplewidth, uiChannels, avEnc,
                                          mgn->GetCallback()->ctx);
}


/*
 *p2p 用户校验结果事件
 *ret: [0：成功;1:校验超时;2:用户名密码错误]
 */
void OnOldP2pLoginResult(JAP2P_HANDLE p2p_handle, int ret) {
    JAP2PManager *mgn = GetP2PConnector((long) p2p_handle);
    if (mgn)
        mgn->GetCallback()->OnLoginResult(mgn, ret, mgn->GetCallback()->ctx);
}

/*p2p云台控制结果事件
 */
void OnOldP2pPtzResult(JAP2P_HANDLE p2p_handle, int ret) {
    JAP2PManager *mgn = GetP2PConnector((long) p2p_handle);
    if (mgn)
        mgn->GetCallback()->OnPtzResult(mgn, ret, mgn->GetCallback()->ctx);
}

/*虚拟通道数据接收事件，注意区别于音视频接收事件回调* */
void OnOldP2pVconRecv(JAP2P_HANDLE p2p_handle, void *buf, int len) {
    JAP2PManager *mgn = GetP2PConnector((long) p2p_handle);
    if (mgn)
        mgn->GetCallback()->OnVconRecv(mgn, buf, len, mgn->GetCallback()->ctx);
}

void OnOldP2pVconEvent(JAP2P_HANDLE p2p_handle, char *event) {
    JAP2PManager *mgn = GetP2PConnector((long) p2p_handle);
    if (mgn)
        mgn->GetCallback()->OnVconEvent(mgn, event, mgn->GetCallback()->ctx);
}

void OnOldP2pP2PError(JAP2P_HANDLE p2p_handle, char *error_msg, int len) {
    JAP2PManager *mgn = GetP2PConnector((long) p2p_handle);
    if (mgn)
        mgn->GetCallback()->OnP2PError(mgn, error_msg, len, mgn->GetCallback()->ctx);
}

/**
 * 新增虚拟通道接收事件，只仍出对应vcon_handle,
 * 注意:只需要设置OnVconRecv/OnVconRecv2 中一个回调*
 */
void OnOldP2pVconRecv2(VCON_HANDLE *vcon_handle, void *buf, int len) {
    JAP2PManager *mgn = GetP2PConnector((long) vcon_handle->p2p_handle);
    if (mgn)
        mgn->GetCallback()->OnVconRecv2(mgn, vcon_handle, buf, len, mgn->GetCallback()->ctx);
}


void OnOldP2pConnectReq(JAP2P_HANDLE p2p_handle,
                        int proto_type,
                        int action,
                        unsigned int ip,
                        unsigned short port,
                        unsigned int random) {
    JAP2PManager *mgn = GetP2PConnector((long) p2p_handle);
    if (mgn)
        mgn->GetCallback()->OnConnectReq(mgn, proto_type, action, ip, port, random,
                                         mgn->GetCallback()->ctx);
}

void OnOldP2pOpenChannel(JAP2P_HANDLE p2p_handle,
                         int nChannelId,
                         int nStreamId,
                         const char *camDes) {
    JAP2PManager *mgn = GetP2PConnector((long) p2p_handle);
    if (mgn)
        mgn->GetCallback()->OnOpenChannel(mgn, nChannelId, nStreamId, camDes,
                                          mgn->GetCallback()->ctx);
}

/**
 * 注意有可能一次回调的数据不是完整的一张图或一段视频
 */
void OnOldP2pRecvAlarmMsg(JAP2P_HANDLE p2p_handle,
                          struct P2PAlarmMsgData *msgData) {
    JAP2PManager *mgn = GetP2PConnector((long) p2p_handle);
    if (mgn)
        mgn->GetCallback()->OnRecvAlarmMsg(mgn, msgData, mgn->GetCallback()->ctx);
}

JAP2PConnector::~JAP2PConnector() {
    pthread_mutex_lock(&g_listLock);
    for (int i = 0; i < g_ListManager.length(); i++) {
        if (g_ListManager.at(i) == this) {
            g_ListManager.remove(i);
            break;
        }
    }
    pthread_mutex_unlock(&g_listLock);
}

JAP2PConnector::JAP2PConnector(void *callback) {
    pthread_mutex_lock(&g_listLock);
    if (!g_InitP2P_V1) {
        g_Callback_V1.OnConnect = OnOldP2pConnect;
        g_Callback_V1.OnConnectReq = OnOldP2pConnectReq;
        g_Callback_V1.OnDisconnect = OnOldP2pDisconnect;
        g_Callback_V1.OnLoginResult = OnOldP2pLoginResult;
        g_Callback_V1.OnP2PError = OnOldP2pP2PError;
        g_Callback_V1.OnPtzResult = OnOldP2pPtzResult;
        g_Callback_V1.OnRecvFrameEx = OnOldP2pRecvFrameEx;
        g_Callback_V1.OnVconEvent = OnOldP2pVconEvent;
        g_Callback_V1.OnVconRecv = OnOldP2pVconRecv;
        //g_Callback_V1.OnVconRecv2 = OnOldP2pVconRecv2;//XM下载专属
        g_Callback_V1.OnRecvAlarmMsg = OnOldP2pRecvAlarmMsg;
        g_Callback_V1.OnOpenChannel = OnOldP2pOpenChannel;
        ja_p2p_init(&g_Callback_V1);
        g_InitP2P_V1 = true;
    }


    memset(mDeviceID, 0, 20);
    memset(mIP, 0, 80);
    mPort = 0;
    mP2P_version = UDX_P2P;
    mOutCallback = (PJACallBackManager) calloc(1, sizeof(TJACallBackManager));
    memcpy(mOutCallback, callback, sizeof(TJACallBackManager));
    g_ListManager.insert(-1, this);
    pthread_mutex_unlock(&g_listLock);
}

void JAP2PConnector::setDebugLevel(int level) {
    ja_p2p_set_debug_level(level);
}

long JAP2PConnector::connect(const char *devid, int connType) {
    JAP2P_HANDLE h = ja_p2p_connect(devid, connType);
    if (devid)
        strcpy(mDeviceID, devid);
    mHandle = (void *) h;
    return (long) mHandle;
}

long JAP2PConnector::createP2PHandle(void *ctx) {
    JAP2P_HANDLE h = ja_p2p_create_p2phandle();
    mHandle = (void *) h;
    return (long) mHandle;
}

int JAP2PConnector::connect2(const char *devid, const char *ipaddr, unsigned short port,
                             const char *verify_str) {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return -1;
    if (devid)
        strcpy(mDeviceID, devid);


    if (ipaddr && strlen(ipaddr) <= 80) {
        strcpy(mIP, ipaddr);
        //strcpy();
    } else {
        if (ipaddr) {
            LOGD("AP2PConnector::connect2,ipaddr: %s", ipaddr);
        }
    }


    if (port)
        mPort = port;
    LOGD("JAP2PConnector::connect2  ---->");
    return ja_p2p_connect2(h, devid, ipaddr, port, verify_str);
}

int JAP2PConnector::connect3(const char *devid, const char *ipaddr, unsigned short port) {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return -1;
    if (devid)
        strcpy(mDeviceID, devid);
    if (ipaddr)
        strcpy(mIP, ipaddr);
    if (port)
        mPort = port;
    return ja_p2p_connect3(h, devid, ipaddr, port);
}

const char *JAP2PConnector::getConnectProto() {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return NULL;
    return ja_p2p_GetConnectProto(h);
}

const char *JAP2PConnector::getConnectStepStr(const char *proto_name) {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return NULL;
    return ja_p2p_GetConnectStepStr(h, proto_name);
}

void JAP2PConnector::closeP2P() {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return;
    ja_p2p_close(h);
}

int JAP2PConnector::loginP2P(const char *user, const char *pwd) {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return -1;
    return ja_p2p_login(h, user, pwd);
}

int JAP2PConnector::openChannel(int nChannelId, int nStreamId, int bOpen, void *ctx) {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return -1;
    return ja_p2p_open_channel(h, nChannelId, nStreamId, bOpen);
}

int JAP2PConnector::ptzCtrl(int nChannel, int act, int bStart, int param1, int param2) {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return -1;
    if (bStart == 0) {
        if (JA_PTZ_CMD_STOP != act) {
            act = JA_PTZ_CMD_STOP;
        }
    }
    if (JA_PTZ_CMD_STOP == act) {
        bStart = 0;
    }
    LOGV("JAP2PConnector::ptzCtrl nChannel:%d act:%d,start:%d", nChannel, act, bStart);
    return ja_p2p_ptz_ctrl(h, nChannel, act, bStart, param1, param2);
}

int JAP2PConnector::getChannelCnt() {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return -1;
    return ja_p2p_getChannelCount(h);
}

long JAP2PConnector::vconCreate(const char *appName) {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return -1;
    VCON_HANDLE *sh = ja_p2p_vcon_create(h, appName);
    return (long) sh;
}

int JAP2PConnector::vconSend(long vcon_handle, void *buf, int buf_len) {
    VCON_HANDLE *h = (VCON_HANDLE *) vcon_handle;
    if (!h)
        return -2;
    return ja_p2p_vcon_send(h, buf_len, buf);
}

int JAP2PConnector::vconDestroy(long vcon_handle) {
    VCON_HANDLE *h = (VCON_HANDLE *) vcon_handle;
    if (!h)
        return -2;
    return ja_p2p_vcon_destroy(h);
}

long JAP2PConnector::findFileTaskStart(char *chn, int chn_count, int recfile_starttime,
                                       int recfile_endtime, int recfile_type) {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return -1;
    PlaybackSearchTaskHandle sh = ja_p2p_rec_FindFileTaskStart(h, chn, chn_count, recfile_starttime,
                                                               recfile_endtime, recfile_type);
    return (long) sh;
}

int JAP2PConnector::findFileCnt(long findFileTask_handle) {
    PlaybackSearchTaskHandle sh = (PlaybackSearchTaskHandle) findFileTask_handle;
    if (!sh)
        return -2;
    return ja_p2p_rec_FindFileCount(sh);
}

pjap2p_rec_files JAP2PConnector::findFileNext(long findFileTask_handle) {
    PlaybackSearchTaskHandle sh = (PlaybackSearchTaskHandle) findFileTask_handle;
    if (!sh)
        return NULL;

    stFileRecords *recs = ja_p2p_rec_FindFileNext(sh);
    memset(&recFiles, 0, sizeof(recFiles));
    recFiles.cnt = recs->records_cnt;
    for (int i = 0; i < recs->records_cnt; i++) {
        //memcpy(&recFiles.records[i], &recs->records[i], sizeof(stFileRecord));
        recFiles.records[i].type = recs->records[i].type;
        recFiles.records[i].begin_time = recs->records[i].starttime;
        recFiles.records[i].end_time = recs->records[i].endtime;
        recFiles.records[i].channel = recs->records[i].chn;
        recFiles.records[i].quality = 0;
    }
    //return (long)recs;
    return &recFiles;
}

int JAP2PConnector::findFileTaskClose(long findFileTask_handle) {
    PlaybackSearchTaskHandle sh = (PlaybackSearchTaskHandle) findFileTask_handle;
    if (!sh)
        return -2;
    return ja_p2p_rec_FindFileTaskClose(&sh);
}

long
JAP2PConnector::playBackStart(char *chn, int chn_count, int recfile_starttime, int recfile_endtime,
                              int recfile_type, fOnRecvRecFrame OnRecvRecFrame, void *ctx) {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return -1;
    PlaybackHandle ph = ja_p2p_rec_PlaybackStart(h, chn, chn_count, recfile_starttime,
                                                 recfile_endtime, recfile_type, OnRecvRecFrame,
                                                 ctx);
    return (long) ph;
}

int JAP2PConnector::playBackPause(long playBack_handle) {
    PlaybackHandle ph = (PlaybackHandle) playBack_handle;
    if (!ph)
        return -2;
    return ja_p2p_rec_PlaybackPause(ph);
}

int JAP2PConnector::playBackContinue(long playBack_handle) {
    PlaybackHandle ph = (PlaybackHandle) playBack_handle;
    if (!ph)
        return -2;
    return ja_p2p_rec_PlaybackContinue(ph);
}

int JAP2PConnector::playBackSeek(long playBack_handle, int recfile_starttime, int recfile_endtime,
                                 int recfile_type) {
    PlaybackHandle ph = (PlaybackHandle) playBack_handle;
    if (!ph)
        return -2;
    return ja_p2p_rec_PlaybackSeek(ph, recfile_starttime, recfile_endtime, recfile_type);
}

int JAP2PConnector::playBackStop(long playBack_handle) {
    PlaybackHandle ph = (PlaybackHandle) playBack_handle;
    if (!ph)
        return -2;
    return ja_p2p_rec_PlaybackStop(ph);
}

void jaP2PConnector_OnSearchDeviceResult(struct SearchDeviceResult *pResult, void *ctx) {
    JAP2PConnector *c = (JAP2PConnector *) ctx;
    if (!c)
        return;
    c->doOnSearchResult(pResult);
}

int JAP2PConnector::searchDevice(OnJAP2PSearchDeviceResult on, void *ctx) {
    mOnSearchDeviceResult = on;
    mSearchCtx = ctx;
    fOnSearchDeviceResult result = jaP2PConnector_OnSearchDeviceResult;
    return ja_p2p_search_device(result, this);
}

void JAP2PConnector::doOnSearchResult(struct SearchDeviceResult *pResult) {
    JAP2PSearchDeviceResult result;
    memset((void *) &result, 0, sizeof(JAP2PSearchDeviceResult));
//    memcpy((void *)result.esee_id, pResult->esee_id, 32);
//    memcpy((void *)result.ip, pResult->ip, 32);
//    result.port = pResult->port;
//    result.max_ch = pResult->max_ch;
//    memcpy((void *)result.dev_model, pResult->dev_model, 32);
    memcpy((void *) &result, (const void *) pResult, sizeof(JAP2PSearchDeviceResult));
    if (mOnSearchDeviceResult)
        mOnSearchDeviceResult(&result, mSearchCtx);
}

int JAP2PConnector::call(int chn) {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return -1;
    return ja_p2p_vop2p_call(h, chn);
}

int JAP2PConnector::talkSend(void *voiceData, size_t dataSz, JAP2P_UINT64 tsMs, const char *enc,
                             int samplerate, int samplewidth, int channels, float compressRatio) {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return -1;
    return ja_p2p_vop2p_talk_send(h, voiceData, dataSz, tsMs, enc, samplerate, samplewidth,
                                  channels, compressRatio);
}

int JAP2PConnector::hangup() {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return -1;
    return ja_p2p_vop2p_hangup(h);
}

int JAP2PConnector::pullAlarm(int type, int ch, time_t ticket) {
    JAP2P_HANDLE h = (JAP2P_HANDLE) mHandle;
    if (!h)
        return -1;
    return ja_p2p_pull_alarmmsg(h, type, ch, ticket);
}

int JAP2PConnector::setClientType(int type) {
    return ja_p2p_set_clienttype(type);
}

int JAP2PConnector::setClientInfoType(const char *clientType) {
    return ja_p2p_set_clientinfo_type(clientType);
}

int JAP2PConnector::setClientInfoImei(const char *imei) {
    return ja_p2p_set_clientinfo_imei(imei);
}

int JAP2PConnector::setClientInfoIsp(const char *isp) {
    return ja_p2p_set_clientinfo_isp(isp);
}

int JAP2PConnector::setClientInfoBundleID(const char *bundle_id) {
    return ja_p2p_set_clientinfo_bundleid(bundle_id);
}

int JAP2PConnector::setClientInfoVersion(const char *version) {
    return ja_p2p_set_clientinfo_version(version);
}

int JAP2PConnector::setTCPServer(const char *addr, unsigned short port) {
    return ja_p2p_set_tcpt_server(addr, port);
}

int JAP2PConnector::resetTransfer() {
    return ja_p2p_reset_transfer();
}

void jap2pconnector_OnCheckTutkOnlineResult(int result, void *ctx) {
    JAP2PConnector *c = (JAP2PConnector *) ctx;
    if (c)
        c->doOnCheckTutkOnlineResult(result);
}

void JAP2PConnector::doOnCheckTutkOnlineResult(int result) {
    if (mOnCheckTutkOnlineResult)
        mOnCheckTutkOnlineResult(result, mCheckTutkOnlineCtx);
}

int JAP2PConnector::checkTutkDeviceOnline(const char *tutkid, unsigned int timeout,
                                          OnJAP2PManagerCheckTUTKDeviceOnlineResult on, void *ctx) {
    mOnCheckTutkOnlineResult = on;
    mCheckTutkOnlineCtx = ctx;
    return ja_p2p_check_tutk_dev_online(tutkid, timeout, jap2pconnector_OnCheckTutkOnlineResult,
                                        this);
}

int JAP2PConnector::setTutkLogPath(const char *iotc_log_path, int max_iotc_log_sz,
                                   const char *rdt_log_path, int max_rdt_log_sz) {
    //return ja_p2p_set_tutk_log_path(iotc_log_path, max_rdt_log_sz, rdt_log_path, max_rdt_log_sz);
}

