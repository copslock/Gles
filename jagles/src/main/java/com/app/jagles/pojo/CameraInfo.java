package com.app.jagles.pojo;

import com.google.gson.annotations.SerializedName;

import java.io.Serializable;

/**
 * Created by edison on 2018/2/28.
 */

public class CameraInfo implements Serializable{
    @SerializedName("camera_id")
    private long cameraId;
    private int channel;
    private String name;
    private String remark;
    @SerializedName("cloud_status")
    private int cloudStatus;
    private long endtime;
    @SerializedName("cloud_id")
    private int cloudId;

    //UI
    private int connectParameter;
    private String deviceCachePathKey;
    private String deviceCachePath;

    public String getDeviceCachePath() {
        return deviceCachePath;
    }

    public void setDeviceCachePath(String deviceCachePath) {
        this.deviceCachePath = deviceCachePath;
    }

    public String getDeviceCachePathKey() {
        return deviceCachePathKey;
    }

    public void setDeviceCachePathKey(String deviceCachePathKey) {
        this.deviceCachePathKey = deviceCachePathKey;
    }

    public int getConnectParameter() {
        return connectParameter;
    }

    public void setConnectParameter(int connectParameter) {
        this.connectParameter = connectParameter;
    }

    public long getCamera_id() {
        return cameraId;
    }

    public void setCamera_id(long camera_id) {
        this.cameraId = camera_id;
    }

    public int getChannel() {
        return channel;
    }

    public void setChannel(int channel) {
        this.channel = channel;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getRemark() {
        return remark;
    }

    public void setRemark(String remark) {
        this.remark = remark;
    }

    public int getCloudStatus() {
        return cloudStatus;
    }

    public void setCloudStatus(int cloudStatus) {
        this.cloudStatus = cloudStatus;
    }


    public long getEndtime() {
        return endtime;
    }

    public void setEndtime(long endtime) {
        this.endtime = endtime;
    }

    public int getCloudId() {
        return cloudId;
    }

    public void setCloudId(int cloudId) {
        this.cloudId = cloudId;
    }
}
