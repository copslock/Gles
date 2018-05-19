package com.app.jagles.video;

import android.util.Log;

import com.app.jagles.networkCallback.OnCaptureImageListener;
import com.app.jagles.networkCallback.OnOSDTxtTimeListener;
import com.app.jagles.networkCallback.OnPlaybackUpdateTimeListener;
import com.app.jagles.networkCallback.OnPullAlarmmsgListener;
import com.app.jagles.networkCallback.OnSearchDeviceListener;
import com.app.jagles.networkCallback.OnSearchRecDataListener;
import com.app.jagles.networkCallback.OnVconDataListener;

/**
 * Created by MiQi on 2016/10/18.
 */

public class Manager {
    public String msg = "";
    private GLVideoDisplay mGLVideoDisplay = null;
    private static Manager mManager;

    /**
     * 带bundleid的私有实例化
     *
     * @param bundleid
     */
    private Manager(String bundleid) {
        //     init(bundleid);
    }

    /**
     * 带bundleid的私有化实例
     */
    private Manager() {
        //    init(null);
    }

    /**
     * 获取一个实例
     *
     * @param mGLVideoDisplay
     * @return
     */
    public static Manager getInstance(final GLVideoDisplay mGLVideoDisplay) {
        if (mManager == null) {
            mManager = new Manager();
            mManager.mGLVideoDisplay = mGLVideoDisplay;
            mManager.init("");
        } else {
            mManager.mGLVideoDisplay = mGLVideoDisplay;
            mManager.init("");
        }
        return mManager;
    }

    /**
     * 获取一个实例
     *
     * @param mGLVideoDisplay
     * @param bundleid
     * @return
     */
    public static Manager getInstance(final GLVideoDisplay mGLVideoDisplay, String bundleid) {
        if (mManager == null) {
            mManager = new Manager(bundleid);
            mManager.mGLVideoDisplay = mGLVideoDisplay;
        } else {
            mManager.mGLVideoDisplay = mGLVideoDisplay;
            mManager.init(bundleid);
        }
        return mManager;
    }

    public static Manager getInstance(String bundleid) {
        if (mManager == null) {
            mManager = new Manager(bundleid);
        } else {
            mManager.init(bundleid);
        }
        return mManager;
    }

    /**
     * 设置一些必要的回调函数
     *
     * @param bundleid
     */
    private void init(String bundleid) {
        GLVideoConnect.getInstance(bundleid).mOnStatusListener = mGLVideoDisplay.mRender;
        GLVideoConnect.getInstance().mOnTextureAvaibleListener = mGLVideoDisplay.mRender;
        GLVideoConnect.getInstance().mOnDirectTextureFrameUpdataListener = mGLVideoDisplay.mRender;
        GLVideoConnect.getInstance().mOnOSDTextureAvaibleListener = mGLVideoDisplay.mRender;
        GLVideoConnect.getInstance().mOnAudioDataComeListener = mGLVideoDisplay.mRender;
        GLVideoConnect.getInstance().mOnPlayedFirstFrameListener = mGLVideoDisplay.mRender;
    }

    public int setAddr(String addr, int port) {
        return GLVideoConnect.getInstance().setAddr(addr, port);
    }

    /**
     * 底层生成一个带身份msg的网络连接实例
     *
     * @param msg
     * @return
     */
    public String getConnect(String msg) {
        this.msg = msg;
        return GLVideoConnect.getInstance().GetConnect(msg);
    }

    /**
     * 根据一个带身份msg的连接实例进行对设备进行连接
     *
     * @param msg
     * @param idOrIp
     * @param verify
     * @param index
     */
    public void connect(String msg, String idOrIp, String verify, int index) {
        GLVideoConnect.getInstance().Connect(msg, idOrIp, verify, index);
        if (mGLVideoDisplay != null) {
            if (index == 0)
                GLVideoConnect.getInstance().SetHardwareDecoder(msg, mGLVideoDisplay.mRender.mHardwareDecoder, index, mGLVideoDisplay.mRender.mMaxSupportWidth, mGLVideoDisplay.mRender.mMaxSupportHeight);
            mGLVideoDisplay.ShowVideoLoading(index);
            mGLVideoDisplay.msg = msg;
        }
    }

    /**
     * 根据一个带身份msg的连接实例进行对设备进行连接
     *
     * @param msg
     * @param idOrIp
     * @param verify
     * @param index
     * @param channel
     */
    public void connect(String msg, String idOrIp, String verify, int index, int channel) {
        GLVideoConnect.getInstance().Connect(msg, idOrIp, verify, index, channel);
        if (mGLVideoDisplay != null) {
            mGLVideoDisplay.ShowVideoLoading(index);
            mGLVideoDisplay.msg = msg;
        }
    }

