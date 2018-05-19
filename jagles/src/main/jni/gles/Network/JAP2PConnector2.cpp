//
//  JAP2PConnector22.cpp
//  iOS
//
//  Created by Chan Jason on 2017/5/8.
//  Copyright © 2017年 LiHong. All rights reserved.
//

#include "JAP2PConnector2.h"
#include <string.h>
#include <list.h>
#include "JADefine.h"
#include "JAP2PManager.h"
#include "pthread.h"
#include "kp2psdk-1.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef __ANDROID__
#include <android/log.h>
#define LOGV(...) __android_log_print(ANDROID_LOG_DEBUG, "honglee_1111", __VA_ARGS__)
#endif

extern bool g_InitP2P_V2;

extern kp2p_callback_t g_Callback_V2;
extern pthread_mutex_t g_listLock;
extern List <JAP2PManager *> g_ListManager;


PTZEXCHANGE ptzexch[16] = {
        {JA_PTZ_CMD_UP,KP2P_PTZ_CONTROL_ACTION_UP},
        {JA_PTZ_CMD_DOWN,KP2P_PTZ_CONTROL_ACTION_DOWN},
        {JA_PTZ_CMD_LEFT,KP2P_PTZ_CONTROL_ACTION_LEFT},
        {JA_PTZ_CMD_RIGHT,KP2P_PTZ_CONTROL_ACTION_RIGHT},
        {JA_PTZ_CMD_AUTOPAN,KP2P_PTZ_CONTROL_ACTION_AUTO},
        {JA_PTZ_CMD_IRIS_OPEN,KP2P_PTZ_CONTROL_ACTION_IRIS_OPEN},
        {JA_PTZ_CMD_IRIS_CLOSE,KP2P_PTZ_CONTROL_ACTION_IRIS_CLOSE},
        {JA_PTZ_CMD_ZOOM_IN,KP2P_PTZ_CONTROL_ACTION_ZOOM_IN},
        {JA_PTZ_CMD_ZOOM_OUT,KP2P_PTZ_CONTROL_ACTION_ZOOM_OUT},
        {JA_PTZ_CMD_FOCUS_FAR,KP2P_PTZ_CONTROL_ACTION_FOCUS_F},
        {JA_PTZ_CMD_FOCUS_NEAR,KP2P_PTZ_CONTROL_ACTION_FOCUS_N},
        {JA_PTZ_CMD_STOP,KP2P_PTZ_CONTROL_ACTION_STOP},
        {JA_PTZ_CMD_AUX1,KP2P_PTZ_CONTROL_ACTION_AUX},
        {JA_PTZ_CMD_PRESET_SET,KP2P_PTZ_CONTROL_ACTION_PRESET_SET},
        {JA_PTZ_CMD_PRESET_CLEAR,KP2P_PTZ_CONTROL_ACTION_PRESET_CLEAR},
        {JA_PTZ_CMD_PRESET_GOTO,KP2P_PTZ_CONTROL_ACTION_PRESET_GO}
};

kp2p_ptz_control_action_t getnewptzcmd(enum  JA_PTZ_CMD cmd)
{
    for(int i=0;i<16;i++)
        if(ptzexch[i].old_cmd==cmd)
            return ptzexch[i].new_cmd;
    return KP2P_PTZ_CONTROL_ACTION_STOP;
}

/* 连接事件 */
void OnKConnect(kp2p_handle_t p2p_handle, void *ctx, const char *conn_type)
{
    //JAP2PManager *mgn = GetP2PConnector((long)p2p_handle);
    JAP2PConnector2 *mgn = (JAP2PConnector2 *)ctx;
    if(!GetP2PConnector(mgn))
        return;
    if(mgn && mgn->GetCallback()->OnConnect) {
        mgn->GetCallback()->OnConnect(mgn, conn_type, mgn->GetCtx(0,CTX_TYPE_GLOBAL));
    }
}

/* 断开连接事件 */
void OnKDisconnect(kp2p_handle_t p2p_handle, void *ctx, int ret)
{
//    JAP2PManager *mgn = GetP2PConnector((long)p2p_handle);
    JAP2PConnector2 *mgn = (JAP2PConnector2 *)ctx;
    if(!GetP2PConnector(mgn))
        return;
    if(mgn && mgn->GetCallback()->OnDisconnect) {
        //LOGV("OnStatus------>JAP2PConnector2.cpp");
        mgn->GetCallback()->OnDisconnect(mgn, ret, mgn->GetCtx(0, CTX_TYPE_GLOBAL));
        mgn->ResetConnection(ret);
        //mgn->closeP2P();
    }
}

