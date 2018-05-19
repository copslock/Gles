package com.app.jagles.activity;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.app.jagles.R;
import com.app.jagles.activity.adapter.DemoDeviceRecyclerAdapter;
import com.app.jagles.activity.base.BaseActivity;
import com.app.jagles.activity.cache.UserCache;
import com.app.jagles.activity.decoration.DefaultDecoration;
import com.app.jagles.activity.dialog.AddDeviceDialog;
import com.app.jagles.activity.dialog.DeleteDialog;
import com.app.jagles.activity.utils.PermissionUtil;
import com.app.jagles.connect.JAConnector;
import com.app.jagles.controller.JAParameter;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

/**
 * Created by zasko on 2018/1/11.
 */

public class DemoConnectActivity extends BaseActivity {
    private static final String TAG = "DemoConnectActivity";


    private RecyclerView mRecyclerView;
    private DemoDeviceRecyclerAdapter mAdapter;
    private List<DemoDeviceRecyclerAdapter.ItemDemoInfo> mData = new ArrayList<>();
    private List<DemoDeviceRecyclerAdapter.ItemDemoInfo> mCacheData = new ArrayList<>();


    private String mCurrentFilePath;
    private SimpleDateFormat mDateFormat;

    private UserCache mUserCache;
    private Gson mGson;


    private TextView mStartTv;

    private Button mAddDeviceBtn;
    private Button mCancelBtn;

