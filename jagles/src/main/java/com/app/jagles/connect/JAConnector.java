package com.app.jagles.connect;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.compat.BuildConfig;
import android.support.v4.content.LocalBroadcastManager;
import android.text.TextUtils;
import android.util.Log;

import com.app.jagles.connect.listener.osd.FrameResultAbs;
import com.app.jagles.connect.listener.osd.FrameResultSubjectImpl;
import com.app.jagles.connect.listener.playback.FramePlaybackAbs;
import com.app.jagles.connect.listener.playback.FramePlaybackSubjectImpl;
import com.app.jagles.connect.listener.searchBackTime.SearchRecordTimeAbs;
import com.app.jagles.connect.listener.searchBackTime.SearchRecordTimeSubjectImpl;
import com.app.jagles.listener.OnFrameOSDListener;
import com.app.jagles.networkCallback.OnCaptureImageListener;
import com.app.jagles.networkCallback.OnCheckTUTKDevOnlineListener;
import com.app.jagles.networkCallback.OnPlaybackUpdateTimeListener;
import com.app.jagles.networkCallback.OnSearchRecDataListener;
import com.app.jagles.networkCallback.OnTextureAvaibleListener;
import com.app.jagles.networkCallback.OnVconDataListener;
import com.app.jagles.video.ConnectManager;
import com.app.jagles.video.GLVideoConnect;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * Created by zasko on 2018/1/6.
 */

public class JAConnector extends GLVideoConnect {

    private static final String TAG = "JAConnect";

    private static JAConnector mJAConnector;

    private GLVideoConnect mGLVideoConnect;

    private Context mContext;

    private JAConnector(Context context) {
        super();
        Log.d(TAG, "JAConnect: ------>创建！！！！！");
        if (mGLVideoConnect == null) {
            mGLVideoConnect = GLVideoConnect.getInstance();
            GLVideoConnect.bundleid = "";
            mContext = context;
        }
    }

    private JAConnector(Context context, String id) {
        super();
        if (mGLVideoConnect == null) {
            mGLVideoConnect = GLVideoConnect.getInstance(id);
            mContext = context;
        }
    }


    /**
     * 初始化 链接层
     *
     * @return
     */
    public static JAConnector initialize(Context context) {
        if (mJAConnector == null) {
            mJAConnector = new JAConnector(context);
            mJAConnector.init();
        }
        return mJAConnector;
    }

    /**
     * 初始化 链接层
     *
     * @param build 带标识
     * @return
     */
    public static JAConnector initialize(Context context, String build) {
        if (mJAConnector == null) {
            mJAConnector = new JAConnector(context, build);
            mJAConnector.init();
        }
        return mJAConnector;

    }

    /**
     * 初始化链路层
     *
     * @param build        ""  带标识
     * @param jniCrashPath jni 抓取路径
     * @param version      app版本
     * @return
     */
    public static JAConnector initialize(Context context, String build, String jniCrashPath, String version) {
        if (mJAConnector == null) {
            mJAConnector = new JAConnector(context, build);
            GLVideoConnect.initJNICrash(jniCrashPath, version);
            mJAConnector.init();

        }
        return mJAConnector;
    }


    private void init() {

        initListener();
        initImpl();
    }

    /*视频帧接口控制*/
    private static FrameResultSubjectImpl mFrameResultSubject;
    /*视频回放接口回调*/
    private static FramePlaybackSubjectImpl mFramePlaybackSubject;

    private static SearchRecordTimeSubjectImpl mSearchRecordTimeSubject;

    private void initImpl() {
        mFrameResultSubject = new FrameResultSubjectImpl();
        mFramePlaybackSubject = new FramePlaybackSubjectImpl();
        mSearchRecordTimeSubject = new SearchRecordTimeSubjectImpl();
    }

    /**
     * 初始化调用接口
     * <p>
     * {@link OnDeviceVideoTimeImpl}
     */
    private void initListener() {
        mGLVideoConnect.setOnFrameOSDListener(new OnDeviceVideoTimeImpl());
        mGLVideoConnect.mOnPlaybackUpdateTimeListener = new OnDeviceVideoPlaybackTimeImpl();
        mGLVideoConnect.setOnConnectResultListener(new OnDeviceConnectResultImpl());
        mGLVideoConnect.mOnSearchRecDataListener = new OnSearchRecordTimeImpl();
        mGLVideoConnect.mOnVconDataListener = new OnDeviceRemoteDataImpl();
        mGLVideoConnect.mOnCheckTUTKDevOnlineListener = new OnTutkStatusImpl();


    }