    /**
     * 断开链接
     *
     * @param msg
     * @param index
     */
    public void disConnect(String msg, int index) {
        GLVideoConnect.getInstance().DisConnect(msg, index);
    }

    //断开实验性接口
    public void doDisConnect(String msg, int index) {
        GLVideoConnect.getInstance().DoDisConnect(msg, index);
    }

    public String doGetFileDownloadList(String msg, int index) {
        return GLVideoConnect.getInstance().getFileDownloadList(msg, index);
    }

    public void doDownloadFile(String msg, int opType, int opFlag, String file_name, String suffix, int index) {
        GLVideoConnect.getInstance().doDownloadFile(msg, opType, opFlag, file_name, suffix, index);
    }

    public void doStopDownloadFile(String msg) {
        GLVideoConnect.getInstance().doStopDownload(msg);
    }

    /**
     * 打开设备视频直播码流
     *
     * @param msg
     * @param bitrate
     * @param channel
     * @param index
     */
    public void openVideo(String msg, int bitrate, int channel, int index) {
        mGLVideoDisplay.msg = msg;
        Log.d("GLVideoConnect", "GLVideoConnect open video");
        GLVideoConnect.getInstance().OpenChannel(msg, bitrate, channel, index);
        if (index == 0)
            GLVideoConnect.getInstance().SetHardwareDecoder(msg, mGLVideoDisplay.mRender.mHardwareDecoder, index, mGLVideoDisplay.mRender.mMaxSupportWidth, mGLVideoDisplay.mRender.mMaxSupportHeight);
        mGLVideoDisplay.ShowVideoLoading(index);
    }

    /**
     * 关闭视频直播码流
     *
     * @param msg
     * @param bitrate
     * @param channel
     * @param index
     */
    public void closeVideo(String msg, int bitrate, int channel, int index) {
        GLVideoConnect.getInstance().CloseChannel(msg, bitrate, channel, index);
    }

    /**
     * 暂停远程回放的视频码流
     *
     * @param msg
     * @param channel
     * @param index
     */
    public void pausePlayback(String msg, int channel, int index) {
        GLVideoConnect.getInstance().PausePlayback(msg, channel, index);
    }

    /**
     * 继续播放远程回放
     *
     * @param msg
     * @param channel
     * @param index
     */
    public void resumePlayback(String msg, int channel, int index) {
        GLVideoConnect.getInstance().ResumePlayback(msg, channel, index);
    }

    /**
     * 开始播放某个时间段的远程回放录像
     *
     * @param msg
     * @param startTime
     * @param channel
     * @param index
     */
    public void startPlayback(String msg, int startTime, int channel, int index) {
        mGLVideoDisplay.msg = msg;
        GLVideoConnect.getInstance().StartPlayback(msg, startTime, channel, index);
        mGLVideoDisplay.ShowVideoLoading(index);
    }

    /**
     * 停止远程回放
     *
     * @param msg
     * @param channel
     * @param index
     */
    public void stopPlayback(String msg, int channel, int index) {
        GLVideoConnect.getInstance().StopPlayback(msg, channel, index);
    }

    /**
     * 开始云台命令的控制
     *
     * @param msg
     * @param action
     * @param channel
     * @param index
     */
    public void startPtz(String msg, int action, int channel, int index) {
        GLVideoConnect.getInstance().PtzCtrl(msg, action, channel, index);
    }

    /**
     * 停止云台控制
     *
     * @param msg
     * @param channel
     * @param index
     */
    public void stopPtz(String msg, int channel, int index) {
        GLVideoConnect.getInstance().StopCtrl(msg, channel, index);
    }

    /**
     * 远程回放那个的视频段列表搜索
     *
     * @param msg
     * @param startTime
     * @param endTime
     * @param channel
     * @param index
     */
    public void searchRec(String msg, int startTime, int endTime, int channel, int index) {
        GLVideoConnect.getInstance().SearchRec(msg, startTime, endTime, channel, index);
    }

    /**
     * 获取当前码流
     *
     * @param msg
     * @param index
     * @return
     */
    public int getBitrate(String msg, int index) {
        return GLVideoConnect.getInstance().GetBitrate(msg, index);
    }

    /**
     * 语音对讲呼叫
     *
     * @param msg
     * @param channel
     * @param index
     * @return
     */
    public int callAudio(String msg, int channel, int index) {
        return GLVideoConnect.getInstance().Call(msg, channel, index);
    }

