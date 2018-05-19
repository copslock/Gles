package com.app.jagles.activity.cache;

import android.content.Context;
import android.content.SharedPreferences;

/**
 * Created by zasko on 2018/1/12.
 */

public class UserCache {


    private Context mContext;

    private UserCache(Context context) {
        this.mContext = context.getApplicationContext();
    }


    private SharedPreferences mSharedPre;
    private SharedPreferences.Editor mEditor;

    public static UserCache mInstance;

    public static UserCache getInstance(Context context) {

        if (mInstance == null) {
            mInstance = new UserCache(context);
            mInstance.init();
        }
        return mInstance;
    }

    private static final String SHARE_NAME = "share_name";

    private void init() {
        mSharedPre = mContext.getSharedPreferences(SHARE_NAME, Context.MODE_PRIVATE);
        mEditor = mSharedPre.edit();
    }

    public static final String DEFAULT_STRING = "";
    public static final String KEY_DEVICE_LIST = "key_device_list";

    public void setDeviceList(String json) {

        mEditor.putString(KEY_DEVICE_LIST, json);
        mEditor.commit();

    }

    public String getDeviceList() {
        return mSharedPre.getString(KEY_DEVICE_LIST, DEFAULT_STRING);
    }


}