    /**
     * @return
     */
    public GLVideoConnect getJAConnect() {
        return mGLVideoConnect;
    }

    public static GLVideoConnect getConnect() {
        return mJAConnector.getJAConnect();
    }


    /**
     * 连接操作：
     *
     * 设备连接 ，设备断开连接
     *
     *
     * {@link #connectDevice(String, String, String, int, int)}
     * {@link #connectDevice(String, String, String, int, int, boolean)}
     *
     * {@link #disconnectDevice(String, int)}
     *
     *
     *
     */

    /**
     * 连接设备
     *
     * @param key
     * @param deviceId
     * @param password
     * @param index
     * @param channel
     * @return 操作状态
     */
    public static boolean connectDevice(String key, String deviceId, String password, int index, int channel) {

        if (TextUtils.isEmpty(key) || TextUtils.isEmpty(deviceId) || TextUtils.isEmpty(password)) {
            return false;
        }
        String tmpKey = getConnect().GetConnect(key);
        getConnect().Connect(tmpKey, deviceId, password, index, channel);

        return true;
    }

    /**
     * 连接设备
     *
     * @param key
     * @param deviceId
     * @param password
     * @param index
     * @param channel
     * @param isDisconnect 是否断开上次连接状态
     * @return
     */
    public static boolean connectDevice(String key, String deviceId, String password, int index, int channel, boolean isDisconnect) {
        if (isDisconnect) {
            disconnectDevice(key, index);
        }
        return connectDevice(key, deviceId, password, index, channel);
    }

    /**
     * 断开连接
     *
     * @param key
     * @param index
     * @return
     */
    public static boolean disconnectDevice(String key, int index) {
        if (TextUtils.isEmpty(key)) {
            return false;
        }

        getConnect().DisConnect(key, index);
        return true;
    }

    /**
     * 摧毁特定连接
     *
     * @param key
     * @return
     */
    public static int destroyConnector(String key) {
        if (TextUtils.isEmpty(key)) {
            return -1;
        }
        return getConnect().DestroyConnector(key);
    }

    /**
     * @param index
     * @deprecated
     */
    public static void disconnectAll(int index) {
        HashMap<String, Long> tmp = ConnectManager.getInstance().getConnectList();
        for (String key : tmp.keySet()) {
            Log.d(TAG, "disconnectAll: ----->" + key + " value：" + tmp.get(key));
            JAConnector.disconnectDevice(key, index);
        }


    }

    /**
     * 获取当前播放的状态
     * #define NET_PLAY_NONE            0
     * #define NET_PLAY_REAL            1
     * #define NET_PLAY_RECORD          2
     */
    public static int getRealStatus(String key) {
        return getConnect().GetRealStatus(key);
    }

    /**
     * 获取所有连接通道的句柄
     *
     * @param key
     * @return
     */
    public static long[] getAllConnectCtx(String key) {
        return getConnect().GetAllConnectCtx(key);
    }

    /**
     * 重置底层socket连接
     */
    public static void resetTransfer() {
        getConnect().ResetTransfer();
    }

    /**
     * 检查tutk状态
     *
     * @param key
     * @param tutkId
     * @param timeout
     */
    public static void checkTutkOnline(String key, String tutkId, int timeout) {
        getConnect().DoCheckTutkOnline(key, tutkId, timeout);
    }