    /**
     * 语音对讲挂断
     *
     * @param msg
     * @param index
     * @return
     */
    public int hangUp(String msg, int index) {
        return GLVideoConnect.getInstance().HangUp(msg, index);
    }

    /**
     * 语音对讲音频发送
     *
     * @param msg
     * @param buffer
     * @param size
     * @param tsMs
     * @param enc
     * @param samplerate
     * @param samplewidth
     * @param channel
     * @param compressRatio
     * @param index
     * @return
     */
    public int sendAudioPacket(String msg, byte[] buffer, int size, long tsMs, String enc, int samplerate, int samplewidth, int channel, float compressRatio, int index) {
        return GLVideoConnect.getInstance().SendAudioPacket(msg, buffer, size, tsMs, enc, samplerate, samplewidth, channel, compressRatio, index);
    }

    /**
     * 虚拟通道数据发送
     *
     * @param msg
     * @param buffer
     * @param size
     * @param index
     */
    public void sendData(String msg, byte[] buffer, int size, int index) {
        GLVideoConnect.getInstance().SendData(msg, buffer, size, index);
    }

    /**
     * 截图
     *
     * @param msg
     * @param filename
     * @param type
     * @param index
     * @return
     */
    public boolean captureImage(String msg, final String filename, int type, int index) {
        if (index == 0 && mGLVideoDisplay.mRender.mHardwareDecoder) {
            mGLVideoDisplay.queueEvent(new Runnable() {
                @Override
                public void run() {
                    mGLVideoDisplay.mRender.CaptureImage(filename);
                }
            });
            return true;
        }
        return GLVideoConnect.getInstance().CaptureImage(msg, filename, type, index);
    }

    public boolean captureImgByMonitor() {
        return false;
    }


    /**
     * 获取缩略图
     *
     * @param msg
     * @param filename
     * @param index
     * @return
     */
    public boolean CaptureThumbnailImage(String msg, String filename, int index) {
        return GLVideoConnect.getInstance().CaptureThumbnailImage(msg, filename, index);
    }

    /**
     * 开始录像
     *
     * @param msg
     * @param filename
     * @param index
     * @return
     */
    public boolean startRecord(String msg, String filename, int index) {
        return GLVideoConnect.getInstance().StartRecord(msg, filename, index);
    }

    public void startRecordScreen(final String filename) {
        mIsStartRecord = true;
        mGLVideoDisplay.queueEvent(new Runnable() {
            @Override
            public void run() {
                mGLVideoDisplay.mRender.startRecordScreen(filename);
            }
        });
    }

    private boolean mIsStartRecord = false;

    public boolean isStartRecord() {
        return mIsStartRecord;
    }

    public void setmIsStartRecord(boolean mIsStartRecord) {
        this.mIsStartRecord = mIsStartRecord;
    }

    /**
     * 停止录像
     *
     * @param msg
     * @param index
     */
    public void stopRecord(String msg, int index) {
        GLVideoConnect.getInstance().StopRecord(msg, index);
    }

    public void stopRecordScreen() {
        mIsStartRecord = false;
        mGLVideoDisplay.queueEvent(new Runnable() {
            @Override
            public void run() {
                mGLVideoDisplay.mRender.stopRecordScreen();
            }
        });
    }


    /**
     * 获取网络速度
     *
     * @param msg
     * @param index
     * @return
     */
    public int getNetWorkSpeed(String msg, int index) {
        return GLVideoConnect.getInstance().GetNetWorkSpeed(msg, index);
    }

    /**
     * 获取所有通道的网络速度
     *
     * @param msg
     * @return
     */
    public int getAllNetWorkSpeed(String msg) {
        return GLVideoConnect.getInstance().GetAllNetWorkSpeed(msg);
    }

    /**
     * 获取当前的通道
     *
     * @param msg
     * @param index
     * @return
     */
    public int getChannel(String msg, int index) {
        return GLVideoConnect.getInstance().GetChannel(msg, index);
    }

    /**
     * 获取录像状态
     *
     * @param msg
     * @param index
     * @return
     */
    public boolean getRecordState(String msg, int index) {
        return GLVideoConnect.getInstance().GetRecordState(msg, index);
    }

    /**
     * 局域网搜索设备
     *
     * @return
     */
    public int searchDevice() {
        return GLVideoConnect.getInstance().SearchDevice();
    }

    /**
     * 获取设备的报警推送消息
     *
     * @param msg
     * @param type
     * @param channel
     * @param ticket
     * @param index
     */
    public void pullAlarmmsg(String msg, int type, int channel, int ticket, int index) {
        GLVideoConnect.getInstance().PullAlarmmsg(msg, type, channel, ticket, index);
    }

