package com.app.jagles.activity.utils.protocol;

import android.text.TextUtils;

/**
 * Created by edison on 2018/1/6.
 */

public class SetDeviceProtocolUtil {

    /**
     * 设置设备
     * String verify            Authorization的verify
     * String userName          Authorization的username
     * String userPWD           Authorization的pwd
     * boolean isPush           设备推送开关
     * boolean isSound          设备语音开关
     * String soundType         设备语音类型  "chinese", "english", "french", "german", "russian", "korean", "japanese", "spanish", "arabic", "italian", "portuguese"
     * String definition        录像清晰度    "auto", "fluency", "BD", "HD"
     * boolean isMotionRecord   移动录像开关
     * String recordList        设备录像日程列表  如:"TFcard_recordSchedule" :[{"Weekday" : "0,1,2,3,4,5,6","BeginTime" : "00:00:30","EndTime" : "23:59:30","ID" : "0"}]
     */
    public static final int TYPE_DEVICE_SET_SETTING = 200;

    /**
     * 设置时区和设置设备密码
     * int timeZone 时区
     * int utcTime  utc时间
     * String verify  Authorization的verify
     * String userName  Authorization的username
     * String password  Authorization的pwd
     * String userMangerVerify  设置设备密码需要用到UserManager的Verify
     */
    public static final int TYPE_DEVICE_SET_TIME_ZONE_SET_DEVICE_PWD = 201;


    /**
     * 设置设备进行固件升级
     * String verify    Authorization的verify
     * String userName  Authorization的username
     * String password  Authorization的pwd
     */
    public static final int TYPE_DEVICE_FIRMWARE_UPGRADE = 202;

    /**
     * 重置/格式化内存卡
     * String verify    Authorization的verify
     * String userName  Authorization的username
     * String password  Authorization的pwd
     */
    public static final int TYPE_DEVICE_RESET_TFCARD = 203;


    /**
     * Gateway通道升级
     * String channel   所需要的通道
     * String verify    Authorization的verify
     * String userName  Authorization的username
     * String password  Authorization的pwd
     */
    public static final int TYPE_GW_DEVICE_CHANNEL_UPGRADE = 204;


    /**
     * Gateway通道删除
     * String channel   所需要的通道
     * String verify    Authorization的verify
     * String userName  Authorization的username
     * String password  Authorization的pwd
     */
    public static final int TYPE_GW_DEVICE_DELETE_CHANNEL = 205;

    /**
     * Gateway一键升级
     * boolean isEnabled    true:升级Gateway与所支持通道   false:单独升级Gateway
     * String verify        Authorization的verify
     * String userName      Authorization的username
     * String password      Authorization的pwd
     */
    public static final int TYPE_GW_DEVICE_ONE_KEY_UPGRADE = 206;

    /**
     * 修复设备密码
     * String verify
     * String userName
     * String userPwd
     * String userVerify  UserManager的Verify
     */
    public static final int TYPE_MODIFY_DEVICE_PWD = 207;

    /**
     * GW时间同步
     * String verify
     * String userName
     * String userPwd
     * String localTime
     * String utcTime
     */
    public static final int TYPE_GW_TIME_SYNC = 208;

