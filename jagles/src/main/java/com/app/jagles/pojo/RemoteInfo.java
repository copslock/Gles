package com.app.jagles.pojo;

import com.google.gson.annotations.SerializedName;

import java.io.Serializable;
import java.util.List;

/**
 * Created by zasko on 2018/2/10.
 */

public class RemoteInfo implements Serializable {


    private String Version;
    private String Method;
    private IPCamClass IPCam;
    private AuthorizationClass Authorization;
    private UserManagerClass UserManager;

    public AuthorizationClass getAuthorization() {
        return Authorization;
    }

    public void setAuthorization(AuthorizationClass authorization) {
        Authorization = authorization;
    }

    public String getVersion() {
        return Version;
    }

    public void setVersion(String version) {
        Version = version;
    }

    public String getMethod() {
        return Method;
    }

    public void setMethod(String method) {
        Method = method;
    }

    public UserManagerClass getUserManager() {
        return UserManager;
    }

    public void setUserManager(UserManagerClass userManager) {
        UserManager = userManager;
    }

    public IPCamClass getIPCam() {
        return IPCam;
    }

    public void setIPCam(IPCamClass IPCam) {
        this.IPCam = IPCam;
    }

    public static class IPCamClass {

        private DeviceInfoClass DeviceInfo;
        private ModeSettingClass ModeSetting;
        private AlarmSettingClass AlarmSetting;
        private PromptSoundsClass PromptSounds;
        private TfcardManagerClass TfcardManager;
        private RecordManagerClass RecordManager;
        private SystemOperationClass SystemOperation;
        private ChannelManagerClass ChannelManager;
        private List<ChannelStatusClass> ChannelStatus;
        private List<ChannelInfoClass> ChannelInfo;
        private LedPwm ledPwm;
        private DoorbellManagerClass DoorbellManager;
        private int powerLineFrequencyMode;

        public PromptSoundsClass getPromptSounds() {
            return PromptSounds;
        }

        public void setPromptSounds(PromptSoundsClass promptSounds) {
            PromptSounds = promptSounds;
        }

        public LedPwm getLedPwm() {
            return ledPwm;
        }

        public void setLedPwm(LedPwm ledPwm) {
            this.ledPwm = ledPwm;
        }

        public SystemOperationClass getSystemOperation() {
            return SystemOperation;
        }

        public void setSystemOperation(SystemOperationClass systemOperation) {
            SystemOperation = systemOperation;
        }

        public RecordManagerClass getRecordManager() {
            return RecordManager;
        }

        public void setRecordManager(RecordManagerClass recordManager) {
            RecordManager = recordManager;
        }

        public TfcardManagerClass getTfcardManager() {
            return TfcardManager;
        }

        public void setTfcardManager(TfcardManagerClass tfcardManager) {
            TfcardManager = tfcardManager;
        }

        public AlarmSettingClass getAlarmSetting() {
            return AlarmSetting;
        }

        public void setAlarmSetting(AlarmSettingClass alarmSetting) {
            AlarmSetting = alarmSetting;
        }

        public ModeSettingClass getModeSetting() {
            return ModeSetting;
        }

        public void setModeSetting(ModeSettingClass modeSetting) {
            ModeSetting = modeSetting;
        }

        public DeviceInfoClass getDeviceInfo() {
            return DeviceInfo;
        }

        public void setDeviceInfo(DeviceInfoClass deviceInfo) {
            DeviceInfo = deviceInfo;
        }

        public ChannelManagerClass getChannelManager() {
            return ChannelManager;
        }

        public void setChannelManager(ChannelManagerClass channelManager) {
            ChannelManager = channelManager;
        }

        public List<ChannelInfoClass> getChannelInfo() {
            return ChannelInfo;
        }

        public void setChannelInfo(List<ChannelInfoClass> channelInfo) {
            ChannelInfo = channelInfo;
        }

        public List<ChannelStatusClass> getChannelStatus() {
            return ChannelStatus;
        }

        public void setChannelStatus(List<ChannelStatusClass> channelStatus) {
            ChannelStatus = channelStatus;
        }

        public DoorbellManagerClass getDoorbellManager() {
            return DoorbellManager;
        }

        public void setDoorbellManager(DoorbellManagerClass doorbellManager) {
            DoorbellManager = doorbellManager;
        }

        public int getPowerLineFrequencyMode() {
            return powerLineFrequencyMode;
        }

