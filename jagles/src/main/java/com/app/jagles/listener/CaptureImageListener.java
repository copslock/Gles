package com.app.jagles.listener;

/**
 * Created by Administrator on 2016/6/20.
 *
 * {@link com.app.jagles.video.Parameter}
 */
public interface CaptureImageListener {
    /**
     * 截图的回调
     * @param success
     * @param index
     */
    public void OnCaptureImage(int success, int index);
}
