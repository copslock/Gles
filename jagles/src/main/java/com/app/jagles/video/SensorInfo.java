package com.app.jagles.video;

/**
 * Created by huangqinghe on 2018/3/12.
 */

public class SensorInfo {

    private long timeStamp;
    private double x;
    private double y;
    private double z;

    public SensorInfo(long timeStamp, double x, double y, double z) {
        this.timeStamp = timeStamp;
        this.x = x;
        this.y = y;
        this.z = z;
    }

    public long getTimeStamp() {
        return timeStamp;
    }

    public void setTimeStamp(long timeStamp) {
        this.timeStamp = timeStamp;
    }

    public double getX() {
        return x;
    }

    public void setX(double x) {
        this.x = x;
    }

    public double getY() {
        return y;
    }

    public void setY(double y) {
        this.y = y;
    }

    public double getZ() {
        return z;
    }

    public void setZ(double z) {
        this.z = z;
    }
}
