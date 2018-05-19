//
// Created by LiHong on 16/2/6.
// Copyright (c) 2016 LiHong. All rights reserved.
//
#pragma once

#pragma pack(8)

#ifndef FISHEYE_JACONNECT_H
#define FISHEYE_JACONNECT_H

#include "libjnnat.h"
#include "../Utils/list.h"
#include <pthread.h>
#include "../Media/JAMedia.h"
#include "netinet/in.h"  //Linux要用的头文件
#include "../Media/ag711.h"
#include "../GLES/ParametricSurface.h"
#include "JAP2PManager.h"

#define CMD_CONNECT         1
#define CMD_OPENCHANNEL     2
#define CMD_CLOSECHANNEL    3
#define CMD_PTZSTART        4
#define CMD_PTZSTOP         5
#define CMD_SEARCHREC       6
#define CMD_STARTPLAYBACK   7
#define CMD_STOPPLAYBACK    8
#define CMD_DISCONNECT      9
#define CMD_OPENDATACHANNEL 10
#define CMD_CLOSEDATACHANNEL 11
#define CMD_SENDDATA        12
#define CMD_PLAYBACK_CONTINUE 13
#define CMD_PLAYBACK_PAUSE   14
#define CMD_DESTROY_VCON     15
#define CMD_CALL            16
#define CMD_HANG_UP            17
#define CMD_SEND_AUDIO        18
#define CMD_SEARCH_DEVICE   19
#define CMD_PULL_ALARMMSG   20


#define NET_STATUS_CONNECTING   0
#define NET_STATUS_CONNECTED    1
#define NET_STATUS_CONNECTFAIL  2
#define NET_STATUS_LOGINING     3
#define NET_STATUS_LOGINED      4
#define NET_STATUS_LOGINFAIL    5
#define NET_STATUS_OPENING      6
#define NET_STATUS_OPENFAIL     7
#define NET_STATUS_DISCONNECTED 8
#define NET_STATUS_CLOSED       9
#define NET_STATUS_AUTHOFAIL    10
#define NET_STATUS_TIMEOUT      11
#define NET_STATUS_ACTIVECLOSED 12
#define NET_STATUS_OPEN_VIDEO   13
#define NET_STATUS_CLOSE_VIDEO    14
#define NET_STATUS_FIRST_FRAME  15

#define NET_PLAY_NONE            0
#define NET_PLAY_REAL            1
#define NET_PLAY_RECORD            2

#define PLAYBACK_STATUS_NONE    0
#define PLAYBACK_STATUS_GETDATA 1
#define PLAYBACK_STATUS_PAUSE   2

#define H264_Enc "h264"
#define H265_Enc "h265"


#define FRAME_TYPE_RAW  15

class JAConnect;

static List<JAConnect *> gConnector;