/* 登录设备结果事件 */
void OnKLoginResult(kp2p_handle_t p2p_handle, void *ctx, int ret)
{
//    JAP2PManager *mgn = GetP2PConnector((long)p2p_handle);
    JAP2PConnector2 *mgn = (JAP2PConnector2 *)ctx;
    if(!GetP2PConnector(mgn))
        return;
    //LOGV("OnKLoginResult ret---->%d %u %u",ret,mgn,p2p_handle);
//    mgn->loginResult = ret;
    if(mgn && mgn->GetCallback()->OnLoginResult) {
        mgn->GetCallback()->OnLoginResult(mgn, ret, mgn->GetCtx(0,CTX_TYPE_GLOBAL));
    }
}

/* 云台控制结果事件 */
void OnKPtzResult(kp2p_handle_t p2p_handle, void *ctx, int ret)
{
//    JAP2PManager *mgn = GetP2PConnector((long)p2p_handle);
    JAP2PConnector2 *mgn = (JAP2PConnector2 *)ctx;
    if(!GetP2PConnector(mgn) && mgn->GetCallback()->OnPtzResult)
        return;
    if(mgn)
        mgn->GetCallback()->OnPtzResult(mgn,ret,mgn->GetCtx(0,CTX_TYPE_GLOBAL));
}

/* 打开直播通道事件 */
void OnKOpenChannel(kp2p_handle_t p2p_handle, void *ctx, uint32_t channel, uint32_t stream, int err, const char *cam_des)
{
//    JAP2PManager *mgn = GetP2PConnector((long)p2p_handle);
    JAP2PConnector2 *mgn = (JAP2PConnector2 *)ctx;
    //LOGV("OnOpenStream --channel:%d --stream:%d --err:%d --cam_des:%s",channel,stream,err,cam_des);
    if(!GetP2PConnector(mgn))
        return;
    if(mgn && mgn->GetCallback()->OnOpenChannel)
        mgn->GetCallback()->OnOpenChannel(mgn,channel,stream,cam_des,mgn->GetCtx(channel,CTX_TYPE_LIVE));
}

/* 直播数据事件 */
void OnKRecvFrame(kp2p_handle_t p2p_handle, void *ctx, uint8_t media_type, uint8_t frame_type, void *frame, size_t frame_size)
{
//    JAP2PManager *mgn = GetP2PConnector((long)p2p_handle);
    JAP2PConnector2 *mgn = (JAP2PConnector2 *)ctx;
    if(!GetP2PConnector(mgn))
        return;
    if(mgn && mgn->GetCallback()->OnRecvFrame)
        mgn->GetCallback()->OnRecvFrame(mgn,media_type,frame_type,frame,frame_size,mgn->GetCtx(0,CTX_TYPE_LIVE));
}

void OnKRecvFrameEx(kp2p_handle_t p2p_handle, void *ctx,
                      uint32_t frame_type,
                      void *frame, size_t frame_size,
                      uint32_t channel,
                      uint64_t time_stamp,
                      uint32_t width,
                      uint32_t height,
                      uint32_t sample_rate,
                      uint32_t sample_width,
                      uint32_t channels,
                      const char *enc)
{
//    JAP2PManager *mgn = GetP2PConnector((long)p2p_handle);
    JAP2PConnector2 *mgn = (JAP2PConnector2 *)ctx;
    if(!GetP2PConnector(mgn))
        return;
    if(mgn && mgn->GetCallback()->OnRecvFrameEx) {
        if(frame_type==15)
            channel = 0;
        //LOGV("KP2PL OnKRecvFrameEx ---- channel:%d",channel);
        //LOGV("recv video--------->%ld, frametype:%d, frame_size:%d, channel:%d, width:%d, height:%d, enc:%s %p",p2p_handle, frame_type, frame_size, channel, width, height, enc,ctx);
        mgn->GetCallback()->OnRecvFrameEx(mgn, frame_type, frame, frame_size, time_stamp, width,
                                          height, sample_rate, sample_width, channel, (char *) enc,
                                          mgn->GetCtx(channel,CTX_TYPE_LIVE));
    }
}

