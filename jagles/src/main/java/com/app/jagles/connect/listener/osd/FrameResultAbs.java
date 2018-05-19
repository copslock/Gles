package com.app.jagles.connect.listener.osd;

/**
 * Created by zasko on 2018/1/8.
 */

public abstract class FrameResultAbs implements FrameResultListener {
    @Override
    public void onFrameResultCallBack(int width, int height, long frame, int length, long time) {

    }

    @Override
    public void onOOBFrameResultCallBack(int installMode, int scene, long time) {

    }
}