#ifdef __cplusplus
extern "C" {
#endif


static TJACallBackManager gJACallbackInit;
static bool gIsInitNetwork = false;

enum {
    P2P_BYPASS_FRAME_TYPE_FISHEYE_PARAM = 0,
    P2P_BYPASS_FRAME_TYPE_LENS_PARAM,
    P2P_BYPASS_FRAME_TYPE_GSENSER_PARAM,
    P2P_BYPASS_FRAME_TYPE_CNT,
};

enum {
    eP2P_IMAGE_FISHEYE_FIX_MODE_WALL = 0,
    eP2P_IMAGE_FISHEYE_FIX_MODE_CEIL,
    eP2P_IMAGE_FISHEYE_FIX_MODE_TABLE,
    eP2P_IMAGE_FISHEYE_FIX_MODE_NONE,
};


#define DEVICE_TYPE_360 1
#define DEVICE_TYPE_720 2

typedef struct gsensorAngles {
    struct {
        double min_angle_range;
        double max_angle_range;
        double angle_x;
        double angle_y;
        double angle_z;
    };
    int version;
    int Reverse;
} ST_GSENSOR_ANGLE,*LP_GSENSOR_ANGLE;


typedef struct fisheyeParam {
    struct {
        int CenterCoordinateX;
        int CenterCoordinateY;
        int Radius;
        int AngleX;
        int AngleY;
        int AngleZ;
    } param[2];
    int fixMode;
    int version;
    int Reverse;
} ST_FISHEYE_PARAM, *LP_FISHEYE_PARAM;

typedef struct fisheyeParam2 {
    struct {
        float CenterCoordinateX;
        float CenterCoordinateY;
        float Radius;
        float AngleX;
        float AngleY;
        float AngleZ;
    } param[2];
    int fixMode;
    int version;
    int Reverse;
} ST_FISHEYE_PARAM2, *LP_FISHEYE_PARAM2;

typedef struct angle {
    void *data;
} ST_ANGLE, *LP_ANGLE;


typedef struct byPassFrame {
    int frameType;//特殊帧类型，目前只有鱼眼校正参数
    int dataSize;//data指向的数据结构大小
    void *data;//具体数据内容，目前指向ST_FISHEYE_PARAM
} ST_BYPASS_FRAME, *LP_BYPASS_FRAME;

////////////////////////////////
////////////// 下载相关
////////////////////////////////

#define JA_DL_VCON_APPNAME "ja_trans_file"
#pragma pack(push, 1)
/** 下行协议 */
enum {
    JA_DEV_FILE_TYPE_LIST = 0,  //0
    JA_DEV_FILE_TYPE_VIDEO_MP4,//1
    JA_DEV_FILE_TYPE_VIDEO_MOV,//2
    JA_DEV_FILE_TYPE_IMAGE_JPG,//3
    JA_DEV_FILE_TYPE_IMAGE_PNG,//4
};

typedef struct ja_dev_file_transfer {
    int file_type;  /* buf 过来的类型 */
    int dataSize;
    void *data;
} ST_JAFILETRANSFER, *LP_JAFILETRANSFER;


typedef struct ja_dev_file_list_item {  //当ja_dev_file_transfer.file_type被确定为JA_DEV_FILE_TYPE_LIST = 0时用
    uint32_t file_type;
    uint32_t file_size;
    uint32_t opera_flag;
    char suffix[16];
    char fp[256];
} ST_JAFILELISTITEM, *LP_JAFILELISTITEM;


typedef struct ja_dev_file_frame {  //当ja_dev_file_transfer.file_type被确定为JA_DEV_FILE_TYPE_VIDEO_MP4,JA_DEV_FILE_TYPE_VIDEO_MOV,/JA_DEV_FILE_TYPE_IMAGE_JPG,JA_DEV_FILE_TYPE_IMAGE_PNG时用
    uint32_t frame_type;
    uint32_t dataSize;
    uint32_t end;        /* 传输中是0，结束为1 */
    uint32_t opera_flag;
    void *data;
} ST_JAFILEFRAME, *LP_JAFILEFRAME;

/** 上行协议 */
enum {
    JA_DEV_FILE_OPERATE_TYPE_GETLIST = 0,
    JA_DEV_FILE_OPERATE_TYPE_GETLIST_VIDEO_ONLY,
    JA_DEV_FILE_OPERATE_TYPE_GETLIST_IAMGE_ONLY,
    JA_DEV_FILE_OPERATE_TYPE_DELETE,
    JA_DEV_FILE_OPERATE_TPYE_DOWNLOAD,
};

typedef struct ja_dev_file_operate {
    uint32_t opera_type;
    uint32_t opera_flag; //多个下载任务并行
    char fp[256];   //设备要被操作的文件路径
} ST_JADEVFILEOPERATE, *LP_JADEVFILEOPERATE;
#pragma pack(pop)


typedef struct JAConnect_Init {
	void *ctx;
	void (*OnFrame)(JAConnect *, unsigned char *Frame, int frame_len, int frame_type, void *ctx);
	void (*OnOSDFrame)(JAConnect *,unsigned char *Frame,int Frame_len,void *ctx,int64_t utctime);
    void (*OnGsensorData)(JAConnect *, uint64_t timeStamp, double x,double y,double z,void *ctx);
    void
    (*OnSearchData)(JAConnect *, unsigned int channel, unsigned char type, unsigned int begin_time,
                    unsigned int end_time, unsigned char quality, unsigned char isend, void *ctx);

    void (*OnStatus)(JAConnect *, void *ctx, int status_code, int content_code);

    void (*OnYUVFrame)(JAConnect *, unsigned char *y_Frame, unsigned char *u_Frame,
                       unsigned char *v_Frame, int frame_len, int frame_type, void *ctx);

    void (*OnYUVOSDFrame)(JAConnect *, unsigned char *y_Frame, unsigned char *u_Frame,
                          unsigned char *v_Frame, int Frame_len, void *ctx);

    void (*PlaybackTimeUpdate)(JAConnect *, int time, void *ctx);

    void (*OnVconData)(JAConnect *, char *data, int len, void *ctx);

    void (*OnVconFileOperate)(JAConnect *conn, void *data, int len, void *ctx);  /* 下载相关，vcon接收回调 */
    void (*OnAudioData)(JAConnect *, void *data, int len, void *ctx);

    void (*OnSearchDevice)(JAConnect *, struct SearchDeviceResult *pResult, void *ctx);

    void (*OnCaptureImage)(JAConnect *, bool success, char *path, void *ctx);

    void (*OnRecordVideo)(JAConnect *, bool success, char *path, void *ctx);

    void (*OnOSDTextTime)(JAConnect *,
#ifdef __ANDROID__
                          unsigned int OSDTime,
#else
            uint64_t OSDTime,
#endif
                          void *ctx);

    void (*OnPullAlarmmsg)(JAConnect *, struct P2PAlarmMsgData *msgData, void *ctx);

#ifdef __ANDROID__

    void (*OnNotSupportDirectTexture)(bool success, void *ctx);

    void (*OnDirectTextureFrameUpdata)(JAConnect *, void *directBuffer, void *ctx);

    void (*OnDirectTextureOSDFrameUpdata)(JAConnect *, void *textbuffer, void *ctx);

#endif

    void
    (*OnOpenChannel)(JAConnect *, int nChannelId, int nStreamId, const char *camDes, void *ctx);

    void (*OnDeviceInfo)(JAConnect *, bool isP720, void *ctx);

    void (*OnOOBFrameAvailable)(JAConnect *, int installMode, int scene, void *ctx);
} JAConnectInit, *PJAConnectInit;

//语音对讲
typedef struct JAConnect_Audio {
    void *voiceData;
    size_t dataSz;
    JAP2P_UINT64 tsMs;
    char enc[7];
    int samplerate;
    int samplewidth;
    int channels;
    float compressRatio;
} JAConnect_Audio_Packet, *PJAConnect_Audio_Packet;

typedef struct JAConnect_Cmd {
    int commandNo;
    char ConnectStr[80];
    char verifystr[200];
    long StartTime;
    long EndTime;
    int channelNo;
    int streamNo;
    int ptzCmd;
    unsigned char *data;
    int datasize;
    int alarmmsg_type;        //报警类型
    time_t ticket;            //报警时间戳
    PJAConnect_Audio_Packet paudioPacket;
    fOnSearchDeviceResult OnSearchDeviceResult;
} JAConnect_Command, *PJAConnect_Command;

void JAConnect_InitLib(const char *bundleid);
int JAConnect_InitAddr(const char *addr, unsigned short port);

#ifdef __cplusplus
}
#endif