/*
 * typedef void (*fOnRecvRecFrame)(void *frame,
                                int len,
                                int type,
                                int vWidth,
                                int vHeight,
                                const char* avencode,
                                int aSampleRate,
                                int aSampleWidth,
                                int aSampleChn,
                                int chn,
                                JAP2P_UINT64 timestamp,
                                unsigned int gentime,
                                void *ctx);
 */

/* 录像数据事件 */
void OnKRecvRecFrame(kp2p_handle_t p2p_handle, void *ctx,
                       uint32_t frame_type,
                       void *frame, size_t frame_size,
                       uint32_t channel,
                       uint64_t time_stamp,
                       uint32_t width, uint32_t height,
                       uint32_t sample_rate, uint32_t sample_width, uint32_t sample_chns,
                       uint32_t gen_time,
                       const char* enc)
{
//    JAP2PManager *mgn = GetP2PConnector((long)p2p_handle);
    JAP2PConnector2 *mgn = (JAP2PConnector2 *)ctx;
    if(!GetP2PConnector(mgn))
        return;
    if(mgn && mgn->GetCallback()->OnRecvRecFrame) {
        uint32_t gt = (uint32_t)(time_stamp / 1000L);
        mgn->GetCallback()->OnRecvRecFrame(mgn, frame, frame_size, frame_type, width, height, enc,
                                           sample_rate, sample_width, sample_chns, channel,
                                           time_stamp, gt, mgn->GetCtx(channel,CTX_TYPE_PLAYBACk));
    }
}

/* vcon数据接收事件 */
void OnKVconRecv(kp2p_handle_t p2p_handle, void *ctx, const void *data, size_t data_size)
{
//    JAP2PManager *mgn = GetP2PConnector((long)p2p_handle);
    JAP2PConnector2 *mgn = (JAP2PConnector2 *)ctx;
    if(!GetP2PConnector(mgn))
        return;
    if(mgn && mgn->GetCallback()->OnVconRecv2) {
        //LOGV("OnkVconRecv---%s",data);
        mgn->GetCallback()->OnVconRecv2(mgn,0, (void *) data, data_size, mgn->GetCtx(0,CTX_TYPE_GLOBAL));
    }
}

/* 远程设置事件 */
void OnKRemoteSetup(kp2p_handle_t p2p_handle, void *ctx, const void *data, size_t data_size)
{
//    JAP2PManager *mgn = GetP2PConnector((long)p2p_handle);
    JAP2PConnector2 *mgn = (JAP2PConnector2 *)ctx;
    if(!GetP2PConnector(mgn))
        return;
    if(mgn && mgn->GetCallback()->OnVconRecv2)
        mgn->GetCallback()->OnVconRecv2(mgn,0, (void *) data, data_size, mgn->GetCtx(0,CTX_TYPE_GLOBAL));
        //mgn->GetCallback()->OnRemoteSetup(mgn,data,data_size,ctx);
}

/* 错误事件 */
void OnKP2PError(kp2p_handle_t p2p_handle, void *ctx, const char *msg, size_t msg_len)
{
//    JAP2PManager *mgn = GetP2PConnector((long)p2p_handle);
    JAP2PConnector2 *mgn = (JAP2PConnector2 *)ctx;
    if(!GetP2PConnector(mgn))
        return;
    if(mgn && mgn->GetCallback()->OnP2PError)
        mgn->GetCallback()->OnP2PError(mgn,(char *)msg,msg_len,mgn->GetCtx(0,CTX_TYPE_GLOBAL));
}


void OnCloseStream(kp2p_handle_t p2p_handle, void *context, unsigned int channel, unsigned int stream, int err)
{

}

JAP2PConnector2::~JAP2PConnector2()
{
    LOGV("JAP2PConnector2::~JAP2PConnector2-------1 ----%d",mHandle);
    if(mHandle) {
        LOGV("JAP2PConnector2::~JAP2PConnector2");
        closeP2P();
    }
    pthread_mutex_lock(&g_listLock);
    for(int i=0;i<g_ListManager.length();i++)
    {
        if(g_ListManager.at(i)==this){
            g_ListManager.remove(i);
            break;
        }
    }
    pthread_mutex_unlock(&g_listLock);
}

