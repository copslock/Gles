package com.app.jagles.activity.utils.protocol;


import android.text.TextUtils;
import android.util.Log;

import java.util.List;

/**
 * Created by JuAn_Zhangsongzhou on 2017/7/8.
 * <p>
 * 添加设备相关协议
 */

public class DeviceProtocolUtil {

    private static final String TAG = "DeviceProtocolUtil";

    /**
     * 局域网搜索 / 获取随机数
     * long requestID
     */
    public static final int TYPE_BROADCAST_DEVICE_BY_WIFI_SCAN = 100;

    /**
     * int requestID
     * String deviceID
     * String wifiSSID
     * String wifiPWD
     */
    public static final int TYPE_BROADCAST_SEND_WIFI_TO_DEVICE = 101;

    /**
     * int requestID
     * String deviceID
     * String wifiSSID
     * String wifiPWD
     * String accessToken 百度MediaServer的token
     * String streamName  百度MediaServer的streamName
     */
    public static final int TYPE_BROADCAST_SEND_WIFI_TO_DEVICE_BAIDU = 102;


    /**
     * 获取设备基本信息
     * String verify    Authorization的verify
     * String userName  Authorization的username
     * String password  Authorization的pwd
     */
    public static final int TYPE_DEVICE_GET_INFO = 103;


    /**
     * 获取灯泡设备信息
     * String verify    Authorization的verify
     * String userName  Authorization的username
     * String password  Authorization的pwd
     */
    public static final int TYPE_DEVICE_LED_PWD_INFO = 104;

    /**
     * 获取设备TFCard的信息
     * String verify    Authorization的verify
     * String userName  Authorization的username
     * String password  Authorization的pwd
     */
    public static final int TYPE_DEVICE_TF_CARD_INFO = 105;


    /**
     * 获取设备时区
     * int timeZone     时区
     * int utcTime      utc时间
     * String userName  Authorization的username
     * String userPWD   Authorization的pwd
     */
    public static final int TYPE_DEVICE_GET_TIME_ZONE = 106;


    /**
     * 获取设备附近Wi-Fi
     * String deviceID
     * int requestID
     */
    public static final int TYPE_DEVICE_GET_LOCAL_WIFI = 107;


    /**
     * 获取Gateway设备信息
     * String verify    Authorization的verify
     * String userName  Authorization的username
     * String password  Authorization的pwd
     */
    public static final int TYPE_GW_DEVICE_GET_INFO = 109;


    /**
     * 获取通道信息
     * String channel   所需要的通道
     * String verify    Authorization的verify
     * String userName  Authorization的username
     * String password  Authorization的pwd
     */
    public static final int TYPE_GW_DEVICE_CHANNEL_GET_INFO = 110;


    /**
     * 获取Gateway所有通道的电量信息
     * String verify    Authorization的verify
     * String userName  Authorization的username
     * String password  Authorization的pwd
     */
    public static final int TYPE_GW_DEVICE_CHANNEL_BATTERY = 111;


    /**
     * 获取Gateway当前支持的通道数与通道部分信息
     * String verify    Authorization的verify
     * String userName  Authorization的username
     * String password  Authorization的pwd
     */
    public static final int TYPE_GW_DEVICE_ENABLE_CHANNEL = 112;


    /**
     * 获取Gateway一键升级进度与状态
     * String verify    Authorization的verify
     * String userName  Authorization的username
     * String password  Authorization的pwd
     */
    public static final int TYPE_GW_DEVICE_UPGRADE_STATUS = 113;

    /**
     * 添加设备蓝牙协议
     * int requestID
     * String wifiSSID
     * String wifiPwd
     * String accessToken   用户的access_token
     */
    public static final int TYPE_BROADCAST_BULETOOTH_REQUEST_INFO = 114;


    /**
     * 获取录像日程协议
     * String verify    Authorization的verify
     * String userName  Authorization的username
     * String password  Authorization的pwd
     */
    public static final int TYPE_DEVICE_GET_RECORD_SCHEDULE = 115;


