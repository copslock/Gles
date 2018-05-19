package com.app.jagles.listener;

/**
 * Created by JuAn_Zhangsongzhou on 2017/9/1.
 */

public interface OnFrameOSDListener {
    void onFrameOSD(int width, int height, long frame, int length, long osdTime);

    void onOOBFrameOSD(int installMode, int scene, long osdTime);
}
