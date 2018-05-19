package com.app.jagles.video;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.EGL14;
import android.opengl.EGLConfig;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.os.SystemClock;
import android.text.TextUtils;
import android.util.Log;

import com.app.jagles.HWCodec.FullFrameRect;
import com.app.jagles.HWCodec.GlUtil;
import com.app.jagles.HWCodec.HWDecoder;
import com.app.jagles.HWCodec.HWMP4Encoder;
import com.app.jagles.HWCodec.HWPlayer;
import com.app.jagles.HWCodec.Texture2dProgram;
import com.app.jagles.HWCodec.gles.EglCore;
import com.app.jagles.controller.JAParameter;
import com.app.jagles.listener.AudioDataListener;
import com.app.jagles.listener.CaptureImageListener;
import com.app.jagles.listener.ConnectStatusListener;
import com.app.jagles.listener.DestoryListener;
import com.app.jagles.listener.FrameDataComeListener;
import com.app.jagles.listener.GLVideoSurfaceCreateListener;
import com.app.jagles.listener.HWDecoderCaptureLinstener;
import com.app.jagles.listener.OSDTimeListener;
import com.app.jagles.listener.OnGSenserDataListener;
import com.app.jagles.listener.OnVideoTextureComeListener;
import com.app.jagles.listener.PlaybackUpdateTimeListener;
import com.app.jagles.listener.PlayfileProgress;
import com.app.jagles.listener.RecordVideoListener;
import com.app.jagles.listener.SearchDeviceListener;
import com.app.jagles.listener.SearchRecDataListener;
import com.app.jagles.listener.VconDataListener;
import com.app.jagles.networkCallback.OnAudioDataComeListener;
import com.app.jagles.networkCallback.OnDirectTextureFrameUpdataListener;
import com.app.jagles.networkCallback.OnOSDTextureAvaibleListener;
import com.app.jagles.networkCallback.OnPlayedFirstFrameListener;
import com.app.jagles.networkCallback.OnStatusListener;
import com.app.jagles.networkCallback.OnTextureAvaibleListener;
import com.app.jagles.util.ImageUtil;
import com.app.jagles.util.LanguageUtil;
import com.app.jagles.view.OnGLDisplayCreateListener;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.concurrent.atomic.AtomicBoolean;

import javax.microedition.khronos.opengles.GL10;


/**
 * Created by MiQi on 16/3/13.
 */
