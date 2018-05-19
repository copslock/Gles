package com.app.jagles.activity.adapter;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.app.jagles.R;

import java.util.List;

/**
 * Created by zasko on 2018/1/15.
 */

public class ScreenRecyclerAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {

    private Context mContext;
    private List<ScreenInfo> mData;


    public void setData(List<ScreenInfo> data) {
        if (data != null) {
            mData = data;

            notifyDataSetChanged();
        }
    }

    public ScreenRecyclerAdapter(Context context) {
        mContext = context;
    }


    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        return new ScreenHolder(LayoutInflater.from(mContext).inflate(R.layout.item_screen_layout, parent, false));
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder holder, int position) {
        ScreenHolder screenHolder = (ScreenHolder) holder;
        ScreenInfo info = mData.get(position);

        screenHolder.itemNameTv.setText(info.getName());

    }

    @Override
    public int getItemCount() {

        return mData == null ? 0 : mData.size();
    }

    public class ScreenHolder extends RecyclerView.ViewHolder {


        public TextView itemNameTv;
        public LinearLayout itemBg;

        public ScreenHolder(View itemView) {

            super(itemView);
            itemBg = (LinearLayout) itemView.findViewById(R.id.item_bg);
            itemNameTv = (TextView) itemView.findViewById(R.id.item_name_tv);

            itemBg.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (mItemClickListener != null) {
                        mItemClickListener.onItemClick(v, mData.get(getAdapterPosition()), getAdapterPosition());
                    }

                }
            });
        }
    }

    private OnItemClickListener mItemClickListener;

    public void setOnItemClickListener(OnItemClickListener listener) {
        mItemClickListener = listener;
    }

    public interface OnItemClickListener {
        void onItemClick(View view, ScreenInfo info, int position);
    }

    public static class ScreenInfo {

        private String name;
        private int value;

        public int getValue() {
            return value;
        }

        public void setValue(int value) {
            this.value = value;
        }

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
        }
    }
}
