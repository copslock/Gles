package com.app.jagles.listener;

/**
 * Created by hongli on 2018/3/9.
 */

public interface OnGSenserDataListener {
    public void OnGSensorData(long timeStamp, double x,double y,double z);
}