    public static String getString(int code, Object... objects) {
        String temp = "";
        switch (code) {
            case TYPE_DEVICE_SET_SETTING:
                temp = setDevice((String) objects[0], (String) objects[1], (String) objects[2],
                        (boolean) objects[3], (boolean) objects[4], (String) objects[5],
                        (String) objects[6], (boolean) objects[7], (String) objects[8]);
                break;
            case TYPE_DEVICE_SET_TIME_ZONE_SET_DEVICE_PWD:
                temp = getTimeZoneSetWithPassword((int) objects[0], (int) objects[1], (String) objects[2],
                        (String) objects[3], (String) objects[4], (String) objects[5]);
                break;
            case TYPE_DEVICE_FIRMWARE_UPGRADE:
                temp = setFirmwareUpgrade((String) objects[0], (String) objects[1], (String) objects[2]);
                break;
            case TYPE_DEVICE_RESET_TFCARD:
                temp = getResetDeviceSDCard((String) objects[0], (String) objects[1], (String) objects[2]);
                break;
            case TYPE_GW_DEVICE_CHANNEL_UPGRADE:
                temp = updateChannel((int) objects[0], (String) objects[1], (String) objects[2],
                        (String) objects[3]);
                break;
            case TYPE_GW_DEVICE_DELETE_CHANNEL:
                temp = deleteChannel((int) objects[0], (String) objects[1], (String) objects[2],
                        (String) objects[3]);
                break;
            case TYPE_GW_DEVICE_ONE_KEY_UPGRADE:
                temp = setGWOneKeyUpdate((boolean) objects[0], (String) objects[1], (String) objects[2],
                        (String) objects[3]);
                break;
            case TYPE_MODIFY_DEVICE_PWD:
                temp = modifyDevicePwd((String) objects[0], (String) objects[1], (String) objects[2]
                        , (String) objects[3]);
                break;
            case TYPE_GW_TIME_SYNC:
                temp = timeSyncGW((String) objects[0], (String) objects[1], (String) objects[2]
                        , (String) objects[3], (String) objects[4]);
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


    private static String setDevice(String verify, String userName, String userPWD, boolean isPush, boolean isSound, String soundType, String definition, boolean isMotionRecord, String recordList) {
        return "{\n" +
                "   \"Version\":\"1.0.0\",\n" +
                "   \"Method\":\"set\",\n" +
                "   \"IPCam\":{\n" +
                "               \"ModeSetting\":{\n" +
                "                           \"Definition\":\"" + definition + "\"\n" +
                "                               },\n" +
                "               \"AlarmSetting\":{\n" +
                "                   \"MotionDetection\":{\n" +
                "                                   \"Enabled\":" + isMotionRecord + "\n" +
                "                                 },\n" +
                "                   \"MessagePushEnabled\":" + isPush + "\n" +
                "                      },\n" +
                "               \"PromptSounds\":{\n" +
                "                      \"Enabled\":" + isSound + ",\n" +
                "                      \"Type\":\"" + soundType + "\"\n" +
                "                      },\n" +
                "               \"TfcardManager\":{\n" +
                "                      \"TFcard_recordSchedule\":" + recordList + "\n" +
                "                      }\n" +
                "            },\n" +
                "   \"Authorization\":{\n" +
                "               \"Verify\":\"" + verify + "\",\n" +
                "               \"username\":\"" + userName + "\",\n" +
                "               \"password\":\"" + userPWD + "\"" + "\n" +
                "               }\n" +
                "";
    }


    /**
     * 设时区时设设备密码
     *
     * @param timeZone
     * @param verify
     * @param userName
     * @param pwd
     * @param newVerifyString
     * @return
     */
    private static String getTimeZoneSetWithPassword(int timeZone, int utcTime, String verify, String userName, String pwd, String newVerifyString) {
        return "{\n" +
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
                "        },\n" +
                "   \"UserManager\":{\n" +
                "           \"Method\":\"modify\",\n" +
                "           \"Verify\":\"" + newVerifyString + "\",\n" +
                "           \"username\":\"\",\n" +
                "           \"password\":\"\"\n" +
                "           }\n" +
                "}";
    }

    /**
     * 固件升级
     *
     * @param userName
     * @param pwd
     * @return
     */
    private static String setFirmwareUpgrade(String verify, String userName, String pwd) {
        return "{\n" +
                "\t\"Version\": \"1.0.0\",\n" +
                "\t\"Method\":\"set\",\n" +
                "\t\"IPCam\": {\n" +
                "\t        \"SystemOperation\": {\n" +
                "\t                \"Upgrade\": {\n" +
                "\t                        \"Enabled\":true,\n" +
                "\t                        \"Status\":\"\",\n" +
                "\t                        \"error\":\"\"\n" +
                "\t                        }\n" +
                "\t                }\n" +
                "\t        },\n" +
                "\t\"Authorization\": {\n" +
                "\t        \"Verify\":\"" + verify + "\",\n" +
                "\t        \"username\":\"" + userName + "\",\n" +
                "\t        \"password\":\"" + pwd + "\"\n" +
                "\t        }\n" +
                "\n" +
                "}";
    }


    /**
     * 获取设备重置SDCard的协议
     *
     * @param userName
     * @param password
     * @return
     */
    private static String getResetDeviceSDCard(String verify, String userName, String password) {

        return "{\n" +
                "    \"Version\":\"1.0.0\",\n" +
                "    \"Method\":\"set\",\n" +
                "    \"IPCam\":{\n" +
                "        \"TfcardManager\":{\n" +
                "            \"Operation\":\"format\"\n" +
                "        }\n" +
                "    },\n" +
                "    \"Authorization\":{\n" +
                "        \"Verify\":\"" + verify + "\",\n" +
                "        \"username\":\"" + userName + "\",\n" +
                "        \"password\":\"" + password + "\"\n" +
                "    }\n" +
                "}";
    }


    /**
     * Gateway通道升级
     *
     * @param channel
     * @param verify
     * @return
     */
    private static String updateChannel(int channel, String verify, String userName, String userPwd) {
        return "{\n" +
                "       \"Version\":\"1.0.2\",\n" +
                "       \"Method\":\"set\",\n" +
                "       \"IPCam\":{\n" +
                "                   \"SystemOperation\":{\n" +
                "                                           \"Upgrade\":{\n" +
                "                                                       \"Enabled\":true,\n" +
                "                                                       \"Status\":\"\",\n" +
                "                                                       \"error\":\"\"\n" +
                "                                                       }\n" +
                "                                       },\n" +
                "                   \"ChannelManager\":{\n" +
                "                               \"enableChannel\":\"\",\n" +
                "                               \"ChannelList\":\"" + channel + "\",\n" +
                "                               \"Operation\":\"SetChannel\",\n" +
                "                               \"OperationProperty\":{\n" +
                "                                                       \"opt\":[]\n" +
                "                                                       }\n" +
                "                                      }\n" +
                "                  },\n" +
                "    \"Authorization\":{\n" +
                "          \"Verify\":\"" + verify + "\",\n" +
                "          \"username\":\"" + userName + "" + "\",\n" +
                "          \"password\":\"" + userPwd + "" + "\"\n" +
                "    }\n" +
                "}";
    }

    /**
     * Gateway通道删除
     *
     * @param channel
     * @param verify
     * @return
     */
    private static String deleteChannel(int channel, String verify, String userName, String userPwd) {
        return "{\n" +
                "       \"Method\":\"set\",\n" +
                "       \"IPCam\":{\n" +
                "                   \"ChannelManager\":{\n" +
                "                                       \"ChannelList\":\"" + channel + "\",\n" +
                "                                       \"Operation\":\"DelChannel\"\n" +
                "                                       }\n" +
                "                 },\n" +
                "       \"Authorization\":{\n" +
                "          \"Verify\":\"" + verify + "\",\n" +
                "          \"username\":\"" + userName + "\",\n" +
                "          \"password\":\"" + userPwd + "\"\n" +
                "    }\n" +
                "}";
    }

    /**
     * Gateway一键更新
     *
     * @param isEnabled 为true则同时升级IPC，否则只升级GATEWAY
     * @param verify
     * @return
     */
    private static String setGWOneKeyUpdate(boolean isEnabled, String verify, String userName, String userPwd) {
        return "{\n" +
                "   \"Method\":\"set\",\n" +
                "   \"IPCam\":{\n" +
                "           \"SystemOperation\":{\n" +
                "                   \"Upgrade\":{\n" +
                "                           \"Enabled\":true,\n" +
                "                           \"EnabledUpgradeChannel\":" + isEnabled + "\n" +
                "                   }\n" +
                "           }\n" +
                "   },\n" +
                "   \"Authorization\":{\n" +
                "       \"Verify\":\"" + verify + "\",\n" +
                "       \"username\":\"" + userName + "\",\n" +
                "       \"password\":\"" + userPwd + "\"\n" +
                "       }\n" +
                "}";
    }

    /**
     * 修改设备密码
     *
     * @param verify
     * @param userName
     * @param userPwd
     * @param userVerify
     * @return
     */
    private static String modifyDevicePwd(String verify, String userName, String userPwd, String userVerify) {
        return "{\n" +
                "   \"Method\":\"set\",\n" +
                "   \"IPCam\":{},\n" +
                "   \"Authorization\":{\n" +
                "       \"Verify\":\"" + verify + "\",\n" +
                "       \"username\":\"" + userName + "\",\n" +
                "       \"password\":\"" + userPwd + "\"\n" +
                "       },\n" +
                "   \"UserManager\":{\n" +
                "       \"Method\":\"modify\",\n" +
                "       \"Verify\":\"" + userVerify + "\",\n" +
                "       \"username\":\"\",\n" +
                "       \"password\":\"\"\n" +
                "   }\n" +
                "}";
    }

    private static String timeSyncGW(String verify, String utcTime, String localTime, String userName, String pwd) {
        return "{\n" +
                "   \"Method\":\"set\",\n" +
                "   \" Version\":\"1.0.0\",\n" +
                "   \"IPCam\": {\n" +
                "        \"SystemOperation\": {\n" +
                "                \"TimeSync\": {\n" +
                "                        \"LocalTime\":" + localTime + ",\n" +
                "                        \"UTCTime\":" + utcTime + ",\n" +
                "                        }\n" +
                "                }\n" +
                "        },\n" +
                "   \"Authorization\":{\n" +
                "       \"Verify\":\"" + verify + "\",\n" +
                "       \"username\":\"" + userName + "\",\n" +
                "       \"password\":\"" + pwd + "\"\n" +
                "       }\n" +
                "}";

    }

}