    /**
     * 远程数据处理：
     *
     *  更改设备信息
     *
     *
     *
     *
     *
     *
     *　　　 　　　　┏┓　　　┏┓+ +
     *　　　　　　　┏┛┻━━━┛┻┓ + +
     *　　　　　　　┃　　　　　　　┃ 　
     *　　　　　　　┃　　　━　　　┃ ++ + + +
     *　　　　　　 ████━████ ┃+
     *　　　　　　　┃　　　　　　　┃ +
     *　　　　　　　┃　　　┻　　　┃
     *　　　　　　　┃　　　　　　　┃ + +
     *　　　　　　　┗━┓　　　┏━┛
     *　　　　　　　　　┃　　　┃　　　　　　　　　　　
     *　　　　　　　　　┃　　　┃ + + + +
     *　　　　　　　　　┃　　　┃　　　　Code is far away from bug with the animal protecting　　　　　　　
     *　　　　　　　　　┃　　　┃ + 　　　　神兽保佑,代码无bug　　
     *　　　　　　　　　┃　　　┃
     *　　　　　　　　　┃　　　┃　　+　　　　　　　　　
     *　　　　　　　　　┃　 　　┗━━━┓ + +
     *　　　　　　　　　┃ 　　　　　　　┣┓
     *　　　　　　　　　┃ 　　　　　　　┏┛
     *　　　　　　　　　┗┓┓┏━┳┓┏┛ + + + +
     *　　　　　　　　　　┃┫┫　┃┫┫
     *　　　　　　　　　　┗┻┛　┗┻┛+ + + +
     */

    /**
     * 发送通道数据
     * <p>
     * {@link #sendDeviceData(String, int, int, String, String, String)}}
     *
     * @param type
     * @param userName
     * @param password
     * @hide
     */
    public static void sendDeviceData(String key, int index, int type, String userName, String password) {
        sendDeviceData(key, index, type, userName, password, "");
    }

    /**
     * {@link #sendDeviceData(String, int, int, String, String, String)}}
     *
     * @param type
     * @param verify
     * @hide
     */
    public static void sendDeviceData(String key, int index, int type, String verify) {
        sendDeviceData(key, index, type, "", "", verify);
    }

    /**
     * @param type
     * @param userName
     * @param password
     * @param verify
     * @hide
     */
    public static void sendDeviceData(String key, int index, int type, String userName, String password, String verify) {


        //TODO: 工具类的选择
        String tmp = "";
        getConnect().SendData(key, tmp.getBytes(), tmp.getBytes().length, index);
    }

    /**
     * @param key
     * @param index
     * @param json
     * @hide
     */
    public static void sendDeviceData(String key, int index, String json) {
        String tmp = json;
        if (BuildConfig.DEBUG) {
            Log.d(TAG, "sendDeviceData: " + tmp);
        }
        getConnect().SendData(key, tmp.getBytes(), tmp.getBytes().length, index);

    }


    /**
     * 更改设备信息
     * 如果更改成功，广播就会返回
     * {
     * "option" :"success"
     * }
     *
     * @param key      返回标识
     * @param index    设备下标
     * @param type     设备数据类型
     * @param userName 设备id
     * @param password 设备密码
     * @param verify   如果填写，就不需要设置userName和password
     */
    public static void modifyDeviceSetting(String key, int index, int type, String userName, String password, String verify) {
        sendDeviceData(key, index, type, userName, password, verify);
    }

    /**
     * 更改设备信息
     * <p>
     * 如果更改成功，广播就会返回
     * {
     * "option" :"success"
     * }
     *
     * @param key
     * @param index
     * @param type
     * @param userName
     * @param password
     */
    public static void modifyDeviceSetting(String key, int index, int type, String userName, String password) {
        sendDeviceData(key, index, type, userName, password);
    }

    /**
     * 更改设备信息
     * <p>
     * 如果更改成功，广播就会返回
     * {
     * "option" :"success"
     * }
     *
     * @param key
     * @param index
     * @param type
     * @param verify
     */
    public static void modifyDeviceSetting(String key, int index, int type, String verify) {
        sendDeviceData(key, index, type, verify);
    }

    /**
     * @param key
     * @param index
     * @param json
     */
    public static void modifyDeviceSetting(String key, int index, String json) {
        sendDeviceData(key, index, json);
    }