    /**
     * 获取协议
     *
     * @param code
     * @param objects
     * @return
     */
    public static String getString(int code, Object... objects) {
        String temp = "";
        switch (code) {
            case TYPE_BROADCAST_DEVICE_BY_WIFI_SCAN:
                temp = getWifiScanDeviceInfo((int) objects[0]);
                break;
            case TYPE_BROADCAST_SEND_WIFI_TO_DEVICE:
                temp = getSendWIFIToDevice((int) objects[0], (String) objects[1], (String) objects[2],
                        (String) objects[3]);
                break;
            case TYPE_BROADCAST_SEND_WIFI_TO_DEVICE_BAIDU:
                temp = getSendWIFIToDevice2((int) objects[0], (String) objects[1], (String) objects[2],
                        (String) objects[3], (String) objects[4], (String) objects[5]);
                break;
            case TYPE_DEVICE_GET_INFO:
                temp = getDeviceInfo((String) objects[0], (String) objects[1], (String) objects[2]);

                Log.d(TAG, "getString: ----->>" + temp + "\t1:" + objects[0] + "\t2:" + objects[1] + "\t3:" + objects[2]);
                break;
            case TYPE_DEVICE_LED_PWD_INFO:
                temp = getLedPwm((String) objects[0], (String) objects[1], (String) objects[2]);
                break;
            case TYPE_DEVICE_TF_CARD_INFO:
                temp = getTFCardInfo((String) objects[0], (String) objects[1], (String) objects[2]);
                break;
            case TYPE_DEVICE_GET_TIME_ZONE:
                temp = getDeviceTimeZone((int) objects[0], (int) objects[1], (String) objects[2],
                        (String) objects[3], (String) objects[4], (String) objects[5]);
                break;
            case TYPE_DEVICE_GET_LOCAL_WIFI:
                temp = getDeviceLocalWifiInfo((String) objects[0], (int) objects[1]);
                break;
            case TYPE_GW_DEVICE_GET_INFO:
                temp = gwDeviceInfo((String) objects[0], (String) objects[1], (String) objects[2]);
                break;
            case TYPE_GW_DEVICE_CHANNEL_GET_INFO:
                temp = getGWChannelInfo((String) objects[0], (String) objects[1], (String) objects[2], (int) objects[3]);
                break;
            case TYPE_GW_DEVICE_CHANNEL_BATTERY:
                temp = getChannelBattery((String) objects[0], (String) objects[1], (String) objects[2]);
                break;
            case TYPE_GW_DEVICE_ENABLE_CHANNEL:
                temp = getChannel((String) objects[0], (String) objects[1], (String) objects[2]);
                break;
            case TYPE_GW_DEVICE_UPGRADE_STATUS:
                temp = getGWOneKeyUpdateStatus((String) objects[0], (String) objects[1], (String) objects[2]);
                break;
            case TYPE_BROADCAST_BULETOOTH_REQUEST_INFO:
                temp = getRequestPutInfo((int) objects[0], (String) objects[1], (String) objects[2],
                        (String) objects[3]);
                break;

            case TYPE_DEVICE_GET_RECORD_SCHEDULE:
                temp = getRecordScheduleInfo((String) objects[0], (String) objects[1], (String) objects[2]);
                break;
        }

        if (TextUtils.isEmpty(temp)) {
            return "gg";
        }

        //TODO:加密不加密
        boolean isLock = false;
        if (isLock) {

        }

        return temp;
    }


    /**
     * 获取wifi局域网搜索设备协议
     *
     * @param requestId
     * @return
     * @hide
     */
    private static String getWifiScanDeviceInfo(int requestId) {
        return "{\"fromApp\":true,\"requestID\":" + requestId + ",\"tokenID\":\"1473433878\",\"command\":\"discovery\"}";
    }

    private static String getSendWIFIToDevice(int requestId, String deviceId, String wifiSSID, String pwd) {
        return "{\n" +
                "    \"fromApp\":true,\n" +
                "    \"requestID\":" + requestId + ",\n" +
                "    \"deviceID\":\"" + deviceId + "\",\n" +
                "    \"tokenID\":" + 754276439 + ",\n" +
                "    \"command\":\"setup\",\n" +
                "    \"Content\":{\n" +
                "        \"Ethernet\":{\n" +
                "            \"dhcp\":true,\n" +
                "            \"Wireless\":{\n" +
                "                \"securityMode\":\"WEP\",\n" +
                "                \"ssid\":\"" + wifiSSID + "\",\n" +
                "                \"password\":\"" + pwd + "\"" +
                "            }\n" +
                "        }\n" +
                "    }\n" +
                "}";
    }

