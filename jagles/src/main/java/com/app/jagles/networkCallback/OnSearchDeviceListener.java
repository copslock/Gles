package com.app.jagles.networkCallback;

/**
 * Created by Administrator on 2016/10/13.
 */

public interface OnSearchDeviceListener {

    public void OnSearchDevice(String eseeId, String ip, int port, int max_ch, String devModel);
}