JAP2PConnector2::JAP2PConnector2(void *callback) {
    if(!g_InitP2P_V2) {
        memset(&g_Callback_V2,0,sizeof(g_Callback_V2));
        g_Callback_V2.OnConnect = OnKConnect;
        g_Callback_V2.OnDisconnect = OnKDisconnect;
        g_Callback_V2.OnLoginResult = OnKLoginResult;
        g_Callback_V2.OnP2PError = OnKP2PError;
        g_Callback_V2.OnPtzResult = OnKPtzResult;
        g_Callback_V2.OnRecvFrameEx = OnKRecvFrameEx;
        g_Callback_V2.OnVconRecv = OnKVconRecv;
        g_Callback_V2.OnRemoteSetup = OnKRemoteSetup;//XM下载专属
        g_Callback_V2.OnOpenStream = OnKOpenChannel;
        g_Callback_V2.OnRecvRecFrame = OnKRecvRecFrame;
        g_Callback_V2.OnCloseStream = OnCloseStream;
        kp2p_init(&g_Callback_V2);
    }
    //createP2PHandle(this);
    memset(mDeviceID, 0, 20);
    memset(mIP, 0, 80);
    mPort = 0;
    mP2P_version = KCP_P2P;
    mHandle = 0;
    mUse = 0;
    mOutCallback = (PJACallBackManager) callback;
    mConnectResult=0x12345678;
    mLoginResult = 0x12345678;
    mDoneDisconnect = false;
    pthread_mutex_init(&_lock,NULL);
    //pthread_mutex_lock(&g_listLock);
    //LOGV("JAP2PConnector2::JAP2PConnector2 --->%u",this);
    g_ListManager.insert(-1,this);
    //LOGV("JAP2PConnector2::JAP2PConnector2 end --->%u",this);
    //pthread_mutex_unlock(&g_listLock);
}


void JAP2PConnector2::setDebugLevel(int level) {
//    ja_p2p_set_debug_level(level);

}

long JAP2PConnector2::connect(const char *devid, int connType) {
//    kp2p_handle_t h = ja_p2p_connect(devid, connType);
//    mP2P = h;
//    return (long)mP2P;
    return -1;
}

long JAP2PConnector2::createP2PHandle(void *ctx) {
    Lock();
    kp2p_handle_t h1 =(kp2p_handle_t) mHandle;
    if(h1 && !mDoneDisconnect) {
        UnLock();
        return (long) mHandle;
    }
    if(mDoneDisconnect && h1)
    {
        //kp2p_handle_t h1 = (kp2p_handle_t)mHandle;
        kp2p_close(h1);
        mDoneDisconnect = false;
    }
    LOGV("createP2PHandle..............");
    AddCtx(ctx,0,CTX_TYPE_GLOBAL);
    kp2p_handle_t h = kp2p_create(this);
    mHandle = (void *)h;
    UnLock();
    return (long)mHandle;
}


int JAP2PConnector2::connect2(const char *devid, const char *ipaddr, unsigned short port, const char *verify_str) {
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h)
        return -1;

    Lock();
    if(mConnectResult!=0x12345678)
    {
        UnLock();
        //LOGV("direct return result.......................^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^.");
        return mConnectResult;
    }
    //LOGV("start kp2p_connect2.......");
    if(devid)
        strncpy(mDeviceID,devid,20);
    if(ipaddr)
        strncpy(mIP,ipaddr,80);
    if(port)
        mPort = port;
    int ret = kp2p_connect2(h, devid, ipaddr, port, verify_str);
    mConnectResult = ret;
    //LOGV("kp2p connect .......%d",ret);
    UnLock();
    return ret;

}

int JAP2PConnector2::connect3(const char *devid, const char *ipaddr, unsigned short port) {
    //LOGV("start kp2p_connect.......");
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h)
        return -1;
    Lock();
    if(mConnectResult!=0x12345678)
    {
        UnLock();
        //LOGV("direct return result.......................^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^.");
        return mConnectResult;
    }
    if(devid)
        strncpy(mDeviceID,devid,20);
    if(ipaddr)
        strncpy(mIP,ipaddr,80);
    if(port)
        mPort = port;
    int ret = kp2p_connect(h, devid, ipaddr, port);
    mConnectResult = ret;
    UnLock();
    return ret;
}

const char* JAP2PConnector2::getConnectProto() {
//    kp2p_handle_t h = (kp2p_handle_t)mHandle;
//    if (!h)
//        return NULL;
//    return ja_p2p_GetConnectProto(h);
    return NULL;
}

const char* JAP2PConnector2::getConnectStepStr(const char *proto_name) {
//    kp2p_handle_t h = (kp2p_handle_t)mHandle;
//    if (!h)
//        return NULL;
//    return ja_p2p_GetConnectStepStr(h, proto_name);
    return NULL;
}