    /**
     * 设备视频操作：
     * <p>
     * 硬解和软解
     * 打开码流 || 关闭码流
     * 回放功能：
     * <p>
     * <p>
     * *　　　　　　　　┏┓　　　┏┓+ +
     * 　　　　　　　┏┛┻━━━┛┻┓ + +
     * 　　　　　　　┃　　　　　　　┃
     * 　　　　　　　┃　　　━　　　┃ ++ + + +
     * 　　　　　　 ████━████ ┃+
     * 　　　　　　　┃　　　　　　　┃ +
     * 　　　　　　　┃　　　┻　　　┃
     * 　　　　　　　┃　　　　　　　┃ + +
     * 　　　　　　　┗━┓　　　┏━┛
     * 　　　　　　　　　┃　　　┃
     * 　　　　　　　　　┃　　　┃ + + + +
     * 　　　　　　　　　┃　　　┃　　　　Code is far away from bug with the animal protecting
     * 　　　　　　　　　┃　　　┃ + 　　　　神兽保佑,代码无bug
     * 　　　　　　　　　┃　　　┃
     * 　　　　　　　　　┃　　　┃　　+
     * 　　　　　　　　　┃　 　　┗━━━┓ + +
     * 　　　　　　　　　┃ 　　　　　　　┣┓
     * 　　　　　　　　　┃ 　　　　　　　┏┛
     * 　　　　　　　　　┗┓┓┏━┳┓┏┛ + + + +
     * 　　　　　　　　　　┃┫┫　┃┫┫
     * 　　　　　　　　　　┗┻┛　┗┻┛+ + + +
     */

    /**
     * 设置时区
     *
     * @param key
     * @param timezone 时区 -1200~1200
     * @param index    设备下标
     */
    public static void setTimezone(String key, int timezone, int index) {
        getConnect().Set_TimeZone(key, timezone, index);
    }

    /**
     * @param key
     * @param isHard 是否是硬解
     * @param index  设备下标
     * @param width  最大宽
     * @param height 最大高
     */
    public static void setDeviceHardWareDecoder(String key, boolean isHard, int index, int width, int height) {
        getConnect().SetHardwareDecoder(key, isHard, index, width, height);
    }

    /**
     * 打开码流
     *
     * @param key
     * @param bitrate 码流{1:标清，0：高清}  {@link com.app.jagles.controller.JAParameter}
     * @param channel
     * @param index
     */
    public static void openDevice(String key, int bitrate, int channel, int index) {
        Log.d(TAG, "openDevice: ------->key:" + key + " bitrate:" + bitrate + " channel:" + channel + " index:" + index);
        getConnect().OpenChannel(key, bitrate, channel, index);
    }

    /**
     * 关闭码流
     *
     * @param key
     * @param bitrate 码流{1:标清，0：高清} {@link com.app.jagles.controller.JAParameter}
     * @param channel
     * @param index
     */
    public static void closeDevice(String key, int bitrate, int channel, int index) {
        getConnect().CloseChannel(key, bitrate, channel, index);
    }

    /**
     * 获取设备码流
     *
     * @param key
     * @param index
     * @return
     */
    public static int getDeviceBitrate(String key, int index) {
        return getConnect().GetBitrate(key, index);
    }


    /**
     * 设备搜索回放
     *
     * @param key
     * @param startTime
     * @param endTime
     * @param channel
     * @param index
     */
    public static void searchDeviceREC(String key, int startTime, int endTime, int channel, int index) {
        getConnect().SearchRec(key, startTime, endTime, channel, index);
    }

    /**
     * 开始设备回放
     *
     * @param key
     * @param startTime
     * @param channel
     * @param index
     */
    public static void startDevicePlayback(String key, int startTime, int channel, int index) {
        getConnect().StartPlayback(key, startTime, channel, index);
    }

    /**
     * 暂停设备回放
     *
     * @param key
     * @param channel
     * @param index
     */
    public static void pauseDevicePlayback(String key, int channel, int index) {
        getConnect().PausePlayback(key, channel, index);

    }

    /**
     * 唤醒设备回放
     *
     * @param key
     * @param channel
     * @param index
     */
    public static void resumeDevicePlayback(String key, int channel, int index) {
        getConnect().ResumePlayback(key, channel, index);

    }

    /**
     * 停止设备回放
     *
     * @param key
     * @param channel
     * @param index
     */
    public static void stopDevicePlayback(String key, int channel, int index) {
        getConnect().StopPlayback(key, channel, index);
    }