    /**
     * 使用directtexture
     *
     * @param msg
     * @param isDirect
     */
    public void useDirectTexture(String msg, boolean isDirect) {
        GLVideoConnect.getInstance().UseDirectTexture(msg, isDirect);
    }

    /**
     * 播放音频
     */
    public void playAudio() {
        mGLVideoDisplay.PlayAudio();
    }

    /**
     * 关闭音频
     */
    public void stopAudio() {
        mGLVideoDisplay.StopAudio();
    }

    /**
     * 打开网格
     *
     * @param value
     */
    public void enableGrid(boolean value) {
        mGLVideoDisplay.EnableGrid(value);
    }

    /**
     * 销毁视频渲染控件的底层资源
     */
    public void destoryVideo() {
        mGLVideoDisplay.DestroyManager();
    }

    /**
     * 销毁网络的底层资源
     */
    public void destoryConnect() {
        GLVideoConnect.destroyInstance();
        ConnectManager.getInstance().removeAll();
    }

    /**
     * 播放那一路的音频
     *
     * @param msg
     * @param index
     */
    public void playAudioIndex(String msg, int index) {
        GLVideoConnect.getInstance().PlayAudioIndex(msg, index);
    }

    /**
     * 是否打开音频的功能
     *
     * @param value
     */
    public void enableAudio(boolean value) {
        mGLVideoDisplay.EnableAudio(value);
    }

    /**
     * 强制壁挂或者倒挂
     *
     * @param isWallMode
     */
    public void isForceWallMode(boolean isWallMode) {
        GLVideoConnect.getInstance().IsForceWallMode(isWallMode);
    }

    /**
     * 新添加的获取多少个码流的接口
     *
     * @param index
     * @return
     */
    public int getHowBitrate(int index) {
        return GLVideoConnect.getInstance().GetHowBitrate(mGLVideoDisplay.msg, index);
    }

    /**
     * 获取是否需要打开第三码流
     *
     * @param index
     * @return
     */
    public boolean isNeedOpen64Stream(int index) {
        return GLVideoConnect.getInstance().isNeedOpen64Stream(mGLVideoDisplay.msg, index);
    }

    public void SetTimeZone(String msg, float pTimeZone, int index) {
        GLVideoConnect.getInstance().Set_TimeZone(msg, pTimeZone, index);
    }

    /**
     * 获取是否有自动识别倒挂壁挂
     *
     * @param index
     * @return
     */
    public int getIsInstallModeCome(int index) {
        return GLVideoConnect.getInstance().getIsInstallModeCome(mGLVideoDisplay.msg, index);
    }

    public int getWallModelType(int index) {
        return GLVideoConnect.getInstance().GetWallModelType(mGLVideoDisplay.msg, index);
    }

    public int getFps(String msg, int index) {
        return GLVideoConnect.getInstance().GetFps(msg, index);
    }

    public void setOnCaptureImageListener(OnCaptureImageListener mOnCaptureImageListener) {
        GLVideoConnect.getInstance().mOnCaptureImageListener = mOnCaptureImageListener;
    }

    public void setOnVconDataListener(OnVconDataListener mOnVconDataListener) {
        GLVideoConnect.getInstance().mOnVconDataListener = mOnVconDataListener;
    }

    public void setOnSearchRecDataListener(OnSearchRecDataListener mOnSearchRecDataListener) {
        GLVideoConnect.getInstance().mOnSearchRecDataListener = mOnSearchRecDataListener;
    }

    public void setOnPullAlarmmsgListener(OnPullAlarmmsgListener mOnPullAlarmmsgListener) {
        GLVideoConnect.getInstance().mOnPullAlarmmsgListener = mOnPullAlarmmsgListener;
    }

    public void setOnSearchDeviceListener(OnSearchDeviceListener mOnSearchDeviceListener) {
        GLVideoConnect.getInstance().mOnSearchDeviceListener = mOnSearchDeviceListener;
    }

    public void setOnPlaybackUpdateTimeListener(OnPlaybackUpdateTimeListener mOnPlaybackUpdateTimeListener) {
        GLVideoConnect.getInstance().mOnPlaybackUpdateTimeListener = mOnPlaybackUpdateTimeListener;
    }

    public void setOnOSDTxtTimeListener(OnOSDTxtTimeListener mOnOSDTxtTimeListener) {
        GLVideoConnect.getInstance().mOnOSDTxtTimeListener = mOnOSDTxtTimeListener;
    }
}
