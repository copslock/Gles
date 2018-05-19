package com.app.jagles.listener;

/**
 * Created by Administrator on 2016/6/13.
 */
public interface ConnectStatusListener {
    /**
     * 链接状态的回调
     * @param code
     * @param index
     */
    public void onStatus( long handle,  int status_code,  int content_code,  int index, String msg);
}