    /**
     * 获取设备网速
     *
     * @param key
     * @param index 返回设备网络状态：{ -1 返回所有的设备网络状态}
     * @return
     */
    public static int getDeviceNetworkSpeed(String key, int index) {
        if (index == -1) {
            return getConnect().GetAllNetWorkSpeed(key);
        }
        return getConnect().GetNetWorkSpeed(key, index);
    }


    public static void captureImageByIndex(String key, String fileName, int type, int index) {
        getConnect().CaptureImage(key, fileName, type, index);
    }

    public static void captureThumbByIndex(String key, String fileName, int index) {
        getConnect().CaptureThumbnailImage(key, fileName, index);
    }

    /**
     * 截图成功回调
     */
    public static void setCaptureResultListener(OnCaptureImageListener listener) {
        getConnect().mOnCaptureImageListener = listener;
    }


    /**
     * 获取设备通道
     *
     * @param key
     * @param index
     * @return
     */
    public static int getDeviceChannel(String key, int index) {
        return getConnect().GetChannel(key, index);
    }

    /**
     *
     *
     * 音频处理：
     *
     *
     * {@link #sendAudioPacket(String, byte[], int, long, String, int, int, int, float, int)}
     *
     * {@link #callDeviceAudio(String, int, int)}
     *
     * {@link #handUpDeviceAudio(String, int)}
     *
     * *　　　　　　　　┏┓　　　┏┓+ +
     *　　　　　　　┏┛┻━━━┛┻┓ + +
     *　　　　　　　┃　　　　　　　┃ 　
     *　　　　　　　┃　　　━　　　┃ ++ + + +
     *　　　　　　 ████━████ ┃+
     *　　　　　　　┃　　　　　　　┃ +
     *　　　　　　　┃　　　┻　　　┃
     *　　　　　　　┃　　　　　　　┃ + +
     *　　　　　　　┗━┓　　　┏━┛
     *　　　　　　　　　┃　　　┃　　　　　　　　　　　
     *　　　　　　　　　┃　　　┃ + + + +
     *　　　　　　　　　┃　　　┃　　　　Code is far away from bug with the animal protecting　　　　　　　
     *　　　　　　　　　┃　　　┃ + 　　　　神兽保佑,代码无bug　　
     *　　　　　　　　　┃　　　┃
     *　　　　　　　　　┃　　　┃　　+　　　　　　　　　
     *　　　　　　　　　┃　 　　┗━━━┓ + +
     *　　　　　　　　　┃ 　　　　　　　┣┓
     *　　　　　　　　　┃ 　　　　　　　┏┛
     *　　　　　　　　　┗┓┓┏━┳┓┏┛ + + + +
     *　　　　　　　　　　┃┫┫　┃┫┫
     *　　　　　　　　　　┗┻┛　┗┻┛+ + + +
     */


    /**
     * 对讲
     * 发送设备对讲音频数据
     *
     * @param key
     * @param audioData       音频数据
     * @param audioDataLength 音频长度
     * @param tsMs            当前时间  默认"20"
     * @param enc             音频类型 默认"G711A"
     * @param sampleRate      采样率  默认"8000"
     * @param sampleWidth     长度  默认"16"
     * @param channel         单通道  只能是"1"
     * @param compressRatio   压缩率  默认是"2.0f"
     * @param index           对应设备下标
     * @return
     */
    public static int sendAudioPacket(String key, byte[] audioData, int audioDataLength, long tsMs
            , String enc, int sampleRate, int sampleWidth, int channel, float compressRatio, int index) {
        return getConnect().SendAudioPacket(key, audioData, audioDataLength, tsMs, enc, sampleRate, sampleWidth, channel, compressRatio, index);
    }

    /**
     * 开启设备对讲功能
     *
     * @param key
     * @param channel
     * @param index
     * @return 0 代表开启正常
     */
    public static int callDeviceAudio(String key, int channel, int index) {
        return getConnect().Call(key, channel, index);
    }

    /**
     * 挂断设备对讲功能
     *
     * @param key
     * @param index
     * @return
     */
    public static int handUpDeviceAudio(String key, int index) {
        return getConnect().HangUp(key, index);

    }

