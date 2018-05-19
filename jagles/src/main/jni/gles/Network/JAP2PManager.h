//
//  JAP2PManager.hpp
//  iOS
//
//  Created by Chan Jason on 2017/5/6.
//  Copyright © 2017年 LiHong. All rights reserved.
//

#ifndef JAP2PManager_hpp
#define JAP2PManager_hpp

#include <stdio.h>
#include "libjnnat.h"
#include "kp2psdk.h"

class JAP2PManager;

#define JAP2P_REC_FILES_MAX 200

#define UDX_P2P 0
#define KCP_P2P 1

typedef struct {
    unsigned int channel;
    unsigned int type;
    unsigned int begin_time;
    unsigned int end_time;
    unsigned int quality;
} jap2p_rec_file, *pjap2p_rec_file;

typedef struct ptzexchange {
    enum JA_PTZ_CMD old_cmd;
    kp2p_ptz_control_action_t new_cmd;
} PTZEXCHANGE;

typedef struct {
    unsigned int cnt;
    jap2p_rec_file records[JAP2P_REC_FILES_MAX];
} jap2p_rec_files, *pjap2p_rec_files;

typedef struct _ja_manager_callback {
    /*连接过程中产生的成功连接事件*/
    void *ctx;

    void (*OnConnect)(JAP2PManager *p2p, const char *conn_type_msg, void *ctx);

    /*断开连接事件,连接失败，及关闭连接的时候会有此事件
     **/
    void (*OnDisconnect)(JAP2PManager *p2p, int ret, void *ctx);

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
    void (*OnRecvFrame)(
            JAP2PManager *p2p,
            unsigned short media_type,
            unsigned short frame_type,
            void *frame,
            int frame_len, void *ctx);

    void (*OnRecvFrameEx)(
            JAP2PManager *p2p,
            unsigned short frame_type,
            void *frame,
            int frame_len,
            JAP2P_UINT64 uiTimeStampMs,
            unsigned int uiWidth,
            unsigned int uiHeight,
            unsigned int uiSamplerate,
            unsigned int uiSamplewidth,
            unsigned int uiChannels,
            char *avEnc, void *ctx);


    /*
     *p2p 用户校验结果事件
     *ret: [0：成功;1:校验超时;2:用户名密码错误]
     */
    void (*OnLoginResult)(JAP2PManager *p2p, int ret, void *ctx);

    /*p2p云台控制结果事件
     */
    void (*OnPtzResult)(JAP2PManager *p2p, int ret, void *ctx);

    /*虚拟通道数据接收事件，注意区别于音视频接收事件回调* */
    void (*OnVconRecv)(JAP2PManager *p2p, void *buf, int len, void *ctx);

    void (*OnVconEvent)(JAP2PManager *p2p, char *event, void *ctx);

    void (*OnP2PError)(JAP2PManager *p2p, char *error_msg, int len, void *ctx);

    /**
     * 新增虚拟通道接收事件，只仍出对应vcon_handle,
     * 注意:只需要设置OnVconRecv/OnVconRecv2 中一个回调*
     */
    void (*OnVconRecv2)(JAP2PManager *p2p, VCON_HANDLE *vcon_handle, void *buf, int len, void *ctx);

    void (*OnConnectReq)(JAP2PManager *p2p,
                         int proto_type,
                         int action,
                         unsigned int ip,
                         unsigned short port,
                         unsigned int random, void *ctx);

    void (*OnOpenChannel)(JAP2PManager *p2p,
                          int nChannelId,
                          int nStreamId,
                          const char *camDes, void *ctx);

    /**
     * 注意有可能一次回调的数据不是完整的一张图或一段视频
     */
    void (*OnRecvAlarmMsg)(JAP2PManager *p2p,
                           struct P2PAlarmMsgData *msgData, void *ctx);

    void (*OnRecvRecFrame)(JAP2PManager *p2p, void *frame,
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
                           void *ctx);

    void (*OnRemoteSetup)(JAP2PManager *p2p, const void *data, size_t data_size, void *ctx);

} TJACallBackManager, *PJACallBackManager;


class JAP2PManager {

public:

    JAP2PManager() { mUse = 0; };

    virtual ~JAP2PManager() {};

    //virtual void setDebugLevel(int level){;

    virtual long connect(const char *devid, int connType) {};

    virtual long createP2PHandle(void *ctx) {};

    virtual int connect2(const char *devid,
                         const char *ipaddr,
                         unsigned short port,
                         const char *verify_str) {};