    private static String getSendWIFIToDevice2(int requestID, String deviceId, String wifiSSID,
                                               String pwd, String accessToken, String devName) {
        return "{\n" +
                "    \"fromApp\":true,\n" +
                "    \"requestID\":" + requestID + ",\n" +
                "    \"deviceID\":\"" + deviceId + "\",\n" +
                "    \"tokenID\":" + 754276439 + ",\n" +
                "    \"command\":\"setup\",\n" +
                "    \"Content\":{\n" +
                "        \"Ethernet\":{\n" +
                "            \"dhcp\":true,\n" +
                "            \"Wireless\":{\n" +
                "                \"securityMode\":\"WEP\",\n" +
                "                \"ssid\":\"" + wifiSSID + "\",\n" +
                "                \"password\":\"" + pwd + "\"" +
                "            }\n" +
                "        },\n" +
                "        \"BaiduMediaServer\":{\n" +
                "            \"accessToken\":\"" + accessToken + "\",\n" +
                "            \"streamName\":\"" + devName + "\"\n" +
                "        }\n" +
                "    }\n" +
                "}";
    }

    /**
     * @param verify
     * @param userName
     * @param pwd
     * @return
     */
    private static String getDeviceInfo(String verify, String userName, String pwd) {
        return "{\n" +
                "   \"Version\":\"1.0.0\",\n" +
                "   \"Method\":\"get\",\n" +
                "   \"IPCam\":{\n" +
                "           \"DeviceInfo\":{},\n" +
                "           \"ModeSetting\":{},\n" +
                "           \"AlarmSetting\":{\n" +
                "                   \"MotionDetection\":{},\n" +
                "                   \"MessagePushSchedule\":[]\n" +
                "                   },\n" +
                "           \"SystemOperation\":{\n" +
                "                   \"TimeSync\":{},\n" +
                "                   \"DaylightSavingTime\":{" +
                "                     \"Week\":[{},{}]" +
                "                     }" +
                "                   },\n" +
                "           \"PromptSounds\":{},\n" +
                "           \"RecordManager\":{},\n" +
                "            \"ledPwm\":{" +
                "                   \"channelInfo\":[]\n" +
                "                   },\n" +
                "           \"powerLineFrequencyMode\":0\n" +
                "           },\n" +
                "   \"Authorization\":{\n" +
                "           \"Verify\":\"" + verify + "\",\n" +
                "           \"username\":\"" + userName + "\",\n" +
                "           \"password\":\"" + pwd + "\"\n" +
                "           }\n" +
                "}";
    }


    /**
     * @param verify
     * @param username
     * @param pwd
     * @return
     */
    private static String getLedPwm(String verify, String username, String pwd) {
        return "{\n" +
                "   \"Version\":\"1.0.0\",\n" +
                "    \"Method\":\"get\",\n" +
                "    \"IPCam\":{\n" +
                "        \"ledPwm\":{ \n" +
                "           \"project\":" + 0 + ",\n" +
                "           \"product\":\"\",\n" +
                "           \"channelCount\":" + 0 + ",\n" +
                "           \"infraredLampSwitch\":" + 0 + ",\n" +
                "           \"switch\":" + 0 + ",\n" +
                "           \"channelInfo\":[]\n" +
                "       }\n" +
                "    },\n" +
                "    \"Authorization\":{\n" +
                "        \"Verify\":\"" + verify + "\",\n" +
                "        \"username\":\"" + username + "\",\n" +
                "        \"password\":\"" + pwd + "\"\n" +
                "    }\n" +
                "}";
    }


