#include "ConnectManager.h"

#include <string.h>
#include "libjnnat.h"

#define LOG_TAG "JAVideo"
#define LOGD(...)__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define LOGV(...)__android_log_print(ANDROID_LOG_DEBUG,"Lee",__VA_ARGS__)
#define INFO(...) {__android_log_write(ANDROID_LOG_INFO,"videoconnect",__VA_ARGS__);}
#define LOGE(...) {__android_log_print(ANDROID_LOG_ERROR, "videoconnect", __VA_ARGS__);}

void OnOSDTimeTxt(
        JAConnect *conn,
#ifdef __ANDROID__
        unsigned int OSDTime,
#else
        uint64_t OSDTime,
#endif
        void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
#if !defined(__ANDROID__)
    OSDTime = OSDTime / 1000;
#endif
    if (con->mgn->OnOSDTxtTime)
        con->mgn->OnOSDTxtTime(conn, OSDTime, con->index, con->ctx);
}

const char *DownLoadFile_path = NULL;
const char *FileNeedToDownLoad_name = NULL;
int DownLoadFile_size = 0;


void OnJAConnectGsensorData(JAConnect *conn, uint64_t timeStamp, double x, double y, double z,
                            void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (con)
        con->mgn->OnGsensorData(conn, timeStamp, x, y, z, con->ctx);
}


void
OnOpenChanneled(JAConnect *conn, int nChannelId, int nStreamId, const char *camDes, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (con && con->mgn && con->mgn->OnOpenChanneledSuccess)
        con->mgn->OnOpenChanneledSuccess(conn, nChannelId, nStreamId, camDes, con->ctx);
}

//解码后的视频帧回调
void OnFrame(JAConnect *conn, unsigned char *Frame, int frame_len, int frame_type, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (conn->HardwareDecoder() && frame_type == FRAME_TYPE_RAW) {
        if (con && con->mgn && con->mgn->OnTextureAvaible) {
            con->mgn->OnTextureAvaible(conn->VideoWidth(), conn->VideoHeight(), Frame, frame_len,
                                       frame_type, 0, con->index, con->conn);
        }
        return;
    }
    if (con && con->mgn && con->mgn->OnTextureAvaible) {
        con->mgn->OnTextureAvaible(conn->VideoWidth(), conn->VideoHeight(), Frame, frame_len,
                                   frame_type, 0, con->index, con->conn);
    }
    if (con && con->mgn && con->mgn->OnFrameDataCome) {
        con->mgn->OnFrameDataCome(con->index);
    }

    if (conn->VideoWidth() > 0 && conn->VideoHeight() > 0 && con->isPlaying == false) {
        con->isPlaying = true;
        if (con && con->mgn && con->mgn->OnPlayedFirstFrame) {
            con->mgn->OnPlayedFirstFrame(conn->VideoWidth(), conn->VideoHeight(), Frame, frame_len,
                                         frame_type, 0, con->index, con->ctx);
        }
    }

}

void OnOSDFrame(JAConnect *conn, unsigned char *Frame, int frame_len, void *ctx, int64_t utctime) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (con && con->mgn && con->mgn->OnOSDTextureAvaible) {
        con->mgn->OnOSDTextureAvaible(conn, Frame, frame_len, con->ctx, utctime);
    }
}

void
OnYUVFrame(JAConnect *conn, unsigned char *y_Frame, unsigned char *u_Frame, unsigned char *v_Frame,
           int frame_len, int frame_type, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (con && con->mgn && con->mgn->OnYUVTextureAvaible) {
        con->mgn->OnYUVTextureAvaible(conn->VideoWidth(), conn->VideoHeight(), y_Frame, u_Frame,
                                      frame_len, frame_type, 0, con->index, con->ctx);
    }
}

void OnYUVOSDFrame(JAConnect *conn, unsigned char *y_Frame, unsigned char *u_Frame,
                   unsigned char *v_Frame, int Frame_len, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (con && con->mgn && con->mgn->OnOSDYUVTextureAvaible) {
        con->mgn->OnOSDYUVTextureAvaible(conn, y_Frame, u_Frame, Frame_len, con->ctx);
    }
}

