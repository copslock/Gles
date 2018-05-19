package com.app.jagles.activity.adapter;

import android.content.Context;
import android.graphics.Color;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.app.jagles.R;

import java.util.List;

/**
 * Created by zasko on 2018/1/9.
 */

public class DeviceRecyclerAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {

    public static final int TYPE_NORMAL = 1;
    public static final int TYPE_DEVICE = 2;


    private List<ItemInfo> mData;
    private Context mContext;

    private int mLastIndex = -1;

    public DeviceRecyclerAdapter(Context context) {
        mContext = context;
    }

    public void setData(List<ItemInfo> data) {
        mData = data;
    }

    @Override

    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        return new ItemHolder(LayoutInflater.from(mContext).inflate(R.layout.item_device_list_layout, parent, false));
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
        ItemHolder itemHolder = (ItemHolder) holder;

        ItemInfo info = mData.get(position);

        itemHolder.itemStateTv.setText(info.getState());
        itemHolder.itemNameTv.setText(info.getName());
        itemHolder.itemDeviceIDTv.setText(info.getDeviceId());

        if (info.isSelected()) {
            itemHolder.itemNameTv.setTextColor(mContext.getResources().getColor(R.color.colorAccent));
        } else {
            itemHolder.itemNameTv.setTextColor(Color.parseColor("#000000"));
        }

    }

    @Override
    public int getItemCount() {
        return mData == null ? 0 : mData.size();
    }

    public class ItemHolder extends RecyclerView.ViewHolder {


        public TextView itemNameTv;
        public TextView itemStateTv;
        public TextView itemDeviceIDTv;

        public LinearLayout mItemLl;

        public ItemHolder(View itemView) {
            super(itemView);

            itemNameTv = (TextView) itemView.findViewById(R.id.item_name_tv);
            itemStateTv = (TextView) itemView.findViewById(R.id.item_state_tv);
            mItemLl = (LinearLayout) itemView.findViewById(R.id.item_bg);
            itemDeviceIDTv = (TextView) itemView.findViewById(R.id.item_device_id_tv);


            mItemLl.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    int position = getAdapterPosition();
                    if (mItemClickListener != null) {
                        mItemClickListener.onItemClick(v, mData.get(position), position);
                    }
                    if (mLastIndex == -1) {
                        mLastIndex = position;
                        mData.get(mLastIndex).setSelected(true);
                        notifyItemChanged(mLastIndex);
                    } else if (mLastIndex == position) {
                        notifyItemChanged(mLastIndex);
                    } else {
                        mData.get(mLastIndex).setSelected(false);
                        notifyItemChanged(mLastIndex);
                        mData.get(position).setSelected(true);
                        notifyItemChanged(position);
                        mLastIndex = position;
                    }

                }
            });
        }
    }

    private OnItemClickListener mItemClickListener;

    public void setOnItemClickListener(OnItemClickListener listener) {
        this.mItemClickListener = listener;
    }

    public interface OnItemClickListener {
        void onItemClick(View view, ItemInfo info, int position);
    }

    public static class ItemInfo {


        private String name;
        private String state;

        private String deviceId;
        private String password;

        private boolean isSelected; /*是否是选中状态*/

        private boolean isSingle; /*是否是单通道设备*/


        private boolean isConnectOpen;


        private int channelCount;

        private int type;


        public boolean isConnectOpen() {
            return isConnectOpen;
        }

        public void setConnectOpen(boolean connectOpen) {
            isConnectOpen = connectOpen;
        }

        public int getChannelCount() {
            return channelCount;
        }

        public void setChannelCount(int channelCount) {
            this.channelCount = channelCount;
        }

        public int getType() {
            return type;
        }

        public void setType(int type) {
            this.type = type;
        }

        public boolean isSingle() {
            return isSingle;
        }

        public void setSingle(boolean single) {
            isSingle = single;
        }

        public boolean isSelected() {
            return isSelected;
        }

        public void setSelected(boolean selected) {
            isSelected = selected;
        }

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
        }

        public String getState() {
            return state;
        }

        public void setState(String state) {
            this.state = state;
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
    }
}
