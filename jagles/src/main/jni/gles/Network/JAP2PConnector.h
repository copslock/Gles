//
//  JAP2PConnector.h
//  iOS
//
//  Created by Chan Jason on 2017/5/7.
//  Copyright © 2017年 LiHong. All rights reserved.
//

#ifndef JAP2PConnector_hpp
#define JAP2PConnector_hpp

#define LOG_TAG "JAVideo"
#define LOGD(...)__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#include <stdio.h>
#include "JAP2PManager.h"

class JAP2PConnector : public JAP2PManager {
    
public:
    JAP2PConnector(void *callback);
    ~JAP2PConnector();

    long createP2PHandle(void *ctx);

    void setDebugLevel(int level);
    
    long connect(const char *devid, int connType);
    
    int connect2(const char* devid,
                         const char *ipaddr,
                         unsigned short port,
                         const char* verify_str);
    
    int connect3(const char* devid,
                         const char *ipaddr,
                         unsigned short port);
    
    const char *getConnectProto();
    const char *getConnectStepStr(const char *proto_name);
    void closeP2P();
    
    int loginP2P(const char *user, const char *pwd);
    int openChannel(int nChannelId,
                            int nStreamId,
                            int bOpen,
                            void *ctx);
    int ptzCtrl(int nChannel,
                        int act,
                        int bStart,
                        int param1,
                        int param2);
    int getChannelCnt();
    
    long vconCreate(const char *appName);
    int vconSend(long vcon_handle, void *buf, int buf_len);
    int vconDestroy(long vcon_handle);
    
    long findFileTaskStart(char chn[],
                                   int chn_count,
                                   int recfile_starttime,
                                   int recfile_endtime,
                                   int recfile_type);
    int findFileCnt(long findFileTask_handle);
    pjap2p_rec_files findFileNext(long findFileTask_handle);
    int findFileTaskClose(long findFileTask_handle);
    
    long playBackStart(char chn[],
                               int chn_count,
                               int recfile_starttime,
                               int recfile_endtime,
                               int recfile_type,
                               fOnRecvRecFrame OnRecvRecFrame,
                               void *ctx);
    int playBackPause(long playBack_handle);
    int playBackContinue(long playBack_handle);
    int playBackSeek(long playBack_handle,
                             int recfile_starttime,
                             int recfile_endtime,
                             int recfile_type);
    int playBackStop(long playBack_handle);
    
    struct JAP2PSearchDeviceResult {
        char esee_id[32];
        char ip[32];
        unsigned short port;
        unsigned int max_ch;
        char dev_model[32];
    };
    typedef void (*OnJAP2PSearchDeviceResult)(struct JAP2PSearchDeviceResult *pResult, void *ctx) ;
    int searchDevice(OnJAP2PSearchDeviceResult on, void *ctx);
    
    int call(int chn);
    int hangup();
    int talkSend(void *voiceData,
                         size_t dataSz,
                         JAP2P_UINT64 tsMs,
                         const char *enc,
                         int samplerate,
                         int samplewidth,
                         int channels,
                         float compressRatio);
    
    int pullAlarm(int type,
                          int ch,
                          time_t ticket);
    
    int setClientType(int type);
    int setClientInfoType(const char *clientType);
    int setClientInfoImei(const char *imei);
    int setClientInfoIsp(const char *isp);
    int setClientInfoBundleID(const char *bundle_id);
    int setClientInfoVersion(const char *version);
    
    int setTCPServer(const char *addr, unsigned short port);
    int resetTransfer();
    
    typedef void( *OnJAP2PManagerCheckTUTKDeviceOnlineResult)(int result, void * ctx) ;
    void doOnCheckTutkOnlineResult(int result);
    int checkTutkDeviceOnline(const char *tutkid, unsigned int timeout, OnJAP2PManagerCheckTUTKDeviceOnlineResult on, void *ctx);
    int setTutkLogPath(const char* iotc_log_path,
                               int max_iotc_log_sz,
                               const char* rdt_log_path,
                               int max_rdt_log_sz);
    PJACallBackManager GetCallback(){return mOutCallback;};

private:
    PJACallBackManager mOutCallback;

    OnJAP2PSearchDeviceResult mOnSearchDeviceResult;
    void *mSearchCtx;
    
    OnJAP2PManagerCheckTUTKDeviceOnlineResult mOnCheckTutkOnlineResult;
    void *mCheckTutkOnlineCtx;

public:
    void doOnSearchResult(struct SearchDeviceResult *pResult);
};

#endif /* JAP2PConnector_h */