void
OnSearchData(JAConnect *conn, unsigned int channel, unsigned char type, unsigned int begin_time,
             unsigned int end_time, unsigned char quality, unsigned char isend, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (con && con->mgn && con->mgn->OnSearchRecData) {
        //for (int i = 0; i < conn->mSearchData.length(); i++)
        //	con->mgn->OnSearchRecData(conn, conn->mSearchData.at(i)->starttime, conn->mSearchData.at(i)->endtime, conn->mSearchData.at(i)->type, con->index, false, con->ctx);
        //con->mgn->OnSearchRecData(conn, 0, 0, 0, con->index, true, con->ctx);
        con->mgn->OnSearchRecData(conn, begin_time, end_time, type, con->index, isend, con->ctx);
    }
}

void OnConnectStatus(JAConnect *conn, void *ctx, int status_code, int content_code) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (con && con->mgn && con->mgn->OnStatus) {
        con->mgn->OnStatus(conn, con->ctx, status_code, content_code, con->index);

    }
}

void OnPlaybackTimeUpdate(JAConnect *conn, int time, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (con && con->mgn && con->mgn->OnPlaybackUpdateTime)
        con->mgn->OnPlaybackUpdateTime(conn, time, con->index, con->ctx);
}

void OnVconDataCallback(JAConnect *conn, char *data, int len, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;

    if (con && con->mgn && con->mgn->OnVconData)
        con->mgn->OnVconData(conn, data, len, con->index, con->ctx);
}

void OnAudioDataCome(JAConnect *conn, void *data, int len, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (con->mgn->OnAudioData && con->index == con->mgn->GetPlayAudioIndex()) {
        con->mgn->OnAudioData(conn, data, len, con->ctx);
    }
}

void OnImageCapture(JAConnect *conn, bool success, char *path, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (con && con->mgn && con->mgn->OnCaptureImage)
        con->mgn->OnCaptureImage(success, con->index, path);
}

void OnVideoRecord(JAConnect *conn, bool success, char *path, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (con && con->mgn && con->mgn->OnRecordVideo)
        con->mgn->OnRecordVideo(success, con->index, path);
}

void OnPullAlarmmsg(JAConnect *conn, struct P2PAlarmMsgData *msgData, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;

    if (con && con->mgn && con->mgn->OnPullAlarmmsg)
        con->mgn->OnPullAlarmmsg(conn, msgData, con->ctx);
}

void OnDeviceInfo(JAConnect *conn, bool isP720, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;

    if (con && con->mgn && con->mgn->OnDeviceInfo) {
        con->mgn->OnDeviceInfo(conn, isP720, con->ctx);
    }
}


void OnOOBFrameAvailable(JAConnect *conn, int installMode, int scene, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;

    if (con && con->mgn && con->mgn->OnOOBFrameAvailable) {
        con->mgn->OnOOBFrameAvailable(conn, installMode, scene, con->ctx);
    }
}

void OnVconFileOperateCallback(JAConnect *conn, void *data, int len, void *ctx);

void OnCheckTUTKDevOnlineListener(int result, void *ctx) {

    //LOGV("ConnectManager::OnCheckTUTKDevOnlineListener  result = %d ctx = %p", result, ctx);

    ConnectManager *con = (ConnectManager *) ctx;

    con->DoCheckTUTKDevOnlineListener(result, con->TutkFlag);
}

#ifdef __ANDROID__

void OnDirectTextureOSDFrameUpdatas(JAConnect *conn, void *textbuffer, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (con)
        con->mgn->OnDirectTextureOSDFrameUpdata(480, 24, textbuffer, con->index, con->ctx);
}

void OnConnDirectTextureFrameUpdata(JAConnect *conn, void *directBuffer, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (con && con->mgn)
        con->mgn->OnDirectTextureFrameUpdata(conn->VideoWidth(), conn->VideoHeight(), directBuffer,
                                             con->index, con->ctx);
}

void OnNotSupportDirectTexture(bool success, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
    if (con && con->mgn)
        con->mgn->OnNotSupportDirectTexture(success, con->ctx);
}

#endif // __ANDROID__