void JAP2PConnector2::ResetConnection(int ret) {
//    if(mHandle)
//    {
//        kp2p_handle_t h = (kp2p_handle_t)mHandle;
//        kp2p_close(h);
//        mHandle = NULL;
//    }
    for(int i=0;i<_ctx.length();i++)
    {
        LOGV("ctx---------ptr:%p,index:%d,type:%d use:%d",_ctx.at(i)->ctx,_ctx.at(i)->index,_ctx.at(i)->type,GetUse());
        if(_ctx.at(i)->type!=CTX_TYPE_GLOBAL&&GetCallback()->OnDisconnect)
        {
            GetCallback()->OnDisconnect(this,ret,_ctx.at(i)->ctx);
        }
    }
    mConnectResult = 0x12345678;
    mLoginResult = 0x12345678;
    mDoneDisconnect = true;
}

void JAP2PConnector2::closeP2P() {
    LOGV("close p2p---------->");
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h)
        return ;
    Lock();
    kp2p_close(h);
    mHandle = NULL;
    mConnectResult = 0x12345678;
    mLoginResult = 0x12345678;
    for(int i=_ctx.length()-1;i>=0;i--)
    {
        PNEWP2PCTX lctx = _ctx.at(i);
        _ctx.remove(i);
        free(lctx);
    }
    LOGV("close p2p---------->_end");
    UnLock();
}

int JAP2PConnector2::loginP2P(const char *user, const char *pwd) {
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h) {
        //LOGV("invalid handle-----------");
        return JA_CONNECT_ERROR_UNKNOW;
    }
    Lock();
    if(mLoginResult!=0x12345678)
    {
        //LOGV("aready login........... %x",mLoginResult);
        UnLock();
        return mLoginResult;
    }
    LOGV("Start kp2pLogin------");
    mLoginResult = kp2p_login(h, user, pwd);
//    if(kp2p_login(h, user, pwd) == 0)
//    {
//        int login_timeout = 0;
//        loginResult = 0xFFFFFFFF;
//        while(true)
//        {
//            ja_sleep(1000);
//            login_timeout++;
//            if(loginResult != 0xFFFFFFFF)break;
//            if(login_timeout > JA_CONNECT_LOGIN_TIMEOUT) {
//                LOGV("kp2p_login timeout --->");
//                return JA_CONNECT_ERROR_TIMEOUT;
//            }
//        }
//    } else
//    {
//        return JA_CONNECT_ERROR_UNKNOW;
//    }
    //LOGV("kp2p_login result:--->%d",mLoginResult);
    UnLock();
    return mLoginResult;
}

void JAP2PConnector2::AddCtx(void *ctx,int channel,int type)
{
    void *ectx = GetCtx(channel,type);
    if(ectx==ctx)
        return;
    PNEWP2PCTX lctx = (PNEWP2PCTX)calloc(1,sizeof(NEWP2PCTX));
    lctx->ctx = ctx;
    lctx->index = channel;
    lctx->type = type;
    _ctx.insert(-1,lctx);
}
void * JAP2PConnector2::GetCtx(int channel,int type)
{
    for(int i=0;i<_ctx.length();i++)
    {
        if(_ctx.at(i)->index==channel&&_ctx.at(i)->type == type)
            return _ctx.at(i)->ctx;
    }
    return NULL;
}
void JAP2PConnector2::DelCtx(int channel,int type)
{
    for(int i=0;i<_ctx.length();i++)
    {
        if(_ctx.at(i)->index==channel&&_ctx.at(i)->type == type)
        {
            PNEWP2PCTX lctx = _ctx.at(i);
            _ctx.remove(i);
            free(lctx);
            break;
        }
    }
}


