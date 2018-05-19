package com.app.jagles.smartlink;

/**
 * Created by Administrator on 2016/6/2.
 */
public class SmartLink {

    static{
        System.loadLibrary("smartlink");
    }

    public native int smartLink(String msg);
    public native void quitsmartLink();
    public native void smartLinkack();
}
