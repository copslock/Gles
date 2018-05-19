#pragma once
#ifndef CONNECTMANAGER
#define CONNECTMANAGER

#include "../GLES/GLESMath.h"
#include "JAConnect.h"
#include "../Utils/list.h"
#include "../Media/JAText.h"
#include <unistd.h>

class ConnectManager;

typedef struct struct_jaconnect {
    int index;
    bool isPlaying;
    ConnectManager *mgn;
    void *ctx;
    JAConnect *conn;
} JACONNECT, *PJACONNECT;

class ConnectManager {
private:
    JACONNECT _connector[MAX_CONN];
    bool mPlayaudio;
    int _audioIndex;

    void *mDownloadDataPool;
    int mDownloadDataPoolSize;
    pthread_t mDownloadThread;
    pthread_mutex_t mDownloadLock;
    char mDownloadSavePath[128];
    bool mIsDownloading;

public:
    void *_ctx;
    char TutkFlag[32];
    void (*OnGsensorData)(JAConnect *,uint64_t timeStamp, double x,double y,double z,void *ctx);
    void
    (*OnTextureAvaible)(int w, int h, void *frame, int frame_len, int frame_type, int timestamp,
                        int index, void *ctx);

    void (*OnYUVTextureAvaible)(int w, int h, void *frame_y, void *frame_cbcr, int frame_len,
                                int frame_type, int timestamp, int index, void *ctx);

    void (*OnCaptureImage)(bool success, int index, char *path);

    void (*OnRecordVideo)(bool success, int index, char *path);

    void
    (*OnOSDTextureAvaible)(JAConnect *con, void *frame, int frame_len, void *ctx, int64_t utctime);

    void (*OnOSDYUVTextureAvaible)(JAConnect *con, void *frame_y, void *frame_cbcr, int frame_len,
                                   void *ctx);

    void (*OnStatus)(JAConnect *con, void *ctx, int status_code, int content_code, int index);

    void (*OnSearchRecData)(JAConnect *con, int starttime, int endtime, int rectype, int index,
                            bool theend, void *ctx);

    void (*OnPlaybackUpdateTime)(JAConnect *con, int time, int index, void *ctx);

    void (*OnVconData)(JAConnect *con, char *data, int len, int index, void *ctx);

    void (*OnAudioData)(JAConnect *con, void *data, int len, void *ctx);

    void (*OnFrameDataCome)(int index);

    void (*OnSearchDevice)(JAConnect *con, SearchDeviceResult *pResult, void *ctx);

    void (*OnOSDTxtTime)(JAConnect *con, unsigned int OSDTime, int index, void *ctx);

    void (*OnPullAlarmmsg)(JAConnect *con, struct P2PAlarmMsgData *msgData, void *ctx);

    void
    (*OnPlayedFirstFrame)(int w, int h, void *frame, int frame_len, int frame_type, int timestamp,
                          int index, void *ctx);

    void (*OnDirectTextureOSDFrameUpdata)(int w, int h, void *textBuffer, int index, void *ctx);

    void (*OnDirectTextureFrameUpdata)(int w, int h, void *directBuffer, int index, void *ctx);

    void (*OnNotSupportDirectTexture)(bool success, void *ctx);

    void
    (*OnOpenChanneledSuccess)(JAConnect *con, int nChannelId, int nStreamId, const char *camDes,
                              void *ctx);

    void (*OnDeviceInfo)(JAConnect *con, bool pIsP720, void *ctx);

    void (*OnOOBFrameAvailable)(JAConnect *con, int installMode, int scene, void *ctx);

    void (*OnDeviceFileOperate)(JAConnect *conn, void *data, int len, int index, void *ctx);

    void (*OnDownloadList)(char *item_fp, int item_size, char *suffix, int item_type, int flag);

    void (*OnDownloading)(int flag, int fdsize, long pos, int end);

    void (*OnCheckTUTKDevOnline)(int result, char *msg);