class JAConnect {
private:
    void AddCommand(int cmdno,
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
                    fOnSearchDeviceResult OnSearchDeviceResult);

    void ParseConnectString(char *connstr, char *verifystr);

    void TaskLock();

    void TaskUnLock();

    int DoConnect(JAConnect_Command *cmd);

    int DoOpenChannel(JAConnect_Command *cmd, int bOpen);

    void DoDisconnect(JAConnect_Command *cmd);

    int DoSearchRec(JAConnect_Command *cmd);

    int DoStartPlayback(JAConnect_Command *cmd);

    int DoStopPlayback(JAConnect_Command *cmd);

    int DoPlaybackContinueGetdata(JAConnect_Command *cmd);

    int DoPlaybackPauseGetData(JAConnect_Command *cmd);

    int DoSendData(JAConnect_Command *cmd);

    int DoStartPtzAction(JAConnect_Command *cmd);

    int DoStopPtzAction(JAConnect_Command *cmd);

    int DoCall(JAConnect_Command *cmd);

    int DoHangUp(JAConnect_Command *cmd);

    int DoSendAudioPacket(JAConnect_Command *cmd);

    int DoSearchDevice(JAConnect_Command *cmd);

    int DoPullAlarmmsg(JAConnect_Command *cmd);

    void DecoderLock();