        public void setPowerLineFrequencyMode(int powerLineFrequencyMode) {
            this.powerLineFrequencyMode = powerLineFrequencyMode;
        }
    }


    public static class DeviceInfoClass {
        private String OEMNumber;
        private String FWVersion;
        private String Model;
        private String ID;
        private String FWMagic;

        public String getOEMNumber() {
            return OEMNumber;
        }

        public void setOEMNumber(String OEMNumber) {
            this.OEMNumber = OEMNumber;
        }

        public String getFWVersion() {
            return FWVersion;
        }

        public void setFWVersion(String FWVersion) {
            this.FWVersion = FWVersion;
        }

        public String getModel() {
            return Model;
        }

        public void setModel(String model) {
            Model = model;
        }

        public String getID() {
            return ID;
        }

        public void setID(String ID) {
            this.ID = ID;
        }

        public String getFWMagic() {
            return FWMagic;
        }

        public void setFWMagic(String FWMagic) {
            this.FWMagic = FWMagic;
        }
    }

    public static class ModeSettingClass {
        private boolean AudioEnabled;
        private boolean ledType;
        private int ledBrightness;
        private int ledColour;
        private String SceneMode;
        private String IRCutFilterMode;
        private String ConvenientSetting;
        private String imageStyle;
        private String Definition;

        public boolean isAudioEnabled() {
            return AudioEnabled;
        }

        public void setAudioEnabled(boolean audioEnabled) {
            AudioEnabled = audioEnabled;
        }

        public boolean isLedType() {
            return ledType;
        }

        public void setLedType(boolean ledType) {
            this.ledType = ledType;
        }

        public int getLedBrightness() {
            return ledBrightness;
        }

        public void setLedBrightness(int ledBrightness) {
            this.ledBrightness = ledBrightness;
        }

        public int getLedColour() {
            return ledColour;
        }

        public void setLedColour(int ledColour) {
            this.ledColour = ledColour;
        }

        public String getSceneMode() {
            return SceneMode;
        }

        public void setSceneMode(String sceneMode) {
            SceneMode = sceneMode;
        }

        public String getIRCutFilterMode() {
            return IRCutFilterMode;
        }

        public void setIRCutFilterMode(String IRCutFilterMode) {
            this.IRCutFilterMode = IRCutFilterMode;
        }

        public String getConvenientSetting() {
            return ConvenientSetting;
        }

        public void setConvenientSetting(String convenientSetting) {
            ConvenientSetting = convenientSetting;
        }

        public String getImageStyle() {
            return imageStyle;
        }

        public void setImageStyle(String imageStyle) {
            this.imageStyle = imageStyle;
        }

        public String getDefinition() {
            return Definition;
        }

        public void setDefinition(String definition) {
            Definition = definition;
        }
    }

    public static class AlarmSettingClass {
        private MotionDetectionClass MotionDetection;
        private boolean MessagePushEnabled;
        private boolean ScheduleSupport;
        private List<MessagePushScheduleClass> MessagePushSchedule;

        public boolean isMessagePushEnabled() {
            return MessagePushEnabled;
        }

        public void setMessagePushEnabled(boolean messagePushEnabled) {
            MessagePushEnabled = messagePushEnabled;
        }

        public boolean isScheduleSupport() {
            return ScheduleSupport;
        }

        public void setScheduleSupport(boolean scheduleSupport) {
            ScheduleSupport = scheduleSupport;
        }

        public MotionDetectionClass getMotionDetection() {
            return MotionDetection;
        }

        public void setMotionDetection(MotionDetectionClass motionDetection) {
            MotionDetection = motionDetection;
        }

        public List<MessagePushScheduleClass> getMessagePushSchedule() {
            return MessagePushSchedule;
        }

        public void setMessagePushSchedule(List<MessagePushScheduleClass> messagePushSchedule) {
            MessagePushSchedule = messagePushSchedule;
        }
    }

    public static class MessagePushScheduleClass {
        private String Weekday;
        private String BeginTime;
        private String EndTime;
        private String ID;

        public String getWeekday() {
            return Weekday;
        }

        public void setWeekday(String weekday) {
            Weekday = weekday;
        }

        public String getBeginTime() {
            return BeginTime;
        }

        public void setBeginTime(String beginTime) {
            BeginTime = beginTime;
        }

        public String getEndTime() {
            return EndTime;
        }

        public void setEndTime(String endTime) {
            EndTime = endTime;
        }