ConnectManager::ConnectManager(void *ctx) {
    OnGsensorData = NULL;
    OnTextureAvaible = NULL;
    OnYUVTextureAvaible = NULL;
    OnYUVTextureAvaible = NULL;
    OnCaptureImage = NULL;
    OnRecordVideo = NULL;
    OnOSDTextureAvaible = NULL;
    OnOSDYUVTextureAvaible = NULL;
    OnStatus = NULL;
    OnSearchRecData = NULL;
    OnPlaybackUpdateTime = NULL;
    OnVconData = NULL;
    OnAudioData = NULL;
    OnFrameDataCome = NULL;
    OnSearchDevice = NULL;
    OnOSDTxtTime = NULL;
    OnPullAlarmmsg = NULL;
    OnPlayedFirstFrame = NULL;
    OnDirectTextureOSDFrameUpdata = NULL;
    OnDirectTextureFrameUpdata = NULL;
    OnNotSupportDirectTexture = NULL;
    OnOpenChanneledSuccess = NULL;
    OnCheckTUTKDevOnline = NULL;
    _audioIndex = 0;
    memset(&_connector, 0, sizeof(_connector));
    _ctx = ctx;

    mDownloadDataPool = NULL;
    mDownloadDataPoolSize = 0;
    mIsDownloading = false;
    memset((void *) mDownloadSavePath, 0, 128);
}


ConnectManager::~ConnectManager() {
    for (int i = 0; i < 36; i++) {
        if (_connector[i].conn)
//            LOGV("Java_com_juanvision_video_GLVideoConnect_DestroyManager  delete _connector[i].conn前");
            delete _connector[i].conn;
//            LOGV("Java_com_juanvision_video_GLVideoConnect_DestroyManager  delete _connector[i].conn后");
//        }
    }
//    }
}


long long ConnectManager::GetConnectCtx(int index) {
    if (index < 0 || index >= MAX_CONN) {
        return 0;
    }
    return (long long) _connector[index].conn;
}


int ConnectManager::GetRealStatus(int index) {
    JAConnect *conn = _connector[index].conn;
    if (conn) {
        LOGD("hqh2 GetRealStatus 1");
        return conn->GetRealStatus();
    }
    LOGD("hqh2 GetRealStatus 2");
    return 0;
}


void *ConnectManager::Connect(char *connectstr, char *verifystr, int index) {
    return Connect(connectstr, verifystr, index, 0);
}

void *ConnectManager::Connect(char *connectstr, char *verifystr, int index, int channel) {
    if (index < 0 || index >= MAX_CONN)return NULL;

    if (!_connector[index].conn) {
        //LOGV("ConnectManager::Connect  index = %d",index);
        PJAConnectInit lcallback = (PJAConnectInit) malloc(sizeof(JAConnectInit));
        memset(lcallback, 0, sizeof(JAConnectInit));
        lcallback->ctx = &_connector[index];
        lcallback->OnOSDTextTime = OnOSDTimeTxt;
#ifdef __USE_RGB32__
        lcallback->OnFrame = OnFrame;
        lcallback->OnOSDFrame = OnOSDFrame;
#else
        lcallback->OnYUVFrame = OnYUVFrame;
        lcallback->OnYUVOSDFrame = OnYUVOSDFrame;
#endif
        lcallback->OnSearchData = OnSearchData;
        lcallback->OnStatus = OnConnectStatus;
        lcallback->PlaybackTimeUpdate = OnPlaybackTimeUpdate;
        lcallback->OnVconData = OnVconDataCallback;
        lcallback->OnAudioData = OnAudioDataCome;
        lcallback->OnCaptureImage = OnImageCapture;
        lcallback->OnRecordVideo = OnVideoRecord;
        lcallback->OnPullAlarmmsg = OnPullAlarmmsg;
        lcallback->OnOpenChannel = OnOpenChanneled;
        lcallback->OnDeviceInfo = OnDeviceInfo;
        lcallback->OnOOBFrameAvailable = OnOOBFrameAvailable;
        lcallback->OnVconFileOperate = OnVconFileOperateCallback;
        lcallback->OnGsensorData = OnJAConnectGsensorData;
#ifdef __ANDROID__
        lcallback->OnNotSupportDirectTexture = OnNotSupportDirectTexture;
        lcallback->OnDirectTextureFrameUpdata = OnConnDirectTextureFrameUpdata;
        lcallback->OnDirectTextureOSDFrameUpdata = OnDirectTextureOSDFrameUpdatas;
#endif // __ANDROID__
        _connector[index].index = index;
        _connector[index].conn = new JAConnect(lcallback);
        _connector[index].mgn = this;
        _connector[index].ctx = _ctx;
    }

    _connector[index].conn->CleanBuffer();
    _connector[index].conn->Connect(connectstr, verifystr, channel);
    LOGV("JAConnect::DoConnect   _connector[%d].conn->Connect(connectstr, verifystr, channel); ",
         index);
    _connector[index].isPlaying = false;
    return (void *) &_connector[index];
}