    /**
     * 播放哪路设备音频
     *
     * @param key
     * @param index
     */
    public static void setDeviceAudioIndex(String key, int index) {
        getConnect().PlayAudioIndex(key, index);
    }


    /**
     * 设备视图操作：
     * <p>
     * <p>
     * <p>
     * * *　　　　　　　　┏┓　　　┏┓+ +
     * 　　　　　　　┏┛┻━━━┛┻┓ + +
     * 　　　　　　　┃　　　　　　　┃
     * 　　　　　　　┃　　　━　　　┃ ++ + + +
     * 　　　　　　 ████━████ ┃+
     * 　　　　　　　┃　　　　　　　┃ +
     * 　　　　　　　┃　　　┻　　　┃
     * 　　　　　　　┃　　　　　　　┃ + +
     * 　　　　　　　┗━┓　　　┏━┛
     * 　　　　　　　　　┃　　　┃
     * 　　　　　　　　　┃　　　┃ + + + +
     * 　　　　　　　　　┃　　　┃　　　　Code is far away from bug with the animal protecting
     * 　　　　　　　　　┃　　　┃ + 　　　　神兽保佑,代码无bug
     * 　　　　　　　　　┃　　　┃
     * 　　　　　　　　　┃　　　┃　　+
     * 　　　　　　　　　┃　 　　┗━━━┓ + +
     * 　　　　　　　　　┃ 　　　　　　　┣┓
     * 　　　　　　　　　┃ 　　　　　　　┏┛
     * 　　　　　　　　　┗┓┓┏━┳┓┏┛ + + + +
     * 　　　　　　　　　　┃┫┫　┃┫┫
     * 　　　　　　　　　　┗┻┛　┗┻┛+ + + +
     */

    /**
     * 设备云台控制
     *
     * @param key
     * @param action
     * @param channel
     * @param index
     */
    public static void startDeviceCloudControl(String key, int action, int channel, int index) {
        getConnect().PtzCtrl(key, action, channel, index);
    }

    /**
     * 停止云台控制
     *
     * @param key
     * @param channel
     * @param index
     */
    public static void stopDeviceCloudControl(String key, int channel, int index) {
        getConnect().StopCtrl(key, channel, index);
    }

    /**
     * 开始设备视频录像（本地）
     *
     * @param key
     * @param fileName
     * @param index
     */
    public static void startDeviceRecord(String key, String fileName, int index) {
        getConnect().StartRecord(key, fileName, index);
    }

    /**
     * 结束设备视频录像(本地)
     *
     * @param key
     * @param index
     */
    public static void stopDeviceRecord(String key, int index) {
        getConnect().StopRecord(key, index);
    }

    /**
     * 获取设备录像状态
     *
     * @param key
     * @param index
     * @return
     */
    public static boolean getDeviceRecordState(String key, int index) {
        return getConnect().GetRecordState(key, index);
    }

    /**
     * 设备缩略图
     *
     * @param key
     * @param fileName
     * @param type
     * @param index
     * @return
     */
    public static boolean captureDeviceImage(String key, String fileName, int type, int index) {
        return getConnect().CaptureImage(key, fileName, type, index);
    }

    /**
     * 设备缩略图
     *
     * @param key
     * @param fileName
     * @param index
     * @return
     */
    public static boolean captureDeviceThumbnailImage(String key, String fileName, int index) {
        return getConnect().CaptureThumbnailImage(key, fileName, index);
    }


    /**
     * default
     */


    /**
     * 接口封装
     * <p>
     * <p>
     * <p>
     * ---------------------------------------------------》》》》》》
     */
    /**
     * 增加OSD帧的回调观察者
     *
     * @param abs
     */
    public static void addFrameResultAbs(FrameResultAbs abs) {
        if (abs != null) {
            mFrameResultSubject.addObserver(abs);
        } else {
            Log.e(TAG, "Add FrameResultAbs failed.");
        }


    }

    /**
     * 移除OSD观察者
     *
     * @param abs
     */
    public static void removeFrameResultAbs(FrameResultAbs abs) {
        if (abs != null) {
            mFrameResultSubject.removeObserver(abs);
        } else {
            Log.e(TAG, "Remove FrameResultAbs failed.");
        }


    }