        public String getID() {
            return ID;
        }

        public void setID(String ID) {
            this.ID = ID;
        }
    }

    public static class PromptSoundsClass {

        private boolean Enabled;
        private String Type;
        private List<String> TypeOption;


        public boolean isEnabled() {
            return Enabled;
        }

        public void setEnabled(boolean enabled) {
            Enabled = enabled;
        }

        public String getType() {
            return Type;
        }

        public void setType(String type) {
            Type = type;
        }

        public List<String> getTypeOption() {
            return TypeOption;
        }

        public void setTypeOption(List<String> typeOption) {
            TypeOption = typeOption;
        }
    }


    public static class MotionDetectionClass {
        private boolean Enabled;
        private String SensitivityLevel;

        public boolean isEnabled() {
            return Enabled;
        }

        public void setEnabled(boolean enabled) {
            Enabled = enabled;
        }

        public String getSensitivityLevel() {
            return SensitivityLevel;
        }

        public void setSensitivityLevel(String sensitivityLevel) {
            SensitivityLevel = sensitivityLevel;
        }
    }

    public static class RecordManagerClass {
        private String Mode;

        public String getMode() {
            return Mode;
        }

        public void setMode(String mode) {
            Mode = mode;
        }
    }

    public static class TfcardManagerClass {

        private String TotalSpacesize;
        private String LeaveSpacesize;
        private String Status;
        private List<RecordScheduleClass> TFcard_recordSchedule;

        public String getStatus() {
            return Status;
        }

        public void setStatus(String status) {
            Status = status;
        }

        public String getTotalSpacesize() {
            return TotalSpacesize;
        }

        public void setTotalSpacesize(String totalSpacesize) {
            TotalSpacesize = totalSpacesize;
        }

        public String getLeaveSpacesize() {
            return LeaveSpacesize;
        }

        public void setLeaveSpacesize(String leaveSpacesize) {
            LeaveSpacesize = leaveSpacesize;
        }

        public List<RecordScheduleClass> getTFcard_recordSchedule() {
            return TFcard_recordSchedule;
        }

        public void setTFcard_recordSchedule(List<RecordScheduleClass> TFcard_recordSchedule) {
            this.TFcard_recordSchedule = TFcard_recordSchedule;
        }
    }


    public static class RecordScheduleClass implements Serializable {

        private String Weekday;
        private String BeginTime;
        private String EndTime;
        private String ID;

        public String getWeekday() {
            return Weekday;
        }

        public void setWeekday(String weekday) {
            Weekday = weekday;
        }

        public String getBeginTime() {
            return BeginTime;
        }

        public void setBeginTime(String beginTime) {
            BeginTime = beginTime;
        }

        public String getEndTime() {
            return EndTime;
        }

        public void setEndTime(String endTime) {
            EndTime = endTime;
        }

        public String getID() {
            return ID;
        }

        public void setID(String ID) {
            this.ID = ID;
        }
    }


    public static class SystemOperationClass {
        private TimeSyncClass TimeSync;
        private DaylightSavingTimeClass DaylightSavingTime;
        private UpgradeClass Upgrade;
        private UpgradeStatusClass UpgradeStatus;

        public TimeSyncClass getTimeSync() {
            return TimeSync;
        }

        public void setTimeSync(TimeSyncClass timeSync) {
            TimeSync = timeSync;
        }

        public DaylightSavingTimeClass getDaylightSavingTime() {
            return DaylightSavingTime;
        }

        public void setDaylightSavingTime(DaylightSavingTimeClass daylightSavingTime) {
            DaylightSavingTime = daylightSavingTime;
        }

        public UpgradeClass getUpgrade() {
            return Upgrade;
        }

        public void setUpgrade(UpgradeClass upgrade) {
            Upgrade = upgrade;
        }

        public UpgradeStatusClass getUpgradeStatus() {
            return UpgradeStatus;
        }

        public void setUpgradeStatus(UpgradeStatusClass upgradeStatus) {
            UpgradeStatus = upgradeStatus;
        }
    }

    public static class ChannelManagerClass {
        private int maxChannel;
        private int enableChannel;
        private String ChannelList;
        private String Operation;
        private OperationPropertyClass OperationProperty;

        public int getMaxChannel() {
            return maxChannel;
        }

        public void setMaxChannel(int maxChannel) {
            this.maxChannel = maxChannel;
        }

        public int getEnableChannel() {
            return enableChannel;
        }

