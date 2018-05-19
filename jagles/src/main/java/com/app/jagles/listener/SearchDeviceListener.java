package com.app.jagles.listener;

/**
 * Created by Administrator on 2016/6/1.
 */
public interface SearchDeviceListener {

    /**
     * 局域网搜索设备的回调
     * @param eseeId
     * @param ip
     * @param port
     * @param max_ch
     * @param devModel
     */
    public void OnSearchDevice(String eseeId, String ip, int port, int max_ch, String devModel);
}
