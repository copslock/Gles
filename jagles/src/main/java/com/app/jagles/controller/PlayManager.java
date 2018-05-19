package com.app.jagles.controller;

import android.util.Log;

import com.app.jagles.listener.ConnectStatusListener;
import com.app.jagles.listener.GLVideoSurfaceCreateListener;
import com.app.jagles.video.GLVideoConnect;
import com.app.jagles.video.GLVideoDisplay;

/**
 * Created by JuAn_Zhangsongzhou on 2017/6/21.
 * <p>
 * be use for control all operate by Video player  {@link com.app.jagles.video.GLVideoDisplay}
 */

public class PlayManager {
    private static final String TAG = "PlayManager";

    private PlayManager() {

    }

    private String msg;
    private GLVideoDisplay mGLVideoDisplay;

    private static PlayManager mPlayManager;

    /**
     * @param player
     * @return
     */
    public static PlayManager getInstance(GLVideoDisplay player) {

        if (mPlayManager == null) {
            mPlayManager = new PlayManager();
        }
        mPlayManager.mGLVideoDisplay = player;
        mPlayManager.init("");

        return mPlayManager;
    }

    /**
     * @param player
     * @return
     */
    public static PlayManager getInstance(GLVideoDisplay player, String bundleId) {

        if (mPlayManager == null) {
            mPlayManager = new PlayManager();
        }
        mPlayManager.init(bundleId);
        mPlayManager.mGLVideoDisplay = player;
        return mPlayManager;
    }

    /**
     * 初始化接口回调
     */
    private void init(String bundleId) {
//        GLVideoConnect.getInstance(bundleId).mOnStatusListener = mGLVideoDisplay.mRender;
//        GLVideoConnect.getInstance().mOnTextureAvaibleListener = mGLVideoDisplay.mRender;
//        GLVideoConnect.getInstance().mOnDirectTextureFrameUpdataListener = mGLVideoDisplay.mRender;
//        GLVideoConnect.getInstance().mOnOSDTextureAvaibleListener = mGLVideoDisplay.mRender;
//        GLVideoConnect.getInstance().mOnAudioDataComeListener = mGLVideoDisplay.mRender;
//        GLVideoConnect.getInstance().mOnPlayedFirstFrameListener = mGLVideoDisplay.mRender;
    }
/**
 *------------------------------------------------- listener -----------------------------------------
 * -----------------------------------start-----------------------
 */
    /**
     * 播放器创建的回调
     *
     * @param listener
     */
    public void setGLVideoSurfaceCreateListener(GLVideoSurfaceCreateListener listener) {
        mGLVideoDisplay.mRender.mGLVideoSurfaceCreateListener = listener;
    }

    /**
     * 播放器连接网络的回调
     *
     * @param listener
     */
    public void setConnectStatusListener(ConnectStatusListener listener) {
        mGLVideoDisplay.mRender.mConnectStatusListener = listener;
    }
/**
 * ------------------------------------------------- listener -----------------------------------------
 * -----------------------------------end-----------------------
 */

    /**
     * 底层生成一个带身份msg的网络连接实例
     * create an identity message by connect
     *
     * @param msg
     * @return
     */
    public String getIdentityByConnect(String msg) {
        this.msg = msg;
        return GLVideoConnect.getInstance().GetConnect(msg);
    }

    /**
     * connect device
     * <p>
     * it will callback  {@link com.app.jagles.listener.ConnectStatusListener}
     *
     * @param msg    device  identity
     * @param idOrIp device id
     * @param verify device password
     * @param index  0~35     connect to the specified camera in Device (show in disPlayer)
     */
    public void connectDevice(String msg, String idOrIp, String verify, int index) {
        GLVideoConnect.getInstance().Connect(msg, idOrIp, verify, index);
        if (mGLVideoDisplay != null) {
            if (index == 0)
                GLVideoConnect.getInstance().SetHardwareDecoder(msg, mGLVideoDisplay.mRender.mHardwareDecoder, index, mGLVideoDisplay.mRender.mMaxSupportWidth, mGLVideoDisplay.mRender.mMaxSupportHeight);
            mGLVideoDisplay.ShowVideoLoading(index);
            mGLVideoDisplay.msg = msg;
        }
    }

    /**
     * connect device
     * <p>
     * it will callback  {@link com.app.jagles.listener.ConnectStatusListener}
     *
     * @param msg
     * @param idOrIp
     * @param verify
     * @param index   0~35     connect to the specified camera in Device (show in disPlayer)
     * @param channel 0~35     connect to the specified camera in Device (connect in device)
     */
    public void connectDevice(String msg, String idOrIp, String verify, int index, int channel) {
        GLVideoConnect.getInstance().Connect(msg, idOrIp, verify, index, channel);
        if (mGLVideoDisplay != null) {
            mGLVideoDisplay.ShowVideoLoading(index);
            mGLVideoDisplay.msg = msg;
        }
    }

    /**
     * disconnect device
     * <p> it will callback {@link com.app.jagles.listener.ConnectStatusListener}
     * <p>
     * </p>
     *
     * @param msg
     * @param index 0~35
     */
    public void disConnectDevice(String msg, int index) {
        GLVideoConnect.getInstance().DisConnect(msg, index);
    }

    /**
     * 打开视频
     * player open video when connect successful
     *
     * @param msg
     * @param bitrate HD & SD  0 & 1  {@link JAParameter}
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
     * 关闭播放器
     * close video
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
     * 设置播放器的方法模式
     * set player play mode
     *
     * @param mode {@link JAParameter}
     */
    public void setPanoramaMode(int mode) {
        mGLVideoDisplay.SwitchPanoramaMode(mode);
    }

    /**
     * 设置所有的页面数
     * set all the number of pages
     *
     * @param num
     */
    public void setAllPage(int num) {
        mGLVideoDisplay.SetAllPage(num);
    }

    /**
     * 设置底层渲染大小
     * <p>
     * set the underlying rendering size
     *
     * @param width
     * @param height
     */
    public void setUpdateAspect(int width, int height) {
        mGLVideoDisplay.mRender.UpdateAspect(mGLVideoDisplay.mRender.mParametricManager, (float) ((float) width / (float) height));
    }

    /**
     * 获取设备文件列表
     * get device file list
     *
     * @param msg
     * @param index
     * @return
     */
    public String getDeviceFileList(String msg, int index) {
        return GLVideoConnect.getInstance().getFileDownloadList(msg, index);
    }

    /**
     * 下载设备文件
     * download device file
     *
     * @param msg
     * @param opType
     * @param opFlag
     * @param file_name
     * @param suffix
     * @param index
     */
    public void downloadDeviceFile(String msg, int opType, int opFlag, String file_name, String suffix, int index) {
        GLVideoConnect.getInstance().doDownloadFile(msg, opType, opFlag, file_name, suffix, index);
    }

    /**
     * 播放设备录像视频
     *  play device record video
     *
     * @param msg
     * @param startTime
     * @param channel
     * @param index
     */
    public void startPlayRecordVideo(String msg, int startTime, int channel, int index) {

    }


}