void ConnectManager::Disconnect(int index) {
    if (index < 0 || index >= MAX_CONN)return;
    if (_connector[index].conn) {
        //_connector[index].conn->CloseChannel(channel, streamid);
        _connector[index].conn->Disconnect();
        //	_connector[index].conn->GetDecoder()->EnableCrop(false);
    }
}

int ConnectManager::SendAudioPacket(void *voiceData,
                                    size_t dataSz,
                                    uint64_t tsMs,
                                    char *enc,
                                    int samplerate,
                                    int samplewidth,
                                    int channels,
                                    float compressRatio,
                                    int index) {
    if (index < 0 || index >= MAX_CONN)return -1;
    if (_connector[index].conn) {
        LOGV("ConnectManager----->voice----> %d", dataSz);
        return _connector[index].conn->SendAudioPacket(voiceData,
                                                       dataSz,
                                                       tsMs,
                                                       enc,
                                                       samplerate,
                                                       samplewidth,
                                                       channels,
                                                       compressRatio);
    } else {
        return -1;
    }
}

void ConnectManager::OpenChannel(int bitrate, int channel, int index) {
    if (index < 0 || index >= MAX_CONN)return;
    if (_connector[index].conn) {
        _connector[index].conn->OpenChannel(channel, bitrate);
    }
}

void ConnectManager::CloseChannel(int bitrate, int channel, int index) {
    if (index < 0 || index >= MAX_CONN)return;
    if (_connector[index].conn)
        _connector[index].conn->CloseChannel(channel, bitrate);
}

int ConnectManager::Call(int channelNo, int index) {
    if (index < 0 || index >= MAX_CONN)return -2;
    if (_connector[index].conn) {
        return _connector[index].conn->Call(channelNo);
    } else {
        return -1;
    }
}

int ConnectManager::HangUp(int index) {
    if (index < 0 || index >= MAX_CONN)return -2;
    if (_connector[index].conn) {
        return _connector[index].conn->HangUp();
    } else {
        return -1;
    }
}

void ConnectManager::SendData(char *data, int datasize, int index) {
    if (index < 0 || index >= MAX_CONN)return;
    if (_connector[index].conn) {
        _connector[index].conn->SendData(data, datasize);
    }
}

void ConnectManager::PausePlayback(int channel, int index) {
    if (index < 0 || index >= MAX_CONN)return;
    if (_connector[index].conn) {
        _connector[index].conn->PlaybackPauseGetData(channel);
    }
}

void ConnectManager::ResumePlayback(int channel, int index) {
    if (index < 0 || index >= MAX_CONN)return;
    if (_connector[index].conn) {
        _connector[index].conn->PlaybackContinueGetData(channel);
    }
}

int ConnectManager::GetStreamNo(int index) {
    if (index < 0 || index >= MAX_CONN)return -2;
    if (_connector[index].conn) {
        return _connector[index].conn->getStreamNo();
    }
    return -1;
}

void ConnectManager::PtzCtrl(int type, int channel, int index) {
    if (index < 0 || index >= MAX_CONN)return;
    _connector[index].conn->CtrlPTZ(type, channel);
}

void ConnectManager::StopPTZ(int channel, int index) {
    if (index < 0 || index >= MAX_CONN)return;
    if (_connector[index].conn)
        _connector[index].conn->StopPTZ(channel);
}

void ConnectManager::SearchRec(int startTime, int endTime, int channel, int index) {
    if (index < 0 || index >= MAX_CONN)return;
    if (_connector[index].conn)
        _connector[index].conn->SearchRec(startTime, endTime, channel);
}

void ConnectManager::StartPlayback(int startTime, int channel, int index) {
    if (index < 0 || index >= MAX_CONN)return;
    if (_connector[index].conn)
        _connector[index].conn->StartPlayback(startTime, channel);
}

void ConnectManager::StopPlayback(int channel, int index) {
    if (index < 0 || index >= MAX_CONN)return;
    if (_connector[index].conn)
        _connector[index].conn->StopPlayback(channel);
}

