package com.app.jagles.pojo;

import com.google.gson.annotations.SerializedName;

import java.util.List;

/**
 * Created by edison on 2018/3/21.
 */

public class LightInfo {
    private String Method;
    private IPCamClass IPCam;
    private AuthorizationClass Authorization;

    public String getMethod() {
        return Method;
    }

    public void setMethod(String method) {
        Method = method;
    }

    public IPCamClass getIPCam() {
        return IPCam;
    }

    public void setIPCam(IPCamClass IPCam) {
        this.IPCam = IPCam;
    }

    public AuthorizationClass getAuthorization() {
        return Authorization;
    }

    public void setAuthorization(AuthorizationClass authorization) {
        Authorization = authorization;
    }

    public static class IPCamClass {
        private LedPwmClass ledPwm;

        public LedPwmClass getLedPwm() {
            return ledPwm;
        }

        public void setLedPwm(LedPwmClass ledPwm) {
            this.ledPwm = ledPwm;
        }
    }

    public static class LedPwmClass {
        private int channelCount;
        @SerializedName("switch")
        private int newSwitch;
        private List<ChannelInfo> channelInfo;
        private int project;
        private String product;
        private int infraredLampSwitch;

        public int getChannelCount() {
            return channelCount;
        }

        public void setChannelCount(int channelCount) {
            this.channelCount = channelCount;
        }

        public int getNewSwitch() {
            return newSwitch;
        }

        public void setNewSwitch(int newSwitch) {
            this.newSwitch = newSwitch;
        }

        public List<ChannelInfo> getChannelInfo() {
            return channelInfo;
        }

        public void setChannelInfo(List<ChannelInfo> channelInfo) {
            this.channelInfo = channelInfo;
        }

        public int getProject() {
            return project;
        }

        public void setProject(int project) {
            this.project = project;
        }

        public String getProduct() {
            return product;
        }

        public void setProduct(String product) {
            this.product = product;
        }

        public int getInfraredLampSwitch() {
            return infraredLampSwitch;
        }

        public void setInfraredLampSwitch(int infraredLampSwitch) {
            this.infraredLampSwitch = infraredLampSwitch;
        }
    }

    public static class ChannelInfo {
        private int type;
        private int num;
        private int channel;

        public int getType() {
            return type;
        }

        public void setType(int type) {
            this.type = type;
        }

        public int getNum() {
            return num;
        }

        public void setNum(int num) {
            this.num = num;
        }

        public int getChannel() {
            return channel;
        }

        public void setChannel(int channel) {
            this.channel = channel;
        }
    }

    public static class AuthorizationClass {

        private String Verify;
        private String username;
        private String password;

        public String getVerify() {
            return Verify;
        }

        public void setVerify(String verify) {
            Verify = verify;
        }

        public String getUsername() {
            return username;
        }

        public void setUsername(String username) {
            this.username = username;
        }

        public String getPassword() {
            return password;
        }

        public void setPassword(String password) {
            this.password = password;
        }
    }
}