    /**
     * 获取TFCard信息
     *
     * @param verify
     * @return
     */
    private static String getTFCardInfo(String verify, String userName, String userPwd) {
        return "{\n" +
                "  \"Version\":\"1.0.0\",\n" +
                "  \"Method\":\"get\",\n" +
                "  \"IPCam\":{\n" +
                //                "           \"DeviceInfo\":{},\n" +
                "           \"TfcardManager\":{\n" +
                //                "                   \"TFcard_recordSchedule\":[]\n" +
                "                   }\n" +
                "               },\n" +
                "  \"Authorization\":{\n" +
                "           \"Verify\":\"" + verify + "\",\n" +
                "           \"username\":\"" + userName + "\",\n" +
                "           \"password\":\"" + userPwd + "\"\n" +
                "           }\n" +
                "}";
    }


    private static String getRecordScheduleInfo(String verify, String userName, String userPwd) {
        return "{\n" +
                "  \"Version\":\"1.0.0\",\n" +
                "  \"Method\":\"get\",\n" +
                "  \"IPCam\":{\n" +
                "           \"TfcardManager\":{\n" +
                "                   \"TFcard_recordSchedule\":[]\n" +
                "                   }\n" +
                "               },\n" +
                "  \"Authorization\":{\n" +
                "           \"Verify\":\"" + verify + "\",\n" +
                "           \"username\":\"" + userName + "\",\n" +
                "           \"password\":\"" + userPwd + "\"\n" +
                "           }\n" +
                "}";
    }


    /**
     * 获取时区协议
     *
     * @param timeZone
     * @param userName
     * @param pwd
     * @return
     */
    private static String getDeviceTimeZone(int timeZone, int utcTime, String verify, String userName, String pwd, String newVerifyString) {
        String data = "{\n" +
                "   \"Version\": \"1.0.0\",\n" +
                "   \"Method\": \"set\",\n" +
                "   \"IPCam\": {\n" +
                "        \"SystemOperation\": {\n" +
                "                \"TimeSync\": {\n" +
                "                        \"LocalTime\":" + utcTime + ",\n" +
                "                        \"UTCTime\":" + utcTime + ",\n" +
                "                        \"TimeZone\":" + timeZone + "\n" +
                "                        }\n" +
                "                }\n" +
                "        },\n" +
                "   \"Authorization\": {\n" +
                "        \"Verify\":\"" + verify + "\",\n" +
                "        \"username\":\"" + userName + "\",\n" +
                "        \"password\":\"" + pwd + "\"\n" +
                "        }\n";

        if (!TextUtils.isEmpty(newVerifyString)) {
            data += "   ,\"UserManager\":{\n" +
                    "           \"Method\":\"modify\",\n" +
                    "           \"Verify\":\"" + newVerifyString + "\",\n" +
                    "           \"username\":\"\",\n" +
                    "           \"password\":\"\"\n" +
                    "           }\n";
        }
        data += "}";
        return data;
    }


    /**
     * 获取设备周围附近的wifi
     *
     * @param deviceId
     * @param requestId
     * @return
     */
    private static String getDeviceLocalWifiInfo(String deviceId, int requestId) {

        return "{\n" +
                "  \"jsonrpc\" : \"2.0\",\n" +
                "  \"method\" : \"getNearbyAPs\",\n" +
                "  \"params\" : {\n" +
                "    \"version\":\"1.0\",\n" +
                "    \"deviceID\":\"" + deviceId + "\"\n" +
                "  },\n" +
                "  \"id\" :" + requestId + "\n" +
                "}";
    }


    /**
     * 获取GY设备
     *
     * @param verify
     * @param username
     * @param password
     * @return
     */
    private static String NVRDeviceInfo(String verify, String username, String password) {
        return "{\n" +
                "   \"Version\":\"1.0.0\",\n" +
                "   \"Method\":\"get\",\n" +
                "   \"IPCam\":{\n" +
                "          \"DeviceInfo\":{},\n" +
                "          \"ModeSetting\":{},\n" +
                "          \"AlarmSetting\":{\n" +
                "                  \"MotionDetection\":{},\n" +
                "                  \"MessagePushSchedule\":[]\n" +
                "                  },\n" +
                "          \"SystemOperation\":{\n" +
                "                  \"TimeSync\":{},\n" +
                "                  \"DaylightSavingTime\":{\n" +
                "                           \"Week\":[{},{}]\n" +
                "                            },\n" +
                "                  \"Upgrade\":{\n" +
                "                           \"Enabled\":\"\"\n" +
                "                            }" +
                "                   },\n" +
                "          \"PromptSounds\":{},\n" +
                "          \"TfcardManager\":{\n" +
                "                   \"TFcard_recordSchedule\":[]\n" +
                "                   },\n" +
                "          \"RecordManager\":{},\n" +
                "          \"ChannelManager\":{},\n" +
                "          \"ChannelInfo\":[],\n" +
                "          \"AlarmManger\":{}\n" +
                "    },\n" +
                "    \"Authorization\":{\n" +
                "          \"Verify\":\"" + verify + "\",\n" +
                "          \"username\":\"" + username + "\",\n" +
                "          \"password\":\"" + password + "\",\n" +
                "    }\n" +
                "}";
    }