int JAP2PConnector2::openChannel(int nChannelId, int nStreamId, int bOpen,void *ctx) {
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h)
        return -1;
    int ret;
    Lock();
    if(bOpen == 1)
    {
        //LOGV("Start Open kp2p_open_stream.........");
        for(int i=0;i<_ctx.length();i++)
        {
            if(_ctx.at(i)->ctx==ctx&&_ctx.at(i)->type == CTX_TYPE_LIVE&&_ctx.at(i)->index!=nChannelId)
            {
                PNEWP2PCTX lctx = _ctx.at(i);
                _ctx.remove(i);
                ret = kp2p_close_stream(h,lctx->index,1);
                ret = ret | kp2p_close_stream(h,lctx->index,0);
                free(lctx);
                LOGV("KP2PL,kp2p_close_stream:%d,channel:,%d,streamid:%d", ret, nChannelId, nStreamId);
                break;
            }
        }
        AddCtx(ctx,nChannelId,CTX_TYPE_LIVE);
        ret = kp2p_open_stream(h, nChannelId, nStreamId);
        LOGV("KP2PL,kp2p_open_stream:%d,channel:,%d,streamid:%d", ret, nChannelId, nStreamId);
    } else
    {
        //LOGV("Start Open kp2p_close_stream.........");
        DelCtx(nChannelId,CTX_TYPE_LIVE);
        ret = kp2p_close_stream(h, nChannelId, nStreamId);
        LOGV("KP2PL,kp2p_close_stream:%d,channel:,%d,streamid:%d", ret, nChannelId, nStreamId);
    }
    //LOGV("open or close stream ret---->%d",ret);
    UnLock();
    return ret;
}

int JAP2PConnector2::ptzCtrl(int nChannel, int act, int bStart, int param1, int param2) {
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h)
        return -1;
    Lock();
    int ret = kp2p_ptz_ctrl(h, nChannel, getnewptzcmd((enum  JA_PTZ_CMD)act), param1, param2);
    //int ret = kp2p_ptz_ctrl(h, nChannel,(kp2p_ptz_control_action_t) act, param1, param2);
    UnLock();
    return ret;
}

int JAP2PConnector2::getChannelCnt() {
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h)
        return -1;
    return 0;
}

long JAP2PConnector2::vconCreate(const char *appName) {
    return 0;
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h)
        return -1;
    kp2p_vcon_handle_t sh = kp2p_vcon_create(h, appName);
    LOGV("JAP2PConnector2::vconCreate ---> %u %s",sh,appName);
    return (long)sh;
}

int JAP2PConnector2::vconSend(long vcon_handle, void *buf, int buf_len) {
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h)
        return -1;
    Lock();
    LOGV("JAP2PConnector2::vconSend(%d)-->%s",buf_len,buf);
    int ret = kp2p_remote_setup(h,buf,buf_len);
    LOGV("JAP2PConnector2::vconSend-->%d",ret);
    UnLock();
    return ret;
//    kp2p_vcon_handle_t h = (kp2p_vcon_handle_t *)vcon_handle;
//    if (!h)
//        return -2;
//    LOGV("JAP2PConnector2::vconSend-->%s",buf);
//    int ret = kp2p_vcon_send(h, (const void *)buf, (size_t)buf_len);
//    LOGV("JAP2PConnector2::vconSend-->%d",ret);
//    return ret;
}

int JAP2PConnector2::vconDestroy(long vcon_handle) {
    return 0;
    kp2p_vcon_handle_t h = (VCON_HANDLE *)vcon_handle;
    if (!h)
        return -2;
    return kp2p_vcon_destroy(h);
}

long JAP2PConnector2::findFileTaskStart(char *chn, int chn_count, int recfile_starttime, int recfile_endtime, int recfile_type) {
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h)
        return -1;
    Lock();
    LOGV("JAP2PConnector2::findFileTaskStart-->start:%d end:%d",recfile_starttime,recfile_endtime);
    long ret = (long)kp2p_rec_find_file_start(h, (uint32_t)chn_count, (uint8_t *)chn, (time_t)recfile_starttime, (time_t)recfile_endtime, (uint8_t)recfile_type);
    LOGV("JAP2PConnector2::findFileTaskStart-->%d",ret);
    UnLock();
    return ret;
}

int JAP2PConnector2::findFileCnt(long findFileTask_handle) {
    Lock();
    kp2p_rec_search_handle_t h = (kp2p_rec_search_handle_t)findFileTask_handle;
    int ret = (int)kp2p_rec_find_file_count(h);
    LOGV("JAP2PConnector2::findFileCnt-->%d",ret);
    UnLock();
    return ret;
}