        public void setEnableChannel(int enableChannel) {
            this.enableChannel = enableChannel;
        }

        public String getChannelList() {
            return ChannelList;
        }

        public void setChannelList(String channelList) {
            ChannelList = channelList;
        }

        public String getOperation() {
            return Operation;
        }

        public void setOperation(String operation) {
            Operation = operation;
        }

        public OperationPropertyClass getOperationProperty() {
            return OperationProperty;
        }

        public void setOperationProperty(OperationPropertyClass operationProperty) {
            OperationProperty = operationProperty;
        }
    }

    public static class ChannelInfoClass {
        private int Channel;
        private boolean Enabled;
        private String Model;
        private String Version;
        private String OdmNum;
        private String SerialNum;
        private String FWMagic;
        private int Signal;
        private int Battery;
        private String DeviceType;

        public int getChannel() {
            return Channel;
        }

        public void setChannel(int channel) {
            this.Channel = channel;
        }

        public boolean isEnabled() {
            return Enabled;
        }

        public void setEnabled(boolean enabled) {
            Enabled = enabled;
        }

        public String getModel() {
            return Model;
        }

        public void setModel(String model) {
            Model = model;
        }

        public String getVersion() {
            return Version;
        }

        public void setVersion(String version) {
            Version = version;
        }

        public String getOdmNum() {
            return OdmNum;
        }

        public void setOdmNum(String odmNum) {
            OdmNum = odmNum;
        }

        public String getSerialNum() {
            return SerialNum;
        }

        public void setSerialNum(String serialNum) {
            SerialNum = serialNum;
        }

        public int getSignal() {
            return Signal;
        }

        public void setSignal(int signal) {
            Signal = signal;
        }

        public int getBattery() {
            return Battery;
        }

        public void setBattery(int battery) {
            Battery = battery;
        }

        public String getFWMagic() {
            return FWMagic;
        }

        public void setFWMagic(String FWMagic) {
            this.FWMagic = FWMagic;
        }

        public String getDeviceType() {
            return DeviceType;
        }

        public void setDeviceType(String deviceType) {
            DeviceType = deviceType;
        }
    }

    public static class ChannelStatusClass {
        private int Channel;
        private int Signal;
        private int Battery;
        private String BatteryStatus;
        private boolean PowerCablePlugIn;
        private String RecordStatus;
        private boolean IsLastest;

        public int getChannel() {
            return Channel;
        }

        public void setChannel(int channel) {
            Channel = channel;
        }

        public int getSignal() {
            return Signal;
        }

        public void setSignal(int signal) {
            Signal = signal;
        }

        public int getBattery() {
            return Battery;
        }

        public void setBattery(int battery) {
            Battery = battery;
        }

        public String getRecordStatus() {
            return RecordStatus;
        }

        public void setRecordStatus(String recordStatus) {
            RecordStatus = recordStatus;
        }

        public boolean isLastest() {
            return IsLastest;
        }

        public void setLastest(boolean lastest) {
            IsLastest = lastest;
        }

        public String getBatteryStatus() {
            return BatteryStatus;
        }

        public void setBatteryStatus(String batteryStatus) {
            BatteryStatus = batteryStatus;
        }

        public boolean isPowerCablePlugIn() {
            return PowerCablePlugIn;
        }

        public void setPowerCablePlugIn(boolean powerCablePlugIn) {
            PowerCablePlugIn = powerCablePlugIn;
        }
    }

    public static class TimeSyncClass {
        private String UTCTime;
        private int TimeZone;
        private String LocalTime;

        public String getUTCTime() {
            return UTCTime;
        }

        public void setUTCTime(String UTCTime) {
            this.UTCTime = UTCTime;
        }

        public int getTimeZone() {
            return TimeZone;
        }

        public void setTimeZone(int timeZone) {
            TimeZone = timeZone;
        }

        public String getLocalTime() {
            return LocalTime;
        }

        public void setLocalTime(String localTime) {
            LocalTime = localTime;
        }
    }

    public static class DaylightSavingTimeClass implements Serializable {
        private boolean Enabled;
        private String Country;
        private int Offset;
        private List<WeekClass> Week;

        public boolean isEnabled() {
            return Enabled;
        }

        public void setEnabled(boolean enabled) {
            Enabled = enabled;
        }

        public int getOffset() {
            return Offset;
        }

        public void setOffset(int offset) {
            Offset = offset;
        }