public class GLVideoRender implements GLSurfaceViewOrg.Renderer, OnDirectTextureFrameUpdataListener
        , OnTextureAvaibleListener, OnOSDTextureAvaibleListener,
        OnStatusListener, OnAudioDataComeListener, OnPlayedFirstFrameListener, HWDecoderCaptureLinstener, HWPlayer.PresentAudioData,
        OnGSenserDataListener {

    private static final String TAG = "GLVideoRender";

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

    /**
     * jni层实例化的handle
     */
    public long mParametricManager = 0;

    /**
     * opengl要用到的屏幕的宽高比
     */
    private float mAspect = 1;
    private final GLVideoDisplay mView;
    private boolean[] runend = new boolean[36];
    private boolean[] comed = new boolean[36];
    private boolean[] runendOSD = new boolean[1];
    public AnimationEvent mEvent = null;
    private int mWidth = 0;
    private int mHeight = 0;
    private int recordStatuTexId_1 = 0;
    private int recordStatuTexId_2 = 0;
    public boolean mHardwareDecoder = false;
    public int mMaxSupportWidth = 0;
    public int mMaxSupportHeight = 0;

    private HWDecoder hwdecoder = null;
    private HWMP4Encoder hwencoder = null;
    private HWPlayer hwplayer = null;
    private HWPlayer.PresentTimeCallback mCallback = null;
    private FullFrameRect mFullScreen;
    private final float[] mIdentityMatrix;
    public static long conn = 0;
    private boolean mRecordScreen = false;
    private String mRecordFileName = "";
    private int mFileTexture = 0;
    private int mFileWidth = 0;
    private int mFileHeight = 0;

    private long nowTime;
    private long lastTime = 0;

    public boolean isRotateScreen = false;
    public boolean isPauseDraw = false;

    public AtomicBoolean isDestory = new AtomicBoolean();
    private AtomicBoolean isDestoryed = new AtomicBoolean();
    public int mScreenMode = 0;

    public float aspect;

    private boolean[] mEnableKeepAspect = new boolean[36];
    private boolean[] mShowLoading = new boolean[36];
    private int mInputTexture = 0;

    //蓝牙sensor数据
    private List<SensorInfo> mSensorList = new ArrayList<>();
    private long[] mConnectCtxs;

    /**
     * 初始化底层资源
     *
     * @param aspect        屏幕宽高比
     * @param window_width
     * @param window_height
     * @param FontPath
     * @return
     */
    public native long InitManager(float aspect, int window_width, int window_height, String FontPath);

    /**
     * 设置播放的显示模式
     *
     * @param handle
     * @param index
     */
    public native void SetMode(long handle, int index);

    public native void DrawRequest();

    /**
     * 开始动画的动作
     *
     * @param handle
     * @param pend     XYZ 目标坐标
     * @param step     分多少步执行
     * @param duration 执行时间
     * @param isloop   是否循环
     * @param type     0 位置 1缩放 2 旋转
     * @param texture  是否是贴图
     * @param scnindex 多分屏 {0,1,2}
     * @param inertia  是否是惯性
     * @param endmsg   动画结束标志
     * @return
     */
    public native long StartAnimation(long handle, float[] pend, int step, int duration, boolean isloop, int type, boolean texture, int scnindex, boolean inertia, int endmsg);

    /**
     * 停止动画的动作
     *
     * @param handle
     * @param anihandle
     */
    public native void StopAnimation(long handle, long anihandle);

    /**
     * 贴图数据传入底层
     *
     * @param handle
     * @param w
     * @param h
     * @param texture
     * @param frame_len
     * @param frame_type
     * @param timestamp
     * @param index
     * @param ctx
     */
    public native void TextureAvaible(long handle, int w, int h, byte[] texture, int frame_len, int frame_type, int timestamp, int index, long ctx);

    /**
     * OSD的贴图数据传进底层
     *
     * @param handle
     * @param texture
     * @param frame_len
     * @param width
     * @param height
     */
    public native void OSDTextureAvaible(long handle, long texture, int frame_len, int width, int height);

    public native void OSDTextureAvaible2(long handle, String texture);

    /**
     * 销毁底层数据
     *
     * @param handle
     * @return
     */
    public native boolean DestroyManager(long handle);

    /**
     * 画图函数
     *
     * @param handle
     */
    public native void DrawParametric(long handle);

    /**
     * 设置modle或贴图的位置，大小，和旋转角度
     *
     * @param handle
     * @param pos
     * @param type
     * @param texture
     * @param index
     */
    public native void TransformObject(long handle, float[] pos, int type, boolean texture, int index);

    /**
     * 文件播放，目前还不生效
     *
     * @param handle
     * @param filename
     * @param isImage
     * @param isFishEye
     */
    public native void Playfile(long handle, String filename, boolean isImage, boolean isFishEye);

    /**
     * 停止文件播放
     *
     * @param handle
     */
    public native void StopPlay(long handle);

    /**
     * 获取目前选择的窗口的位置，大小和旋转角度
     *
     * @param handle
     * @param type
     * @param texture
     * @param index
     * @return
     */
    public native float[] GetObjectPosition(long handle, int type, boolean texture, int index);

    /**
     * 普通模式设置当前的屏幕分割
     *
     * @param handle
     * @param index
     */
    public native void SetSplit(long handle, int index);

    /**
     * 普通模式设置当前显示的是那一个页面
     *
     * @param handle
     * @param page
     */
    public native void SetScreenPage(long handle, int page);

    /**
     * 普通模式下设置当前选择的video
     *
     * @param handle
     * @param x
     * @param y
     * @param w
     * @param h
     */
    public native void SetSelected(long handle, int x, int y, int w, int h);
    //新的加上的接口

    /**
     * 设置当前选中的video的下标
     *
     * @param handle
     * @param index
     */
    public native void SetSelectedByIndex(long handle, int index);

    /**
     * 获取当前的显示页面的下标
     *
     * @param handle
     * @return
     */
    public native int GetPageIndex(long handle);

    /**
     * 获取当前显示页面的video的总数
     *
     * @param handle
     * @return
     */
    public native int GetScreenCount(long handle);

    /**
     * 获取当前普通模式下所有的页面数
     *
     * @param handle
     * @return
     */
    public native int GetAllPage(long handle);

    /**
     * 获取当前选择的video的下标
     *
     * @param handle
     * @return
     */
    public native int GetVideoIndex(long handle);

    /**
     * 获取当前的显示模式
     *
     * @param handle
     * @return
     */
    public native int GetMode(long handle);

    /**
     * 设置当前的某个video变成单屏
     *
     * @param handle
     * @param index
     * @param ani
     */
    public native void SetSingVideo(long handle, int index, boolean ani);

    /**
     * 获取当前的普通模式的切割模式
     *
     * @param handle
     * @return
     */
    public native int GetSplitMode(long handle);

    /**
     * 清理动画队列里面的动画
     *
     * @param handle
     */
    public native void ClearAnimation(long handle);

    /**
     * 获取当前选中的video的放大倍数
     *
     * @param handle
     * @param texture
     * @param index
     * @return
     */
    public native float[] GetScale(long handle, boolean texture, int index);

    /**
     * 重置modle或贴图的顶点
     *
     * @param handle
     * @param texture
     * @param index
     */
    public native void ResetPosition(long handle, boolean texture, int index);

    /**
     * 获取当前传入下标的video是否显示出来
     *
     * @param handle
     * @param index
     * @return
     */
    public native boolean GetVisibility(long handle, int index);

    /**
     * 传送当前的网络状态进去
     *
     * @param handle
     * @param status_code
     * @param content_code
     * @param index
     */
    public native void DoStatus(long handle, String status_code, int content_code, int index);

    /**
     * 在普通模式下横竖屏幕的时候要执行这个方法重新分割
     *
     * @param handle
     * @param aspect
     * @param width
     * @param height
     */
    public native void ReSizeSplite(long handle, float aspect, int width, int height);

    /**
     * 陀螺仪的接口
     *
     * @param handle
     * @param rate
     * @param gra
     */
    public native void VRSensor(long handle, float[] rate, float[] gra, int or_type, int mode);

    /**
     * 加载录像的贴图
     *
     * @param handle
     * @param textid
     */
    public native void LoadRecordStatuTexture(long handle, int textid);

    /**
     * 设置屏幕的宽和高
     *
     * @param handle
     * @param width
     * @param height
     */
    public native void SetWindowWidthHeight(long handle, int width, int height);

    /**
     * 手势操作触摸屏按下去
     *
     * @param handle
     * @param x
     * @param y
     */
    public native void DoTapOrMouseDown(long handle, int x, int y);

    /**
     * 手势操作触摸屏移动
     *
     * @param handle
     * @param x
     * @param y
     * @param wallmode
     * @param index
     */
    public native void DoTapOrMouseMove(long handle, int x, int y, int wallmode, int index);

    /**
     * 手势操作触摸屏放松
     *
     * @param handle
     * @param x
     * @param y
     * @param wallMode
     */
    public native void DoTapOrMouseUp(long handle, int x, int y, int wallMode);

    /**
     * 手势操作触摸屏放大
     *
     * @param handle
     * @param intra
     * @param x
     * @param y
     * @param index
     */
    public native void DoTapOrMouseWheel(long handle, int intra, int x, int y, int index, int wallMode);

    /**
     * 手势操作双击触摸屏
     *
     * @param handle
     */
    public native void DoDoubleTap(long handle, int wallMode, int scene, int scrnFourIndex, int index);


    /**
     * 横竖屏更新屏幕比例
     *
     * @param handle
     * @param aspect
     */
    public native void UpdateAspect(long handle, float aspect);

    /**
     * 设置总的通道数
     *
     * @param handle
     * @param allPage
     */
    public native void SetAllPage(long handle, int allPage);

    /**
     * 圆通收缩
     *
     * @param handle
     */
    public native void CylinderWind(long handle);

    /**
     * 圆通展开
     *
     * @param handle
     */
    public native void CylinderUnwind(long handle);

    /**
     * 鱼眼收缩
     *
     * @param handle
     */
    public native void HemisphereWind(long handle);

    /**
     * 鱼眼展开
     *
     * @param handle
     */
    public native void HemisphereUnwind(long handle);

    /**
     * 定点数据调节
     *
     * @param handle
     * @param mode
     * @param vect
     * @param texture
     * @param type
     * @param index
     */
    public native void TransformVertex(long handle, int mode, float[] vect, boolean texture, int type, int index);

    /**
     * 开启音频功能
     *
     * @param handle
     * @param enable
     */
    public native void EnableAudio(long handle, boolean enable);

    /**
     * 打开音频播放
     *
     * @param handle
     */
    public native void OpenAudioPlaying(long handle);

    /**
     * 暂停音频的播放
     *
     * @param handle
     */
    public native void PauseAudioPlaying(long handle);

    /**
     * 硬件加速渲染的buffer接口
     *
     * @param handle
     * @param conn
     * @param width
     * @param height
     * @param directBuffer
     * @param index
     */
    private native void DoDirectTextureFrameUpdata(long handle, long conn, int width, int height, long directBuffer, int index);

    private native void DoDirectTextureOSDFrameUpdata(long handle, long conn, int width, int height, long textBuffer, int index);

    /**
     * 播放音频的数据
     *
     * @param handle
     * @param audioDatam
     * @param length
     */
    private native void PlayAudioData(long handle, byte[] audioDatam, int length);

    /**
     * 开启描线
     *
     * @param handle
     * @param value
     */
    public native void EnableGrid(long handle, boolean value);

    /**
     * 显示加载logo
     *
     * @param handle
     * @param index
     */
    private native void ShowVideoLoading(long handle, int index);

    /**
     * 隐藏加载logo
     *
     * @param handle
     * @param index
     */
    private native void HideVideoLoading(long handle, int index);

    /**
     * 开始移动跟踪
     *
     * @param handle
     * @param index
     */
    public native void StartMotionTracking(long handle, int index);

    /**
     * 停止移动跟踪
     *
     * @param handle
     * @param index
     */
    public native void StopMotionTracking(long handle, int index);

    /**
     * 手势的惯性调整接口
     *
     * @param handle
     * @param type
     * @param actionType
     * @param value
     */
    public native void AdjustActionExperience(long handle, int type, int actionType, float value);

    /**
     * 设置普通模式下设置边框颜色
     *
     * @param handle
     * @param value
     */
    private native void SetBorderColor(long handle, int value);

    public native void SetViewAngle(long handle, float value);

    public native boolean GetDualMode(long handle);

    public native int GetDualTexture(long handle);

    public native void CleanTexture(long handle);

    public native void SetHardwareTexture(long handle, int texture, int w, int h, long ctx);

    public native boolean GetHaveCropParmeter(long ctx);

    public native int[] GetCropParameter(long ctx, int w, int h);

    public native int[] SetCropInfo(long handle, long conn, int w, int h);

    public native void SetFBOTexture(long handle, int texid);

    public native void RenderFBO(long handle);

    public native boolean GetIsHEVC(long conn);

    private native void SetFishEyeParameters(long handle, float centerX, float centerY, float radius, float angleX, float angleY, float angleZ, int index);

    private native void SetDistortion(long handle, float[] distortion, float[] angle, int dist_len);

    /**
     * 设置普通显示模式是否保持宽高比
     *
     * @param handle
     * @param aspect 设为1.0填充满窗口，设置为 宽/高 为保持宽高比。
     * @param index
     */

    public native void SetKeepAspect(long handle, float aspect, int index);


    public void KeepAspect(boolean enable, int index) {
        if (index < 0 || index > 35)
            return;
        mEnableKeepAspect[index] = enable;
    }

    public void SetFishEyeParameters(float centerX, float centerY, float radius, float angleX, float angleY, float angleZ, int index) {
        SetFishEyeParameters(mParametricManager, centerX, centerY, radius, angleX, angleY, angleZ, index);
    }

    public void SetDistortion(float[] distortion, float[] angle, int dist_len) {
        SetDistortion(mParametricManager, distortion, angle, dist_len);
    }


    public native void CloseInfo(long handle);

    private final Context mContext;

    public void SetBorderColor(final int color) {
        mView.queueEvent(new Runnable() {
            @Override
            public void run() {
                SetBorderColor(mParametricManager, color);
            }
        });
    }


    /**
     * 動畫結束callback函數
     *
     * @param msgid
     */
    public void OnAnimationEnd(int msgid) {
        if (mEvent != null)
            mEvent.AnimationEndEvent(msgid);
    }

    /**
     * 顯示菊花loading的動畫
     *
     * @param index
     */

    private int mRcvFrameCount = 0;

    public void ShowVideoLoading(final int index) {
        if (isDestory.get()) {
            return;
        }
        mView.queueEvent(new Runnable() {
            @Override
            public void run() {
                ShowVideoLoading(mParametricManager, index);
                mShowLoading[index] = true;
                mRcvFrameCount = 0;
            }
        });
    }

    public void LoadFileToPlay(final String fn) {
        mView.queueEvent(new Runnable() {
            @Override
            public void run() {
                Bitmap bmp = BitmapFactory.decodeFile(fn);
                if (bmp != null) {
                    mFileWidth = bmp.getWidth();
                    mFileHeight = bmp.getHeight();
                    mFileTexture = GlUtil.loadTextureFromBitmap(bmp);
                }
            }
        });
    }

    public void CloseFilePlay() {
        mView.queueEvent(new Runnable() {
            @Override
            public void run() {
                if (mFileTexture != 0) {
                    int[] tex = new int[1];
                    tex[0] = mFileTexture;
                    GLES20.glDeleteTextures(1, tex, 0);
                    mFileTexture = 0;
                    mFileWidth = 0;
                    mFileHeight = 0;
                }
            }
        });
    }

    /**
     * 關閉菊花loading的動畫
     *
     * @param index
     */
    public void HideVideoLoading(final int index) {
        if (isDestory.get()) {
            return;
        }
        if (!mShowLoading[index])
            return;
        mView.queueEvent(new Runnable() {
            @Override
            public void run() {
                HideVideoLoading(mParametricManager, index);
                mShowLoading[index] = false;

//                ReSizeSplite(mParametricManager,mAspect,mWidth,mHeight);
//                mView.setVisibility(mView.VISIBLE);
//                mView.setZOrderOnTop(true);

            }
        });

    }

    /**
     * 视频有效贴图控件使用
     *
     * @param w
     * @param h
     * @param texture
     * @param frame_len
     * @param frame_type
     * @param timestamp
     * @param index
     */
    public void DoTextureAvaible(final int w, final int h, final byte[] texture, final int frame_len, final int frame_type, final int timestamp, final int index, final long ctx) {
        if (isDestory.get() || GLVideoDisplay.isDestory) {
            return;
        }
        runend[index] = false;
        if (mRcvFrameCount > 15) {
            HideVideoLoading(index);
        } else {
            mRcvFrameCount++;
        }
        if (mView.getQueueCount() > 10) {
            mInputTexture = 1;
        }
        if (mView.getQueueCount() == 0 && mInputTexture > 0) {
            mInputTexture++;
        }
        if (mInputTexture > 10) {
            mInputTexture = 0;
        }
        if (mInputTexture > 0)
            return;
        if ((!mHardwareDecoder) || (!hwdecoder.IsInitDecoder))
            mView.queueEvent(new Runnable() {
                @Override
                public void run() {
                    //Log.d("GLVideoRender.java","Draw Video......DoTextureAvaible");
                    CloseInfo(mParametricManager);
                    if (((w > mMaxSupportWidth || h > mMaxSupportHeight) && index == 0) || GetIsHEVC(ctx))
                        mHardwareDecoder = false;

                    Log.i(TAG, "mHardwareDecoder-------->" + mHardwareDecoder);
                    if (!mHardwareDecoder || index > 0) {
//                        SwitchDecoder(mParametricManager, false);
                        mView.mRender.TextureAvaible(mParametricManager, w, h, texture, frame_len, frame_type, timestamp, index, ctx);
                        if (!comed[index] /*&& runend[index]*/ && mOnVideoTextureComeListener != null) {
                            comed[index] = true;
                            mOnVideoTextureComeListener.OnVideoTextureCome(index);
                        }
                    } else {
                        conn = ctx;
                        if (!hwdecoder.IsInitDecoder) {
                            try {
                                Log.i("testinfo", "-------------------first frame come-->" + comed[index] + "----" + (mOnVideoTextureComeListener != null));
                                //SwitchDecoder(mParametricManager, true);
                                hwdecoder.InitDecoder(w, h, texture);
                                if (!hwdecoder.IsInitDecoder) {
                                    mHardwareDecoder = false;
                                    GLVideoConnect.getInstance().SetHardwareDecoder(mView.msg, mHardwareDecoder, index, mMaxSupportWidth, mMaxSupportHeight);
                                }
                            } catch (IOException e) {
                                e.printStackTrace();
                            } catch (IllegalArgumentException e) {
                                e.printStackTrace();
                            }
                        } else {
                            //SwitchDecoder(mParametricManager, true);
                            if (hwdecoder.mWidth != w && hwdecoder.mHeight != h) {
                                hwdecoder.Release();
                                hwdecoder.InitDecoderSurface();
                                try {
                                    hwdecoder.InitDecoder(w, h, texture);
                                } catch (IOException e) {
                                    e.printStackTrace();
                                } catch (IllegalArgumentException e) {
                                    e.printStackTrace();
                                }
                            }
                        }
                        if (!comed[index] /*&& runend[index]*/ && mOnVideoTextureComeListener != null) {
                            Log.i("testinfo", "-------------------first frame come");
                            comed[index] = true;
                            mOnVideoTextureComeListener.OnVideoTextureCome(index);
                        }
                    }
                    runend[index] = true;
                }
            });
        int count = 0;
        if ((!mHardwareDecoder) || (!hwdecoder.IsInitDecoder))
            while (!runend[index])
                try {
                    Thread.sleep(1);
                    count++;
                    if (count > 1000)
                        break;
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
        if (mEnableKeepAspect[index]) {
            if (w > 0 && h > 0 && count < 1000)
                SetKeepAspect(mParametricManager, (float) w / (float) h, index);
        } else
            SetKeepAspect(mParametricManager, 1.0f, index);
        if (mHardwareDecoder && hwdecoder.IsInitDecoder && count < 1000) {
            hwdecoder.InputCompressBuffer(texture, frame_len);
        }
    }

    public GLVideoRender(final Context context, final GLVideoDisplay view) {
        mContext = context;
        mView = view;
        for (int i = 0; i < comed.length; i++) {
            comed[i] = false;
        }
        mIdentityMatrix = new float[16];
        Matrix.setIdentityM(mIdentityMatrix, 0);
    }

    public Context getmContext() {
        return mContext;
    }

    public void enableAudio(boolean enabled) {
        hwplayer.EnabledAudio(enabled);
    }


    public void PlayFile(String fn) {
        hwplayer.PlayFile(fn);
    }

    public void StopPlay() {
        if (hwplayer.mIsPlaying)
            hwplayer.Stop();
    }

    public void PausePlay() {
        if (hwplayer.mIsPlaying)
            hwplayer.Pause();
    }

    public void ContinuePlay() {
        if (hwplayer.mIsPlaying)
            hwplayer.Continue();
    }

    public long GetPlayFileTime() {
        if (hwplayer != null)
            return hwplayer.mFileDurationTime;
        return 0;
    }

    public void SetPresentTimeCallback(HWPlayer.PresentTimeCallback pts) {
        mCallback = pts;
    }

    public boolean SeekToTime(long ms) {
        if (hwplayer != null) {
            return hwplayer.SeekToTime(ms);
        }
        return false;
    }

    @Override
    public void onSurfaceCreated(GL10 unused, javax.microedition.khronos.egl.EGLConfig config) {
        Log.d("JAVideo", ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,");
        isDestoryed.set(false);
        hwdecoder = new HWDecoder(mView);
        Log.d("honglee_0808", "create.......................");
        hwdecoder.setCaptureImageListener(this);
        mHardwareDecoder = hwdecoder.isSupportHardWare;
        if (mHardwareDecoder)
            hwdecoder.InitDecoderSurface();
        hwencoder = new HWMP4Encoder(mView);
        hwplayer = new HWPlayer(mView);
        hwplayer.SetPresetnAudioCallback(this);
        if (mCallback != null)
            hwplayer.SetPresentTimeCallback(mCallback);
        mFullScreen = new FullFrameRect(
                new Texture2dProgram(Texture2dProgram.ProgramType.TEXTURE_2D));
    }

    @Override
    public void onDrawFrame(GL10 unused) {
        //Log.d("testplaying", "onDrawFrame runing.....");
        if (hwplayer.mIsPlaying) {
            synchronized (this) {
                if (hwplayer.GetUpdateSurface()) {
                    hwplayer.UpdateSurfaceImage();
                }
            }
            hwplayer.Draw(false);
        }
//        Log.i(TAG, "onDrawFrame: ------>" + mFileTexture + "-----" + hwdecoder.IsInitDecoder + "----" + hwplayer.mIsPlaying);
        if (hwdecoder.IsInitDecoder) {
            synchronized (this) {
                if (hwdecoder.IsInitDecoder)
                    if (hwdecoder.GetUpdateSurface()) {
                        hwdecoder.UpdateSurfaceImage();
                        //Log.v("honglee_old","DoDrawframe..........................................................");
                        //HideVideoLoading(mParametricManager, 0);
                    }
            }

            if (!GetHaveCropParmeter(conn) && hwdecoder.mHaveImage && mScreenMode != Parameter.SCRN_NORMAL) {
                hwdecoder.Draw(true);
                //SetCropInfo(mParametricManager,conn,hwdecoder.mWidth,hwdecoder.mHeight,hwdecoder.mCenterX,hwdecoder.mCenterY,hwdecoder.mRadius);
            } else {
                if (GetHaveCropParmeter(conn) && hwdecoder.mCropInfo == null && mScreenMode != Parameter.SCRN_NORMAL)
                    hwdecoder.mCropInfo = GetCropParameter(conn, hwdecoder.mWidth, hwdecoder.mHeight);
                hwdecoder.Draw(false);
            }
        }
        if (isDestoryed.get()) {
            return;
        }
        if (isDestory.get() && !isDestoryed.get()) {
            isDestoryed.set(DestroyManager(mParametricManager));
            mParametricManager = 0;
            if (mDestoryListener != null)
                mDestoryListener.OnDestoryListener();
            if (hwdecoder.IsInitDecoder)
                hwdecoder.Release();
            if (hwencoder.mRecordingEnabled)
                hwencoder.StopRecord();
            if (hwplayer.mIsPlaying)
                hwplayer.Stop();
            if (mFileTexture != 0)
                CloseFilePlay();
            return;
        }
        if (isRotateScreen) {
            return;
        }
        if (isPauseDraw) {
            return;
        }
        nowTime = SystemClock.uptimeMillis();
        if (recordStatuTexId_1 == 0) {
            String packName = mContext.getPackageName();
            Resources rec = mContext.getResources();
//            recordStatuTexId_1 = ToolsUtil.loadTexture(mContext, rec.getIdentifier("rec_1", "drawable", packName));
//            recordStatuTexId_2 = ToolsUtil.loadTexture(mContext, rec.getIdentifier("rec_2", "drawable", packName));
//            LoadRecordStatuTexture(mParametricManager, recordStatuTexId_1);
        }
        if (lastTime == 0) {
            lastTime = nowTime;
        } else {
            if (nowTime - lastTime > 200) {
                LoadRecordStatuTexture(mParametricManager, recordStatuTexId_2);
            }
        }

//        Log.i(TAG, "onDrawFrame: ---------->" + mParametricManager + "--" + mFileTexture + "--"
//                + mFileWidth + "--" + mFileHeight + "--" + conn + "---isPlaying:" + hwplayer.mIsPlaying + "---mHardwareDecoder:" + mHardwareDecoder);
        if (mFileTexture != 0) {
            SetHardwareTexture(mParametricManager, mFileTexture, mFileWidth, mFileHeight, conn);
        }
        if (hwplayer.mIsPlaying && mFileTexture == 0) {
            SetHardwareTexture(mParametricManager, hwplayer.mOffscreenTexture, hwplayer.mWidth, hwplayer.mHeight, conn);
        }

        if (mHardwareDecoder && hwdecoder.IsInitDecoder && !hwplayer.mIsPlaying && mFileTexture == 0) {
            SetHardwareTexture(mParametricManager, hwdecoder.mOffscreenTexture, hwdecoder.mWidth, hwdecoder.mHeight, conn);
//            if((mScreenMode == Parameter.SCRN_CYLINDER || mScreenMode == Parameter.SCRN_EXPAND || mScreenMode==Parameter.SCRN_UPDOWN||hwencoder.mRecordingEnabled)&&GetDualMode(mParametricManager))
//                hwdecoder.DrawFBO();
        }

        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
        GLES20.glEnable(GLES20.GL_BLEND);
        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);
        GLES20.glViewport(0, 0, mWidth, mHeight);
        DrawParametric(mParametricManager);
        /**
         * -------监控模式下的截图---start----
         */
        if (isCaptureImageByMon) {
            captureImageByMonitor();
        }

        /**
         * -------监控模式下的截图---end----
         */
//        GLES20.glViewport(0, 0, mWidth, mHeight);
//        mFullScreen.drawFrame(hwdecoder.mOffscreenTexture,mIdentityMatrix);
//        Log.d(TAG, "onDrawFrame: ------------------------>录屏: " + mRecordScreen + "----mHardwareDecoder:" + mHardwareDecoder + "---mRecordingEnabled: " + hwencoder.mRecordingEnabled);
//        if (mRecordScreen && mHardwareDecoder) {
//            if (!hwencoder.mRecordingEnabled) {
////                hwencoder.StartRecord(mWidth,mHeight,mRecordFileName,mView.GetEglCore());
////                hwdecoder.DrawFBO();
//            }
//            //hwencoder.WriteFrame(GetDualTexture(mParametricManager));
//            hwencoder.WriteFrame(hwdecoder.mOffscreenTexture);
//        } else {
//            if (hwencoder.mRecordingEnabled)
//                hwencoder.StopRecord();
//        }
        if (mRecordScreen) {
            if (!hwencoder.mRecordingEnabled) {
                Log.d("hqh2", "onDrawFrame: StartRecord");
                //                if (mHardwareDecoder && hwdecoder.IsInitDecoder && !hwplayer.mIsPlaying && mFileTexture == 0)
                int width = mWidth;
                int height = mHeight;
                if (width > height) {
                    width = width - 1;
                }
                hwencoder.StartRecord(width, height, mRecordFileName, new EglCore(EGL14.eglGetCurrentContext(), EglCore.FLAG_RECORDABLE | EglCore.FLAG_TRY_GLES3));
                //                if (hwplayer.mIsPlaying && mFileTexture == 0)
                //                    hwencoder.StartRecord(hwplayer.mWidth,hwplayer.mHeight,mRecordFileName,new EglCore(EGL14.eglGetCurrentContext(),EglCore.FLAG_RECORDABLE | EglCore.FLAG_TRY_GLES3));
            }

            if (mFileTexture == 0) {
                hwencoder.WriteFrame(hwplayer.mOffscreenTexture);
            }

//            if (hwplayer.mIsPlaying && mFileTexture == 0)
//                hwencoder.WriteFrame(hwplayer.mOffscreenTexture);
//            if (mHardwareDecoder && hwdecoder.IsInitDecoder && !hwplayer.mIsPlaying && mFileTexture == 0)
//                hwencoder.WriteFrame(hwdecoder.mOffscreenTexture);
        } else {
            if (hwencoder.mRecordingEnabled) {
                Log.d("hqh2", "onDrawFrame: StopRecord");
                hwencoder.StopRecord();
            }
        }
    }


    private boolean isCaptureImageByMon = false;
    private String mCaptureFileName = null;

    /**
     * @param status
     * @param fileName
     */
    public void setCaptureImgByMonitor(boolean status, String fileName) {
        this.isCaptureImageByMon = status;
        this.mCaptureFileName = fileName;
    }


    /**
     * 尽可能在主线程控制。。。。。{@link #onDrawFrame(GL10)}
     * 监控模式下的截图{@link #isCaptureImageByMon}只有一次，最终（finally）设置为false
     */
    public void captureImageByMonitor() {
        Log.d(TAG, "captureImageByMonitor: ----->截屏中" + mCaptureFileName);
        if (TextUtils.isEmpty(mCaptureFileName)) {
            return;
        }
        GLES20.glFlush();
        ByteBuffer buf = ByteBuffer.allocateDirect(mWidth * mHeight * 4);
        buf.order(ByteOrder.BIG_ENDIAN);
        GLES20.glReadPixels(0, 0, mWidth, mHeight,
                GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, buf);
        GlUtil.checkGlError("glReadPixels");
        buf.rewind();

        BufferedOutputStream bos = null;
        try {


            //图片大小
            int bitmapSize = mWidth * mHeight * 4;

            Bitmap bmp = null;
            if (ImageUtil.hasEnoughMemoryToCapture(bitmapSize)) {
                bmp = Bitmap.createBitmap(mWidth, mHeight, Bitmap.Config.ARGB_8888);
                bmp.copyPixelsFromBuffer(buf);
            }

            if (bmp != null && ImageUtil.hasEnoughMemoryToCapture(bitmapSize)) {
                try {
                    bos = new BufferedOutputStream(new FileOutputStream(mCaptureFileName));
                } catch (IOException e) {
                    e.printStackTrace();
                    Log.d(TAG, "captureImageByMonitor: ---->" + e.toString());
                }

                android.graphics.Matrix matrix = new android.graphics.Matrix();
                matrix.postScale(1, -1);

                Bitmap bmp2 = Bitmap.createBitmap(bmp, 0, 0, bmp.getWidth(), bmp.getHeight(), matrix, true);
                bmp2.compress(Bitmap.CompressFormat.JPEG, 90, bos);

                bmp2.recycle();
                bmp.recycle();
            }

        } finally {
            if (bos != null) {
                try {
                    bos.close();
                    isCaptureImageByMon = false;
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        if (mCaptureImageListener != null) {
            File tempFile = new File(mCaptureFileName);
            if (tempFile.exists()) {
                mCaptureImageListener.OnCaptureImage(1, -1);
            } else {
                mCaptureImageListener.OnCaptureImage(-1, -1);
            }

        }

    }


    public void CaptureImage(String Filename) {
        if (mHardwareDecoder)
            if (hwdecoder.IsInitDecoder)
                hwdecoder.CaptureImage(Filename);
    }

    public void startRecordScreen(String Filename) {
        mRecordFileName = Filename;
        mRecordScreen = true;
    }

    public void stopRecordScreen() {
        mRecordScreen = false;
    }

    public void DirectDestroy() {
        Log.d("honglee_0808", "destroy...............");
        isDestoryed.set(DestroyManager(mParametricManager));
        mParametricManager = 0;

        mSensorList.clear();
        mSensorList = null;
        //        if (mDestoryListener != null)
        //            mDestoryListener.OnDestoryListener();
        if (hwdecoder != null) {
            if (hwdecoder.IsInitDecoder)
                hwdecoder.Release();
            if (hwencoder.mRecordingEnabled)
                hwencoder.StopRecord();
            if (hwplayer.mIsPlaying)
                hwplayer.Stop();
            if (mFileTexture != 0)
                CloseFilePlay();
        }

        // out of memory
        mAudioDataListener = null;
        mEvent = null;
    }

    public int mOnSurfaceChangedCount = 0;

    @Override
    public void onSurfaceChanged(GL10 unused, int width, int height) {
        GLES20.glViewport(0, 0, width, height);
        isRotateScreen = false;

        Log.d(TAG, "onSurfaceChanged: ------->width:" + width + "----" + height);
        if (height > width) {
            height = width;
        }

        mWidth = width;
        mHeight = height;
        mAspect = (float) width / (float) height;
        aspect = mAspect;

        Log.i(TAG, "onSurfaceChanged: 2017-12-09------------->" + mParametricManager + "---" + mWidth + "---" + mHeight + " mAspect:" + mAspect);


//        mHeight = 1080;


        if (mParametricManager != 0) {


            ReSizeSplite(mParametricManager, mAspect, mWidth, mHeight);
            UpdateAspect(mParametricManager, mAspect);


//            if (mOnSurfaceChangedCount == 1 && !mHardwareDecoder && android.os.Build.BRAND.equals("HUAWEI")) {
//                Log.i(TAG, "onSurfaceChanged: ----->重新创建一次");
//                updateSurfaceChanged();
//            }
//            mOnSurfaceChangedCount++;

//            if (mOnSurfaceChangedCount == 1 && android.os.Build.BRAND.equals("HUAWEI")) {
//                Log.i(TAG, "onSurfaceChanged: ----->重新创建一次");
//                ReSizeSplite(mParametricManager, mAspect, mWidth, mHeight);
//                UpdateAspect(mParametricManager, mAspect);
//            }

        } else {
            String path = getDataFileName();
            mParametricManager = InitManager(mAspect, mWidth, mHeight, path);

            ReSizeSplite(mParametricManager, mAspect, mWidth, mHeight);
            UpdateAspect(mParametricManager, mAspect);
        }

        if (mGLVideoSurfaceCreateListener != null) {
            mGLVideoSurfaceCreateListener.onGLVideoSurfaceCreate();
        }
        if (mGLDisplayCreateListener != null) {
            mGLDisplayCreateListener.onGLDisplayCreateCallBack(unused, mParametricManager, width, height);
        }


        Log.v("honglee", "........................onSurfaceChanged...................");


    }

    public void updateSurfaceChanged() {
        ReSizeSplite(mParametricManager, mAspect, mWidth, mHeight);
        UpdateAspect(mParametricManager, mAspect);

    }

    private OnGLDisplayCreateListener mGLDisplayCreateListener;

    public void setGLDisplayCreateListener(OnGLDisplayCreateListener listener) {
        mGLDisplayCreateListener = listener;
    }


    /**
     * 转软解
     */
    public void disableHardWareDecoder() {
        mHardwareDecoder = false;
        GLVideoConnect.getInstance().SetHardwareDecoder(mView.msg, mHardwareDecoder, 0, mMaxSupportWidth, mMaxSupportHeight);
    }

    public void enableHarWareDecoder(boolean enable, int index) {
        CleanTexture(mParametricManager);

        mHardwareDecoder = enable;
        Log.d(TAG, "enableHarWareDecoder: ----->mMaxSupportWidth：" + mMaxSupportWidth + "----" + "mMaxSupportHeight：" + mMaxSupportHeight);
        GLVideoConnect.getInstance().SetHardwareDecoder(mView.msg, mHardwareDecoder, index, mMaxSupportWidth, mMaxSupportHeight);
    }

    /**
     * 获取字体的路径
     *
     * @return
     */
    private String getDataFileName() {
        String path = mContext.getApplicationContext().getExternalFilesDir(null).getPath();
        return path;
    }

    /**
     * 判断中英文
     *
     * @return
     */
    public boolean isZh() {
        Locale locale = mContext.getResources().getConfiguration().locale;
        String language = locale.getLanguage();
        if (language.endsWith("zh"))
            return true;
        else
            return false;
    }

    /**
     * 设置销毁标志
     */
    public void destory() {
        Log.i("Lee", "GLVideoRender.destory");
        isDestory.set(true);
    }

    public void OnPlayfileProgress(int time, int duration) {
        if (mProgress != null)
            mProgress.OnProgress(time, duration);
    }

    public AudioDataListener mAudioDataListener;
    public FrameDataComeListener mFrameDataComeListener;
    public PlaybackUpdateTimeListener mPlaybackUpdateTimeListener;
    public SearchRecDataListener mSearchRecDataListener;
    public VconDataListener mVconDataListener;
    public SearchDeviceListener mSearchDeviceListener;
    public ConnectStatusListener mConnectStatusListener;
    public CaptureImageListener mCaptureImageListener;
    public RecordVideoListener mRecordVideoListener;
    public GLVideoSurfaceCreateListener mGLVideoSurfaceCreateListener;
    public OSDTimeListener mOSDTimeListener;
    public OnVideoTextureComeListener mOnVideoTextureComeListener;
    public DestoryListener mDestoryListener;
    public PlayfileProgress mProgress;

    public void setConnectStateListener(ConnectStatusListener listener) {
        this.mConnectStatusListener = listener;
    }


    //-------------------------------------分割线---------------------------------------------------

    public void setCaptureImageListener(CaptureImageListener mCaptureImageListener) {
        this.mCaptureImageListener = mCaptureImageListener;
    }


    public void OnGSensorData(long timeStamp, double x, double y, double z) {
        Log.d("gsensor", "gsensor --timeStamp:" + timeStamp + ",x:" + x + ",y:" + y + ",z:" + z);
        if (mSensorList != null) {
            mSensorList.add(new SensorInfo(timeStamp, x, y, z));
        }

        //        if (z < 40.0 && z > -40.0) {
        //            if (GLVideoConnect.getInstance().GetWallModelType(this.mView.msg, 0) == 1) {
        //                ResetPosition(mParametricManager, false, 0);
        //                GLVideoConnect.getInstance().IsForceWallMode(false);
        //            }
        //        } else {
        //            if (GLVideoConnect.getInstance().GetWallModelType(this.mView.msg, 0) == 0) {
        //                GLVideoConnect.getInstance().IsForceWallMode(true);
        //                ResetPosition(mParametricManager, false, 0);
        //            }
        //            float[] rotate = GetObjectPosition(mParametricManager, 2, false, 0);
        //            if (x > 0)
        //                rotate[2] = (float) y;
        //            else if (y > 0)
        //                rotate[2] = (float) (90.0f - y) + 90.0f;
        //            else
        //                rotate[2] = (float) -y - 180.0f;
        //            TransformObject(mParametricManager, rotate, 2, false, 0);
        //        }
    }

    /**
     * 使用硬件渲染加速后，的视频贴图buffer
     *
     * @param width
     * @param height
     * @param directBuffer
     * @param index
     */
    @Override
    public void OnDirectTextureFrameUpdata(final int width, final int height, final long directBuffer, final int index) {
        //HideVideoLoading(index);
        runend[index] = false;
        mView.queueEvent(new Runnable() {
            @Override
            public void run() {
                DoDirectTextureFrameUpdata(mParametricManager, GLVideoConnect.getInstance().getpGLVideoConnect(mView.msg),
                        width, height, directBuffer, index);
                runend[index] = true;
                if (!comed[index] && runend[index] && mOnVideoTextureComeListener != null) {
                    comed[index] = true;
                    mOnVideoTextureComeListener.OnVideoTextureCome(index);
                    Log.i("Lee", "[jasonchan] OnFistFrame---> " + (float) width / (float) height);
                }
            }
        });
        while (!runend[index])
            try {
                Thread.sleep(0);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
    }


    public void setConnectCtxs(long[] ctxs) {
        mConnectCtxs = ctxs;
        if (ctxs == null) {
            return;
        }
        for (int i = 0; i < ctxs.length; i++) {
            Log.d(TAG, "setConnectCtxs: clean index = " + i + ", ctx = " + ctxs[i] + ", this = " + this);
        }
    }


    /**
     * 正常贴图方式
     *
     * @param width
     * @param height
     * @param frame
     * @param length
     * @param frame_type
     * @param timestamp
     * @param index
     */
    @Override
    public void OnTextureAvaible(int width, int height, byte[] frame, int length, int frame_type, int timestamp, int index, long ctx) {
        //if (!mHardwareDecoder)
        //HideVideoLoading(index);
        if (mConnectCtxs != null) {
            Log.d(TAG, "OnTextureAvaible: clean index = " + index + ", ctx = " + ctx + ", ctxs = " + mConnectCtxs[index]);
            if (mConnectCtxs[index] != ctx) {
                //防止串屏
                return;
            }
        }

        DoTextureAvaible(width, height, frame, length, frame_type, timestamp, index, ctx);
    }

    /**
     * OSD有效贴图回掉
     *
     * @param width
     * @param height
     * @param texture
     * @param length
     */
    @Override
    public int OnOSDTextureAvaible(final int width, final int height, final long texture, final int length, long utctime) {
        int ret = -1;
        if (isDestory.get()) {
            return ret;
        }

        if (mSensorList != null && mSensorList.size() > 0) {
            SensorInfo sensorInfo = mSensorList.get(0);
            Log.d(TAG, "mSensorList.size() = " + mSensorList.size() + ", utctime = " + utctime + ", sensorInfo.time = " + sensorInfo.getTimeStamp());
            if (sensorInfo.getTimeStamp() == 0) {
                mSensorList.clear();
                return ((sensorInfo.getZ() < 40.0 && sensorInfo.getZ() > -40.0) ? 0 : 1);
            }
            if (Math.abs(utctime - sensorInfo.getTimeStamp()) > 60000) {
                sensorInfo = null;
                mSensorList.clear();
            }

            if (sensorInfo != null && utctime >= sensorInfo.getTimeStamp()) {
                double x = sensorInfo.getX();
                double y = sensorInfo.getY();
                double z = sensorInfo.getZ();

                mSensorList.remove(0);
                //Log.d("gsensor", "OnTextureAvaible x:" + x + ",y:" + y + ",z:" + z + ", utctime = " + utctime + ", sensorInfo.time = " + sensorInfo.getTimeStamp());
                if (z < 40.0 && z > -40.0) {
                    if (GLVideoConnect.getInstance().GetWallModelType(this.mView.msg, 0) == 1) {
                        ResetPosition(mParametricManager, false, 0);
                        GLVideoConnect.getInstance().IsForceWallMode(false);
                        ret = 0;
                    }
                } else {
                    if (GLVideoConnect.getInstance().GetWallModelType(this.mView.msg, 0) == 0) {
                        GLVideoConnect.getInstance().IsForceWallMode(true);
                        ResetPosition(mParametricManager, false, 0);
                        ret = 1;
                    }

                    int mode = GetMode(mParametricManager);
                    //只有鱼眼模式才旋转
                    if (mode == JAParameter.SCRN_HEMISPHERE) {
                        float[] rotate = GetObjectPosition(mParametricManager, 2, false, 0);
                        float beforeRotate = rotate[2];
                        if (x > 0)
                            rotate[2] = (float) y;
                        else if (y > 0)
                            rotate[2] = (float) (90.0f - y) + 90.0f;
                        else
                            rotate[2] = (float) -y - 180.0f;
                        //TransformObject(mParametricManager, rotate, 2, false, 0);

                        float delta = Math.abs(beforeRotate - rotate[2]);
                        if (delta > 1f) {
                            //Log.d(TAG, "OnOSDTextureAvaible: StartAnimation adjust");
                            StartAnimation(mParametricManager, rotate, 100, 1000, false, 2, false, 0, false, 33);
                        }
                    }
                }
            }

        }


        runendOSD[0] = false;
        if (mScreenMode == Parameter.SCRN_NORMAL)
            return ret;
        mView.queueEvent(new Runnable() {
            @Override
            public void run() {
                mView.mRender.OSDTextureAvaible(mParametricManager, texture, length, width, height);
                runendOSD[0] = true;
            }
        });
        int count = 0;
        while (!runendOSD[0]) {
            try {
                Thread.sleep(1);
                count++;
                if (count > 1000)
                    break;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        return ret;
    }


    public boolean mIsConnectStateTip = true;

    public void setConnectStateTip(boolean state) {
        mIsConnectStateTip = state;
    }


    /**
     * 链接状态回掉
     *
     * @param handle
     * @param status_code
     * @param content_code
     * @param index
     * @param msg
     */
    @Override
    public void OnStatus(final long handle, final int status_code, final int content_code, final int index, String msg) {
        Log.d("zasko", "OnStatus: ---------->" + status_code + "---" + content_code + "---" + msg);
        if (isDestory.get()) {
            return;
        }
//        if (!msg.equals(mView.msg)) {
//            return;
//        }
        if (mConnectStatusListener != null) {
            mConnectStatusListener.onStatus(handle, status_code, content_code, index, msg);
            if (GetMode(mParametricManager) != 0) {
                return;
            }
        }
        if (GetSplitMode(mParametricManager) > 2) {
            return;
        }


//        mView.queueEvent(new Runnable() {
//            @Override
//            public void run() {
//                String status = "";
//                switch (status_code) {
//                    case Parameter.NET_STATUS_CONNECTING:
//                        status = LanguageUtil.isZh(mContext) == true ? "正在链接..." : "connecting...";
//                        break;
//                    case Parameter.NET_STATUS_CONNECTED:
//                        break;
//                    case Parameter.NET_STATUS_CONNECTFAIL:
//                        status = LanguageUtil.isZh(mContext) == true ? "链接失败..." : "connect fail...";
//                        HideVideoLoading(mParametricManager, index);
//                        break;
//                    case Parameter.NET_STATUS_LOGINING:
//                        status = LanguageUtil.isZh(mContext) == true ? "链接成功..." : "logining...";
//                        break;
//                    case Parameter.NET_STATUS_LOGINED:
//                        status = LanguageUtil.isZh(mContext) == true ? "正在登陆..." : "logining...";
//                        break;
//                    case 10:
//                        status = LanguageUtil.isZh(mContext) == true ? "密码错误..." : "pwd error...";
//                        HideVideoLoading(mParametricManager, index);
//                        break;
//                    case Parameter.NET_STATUS_OPENING:
//                        status = LanguageUtil.isZh(mContext) == true ? "正在加载..." : "loading...";
//                        break;
//                    case Parameter.NET_STATUS_DISCONNECTED:
//                        status = LanguageUtil.isZh(mContext) == true ? "设备已断开..." : "disconntcted...";
//                        HideVideoLoading(mParametricManager, index);
//                        break;
//                    case 11:
//                        status = LanguageUtil.isZh(mContext) == true ? "设备超时..." : "timeout...";
//                        HideVideoLoading(mParametricManager, index);
//                        break;
//                }
//                Log.d("testinfo", "----------------->" + status);
//                if (status != null) {
//                    mView.mRender.DoStatus(mParametricManager, status, content_code, index);
//                }
//
//            }
//        });
    }

    public void showChannelConnectTip(final int state, final int contentCode, final int index) {
        mView.queueEvent(new Runnable() {
            @Override
            public void run() {
                String tip = "";
                switch (state) {
                    case Parameter.NET_STATUS_CONNECTING:
                        tip = LanguageUtil.isZh(mContext) == true ? "正在链接..." : "connecting...";
                        break;
                    case Parameter.NET_STATUS_CONNECTED:
                        break;
                    case Parameter.NET_STATUS_CONNECTFAIL:
                        tip = LanguageUtil.isZh(mContext) == true ? "链接失败..." : "connect fail...";
                        HideVideoLoading(mParametricManager, index);
                        break;
                    case Parameter.NET_STATUS_LOGINING:
                        tip = LanguageUtil.isZh(mContext) == true ? "链接成功..." : "logining...";
                        break;
                    case Parameter.NET_STATUS_LOGINED:
                        tip = LanguageUtil.isZh(mContext) == true ? "正在登陆..." : "logining...";
                        break;
                    case 10:
                        tip = LanguageUtil.isZh(mContext) == true ? "密码错误..." : "pwd error...";
                        HideVideoLoading(mParametricManager, index);
                        break;
                    case Parameter.NET_STATUS_OPENING:
                        tip = LanguageUtil.isZh(mContext) == true ? "正在加载..." : "loading...";
                        break;
                    case Parameter.NET_STATUS_DISCONNECTED:
                        tip = LanguageUtil.isZh(mContext) == true ? "设备已断开..." : "disconntcted...";
                        HideVideoLoading(mParametricManager, index);
                        break;
                    case 11:
                        tip = LanguageUtil.isZh(mContext) == true ? "设备超时..." : "timeout...";
                        HideVideoLoading(mParametricManager, index);
                        break;
                }
                if (!TextUtils.isEmpty(tip)) {
                    Log.d(TAG, "showChannelConnectTip----->" + mParametricManager + "----" + tip + "---" + state + "----" + index);

                    mView.mRender.DoStatus(mParametricManager, tip, contentCode, index);
                }
            }
        });
    }


    /**
     * 音频数据回调函数
     *
     * @param data
     */
    @Override
    public void OnAudioDataCome(byte[] data) {
        if (isDestory.get()) {
            return;
        }
        Log.d(TAG, "OnAudioDataCome: -------------->" + mParametricManager + "------" + data.length);
        PlayAudioData(mParametricManager, data, data.length);
        //TODO：设置声音
//        Log.d(TAG, "OnAudioData: ---------" + mRecordScreen + "---" + mView.isPlayAudio_ + "---" + hwencoder.hashCode());
        if (mRecordScreen /*&& mHardwareDecoder*/ && hwencoder != null) {
            Log.d(TAG, "OnAudioDataCome: --------");
            hwencoder.writeAudioData(data);
        }
    }

    @Override
    public void OnPlayedFirstFrame(int is180) {
        Log.i("Lee", "GLVideoRender.OnPlayedFirstFrame  is180 = " + is180);
        if (is180 == 0)
            SetViewAngle(mParametricManager, 90);
        else
            SetViewAngle(mParametricManager, 60);
    }


    /*

    截图成功回调
     */
    @Override
    public void OnCaptureImage(int success, int index) {
        if (mCaptureImageListener != null)
            mCaptureImageListener.OnCaptureImage(success, index);

    }

    @Override
    public void OnAudioData(byte[] data) {
        if (isDestory.get()) {
            return;
        }
        PlayAudioData(mParametricManager, data, data.length);
        //TODO：设置声音
        Log.d(TAG, "OnAudioData: ---------" + mRecordScreen + "---" + mView.isPlayAudio_);
        if (mRecordScreen /*&& mHardwareDecoder*/ && mView.isPlayAudio_) {
            Log.d(TAG, "OnAudioData: ------>" + hwencoder.hashCode());
            hwencoder.writeAudioData(data);
        }

    }
}
