package com.app.jagles.video;

import android.text.TextUtils;
import android.util.Log;

import com.app.jagles.controller.JAParameter;
import com.app.jagles.listener.OnFrameOSDListener;
import com.app.jagles.listener.OnGSenserDataListener;
import com.app.jagles.networkCallback.OnAudioDataComeListener;
import com.app.jagles.networkCallback.OnCaptureImageListener;
import com.app.jagles.networkCallback.OnCheckTUTKDevOnlineListener;
import com.app.jagles.networkCallback.OnDeviceInfoListener;
import com.app.jagles.networkCallback.OnDirectTextureFrameUpdataListener;
import com.app.jagles.networkCallback.OnDirectTextureOSDFrameUpdataListener;
import com.app.jagles.networkCallback.OnDownloadListListener;
import com.app.jagles.networkCallback.OnDownloadingListener;
import com.app.jagles.networkCallback.OnOSDTextureAvaibleListener;
import com.app.jagles.networkCallback.OnOSDTxtTimeListener;
import com.app.jagles.networkCallback.OnOpenChannelSuccessLinstener;
import com.app.jagles.networkCallback.OnPlaybackUpdateTimeListener;
import com.app.jagles.networkCallback.OnPlayedFirstFrameListener;
import com.app.jagles.networkCallback.OnPullAlarmmsgListener;
import com.app.jagles.networkCallback.OnSearchDeviceListener;
import com.app.jagles.networkCallback.OnSearchRecDataListener;
import com.app.jagles.networkCallback.OnStatusListener;
import com.app.jagles.networkCallback.OnTextureAvaibleListener;
import com.app.jagles.networkCallback.OnVconDataListener;
import com.app.jagles.util.DownloadUtil;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by MiQi on 2016/10/12.
 */

public class GLVideoConnect {

    private final String TAG = this.getClass().getSimpleName();
    private List<DownloadUtil> mList = new ArrayList<>();

    static {
        System.loadLibrary("avutil");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("IOTCAPIs");
        System.loadLibrary("RDTAPIs");
        System.loadLibrary("jnnat");
        System.loadLibrary("JAVideo");
    }

    private String[] mRecFile = new String[36];
    private boolean[] mIsRec = new boolean[36];

    private static GLVideoConnect mGLVideoConnect = new GLVideoConnect();

    protected GLVideoConnect() {
    }

    public int setAddr(String addr, int port) {
        return InitAddr(addr, port);
    }

    /**
     * 带bundleid的初始化
     *
     * @param bundleid
     * @return
     */
    public static GLVideoConnect getInstance(String bundleid) {
        if (pGLVideoConnect == 0) {

            //            initJNICrash();
            InitManager(bundleid);
            GLVideoConnect.bundleid = bundleid;
            pGLVideoConnect = 1;
        }
        return mGLVideoConnect;
    }

    /**
     * 不带bundleid的初始化
     *
     * @return
     */
    public static GLVideoConnect getInstance() {
        if (pGLVideoConnect == 0) {
            //            initJNICrash();
            InitManager("");
            pGLVideoConnect = 1;
        }
        return mGLVideoConnect;
    }

    /**
     * 初始化 jni收集异常
     */
    public static void initJNICrash(String path, String version) {

        if (TextUtils.isEmpty(path)) {
            path = "/JAGlesCrash/";
        }

        File dir = new File(path);
        if (!dir.exists()) {
            boolean temp = dir.mkdir();
            if (!temp) {
                dir.mkdirs();
            }
        }

        nativeInit(dir.getPath(), version);
    }

    /**
     * 销毁网络层底层资源
     */
    public static void destroyInstance() {
        if (pGLVideoConnect != 0) {
            DestroyManager();
            ConnectManager.getInstance().removeAll();
            pGLVideoConnect = 1;
        }
    }

    /**
     * 初始化底层返回的handle
     */
    private static long pGLVideoConnect = 0;
    public static String bundleid = null;

    /**
     * 获取bundleid 如果没有用bundleid初始化获取的是一个空值。
     *
     * @return
     */
    public String getBundleid() {
        return bundleid;
    }

    public long getpGLVideoConnect(String msg) {
        return ConnectManager.getInstance().getConnect(msg);
    }

    private static native void InitManager(String bundleid);