JAConnect *ConnectManager::GetConnector(int index) {
    if (index < 0 || index >= MAX_CONN) {
        return NULL;
    }
    if (_connector[index].conn) {
        return _connector[index].conn;
    }
    return NULL;
}

bool ConnectManager::CaptureImage(char *filename, int type, int index) {
    if (index < 0 || index >= MAX_CONN)
        return NULL;
    if (!_connector[index].conn)
        return false;
    return _connector[index].conn->CaptureImage(filename, type);
}

bool ConnectManager::CaptureCurImage(char *filename, int index) {
    if (index < 0 || index >= MAX_CONN)
        return false;
    if (!_connector[index].conn)
        return false;
    return _connector[index].conn->CaptureCurImage(filename);
}

bool ConnectManager::StartRecord(char *filename, int index) {
    if (index < 0 || index >= MAX_CONN)
        return false;
    if (!_connector[index].conn)
        return false;
    return _connector[index].conn->RecordFile(filename);
}

void ConnectManager::StopRecord(int index) {
    if (index < 0 || index >= MAX_CONN)
        return;
    if (!_connector[index].conn)
        return;
    _connector[index].conn->StopRecord();
}

int ConnectManager::GetNetWorkSpeed(int index) {
    if (index < 0 || index >= MAX_CONN)
        return -1;
    if (_connector[index].conn) {
        return _connector[index].conn->GetBitrate();
    }
    return 0;
}

int ConnectManager::GetAllNetWorkSpeed() {
    int i;
    int all = 0;
    for (i = 0; i < MAX_CONN; i++) {
        if (_connector[i].conn) {
            all = all + _connector[i].conn->GetBitrate();
        }
    }
    return all;
}

int ConnectManager::GetChannel(int index) {
    if (index < 0 || index >= MAX_CONN)
        return -2;
    if (_connector[index].conn) {
        return _connector[index].conn->GetChannel();
    }
    return -1;
}

bool ConnectManager::GetRecordState(int index) {
    if (index < 0 || index >= MAX_CONN)
        return false;
    if (_connector[index].conn) {
        return _connector[index].conn->GetRecordState();
    }
    return false;
}

void ConnectManager::LivePause() {
    for (int i = 0; i < MAX_CONN; i++) {
        if (_connector[i].conn) {
            int channel = _connector[i].conn->GetChannel();
            int streamNo = _connector[i].conn->getStreamNo();
            _connector[i].conn->CloseChannel(channel, streamNo);
        }
    }
}

void ConnectManager::LiveResume() {
    for (int i = 0; i < MAX_CONN; i++) {
        if (_connector[i].conn) {
            int channel = _connector[i].conn->GetChannel();
            int streamNo = _connector[i].conn->getStreamNo();
            _connector[i].conn->OpenChannel(channel, streamNo);
        }
    }
}

void ConnectManager::SearchDevice() {
    if (_connector[0].conn)
        _connector[0].conn->SearchDevice();
}

void ConnectManager::PullAlarmmsg(int type, int channle, time_t time, int index) {
    if (index < 0 || index >= MAX_CONN)return;
    if (_connector[index].conn)
        _connector[index].conn->PullAlarmmsg(type, channle, time);
}

void ConnectManager::UseDirectTexture(bool isDirect) {
    for (int i = 0; i < MAX_CONN; i++) {
        if (_connector[i].conn) {
            _connector[i].conn->GetDecoder()->isDirectTexture = isDirect;
        }
    }
}

void ConnectManager::DecoderPause(int index) {
    if (index < 0 || index >= MAX_CONN)return;
    if (_connector[index].conn) {
        _connector[index].conn->DecoderPause();
    }
}

void ConnectManager::SetHardwareDecoder(bool value, int index, int width, int height) {
    if (index < 0 || index >= MAX_CONN)return;
    if (_connector[index].conn) {
        _connector[index].conn->SetHardwareDecoder(value, width, height);
    }
}

void ConnectManager::DecoderResume(int index) {
    if (index < 0 || index >= MAX_CONN)return;
    if (_connector[index].conn) {
        _connector[index].conn->DecoderResume();
    }
}

bool ConnectManager::IsCrop(int index) {
    if (index < 0 || index >= MAX_CONN)
        return false;
    return _connector[index].conn->GetDecoder()->GetCrop();
}