    private static String gwDeviceInfo(String verify, String userName, String userPwd) {
        return "{\n" +
                "   \"Versioin\":\"1.0.3\",\n" +
                "   \"Method\":\"get\",\n" +
                "   \"IPCam\":{\n" +
                "           \"DeviceInfo\":{},\n" +
                "           \"SystemOperation\":{\n" +
                "                       \"TimeSync\":{}\n" +
                "                       },\n" +
                "           \"TfcardManager\":{\n" +
                "                       \"Status\":\"\",\n" +
                "                       \"TotalSpacesize\":\"\",\n" +
                "                       \"LeaveSpacesize\":\"\"\n" +
                "                       },\n" +
                "           \"ChannelManager\":{\n" +
                "                       \"enableChannel\":\"\"\n" +
                "                       },\n" +
                "          \"ChannelInfo\":[]\n" +
                "       },\n" +
                "   \"Authorization\":{\n" +
                "           \"Verify\":\"" + verify + "\",\n" +
                "           \"username\":\"" + userName + "\",\n" +
                "           \"password\":\"" + userPwd + "\"\n" +
                "       }\n" +
                "}";
    }


    //    /**
    //     * 获取通道信息
    //     *
    //     * @param channel
    //     * @return
    //     */
    //    public static String getChannelInfo(String channel, String verify, String userName, String userPwd) {
    //        return "{\n" +
    //                "   \"Version\":\"1.0.2\",\n" +
    //                "   \"Method\":\"get\",\n" +
    //                "   \"IPCam\":{\n" +
    //                "           \"DeviceInfo\":{},\n" +
    //                "           \"ModeSetting\":{},\n" +
    //                "           \"ChannelManager\":{\n" +
    //                "                   \"enableChannel\":\"\",\n" +
    //                "                   \"ChannelList\":\"" + channel + "\",\n" +
    //                "                   \"Operation\":\"GetChannel\",\n" +
    //                "                   \"OperationProperty\":{\n" +
    //                "                                  \"opt\":[]\n" +
    //                "                                 }\n" +
    //                "                     },\n" +
    //                "           \"AlarmSetting\":{\n" +
    //                "                     \"MotionDetection\": {\n" +
    //                "                             \"Enabled\":\"\",\n" +
    //                "                             \"SensitivityLevel\":\"\"\n" +
    //                "                            }\n" +
    //                "                     },\n" +
    //                "          \"PromptSounds\":{},\n" +
    //                "          \"RecordManager\":{},\n" +
    //                "           \"ChannelInfo\":[{\n" +
    //                "                    \"Channel\":\"\",\n" +
    //                "                    \"Model\":\"\",\n" +
    //                "                    \"Version\":\"\"\n" +
    //                "                    }],\n" +
    //                "           \"AlarmManager\":{\n" +
    //                "                     \"Enable\":\"\",\n" +
    //                "                     \"Duration\":\"\"\n" +
    //                "                     }\n" +
    //                "   },\n" +
    //                "    \"Authorization\":{\n" +
    //                "          \"Verify\":\"" + verify + "\",\n" +
    //                "          \"username\":\"" + userName + "\",\n" +
    //                "          \"password\":\"" + userPwd + "\"\n" +
    //                "    }\n" +
    //                "}";
    //    }