    /**
     * 视频帧的回调
     */
    private class OnDeviceVideoTimeImpl implements OnFrameOSDListener {

        @Override
        public void onFrameOSD(int width, int height, long frame, int length, long osdTime) {
            if (mFrameResultSubject != null) {
                mFrameResultSubject.notifyObserver(width, height, frame, length, osdTime);
            }
        }

        @Override
        public void onOOBFrameOSD(int installMode, int scene, long osdTime) {
            if (mFrameResultSubject != null) {
                mFrameResultSubject.notifyObserver(installMode, scene, osdTime);
            }
        }
    }


    /**
     * 增加回放视频帧观察者
     *
     * @param abs
     */
    public static void addFramePlaybackAbs(FramePlaybackAbs abs) {
        if (abs != null) {
            mFramePlaybackSubject.addObserver(abs);
        }
    }

    /**
     * 移除回放视频帧观察者
     *
     * @param abs
     */
    public static void removeFramePlaybackAbs(FramePlaybackAbs abs) {
        if (abs != null) {
            mFramePlaybackSubject.removeObserver(abs);
        }
    }


    /**
     * 回放视频帧的回调
     * <p>
     * 内部类实现所有相关接口分发
     * <p>
     * <p>
     * <p>  addObserver
     * {@link #addFramePlaybackAbs(FramePlaybackAbs)}
     * <p>
     * <p>
     * <p>  removeObserver
     * {@link #removeFramePlaybackAbs(FramePlaybackAbs)}
     */
    private class OnDeviceVideoPlaybackTimeImpl implements OnPlaybackUpdateTimeListener {

        @Override
        public void OnPlaybackUpdateTime(int time, int index) {
            if (mFramePlaybackSubject != null) {
                mFramePlaybackSubject.notifyObserver(time, index);
            }

        }
    }


    /**
     * 增加设备录像时间获取观察者
     *
     * @param abs
     */
    public static void addDeviceSearchRecordAbs(SearchRecordTimeAbs abs) {
        mSearchRecordTimeSubject.addObserver(abs);

    }

    /**
     * 移除设备录像时间获取观察者
     *
     * @param abs
     */
    public static void removeDeviceSearchRecordAbs(SearchRecordTimeAbs abs) {
        mSearchRecordTimeSubject.removeObserver(abs);
    }

    /**
     * 设备录像时间段
     * <p>
     * <p>
     * <p>
     * <>
     * <p>
     * <p>
     * </>
     */
    private class OnSearchRecordTimeImpl implements OnSearchRecDataListener {

        @Override
        public void OnSearchRecData(int startTime, int endTime, int recType, int index, int end) {
            if (mSearchRecordTimeSubject != null) {
                mSearchRecordTimeSubject.notifyObserver(startTime, endTime, recType, index, end);
            }

        }
    }


    /**
     * 视频帧的回调
     *
     * @deprecated
     */
    private class OnTextureAvailableImpl implements OnTextureAvaibleListener {

        @Override
        public void OnTextureAvaible(int width, int height, byte[] frame, int length, int frame_type, int timestamp, int index, long ctx) {

        }
    }

    /**
     * 链路层网络回调
     * 内部类实现所有接口，通过广播分发所有回调
     * <p>
     * <p>  分发的广播动作
     * <p> {@link #JA_RESULT_CONNECT}
     * <p>
     * <p>
     * <p>
     * <p>  存储的key
     * <p> {@link JAKey}
     * <p>
     * <p>{@link com.app.jagles.controller.JAParameter}
     * <p>
     * <p>
     * <p>
     * 通过发送广播通知
     */
    private class OnDeviceConnectResultImpl implements GLVideoConnect.OnConnectResultListener {