bool ConnectManager::IsWallMode(int index) {
    if (index < 0 || index >= MAX_CONN)
        return false;
    return _connector[index].conn->GetDecoder()->WallMode();
}

void ConnectManager::EnableCrop(int index) {
    if (index < 0 || index >= MAX_CONN)
        return;
    _connector[index].conn->GetDecoder()->EnableCrop(true);
}

void ConnectManager::DisableCrop(int index) {
    if (index < 0 || index >= MAX_CONN)
        return;
    _connector[index].conn->GetDecoder()->EnableCrop(false);
}

void ConnectManager::WriteTextOSDDirectTextureFrame(unsigned char *txttex) {
    if (NULL == _connector[0].conn)return;
    _connector[0].conn->GetDecoder()->WriteTextOSDDirectTextureFrame(txttex);
}

JAMedia *ConnectManager::GetDecoder(int index) {
    if (index < 0 || index >= MAX_CONN) {
        return NULL;
    }
    return _connector[index].conn->GetDecoder();
}

void *ConnectManager::GetCtx() {
    return _ctx;
}

void ConnectManager::PlaybackPauseGetData(int channel, int index) {
    if (index < 0 || index >= MAX_CONN)
        return;
    _connector[index].conn->PlaybackPauseGetData(channel);
}

void ConnectManager::PlaybackContinueGetData(int channel, int index) {
    if (index < 0 || index >= MAX_CONN)
        return;
    _connector[index].conn->PlaybackContinueGetData(channel);
}

int ConnectManager::GetBitrate(int index) {
    if (index < 0 || index >= MAX_CONN)
        return 0;
    if (_connector[index].conn)
        return _connector[index].conn->getStreamNo();
    return 0;
}

void ConnectManager::DoDisconnect(int index) {
    if (index < 0 || index >= MAX_CONN)
        return;
    if (_connector[index].conn)
        _connector[index].conn->DoDisconnect();

}

int ConnectManager::GetFps(int index) {
    if (index < 0 || index >= MAX_CONN)
        return 0;
    if (_connector[index].conn)
        return _connector[index].conn->GetFPS();
}

int ConnectManager::getHowBitrate(int index) {
    if (index < 0 || index >= MAX_CONN)
        return 2;
    if (_connector[index].conn)
        return _connector[index].conn->getHowBirate();
}

bool ConnectManager::isNeedOpen64Stream(int index) {
    if (index < 0 || index >= MAX_CONN)
        return false;
    if (_connector[index].conn)
        return _connector[index].conn->isNeedOpen64Stream();
}

int ConnectManager::getIsInstallModeCome(int index) {
    if (index < 0 || index >= MAX_CONN)
        return 3;
    if (_connector[index].conn)
        return _connector[index].conn->getIsInstallModeCome();
}

void ConnectManager::setOnlyDecoderIFrame(bool value, int index) {
    if (index < 0 || index >= MAX_CONN)
        return;
    if (_connector[index].conn)
        _connector[index].conn->SetOnlyDecoderIFrame(value);
}


void ConnectManager::GetfishEyeParam(float *centerX, float *centerY, float *radius, float *angleX,
                                     float *angleY, float *angleZ, int chl_index, int index) {
    JAConnect *conn = _connector[chl_index].conn;

    if (!conn)
        return;

    conn->GetFishEyeParameter(centerX, centerY, radius, angleX, angleY, angleZ, index);
}


void *ConnectManager::GetDistTortion(int chl_index, int *dist_len) {
    JAConnect *conn = _connector[chl_index].conn;
    if (!conn)
        return NULL;

    return conn->GetDistTortion(dist_len);
}


void OnVconFileOperateCallback(JAConnect *conn, void *data, int len, void *ctx) {
    JACONNECT *con = (JACONNECT *) ctx;
    /*if (con->mgn->OnDeviceFileOperate)
        con->mgn->OnDeviceFileOperate(conn, data, len, con->index, con->ctx);
    LOGE("OnVconFileOperateCallback");*/

    con->mgn->doOnFileDownloadCallback(data, len, ctx);
}

////////////////////////////
/////// 文件下载
////////////////////////////



char *ConnectManager::GetFileDownloadList(int idx) {

    LOGV("ConnectManager::GetFileDownloadList");
    //创建虚拟通道
    OperateDeviceFile(0, 0, NULL, NULL, 0);

    return NULL;
}