    private static native int InitAddr(String addr, int port);

    public static native void DestroyManager();

    public native int DestroyConnector(long handle);

    private native long GetConnectInstance(String msg);

    private native int GetRealState(long handle);

    private native long[] GetAllConnectCtx(long handle);

    public native void ResetTransfer();

    private native void Connect1(long handle, String connectstr, String verifystr, int index, int channel);

    private native void Connect(long handle, String connectstr, String verifystr, int index);

    private native void DisConnect(long handle, int index);

    private native void OpenChannel(long handle, int bitrate, int channel, int index);

    private native void CloseChannel(long handle, int bitrate, int channel, int index);

    private native int SendAudioPacket(long handle, byte[] buffer, int size, long tsMs, String enc, int samplerate, int samplewidth, int channel, float compressRatio, int index);

    private native int Call(long handle, int channel, int index);

    private native int HangUp(long handle, int index);

    private native void SendData(long handle, byte[] buffer, int size, int index);

    private native void PausePlayback(long handle, int channel, int index);

    private native void ResumePlayback(long handle, int channel, int index);

    private native int GetBitrate(long handle, int index);

    private native void PtzCtrl(long handle, int action, int channel, int index);

    private native void StopCtrl(long handle, int channel, int index);

    private native void SearchRec(long handle, int startTime, int endTime, int channel, int index);

    private native void StartPlayback(long handle, int startTime, int channel, int index);

    private native void StopPlayback(long handle, int channel, int index);

    private native boolean CaptureImage(long handle, String filename, int type, int index);

    private native boolean CaptureThumbnailImage(long handle, String filename, int index);

    private native boolean StartRecord(long handle, String filename, int index);

    private native void StopRecord(long handle, int index);

    private native int GetNetWorkSpeed(long handle, int index);

    private native int GetAllNetWorkSpeed(long handle);

    private native int GetChannel(long handle, int index);

    private native boolean GetRecordState(long handle, int index);

    public native int SearchDevice();

    private native void PullAlarmmsg(long handle, int type, int channel, long ticket, int index);

    private native void UseDirectTexture(long handle, boolean isDirect);

    private native void DecoderPause(long handle, int index);

    private native void DecoderResume(long handle, int index);

    private native void PlayAudioIndex(long handle, int index);

    private native int GetWallModelType(long handle, int index);

    public native void IsForceWallMode(boolean isWallMode);

    private native void EnableCrop(long handle, boolean value);

    private native void DoDisConnect(long handle, int index);

    private native int GetFPS(long handle, int index);

    private native int GetHowBitrate(long handle, int index);

    private native boolean isNeedOpen64Stream(long handle, int index);

    private native int getIsInstallModeCome(long handle, int index);

    public native void ResetForceWallMode();

    private native void setOnlyDecoderIFrame(long handle, boolean value, int index);

    public native String getFileDownloadList(long handle, int index);

    public native void doStopDownload(long handle);

    public native void doDownloadFile(long handle, int opType, int opFlag, String file_name, String file_path, int index);

    public native int doCheckTutkOnline(long handle, String tutkid, int timeout, String msg);

    public native void SetHardwareDecoder(long handle, int index, boolean value, int width, int height);

    public native int FinishDeviceFileTransfer(long handle, int index);

    public native void SetTimeZone(long handle, float timezone, int index);

    public static native int nativeInit(String crashPath, String version);

    //    public static native void DoCrash();