    virtual int connect3(const char *devid,
                         const char *ipaddr,
                         unsigned short port) {};

    virtual const char *getConnectProto() {};

    virtual const char *getConnectStepStr(const char *proto_name) {};

    virtual void closeP2P() {};

    virtual int loginP2P(const char *user, const char *pwd) {};

    virtual int openChannel(int nChannelId,
                            int nStreamId,
                            int bOpen, void *ctx) {};

    virtual int ptzCtrl(int nChannel,
                        int act,
                        int bStart,
                        int param1,
                        int param2) {};

    virtual int getChannelCnt() {};

    virtual long vconCreate(const char *appName) {};

    virtual int vconSend(long vcon_handle, void *buf, int buf_len) {};

    virtual int vconDestroy(long vcon_handle) {};

    virtual long findFileTaskStart(char chn[],
                                   int chn_count,
                                   int recfile_starttime,
                                   int recfile_endtime,
                                   int recfile_type) {};

    virtual int findFileCnt(long findFileTask_handle) {};

    virtual pjap2p_rec_files findFileNext(long findFileTask_handle) {};

    virtual int findFileTaskClose(long findFileTask_handle) {};

    virtual long playBackStart(char chn[],
                               int chn_count,
                               int recfile_starttime,
                               int recfile_endtime,
                               int recfile_type,
                               fOnRecvRecFrame OnRecvRecFrame,
                               void *ctx) {};

    virtual int playBackPause(long playBack_handle) {};

    virtual int playBackContinue(long playBack_handle) {};

    virtual int playBackSeek(long playBack_handle,
                             int recfile_starttime,
                             int recfile_endtime,
                             int recfile_type) {};

    virtual int playBackStop(long playBack_handle) {};

    struct JAP2PSearchDeviceResult {
        char esee_id[32];
        char ip[32];
        unsigned short port;
        unsigned int max_ch;
        char dev_model[32];
    };

    typedef void (*OnJAP2PSearchDeviceResult)(struct JAP2PSearchDeviceResult *pResult, void *ctx);

    virtual int searchDevice(OnJAP2PSearchDeviceResult on, void *ctx) {};

    virtual int call(int chn) {};

    virtual int hangup() {};

    virtual int talkSend(void *voiceData,
                         size_t dataSz,
                         JAP2P_UINT64 tsMs,
                         const char *enc,
                         int samplerate,
                         int samplewidth,
                         int channels,
                         float compressRatio) {};

    virtual int pullAlarm(int type,
                          int ch,
                          time_t ticket) {};

    virtual int setClientType(int type) {};

    virtual int setClientInfoType(const char *clientType) {};

    virtual int setClientInfoImei(const char *imei) {};

    virtual int setClientInfoIsp(const char *isp) {};

    virtual int setClientInfoBundleID(const char *bundle_id) {};

    virtual int setClientInfoVersion(const char *version) {};

    virtual int setTCPServer(const char *addr, unsigned short port) {};

    virtual int resetTransfer() {};

    typedef void( *OnJAP2PManagerCheckTUTKDeviceOnlineResult)(int result, void *ctx);

    virtual int checkTutkDeviceOnline(const char *tutkid, unsigned int timeout,
                                      OnJAP2PManagerCheckTUTKDeviceOnlineResult on, void *ctx) {};

    virtual int setTutkLogPath(const char *iotc_log_path,
                               int max_iotc_log_sz,
                               const char *rdt_log_path,
                               int max_rdt_log_sz) {};

    virtual PJACallBackManager GetCallback()=0;

    long GetHandle() { return (long) mHandle; };

    bool CompareConnectStr(const char *devid, const char *ipaddr, unsigned short port);

    void UpdateProperty(char *devid, char *ipaddr, int port);

    int loginResult;

    int GetUse() { return mUse; }

    void UpdateUse(bool add) {
        if (add)
            mUse++;
        else
            mUse--;
        if (mUse < 0) {
            mUse = 0;
        }
    }

    int GetP2PVersion() {
        return mP2P_version;
    }

protected:
    void *mHandle;
    jap2p_rec_files recFiles;
    int mP2P_version;
    char mDeviceID[20];
    char mIP[80];
    short mPort;
    int mUse;
};

JAP2PManager *GetP2PConnector(long handle);

bool GetP2PConnector(JAP2PManager *mgn);

#endif /* JAP2PManager_h */