void ConnectManager::StopDownloadThread(bool value) {
    mIsDownloading = value;
}

void ConnectManager::DoDownloadFile(int opType, int opFlag, const char *fp, const char *file_path,
                                    int index) {
    LOGV("ConnectManager::DoDownloadFile ");
    //设置全局变量file_path为安卓传入的文件下载路径
    DownLoadFile_path = file_path;
    //FileNeedToDownLoad_name = fp;
    OperateDeviceFile(opType, opFlag, fp, file_path, index);
}

void *doDownLoadThread(void *args) {

    ConnectManager *c = (ConnectManager *) args;
    c->doDownloadThread();
    pthread_exit(NULL);
    return NULL;
}

int ConnectManager::OperateDeviceFile(int opType, int opFlag, const char *fp, const char *savePath,
                                      int idx) {
    if (idx > 32 || idx < 0) {
        return -1;
    }
    LOGV("ConnectManager::OperateDeviceFile---------------------> ");
    JAConnect *conn = _connector[idx].conn;


    if (!mIsDownloading && conn) {
        mIsDownloading = true;
        mDownloadDataPool = malloc(1024 * 1024 * 2);
        memset(mDownloadDataPool, 0, 1024 * 1024 * 2);
        if (savePath) {
            strcpy(mDownloadSavePath, savePath);
        }

        pthread_mutex_init(&mDownloadLock, NULL);
        pthread_create(&mDownloadThread, NULL, doDownLoadThread, (void *) this);

        conn->OperateDeviceFile(opType, opFlag, fp);
    }

    return 0;
}

int ConnectManager::FinishDeviceFileTransfer(int idx) {
    if (idx < 0 || idx > 32)
        return -1;
    LOGV("ConnectManager::----->FinishDeviceFileTransfer");
    JAConnect *conn = _connector[idx].conn;
    conn->DestroyDownloadVcon();

    mIsDownloading = false;
    pthread_join(mDownloadThread, NULL);

    pthread_mutex_destroy(&mDownloadLock);

    /////// 释放资源
    if (mDownloadDataPool) {
        free(mDownloadDataPool);
        mDownloadDataPool = NULL;
    }

    mDownloadDataPoolSize = 0;
    memset((void *) mDownloadSavePath, 0, 128);

    return 0;
}

void ConnectManager::doOnFileDownloadCallback(void *data, int len, void *ctx) {
    if (mIsDownloading) {
        pthread_mutex_lock(&mDownloadLock);
        memcpy((void *) ((long) mDownloadDataPool + mDownloadDataPoolSize), (const void *) data,
               len);
        mDownloadDataPoolSize += len;
        pthread_mutex_unlock(&mDownloadLock);
    }
}

