//
//  JAP2PConnector2.hpp
//  iOS
//
//  Created by Chan Jason on 2017/5/8.
//  Copyright © 2017年 LiHong. All rights reserved.
//

#ifndef JAP2PConnector2_hpp
#define JAP2PConnector2_hpp

#include <stdio.h>
#include <list.h>
#include "JAP2PManager.h"
#include <pthread.h>

#define CTX_TYPE_LIVE 0
#define CTX_TYPE_PLAYBACk 1
#define CTX_TYPE_TALK 2
#define CTX_TYPE_GLOBAL 3

typedef struct newp2p_ctx {
    void *ctx;
    int index;
    int type;
}NEWP2PCTX,*PNEWP2PCTX;

class JAP2PConnector2 : public JAP2PManager{
    
public:
    JAP2PConnector2(void *callback);
    ~JAP2PConnector2();

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
    void ResetConnection(int ret);
    
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
    void * GetCtx(int channel,int type);


private:

    int mConnectResult;
    int mLoginResult;
    int mPlayChannel;
    bool mDoneDisconnect;

    PJACallBackManager mOutCallback;

    OnJAP2PSearchDeviceResult mOnSearchDeviceResult;
    void *mSearchCtx;
    
    OnJAP2PManagerCheckTUTKDeviceOnlineResult mOnCheckTutkOnlineResult;
    void *mCheckTutkOnlineCtx;

    List <PNEWP2PCTX> _ctx;
    pthread_mutex_t _lock;
    void Lock(){pthread_mutex_lock(&_lock);}
    void UnLock(){pthread_mutex_unlock(&_lock);}

    void AddCtx(void *ctx,int channel,int type);
    void DelCtx(int channel,int type);

public:
    void doOnSearchResult(struct SearchDeviceResult *pResult);
};

#endif /* JAP2PConnector2_h */
