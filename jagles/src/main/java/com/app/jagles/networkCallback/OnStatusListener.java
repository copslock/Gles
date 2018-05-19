package com.app.jagles.networkCallback;

/**
 * Created by Administrator on 2016/10/13.
 * <p>
 * {@link com.app.jagles.video.Parameter}
 */

public interface OnStatusListener {

    public void OnStatus(long handle, int status_code, int content_code, int index, String msg);
}
