package com.app.jagles.listener;

/**
 * Created by lee on 2017/5/16.
 */

public interface HWDecoderCaptureLinstener {
    /**
     * 截图的回调
     *
     * @param success
     * @param index
     */
    public void OnCaptureImage(int success, int index);

}