    void DecoderUnLock();


    void StopLock();

    void StopUnLock();

    void GetSPSVideoWH(void *buf, int buflen, int *w, int *h);

    long CreateVcon(const char *appName);

    int SendVconData(long vconHandle, void *buf, int buf_len);

    void DestroyVcon();

//解决卡顿问题
    void preRender(uint64_t presentationTimeUsec);


public:
    bool mAddBuffer;
    List<stFileRecord *> mSearchData;
    uint8_t *mVconbuffer;
    int mVconRecvLength;

    JAConnect(PJAConnectInit pCallback);

    ~JAConnect();

    void DoGsensorData(uint64_t timeStamp, double x,double y,double z);
    /**
     *  连接设备
     *
     *  @param connectstr ID / ip:port
     *  @param verifystr  verify / user:pwd
     */
    virtual void Connect(char *connectstr, char *verifystr);

    virtual void Connect(char *connectstr, char *verifystr, int channel);

    /**
     * 打开码流
     *
     * @param channelNo the dvr/nvr channdel number
     * @param StreamNo the video stream number
     */
    virtual void OpenChannel(int channelNo, int StreamNo);

    /**
     * 关闭码流
     *
     * @param channelNo
     * @param StreamNo
     */
    virtual void CloseChannel(int channelNo, int StreamNo);

    virtual void OpenDataChannel();

    virtual void CloseDataChannel();

    virtual void SendData(char *data, int datasize);

    virtual void SearchRec(long pStartTime, long pEndTime, int channelNo);

    virtual void StartPlayback(long pStartTime, int channelNo);

    virtual void StopPlayback(int channelNo);

    virtual void PlaybackPauseGetData(int channelNo);

    virtual void PlaybackContinueGetData(int channelNo);

    virtual void CtrlPTZ(int cmd, int channelNo);

    virtual void StopPTZ(int channelNo);

    virtual void Disconnect();

    virtual bool CaptureImage(char *filename, int type);

    virtual bool CaptureCurImage(char *filename);

    virtual bool RecordFile(char *filename);

    virtual void StopRecord();

    virtual int Call(int channelNo);

    virtual int HangUp();

    virtual int SendAudioPacket(void *voiceData,
                                size_t dataSz,
                                JAP2P_UINT64 tsMs,
                                char *enc,
                                int samplerate,
                                int samplewidth,
                                int channels,
                                float compressRatio);

    void PullAlarmmsg(int type, int channel, time_t ticket);

    virtual int getStreamNo();

    virtual void SearchDevice();

    void HandleTaskThread();

    void HandleDecodeThread();

    JAP2P_HANDLE getHandle();

    int AddBuffer(unsigned char *buf,
                  int buf_len,
                  int frame_type,
//#ifdef __ANDROID__
//	unsigned int pTimestamp,
//#else
                  uint64_t pTimestamp,
//#endif
                  unsigned int pPlaytime,
                  const char *_enc);

