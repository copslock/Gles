package com.app.jagles.helper.remote;

import android.util.Log;

import com.app.jagles.activity.utils.protocol.DeviceProtocolUtil;
import com.app.jagles.activity.utils.protocol.SetDeviceProtocolUtil;
import com.app.jagles.connect.JAConnector;


/**
 * Created by zasko on 2018/2/10.
 */

public class RemoteHelper {

    private static final String TAG = "RemoteHelper";

    public static void getDeviceInfo(String connectKey, int index, String verify) {
        getDeviceInfo(connectKey, index, verify, "", "");
    }

    public static void getDeviceInfo(String connectKey, int index, String verify, String userName, String pwd) {
        String tmp = DeviceProtocolUtil.getString(DeviceProtocolUtil.TYPE_DEVICE_GET_INFO, verify, userName, pwd);
        JAConnector.sendDeviceData(connectKey, index, tmp);
    }

    public static void setDeviceInfo(String connectkey, int index, String json) {
        JAConnector.sendDeviceData(connectkey, index, json);
    }

    public static void getGWDeviceInfo(String connectKey, int index, String verify) {
        getGWDeviceInfo(connectKey, index, verify, "", "");
    }

    public static void getGWDeviceInfo(String connectKey, int index, String verify, String userName, String pwd) {
        String temp = DeviceProtocolUtil.getString(DeviceProtocolUtil.TYPE_GW_DEVICE_GET_INFO, verify, userName, pwd);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

    public static void setGWDeviceInfo(String connectKey, int index, String json) {
        JAConnector.sendDeviceData(connectKey, index, json);
    }

    public static void getChannelInfo(String connectKey, int index, String verify, int channel) {
        getChannelInfo(connectKey, index, verify, "", "", channel);
    }

    public static void getChannelInfo(String connectKey, int index, String verify, String userName, String pwd, int channel) {
        String temp = DeviceProtocolUtil.getString(DeviceProtocolUtil.TYPE_GW_DEVICE_CHANNEL_GET_INFO, verify, userName, pwd, channel);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

    public static void getEnableChannel(String connectKey, int index, String verify) {
        getEnableChannel(connectKey, index, verify, "", "");
    }

    public static void getEnableChannel(String connectKey, int index, String verify, String userName, String pwd) {
        String temp = DeviceProtocolUtil.getString(DeviceProtocolUtil.TYPE_GW_DEVICE_ENABLE_CHANNEL, verify, userName, pwd);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

    public static void getGWOneKeyUpgradeStatus(String connectKey, int index, String verify) {
        getGWOneKeyUpgradeStatus(connectKey, index, verify, "", "");
    }

    public static void getGWOneKeyUpgradeStatus(String connectKey, int index, String verify, String userName, String pwd) {
        String temp = DeviceProtocolUtil.getString(DeviceProtocolUtil.TYPE_GW_DEVICE_UPGRADE_STATUS, verify, userName, pwd);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

    public static void setDeleteGWChannel(String connectKey, int index, String verify, int channel) {
        setDeleteGWChannel(connectKey, index, verify, "", "", channel);
    }

    public static void setDeleteGWChannel(String connectKey, int index, String verify, String userName, String pwd, int channel) {
        String temp = SetDeviceProtocolUtil.getString(SetDeviceProtocolUtil.TYPE_GW_DEVICE_DELETE_CHANNEL, channel, verify, userName, pwd);
        Log.i(TAG, "setDeleteGWChannel: ------->" + temp);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

    public static void getTFCardInfo(String connectKey, int index, String verify) {
        getTFCardInfo(connectKey, index, verify, "", "");
    }

    public static void getTFCardInfo(String connectKey, int index, String verify, String userName, String pwd) {
        String temp = DeviceProtocolUtil.getString(DeviceProtocolUtil.TYPE_DEVICE_TF_CARD_INFO, verify, userName, pwd);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

    public static void setTFCardFormat(String connectKey, int index, String verify) {
        setTFCardFormat(connectKey, index, verify, "", "");
    }

    public static void setTFCardFormat(String connectKey, int index, String verify, String userName, String pwd) {
        String temp = SetDeviceProtocolUtil.getString(SetDeviceProtocolUtil.TYPE_DEVICE_RESET_TFCARD, verify, userName, pwd);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

    public static void getEnableChannelInfo(String connectKey, int index, String verify) {
        getEnableChannelInfo(connectKey, index, verify, "", "");
    }

    public static void getEnableChannelInfo(String connectKey, int index, String verify, String userName, String pwd) {
        String temp = DeviceProtocolUtil.getString(DeviceProtocolUtil.TYPE_GW_DEVICE_ENABLE_CHANNEL, verify, userName, pwd);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

    public static void setGWDeviceOneKeyUpgrade(String connectKey, int index, boolean enable, String verify) {
        setGWDeviceOneKeyUpgrade(connectKey, index, enable, verify, "", "");
    }

    public static void setGWDeviceOneKeyUpgrade(String connectKey, int index, boolean enable, String verify, String userName, String pwd) {
        String temp = SetDeviceProtocolUtil.getString(SetDeviceProtocolUtil.TYPE_GW_DEVICE_ONE_KEY_UPGRADE, enable, verify, userName, pwd);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

    public static void getRecordSchedule(String connectKey, int index, String verify) {
        getRecordSchedule(connectKey, index, verify, "", "");
    }

    public static void getRecordSchedule(String connectKey, int index, String verify, String userName, String pwd) {
        String temp = DeviceProtocolUtil.getString(DeviceProtocolUtil.TYPE_DEVICE_GET_RECORD_SCHEDULE
                , verify, userName, pwd);
        Log.i(TAG, "getRecordSchedule: -------->" + temp + "------> connectKey = " + connectKey);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

    public static void getLedPwdInfo(String connectKey, int index, String verify) {
        getLedPwdInfo(connectKey, index, verify, "", "");
    }

    public static void getLedPwdInfo(String connectKey, int index, String verify, String userName, String pwd) {
        String temp = DeviceProtocolUtil.getString(DeviceProtocolUtil.TYPE_DEVICE_LED_PWD_INFO, verify, userName, pwd);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

    public static void setDeviceFirmwareUpgrade(String connectKey, int index, String verify) {
        setDeviceFirmwareUpgrade(connectKey, index, verify, "", "");
    }

    public static void setDeviceFirmwareUpgrade(String connectKey, int index, String verify, String userName, String pwd) {
        String temp = SetDeviceProtocolUtil.getString(SetDeviceProtocolUtil.TYPE_DEVICE_FIRMWARE_UPGRADE, verify
                , userName, pwd);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

    public static void setChannelUpgrade(int channel, String connectKey, int index, String verify) {
        setChannelUpgrade(connectKey, index, channel, verify, "", "");
    }

    public static void setChannelUpgrade(String connectKey, int index, int channel, String verify, String userName, String pwd) {
        String temp = SetDeviceProtocolUtil.getString(SetDeviceProtocolUtil.TYPE_GW_DEVICE_CHANNEL_UPGRADE, channel
                , verify, userName, pwd);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

    public static void modifyDevicePwd(String connectKey, int index, String verify, String userVerify) {
        modifyDevicePwd(connectKey, index, verify, "", "", userVerify);
    }

    public static void modifyDevicePwd(String connectKey, int index, String verify, String userName, String pwd, String userVerify) {
        String temp = SetDeviceProtocolUtil.getString(SetDeviceProtocolUtil.TYPE_MODIFY_DEVICE_PWD
                , verify, userName, pwd, userVerify);
        Log.i(TAG, "modifyDevicePwd: ------->" + temp);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

    public static void setGWTimeSynchronous(String connectKey, int index, String verify, String utcTime, String localTime, String userName, String pwd) {
        String temp = SetDeviceProtocolUtil.getString(SetDeviceProtocolUtil.TYPE_GW_TIME_SYNC, verify, utcTime, localTime, userName, pwd);
        Log.i(TAG, "setGWTimeSynchronous: ------>" + temp);
        JAConnector.sendDeviceData(connectKey, index, temp);
    }

}
