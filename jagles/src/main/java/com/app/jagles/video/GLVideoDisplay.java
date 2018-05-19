package com.app.jagles.video;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.media.AudioFormat;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.SurfaceHolder;

import com.app.jagles.animation.Animation;
import com.app.jagles.audio.VideoAudioTrack;
import com.app.jagles.listener.AnimationEndListener;
import com.app.jagles.listener.AudioDataListener;
import com.app.jagles.listener.DestoryListener;
import com.app.jagles.listener.GLVideoSurfaceCreateListener;
import com.app.jagles.listener.GestureListener;
import com.app.jagles.listener.PlayfileProgress;
import com.app.jagles.listener.SensorListener;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicLong;

/**
 * Created by chen on 16/3/13.
 */
public class GLVideoDisplay extends GLSurfaceViewOrg implements AnimationEvent, AudioDataListener,
        SensorEventListener {

    private static final String TAG = "GLVideoDisplay";
    float SCREEN_SCALE_1[][] = {{2, 2, 1}};
    public String msg = "";
    int SPLIT_1[][] = {{0, 0, 12, 12}};
    int SPLIT_2[][] = {{0, 0, 6, 6}, {6, 0, 6, 6}, {0, 6, 6, 6}, {6, 6, 6, 6}};
    int SPLIT_3[][] = {{0, 0, 8, 8}, {8, 0, 4, 4}, {8, 4, 4, 4}, {0, 8, 4, 4}, {4, 8, 4, 4}, {8, 8, 4, 4}};
    int SPLIT_4[][] = {{0, 0, 9, 9}, {9, 0, 3, 3}, {9, 3, 3, 3}, {9, 6, 3, 3}, {0, 9, 3, 3}, {3, 9, 3, 3}, {6, 9, 3, 3}, {9, 9, 3, 3}};
    int SPLIT_5[][] = {{0, 0, 4, 4}, {4, 0, 4, 4}, {8, 0, 4, 4}, {0, 4, 4, 4}, {4, 4, 4, 4}, {8, 4, 4, 4}, {0, 8, 4, 4}, {4, 8, 4, 4}, {8, 8, 4, 4}};
    int SPLIT_6[][] = {{0, 0, 3, 3}, {3, 0, 3, 3}, {6, 0, 3, 3}, {9, 0, 3, 3}, {0, 3, 3, 3}, {3, 3, 6, 6}, {9, 3, 3, 3}, {0, 6, 3, 3}, {9, 6, 3, 3}, {0, 9, 3, 3}, {3, 9, 3, 3}, {6, 9, 3, 3}, {9, 9, 3, 3}};
    int SPLIT_7[][] = {{0, 0, 3, 3}, {3, 0, 3, 3}, {6, 0, 3, 3}, {9, 0, 3, 3}, {0, 3, 3, 3}, {3, 3, 3, 3}, {6, 3, 3, 3}, {9, 3, 3, 3}, {0, 6, 3, 3}, {3, 6, 3, 3}, {6, 6, 3, 3}, {9, 6, 3, 3}, {0, 9, 3, 3}, {3, 9, 3, 3}, {6, 9, 3, 3}, {9, 9, 3, 3}};

    public int SplitMode[] = {1, 4, 6, 8, 9, 13, 16};

    private List<int[][]> splitModeList = new ArrayList<int[][]>();
    private Animation mAnimation;

    public GLVideoRender mRender;
    private final GestureDetector mGesDetect = new GestureDetector(this.getContext(), new MyGestureListener());
    private final ScaleGestureDetector mScaleGesture = new ScaleGestureDetector(this.getContext(), new MyScaleGestureListener());

    private boolean isOnePlay = false;
    private boolean isBlowUp = false;
    private boolean isTurnRight = false;
    private boolean isOnDoubleAnimation = false;
    private boolean isOnPagaerAnimation = false;
    private boolean isOnSpliteAnimation = false;
    private boolean isHEMISPHERE_VRSensor = false;

    private int oldSplitMode = -1;
    private int oldPage;

    private boolean mNotSingle = false;

    private VideoAudioTrack mVideoAudioTrack;
    private boolean isPlayAudio = false;

    private SensorManager mSensorManager;
    private Sensor mSensor = null;

    private boolean isFirstGYROSCOPE = true;
    private boolean isFirstGRAVITY = true;
    private boolean isFirstAccelerometer = true;

    private float rate[] = new float[3];
    private float gra[] = new float[3];

    private boolean isDoubleFingerClick = false;
    private boolean screenLock = false;

    private float[][] localList = new float[16][3];
    private float[][] scaleList = new float[16][3];

    public String vrHint = "your phone don't support VR";

    private ConcurrentLinkedQueue<byte[]> audioQueue = new ConcurrentLinkedQueue<byte[]>();
    private boolean isLastScale = false;

    /**
     * 解决双手缩放会乱转一通的问题
     * 延迟时间
     */
    private final long delayTime = 1000;
    private AtomicLong delayCount = new AtomicLong(0);

    /**
     * 用来阻止放大的时候误触发滑动
     */
    private boolean canFling = true;
    private boolean isCreateTimeTask = false;

    private boolean isDoDoubleClick = true;


    /**
     * 保存字体信息
     */
    private SharedPreferences mSharedPreferences;
    private SharedPreferences.Editor mEditor;

    public static boolean isDestory = false;


    public void SetProgressListener(PlayfileProgress progress) {
        if (mRender != null)
            mRender.mProgress = progress;
    }

    public GLVideoDisplay(Context context) {
        super(context);
        init(context);
    }

    public GLVideoDisplay(Context context, AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    //初始化GLVideoDisplay
    private void init(Context context) {
        isDestory = false;
        setEGLContextClientVersion(2);
        mSensorManager = (SensorManager) getContext().getSystemService(Context.SENSOR_SERVICE);
        mRender = new GLVideoRender(context.getApplicationContext(), this);
        this.setRenderer(mRender);
        mAnimation = new Animation(mRender);
        mRender.mEvent = this;
        mVideoAudioTrack = new VideoAudioTrack(8000,
                AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_16BIT);
        mVideoAudioTrack.init();
        mRender.mAudioDataListener = this;
        splitModeList.add(SPLIT_1);
        splitModeList.add(SPLIT_2);
        splitModeList.add(SPLIT_3);
        splitModeList.add(SPLIT_4);
        splitModeList.add(SPLIT_5);
        splitModeList.add(SPLIT_6);
        splitModeList.add(SPLIT_7);
        isWriteFont();
    }

    /**
     * 第一次进入的话就写入字符
     */
    private void isWriteFont() {

        mSharedPreferences = getContext().getSharedPreferences("GLVideo", 0);
        mEditor = mSharedPreferences.edit();
        boolean isFirst = mSharedPreferences.getBoolean("first", false);
        if (!isFirst) {
            String path_1 = getContext().getApplicationContext().getExternalFilesDir(null).getPath() + "/" + "asc24.font";
            String path_2 = getContext().getApplicationContext().getExternalFilesDir(null).getPath() + "/" + "hzk24.font";
            File file_1 = new File(path_1);
            File file_2 = new File(path_2);

            if (!file_1.exists()) {
                try {
                    file_1.createNewFile();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            if (!file_2.exists()) {
                try {
                    file_2.createNewFile();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            InputStream inputStream_1;
            InputStream inputStream_2;
            OutputStream outputStream_1;
            OutputStream outputStream_2;
            try {
                String packageName = getContext().getPackageName();
                Resources res = getContext().getResources();

                inputStream_1 = res.openRawResource(res.getIdentifier("asc24", "raw", packageName));
                outputStream_1 = new FileOutputStream(file_1);

                int length = 0;
                byte[] buffer = new byte[1024];
                while ((length = inputStream_1.read(buffer)) != -1) {
                    outputStream_1.write(buffer, 0, 1024);
                }
                outputStream_1.flush();
                outputStream_1.close();
                inputStream_1.close();

                inputStream_2 = res.openRawResource(res.getIdentifier("hzk24", "raw", packageName));
                outputStream_2 = new FileOutputStream(file_2);

                while ((length = inputStream_2.read(buffer)) != -1) {
                    outputStream_2.write(buffer, 0, 1024);
                }
                outputStream_2.flush();
                outputStream_2.close();
                inputStream_2.close();

                isFirst = true;
                mEditor.putBoolean("first", isFirst);
                mEditor.commit();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        super.surfaceDestroyed(holder);
    }

    /**
     * 手势监听
     *
     * @param event
     * @return
     */
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        Log.d("onTouch", "onTouchEvent come");
        mRender.SetWindowWidthHeight(mRender.mParametricManager, getWidth(), getHeight());
        //Log.d(TAG, "onTouchEvent: ---->" + isOnPagaerAnimation + "--" + isOnDoubleAnimation + "--" + isOnSpliteAnimation);
        if (isOnPagaerAnimation || isOnDoubleAnimation || isOnSpliteAnimation) {
            return true;
        }

        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                if (event.getPointerCount() == 1 && !isDoubleFingerClick)
                    mRender.DoTapOrMouseDown(mRender.mParametricManager, (int) event.getX(), (int) event.getY());
                break;
            case MotionEvent.ACTION_UP:
                if (event.getPointerCount() == 1 && !isDoubleFingerClick && !isLastScale) {
                    mRender.DoTapOrMouseUp(mRender.mParametricManager, (int) event.getX(), (int) event.getY(), GLVideoConnect.getInstance().GetWallModelType(msg, 0));
                }
                if (mGestureListener != null) {
                    mGestureListener.onUp();
                }
                break;
        }
        if (event.getPointerCount() == 1) {
            isDoubleFingerClick = false;
            mGesDetect.onTouchEvent(event);
        } else {
            isDoubleFingerClick = true;
            mScaleGesture.onTouchEvent(event);
        }
        return true;
    }

    /**
     * 动画结束监听
     *
     * @param msgid
     */
    @Override
    public void AnimationEndEvent(int msgid) {
        switch (msgid) {
            case 1:
                onDouble();
                mRender.ResetPosition(mRender.mParametricManager, false, 0);
                break;
            case 2:
                onPaging();
                mRender.ResetPosition(mRender.mParametricManager, false, 0);
                break;
            case 3:
                mRender.isPauseDraw = true;
                //             mRender.SetSplit(mRender.mParametricManager,oldSplitMode);
                isOnePlay = false;
                mRender.SetSplit(mRender.mParametricManager, oldSplitMode);
                for (int i = 0; i < SplitMode[oldSplitMode]; i++) {
                    float[] temp = mRender.GetObjectPosition(mRender.mParametricManager, 0, false, i + GetPage() * SplitMode[oldSplitMode]);
                    localList[i] = temp;
                    Log.d("ScreenLocal", "x:" + temp[0] + " y:" + temp[1] + " z:" + temp[2]);
                    temp = mRender.GetObjectPosition(mRender.mParametricManager, 1, false, i + GetPage() * SplitMode[oldSplitMode]);
                    scaleList[i] = temp;
                    Log.d("ScaleLocal", "x:" + temp[0] + " y:" + temp[1] + " z:" + temp[2]);
                }
                SpliteAnimation_2();
                break;
            case 4:
                mRender.ResetPosition(mRender.mParametricManager, false, 0);
                animationIndex = 0;
                isOnSpliteAnimation = false;
                break;
            case 5:
                animationIndex++;
                SpliteAnimation_2(animationIndex);
                break;
        }
        if (mAnimationEndListener != null)
            mAnimationEndListener.OnAnimationEnd(msgid);
    }

    /**
     * 初始化 陀螺仪 传感器
     *
     * @param mode
     */
    private void initSensor(int mode) {
        mSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
        if (mSensor == null || mSensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY) == null) {
            if (mSensorListener != null) {
                mSensorListener.noSensorSupport();
            }
        }
        mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY), SensorManager.SENSOR_DELAY_GAME);
        mSensorManager.registerListener(this, mSensor, SensorManager.SENSOR_DELAY_GAME);
    }

    /**
     * 销毁传感器
     */
    public void destorySensor() {
        if (mSensor != null)
            mSensorManager.unregisterListener(this, mSensor);
        if (mSensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY) != null)
            mSensorManager.unregisterListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY));
        isFirstGRAVITY = true;
        isFirstGYROSCOPE = true;
        isFirstAccelerometer = true;
        mSensor = null;
    }

    public boolean isEnableScroll = true;

    public void setScrollEnable(boolean state) {
        isEnableScroll = state;
    }

    private void onDouble() {
        isOnDoubleAnimation = false;
        if (screenLock)
            return;


        Log.d(TAG, "onDouble: ------>" + GetMode());
        if (GetMode() == Parameter.SCRN_NORMAL) {
            if (!isOnePlay) {
                oldPage = GetPage();
                oldSplitMode = GetSplitMode();
                mRender.SetSingVideo(mRender.mParametricManager, mRender.GetVideoIndex(mRender.mParametricManager), false);
                for (int i = 0; i < 36; i++) {
                    if (!mRender.GetVisibility(mRender.mParametricManager, i)) {
                        //         CloseVideo(i);
                        if (!msg.equals("-1")) {
                            int chn = GLVideoConnect.getInstance().GetChannel(msg, i);
                            int bit = GLVideoConnect.getInstance().GetBitrate(msg, i);
                            Log.d("openchannel", "close this channel:" + chn + " this index is:" + i + " this msg:" + msg);
                            GLVideoConnect.getInstance(null).CloseChannel(msg, bit, chn, i);
                        }
                    }
                }
                isOnePlay = true;
            } else {
                mRender.SetSplit(mRender.mParametricManager, oldSplitMode);
                int page = getVideoIndex() / SplitMode[oldSplitMode];
                mRender.SetScreenPage(mRender.mParametricManager, page);
                mRender.SetSelectedByIndex(mRender.mParametricManager, mRender.GetVideoIndex(mRender.mParametricManager));
                for (int i = 0; i < 36; i++) {
                    if (mRender.GetVisibility(mRender.mParametricManager, i)) {
                        //        OpenVideo(GetBitrate(i), i);
                        if (!msg.equals("-1")) {
                            int chn = GLVideoConnect.getInstance().GetChannel(msg, i);
                            int bit = GLVideoConnect.getInstance().GetBitrate(msg, i);
                            Log.d("openchannel", "open this channel:" + chn + " this index is:" + i + " this msg:" + msg);
                            GLVideoConnect.getInstance(null).OpenChannel(msg, bit, chn, i);
                        }
                    }
                }
                isOnePlay = false;
            }
        }
        //手势回调去titanium的回调
        if (mGestureListener != null)
            mGestureListener.onDoubleClick(GetAllPage(),
                    GetPage(),
                    GetSplitMode(),
                    GLVideoConnect.getInstance().GetRecordState(msg, getVideoIndex()),
                    GetMode() != Parameter.SCRN_NORMAL
                            ? 0 : GLVideoConnect
                            .getInstance(null)
                            .GetBitrate(msg, getVideoIndex()),
                    getVideoIndex());
    }


    private int tempOldPage;

    private void onPaging() {

        isOnPagaerAnimation = false;
        //TODO:isEnableScroll
        if (isEnableScroll && mRender.GetMode(mRender.mParametricManager) == Parameter.SCRN_NORMAL) {
            int page = GetPage();
            int allpage = GetAllPage();
            if (isTurnRight)                      //向右翻页
            {
                page++;
                if (page >= allpage) {
                    page = 0;
                }
            } else                                   //向左翻页
            {
                page--;
                if (page < 0) {
                    page = allpage - 1;
                }
            }
            mRender.SetScreenPage(mRender.mParametricManager, page);
            if (GetSplitMode() == Parameter.SCRN_SPLIT_ONE) {
                mRender.SetSelectedByIndex(mRender.mParametricManager, page);
            } else {
                mRender.SetSelectedByIndex(mRender.mParametricManager, SplitMode[GetSplitMode()] * page);
            }
            for (int i = 0; i < 36; i++) {
                if (mRender.GetVisibility(mRender.mParametricManager, i)) {
                    //         OpenVideo(GetBitrate(i), i);
                    int chn = GLVideoConnect.getInstance(null).GetChannel(msg, i);
                    int bit = GLVideoConnect.getInstance(null).GetBitrate(msg, i);
                    Log.d("textBitrate", "bitrate:" + bit);
                    GLVideoConnect.getInstance(null).OpenChannel(msg, bit, chn, i);
                    ShowVideoLoading(i);
                } else {
                    //         CloseVideo(i);
                    int chn = GLVideoConnect.getInstance(null).GetChannel(msg, i);
                    int bit = GLVideoConnect.getInstance(null).GetBitrate(msg, i);
                    Log.d("textBitrate", "bitrate:" + bit);
                    GLVideoConnect.getInstance(null).CloseChannel(msg, bit, chn, i);
                }
            }
        }
        if (mGestureListener != null)
            mGestureListener.onPageChange(GetAllPage(),
                    GetPage(),
                    GetSplitMode(),
                    GLVideoConnect.getInstance().GetRecordState(msg, getVideoIndex()),
                    GetMode() != Parameter.SCRN_NORMAL
                            ? 0 : GLVideoConnect
                            .getInstance(null)
                            .GetBitrate(msg, getVideoIndex()),
                    getVideoIndex());

    }

    private void SpliteAnimation_1() {
        isOnSpliteAnimation = true;
        int nowPage = GetPage() + 1;
        int nowSpliteMode = GetSplitMode();
        int screenCount = SplitMode[nowSpliteMode];
        int msg = 0;
        float[] temp = new float[3];
        temp[0] = 0.0f;
        temp[1] = 0.0f;
        temp[2] = 1;
        for (int i = (nowPage - 1) * screenCount; i < screenCount * nowPage; i++) {
            if (i + 1 == screenCount * nowPage)
                msg = 3;
            mRender.StartAnimation(mRender.mParametricManager, temp, 80, 3000, false, 1, false, i, true, -1);
            mAnimation.position(0, 0, 0, 80, 3000, false, false, i, true, msg);
        }
    }

    private void SpliteAnimation_2(int index) {
        int nowPage = GetPage() + 1;
        int nowSpliteMode = GetSplitMode();
        int screenCount = SplitMode[nowSpliteMode];
        float temp[];
        int msg = 5;
        Log.d("index", "index:" + index + " index1:" + screenCount * nowPage);
        if (index + 1 == screenCount * nowPage)
            msg = 4;
        temp = scaleList[index - (nowPage - 1) * screenCount];
        mRender.StartAnimation(mRender.mParametricManager, temp, 80, 3000, false, 1, false, index, true, -1);
        temp = localList[index - (nowPage - 1) * screenCount];
        mAnimation.position(temp[0], temp[1], temp[2], 5, 3000, false, false, index, true, msg);

    }

    private int animationIndex = 0;

    private void SpliteAnimation_2() {
        int nowPage = GetPage() + 1;
        int nowSpliteMode = GetSplitMode();
        int screenCount = SplitMode[nowSpliteMode];
        float temp[] = new float[3];
        int msg = 0;
        for (int i = (nowPage - 1) * screenCount; i < screenCount * nowPage; i++) {
            temp[0] = 0;
            temp[1] = 0;
            temp[2] = 0;
            mRender.TransformObject(mRender.mParametricManager, temp, 0, false, i);
            temp[0] = 0.0f;
            temp[1] = 0.0f;
            temp[2] = 1;
            mRender.TransformObject(mRender.mParametricManager, temp, 1, false, i);
        }
        mRender.isPauseDraw = false;
        //      for(int i = (nowPage-1)*screenCount; i < screenCount*nowPage; i++) {
        //          if(i+1 == screenCount*nowPage)
        //              msg = 4;
         /*   temp = scaleList[i-(nowPage-1)*screenCount];
            mRender.StartAnimation(mRender.mParametricManager,temp,80,3000,false,1,false,i,true,-1);
            temp = localList[i-(nowPage-1)*screenCount];
            mAnimation.position(temp[0],temp[1],temp[2],80,3000,false,false,i,true,msg);*/
        //      }
        animationIndex = (nowPage - 1) * screenCount;
        SpliteAnimation_2(animationIndex);
    }

    private void SetSelect(MotionEvent e) {
        if (GetMode() == Parameter.SCRN_NORMAL && GetSplitMode() > 0) {
            int nowSplitMode = GetSplitMode();
            int mWidth = getWidth() / 12;
            int mHeight = getHeight() / 12;
            int m[];
            float eventX = e.getX();
            float eventY = e.getY();
            int i;
            int mode[][] = splitModeList.get(nowSplitMode);

            for (i = 0; i < mode.length; i++) {
                m = mode[i];
                int mX = m[0] * mWidth + m[2] * mWidth;
                int mY = m[1] * mHeight + m[3] * mHeight;
                if (eventX < mX && eventY < mY && eventX > m[0] * mWidth) {
                    break;
                }
            }
            if (i + GetPage() * SplitMode[nowSplitMode] < mRender.GetScreenCount(mRender.mParametricManager)) {
                int index = i + GetPage() * SplitMode[nowSplitMode];
                mRender.SetSelectedByIndex(mRender.mParametricManager, i + GetPage() * SplitMode[nowSplitMode]);
                //        mRender.SwitchAudioIndex(mRender.mParametricManager, i + GetPage() * SplitMode[nowSplitMode]);
                GLVideoConnect.getInstance().PlayAudioIndex(msg, i + GetPage() * SplitMode[nowSplitMode]);
                audioQueue.clear();
            }
        }
    }

    @Override
    public void OnAudioDataListener(byte[] data) {
        if (isPlayAudio) {
            audioQueue.add(data);
        }
    }

    //陀螺仪感应的回调事件
    @Override
    public void onSensorChanged(SensorEvent event) {
        switch (event.sensor.getType()) {
            case Sensor.TYPE_GYROSCOPE:
                if (mRender.GetMode(mRender.mParametricManager) == Parameter.SCRN_VR
                        || mRender.GetMode(mRender.mParametricManager) == Parameter.SCRN_SPHERE
                        || mRender.GetMode(mRender.mParametricManager) == 17)    //vr模式下自动跟踪
                {
                    if (isFirstGYROSCOPE) {
                        isFirstGYROSCOPE = false;
                        rate = event.values;
                    }
                } else if (GetMode() == Parameter.SCRN_HEMISPHERE && !isHEMISPHERE_VRSensor) {
                    if (Math.abs(event.values[0]) > Math.abs(event.values[1]) && Math.abs(event.values[0]) > Math.abs(event.values[2]) && Math.abs(event.values[0]) > 5.0) {
                        if (mSensorListener != null) {
                            mSensorListener.onSensorShake();
                        } else {
                            mRender.DoDoubleTap(mRender.mParametricManager, GLVideoConnect.getInstance().GetWallModelType(msg, 0), 360, -1, 0);
                        }
                    }
                } else if (GetMode() == Parameter.SCRN_HEMISPHERE && isHEMISPHERE_VRSensor) {
                    if (isFirstGYROSCOPE) {
                        isFirstGYROSCOPE = false;
                        rate = event.values;
                    }
                }
                break;
            case Sensor.TYPE_GRAVITY:
                if (isFirstGRAVITY) {
                    isFirstGRAVITY = false;
                    gra = event.values;
                }
                break;
        }
        if (!isFirstGRAVITY && !isFirstGYROSCOPE) {
            Activity activity = (Activity) getContext();
            mRender.VRSensor(mRender.mParametricManager, rate, gra, activity.getResources().getConfiguration().orientation, GetMode());
            isFirstGRAVITY = true;
            isFirstGYROSCOPE = true;
        }
    }


    public void setDoDoubleClick(boolean state) {
        this.isDoDoubleClick = state;
    }

    public void setScaleObject(boolean state, int index) {
        Log.d(TAG, "setScaleObject: ----->" + state + "  index:" + index);
        if (state) {
            mRender.TransformObject(mRender.mParametricManager, new float[]{4, 4, 4}, 1, true, index);
//            mRender.DoTapOrMouseWheel(mRender.mParametricManager, 100, 0, 0, getVideoIndex(), GLVideoConnect.getInstance().GetWallModelType(msg, 0));
        } else {
            mRender.TransformObject(mRender.mParametricManager, new float[]{1, 1, 1}, 1, true, index);
//            mRender.DoTapOrMouseWheel(mRender.mParametricManager, -100, 0, 0, getVideoIndex(), GLVideoConnect.getInstance().GetWallModelType(msg, 0));
        }

    }


    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }

    /**
     *
     */
    private boolean isScaleController;

    ////////////////////手势操作开始///////////////////////////////////////////////////////
    class MyGestureListener extends GestureDetector.SimpleOnGestureListener {
        @Override
        public void onLongPress(MotionEvent e) {

        }

        @Override
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
            Log.d("onTouch", "onFling come");
            isLastScale = false;
            if (isBlowUp || screenLock)
                return true;

            if (!isEnableScroll) {
                return true;
            }
            if (mRender.GetMode(mRender.mParametricManager) == Parameter.SCRN_NORMAL) {
                mRender.ClearAnimation(mRender.mParametricManager);
                int nowSplitMode = mRender.GetSplitMode(mRender.mParametricManager);
                int nowPageIndex = mRender.GetPageIndex(mRender.mParametricManager) + 1;
                tempOldPage = GetAllPage();
                if (tempOldPage == 1)
                    return true;
                int msg;
                isOnPagaerAnimation = true;
                if (e1.getX() > e2.getX())           //向右翻页
                {
                    for (int i = nowPageIndex * SplitMode[nowSplitMode] - SplitMode[nowSplitMode]; i < nowPageIndex * SplitMode[nowSplitMode]; i++) {
                        if (i == nowPageIndex * SplitMode[nowSplitMode] - 1) {
                            msg = 2;
                        } else {
                            msg = -1;
                        }
                        //                    float[] a = mRender.GetObjectPosition(mRender.mParametricManager,2,false, 0);
                        mAnimation.rotate(0, 360, 0, 80, 3000, false, false, i, true, msg);
                    }
                    isTurnRight = true;
                } else                                   //向左翻页
                {
                    for (int i = nowPageIndex * SplitMode[nowSplitMode] - SplitMode[nowSplitMode]; i < nowPageIndex * SplitMode[nowSplitMode]; i++) {
                        if (i == nowPageIndex * SplitMode[nowSplitMode] - 1) {
                            msg = 2;
                        } else {
                            msg = -1;
                        }
                        mAnimation.rotate(0, -360, 0, 80, 3000, false, false, i, true, msg);
                    }
                    isTurnRight = false;
                }
            }
            return true;
        }

        @Override
        public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
            Log.d("onTouch", "onScroll come");
            Log.d("onFling", "this canFling is " + canFling);

            if (!canFling) {
                return true;
            }

            isLastScale = false;
            if (isBlowUp && mRender.GetMode(mRender.mParametricManager) == Parameter.SCRN_NORMAL) {
                //      mRender.ActionMove(mRender.mParametricManager, distanceX, distanceY, mRender.GetMode(mRender.mParametricManager), mRender.GetVideoIndex(mRender.mParametricManager));
                mRender.DoTapOrMouseMove(mRender.mParametricManager, (int) e2.getX(), (int) e2.getY(), 0, getVideoIndex());
            } else if (mRender.GetMode(mRender.mParametricManager) != Parameter.SCRN_NORMAL)
                mRender.DoTapOrMouseMove(mRender.mParametricManager, (int) e2.getX(), (int) e2.getY(), GLVideoConnect.getInstance().GetWallModelType(msg, 0), 0);
            if (mGestureListener != null)
                mGestureListener.onScroll();
            return true;
        }

        @Override
        public boolean onDoubleTap(MotionEvent e) {
            Log.d("onTouch", "双击事件  mRender.GetMode(mRender.mParametricManager) = " + mRender.GetMode(mRender.mParametricManager));

            if (!isDoDoubleClick) {
                return true;
            }
            isLastScale = false;
            if (isBlowUp) {
                mRender.DoTapOrMouseWheel(mRender.mParametricManager, -100, 0, 0, getVideoIndex(), GLVideoConnect.getInstance().GetWallModelType(msg, 0));
                isBlowUp = false;
                Log.d("zasko", "onDoubleTap: --->" + isBlowUp);
                return true;
            }
            SetSelect(e);
            if (mRender.GetMode(mRender.mParametricManager) == Parameter.SCRN_NORMAL) {      //正常模式下的双击
                if (mNotSingle)
                    isOnDoubleAnimation = true;
                if (!isOnePlay) {
            /*        lpos = mRender.GetObjectPosition(mRender.mParametricManager,
                            Animation.JA_ANI_TYPE_POSITION,
                            false,mRender.GetVideoIndex(mRender.mParametricManager));*/

            /*        lscale = mRender.GetObjectPosition(mRender.mParametricManager,
                            Animation.JA_ANI_TYPE_SCALE,
                            false,mRender.GetVideoIndex(mRender.mParametricManager));*/
                    if (!mNotSingle) {
                        if (isScaleController) {
                            mRender.TransformObject(mRender.mParametricManager, new float[]{1, 1, 1}, 1, true, 0);
                        } else {
                            isBlowUp = true;
                            mRender.TransformObject(mRender.mParametricManager, new float[]{4, 4, 4}, 1, true, 0);
                        }
                        isScaleController = !isScaleController;


                    } else {

                        mRender.StartAnimation(mRender.mParametricManager, SCREEN_SCALE_1[0], 80, 3000, false, 1, false, mRender.GetVideoIndex(mRender.mParametricManager), true, 0);
                        mAnimation.position(0, 0, 0, 80, 3000, false, false, mRender.GetVideoIndex(mRender.mParametricManager), true, 1);
                    }
                } else {
                    int currenIndex = getVideoIndex();
                    if (currenIndex >= SplitMode[oldSplitMode])
                        currenIndex = currenIndex % SplitMode[oldSplitMode];
                    float[] lpos = localList[currenIndex];
                    float[] lscale = scaleList[currenIndex];
                    //TODO:07-29
                    if (!mNotSingle) {
                        if (isScaleController) {
                            mRender.TransformObject(mRender.mParametricManager, new float[]{1, 1, 1}, 1, true, 0);
                        } else {
                            isBlowUp = true;
                            mRender.TransformObject(mRender.mParametricManager, new float[]{4, 4, 4}, 1, true, 0);
                        }
                        isScaleController = !isScaleController;


                    } else {
                        mRender.StartAnimation(mRender.mParametricManager, lscale, 80, 3000, false, 1, false, mRender.GetVideoIndex(mRender.mParametricManager), true, 0);
                        mAnimation.position(lpos[0], lpos[1], lpos[2], 100, 3000, false, false, mRender.GetVideoIndex(mRender.mParametricManager), true, 1);
                    }
                }

            } else if (mRender.GetMode(mRender.mParametricManager) == Parameter.SCRN_HEMISPHERE) {                 //鱼眼全景模式下的双击

                if (mGestureListener != null) {
                    Log.d("double", "double.........................");
                    mGestureListener.onDoubleClick(1, 0, 0, false, 0, 0);
                    //mRender.DoDoubleTap(mRender.mParametricManager, GLVideoConnect.getInstance().GetWallModelType(msg, 0), mCurrentScene);
                }
            } else if (mRender.GetMode(mRender.mParametricManager) == Parameter.SCRN_FOUR) {
                int mWidth = getWidth() / 12;
                int mHeight = getHeight() / 12;
                int mode[][] = splitModeList.get(1);
                int m[];
                int i;
                for (i = 0; i < mode.length; i++) {
                    m = mode[i];
                    int mX = m[0] * mWidth + m[2] * mWidth;
                    int mY = m[1] * mHeight + m[3] * mHeight;
                    if (e.getX() < mX && e.getY() < mY && e.getX() > m[0] * mWidth) {
                        break;
                    }
                }
                int index = i + GetPage() * SplitMode[1];
                if (mGestureListener != null)
                    mGestureListener.onDoubleClick(1, 0, 0, false, 0, index);
            }

            if (mRender.GetMode(mRender.mParametricManager) == Parameter.SCRN_STITCH2) {  //   P720
                if (mGestureListener != null)
                    mGestureListener.onDoubleClick(1, 0, 0, false, 0, 0);
            }
            Log.d("zasko", "onDoubleTap: ---> last" + isBlowUp);
            return true;
        }

        @Override
        public boolean onSingleTapUp(MotionEvent e) {
            Log.d("onTouch", "onSingleTapUp come");
            isLastScale = false;
            SetSelect(e);
            if (mGestureListener != null)
                mGestureListener.onSingleClick(GetAllPage(),
                        GetPage(),
                        GetSplitMode(),
                        GLVideoConnect.getInstance().GetRecordState(msg, getVideoIndex()),
                        GetMode() != Parameter.SCRN_NORMAL
                                ? 0 : GLVideoConnect
                                .getInstance(null)
                                .GetBitrate(msg, getVideoIndex()),
                        getVideoIndex());
            return true;
        }
    }

    class MyScaleGestureListener implements ScaleGestureDetector.OnScaleGestureListener {
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            Log.d("onTouch", "onScale come");

            if (mRender.GetMode(mRender.mParametricManager) == Parameter.SCRN_NORMAL && isOnePlay || mRender.GetSplitMode(mRender.mParametricManager) == Parameter.SCRN_SPLIT_ONE) {
                //       mRender.ActionScale(mRender.mParametricManager, detector.getScaleFactor(), mRender.GetVideoIndex(mRender.mParametricManager));
                Log.d("ONSCALE", "onscale:" + (int) detector.getScaleFactor());
                float[] lscale = mRender.GetScale(mRender.mParametricManager, true, mRender.GetVideoIndex(mRender.mParametricManager));
                Log.d("0715", "scale.x:" + lscale[0] + ",scale.y:" + lscale[1] + ",scale.z:" + lscale[2]);
                mRender.DoTapOrMouseWheel(mRender.mParametricManager, (int) detector.getScaleFactor(), 0, 0, getVideoIndex(), GLVideoConnect.getInstance().GetWallModelType(msg, 0));
                if (lscale[0] > 1) {
                    isBlowUp = true;
                    Log.d("BLOWUP", "放大啦啊啦啦啦啦");
                } else {
                    isBlowUp = false;
                }
                return true;
            }
            if (mRender.GetMode(mRender.mParametricManager) != Parameter.SCRN_NORMAL) {
                delayCount.set(0);
                canFling = false;
                int intra = 0;
                if (detector.getScaleFactor() > 1) {
                    intra = 1;
                } else {
                    intra = -1;
                }
                if (intra == 0) {
                    return true;
                }
                isLastScale = true;
                mRender.DoTapOrMouseWheel(mRender.mParametricManager, intra, (int) detector.getFocusX(), (int) detector.getFocusY(), 0, GLVideoConnect.getInstance().GetWallModelType(msg, 0));
            }
            if (mGestureListener != null)
                mGestureListener.onScale(detector);
            return true;
        }

        @Override
        public boolean onScaleBegin(ScaleGestureDetector detector) {

            return true;
        }

        @Override
        public void onScaleEnd(ScaleGestureDetector detector) {
            Log.d("canFliing", "this canFling is :" + canFling);
            if (isCreateTimeTask)
                return;
            isCreateTimeTask = true;
            new Thread(new Runnable() {
                @Override
                public void run() {
                    while (delayCount.get() < delayTime) {
                        delayCount.addAndGet(1);
                        try {
                            Thread.sleep(1);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                    canFling = true;
                    isCreateTimeTask = false;
                }
            }).start();
        }
    }
////////////////////手势操作结束///////////////////////////////////////////////////////

    /////////////////////////////所有的sdk对外接口////////////////////////////////////////////////
    public void SwitchPanoramaMode(int mode) {
        Log.i("Lee", "GLVideoDisplay::SwitchPanoramaMode" + mode);
        if (mode != -1) {
            if (mode != 0)
                GLVideoConnect.getInstance().EnableCrop(msg, true);
            /*else
                GLVideoConnect.getInstance().EnableCrop(msg, false);*/
        }
        mRender.SetMode(mRender.mParametricManager, mode);
        if (mode == Parameter.SCRN_HEMISPHERE) {
            mRender.SetViewAngle(mRender.mParametricManager, 60);
            Log.i("Lee", "SwitchPanoramaMode  设置ViewAngle");
        }
        if (mode == Parameter.SCRN_HEMISPHERE_VRSensor) {
            isHEMISPHERE_VRSensor = true;
        } else {
            isHEMISPHERE_VRSensor = false;
        }
        if (mode == Parameter.SCRN_VR || mode == Parameter.SCRN_HEMISPHERE || mode == Parameter.SCRN_HEMISPHERE_VRSensor || mode == Parameter.SCRN_VR_SPHERE) {
            initSensor(mode);
        } else if (mSensor != null) {
            destorySensor();
        }
        mRender.mScreenMode = mode;
    }

    public int GetSplitMode() {
        return mRender.GetSplitMode(mRender.mParametricManager);
    }

    public int GetAllPage() {
        return mRender.GetAllPage(mRender.mParametricManager);
    }

    public int GetPage() {
        return mRender.GetPageIndex(mRender.mParametricManager);
    }

    public int getVideoIndex() {
        return mRender.GetVideoIndex(mRender.mParametricManager);
    }

    public int getVideoPagerCount() {
        return mRender.GetAllPage(mRender.mParametricManager);
    }

    public int getVideoCurrPager() {
        return mRender.GetPageIndex(mRender.mParametricManager);
    }


    public boolean isPlayAudio_;

    public void PlayAudio() {
        isPlayAudio_ = true;
        mRender.OpenAudioPlaying(mRender.mParametricManager);
    }

    public void StopAudio() {
        isPlayAudio_ = false;
        mRender.PauseAudioPlaying(mRender.mParametricManager);
    }

    public void enableAudio(boolean state) {
        mRender.EnableAudio(mRender.mParametricManager, state);
    }

    public boolean GetAudioPlayState() {
        return isPlayAudio;
    }

    public void SetViewAngle(float value) {
        mRender.SetViewAngle(mRender.mParametricManager, value);
    }

    public void setNotSingleState(boolean state) {
        this.mNotSingle = state;
    }

    public boolean isOnePlay() {
        return isOnePlay;
    }

    public void SetSplit(int mode) {
    /*    if(isOnSpliteAnimation)
            return;*/
        oldSplitMode = mode;
        mNotSingle = true;
        //     SpliteAnimation_1();
        isOnePlay = false;
        if (mode != 0)
            mRender.mHardwareDecoder = false;
        Log.d("GLVideoDisplay.java", "test set split................." + mode);
        mRender.SetSplit(mRender.mParametricManager, mode);


        for (int i = 0; i < 36; i++) {
            int chn = GLVideoConnect.getInstance(null).GetChannel(msg, i);
            int bit = GLVideoConnect.getInstance(null).GetBitrate(msg, i);
            Log.d("textBitrate", "bitrate:" + bit);
            GLVideoConnect.getInstance(null).CloseChannel(msg, bit, chn, i);
        }
        for (int i = 0; i < SplitMode[mode]; i++) {
            float[] temp = mRender.GetObjectPosition(mRender.mParametricManager, 0, false, i + GetPage() * SplitMode[mode]);
            localList[i] = temp;
            Log.d("ScreenLocal", "x:" + temp[0] + " y:" + temp[1] + " z:" + temp[2]);
            temp = mRender.GetObjectPosition(mRender.mParametricManager, 1, false, i + GetPage() * SplitMode[mode]);
            scaleList[i] = temp;
            Log.d("ScaleLocal", "x:" + temp[0] + " y:" + temp[1] + " z:" + temp[2]);
        }
        if (mode > 5) {
            for (int i = 0; i < 36; i++) {
                GLVideoConnect.getInstance().setOnlyDecoderIFrame(msg, true, i);
            }
        } else {
            for (int i = 0; i < 36; i++) {
                GLVideoConnect.getInstance().setOnlyDecoderIFrame(msg, false, i);
            }
        }
        for (int i = 0; i < 36; i++) {
            if (mRender.GetVisibility(mRender.mParametricManager, i)) {
                //         OpenVideo(GetBitrate(i), i);
                int chn = GLVideoConnect.getInstance(null).GetChannel(msg, i);
                int bit = GLVideoConnect.getInstance(null).GetBitrate(msg, i);
                Log.d("textBitrate", "bitrate:" + bit);
                //GLVideoConnect.getInstance(null).OpenChannel(msg, bit, chn, i);
            } else {
                //         CloseVideo(i);
                int chn = GLVideoConnect.getInstance(null).GetChannel(msg, i);
                int bit = GLVideoConnect.getInstance(null).GetBitrate(msg, i);
                Log.d("textBitrate", "bitrate:" + bit);
                GLVideoConnect.getInstance(null).CloseChannel(msg, bit, chn, i);
            }
        }
    }

    public void SetVideoSelectIndex(int index) {
        mRender.SetSelectedByIndex(mRender.mParametricManager, index);
    }

    public void SetPageIndex(int index) {
        mRender.SetScreenPage(mRender.mParametricManager, index);
    }

    /**
     * 获取正常模式下的某个video的显示状态
     *
     * @param index
     * @return
     */
    public boolean GetVisibility(int index) {
        return mRender.GetVisibility(mRender.mParametricManager, index);
    }

    /**
     * 圆筒展开
     */
    public void CylinderUnwind() {
        if (GetMode() == Parameter.SCRN_CYLINDER) {
            mRender.CylinderUnwind(mRender.mParametricManager);
        }
    }

    public int GetMode() {
        return mRender.GetMode(mRender.mParametricManager);
    }

    /**
     * @param fileName
     * @param isImage
     * @param isFishEye
     */
    public void FFPlayFile(String fileName, boolean isImage, boolean isFishEye) {
        mRender.disableHardWareDecoder();
        mRender.ShowVideoLoading(0);
        mRender.Playfile(mRender.mParametricManager, fileName, isImage, isFishEye);
    }

    /**
     */
    public void FFStopPlayFile() {
        mRender.StopPlay(mRender.mParametricManager);
    }


//    /**
//     * 文件播放  包括图片和视频类型
//     *
//     * @param fileName
//     * @param isImage
//     * @param isFishEye
//     */
//    public void PlayFile(String fileName, boolean isImage, boolean isFishEye) {
//        mRender.Playfile(mRender.mParametricManager, fileName, isImage, isFishEye);
//    }

//    /**
//     * 停止文件播放
//     */
//    public void StopPlayFile() {
//        mRender.StopPlay(mRender.mParametricManager);
//    }

    /**
     * 退出的时候释放底层的资源
     */
    public void DestroyManager() {
        Log.i("Lee", "GLVideoDisplay.DestroyManager");
        isDestory = true;
        if (mVideoAudioTrack != null)
            mVideoAudioTrack.release();
        mVideoAudioTrack = null;
        destorySensor();
        mRender.destory();
        msg = "";
        GLVideoConnect.getInstance().ResetForceWallMode();
    }

    /**
     * 设置所有的页面数
     *
     * @param allPage
     */
    public void SetAllPage(int allPage) {
        mRender.SetAllPage(mRender.mParametricManager, allPage);
    }


    public void TransformVertex(int mode, float[] vect, boolean texture, int type, int index) {
        mRender.TransformVertex(mRender.mParametricManager, mode, vect, texture, type, index);
    }

    public void UpdateAspect(int width, int height) {
        float tmp = ((float) width / (float) height);
        Log.d(TAG, "UpdateAspect: ------>" + tmp);
        mRender.UpdateAspect(mRender.mParametricManager, tmp);
    }

    public int GetScreenCount() {
        return mRender.GetScreenCount(mRender.mParametricManager);
    }

    public boolean isSupportVR() {
        if (mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE) != null) {
            if (mSensorManager.getDefaultSensor(Sensor.TYPE_GRAVITY) != null) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }

    }

    public void EnableAudio(boolean value) {
        mRender.EnableAudio(mRender.mParametricManager, value);
    }

    public void EnableGrid(boolean value) {
        mRender.EnableGrid(mRender.mParametricManager, value);
    }

    public void ShowVideoLoading(int index) {
        mRender.ShowVideoLoading(index);
    }

    public void HideVideoLoading(int index) {
        mRender.HideVideoLoading(index);
    }

    public void ViewRotate(boolean value) {
        mRender.isRotateScreen = value;
    }


    public void ClearAnimation() {
        mRender.ClearAnimation(mRender.mParametricManager);
    }

    public void StartAnimation(float[] pend,
                               int step,
                               int duration,
                               boolean isloop,
                               int type,
                               boolean texture,
                               int scnindex,
                               boolean inertia,
                               int endmsg) {
        mRender.StartAnimation(mRender.mParametricManager,
                pend,
                step,
                duration,
                isloop,
                type,
                texture,
                scnindex,
                inertia,
                endmsg);
    }

    public void StartMotionTracking(int index) {
        mRender.StartMotionTracking(mRender.mParametricManager, index);
    }

    public void StopMotionTracking(int index) {
        mRender.StopMotionTracking(mRender.mParametricManager, index);
    }

    public void AdjustActionExperience(int mode, int actionType, float value) {
        mRender.AdjustActionExperience(mRender.mParametricManager, mode, actionType, value);
    }

    /**
     * 屏幕锁定
     */
    public void LockScreen() {
        screenLock = true;
    }

    /**
     * 屏幕解锁
     */
    public void UnLockScreen() {
        screenLock = false;
    }

    private GestureListener mGestureListener;

    public void SetGestureListener(GestureListener mGestureListener) {
        this.mGestureListener = mGestureListener;

        Log.d(TAG, "SetGestureListener: ----->" + mGestureListener);
    }


    //TODO:
    public void defaultDoubleClick(int mode, int scene, int scrnFourIndex, int index) {
        mRender.DoDoubleTap(mRender.mParametricManager, GLVideoConnect.getInstance().GetWallModelType(msg, index), scene, scrnFourIndex, index);
    }


    public void SetGLVideoSurfaceCreateListener(GLVideoSurfaceCreateListener mGLVideoSurfaceCreateListener) {
        mRender.mGLVideoSurfaceCreateListener = mGLVideoSurfaceCreateListener;
    }

    public void SetDestoryListener(DestoryListener mDestoryListener) {
        mRender.mDestoryListener = mDestoryListener;
    }

    public void SetBorderColor(int value) {
        mRender.SetBorderColor(value);
    }

    public void Playfile(final String fn) {
        queueEvent(new Runnable() {
            @Override
            public void run() {
                mRender.PlayFile(fn);
            }
        });
    }

    public void StopPlay() {
        mRender.StopPlay();
    }


    public SensorListener mSensorListener;
    public AnimationEndListener mAnimationEndListener;

//    public void SetP720StartAnimationMode(int type, int index) {
//        mRender.SetP720StartAnimationMode(mRender.mParametricManager, type, index);
//    }
    /////////////////////////////所有的sdk对外接口结束////////////////////////////////////////////////
}
