package com.app.jagles.view;

import android.content.Context;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.util.Log;

import com.app.jagles.connect.JAConnector;
import com.app.jagles.connect.listener.osd.FrameResultAbs;
import com.app.jagles.connect.listener.playback.FramePlaybackAbs;
import com.app.jagles.connect.listener.searchBackTime.SearchRecordTimeAbs;
import com.app.jagles.helper.voice.VoiceHelper;
import com.app.jagles.listener.CaptureImageListener;
import com.app.jagles.listener.GestureListener;
import com.app.jagles.networkCallback.OnCaptureImageListener;
import com.app.jagles.util.LanguageUtil;
import com.app.jagles.video.GLVideoDisplay;
import com.app.jagles.video.GLVideoRender;
import com.app.jagles.video.Parameter;

/**
 * Created by zasko on 2018/1/9.
 */

public class JAGLDisplay extends GLVideoDisplay {

    private static final String TAG = "JAGLDisplay";


    private String mConnectKey = "";

    /*视频帧接口*/
    private FrameResultAbs mFrameResultAbs = null;

    /*回放视频帧*/
    private FramePlaybackAbs mFramePlaybackAbs = null;

    private SearchRecordTimeAbs mSearchRecordTimeAbs = null;

    /**
     * 摧毁标志
     */
    private boolean isDestroy = false;


    public JAGLDisplay(Context context) {
        super(context);
    }

