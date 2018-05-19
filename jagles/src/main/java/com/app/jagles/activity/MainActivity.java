package com.app.jagles.activity;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.Nullable;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v4.widget.SwipeRefreshLayout;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.app.jagles.R;
import com.app.jagles.activity.adapter.DeviceRecyclerAdapter;
import com.app.jagles.activity.adapter.ScreenRecyclerAdapter;
import com.app.jagles.activity.base.BaseActivity;
import com.app.jagles.activity.decoration.DefaultDecoration;
import com.app.jagles.connect.JAConnector;
import com.app.jagles.controller.JAParameter;
import com.app.jagles.view.JAGLDisplay;
import com.app.jagles.view.OnGLDisplayCreateListener;

import java.util.ArrayList;
import java.util.List;

import javax.microedition.khronos.opengles.GL10;


/**
 * Created by JuAn_Zhangsongzhou on 2017/6/5.
 */

public class MainActivity extends BaseActivity implements OnGLDisplayCreateListener, SwipeRefreshLayout.OnRefreshListener, DeviceRecyclerAdapter.OnItemClickListener, ScreenRecyclerAdapter.OnItemClickListener {

    private static final String TAG = "MainActivity";

    private DrawerLayout mDrawerLayout;

    private JAGLDisplay mJADisplay;

    private SwipeRefreshLayout mSwipeRefreshLayout;
    private RecyclerView mRecyclerView;
    private DeviceRecyclerAdapter mRecyclerAdapter;
    private List<DeviceRecyclerAdapter.ItemInfo> mDeviceList;

    /*设备分屏*/
    private RecyclerView mScreenRecycler;
    private ScreenRecyclerAdapter mScreenAdapter;
    private List<ScreenRecyclerAdapter.ScreenInfo> mScreenList;

    private TextView mDeviceName;


    private Button mConnectBtn;/*连接*/

    private Button mDisconnectBtn; /*断开连接*/

    private Button mOpenDeviceBtn; /*打开码流*/

    private Button mCloseDeviceBtn; /*关闭码流*/


    private Button mOpenDemoBtn; /*打开测试demo*/
    private Button mOpenM3u8Btn; /*打开m3u8视频*/

    private DeviceRecyclerAdapter.ItemInfo mCurrentDeviceInfo;
    private String mConnectKey = "ConnectKey";


    private int mCurrentBitrate = 0;/*当前码流*/

    private int mCurrentIndex = 0; /*当前下标*/