    void CleanBuffer();

    int GetFPS();

    int GetBitrate();

    int VideoWidth();

    int VideoHeight();

    int OSDWidth();

    int OSDHeight();

    JAMedia *GetDecoder();

    void DoStatus(int status_code, int content_code);

    void DoOnVconData(char *buf, int len);

    void DoSearchDevice(SearchDeviceResult *pResult);

    void DoCaptureImage(bool success, char *path);

    void DoRecordVideo(bool success, char *path);

    void DoPullAlarmmsg(struct P2PAlarmMsgData *msgData);

    void DoNotSupportDirectTexture(bool success);

    int GetChannel();

    bool GetRecordState();

    char *GetDevId();

    char *GetIp();

    bool GetIsConnected();

    void DoOnAudioData(uint8_t *buf, int buflen);

    void DoOnAudioData2(uint8_t *buf, int buflen, bool isAAC, bool isPlayback, bool haveheader);

    void SetOpanALQueueInfo(int pQueued, int pProcessed);

    void DecoderPause();

    void DecoderResume();

    void DoDisconnectThread();

    void setHowBirate(int value) { howBirate = value; }

    int getHowBirate() { return howBirate; }

    bool isNeedOpen64Stream();

    void setIsInstallModeCome(int value) { isInstallModeCome = value; }

    int getIsInstallModeCome() { return isInstallModeCome; }

    void setConnectNowStates(int value) { connectNowStates = value; }

    int getConnectNowStates() { return connectNowStates; }
    //void SetHardwareDecoder(bool value) { mHardwareDecoder = value;}
#ifdef __ANDROID__

    GraphicBuffer *GetGraphicBuffer();

#endif

    void UpdateCircleCenter(int radius, int centerx, int centery);

    char connectStr[80];
    char verifyStr[100];

    void DoOnOpenChannel(int nChannelId, int nStreamId, const char *camDes);

    /*
    *	Current haven't enable the disconnect method,
    *	this method is also a broken thread to disconnect,
    *	without waiting for the last time the end of the connection
    *
    */
    void DoDisconnect(); //测试断开的方法

    /*
     * Whether to enable the only solution I frame stream Settings,
     * when at the same time play eight or more than eight road video enabling this setting
     *
     * @param value
     * @type bool
     */
    void SetOnlyDecoderIFrame(bool value) { onlyDecodeIFrame = value; }

    void CancelTask();

    ////////////////新增函数///////////////////////////
    void
    GetFishEyeParameter(float *centerX, float *centerY, float *radius, float *angleX, float *angleY,
                        float *angleZ, int index);

    void setFishEyeParameter(float centerX, float centerY, float radius, float angleX, float angleY,
                             float angleZ, int index);

    void setDistTortion(void *pDist, int len);

    void *GetDistTortion(int *len);

    void doOnDeviceInfo(bool pIsP720);

    void doOnOOBFrameAvailable(int installMode, int scene);

    bool GetHasSetP720();

    void SetHasSetP720(bool pSet);

    bool GetHasSetP360();

    void SetHasSetP360(bool pSet);

    void GetFishEyeParameter360(float *centerX, float *centerY, float *radius);

    void setFishEyeParameter360(float centerX, float centerY, float radius);

    int CreateDownLoadVcon();

    int OperateDeviceFile(int optype, int opflag, const char *fp);

    int DestroyDownloadVcon();

    void doOnVconFileOperate(void *buf, int len);

    void SetHardwareDecoder(bool value, int width, int height) {
        mHardwareDecoder = value;
        if (!mHardwareDecoder)
            mRecvIFrame = false;
        mMaxSupportWidth = width;
        mMaxSupportHeight = height;
    };

    bool HardwareDecoder() { return mHardwareDecoder; };

    VCON_HANDLE *getDownLoadVconHandle();

    bool GetIsHEVC() { return mCurrentCodec; };