    public JAGLDisplay(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    /**
     *
     */
    private void initListener() {
        JAConnector.getConnect().mOnGSensorDataListener = mRender;
        JAConnector.getConnect().mOnTextureAvaibleListener = mRender;
        JAConnector.getConnect().mOnDirectTextureFrameUpdataListener = mRender;
        JAConnector.getConnect().mOnOSDTextureAvaibleListener = mRender;
        JAConnector.getConnect().mOnAudioDataComeListener = mRender;
        JAConnector.getConnect().mOnPlayedFirstFrameListener = mRender;


    }

    public GLVideoRender getRender() {
        return mRender;
    }


    /**
     * 连接设备
     *
     * @param key
     * @param deviceId
     * @param password
     * @param index
     * @param channel
     */
    public void connect(String key, String deviceId, String password, int index, int channel, boolean isLoading) {
        if (!TextUtils.isEmpty(key)) {
            setConnectKey(key);
            JAConnector.connectDevice(key, deviceId, password, index, channel);

            /*显示加载loading*/
            if (isLoading) {
                ShowVideoLoading(index);
            }
        }
    }


    /**
     * 断开连接
     *
     * @param index
     */
    public void disconnect(int index) {

        if (TextUtils.isEmpty(mConnectKey)) {
            return;
        }
        JAConnector.disconnectDevice(mConnectKey, index);

    }

    public void showLoading(int index) {

        ShowVideoLoading(index);
    }

    public void hideLoading(int index) {
        HideVideoLoading(index);
    }


    /**
     * @param bitrate
     * @param index
     * @param channel
     */
    public void openVideo(int bitrate, int channel, int index, boolean isLoading) {
        if (isLoading) {
            showLoading(index);
        }
        JAConnector.openDevice(mConnectKey, bitrate, channel, index);
    }

    /**
     * @param bitrate
     * @param index
     * @param channel
     */
    public void closeVideo(int bitrate, int channel, int index) {
        JAConnector.closeDevice(mConnectKey, bitrate, channel, index);
    }

    public void startPlayback(int startTime, int channel, int index) {
        JAConnector.startDevicePlayback(mConnectKey, startTime, channel, index);
    }

    public void stopPlayback(int channel, int index) {
        JAConnector.stopDevicePlayback(mConnectKey, channel, index);
    }

    public void resumePlayback(int channel, int index) {

        Log.d(TAG, "resumePlayback: ------>index:" + index + " channel:" + channel);
        JAConnector.resumeDevicePlayback(mConnectKey, channel, index);
    }

    public void pausePlayback(int channel, int index) {
        JAConnector.pauseDevicePlayback(mConnectKey, channel, index);
    }

    public void searchRecordTime(int startTime, int endTime, int channel, int index) {
        JAConnector.searchDeviceREC(mConnectKey, startTime, endTime, channel, index);
    }

    /**
     * 视频帧的回调
     *
     * @param abs
     */
    public void setFrameResultAbs(FrameResultAbs abs) {

        mFrameResultAbs = abs;
        JAConnector.addFrameResultAbs(mFrameResultAbs);
    }


    /**
     * 移除视频帧回调
     */
    public void removeFrameResultAbs() {

        JAConnector.removeFrameResultAbs(mFrameResultAbs);
        mFrameResultAbs = null;
    }

    /**
     * 设置回放视频帧回调
     *
     * @param abs
     */
    public void setFramePlaybackAbs(FramePlaybackAbs abs) {
        mFramePlaybackAbs = abs;
        JAConnector.addFramePlaybackAbs(abs);
    }

    /**
     * 移除回放视频帧回调接口
     */
    public void removeFramePlaybackAbs() {
        JAConnector.removeFramePlaybackAbs(mFramePlaybackAbs);
        mFramePlaybackAbs = null;
    }

    /**
     * 设置搜索录像回调
     *
     * @param abs
     */
    public void setSearchRecordTimeAbs(SearchRecordTimeAbs abs) {
        mSearchRecordTimeAbs = abs;
        JAConnector.addDeviceSearchRecordAbs(mSearchRecordTimeAbs);
    }

    public void removeSearchRecordTimeAbs() {
        JAConnector.removeDeviceSearchRecordAbs(mSearchRecordTimeAbs);
        mSearchRecordTimeAbs = null;
    }


    /**
     * 播放器的创建 回调
     * <>
     * <p>
     * <p>
     * </>
     * <p>
     * <p>
     * 视 频 相 关 接 口 跟 连 接 库 对 接
     *
     * @param listener
     */
    public void setGLDisplayCreateListener(OnGLDisplayCreateListener listener) {

        getRender().setGLDisplayCreateListener(listener);

        initListener();
    }


    /**
     * @param key
     */
    public void setConnectKey(String key) {
        mConnectKey = key;
        msg = key;
    }

    /**
     * 播放哪路音频
     *
     * @param index
     */
    public void setAudioIndex(int index) {
        JAConnector.setDeviceAudioIndex(mConnectKey, index);
    }


    /**
     * 对讲 帮助类
     */
    private VoiceHelper mVoiceHelper;

    /**
     * 是否是在 对讲ing
     */
    public boolean isRecord;

    /**
     * 开始对讲
     */
    public void startTalking() {
        if (mVoiceHelper == null) {
            mVoiceHelper = VoiceHelper.create();
        }
        isRecord = true;
        if (!mVoiceHelper.isRecord) {
            mVoiceHelper.startTalking(mConnectKey);
        }
    }


    /**
     * 停止对讲
     */
    public void endTalking() {
        isRecord = false;
        if (mVoiceHelper != null) {
            mVoiceHelper.endTalking(mConnectKey);
        }
    }

    public void createVoiceHelper() {
        if (mVoiceHelper == null) {
            mVoiceHelper = VoiceHelper.create();
        }
    }

    public void setTalkChannel(int channel, int index) {
        if (mVoiceHelper != null) {
            mVoiceHelper.setCurrentChannel(channel);
            mVoiceHelper.setCurrentIndex(index);
        }

    }

    /**
     * 是否开启音频
     */
    public boolean isOpenAudio = false;

    /**
     * 开启音频
     */
    public void openAudio() {
        isOpenAudio = true;
        PlayAudio();
    }

    /**
     * 停止音频
     */
    public void stopAudio() {
        isOpenAudio = false;
        StopAudio();
    }

    /**
     * 获取当前分屏模式
     *
     * @return
     */
    public int getCurrentSplitMode() {
        return getRender().GetSplitMode(getRender().mParametricManager);
    }

    /**
     * 获取当前选择下标
     *
     * @return
     */
    public int getVideoCurrentIndex() {
        return getVideoIndex();
    }

    public void setChannelConnectTip(int state, int contentCode, int index) {
        getRender().showChannelConnectTip(state, contentCode, index);
    }


    /**
     * 对应通道截图
     *
     * @param fileName
     * @param type
     * @param index
     */
    public void captureImageByIndex(String fileName, int type, int index) {
        JAConnector.captureImageByIndex(mConnectKey, fileName, type, index);
    }


    public void captureThumbByIndex(String fileName, int index) {
        JAConnector.captureThumbByIndex(mConnectKey, fileName, index);
    }

    /**
     * 截图 截屏 成功或者失败的回调
     * network
     *
     * @param listener
     */
    public void setCaptureResultListener(OnCaptureImageListener listener) {
        JAConnector.setCaptureResultListener(listener);

    }

    public void setCaptureByMonitorResultListener(CaptureImageListener listener) {
        getRender().setCaptureImageListener(listener);
    }


    /**
     * 屏幕截图
     *
     * @param filePath
     */
    public void screenshot(String filePath) {
        getRender().setCaptureImgByMonitor(true, filePath);
    }

    public void screenshotByHWDecoder(String fileName) {
        getRender().CaptureImage(fileName);
    }


    /*录像是否在进行中*/
    public boolean isScreenRecording;

    /**
     * 开始屏幕录制
     *
     * @param filePath
     */
    public void startScreenRecord(final String filePath) {
        queueEvent(new Runnable() {
            @Override
            public void run() {
                isScreenRecording = true;
                getRender().startRecordScreen(filePath);

            }
        });
    }

    /**
     * 停止屏幕录制
     */
    public void stopScreenRecord() {
        queueEvent(new Runnable() {
            @Override
            public void run() {
                isScreenRecording = false;
                getRender().stopRecordScreen();
            }
        });
    }

    /**
     * 通道屏幕录制
     *
     * @param filePath
     * @param channel
     */
    public void startScreenChannelRecord(String filePath, int channel) {
        JAConnector.startDeviceRecord(mConnectKey, filePath, channel);
    }

    /**
     * 通道屏幕录制关闭
     *
     * @param channel
     */
    public void stopScreenChannelRecord(int channel) {
        JAConnector.stopDeviceRecord(mConnectKey, channel);
    }

    /**
     * 手势识别
     *
     * @param listener
     */
    public void setGestureListener(GestureListener listener) {
        SetGestureListener(listener);

    }

    /**
     * 播放本地文件
     *
     * @param fileName
     */
    public void playFile(String fileName) {
        if (!TextUtils.isEmpty(fileName)) {
            Playfile(fileName);
        }

    }

    /**
     * 获取当前单屏播放的索引
     *
     * @return 如果不是单屏，则返回-1
     */
    public int getOnePlayIndex() {
        if (isOnePlay()) {
            return getRender().GetVideoIndex(getRender().mParametricManager);
        }
        return -1;
    }

    /**
     * 设置View的分割
     *
     * @param mode
     */
    public void setSplit(int mode) {
        SetSplit(mode);
    }

    /**
     * 是否显示出来
     *
     * @param index
     * @return
     */
    public boolean getVideoVISIBLE(int index) {
        return GetVisibility(index);
    }

    /**
     * 通过通道数来设置页面
     *
     * @param allChannel
     */
    public void setPageCount(int allChannel) {
        SetAllPage(allChannel);
    }

    /**
     * 获取当前的page的index
     *
     * @return
     */
    public int getCurrentPageIndex() {
        return GetPage();
    }

    /**
     * 获取当前分割模式下的页面数
     *
     * @return
     */
    public int getPageCount() {
        return GetAllPage();
    }

    /**
     * 设置所有的界面
     *
     * @param page
     */
    public void setAllPage(int page) {
        SetAllPage(page);
    }


    public void setSwitchPanoramaMode(int mode) {
        SwitchPanoramaMode(mode);
    }

    public void updateAspect(int width, int height) {
        UpdateAspect(width, height);
    }


    public void setBorderColor(int color) {
        getRender().SetBorderColor(color);
    }

    /*获取当前所有网络的下行*/
    public int getAllNetworkSpeed() {
        return JAConnector.getConnect().GetAllNetWorkSpeed(mConnectKey);
    }

    public void enableHarWareDecoder(boolean enable, int index) {
        getRender().enableHarWareDecoder(enable, index);

    }

    /**
     * 释放对象
     */
    public void directDestroy() {
        isDestroy = true;

        /*防止出现开始对话 线程没有关闭*/
        endTalking();
        stopAudio();


        /*移除视频帧回放接口*/
        if (mFrameResultAbs != null) {
            removeFrameResultAbs();
        }

        if (mFramePlaybackAbs != null) {
            removeFramePlaybackAbs();
        }

        if (mSearchRecordTimeAbs != null) {
            removeSearchRecordTimeAbs();
        }
        destorySensor();

        /*释放解码库*/
        mRender.DirectDestroy();
    }


}