    public void Set_TimeZone(String msg, float timezone, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code setOnlyDecoderIFrame");
            return;
        }
        SetTimeZone(handle, timezone, index);
    }

    public int FinishDeviceFileTransfer(String msg, int index) {
        Log.i("Lee", "测试测试。。。。。");
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code");
            return -1;
        }
        return FinishDeviceFileTransfer(handle, index);
    }

    public void setOnlyDecoderIFrame(String msg, boolean value, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code setOnlyDecoderIFrame");
            return;
        }
        setOnlyDecoderIFrame(handle, value, index);
    }

    public void doDownloadFile(String msg, int opType, int opFlag, String item_fp, String file_path, int index) {
        Log.i("Lee", "GLVideoConnect.doDownloadFile");  //能被执行
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code doDownloadFile");
            return;
        }

        doDownloadFile(handle, opType, opFlag, item_fp, file_path, index);
    }

    public void doStopDownload(String msg) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (mList != null)
            mList.clear();
        if (handle == 0) {
            return;
        }
        doStopDownload(handle);
    }

    public String getFileDownloadList(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code");
            return null;
        }
        Log.d(TAG, "getFileDownloadList: " + handle + "----" + index);
        return getFileDownloadList(handle, index);
        //        return null;
    }

    public int getIsInstallModeCome(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code getIsInstallModeCome");
            return 3;
        }
        int result = getIsInstallModeCome(handle, index);
        if (result == 3 && mFrameWidth != 0 && mFrameHeight != 0) {
            if (mFrameWidth / mFrameHeight < 1.33) {
                result = 1;
            } else {
                result = 0;
            }
        }
        return result;
    }


    public boolean isNeedOpen64Stream(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code isNeedOpen64Stream");
            return false;
        }
        return isNeedOpen64Stream(handle, index);
    }

    public int GetHowBitrate(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code GetHowBitrate");
            return 0;
        }
        return GetHowBitrate(handle, index);
    }

    public int GetFps(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code GetFps");
            return 0;
        }
        return GetFPS(handle, index);
    }


    //实验性接口
    public void DoDisConnect(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code DoDisConnect");
            return;
        }
        DoDisConnect(handle, index);
    }

    public String GetConnect(String msg) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            handle = GetConnectInstance(msg);
            ConnectManager.getInstance().addConnect(msg, handle);
            Log.d("honglee_1110", "^^^^^^^^^^^^^^^^^^^^^^^----" + msg);
        }
        //        else
        //            return "-1";

        return msg;
    }

    public String DoCheckTutkOnline(String msg, String tutkid, int timeout) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        Log.i("Lee", "GLVideoConnect.DoCheckTutkOnline  msg = " + msg + "  tutkid = " + tutkid + "  handle = " + handle);
        if (handle == 0) {
            handle = GetConnectInstance(msg);
            doCheckTutkOnline(handle, tutkid, timeout, msg);
        } else {
            return "-1";
        }
        //doCheckTutkOnline(handle, tutkid, timeout, msg);
        return msg;
    }


    public void Connect(String msg, String connectstr, String verifystr, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code Connect");
            return;
        }
        Connect(handle, connectstr, verifystr, index);
    }

    public void SetHardwareDecoder(String msg, boolean value, int index, int width, int height) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code SetHardwareDecoder");
            return;
        }
        Log.d("testhardwaredecoder", "index-->" + index + ",value-->" + value + ",width-->" + width + ",height-->" + height);
        SetHardwareDecoder(handle, index, value, width, height);
    }

    public int GetRealStatus(String msg) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code Connect1");
            return 0;
        }
        return GetRealState(handle);
    }

    public int DestroyConnector(String msg) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code DestroyConnector");
            return -1;
        }
        return DestroyConnector(handle);
    }

    public long[] GetAllConnectCtx(String msg) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code GetAllConnectCtx");
            return null;
        }
        return GetAllConnectCtx(handle);
    }



    public void Connect(String msg, String connectstr, String verifystr, int index, int channel) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code Connect1");
            return;
        }
        Log.d(TAG, "执行了。。。。。。。。。。。");
        Connect1(handle, connectstr, verifystr, index, channel);
    }

    public void DisConnect(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code DisConnect");
            return;
        }
        Log.d(TAG, "DisConnect: ----------->" + handle);
        DisConnect(handle, index);
        //   ConnectManager.getInstance().removeConnect(msg);
    }

    public void OpenChannel(String msg, int bitrate, int channel, int index) {
        Log.d(TAG, "打开码流" + "msg:" + msg + " bitrate:" + bitrate + " channel:" + channel + " index:" + index);
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code OpenChannel");
            return;
        }
        OpenChannel(handle, bitrate, channel, index);
    }

    public void CloseChannel(String msg, int bitrate, int channel, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code CloseChannel");
            return;
        }
        CloseChannel(handle, bitrate, channel, index);
    }

    public int SendAudioPacket(String msg, byte[] buffer, int size, long tsMs, String enc, int samplerate, int samplewidth, int channel, float compressRatio, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code SendAudioPacket");
            return -1;
        }
        return SendAudioPacket(handle, buffer, size, tsMs, enc, samplerate, samplewidth, channel, compressRatio, index);
    }

    public int Call(String msg, int channel, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code Call");
            return -1;
        }
        return Call(handle, channel, index);
    }

    public int HangUp(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code HangUp");
            return -1;
        }
        return HangUp(handle, index);
    }

    public void SendData(String msg, byte[] buffer, int size, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code SendData");
            return;
        }
        SendData(handle, buffer, size, index);
    }

    public void PausePlayback(String msg, int channel, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code PausePlayback");
            return;
        }
        PausePlayback(handle, channel, index);
    }

    public void ResumePlayback(String msg, int channel, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code ResumePlayback");
            return;
        }
        ResumePlayback(handle, channel, index);
    }

    public int GetBitrate(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code GetBitrate");
            return 0;
        }
        return GetBitrate(handle, index);
    }

    public void PtzCtrl(String msg, int action, int channel, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code PtzCtrl");
            return;
        }
        PtzCtrl(handle, action, channel, index);
    }

    public void StopCtrl(String msg, int channel, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code StopCtrl");
            return;
        }
        StopCtrl(handle, channel, index);
    }

    public void SearchRec(String msg, int startTime, int endTime, int channel, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code SearchRec");
            return;
        }
        SearchRec(handle, startTime, endTime, channel, index);
    }

    public void StartPlayback(String msg, int startTime, int channel, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code StartPlayback");
            return;
        }
        StartPlayback(handle, startTime, channel, index);
    }

    public void StopPlayback(String msg, int channel, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        Log.d(TAG, "StopPlayback: ------->" + handle);
        if (handle == 0) {
            Log.e(TAG, "not init native code StopPlayback");
            return;
        }
        StopPlayback(handle, channel, index);
    }

    public boolean CaptureImage(String msg, String filename, int type, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code CaptureImage");
            return false;
        }
        return CaptureImage(handle, filename, type, index);
    }

    public boolean CaptureThumbnailImage(String msg, String filename, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code CaptureThumbnailImage");
            return false;
        }
        return CaptureThumbnailImage(handle, filename, index);
    }

    public boolean StartRecord(String msg, String filename, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code StartRecord");
            return false;
        }
        //        this.mIsRec[index] = true;
        //        this.mRecFile[index] = filename;
        return StartRecord(handle, filename, index);
    }

    public void StopRecord(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code");
            return;
        }
        //        this.mIsRec[index] = false;
        //        this.mRecFile[index] = "";
        StopRecord(handle, index);
    }

    public int GetNetWorkSpeed(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code GetNetWorkSpeed");
            return 0;
        }
        return GetNetWorkSpeed(handle, index);
    }

    public int GetAllNetWorkSpeed(String msg) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code GetAllNetWorkSpeed");
            return 0;
        }
        return GetAllNetWorkSpeed(handle);
    }

    public int GetChannel(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code GetChannel");
            return 0;
        }
        return GetChannel(handle, index);
    }

    public boolean GetRecordState(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code GetRecordState");
            return false;
        }
        return GetRecordState(handle, index);
    }

    public void PullAlarmmsg(String msg, int type, int channel, long ticket, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code PullAlarmmsg");
            return;
        }
        PullAlarmmsg(handle, type, channel, ticket, index);
    }

    public void UseDirectTexture(String msg, boolean isDirect) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code UseDirectTexture");
            return;
        }
        UseDirectTexture(handle, isDirect);
    }

    public void DecoderPause(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code DecoderPause");
            return;
        }
        DecoderPause(handle, index);
    }

    public void DecoderResume(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code DecoderResume");
            return;
        }
        DecoderResume(handle, index);
    }

    public void PlayAudioIndex(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code PlayAudioIndex");
            return;
        }
        PlayAudioIndex(handle, index);
    }

    public int GetWallModelType(String msg, int index) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code GetWallModelType");
            return 0;
        }
        return GetWallModelType(handle, index);
    }

    public void EnableCrop(String msg, boolean value) {
        long handle = ConnectManager.getInstance().getConnect(msg);
        if (handle == 0) {
            Log.e(TAG, "not init native code EnableCrop");
            return;
        }
        EnableCrop(handle, value);
    }

    private int mFrameWidth = 0;
    private int mFrameHeight = 0;

    //----------------------------------------这是分割线---------------------------------------------
    public void OnTextureAvaible(int width, int height, byte[] frame, int length, int frame_type,
                                 int timestamp, int index, long ctx) {
        //        //TODO:回调帧画面
        //        if (CallBackFirstFrameEvent.getInstance().getOnFirstFrameListener() != null) {
        //            CallBackFirstFrameEvent.getInstance().getOnFirstFrameListener().onCallBackFirstFrame(true);
        //        }
        //        if (GLPlayFrameComeBackEventBus.getInstance().getOnFrameComeBackListener() != null) {
        //            GLPlayFrameComeBackEventBus.getInstance().getOnFrameComeBackListener().onFrameComeBack(true);
        //        }

        //        Log.i(TAG, "OnTextureAvaible:  看看有没有帧过来");
        mFrameWidth = width;
        mFrameHeight = height;
        Log.d(TAG, "OnTextureAvaible:mFrameWidth-->" + mFrameWidth + " mFrameHeight--->" + mFrameHeight + "----" + mOnTextureAvaibleListener);
        if (mOnTextureAvaibleListener != null) {
            mOnTextureAvaibleListener.OnTextureAvaible(width, height, frame, length, frame_type, timestamp, index, ctx);
        }
    }

    public void OnCaptureImage(int success, int index) {

        Log.d(TAG, "OnCaptureImage: -----success: " + success + " index: " + index);
        if (mOnCaptureImageListener != null)
            mOnCaptureImageListener.OnCaptureImage(success, index);
        else
            Log.e(TAG, "mOnCaptureImageListener is null");
    }

    public void OnOSDTextureAvaible(int width, int height, long frame, int length, long utctime) {
//        Log.d(TAG, "OnOSDTextureAvaible: ------>-----" + frame + "---" + mOnOSDTextureAvaibleListener + "----utc:" + utctime);
        int installMode = -1;
        if (mOnOSDTextureAvaibleListener != null) {
            installMode = mOnOSDTextureAvaibleListener.OnOSDTextureAvaible(width, height, frame, length, utctime);
        } else {
            Log.e(TAG, "mOnOSDTextureAvaibleListener is null");
        }
        if (mOnFrameOSDListener != null) {
            mOnFrameOSDListener.onFrameOSD(width, height, frame, length, utctime);
            if (installMode != -1) {
                mOnFrameOSDListener.onOOBFrameOSD(installMode, 0, utctime);
            }
        }

        if (mOnFrameTimeListener != null) {
            mOnFrameTimeListener.onFrameTime(width, height, frame, length, utctime);
        }
    }

    //TODO：新增接口
    private OnFrameTimeListener mOnFrameTimeListener;

    /**
     * @deprecated
     */
    public interface OnFrameTimeListener {
        void onFrameTime(int width, int height, long frame, int length, long utctime);
    }

    /**
     * @param listener
     * @deprecated
     */
    public void setOnFrameTimeListener(OnFrameTimeListener listener) {
        mOnFrameTimeListener = listener;
    }

    private OnFrameOSDListener mOnFrameOSDListener;

    public void setOnFrameOSDListener(OnFrameOSDListener listener) {
        mOnFrameOSDListener = listener;
    }

    public OnFrameOSDListener getOnFrameOSDListener() {
        return mOnFrameOSDListener;
    }


    /**
     * 连接状态 的回调
     *
     * @param handle
     * @param status_code
     * @param content_code
     * @param index
     * @param msg
     */
    public void OnStatus(long handle, int status_code, int content_code, int index, String msg) {
        Log.i("Lee", "GLVideoConnect.OnStatus  status_code = " + status_code + "----index:" + index + " Msg:" + msg + " mOnStatusListener:" + mOnStatusListener + " this:" + this);
        if (mOnStatusListener != null) {
            mOnStatusListener.OnStatus(handle, status_code, content_code, index, msg);
        }
        if (mOnConnectResultListener != null) {
            mOnConnectResultListener.onConnectResult(handle, status_code, content_code, index, msg);
        }
    }

    private OnConnectResultListener mOnConnectResultListener;

    public void setOnConnectResultListener(OnConnectResultListener listener) {
        mOnConnectResultListener = listener;

    }

    public interface OnConnectResultListener {
        void onConnectResult(long handle, int status_code, int content_code, int index, String msg);
    }


    /**
     * 搜索录像时间段 的回调
     *
     * @param startTime
     * @param endTime
     * @param recType
     * @param index
     * @param end
     */
    public void OnSearchRecData(int startTime, int endTime, int recType, int index, int end) {
        if (mOnSearchRecDataListener != null)
            mOnSearchRecDataListener.OnSearchRecData(startTime, endTime, recType, index, end);
        else
            Log.e(TAG, "mOnSearchRecDataListener is null");
    }

    /**
     * 回放时间 的回调
     *
     * @param time
     * @param index
     */
    public void OnPlaybackUpdateTime(int time, int index) {
        Log.d(TAG, "OnPlaybackUpdateTime: -------->" + "time:" + time + "---index:" + index + "----" + mOnPlaybackUpdateTimeListener);
        if (mOnPlaybackUpdateTimeListener != null)
            mOnPlaybackUpdateTimeListener.OnPlaybackUpdateTime(time, index);
    }

    /**
     * 远程数据数据 的回调
     *
     * @param index
     * @param vconBuffer
     * @param dev
     */
    public void OnVconData(int index, byte[] vconBuffer, String dev) {
        Log.d(TAG, "OnVconData: ------>" + vconBuffer.length);
        if (mOnVconDataListener != null)
            mOnVconDataListener.OnVconData(index, vconBuffer, dev);
        else
            Log.e(TAG, "mOnVconDataListener is null");
    }

    public void OnOSDTxtTime(int osdTime, int index) {
        Log.d(TAG, "OnOSDTxtTime: ------->" + osdTime);
        if (mOnOSDTxtTimeListener != null)
            mOnOSDTxtTimeListener.OnOSDTxtTime(osdTime, index);
        else
            Log.e(TAG, "mOnOSDTxtTimeListener is null");
    }

    public void OnSearchDevice(String eseeId, String ip, int port, int max_ch, String devModel) {
        if (mOnSearchDeviceListener != null)
            mOnSearchDeviceListener.OnSearchDevice(eseeId, ip, port, max_ch, devModel);
        else
            Log.e(TAG, "mOnSearchDeviceListener is null");
    }

    public void OnPullAlarmmsg(int type, int channel, long ticket, String src, String enc, int fps,
                               int width, int height, int samplerate, int samplewidth, int channelAudio,
                               float compress_ratio, int totalLen, int curLen, byte[] dataBuffer) {
        if (mOnPullAlarmmsgListener != null)
            mOnPullAlarmmsgListener.OnPullAlarmmsg(type, channel, ticket, src, enc, fps, width, height, samplerate,
                    samplewidth, channelAudio, compress_ratio, totalLen, curLen, dataBuffer);
        else
            Log.e(TAG, "mOnPullAlarmmsgListener is null");
    }

    public void OnDirectTextureOSDFrameUpdata(int width, int height, long textBuffer, int index) {
        Log.i("Lee", "GLVideoConnect.OnDirectTextureOSDFrameUpdata textBuffer = " + textBuffer);
        if (mOnDirectTextureOSDFrameUpdataListener != null)
            mOnDirectTextureOSDFrameUpdataListener.OnDirectTextureOSDFrameUpdata(width, height, textBuffer, index);
        else
            Log.e(TAG, "mOnDirectTextureOSDFrameUpdataListener is null");
    }

    public void OnDirectTextureFrameUpdata(int width, int height, long directBuffer, int index) {
        if (mOnDirectTextureFrameUpdataListener != null)
            mOnDirectTextureFrameUpdataListener.OnDirectTextureFrameUpdata(width, height, directBuffer, index);
        else
            Log.e(TAG, "mOnDirectTextureFrameUpdataListener is null");
    }

    public void OnGSensorData(long timeStamp, double x, double y, double z) {
        if (mOnGSensorDataListener != null)
            mOnGSensorDataListener.OnGSensorData(timeStamp, x, y, z);
    }

    public void OnAudioData(byte[] data) {
        if (mOnAudioDataComeListener != null)
            mOnAudioDataComeListener.OnAudioDataCome(data);
        else
            Log.e(TAG, "mOnAudioDataComeListener is null");
    }


    public void OnOpenChanneledSuccess(int mChannelId, int nStreamId, String camDes) {
        if (mOnOpenChannelSuccessLinstener != null)
            mOnOpenChannelSuccessLinstener.OpenChannelSuccessListener(mChannelId, nStreamId, camDes);
    }

    /**
     * 是否是720设备
     *
     * @param isP720
     */
    public void OnDeviceInfo(int isP720) {
        if (mOnDeviceInfoListener != null) {
            mOnDeviceInfoListener.OnDeviceInfo(isP720 == 0 ? true : false);
        }
    }

    public void OnOOBFrameAvailable(int installMode, int scene) {
        Log.d(TAG, "OnOOBFrameAvailable() called with: installMode = [" + installMode + "], scene = [" + scene + "]");
        if (mOnFrameOSDListener != null) {
            mOnFrameOSDListener.onOOBFrameOSD(installMode, scene, 0);
        }
    }

    //下载列表回调
    public void OnDownloadList(String item_fp, int item_size, String suffix, int item_type, int flag) {
        Log.i("CJZ", "Doing Download"); //这个log用来证明有没有跑到doDownloadFile函数
        DownloadUtil util = new DownloadUtil();
        util.setFile_name(item_fp);
        util.setFile_suffix(suffix);
        util.setFile_size(item_size);
        mList.add(util);

        if (flag == 1 && mOnDownloadListListener != null) {
            mOnDownloadListListener.OnDownloadList(mList);
        }
        Log.i("Lee", "GLVideoConnect.OnDownloadList  item_fp = " + item_fp + "   item_size = " + item_size + "  suffix = " + suffix + "   item_type = " + item_type + "  flag = " + flag);
    }

    public void OnDownloading(int flag, int fdsize, long pos, int end) {
        if (mOnDownloadingListener != null)

            mOnDownloadingListener.OnDownloading(flag, fdsize, pos, end);
    }

    public void OnPlayedFirstFrame(int is180) {
        Log.i("Lee", "GLVideoConnect.OnPlayedFirstFrame");
        if (mOnPlayedFirstFrameListener != null)
            mOnPlayedFirstFrameListener.OnPlayedFirstFrame(is180);
    }

    public void OnCheckTUTKDevOnline(int result, String msg) {
        Log.d(TAG, "OnCheckTUTKDevOnline() called with: result = [" + result + "], msg = [" + msg + "]");
        if (mOnCheckTUTKDevOnlineListener != null)
            mOnCheckTUTKDevOnlineListener.OnCheckTutkDevOnline(result, msg);
    }

    public String GetCurrentRecordFileName(int index) {
        if (index < 0 || index > 35)
            return "";
        return mRecFile[index];
    }

    public boolean GetRecordScreenStatus(int index) {
        if (index < 0 || index > 35)
            return false;
        return mIsRec[index];
    }

    public OnCaptureImageListener mOnCaptureImageListener;
    public OnDirectTextureFrameUpdataListener mOnDirectTextureFrameUpdataListener;
    public OnDirectTextureOSDFrameUpdataListener mOnDirectTextureOSDFrameUpdataListener;
    public OnOSDTextureAvaibleListener mOnOSDTextureAvaibleListener;
    public OnOSDTxtTimeListener mOnOSDTxtTimeListener;
    public OnPlaybackUpdateTimeListener mOnPlaybackUpdateTimeListener;
    public OnPullAlarmmsgListener mOnPullAlarmmsgListener;
    public OnSearchDeviceListener mOnSearchDeviceListener;
    public OnSearchRecDataListener mOnSearchRecDataListener;
    public OnStatusListener mOnStatusListener;
    public OnTextureAvaibleListener mOnTextureAvaibleListener;
    public OnVconDataListener mOnVconDataListener;
    public OnAudioDataComeListener mOnAudioDataComeListener;
    public OnOpenChannelSuccessLinstener mOnOpenChannelSuccessLinstener;
    public OnDeviceInfoListener mOnDeviceInfoListener;
    public OnPlayedFirstFrameListener mOnPlayedFirstFrameListener;
    public OnDownloadListListener mOnDownloadListListener;
    public OnDownloadingListener mOnDownloadingListener;
    public OnCheckTUTKDevOnlineListener mOnCheckTUTKDevOnlineListener;
    public OnGSenserDataListener mOnGSensorDataListener;

}