        @Override
        public void onConnectResult(long handle, int status_code, int content_code, int index, String msg) {
            if (BuildConfig.DEBUG) {
                Log.d(TAG, "OnStatus: ------>handle: " + handle + "---status_code: " + status_code
                        + "---index: " + index + "---message: " + msg);
            }

            Intent intent = new Intent(JA_RESULT_CONNECT);
            Bundle bundle = new Bundle();
            bundle.putLong(JAKey.JA_KEY_CONNECT_HANDLE, handle);
            bundle.putInt(JAKey.JA_KEY_CONNECT_STATE, status_code);
            bundle.putInt(JAKey.JA_KEY_CONNECT_CONTENT, content_code);
            bundle.putInt(JAKey.JA_KEY_CONNECT_INDEX, index);
            bundle.putString(JAKey.JA_KEY_CONNECT_KEY, msg);
            intent.putExtras(bundle);
            mContext.sendBroadcast(intent);
//            LocalBroadcastManager.getInstance(mContext).sendBroadcast(intent);
        }
    }


    /**
     * 设备远程数据回调
     * <p>
     * 所有接口数据通过广播分发
     */
    private class OnDeviceRemoteDataImpl implements OnVconDataListener {

        @Override
        public void OnVconData(int index, byte[] vconBuffer, String dev) {
            String tmpRemote = "";
            try {
                tmpRemote = new String(vconBuffer, "utf-8");
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();
            }

            Log.i(TAG, "OnVconData: ----->" + dev + "\n" + tmpRemote);
            if (TextUtils.isEmpty(tmpRemote)) {
                return;
            }

            Intent intent = new Intent(JA_RESULT_REMOTE_DATA);

            Bundle bundle = new Bundle();
            bundle.putInt(JAKey.JA_KEY_REMOTE_INDEX, index);
            bundle.putString(JAKey.JA_KEY_REMOTE_MESSAGE, tmpRemote);
            bundle.putString(JAKey.JA_KEY_REMOTE_DEVICE, dev);
            bundle.putString(JAKey.JA_KEY_CONNECT_KEY, dev);

            intent.putExtras(bundle);

            mContext.sendBroadcast(intent);
//            LocalBroadcastManager.getInstance(mContext).sendBroadcast(intent);
        }
    }


    private class OnTutkStatusImpl implements OnCheckTUTKDevOnlineListener {

        @Override
        public void OnCheckTutkDevOnline(int result, String msg) {
            Intent intent = new Intent(JA_RESULT_TUTK_STATUS);

            Bundle bundle = new Bundle();
            bundle.putInt(JAKey.JA_KEY_CHECK_STATE, result);
            bundle.putString(JAKey.JA_KEY_CONNECT_KEY, msg);

            intent.putExtras(bundle);

            mContext.sendBroadcast(intent);
        }
    }

    /**
     * 广播动作 ： 设备连接状态动作
     * {@linkplain OnDeviceConnectResultImpl}
     */
    public static final String JA_RESULT_CONNECT = "ja_result_connect";

    /**
     * 广播动作： 设备远程数据动作
     * {@linkplain OnDeviceRemoteDataImpl}
     */
    public static final String JA_RESULT_REMOTE_DATA = "ja_result_remote_data";


    /**
     * 广播动作： tutk设备在线状态动作
     * {@linkplain OnTutkStatusImpl}
     */
    public static final String JA_RESULT_TUTK_STATUS = "ja_result_tutk_status";


    public static final class JAKey {
        /*链接层 句柄*/
        public static final String JA_KEY_CONNECT_HANDLE = "ja_key_connect_handle";
        /*连接状态*/
        public static final String JA_KEY_CONNECT_STATE = "ja_key_connect_state";
        /**/
        public static final String JA_KEY_CONNECT_CONTENT = "ja_key_connect_content";

        /*设备下标*/
        public static final String JA_KEY_CONNECT_INDEX = "ja_key_connect_index";
        /*设备标识*/
        public static final String JA_KEY_CONNECT_KEY = "ja_key_connect_message";


        /**
         * <p>
         * <p>设备远程数据
         * <></>
         * bundle key
         */

        /*设备下标*/
        public static final String JA_KEY_REMOTE_INDEX = "ja_key_remote_index";
        /*设备返回数据*/
        public static final String JA_KEY_REMOTE_MESSAGE = "ja_key_remote_message";
        /*设备名*/
        public static final String JA_KEY_REMOTE_DEVICE = "ja_key_remote_device";


        /**
         * tutk在线状态
         */
        /*在线状态*/
        public static final String JA_KEY_CHECK_STATE = "ja_key_check_state";

    }


}
