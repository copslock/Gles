package com.app.jagles.connect.listener.osd;

/**
 * Created by zasko on 2018/1/8.
 */

public interface FrameResultListener {

    void onFrameResultCallBack(int width, int height, long frame, int length, long time);

    void onOOBFrameResultCallBack(int installMode, int scene, long time);
}