void ConnectManager::doDownloadThread() {
    int mixLen = 0;
    const int minLen = 8;
    LP_JAFILETRANSFER head = NULL;
    LP_JAFILELISTITEM llist = NULL;
    LP_JAFILEFRAME ff = NULL;
    int lfileType = 0;
    int lDataSize = 0;
    bool abort = false;

    FILE *f = NULL;
    int savePathLen = strlen((const char *) mDownloadSavePath);
    LOGV("ConnectManager:: -------------------------->FILE %d", savePathLen);
    if (savePathLen > 0) {
        LOGV("ConnectManager:: -------------------------->if");
        remove((const char *) mDownloadSavePath);
        f = fopen((const char *) mDownloadSavePath, "ab");
        if (!f)
            return;
    }

//    LOGV("ConnectManager:: ---------------sdadasdas----------->");
    do {
        usleep(100);

        LOGV("ConnectManager:: ---------------111111----------->");
        if (mDownloadDataPoolSize < minLen) {
            continue;
        }

        pthread_mutex_lock(&mDownloadLock);  ////// ++++++++++++++++++++++++++++++++++++
        head = (LP_JAFILETRANSFER) mDownloadDataPool;
        lfileType = ntohl(head->file_type);
        lDataSize = ntohl(head->dataSize);
        mixLen = minLen + lDataSize;

        LOGV("ConnectManager:: ---------------22222222----------->");
        if (mDownloadDataPoolSize < mixLen) {
            pthread_mutex_unlock(&mDownloadLock); ///// -----------------------------------
            continue;
        }
        LOGV("ConnectManager:: ---------------33333----------->");

        switch (lfileType) {
            case JA_DEV_FILE_TYPE_LIST: {
                llist = (LP_JAFILELISTITEM) ((long) head + 8);
                int i = 0;
                int itemFileType = 0;
                int itemFileSize = 0;
                int itemOperatFlag = 0;
                for (i = 0; i < (lDataSize) / (sizeof(ST_JAFILELISTITEM)); i++) {
                    ST_JAFILELISTITEM item = llist[i];
                    itemFileType = ntohl(item.file_type);
                    itemFileSize = ntohl(item.file_size);
                    itemOperatFlag = ntohl(item.opera_flag);

                    //////
                    if (i < (lDataSize / sizeof(ST_JAFILELISTITEM)) - 1) {
                        OnDownloadList(item.fp, itemFileSize, item.suffix, itemFileType, 0);
                    } else {
                        OnDownloadList(item.fp, itemFileSize, item.suffix, itemFileType, 1);
                    }


                }
                mIsDownloading = false;
            }
                break;

            case JA_DEV_FILE_TYPE_IMAGE_JPG:
            case JA_DEV_FILE_TYPE_IMAGE_PNG:
            case JA_DEV_FILE_TYPE_VIDEO_MOV:
            case JA_DEV_FILE_TYPE_VIDEO_MP4: {

                LOGV("v:: ---------JA_DEV_FILE_TYPE_VIDEO_MP4----------------->%d", lfileType);

                ff = (LP_JAFILEFRAME) ((long) head + 8);
                int flag = ntohl(ff->opera_flag);
                uint32_t ffFrameType = ntohl(ff->frame_type);
                uint32_t ffFileSize = ntohl(ff->dataSize);
                LOGV("ConnectManager:: ---------JA_DEV_FILE_TYPE_VIDEO_MP4----------------->%d",
                     JA_DEV_FILE_TYPE_VIDEO_MP4);

                uint32_t ffEnd = ntohl(ff->end);
                size_t wirteSize = 0;
                long cuPos = 0;


                wirteSize = fwrite((const void *) ((long) ff + 16), ffFileSize, 1, f);

                LOGV("ConnectManager:: ------------wirteSize-------------->%d", wirteSize);

                if (wirteSize == 1) {
                    printf("文件写入成功");
                    cuPos = ftell(f);
                    LOGV("ConnectManager:: ---------文件写入成功----------------->%d");
                    /////// 回调进度 cuPos/整个文件的大小
                    OnDownloading(flag, ffFileSize, cuPos, ffEnd);

                } else {
                    mIsDownloading = false;
                    /////// 回调下载失败
                    break;
                }

                if (ffEnd == 1) {
                    ////// 回调下载成功
                    OnDownloading(flag, ffFileSize, cuPos, ffEnd);
                    mIsDownloading = false;
                }

            }
                break;
            default:
                break;
        }


        size_t moveSize = 0;
        moveSize = mDownloadDataPoolSize - mixLen;
        memmove(mDownloadDataPool, (void *) ((long) mDownloadDataPool + mixLen), moveSize);
        mDownloadDataPoolSize = (int) moveSize;
        ///// 这里可以打印一下 dataPoolSize

        pthread_mutex_unlock(&mDownloadLock); ///// -----------------------------------

    } while (mIsDownloading);


    if (f) {
        fclose(f);
        f = NULL;
    }

    mDownloadDataPoolSize = 0;
}

////////////////////////////
///////
////////////////////////////


int ConnectManager::doCheckTutkOnline(char *tutkid, unsigned int timeout, char *msg) {
//    LOGV("ConnectManager::doCheckTutkOnline  timeout = %d", timeout);
    strcpy(TutkFlag, msg);
    return ja_p2p_check_tutk_dev_online(tutkid, timeout, OnCheckTUTKDevOnlineListener, this);
    //return 0;
}

void ConnectManager::DoCheckTUTKDevOnlineListener(int result, char *msg) {
    OnCheckTUTKDevOnline(result, msg);
}

void ConnectManager::SetTimeZone(float pTimeZone, int index) {
    if (index < 0 || index > 36)
        return;

    JAConnect *conn = _connector[index].conn;
    if (!conn)
        return;
    conn->SetTimeZone(pTimeZone);
}











