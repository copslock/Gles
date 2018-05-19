package com.app.jagles.activity.adapter;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.app.jagles.R;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by zasko on 2018/1/11.
 */

public class DemoDeviceRecyclerAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {

    private Context mContext;


    private List<ItemDemoInfo> mData = new ArrayList<>();

    public void setData(List<ItemDemoInfo> data) {
        mData = data;
    }
//    public void up


    public DemoDeviceRecyclerAdapter(Context context) {
        mContext = context;
    }

    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        return new ItemDemoHolder(LayoutInflater.from(mContext).inflate((R.layout.item_demo_device_layout), parent, false));
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
        ItemDemoHolder itemDemoHolder = (ItemDemoHolder) holder;
        ItemDemoInfo info = mData.get(position);


        if (info.isStart()) {
            itemDemoHolder.itemDeviceIdTv.setTextColor(mContext.getResources().getColor(R.color.colorPrimaryDark));
        } else {
            itemDemoHolder.itemDeviceIdTv.setTextColor(mContext.getResources().getColor(R.color.jagles_gray));
        }

        itemDemoHolder.itemDeviceIdTv.setText(info.getDeviceName());

        itemDemoHolder.itemConnectStateTv.setText(info.getConnectTip());
        itemDemoHolder.itemConnectCountTv.setText(info.getConnectCount() + "/" + info.getConnectCountAll());
    }

    @Override
    public int getItemCount() {
        return mData == null ? 0 : mData.size();
    }


    public class ItemDemoHolder extends RecyclerView.ViewHolder {

        private LinearLayout itemBgLl;

        private TextView itemDeviceIdTv;

        private TextView itemConnectStateTv;

        private TextView itemConnectCountTv;

        public ItemDemoHolder(final View itemView) {
            super(itemView);

            itemBgLl = (LinearLayout) itemView.findViewById(R.id.item_bg);
            itemDeviceIdTv = (TextView) itemView.findViewById(R.id.item_device_id_tv);
            itemConnectCountTv = (TextView) itemView.findViewById(R.id.item_device_connect_time_tv);
            itemConnectStateTv = (TextView) itemView.findViewById(R.id.item_connect_state_tv);

            itemBgLl.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (mItemListener != null) {
                        mItemListener.onItemClick(v, mData.get(getAdapterPosition()), getAdapterPosition());

                    }
                }
            });
        }
    }

    private OnItemClickListener mItemListener;

    public void setOnItemClickListener(OnItemClickListener listener) {
        mItemListener = listener;

    }

    public interface OnItemClickListener {
        void onItemClick(View view, ItemDemoInfo info, int position);
    }

    public static class ItemDemoInfo {
        private String deviceId; /*连接ID*/
        private String password; /*连接密码*/
        private int connectState; /*连接状态*/
        private int connectCount = 0; /*当前连接次数*/
        private int connectCountAll = 10; /*连接总数*/

        private long startTime;  /*记录一次周期所消耗的时间*/

        private long connectSuccessTime; /*记录连接成功所消耗的时间*/
        private long disconnectTime;  /*出图到设备断开所消耗的时间*/

        private long openFrameTime; /*连接成功到出图所消耗的时间*/

        private long connectFailTime; /*连接失败所需要的时间*/

        private String connectTip;

        public String getConnectTip() {
            return connectTip;
        }

        public void setConnectTip(String connectTip) {
            this.connectTip = connectTip;
        }

        public long getConnectFailTime() {
            return connectFailTime;
        }

        public void setConnectFailTime(long connectFailTime) {
            this.connectFailTime = connectFailTime;
        }

        private boolean isStart = false;

        public boolean isStart() {
            return isStart;
        }

        public void setStart(boolean start) {
            isStart = start;
        }

        public long getDisconnectTime() {
            return disconnectTime;
        }

        public void setDisconnectTime(long disconnectTime) {
            this.disconnectTime = disconnectTime;
        }

        public long getConnectSuccessTime() {
            return connectSuccessTime;
        }

        public void setConnectSuccessTime(long connectSuccessTime) {
            this.connectSuccessTime = connectSuccessTime;
        }

        public long getOpenFrameTime() {
            return openFrameTime;
        }

        public void setOpenFrameTime(long openFrameTime) {
            this.openFrameTime = openFrameTime;
        }

        public long getStartTime() {
            return startTime;
        }

        public void setStartTime(long startTime) {
            this.startTime = startTime;
        }

        private String deviceName;

        public String getDeviceName() {
            return deviceName;
        }

        public void setDeviceName(String deviceName) {
            this.deviceName = deviceName;
        }

        public String getDeviceId() {
            return deviceId;
        }

        public void setDeviceId(String deviceId) {
            this.deviceId = deviceId;
        }

        public String getPassword() {
            return password;
        }

        public void setPassword(String password) {
            this.password = password;
        }

        public int getConnectState() {
            return connectState;
        }

        public void setConnectState(int connectState) {
            this.connectState = connectState;
        }

        public int getConnectCount() {
            return connectCount;
        }

        public void setConnectCount(int connectCount) {
            this.connectCount = connectCount;
        }

        public int getConnectCountAll() {
            return connectCountAll;
        }

        public void setConnectCountAll(int connectCountAll) {
            this.connectCountAll = connectCountAll;
        }
    }

}