    uint64_t GetCurrentTimeStamp() { return mFirstFrameTimeStimp; };
	////// OSD时区设置
	////////////////////////
	void SetTimeZone(float pTimeZone);

    /*
     * 重连函数
     */
    int ReConnect();

    bool GetAreadyConnected() { return mAreadyConnected; };

    /*
     * 处理新 p2p 单链接多通道
     */

    JAP2PManager *CreateConnector();

    void DestroyConnector();

    int GetCurrentChannelNo() { return mChannelNo; };

    int GetRealStatus() { return mRealStatus; };

private:
    PJAConnectInit mCallback;

    pthread_t mTaskDisconnectThread;
    pthread_t mTaskThread;
    pthread_t mDecoderThread;
    pthread_t mAudioThread;   /* only for net record playing */
    pthread_mutex_t mDecoderLock;
    pthread_cond_t mDecoderCond;
    pthread_mutex_t mTaskLock;
    pthread_attr_t attr1;
    pthread_attr_t attr2;
    int mDecodeStatus;
    char mDeviceID[20];
    char mIP[80];
    short mPort;
    char mUser[20];
    char mPwd[40];
    char mVerify[200];
    int mRunThread;
    int mChannelNo;
    int mStreamNo;
    List<JAConnect_Command> mCommand;

    JAP2P_HANDLE mHandle;
    unsigned long mBuffer;
    int mBufferReader;

    JAMedia *mDecoder;
    unsigned char *mVideoBuffer;
    bool mIsConnected;
    int mRealStatus;
    int mPlaybackStatus;
//	PlaybackHandle					mPlaybackHandle;
    long mPlaybackHandle;
    uint64_t mLastTimestamp;
    uint64_t mFirstFrameTimeStimp;
    int mLastUpdateTime;
//	VCON_HANDLE						*mDataHandle;
    long mDataHandle;

    bool mIsAudioCall;
    bool mIsRecord;
    bool mIsRecordStart;
    bool mIsDisconnect;
    int mVideoWidth;
    int mVideoHeight;

    bool mBufferStart;

    //滤掉第一帧不是I帧
    bool mRecvIFrame;

    //调试音频
    FILE *audiofp;
    int mQueued;    /* OpenAL队列中剩余的buf数 每个buf 40 ms */
    int mProcessed; /* OpenAL队列中已播放的buf数 每个buf 40 ms */
    int mDelay;     /* audioPts-videoPts */
    uint64_t mVideoPts;  /* 视频显示时间戳 */
    uint64_t mAudioPts;  /* 音频播放时间戳 */
    int mfps; /* 回放帧率 */
    int isRecyceConnect;

    bool mIsH265;

    int howBirate;
    int delayTime;
    int delayTime1;

    int isInstallModeCome;
    int connectNowStates;
    int needOpen64;
    bool onlyDecodeIFrame;

    ////////////////新增变量///////////////////////////
    float mCenterX[2];
    float mCenterY[2];
    float mRadius[2];
    float mAngleX[2];
    float mAngleY[2];
    float mAngleZ[2];

    void *mDistTortion;
    int mDistLen;

    int mHasSetP720;

    bool mHardwareDecoder;
    int mMaxSupportWidth;
    int mMaxSupportHeight;

    bool mCurrentCodec;

///////// file download////////

//	VCON_HANDLE *mDLHandle;
    long mDLHandle;
    ////////////////////////
    JAP2PManager *mP2p;
    TJACallBackManager moutCallback;
    ////////////////////////
    float mTimeZone;              /////// 保存时区信息
    uint64_t mPrevPresentUsec = 0;
    uint64_t mPrevMonoUsec = 0;
    uint64_t mFixedFrameDurationUsec = 0;

    //处理重连
    bool mAreadyConnected;

};

JAConnect *FindConnector(JAP2P_HANDLE p_handle);


#endif //FISHEYE_JACONNECT_H