    bool isDestroying = false;

    ConnectManager(void *ctx);

    ~ConnectManager();

    long long GetConnectCtx(int index);

    int GetRealStatus(int index);


    /**
    * �����豸��������
    */
    void *Connect(char *connectstr, char *verifystr, int index);

    void *Connect(char *connectstr, char *verifystr, int index, int channel);

    /**
    * �Ͽ�����
    */
    void Disconnect(int index);

    void DoDisconnect(int index);//���ӶϿ���ʵ���Է���
    /**
    * ��ͨ��ʵʱ��Ƶ
    */
    void OpenChannel(int bitrate, int channel, int index);

    /**
    * �ر�ͨ��ʵʱ��Ƶ
    */
    void CloseChannel(int bitrate, int channel, int index);

    void *GetCtx();

    int SendAudioPacket(void *voiceData,
                        size_t dataSz,
                        uint64_t tsMs,
                        char *enc,
                        int samplerate,
                        int samplewidth,
                        int channels,
                        float compressRatio,
                        int index);

    int Call(int channelNo, int index);

    int HangUp(int index);

    void SendData(char *data, int datasize, int index);

    void PausePlayback(int channel, int index);

    void ResumePlayback(int channel, int index);

    int GetStreamNo(int index);

    void PtzCtrl(int type, int channel, int index);

    void StopPTZ(int channel, int index);

    void SearchRec(int startTime, int endTime, int channel, int index);

    void StartPlayback(int startTime, int channel, int index);

    void StopPlayback(int channel, int index);

    JAConnect *GetConnector(int index);

    bool CaptureImage(char *filename, int type, int index);

    bool CaptureCurImage(char *filename, int index);

    bool StartRecord(char *filename, int index);

    void StopRecord(int index);

    int GetNetWorkSpeed(int index);

    int GetAllNetWorkSpeed();

    int GetChannel(int index);

    bool GetRecordState(int index);

    void LivePause();

    void LiveResume();

    void SearchDevice();

    void PullAlarmmsg(int type, int channle, time_t time, int index);

    /*
    * Whether to use hardware acceleration
    */
    void UseDirectTexture(bool isDirect);

    void DecoderPause(int index);

    void DecoderResume(int index);

    bool IsCrop(int index);

    bool IsWallMode(int index);

    void EnableCrop(int index);

    void DisableCrop(int index);

    JAMedia *GetDecoder(int index);

    void WriteTextOSDDirectTextureFrame(unsigned char *txttex);

    void PlaybackPauseGetData(int channel, int index);

    void PlaybackContinueGetData(int channel, int index);

    int GetBitrate(int index);

    void PlayAudio(int index) { _audioIndex = index; };

    int GetPlayAudioIndex() { return _audioIndex; };

    int GetFps(int index);

    int getHowBitrate(int index);

    bool isNeedOpen64Stream(int index);

    int getIsInstallModeCome(int index);

    void setOnlyDecoderIFrame(bool value, int index);

    void
    GetfishEyeParam(float *centerX, float *centerY, float *radius, float *angleX, float *angleY,
                    float *angleZ, int chl_index, int index);

    void *GetDistTortion(int chl_index, int *dist_len);

    ////// 文件下载
    char *GetFileDownloadList(int idx);

    void StopDownloadThread(bool value);

    void DoDownloadFile(int opType, int opFlag, const char *fp, const char *file_path, int index);

    int OperateDeviceFile(int opType, int opFlag, const char *fp, const char *savePath, int idx);

    int FinishDeviceFileTransfer(int idx);

    void doOnFileDownloadCallback(void *data, int len, void *ctx);

    void doDownloadThread();


    int doCheckTutkOnline(char *tutkid, unsigned int timeout, char *msg);

    void DoCheckTUTKDevOnlineListener(int result, char *msg);

    void SetHardwareDecoder(bool value, int index, int width, int height);

    void SetTimeZone(float pTimeZone, int index);
};

#endif