    /**
     * 获取Gateway设备通道信息
     *
     * @param verify
     * @param channel
     * @return
     */
    private static String getGWChannelInfo(String verify, String userName, String userPwd, int channel) {
        return "{\n" +
                "   \"Version\":\"1.0.0\",\n" +
                "   \"Method\":\"get\",\n" +
                "   \"IPCam\":{\n" +
                "           \"DeviceInfo\":{}," +
                "           \"AlarmSetting\":{\n" +
                "                   \"MotionDetection\":{\n" +
                "                           \"SensitivityLevel\":\"\"\n" +
                "                           },\n" +
                "                   \"MessagePushEnabled\":\"\"\n" +
                "               },\n" +
                "           \"RecordManager\":{\n" +
                "                   \"Mode\":\"\"\n" +
                "               },\n" +
                "   \"ChannelManager\":{\n" +
                "           \"ChannelList\":\"" + channel + "\",\n" +
                "           \"Operation\":\"GetChannel\"\n" +
                "               },\n" +
                "   \"ChannelInfo\":[{\n" +
                "           \"Model\":\"\",\n" +
                "           \"Version\":\"\",\n" +
                "           \"OdmNum\":\"\",\n" +
                "           \"SerialNum\":\"\",\n" +
                "           \"FWMagic\":\"\"\n" +
                "               }],\n" +
                "   \"ChannelStatus\":[{" +
                "           \"IsLastest\":false" +
                "               }],\n" +
                "   \"DoorbellManager\":{\n" +
                "       }" +
                "   },\n" +
                "   \"Authorization\":{\n" +
                "           \"Verify\":\"" + verify + "\",\n" +
                "           \"username\":\"" + userName + "\",\n" +
                "           \"password\":\"" + userPwd + "\"\n" +
                "               }\n" +
                "}";
    }


    /**
     * 获取通道电池信息
     *
     * @param verify
     * @return
     */
    private static String getChannelBattery(String verify, String userName, String userPwd) {
        return "{\n" +
                "   \"Version\":\"1.0.0\",\n" +
                "   \"Method\":\"get\",\n" +
                "   \"IPCam\":{\n" +
                "           \"ChannelStatus\":[{\n" +
                "               }],\n" +
                "           \"ChannelInfo\":[{\n" +
                "                    \"Channel\":\"\",\n" +
                "                    \"Model\":\"\",\n" +
                "                    \"Version\":\"\"\n" +
                "                    }],\n" +
                "           \"ChannelManager\":{\n" +
                "                       \"enableChannel\":\"\"\n" +
                "                       }\n" +
                "       },\n" +
                "   \"Authorization\":{\n" +
                "       \"Verify\":\"" + verify + "\",\n" +
                "       \"username\":\"" + userName + "\",\n" +
                "       \"password\":\"" + userPwd + "\"\n" +
                "       }\n" +
                "}";
    }

    /**
     * 获取支持的通道与通道名称
     *
     * @param verify
     * @param username
     * @param password
     * @return
     */
    private static String getChannel(String verify, String username, String password) {
        return "{\n" +
                "   \"Version\":\"1.0.0\",\n" +
                "   \"Method\":\"get\",\n" +
                "   \"IPCam\":{\n" +
                "           \"ChannelInfo\":[{\n" +
                "                    \"Channel\":\"\",\n" +
                "                    \"Model\":\"\",\n" +
                "                    \"Version\":\"\"\n" +
                "                    }],\n" +
                "           \"ChannelManager\":{\n" +
                "                       \"maxChannel\":\"\",\n" +
                "                       \"enableChannel\":\"\"\n" +
                "                       }\n" +
                "       },\n" +
                "   \"Authorization\":{\n" +
                "       \"Verify\":\"" + verify + "\",\n" +
                "       \"username\":\"" + username + "\",\n" +
                "       \"password\":\"" + password + "\"\n" +
                "       }\n" +
                "}";
    }


    /**
     * 获取Gateway升级状态
     *
     * @param verify
     * @return
     */
    private static String getGWOneKeyUpdateStatus(String verify, String userName, String userPwd) {
        return "{\n" +
                "   \"Version\":\"1.0.0\",\n" +
                "   \"Method\":\"get\",\n" +
                "   \"IPCam\":{\n" +
                "               \"SystemOperation\":{\n" +
                "                       \"UpgradeStatus\":{\n" +
                "                       }\n" +
                "               }\n" +
                "   },\n" +
                "   \"Authorization\":{\n" +
                "       \"Verify\":\"" + verify + "\",\n" +
                "       \"username\":\"" + userName + "\",\n" +
                "       \"password\":\"" + userPwd + "\"\n" +
                "       }\n" +
                "}";
    }