    private int mCurrentChannel = 0;   /*当前通道*/

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {

            }
        }
    };

    private ConnectResultReceiver mConnectResultReceiver;


    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {


        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        JAConnector.initialize(this.getApplicationContext());
        JAConnector.initJNICrash(Environment.getExternalStorageDirectory().getPath() + "/JAGles/Crash/", "3.0.0");

        mConnectResultReceiver = new ConnectResultReceiver();
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(JAConnector.JA_RESULT_CONNECT);
        registerReceiver(mConnectResultReceiver, intentFilter);

        initView();
        addListener();
        initData();
        addData();

//        startActivity(new Intent(this, DemoConnectActivity.class));

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mConnectResultReceiver != null) {
            unregisterReceiver(mConnectResultReceiver);
        }

    }

    private void initView() {
        mDrawerLayout = (DrawerLayout) findViewById(R.id.drawer_layout);
        mJADisplay = (JAGLDisplay) findViewById(R.id.ja_gl_display);

        mSwipeRefreshLayout = (SwipeRefreshLayout) findViewById(R.id.swipeRefreshLayout);
        mRecyclerView = (RecyclerView) findViewById(R.id.recycler_view);

        mDeviceName = (TextView) findViewById(R.id.device_name_tv);
        mConnectBtn = (Button) findViewById(R.id.connect_btn);
        mDisconnectBtn = (Button) findViewById(R.id.disconnect_btn);
        mOpenDemoBtn = (Button) findViewById(R.id.open_test_demo_btn);
        mOpenDeviceBtn = (Button) findViewById(R.id.open_device_btn);
        mCloseDeviceBtn = (Button) findViewById(R.id.close_device_btn);

        mOpenM3u8Btn = (Button) findViewById(R.id.open_m3u8_btn);

        mRecyclerAdapter = new DeviceRecyclerAdapter(this);


        mScreenRecycler = (RecyclerView) findViewById(R.id.screen_recycler_view);
        mScreenAdapter = new ScreenRecyclerAdapter(this);

    }


    private void addListener() {
        mJADisplay.setGLDisplayCreateListener(this);
        mSwipeRefreshLayout.setOnRefreshListener(this);

        mRecyclerAdapter.setOnItemClickListener(this);
        mScreenAdapter.setOnItemClickListener(this);


        mDeviceConnectImpl = new OnDeviceConnectImpl();

        mConnectBtn.setOnClickListener(mDeviceConnectImpl);
        mDisconnectBtn.setOnClickListener(mDeviceConnectImpl);
        mOpenDeviceBtn.setOnClickListener(mDeviceConnectImpl);
        mCloseDeviceBtn.setOnClickListener(mDeviceConnectImpl);


        mOnClickOpenDemoImpl = new OnClickOpenDemoImpl();
        mOpenDemoBtn.setOnClickListener(mOnClickOpenDemoImpl);
        mOpenM3u8Btn.setOnClickListener(mOnClickOpenDemoImpl);

    }

    private void initData() {

        mRecyclerView.setLayoutManager(new LinearLayoutManager(this));
        mRecyclerView.setHasFixedSize(true);
        mRecyclerView.setItemAnimator(new DefaultItemAnimator());
        mRecyclerView.addItemDecoration(new DefaultDecoration(this, 16, 16));
        mRecyclerView.setAdapter(mRecyclerAdapter);

        mScreenRecycler.setLayoutManager(new LinearLayoutManager(this));
        mScreenRecycler.setHasFixedSize(true);
        mScreenRecycler.setItemAnimator(new DefaultItemAnimator());
        mScreenRecycler.addItemDecoration(new DefaultDecoration(this, 16, 16));
        mScreenRecycler.setAdapter(mScreenAdapter);

    }

    private void addData() {

        mDeviceList = new ArrayList<>();


        /*昵称*/
        String[] deviceNicks = new String[]{"UCE774", "12路", "16路"};
        /*设备id*/
        String[] deviceIDs = new String[]{"UCE77413V2VW944M111A", "354056446", "100008614"};
        /*设备密码*/
        String[] devicePwds = new String[]{"admin:9ce03a3077f3db063b4a75ae65ed1986", "admin:", "admin:"};
        /*设备通道*/
        int[] deviceChannels = new int[]{1, 12, 16};
        /*设备是否是单屏*/
        boolean[] deviceSingle = new boolean[]{true, false, false};


        for (int i = 0; i < deviceNicks.length; i++) {

            DeviceRecyclerAdapter.ItemInfo itemInfo = new DeviceRecyclerAdapter.ItemInfo();
            itemInfo.setType(DeviceRecyclerAdapter.TYPE_DEVICE);
            itemInfo.setState("-1");

            itemInfo.setName(deviceNicks[i]);
            itemInfo.setDeviceId(deviceIDs[i]);
            itemInfo.setPassword(devicePwds[i]);
            itemInfo.setSingle(deviceSingle[i]);
            itemInfo.setChannelCount(deviceChannels[i]);

            mDeviceList.add(itemInfo);
        }

        mRecyclerAdapter.setData(mDeviceList);
        mRecyclerAdapter.notifyDataSetChanged();

        /*
        *
        *  增加多分屏显示数据
        *
        * */
        int[] tmp = new int[]{1, 4, 6, 8, 9, 12, 16};
        mScreenList = new ArrayList<>();
        for (int i = 0; i < tmp.length; i++) {
            ScreenRecyclerAdapter.ScreenInfo itemInfo = new ScreenRecyclerAdapter.ScreenInfo();
            itemInfo.setName(tmp[i] + " 分屏");
            itemInfo.setValue(i);
            mScreenList.add(itemInfo);
        }
        mScreenAdapter.setData(mScreenList);
    }


    @Override
    public void onGLDisplayCreateCallBack(GL10 unused, long handle, int width, int height) {

        mJADisplay.updateAspect(mJADisplay.getMeasuredWidth(), mJADisplay.getMeasuredHeight());

    }


    @Override
    public void onBackPressed() {
        if (mDrawerLayout.isDrawerOpen(GravityCompat.START)) {
            mDrawerLayout.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
        }
    }


    @Override
    public void onRefresh() {

        mHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                mSwipeRefreshLayout.setRefreshing(false);
            }
        }, 1000);
        startPreLink();
    }

    @Override
    public void onItemClick(View view, ScreenRecyclerAdapter.ScreenInfo info, int position) {
        mJADisplay.setSplit(info.getValue());
    }

    @Override
    public void onItemClick(View view, DeviceRecyclerAdapter.ItemInfo info, int position) {

        mCurrentDeviceInfo = info;
        mConnectKey = info.getDeviceId();

        mDeviceName.setText(info.getName());
        if (mCurrentDeviceInfo.isSingle()) {
            mJADisplay.setSwitchPanoramaMode(JAParameter.SCRN_NORMAL);
            mJADisplay.setAllPage(JAParameter.SCRN_SPLIT_FOUR);
        } else {

        }
    }

    /**
     * 开始设备预链接
     */
    private void startPreLink() {

        for (DeviceRecyclerAdapter.ItemInfo itemInfo : mDeviceList) {
            if (itemInfo.getType() == DeviceRecyclerAdapter.TYPE_NORMAL) {
                continue;
            }
            JAConnector.connectDevice(itemInfo.getDeviceId(), itemInfo.getDeviceId(), itemInfo.getPassword(), 0, 0);
        }
    }


    /**
     * 连接状态接收
     */
    private class ConnectResultReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            Log.d(TAG, "onReceive: ----->" + intent.getAction());
            Bundle bundle = intent.getExtras();
            int connectState = bundle.getInt(JAConnector.JAKey.JA_KEY_CONNECT_STATE);
            String connectKey = bundle.getString(JAConnector.JAKey.JA_KEY_CONNECT_KEY);
            int deviceIndex = bundle.getInt(JAConnector.JAKey.JA_KEY_CONNECT_INDEX);

            Log.d(TAG, "onReceive: ----->连接key：" + connectKey + "--连接状态：" + connectState + "--设备下标：" + deviceIndex);

            if (intent.getAction().equals(JAConnector.JA_RESULT_CONNECT)) {
                synchronized (mDeviceList) {
                    int i = 0;
                    for (DeviceRecyclerAdapter.ItemInfo itemInfo : mDeviceList) {
                        if (itemInfo.getType() == DeviceRecyclerAdapter.TYPE_NORMAL) {
                            i++;
                            continue;
                        }
                        if (connectKey.equals(itemInfo.getDeviceId())) {
                            itemInfo.setState(connectState + "");
                            switch (connectState) {
                                case JAParameter.NET_STATUS_OPENING:
                                    mJADisplay.openVideo(JAParameter.VIDEO_BITRATE_SD, deviceIndex, deviceIndex, true);
                                    break;
                            }

                            mRecyclerAdapter.notifyItemChanged(i);
                            break;
                        }

                        i++;
                    }
                }
            }

        }
    }

    private OnDeviceConnectImpl mDeviceConnectImpl;

    private class OnDeviceConnectImpl implements View.OnClickListener {

        @Override
        public void onClick(View v) {
            mJADisplay.setConnectKey(mConnectKey);
            int i1 = v.getId();
            if (i1 == R.id.connect_btn) {
                if (mCurrentDeviceInfo != null) {
                    if (mCurrentDeviceInfo.isSingle()) {
                        mJADisplay.connect(mConnectKey, mCurrentDeviceInfo.getDeviceId(), mCurrentDeviceInfo.getPassword(), 0, 0, true);
                    } else {

                        for (int i = 0; i < mCurrentDeviceInfo.getChannelCount(); i++) {
                            mJADisplay.connect(mConnectKey, mCurrentDeviceInfo.getDeviceId(), mCurrentDeviceInfo.getPassword(), i, i, true);
                        }
                    }
                }

            } else if (i1 == R.id.disconnect_btn) {
                if (mCurrentDeviceInfo != null) {
                    if (mCurrentDeviceInfo.isSingle()) {
                        mJADisplay.disconnect(0);
                    } else {
                        for (int i = 0; i < mCurrentDeviceInfo.getChannelCount(); i++) {
                            mJADisplay.disconnect(i);
                        }

                    }
                }

            } else if (i1 == R.id.open_device_btn) {
                Log.d(TAG, "onClick: ----->" + "打开码流");
                mJADisplay.openVideo(JAParameter.VIDEO_BITRATE_SD, 0, 0, true);

            } else if (i1 == R.id.close_device_btn) {
                mJADisplay.closeVideo(JAParameter.VIDEO_BITRATE_SD, 0, 0);

            }
        }
    }

    private OnClickOpenDemoImpl mOnClickOpenDemoImpl;

    /**
     * 打开测试demo
     */
    private class OnClickOpenDemoImpl implements View.OnClickListener {

        @Override
        public void onClick(View v) {
            int i = v.getId();
            if (i == R.id.open_test_demo_btn) {
                startActivity(new Intent(MainActivity.this, DemoConnectActivity.class));

            } else if (i == R.id.open_m3u8_btn) {
                String url = "http://download.dvr163.com/Video/pseries_neutral_instruction_chs.m3u8";
//                    Intent intent = new Intent(Intent.ACTION_VIEW);
//                    Uri uri = Uri.parse("http://download.dvr163.com/Video/pseries_neutral_instruction_chs.m3u8");
//                    intent.setDataAndType(uri, "video/*");
//                    startActivity(intent);

//                    mJADisplay.playFile();
                startActivity(new Intent(MainActivity.this, SurfaceViewActivity.class));

            }

        }
    }

}
