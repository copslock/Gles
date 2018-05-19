package com.app.jagles.video;

/**
 * Created by Administrator on 2016/5/23.
 */
public class Parameter {

    //显示模式
    public static final int SCRN_NORMAL = 0;
    public static final int SCRN_HEMISPHERE = 1;
    public static final int SCRN_CYLINDER = 2;
    public static final int SCRN_EXPAND = 3;
    public static final int SCRN_UPDOWN = 4;
    public static final int SCRN_FOUR = 5;
    public static final int SCRN_VR = 6;
    public static final int SCRN_SPHERE = 7;
    public static final int SCRN_PANORAMA = 11;
    public static final int SCRN_PLANET = 14;
    public static final int SCRN_VR_SPHERE = 17;
    public static final int SCRN_STITCH2 = 18;
    public static final int SCRN_ONESCREEN = 0x101;
    public static final int SCRN_ONENORMAL = 0x102;
    public static final int SCRN_HEMISPHERE_VRSensor = -1;

    //分割屏幕模式，只有在SCRN_NORMAL模式下才有效
    public static final int SCRN_SPLIT_ONE = 0;
    public static final int SCRN_SPLIT_FOUR = 1;
    public static final int SCRN_SPLIT_SIX = 2;
    public static final int SCRN_SPLIT_EIGHT = 3;
    public static final int SCRN_SPLIT_NINE = 4;
    public static final int SCRN_SPLIT_THIRTEEN = 5;
    public static final int SCRN_SPLIT_SIXTEEN = 6;

    //网络的错误码
    //正在连接
    public static final int NET_STATUS_CONNECTING = 0;
    //连接成功
    public static final int NET_STATUS_CONNECTED = 1;
    //连接失败
    public static final int NET_STATUS_CONNECTFAIL = 2;
    //正在登陆
    public static final int NET_STATUS_LOGINING = 3;
    //登陆成功
    public static final int NET_STATUS_LOGINED = 4;
    //登陆失败
    public static final int NET_STATUS_LOGINFAIL = 5;
    //正在打开码流
    public static final int NET_STATUS_OPENING = 6;
    //打开码流失败
    public static final int NET_STATUS_OPENFAIL = 7;
    //断开连接
    public static final int NET_STATUS_DISCONNECTED = 8;
    //连接关闭
    public static final int NET_STATUS_CLOSED = 9;
    //用户名、密码验证错误
    public static final int NET_STATUS_AUTHOFAIL = 10;
    //连接超时
    public static final int NET_STATUS_TIMEOUT = 11;
    //连接关闭
    public static final int NET_STATUS_ACTIVECLOSED = 12;
    /*打开视频*/
    public static final int NET_STATUS_OPENCHANNEL = 13;

    /*关闭视频*/
    public static final int NET_STATUS_CLOSECHANNEL = 14;

    /*视频帧开始*/
    public static final int NET_STATUS_FIRST_FRAME = 15;


    /**
     * @param status
     * @return
     * @deprecated
     */
    public static String GetNetCodeString(int status) {
        String sRet = "";
        switch (status) {
            case NET_STATUS_CONNECTING:
                sRet = "开始连接";
                break;
            case NET_STATUS_CONNECTED:
                sRet = "连接成功";
                break;
            case NET_STATUS_CONNECTFAIL:
                sRet = "连接失败";
                break;
            case NET_STATUS_LOGINING:
                sRet = "开始登录";
                break;
            case NET_STATUS_LOGINED:
                sRet = "登录成功";
                break;
            case NET_STATUS_LOGINFAIL:
                sRet = "登录失败";
                break;
            case NET_STATUS_OPENING:
                sRet = "正在加载（等同于登录成功）";
                break;
            case NET_STATUS_OPENFAIL:
                sRet = "打开视频失败";
                break;
            case NET_STATUS_DISCONNECTED:
                sRet = "断开连接";
                break;
            case NET_STATUS_CLOSED:
                sRet = "连接被关闭";
                break;
            case NET_STATUS_AUTHOFAIL:
                sRet = "验证失败";
                break;
            case NET_STATUS_TIMEOUT:
                sRet = "连接超时";
                break;
            case NET_STATUS_ACTIVECLOSED:
                sRet = "连接关闭";
                break;
            case NET_STATUS_OPENCHANNEL:
                sRet = "打开视频";
                break;
            case NET_STATUS_CLOSECHANNEL:
                sRet = "关闭视频";
                break;
            case NET_STATUS_FIRST_FRAME:
                sRet = "打开视频成功";
                break;

            default:
                break;
        }
        return sRet;
    }

    public static class PtzOrder {
        public static final int PTZ_UP = 0;
        public static final int PTZ_DOWN = 1;
        public static final int PTZ_LEFT = 2;
        public static final int PTZ_RIGHT = 3;
        public static final int PTZ_AUTOPAN = 8;
        public static final int PTZ_IRIS_OPEN = 9;
        public static final int PTZ_IRIS_CLOSE = 10;
        public static final int PTZ_ZOOM_IN = 11;
        public static final int PTZ_ZOOM_OUT = 12;
        public static final int PTZ_FOCUS_FAR = 13;
        public static final int PTZ_FOCUS_NEAR = 14;
        public static final int PTZ_STOP = 15;
    }

}