pjap2p_rec_files JAP2PConnector2::findFileNext(long findFileTask_handle) {
    //PlaybackSearchTaskHandle sh = (PlaybackSearchTaskHandle)findFileTask_handle;
    kp2p_rec_search_handle_t h = (kp2p_rec_search_handle_t)findFileTask_handle;
    if (!h)
        return NULL;
    Lock();
    //stFileRecords *recs = ja_p2p_rec_FindFileNext(sh);
    kp2p_rec_files_t *rec = kp2p_rec_find_file_next(h);
    memset(&recFiles, 0, sizeof(recFiles));
    if(rec!=NULL) {
        LOGV("JAP2PConnector2::findFileNext --> %d",rec->records_cnt);

        recFiles.cnt = rec->records_cnt;
        for (int i = 0; i < rec->records_cnt; i++) {
            //memcpy(&recFiles.records[i], &rec->records[i], sizeof(kp2p_rec_file_t));
            recFiles.records[i].quality = rec->records[i].quality;
            recFiles.records[i].begin_time = rec->records[i].begin_time;
            recFiles.records[i].type = rec->records[i].type;
            recFiles.records[i].channel = rec->records[i].channel;
            recFiles.records[i].end_time = rec->records[i].end_time;
        }
    } else
    {
        UnLock();
        return NULL;
    }
    //return (long)recs;
    UnLock();
    return &recFiles;
}

int JAP2PConnector2::findFileTaskClose(long findFileTask_handle) {
    kp2p_rec_search_handle_t handle = (kp2p_rec_search_handle_t)findFileTask_handle;
    if (!handle)
        return -2;
    Lock();
    //int ret = ja_p2p_rec_FindFileTaskClose(&sh);
    int ret = kp2p_rec_find_file_stop(handle);
    //int ret = kp2p_find_stop(handle, 1000);
    LOGV("JAP2PConnector2::findFileTaskClose --> %d",ret);
    UnLock();
    return ret;
}

long JAP2PConnector2::playBackStart(char *chn, int chn_count, int recfile_starttime, int recfile_endtime, int recfile_type, fOnRecvRecFrame OnRecvRecFrame, void *ctx) {
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h)
        return -1;
    Lock();
    mPlayChannel=*chn;
    AddCtx(ctx,*chn,CTX_TYPE_PLAYBACk);
    long ret = (long)kp2p_rec_play_start(h, (uint32_t)chn_count, (uint8_t *)chn, (time_t)recfile_starttime, (uint32_t)recfile_endtime, (uint8_t)recfile_type);
    LOGV("JAP2PConnector2::playBackStart ---->start:%d,end:%d ret:%d",recfile_starttime,recfile_endtime,ret);
    UnLock();
    return ret;
}

int JAP2PConnector2::playBackPause(long playBack_handle) {
    kp2p_rec_playback_handle_t ph = (kp2p_rec_playback_handle_t)playBack_handle;
    if (!ph)
        return -2;
    Lock();
    int ret = kp2p_rec_play_pause(ph);
    UnLock();
    return ret;
}

int JAP2PConnector2::playBackContinue(long playBack_handle) {
    kp2p_rec_playback_handle_t ph = (kp2p_rec_playback_handle_t)playBack_handle;
    if (!ph)
        return -2;
    Lock();
    int ret = kp2p_rec_play_continue(ph);
    UnLock();
    return ret;
}

int JAP2PConnector2::playBackSeek(long playBack_handle, int recfile_starttime, int recfile_endtime, int recfile_type) {
    kp2p_rec_playback_handle_t ph = (kp2p_rec_playback_handle_t)playBack_handle;
    if (!ph)
        return -2;
    return -1;
}

int JAP2PConnector2::playBackStop(long playBack_handle) {
    kp2p_rec_playback_handle_t ph = (kp2p_rec_playback_handle_t)playBack_handle;
    if (!ph)
        return -2;
    Lock();
    DelCtx(mPlayChannel,CTX_TYPE_PLAYBACk);
    int ret = kp2p_rec_play_stop(ph);
    LOGV("JAP2PConnector2::playBackStop ---->",ret);
    UnLock();
    return ret;
}

void JAP2PConnector2_OnSearchDeviceResult(struct SearchDeviceResult *pResult, void *ctx) {
    JAP2PConnector2 *c = (JAP2PConnector2 *)ctx;
    if (!c)
        return ;
    c->doOnSearchResult(pResult);
}

int JAP2PConnector2::searchDevice(OnJAP2PSearchDeviceResult on, void *ctx) {
    mOnSearchDeviceResult = on;
    mSearchCtx = ctx;
    fOnSearchDeviceResult result = JAP2PConnector2_OnSearchDeviceResult;
    Lock();
    int ret = kp2p_search_device_start(this);
    UnLock();
    return ret;
}