    /**
     * 添加设备蓝牙
     *
     * @param requestId
     * @param SSID
     * @param password
     * @param accessToken
     * @return
     */
    private static String getRequestPutInfo(int requestId, String SSID, String password, String accessToken) {
        return "{" +
                "\"ver\":1," +
                "\"id\":" + requestId + "," +
                "\"method\":\"put_info\"," +
                "\"params\":{" +
                "	\"mode\":\"WEP\"," +
                "	\"ssid\":\"" + SSID + "\"," +
                "	\"pass\":\"" + password + "\"," +
                "	\"token\":\"" + accessToken + "\"" +
                "	}" +
                "}";
    }


    /**
     * 独占设备添加服务器字段
     *
     * @param requestId
     * @param deviceId
     * @param accessToken
     * @param devName
     * @return
     */
    private static String getDeviceAddToServer(int requestId, String deviceId, String accessToken, String devName) {
        return "{\n" +
                "    \"fromApp\":true,\n" +
                "    \"requestID\":" + requestId + ",\n" +
                "    \"deviceID\":\"" + deviceId + "\",\n" +
                "    \"command\":\"binding\",\n" +
                "    \"Content\":{\n" +
                //                "        \"nonce\":\"" + nonce + "\",\n" +
                "        \"monopolyMode\":{\n" +
                "            \"accessToken\":\"" + accessToken + "\",\n" +
                "            \"devName\":\"" + devName + "\"\n" +
                "        }\n" +
                "    }\n" +
                "}";
    }


    public static class ResponseLocalDeviceWifiClass {
        private String jsonrpc;
        private ResultClass result;
        private int id;

        public String getJsonrpc() {
            return jsonrpc;
        }

        public void setJsonrpc(String jsonrpc) {
            this.jsonrpc = jsonrpc;
        }

        public ResultClass getResult() {
            return result;
        }

        public void setResult(ResultClass result) {
            this.result = result;
        }

        public int getId() {
            return id;
        }

        public void setId(int id) {
            this.id = id;
        }

    }

    public static class ResultClass {
        private List<APsClass> APs;

        public List<APsClass> getAPs() {
            return APs;
        }

        public void setAPs(List<APsClass> APs) {
            this.APs = APs;
        }
    }

    public static class APsClass {

        private String ssid;
        private int rssi;

        public String getSsid() {
            return ssid;
        }

        public void setSsid(String ssid) {
            this.ssid = ssid;
        }

        public int getRssi() {
            return rssi;
        }

        public void setRssi(int rssi) {
            this.rssi = rssi;
        }
    }

    /**
     *
     */
    public static class ResponseRandomClass {
        private boolean fromApp;
        private String version;
        private int requestID;
        private String deviceID;
        private int errorcode;
        private String errormesg;

        private ContentCalss Content;

        public ContentCalss getContent() {
            return Content;
        }

        public void setContent(ContentCalss content) {
            Content = content;
        }

        public boolean isFromApp() {
            return fromApp;
        }

        public void setFromApp(boolean fromApp) {
            this.fromApp = fromApp;
        }

        public String getVersion() {
            return version;
        }

        public void setVersion(String version) {
            this.version = version;
        }

        public int getRequestID() {
            return requestID;
        }

        public void setRequestID(int requestID) {
            this.requestID = requestID;
        }

        public String getDeviceID() {
            return deviceID;
        }

        public void setDeviceID(String deviceID) {
            this.deviceID = deviceID;
        }

        public int getErrorcode() {
            return errorcode;
        }

        public void setErrorcode(int errorcode) {
            this.errorcode = errorcode;
        }

        public String getErrormesg() {
            return errormesg;
        }

        public void setErrormesg(String errormesg) {
            this.errormesg = errormesg;
        }
    }

    public static class ContentCalss {
        private String nonce;

        public String getNonce() {
            return nonce;
        }

        public void setNonce(String nonce) {
            this.nonce = nonce;
        }
    }

}
