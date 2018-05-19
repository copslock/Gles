#ifndef _LIBJNNAT_H
#define _LIBJNNAT_H
#ifdef __cplusplus
extern "C" {
#endif
#ifdef _WIN32
#if defined KP2P_API_EXPORTS
#define KP2P_API __declspec(dllexport)
#else
#define KP2P_API __declspec(dllimport)
#endif
#define ALIGNED(n) __declspec(align(n))
#else
#define KP2P_API
#define ALIGNED(n) __attribute__((aligned(n)))
#endif

#if defined(__MACH__) || defined(__unix__)
#include <sys/types.h>
#elif defined(_WIN32)
#include <time.h>
typedef int ssize_t;
#endif
#include <stdint.h>


//========================================================
//  macro definitions
//========================================================
/* P2P录像搜索单次最大记录数 */
#define KP2P_REC_FILES_MAX_CNT  100


/* P2P帧类型 */
#define KP2P_FRAME_TYPE_AUDIO   0
#define KP2P_FRAME_TYPE_IFRAME  1
#define KP2P_FRAME_TYPE_PFRAME  2
#define KP2P_FRAME_TYPE_OOB     15


/* P2P用户名和密码加密AES key */
#define KP2P_VERIFY_AES_KEY     "~!JUAN*&Vision-="


//========================================================
//  structrue definitions
//========================================================
/* P2P会话句柄 */
typedef struct proc_context_s *kp2p_handle_t;

/* P2P录像查找句柄 */
typedef struct find_file_context_s *kp2p_find_file_handle_t;

/* P2P录像搜索句柄 */
typedef struct replay_search_context_s *kp2p_rec_search_handle_t;

/* P2P录像回放句柄 */
typedef struct replay_play_context_s *kp2p_rec_playback_handle_t;

/* P2P虚拟通道句柄 */
typedef void *kp2p_vcon_handle_t;


/* 错误码 */
typedef enum kp2p_error_code_e {
    /* 成功 */
    KP2P_ERR_SUCCESS = 0,

    /* 超时错误码 */
    KP2P_ERR_TIMEOUT = -2,

    /* 连接错误码 */
    KP2P_ERR_CLOSE_BY_SELF = -10,
    KP2P_ERR_CLOSE_BY_PEER = -11,
    KP2P_ERR_RESOLVE_FAILED = -12, // 寻址失败
    KP2P_ERR_CONNECT_FAILED = -13, // 连接失败
    KP2P_ERR_SEND_FAILED = -14,

    /* 用户校验错误码 */
    KP2P_ERR_AUTH_FAILED = -20,
    KP2P_ERR_GET_NONCE_FAILED = -21,
    KP2P_ERR_AUTH2_FAILED = -22,

    /* PTZ错误码 */
    KP2P_ERR_PTZ_CTRL_FAILED = -30,

    /* 预览错误码 */
    KP2P_ERR_OPEN_STREAM_FAILED = -40,

    /* 回放错误码 */
    KP2P_ERR_REC_SEARCH_FAILED = -50,
    KP2P_ERR_REC_PLAY_FAILED = -51,

    /* 语音错误码 */
    KP2P_ERR_VOP2P_CALL_FAILED = -60,

    /* 远程设置错误码 */
    KP2P_ERR_REMOTE_SETUP_FAILED = -70,

} kp2p_error_code_t;


/* 连接设备方式 */
typedef enum kp2p_connect_type_e {
    CONNECT_TYPE_NONE = 0,
    /* 局域网IP直连UDP方式 */
    CONNECT_TYPE_IP_UDP = (1 << 0),
    /* 局域网IP直连TCP方式 */
    CONNECT_TYPE_IP_TCP = (1 << 1),
    /* 穿透方式 */
    CONNECT_TYPE_HOLE = (1 << 2),
    /* 转发UDP方式 */
    CONNECT_TYPE_TURN_UDP = (1 << 3),
    /* 转发TCP方式 */
    CONNECT_TYPE_TURN_TCP = (1 << 4),

    CONNECT_TYPE_P2P_TURN_UDP = (1 << 5),
    CONNECT_TYPE_P2P_TURN_TCP = (1 << 6)
} kp2p_connect_type_t;


/* 云台控制类型 */
typedef enum kp2p_ptz_control_action_e {
    /* 停止云台操作 */
    KP2P_PTZ_CONTROL_ACTION_STOP = 0,

    /* 自动水平旋转 */
    KP2P_PTZ_CONTROL_ACTION_AUTO,

    /* 云台方向操作 */
    KP2P_PTZ_CONTROL_ACTION_UP,
    KP2P_PTZ_CONTROL_ACTION_DOWN,
    KP2P_PTZ_CONTROL_ACTION_LEFT,
    KP2P_PTZ_CONTROL_ACTION_RIGHT,

    /* 光圈操作 */
    KP2P_PTZ_CONTROL_ACTION_IRIS_OPEN,
    KP2P_PTZ_CONTROL_ACTION_IRIS_CLOSE,

    /* 镜头调焦操作 */
    KP2P_PTZ_CONTROL_ACTION_ZOOM_IN,
    KP2P_PTZ_CONTROL_ACTION_ZOOM_OUT,

    /* 镜头变倍操作 */
    KP2P_PTZ_CONTROL_ACTION_FOCUS_F,
    KP2P_PTZ_CONTROL_ACTION_FOCUS_N,

    /* 辅助开关 */
    KP2P_PTZ_CONTROL_ACTION_AUX,

    /* 预置点操作　*/
    KP2P_PTZ_CONTROL_ACTION_PRESET_SET,
    KP2P_PTZ_CONTROL_ACTION_PRESET_GO,
    KP2P_PTZ_CONTROL_ACTION_PRESET_CLEAR

} kp2p_ptz_control_action_t;


typedef struct kp2p_client_info_s {
    char user_name[32];
    char phone_model[32];
    char system_name[32];
    char system_version[32];
    char app_name[32];
    char app_version[32];
    char network_type[32];
    char network_provider[32];
} kp2p_client_info_t;


typedef struct kp2p_device_search_info_s {
    char uid[32];
    uint32_t af;
    union {
        uint32_t v4;
        uint8_t  v6[16];
    } ip;
    uint32_t port;
} kp2p_device_search_info_t;


/* P2P回调指针集合 */
typedef struct kp2p_callback_s {
    /**
     * @brief 连接建立事件
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param conn_type         连接类型
     */
    void (*OnConnect)(kp2p_handle_t p2p_handle, void *context, const char *conn_type);

    /**
     * @brief 连接断开事件
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param ret               返回码
     */
    void (*OnDisconnect)(kp2p_handle_t p2p_handle, void *context, int ret);

    /**
     * @brief 设备搜索事件
     * @param info              设备信息
     * @param ctx               用户上下文
     */
    void (*OnDeviceSearch)(kp2p_device_search_info_t *info, void *ctx);

    /**
     * @brief 登录设备结果事件
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param ret               返回码
     */
    void (*OnLoginResult)(kp2p_handle_t p2p_handle, void *context, int ret);

    /**
     * @brief 云台控制结果事件
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param ret               返回码
     */
    void (*OnPtzResult)(kp2p_handle_t p2p_handle, void *context, int ret);

    /**
     * @brief 打开直播码流事件
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param channel           直播通道
     * @param stream            直播码流
     * @param err             打开码流错误码
     * @param cam_des           通道描述字符串
     */
    void (*OnOpenStream)(kp2p_handle_t p2p_handle, void *context, uint32_t channel, uint32_t stream, int err, const char *cam_des);

    /**
     * @brief 关闭直播码流事件
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param channel           直播通道
     * @param stream            直播码流
     * @param err               关闭码流错误码
     */
    void (*OnCloseStream)(kp2p_handle_t p2p_handle, void *context, uint32_t channel, uint32_t stream, int err);

    /**
     * @brief 直播数据接收事件
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param media_type        媒体数据类型
     * @param frame_type        帧类型
     * @param frame             帧数据
     * @param frame_size        帧数据长度
     */
    void (*OnRecvFrame)(kp2p_handle_t p2p_handle, void *context, uint32_t media_type, uint32_t frame_type, void *frame, size_t frame_size);

    /**
     * @brief 直播数据接收事件
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param frame_type        帧类型 e.g.
     *                              音频帧  KP2P_FRAME_TYPE_AUDIO
     *                              视频I帧 KP2P_FRAME_TYPE_IFRAME
     *                              视频P帧 KP2P_FRAME_TYPE_PFRAME
     *                              OOB帧  KP2P_FRAME_TYPE_OOB
     * @param frame             帧数据
     * @param frame_size        帧数据长度
     * @param channel           直播通道
     * @param time_stamp        时间戳
     * @param width             视频宽度
     * @param height            视频高度
     * @param sample_rate       音频采样率
     * @param sample_witdh      音频采样宽度
     * @param sample_channels   音频采样通道数
     * @param enc               视频编码(e.g. H264/H265),音频编码(e.g. G711A)
     */
    void (*OnRecvFrameEx)(kp2p_handle_t p2p_handle, void *context,
                          uint32_t frame_type, void *frame, size_t frame_size, uint32_t channel, uint64_t time_stamp,
                          uint32_t width, uint32_t height,
                          uint32_t sample_rate, uint32_t sample_width, uint32_t sample_channels,
                          const char *enc);

    /**
     * @brief 录像回放开始回调
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param result            返回码
     */
    void (*OnRecPlayStart)(kp2p_handle_t p2p_handle, void *context, int result);

    /**
     * @brief 录像回放控制回调
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param result            返回码
     */
    void (*OnRecPlayControl)(kp2p_handle_t p2p_handle, void *context, int result);

    /**
     * @brief 录像回放停止
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param result            返回码
     */
    void (*OnRecPlayStop)(kp2p_handle_t p2p_handle, void *context, int result);

    /**
     * @brief 回放数据接收事件
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param frame_type        帧类型 e.g.
     *                              音频帧  KP2P_FRAME_TYPE_AUDIO
     *                              视频I帧 KP2P_FRAME_TYPE_IFRAME
     *                              视频P帧 KP2P_FRAME_TYPE_PFRAME
     *                              OOB帧  KP2P_FRAME_TYPE_OOB
     * @param frame             帧数据
     * @param frame_size        帧数据长度
     * @param channel           回放通道
     * @param time_stamp        时间戳
     * @param width             视频宽度
     * @param height            视频高度
     * @param sample_rate       音频采样率
     * @param sample_witdh      音频采样宽度
     * @param sample_channels   音频采样通道数
     * @param gen_time
     * @param enc               视频编码(e.g. H264/H265),音频编码(e.g. G711A)
     */
    void (*OnRecvRecFrame)(kp2p_handle_t p2p_handle, void *context,
                           uint32_t frame_type,
                           void *frame, size_t frame_size, uint32_t channel, uint64_t time_stamp,
                           uint32_t width, uint32_t height,
                           uint32_t sample_rate, uint32_t sample_width, uint32_t sample_channels,
                           uint32_t gen_time,
                           const char* enc);

    /**
     * @brief 打开语音回调
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param result            返回码
     */
    void (*OnVOP2PCallResult)(kp2p_handle_t p2p_handle, void *context, int result);

    /**
     * @brief 关闭语音回调
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param result            返回码
     */
    void (*OnVOP2PHangupResult)(kp2p_handle_t p2p_handle, void *context, int result);

    /**
     * @brief vcon数据接收事件
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param data              数据
     * @param data_size         数据长度
     */
    void (*OnVconRecv)(kp2p_handle_t p2p_handle, void *context, const void *data, size_t data_size);

    /**
     * @brief 远程设置事件
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param data              远程设置数据数据
     * @param data_size         远程设置数据长度
     */
    void (*OnRemoteSetup)(kp2p_handle_t p2p_handle, void *context, const void *data, size_t data_size);

    /**
     * @brief 错误事件
     * @param p2p_handle        P2P连接句柄
     * @param context           用户上下文
     * @param data              错误信息
     * @param data_size         错误信息长度
     */
    void (*OnP2PError)(kp2p_handle_t p2p_handle, void *context, const char *msg, size_t msg_len);
} kp2p_callback_t;


/* 时间结构 */
typedef struct kp2p_time_s {
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
} kp2p_time_t;


/* 录像查找条件 */
typedef struct kp2p_find_file_cond_s {
    /* 查找通道 */
    uint32_t channel;
    /* 类型 */
    uint32_t type;
    /* 开始时间 */
    kp2p_time_t start_time;
    /* 结束时间 */
    kp2p_time_t stop_time;
} kp2p_find_file_cond_t;


/* 录像搜索结果 */
typedef struct kp2p_find_file_data_s {
    /* 开始时间 */
    kp2p_time_t start_time;
    /* 结束时间 */
    kp2p_time_t stop_time;
    /* 录像文件大小 */
    uint32_t file_size;
    /* 录像文件类型 */
    uint32_t file_type;
} kp2p_find_file_data_t;


/* 录像记录 */
typedef struct kp2p_rec_file_s {
    uint32_t channel;
    uint32_t type;
    uint32_t begin_time;
    uint32_t end_time;
    uint32_t quality;
} kp2p_rec_file_t;


/* 录像记录集合 */
typedef struct kp2p_rec_files_s{
    uint32_t records_cnt;
    kp2p_rec_file_t records[KP2P_REC_FILES_MAX_CNT];
} kp2p_rec_files_t;



//========================================================
//  API declarations
//========================================================
/**
 * @brief 获取SDK版本号
 * @return                  有效的版本字符串
 */
KP2P_API const char* kp2p_get_version(void);


/**
 * @brief P2PSDK初始化
 * @param callback          回调事件结构指针
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_init(kp2p_callback_t *callback);


/**
 * @brief P2PSDK反初始化
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_exit(void);


/**
 * @brief 设置P2P连接类型
 * @param type              连接类型(e.g. CONNECT_TYPE_HOLE/CONNECT_TYPE_TURN_UDP)
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_set_connect_type(kp2p_connect_type_t type);


/**
 * @brief 设置P2P转发服务器地址
 * @param addr              IP地址字符串
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_set_turn_server(const char *addr);


/**
 * @brief 设置客户端信息
 * @param client_info       客户端信息
 * @return                  成功  ０
 *                          失败  -1
 */
KP2P_API int kp2p_set_client_info(kp2p_client_info_t *client_info);


/**
 * @brief 开始局域网设备搜索
 * @param ctx               用户上下文
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_search_device_start(void *ctx);


/**
 * @brief 结束局域网设备搜索
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_search_device_stop(void);


/**
 * @brief 创建P2P会话句柄
 * @param context           用户上下文
 * @return                  成功  有效的连接句柄
 */
KP2P_API kp2p_handle_t kp2p_create(void *context);


/**
 * @brief 建立P2P连接(阻塞,成功与否直接检查返回值,也可通过OnConnect回调查询)
 * @param p2p_handle        会话句柄
 * @param dev_id            设备ID
 * @param ipaddr            设备IP
 * @param port              设备端口
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_connect(kp2p_handle_t p2p_handle, const char* dev_id, const char *ipaddr, uint32_t port);


/**
 * @brief 建立P2P连接(阻塞,使用verify string方式登录,成功与否直接检查返回值)
 * @param p2p_handle        会话句柄
 * @param dev_id            设备ID
 * @param ipaddr            设备IP
 * @param port              设备端口
 * @param verify_str        加密字符串
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_connect2(kp2p_handle_t p2p_handle, const char *dev_id, const char *ipaddr, uint32_t port, const char* verify_str);


/**
 * @brief 关闭P2P连接
 * @param p2p_handle        会话句柄
 */
KP2P_API void kp2p_close(kp2p_handle_t p2p_handle);


/**
 * @brief 登录设备(阻塞,成功与否直接检查返回值,也可通过OnLoginResult回调查询)
 * @param p2p_handle        会话句柄
 * @param user              用户名
 * @param pwd               密码
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_login(kp2p_handle_t p2p_handle, const char* user, const char* pwd);


/**
 * @brief 登录设备(非阻塞,必须通过OnLoginResult回调查询)
 * @param p2p_handle        会话句柄
 * @param user              用户名
 * @param pwd               密码
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_login_nonblock(kp2p_handle_t p2p_handle, const char* user, const char* pwd);


/**
 * @brief 独占模式获取nonce
 * @param p2p_handle        会话句柄
 * @param nonce_buff        存储nonce的缓冲
 * @param buff_size         缓冲长度
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_get_nonce(kp2p_handle_t p2p_handle, char *nonce_buff, size_t buff_size);


/**
 * @brief 独占模式登录（阻塞，可以检查返回值或者检查OnLoginResult回调）
 * @param p2p_handle        会话句柄
 * @param auth_info         检验信息
 * @param auth_info_len     校验信息长度
 * @param signature         签名
 * @param signature_len     签名长度
 * @return
 */
KP2P_API int kp2p_login2(kp2p_handle_t p2p_handle,
                         const char *auth_info, size_t auth_info_len,
                         const char *signature, size_t signature_len);


/**
 * @brief 独占模式登录（非阻塞模式，必须检查OnLoginResult回调）
 * @param p2p_handle        会话句柄
 * @param auth_info         检验信息
 * @param signature         签名
 * @return
 */
KP2P_API int kp2p_login2_nonblock(kp2p_handle_t p2p_handle, char *auth_info, char *signature);


/**
 * @brief 云台控制(非阻塞,成功与否通过OnPtzResult回调查询)
 * @param p2p_handle        会话句柄
 * @param channel           通道
 * @param action            控制动作类型(e.g. KP2P_PTZ_CONTROL_ACTION_UP)
 * @param param1            参数1
 * @param param2            参数2
 * @return                  成功  0
 *                          失败  -1
 * 控制动作                              描述    			参数1                   参数2
 * KP2P_PTZ_CONTROL_ACTION_STOP         停止云台运动      无                      无
 * KP2P_PTZ_CONTROL_ACTION_AUTO         自动水平旋转		开启或停止自动，取值1或者0  无
 * KP2P_PTZ_CONTROL_ACTION_UP			控制云台向上运动	运动速度，取值0~7			无
 * KP2P_PTZ_CONTROL_ACTION_DOWN			控制云台向下运动	运动速度，取值0~7			无
 * KP2P_PTZ_CONTROL_ACTION_LEFT			控制云台向左运动	运动速度，取值0~7			无
 * KP2P_PTZ_CONTROL_ACTION_RIGHT        控制云台向右运动	运动速度，取值0~7			无
 * KP2P_PTZ_CONTROL_ACTION_IRIS_OPEN    控制光圈打开		镜头速度，取值0~7			无
 * KP2P_PTZ_CONTROL_ACTION_IRIS_CLOSE   控制光圈关闭		镜头速度，取值0~7			无
 * KP2P_PTZ_CONTROL_ACTION_ZOOM_IN		调整远焦			镜头速度，取值0~7			无
 * KP2P_PTZ_CONTROL_ACTION_ZOOM_OUT     调整近焦			镜头速度，取值0~7			无
 * KP2P_PTZ_CONTROL_ACTION_FOCUS_F		调整变倍大		镜头速度，取值0~7			无
 * KP2P_PTZ_CONTROL_ACTION_FOCUS_N		调整变倍小		镜头速度，取值0~7         无
 * KP2P_PTZ_CONTROL_ACTION_AUX			辅助开关1		    开关号0/1                打开或关闭，取值1或0
 * KP2P_PTZ_CONTROL_ACTION_PRESET_SET   设置预置点位置    预置点                   无
 * KP2P_PTZ_CONTROL_ACTION_PRESET_GO    移动到预置点      预置点                  　无
 * KP2P_PTZ_CONTROL_ACTION_PRESET_CLEAR 清除预置点位置    预置点                   无
 */
KP2P_API int kp2p_ptz_ctrl(kp2p_handle_t p2p_handle, uint32_t channel, kp2p_ptz_control_action_t action, uint32_t param1, uint32_t param2);


/**
 * @brief 打开直播码流(非阻塞,成功与否通过OnOpenStream回调查询)
 * @param p2p_handle        会话句柄
 * @param channel_id        直播通道ID
 * @param stream_id         直播码流ID
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_open_stream(kp2p_handle_t p2p_handle, uint32_t channel_id, uint32_t stream_id);


/**
 * @brief 关闭直播码流(非阻塞,成功与否通过OnCloseStream回调查询)
 * @param p2p_handle        会话句柄
 * @param channel_id        直播通道ID
 * @param stream_id         直播码流ID
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_close_stream(kp2p_handle_t p2p_handle, uint32_t channel_id, uint32_t stream_id);


/**
 * @brief 切换直播码流(非阻塞,成功与否通过OnOpenStream回调查询)
 * @param p2p_handle        会话句柄
 * @param channel_id        直播通道ID
 * @param stream_id         直播码流ID
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_change_stream(kp2p_handle_t p2p_handle, uint32_t channel_id, uint32_t stream_id);


/**
 * @brief 开始录像查找(阻塞)
 * @param p2p_handle        会话句柄
 * @param cond              搜索条件
 * @param timeout           超时时间
 * @return                  成功  有效的查找句柄
 *                          失败  NULL
 */
KP2P_API kp2p_find_file_handle_t kp2p_find_file(kp2p_handle_t p2p_handle, kp2p_find_file_cond_t *cond, uint32_t timeout);


/**
 * @brief 查找下一条录像记录(阻塞)
 * @param find_handle       查找句柄
 * @param data_buf          存放记录的存储区
 * @param timeout           超时时间
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_find_next_file(kp2p_find_file_handle_t find_handle, kp2p_find_file_data_t *data_buf, uint32_t timeout);


/**
 * @brief 停止录像查找(阻塞)
 * @param find_handle       查找句柄
 * @param timeout           超时时间
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_find_stop(kp2p_find_file_handle_t find_handle, uint32_t timeout);


/**
 * @brief 开始搜索录像文件(阻塞)
 * @param p2p_handle        会话句柄
 * @param chn_cnt           通道数
 * @param chns              通道组合
 * @param start_time        开始时间
 * @param end_time          结束时间
 * @param type              类型
 * @return                  成功  返回录像搜索句柄
 *                          失败  NULL
 */
KP2P_API kp2p_rec_search_handle_t kp2p_rec_find_file_start(kp2p_handle_t p2p_handle,
                                                           uint32_t chn_cnt, uint8_t *chns,
                                                           time_t start_time, time_t end_time,
                                                           uint32_t type);


/**
 * @brief 本次录像搜索的总记录数
 * @param search_handle     搜索句柄
 * @return                  成功  返回实际总纪录数
 *                          失败  -1
 */
KP2P_API ssize_t kp2p_rec_find_file_count(kp2p_rec_search_handle_t search_handle);


/**
 * @brief 寻找下一个录像文件记录组合(阻塞)
 * @param search_handle     任务句柄
 * @return                  成功  有效的录像文件记录
 *                          失败  NULL
 */
KP2P_API kp2p_rec_files_t *kp2p_rec_find_file_next(kp2p_rec_search_handle_t search_handle);


/**
 * @brief 停止录像搜索(非阻塞)
 * @param search_handle     搜索句柄
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_rec_find_file_stop(kp2p_rec_search_handle_t search_handle);


/**
 * @brief 开始播放录像文件(非阻塞)
 * @param p2p_handle        会话句柄
 * @param chn_cnt           通道数
 * @param chns              通道组合
 * @param start_time        开始时间
 * @param end_time          结束时间
 * @param type              类型
 * @return                  成功  录像播放句柄
 *                          失败  NULL
 */
KP2P_API kp2p_rec_playback_handle_t kp2p_rec_play_start(kp2p_handle_t p2p_handle,
                                                        uint32_t chn_cnt, uint8_t *chns,
                                                        time_t start_time, time_t end_time,
                                                        uint32_t type);


/**
 * @brief 开始播放录像文件2(非阻塞)
 * @param p2p_handle        会话句柄
 * @param chn_cnt           通道数
 * @param chns              通道组合
 * @param start_time        开始时间
 * @param end_time          结束时间
 * @param type              类型
 * @param open_type         打开类型: 0 录像回放 1 录像下载
 * @return                  成功  录像播放句柄
 *                          失败  NULL
 */
KP2P_API kp2p_rec_playback_handle_t kp2p_rec_play_start2(kp2p_handle_t p2p_handle,
                                                        uint32_t chn_cnt, uint8_t *chns,
                                                        time_t start_time, time_t end_time,
                                                        uint32_t type, uint32_t open_type);

/**
 * @brief 暂停播放录像文件(非阻塞)
 * @param playback_handle   播放句柄
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_rec_play_pause(kp2p_rec_playback_handle_t playback_handle);


/**
 * @brief 继续播放录像文件(非阻塞)
 * @param playback_handle   播放句柄
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_rec_play_continue(kp2p_rec_playback_handle_t playback_handle);


/**
 * @brief 停止播放录像文件(非阻塞)
 * @param playback_handle   播放句柄
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_rec_play_stop(kp2p_rec_playback_handle_t playback_handle);


/**
 * @brief kp2p_vop2p_call(非阻塞)
 * @param p2p_handle
 * @param channel
 * @return
 */
KP2P_API int kp2p_vop2p_call(kp2p_handle_t p2p_handle, uint32_t channel);


/**
 * @brief 语音数据发送(非阻塞)
 * @param voice_data        语音数据
 * @param voice_data_size   语音数据长度
 * @param time_stamp        时间戳
 * @param enc               编码名称
 * @param sample_rate       采样率
 * @param sample_width      采样宽度
 * @param sample_channels   采样通道总数
 * @param compress_ratio    压缩比率
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_vop2p_send(kp2p_handle_t p2p_handle,
                             void *voice_data, size_t voice_data_size,
                             uint64_t time_stamp,
                             const char *enc,
                             uint32_t sample_rate,
                             uint32_t sample_width,
                             uint32_t channels,
                             float compress_ratio);

/**
 * @brief 语音挂断(非阻塞)
 * @param p2p_handle        会话句柄
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_vop2p_hangup(kp2p_handle_t p2p_handle);


/**
 * @brief 创建虚拟通道
 * @param p2p_handle        会话句柄
 * @param channel           通道名称
 * @return                  成功  有效的vcon句柄
 *                          失败  NULL
 */
KP2P_API kp2p_vcon_handle_t kp2p_vcon_create(kp2p_handle_t p2p_handle, const char* channel);


/**
 * @brief vcon数据发送
 * @param vcon_handle       虚拟通道句柄
 * @param data              数据
 * @param data_size         数据长度
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_vcon_send(kp2p_vcon_handle_t vcon_handle, const void *data, size_t data_size);


/**
 * @brief 销毁虚拟通道
 * @param vcon_handle       虚拟通道句柄
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_vcon_destroy(kp2p_vcon_handle_t vcon_handle);


/**
 * @brief 远程设置(非阻塞)
 * @param p2p_handle        连接句柄
 * @param data              数据
 * @param data_size         数据长度
 * @return                  成功  0
 *                          失败  -1
 */
KP2P_API int kp2p_remote_setup(kp2p_handle_t p2p_handle, const void *data, size_t data_size);


#ifdef __cplusplus
}
#endif
#endif