void JAP2PConnector2::doOnSearchResult(struct SearchDeviceResult *pResult) {
    JAP2PSearchDeviceResult result;
    memset((void *)&result, 0, sizeof(JAP2PSearchDeviceResult));
    //    memcpy((void *)result.esee_id, pResult->esee_id, 32);
    //    memcpy((void *)result.ip, pResult->ip, 32);
    //    result.port = pResult->port;
    //    result.max_ch = pResult->max_ch;
    //    memcpy((void *)result.dev_model, pResult->dev_model, 32);
    memcpy((void *)&result, (const void *)pResult, sizeof(JAP2PSearchDeviceResult));
    if (mOnSearchDeviceResult)
        mOnSearchDeviceResult(&result, mSearchCtx);
}

int JAP2PConnector2::call(int chn) {
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h)
        return -1;
    Lock();
    int ret = kp2p_vop2p_call(h, (uint32_t)chn);
    LOGV("JAP2PConnector2::call ---ret:%d chn:%d",ret,chn);
    UnLock();
    return ret;
}

int JAP2PConnector2::talkSend(void *voiceData, size_t dataSz, JAP2P_UINT64 tsMs, const char *enc, int samplerate, int samplewidth, int channels, float compressRatio) {
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h)
        return -1;
    Lock();
    LOGV("JAP2PConnector2::talkSend");
    int ret = kp2p_vop2p_send(h, voiceData, dataSz, tsMs, enc, (uint32_t)samplerate, (uint32_t)samplewidth, (uint32_t)channels, compressRatio);
    UnLock();
    return ret;
}

int JAP2PConnector2::hangup() {
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h)
        return -1;
    Lock();
    LOGV("JAP2PConnector2::hangup");
    int ret = kp2p_vop2p_hangup(h);
    UnLock();
    return ret;
}

int JAP2PConnector2::pullAlarm(int type, int ch, time_t ticket) {
    kp2p_handle_t h = (kp2p_handle_t)mHandle;
    if (!h)
        return -1;
    return -1;
}

int JAP2PConnector2::setClientType(int type) {
    //return ja_p2p_set_clienttype(type);
    return 0;
}

int JAP2PConnector2::setClientInfoType(const char *clientType) {
    //return ja_p2p_set_clientinfo_type(clientType);
    return 0;
}

int JAP2PConnector2::setClientInfoImei(const char *imei) {
    //return ja_p2p_set_clientinfo_imei(imei);
    return 0;
}

int JAP2PConnector2::setClientInfoIsp(const char *isp) {
    //return ja_p2p_set_clientinfo_isp(isp);
    return 0;
}

int JAP2PConnector2::setClientInfoBundleID(const char *bundle_id) {
    //return ja_p2p_set_clientinfo_bundleid(bundle_id);
    return 0;
}

int JAP2PConnector2::setClientInfoVersion(const char *version) {
    //return ja_p2p_set_clientinfo_version(version);
    return 0;
}

int JAP2PConnector2::setTCPServer(const char *addr, unsigned short port) {
    //return ja_p2p_set_tcpt_server(addr, port);
    return 0;
}

int JAP2PConnector2::resetTransfer() {
    //return ja_p2p_reset_transfer();
    return 0;
}

void JAP2PConnector2_OnCheckTutkOnlineResult(int result, void *ctx) {
    JAP2PConnector2 *c = (JAP2PConnector2 *)ctx;
    if (c)
        c->doOnCheckTutkOnlineResult(result);
}

void JAP2PConnector2::doOnCheckTutkOnlineResult(int result) {
    if (mOnCheckTutkOnlineResult)
        mOnCheckTutkOnlineResult(result, mCheckTutkOnlineCtx);
}

int JAP2PConnector2::checkTutkDeviceOnline(const char *tutkid, unsigned int timeout, OnJAP2PManagerCheckTUTKDeviceOnlineResult on, void *ctx) {
    mOnCheckTutkOnlineResult = on;
    mCheckTutkOnlineCtx = ctx;
    //return ja_p2p_check_tutk_dev_online(tutkid, timeout, JAP2PConnector2_OnCheckTutkOnlineResult, this);
    return 0;
}

int JAP2PConnector2::setTutkLogPath(const char *iotc_log_path, int max_iotc_log_sz, const char *rdt_log_path, int max_rdt_log_sz) {
    //return ja_p2p_set_tutk_log_path(iotc_log_path, max_rdt_log_sz, rdt_log_path, max_rdt_log_sz);
}