    private ConnectResultReceiver mConnectResultReceiver;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_demo_connect);


        mUserCache = UserCache.getInstance(this);
        mGson = new Gson();
        mDateFormat = new SimpleDateFormat("yy-MM-dd HH:mm:ss");

        mConnectResultReceiver = new ConnectResultReceiver();
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(JAConnector.JA_RESULT_CONNECT);
        registerReceiver(mConnectResultReceiver, intentFilter);

        initView();
        initData();
        initListener();
        addData();

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mConnectResultReceiver != null) {
            unregisterReceiver(mConnectResultReceiver);
        }
    }

    private void initView() {
        mRecyclerView = (RecyclerView) findViewById(R.id.recycler_view);


        mAddDeviceBtn = (Button) findViewById(R.id.title_add_btn);
        mCancelBtn = (Button) findViewById(R.id.title_cancel_btn);
        mStartTv = (TextView) findViewById(R.id.start_tv);


    }

    private void initData() {
        mRecyclerView.setLayoutManager(new LinearLayoutManager(this));
        mRecyclerView.setHasFixedSize(true);
        mRecyclerView.setItemAnimator(new DefaultItemAnimator());
        mRecyclerView.addItemDecoration(new DefaultDecoration(this));

        mAdapter = new DemoDeviceRecyclerAdapter(this);

        mRecyclerView.setAdapter(mAdapter);


    }


    private void initListener() {

        mStartTv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                PermissionUtil.requestSDCardWrite(DemoConnectActivity.this);

                if (PermissionUtil.isHasSDCardWritePermission(DemoConnectActivity.this)) {

                    String dirPath = Environment.getExternalStorageDirectory().getPath() + "/JAGles/";
                    String fileName = System.currentTimeMillis() + ".txt";

                    File dirFile = new File(dirPath);
                    if (!dirFile.exists()) {
                        boolean tmp = dirFile.mkdir();
                        if (!tmp) {
                            dirFile.mkdirs();
                        }
                    }
                    File file = new File(dirFile.getAbsoluteFile() + "/" + fileName);

                    if (!file.exists()) {
                        try {
                            file.createNewFile();
                            mCurrentFilePath = file.getAbsolutePath();

                            writeFile(file.getAbsolutePath(), "设备名,连接成功时间,打开码流时间,断开时间,开始连接到完全断开时间,连接失败,周期次数,日期\n");

                            synchronized (mData) {
                                for (DemoDeviceRecyclerAdapter.ItemDemoInfo info : mData) {

                                    if (info.getConnectCountAll() == info.getConnectCount() || info.isStart()) {
                                        continue;
                                    }
                                    JAConnector.connectDevice(info.getDeviceId(), info.getDeviceId(), info.getPassword(), 0, 0, true);
                                }
                            }
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }
            }
        });

        mAddDeviceBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showAddDeviceDialog();
            }
        });

        mCancelBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                finish();
            }
        });

        mAdapter.setOnItemClickListener(new DemoDeviceRecyclerAdapter.OnItemClickListener() {
            @Override
            public void onItemClick(View view, DemoDeviceRecyclerAdapter.ItemDemoInfo info, int position) {
                showDeleteDialog(info, position);
            }
        });

    }

    private void addData() {

//        DemoDeviceRecyclerAdapter.ItemDemoInfo itemDemoInfo01 = new DemoDeviceRecyclerAdapter.ItemDemoInfo();
//        itemDemoInfo01.setDeviceName("UCE77413V2VW944M111A");
//        itemDemoInfo01.setDeviceId("UCE77413V2VW944M111A");
//        itemDemoInfo01.setPassword("admin:9ce03a3077f3db063b4a75ae65ed1986");
//        itemDemoInfo01.setConnectState(0);
//        itemDemoInfo01.setConnectCount(0);
//
//        mData.add(itemDemoInfo01);

        mData = mGson.fromJson(mUserCache.getDeviceList(), new TypeToken<List<DemoDeviceRecyclerAdapter.ItemDemoInfo>>() {
        }.getType());


        if (mData != null) {

            for (DemoDeviceRecyclerAdapter.ItemDemoInfo info : mData) {
                info.setConnectTip("未开始");
                info.setStart(false);
                info.setConnectCount(0);
            }
            mCacheData.addAll(mData);
        }

        mAdapter.setData(mData);

        mAdapter.notifyDataSetChanged();


    }

    private class ConnectResultReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            if (mData == null) {
                return;
            }
            if (intent.getAction().equals(JAConnector.JA_RESULT_CONNECT)) {
                Bundle bundle = intent.getExtras();
                String key = bundle.getString(JAConnector.JAKey.JA_KEY_CONNECT_KEY);
                int state = bundle.getInt(JAConnector.JAKey.JA_KEY_CONNECT_STATE);


                synchronized (mData) {
                    int i = 0;
                    for (DemoDeviceRecyclerAdapter.ItemDemoInfo info : mData) {
                        if (info.getDeviceId().equals(key)) {
                            info.setConnectState(state);

                            Log.d(TAG, "onReceive: --------->" + "设备连接key：" + key + " 设备连接状态：" + state);
                            info.setStart(true);
                            switch (state) {
                                /*开始连接*/
                                case JAParameter.NET_STATUS_CONNECTING:
                                    long connectTime = System.currentTimeMillis();
                                    info.setStartTime(connectTime); /*一次周期所需要的时间*/
                                    info.setConnectSuccessTime(connectTime); /*连接成功记录时间*/

                                    info.setConnectFailTime(connectTime);

                                    info.setConnectTip("连接开始");
                                    break;

                                /*连接失败*/
                                case JAParameter.NET_STATUS_CONNECTFAIL:
                                    long failTime = System.currentTimeMillis();
                                    info.setConnectFailTime(failTime - info.getConnectFailTime());
                                    String tmp = toTmpString(info.getDeviceId(), 0, 0, 0, 0, info.getConnectFailTime(), info.getConnectCount(), mDateFormat.format(new Date(failTime)));
                                    writeFile(mCurrentFilePath, tmp);
                                    info.setConnectTip("连接失败");
                                    break;
                                /*连接成功*/
                                case JAParameter.NET_STATUS_OPENING:
                                    long connectSuccessTime = System.currentTimeMillis();
                                    info.setConnectSuccessTime(connectSuccessTime - info.getConnectSuccessTime());

                                    info.setOpenFrameTime(connectSuccessTime);/*记录打开码流的时间*/

                                    JAConnector.openDevice(info.getDeviceId(), JAParameter.VIDEO_BITRATE_SD, 0, 0);

                                    info.setConnectTip("连接成功");
                                    break;
                                /*I帧过来*/
                                case JAParameter.NET_STATUS_FIRST_FRAME:
                                    long frameTime = System.currentTimeMillis();
                                    info.setOpenFrameTime(frameTime - info.getOpenFrameTime());
                                    info.setDisconnectTime(frameTime);

                                    JAConnector.disconnectDevice(info.getDeviceId(), 0);

                                    info.setConnectTip("出图成功");
                                    break;
                                /*断开连接*/
                                case JAParameter.NET_STATUS_CLOSED:
                                    long endTime = System.currentTimeMillis();
                                    info.setStartTime(endTime - info.getStartTime());
                                    info.setConnectCount(info.getConnectCount() + 1);
                                    info.setDisconnectTime(endTime - info.getDisconnectTime());
//                                    Log.d(TAG, "onReceive: --------->" + "周期：" + info.getConnectTime() + "一次连接时间：" + (System.currentTimeMillis() - info.getStartTime()));

//                                    String tmpSave = info.getDeviceId() + "," + info.getConnectSuccessTime() + "," + info.getOpenFrameTime() + ","
//                                            + info.getDisconnectTime() + "," + info.getStartTime() + "," + "0" + "," + info.getConnectCount() + "," + mDateFormat.format(new Date(endTime)) + "\n";

                                    String tmpSave = toTmpString(info.getDeviceId(), info.getConnectSuccessTime(), info.getOpenFrameTime(), info.getDisconnectTime()
                                            , info.getStartTime(), 0, info.getConnectCount(), mDateFormat.format(new Date(endTime)));
                                    Log.d(TAG, "onReceive: --->" + tmpSave);
                                    writeFile(mCurrentFilePath, tmpSave);

                                    info.setConnectTip("断开连接");
                                    if (info.getConnectCount() == info.getConnectCountAll()) {
                                        info.setStart(false);
                                        break;
                                    }


                                    JAConnector.connectDevice(info.getDeviceId(), info.getDeviceId(), info.getPassword(), 0, 0);
                                    break;
                            }
                            mAdapter.notifyItemChanged(i);

                        }
                        i++;
                    }
                }
            }
        }
    }

    /*设备名,连接成功时间,打开码流时间,断开时间,开始连接到完全断开时间,连接失败,周期次数,日期*/
    private String toTmpString(String deviceId, long connectSuccessTime, long openFrameTime, long disconnectTime, long startTime, long failTime
            , int count, String date) {

        return deviceId + "," + ((double) connectSuccessTime / 1000) + "," + ((double) openFrameTime / 1000) + "," + ((double) disconnectTime / 1000) + "," + ((double) startTime / 1000) + "," + ((double) failTime / 1000) + "," + count
                + "," + date + "\n";
    }

    private void writeFile(String filePath, String content) {

        BufferedWriter out = null;

        try {

            content = new String(content.getBytes(), "utf-8");
            out = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(filePath, true)));
            out.write(content);

            Log.d(TAG, "onReceive: --->" + "写入成功！！！");
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {

            if (out != null) {
                try {
                    out.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

    }

    private AddDeviceDialog mAddDeviceDialog;

    private void showAddDeviceDialog() {
        if (mAddDeviceDialog == null) {
            mAddDeviceDialog = new AddDeviceDialog(this);
        }
        mAddDeviceDialog.show();
        mAddDeviceDialog.mNameEdt.setText("");
        mAddDeviceDialog.mPWDEdt.setText("");
        mAddDeviceDialog.mAddBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String name = mAddDeviceDialog.mNameEdt.getText().toString();
                String pwd = mAddDeviceDialog.mPWDEdt.getText().toString();
                String number = mAddDeviceDialog.mNumberEdt.getText().toString();

                if (TextUtils.isEmpty(name) || TextUtils.isEmpty(number)) {
                    mAddDeviceDialog.dismiss();
                    return;
                }
                DemoDeviceRecyclerAdapter.ItemDemoInfo itemInfo = new DemoDeviceRecyclerAdapter.ItemDemoInfo();

                itemInfo.setStart(false);
                itemInfo.setDeviceId(name);
                itemInfo.setPassword("admin:" + pwd);
                itemInfo.setDeviceName(name);
                itemInfo.setConnectTip("未开始");
                itemInfo.setConnectCountAll(Integer.parseInt(number));

                if (mData == null) {
                    mData = new ArrayList<DemoDeviceRecyclerAdapter.ItemDemoInfo>();
                    mAdapter.setData(mData);
                }
                mCacheData.add(itemInfo);

                mData.add(itemInfo);


                mUserCache.setDeviceList(mGson.toJson(mCacheData));


                mAddDeviceDialog.dismiss();
                mAdapter.notifyItemInserted(mData.size() - 1);

            }
        });
    }

    private DeleteDialog mDeleteDialog;

    private void showDeleteDialog(final DemoDeviceRecyclerAdapter.ItemDemoInfo info, final int position) {
        if (mDeleteDialog == null) {
            mDeleteDialog = new DeleteDialog(this);
        }
        mDeleteDialog.show();

        mDeleteDialog.mContentTv.setText(info.getDeviceId());

        mDeleteDialog.mDeleteBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mDeleteDialog.dismiss();
                mCacheData.remove(info);
                mData.remove(info);
                mUserCache.setDeviceList(mGson.toJson(mCacheData));
                mAdapter.notifyItemRemoved(position);
            }
        });


    }


}