        public List<WeekClass> getWeek() {
            return Week;
        }

        public void setWeek(List<WeekClass> week) {
            Week = week;
        }

        public String getCountry() {
            return Country;
        }

        public void setCountry(String country) {
            Country = country;
        }
    }

    public static class UpgradeClass {
        private boolean Enabled;
        private boolean EnabledUpgradeChannel;

        public boolean isEnabled() {
            return Enabled;
        }

        public void setEnabled(boolean enabled) {
            Enabled = enabled;
        }

        public boolean isEnabledUpgradeChannel() {
            return EnabledUpgradeChannel;
        }

        public void setEnabledUpgradeChannel(boolean enabledUpgradeChannel) {
            EnabledUpgradeChannel = enabledUpgradeChannel;
        }
    }

    public static class UpgradeStatusClass {
        private String Status;
        private int DeviceIndex;
        private int Progress;
        private String error;

        public String getStatus() {
            return Status;
        }

        public void setStatus(String status) {
            Status = status;
        }

        public int getDeviceIndex() {
            return DeviceIndex;
        }

        public void setDeviceIndex(int deviceIndex) {
            DeviceIndex = deviceIndex;
        }

        public int getProgress() {
            return Progress;
        }

        public void setProgress(int progress) {
            Progress = progress;
        }

        public String getError() {
            return error;
        }

        public void setError(String error) {
            this.error = error;
        }
    }

    public static class WeekClass implements Serializable {
        private String Type;
        private int Month;
        private int Week;
        private int Weekday;
        private int Hour;
        private int Minute;

        public String getType() {
            return Type;
        }

        public void setType(String type) {
            Type = type;
        }

        public int getMonth() {
            return Month;
        }

        public void setMonth(int month) {
            Month = month;
        }

        public int getWeek() {
            return Week;
        }

        public void setWeek(int week) {
            Week = week;
        }

        public int getWeekday() {
            return Weekday;
        }

        public void setWeekday(int weekday) {
            Weekday = weekday;
        }

        public int getHour() {
            return Hour;
        }

        public void setHour(int hour) {
            Hour = hour;
        }

        public int getMinute() {
            return Minute;
        }

        public void setMinute(int minute) {
            Minute = minute;
        }
    }


    public static class OperationPropertyClass {
        private List<optClass> opt;

        public List<optClass> getOpt() {
            return opt;
        }

        public void setOpt(List<optClass> opt) {
            this.opt = opt;
        }
    }

    public static class optClass {
        private String AddChannel;
        private String DelChannel;
        private String SetChannel;
        private String GetChannel;

        public String getAddChannel() {
            return AddChannel;
        }

        public void setAddChannel(String addChannel) {
            AddChannel = addChannel;
        }

        public String getDelChannel() {
            return DelChannel;
        }

        public void setDelChannel(String delChannel) {
            DelChannel = delChannel;
        }

        public String getSetChannel() {
            return SetChannel;
        }

        public void setSetChannel(String setChannel) {
            SetChannel = setChannel;
        }

        public String getGetChannel() {
            return GetChannel;
        }

        public void setGetChannel(String getChannel) {
            GetChannel = getChannel;
        }
    }

    public static class LedPwm implements Serializable {
        private int channelCount;
        @SerializedName("switch")
        private int newSwitch;
        private List<LedChannelInfo> channelInfo;
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

        public List<LedChannelInfo> getChannelInfo() {
            return channelInfo;
        }

        public void setChannelInfo(List<LedChannelInfo> channelInfo) {
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

    private static class LedChannelInfo implements Serializable {
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

    public static class DoorbellManagerClass implements Serializable {
        private boolean BellEnabled;
        private String BellType;
        private int DigtialCtlTime;

        public boolean isBellEnabled() {
            return BellEnabled;
        }

        public void setBellEnabled(boolean bellEnabled) {
            BellEnabled = bellEnabled;
        }

        public String getBellType() {
            return BellType;
        }

        public void setBellType(String bellType) {
            BellType = bellType;
        }

        public int getDigtialCtlTime() {
            return DigtialCtlTime;
        }

        public void setDigtialCtlTime(int digtialCtlTime) {
            DigtialCtlTime = digtialCtlTime;
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

    public static class UserManagerClass {
        private String Method;
        private String Verify;
        private String username;
        private String password;

        public String getMethod() {
            return Method;
        }

        public void setMethod(String method) {
            Method = method;
        }

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
