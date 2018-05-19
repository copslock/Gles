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
#define KP2P_REC_FILES_MAX_CNT  10

/* P2P帧类型 */
#define KP2P_FRAME_TYPE_AUDIO   0
#define KP2P_FRAME_TYPE_IFRAME  1
#define KP2P_FRAME_TYPE_PFRAME  2
#define KP2P_FRAME_TYPE_OOB     15



//========================================================
//  structrue definitions
//========================================================
/* P2P会话句柄 */
typedef struct session_s *kp2p_handle_t;

/* P2P录像搜索句柄 */
typedef struct replay_search_context_s *kp2p_rec_search_handle_t;

/* P2P录像回放句柄 */
typedef struct replay_play_context_s *kp2p_rec_playback_handle_t;

/* P2P虚拟通道句柄 */
typedef void *kp2p_vcon_handle_t;


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
    KP2P_PTZ_CONTROL_ACTION_AUX
} kp2p_ptz_control_action_t;


/* P2P回调指针集合 */
typedef struct kp2p_callback_s {
    /* 连接事件 */
    void (*OnConnect)(kp2p_handle_t p2p_handle, void *context, const char *conn_type);

    /* 断开连接事件 */
    void (*OnDisconnect)(kp2p_handle_t p2p_handle, void *context, int ret);

    /* 登录设备结果事件 */
    void (*OnLoginResult)(kp2p_handle_t p2p_handle, void *context, int ret);

    /* 云台控制结果事件 */
    void (*OnPtzResult)(kp2p_handle_t p2p_handle, void *context, int ret);

    /* 打开直播通道事件 */
    void (*OnOpenChannel)(kp2p_handle_t p2p_handle, void *context, uint32_t channel, uint8_t stream, const char *cam_des);

    /* 直播数据事件 */
    void (*OnRecvFrame)(kp2p_handle_t p2p_handle, void *context, uint8_t media_type, uint8_t frame_type, void *frame, size_t frame_size);

    void (*OnRecvFrameEx)(kp2p_handle_t p2p_handle, void *context,
                          uint8_t frame_type,
                          void *frame, size_t frame_size,
                          uint64_t time_stamp,
                          uint32_t width,
                          uint32_t height,
                          uint32_t sample_rate,
                          uint32_t sample_width,
                          uint32_t channels,
                          const char *enc);

    /* 录像数据事件 */
    void (*OnRecvRecFrame)(kp2p_handle_t p2p_handle, void *context,
                           uint8_t frame_type,
                           void *frame, size_t frame_size,
                           uint32_t width, uint32_t height,
                           uint32_t sample_rate, uint32_t sample_width, uint8_t sample_chns,
                           uint32_t chn,
                           uint64_t time_stamp,
                           uint32_t gen_time,
                           const char* enc);

    /* vcon数据接收事件 */
    void (*OnVconRecv)(kp2p_handle_t p2p_handle, void *context, const void *data, size_t data_size);

    /* 远程设置事件 */
    void (*OnRemoteSetup)(kp2p_handle_t p2p_handle, void *context, const void *data, size_t data_size);

    /* 错误事件 */
    void (*OnP2PError)(kp2p_handle_t p2p_handle, void *context, const char *msg, size_t msg_len);

} kp2p_callback_t;


/* 录像记录 */
typedef struct kp2p_rec_file_s {
    uint32_t channel;
    uint8_t type;
    uint32_t begin_time;
    uint32_t end_time;
    uint8_t quality;
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
 * @return
 *  有效的版本字符串
 */
KP2P_API const char* kp2p_get_version(void);


/**
 * @brief P2PSDK初始化
 * @param[in] callback
 *  回调指针结构
 * @return
 *  成功  0
 *  失败  -1
 */
KP2P_API int kp2p_init(kp2p_callback_t *callback);


/**
 * @brief P2PSDK反初始化
 * @return
 *  成功  0
 *  失败  -1
 */
KP2P_API int kp2p_exit(void);


/**
 * @brief 建立P2P连接
 * @param[in] context
 *  用户上下文
 * @param[in] dev_id
 *  设备ID
 * @param[in] ipaddr
 *  设备IP
 * @param[in] port
 *  设备端口
 * @return
 *  成功  有效的会话句柄
 *  失败  NULL
 */
KP2P_API kp2p_handle_t kp2p_connect(void *context, const char* dev_id, const char *ipaddr, uint16_t port);


/**
 * @brief 建立P2P连接
 * @param[in] context
 *  用户上下文
 * @param[in] devid
 *  设备ID
 * @param[in] ipaddr
 *  设备IP
 * @param[in] port
 *  设备端口
 * @param[in] verify_str
 *  校验字段
 * @return
 *  成功  有效的会话句柄
 *  失败  NULL
 */
KP2P_API kp2p_handle_t  kp2p_connect2(void *context, const char *devid, const char *ipaddr, uint16_t port, const char* verify_str);


/**
 * @brief 关闭P2P连接
 * @param[in] p2p_handle
 *  会话句柄
 */
KP2P_API void kp2p_close(kp2p_handle_t p2p_handle);


/**
 * @brief 登录设备
 * @param[in] p2p_handle
 *  会话句柄
 * @param[in] user
 *  用户名
 * @param[in] pwd
 *  密码
 * @return
 *  成功  0
 *  失败  -1
 */
KP2P_API int kp2p_login(kp2p_handle_t p2p_handle, const char* user, const char* pwd);


/**
 * @brief 云台控制
 * @param[in] p2p_handle
 *  会话句柄
 * @param[in] channel
 *  通道
 * @param[in] action
 *  控制动作类型
 * @param[in] param1
 *  参数1
 * @param[in] param2
 *  参数2
 * @return
 *  成功  0
 *  失败  -1
 */
KP2P_API int kp2p_ptz_ctrl(kp2p_handle_t p2p_handle, uint32_t channel, kp2p_ptz_control_action_t action, uint32_t param1, uint32_t param2);


/**
 * @brief 打开直播通道
 * @param[in] p2p_handle
 *  会话句柄
 * @param[in] channel_id
 *  直播通道ID
 * @param[in] stream_id
 *  直播码流ID
 * @param[in] act
 *  动作: 0(关闭) 1(打开)
 * @return
 *  成功  0
 *  失败  -1
 */
KP2P_API int kp2p_open_channel(kp2p_handle_t p2p_handle, uint32_t channel_id, uint8_t stream_id, uint8_t act);


/**
 * @brief 开始搜索录像文件
 * @param[in] p2p_handle
 *  会话句柄
 * @param[in] chn_cnt
 *  通道数
 * @param[in] chns
 *  通道组合
 * @param[in] start_time
 *  开始时间
 * @param[in] end_time
 *  结束时间
 * @param[in] type
 *  类型
 * @return
 *  成功  返回录像搜索句柄
 *  失败  NULL
 */
KP2P_API kp2p_rec_search_handle_t kp2p_rec_find_file_start(kp2p_handle_t p2p_handle,
                                                           uint32_t chn_cnt, uint8_t *chns,
                                                           time_t start_time, time_t end_time,
                                                           uint8_t type);


/**
 * @brief 本次录像搜索的总记录数
 * @param[in] search_handle
 *  搜索句柄
 * @return
 *  成功  返回实际总纪录数
 *  失败  -1
 */
KP2P_API ssize_t kp2p_rec_find_file_count(kp2p_rec_search_handle_t search_handle);


/**
 * @brief 寻找下一个录像文件记录组合
 * @param[in] search_handle
 *  任务句柄
 * @return
 *  成功  有效的录像文件记录
 *  失败  NULL
 */
KP2P_API kp2p_rec_files_t *kp2p_rec_find_file_next(kp2p_rec_search_handle_t search_handle);


/**
 * @brief 停止录像搜索
 * @param[in] search_handle
 *  搜索句柄
 * @return
 *  成功  0
 *  失败  -1
 */
KP2P_API int kp2p_rec_find_file_stop(kp2p_rec_search_handle_t search_handle);


/**
 * @brief 开始播放录像文件
 * @param[in] p2p_handle
 *  会话句柄
 * @param[in] chn_cnt
 *  通道数
 * @param[in] chns
 *  通道组合
 * @param[in] start_time
 *  开始时间
 * @param[in] end_time
 *  结束时间
 * @param[in] type
 *  类型
 * @return
 *  成功  录像播放句柄
 *  失败  NULL
 */
KP2P_API kp2p_rec_playback_handle_t kp2p_rec_play_start(kp2p_handle_t p2p_handle,
                                                        uint32_t chn_cnt, uint8_t *chns,
                                                        time_t start_time, time_t end_time,
                                                        uint8_t type);


/**
 * @brief 暂停播放录像文件
 * @param[in] playback_handle
 *  播放句柄
 * @return
 *  成功  0
 *  失败  -1
 */
KP2P_API int kp2p_rec_play_pause(kp2p_rec_playback_handle_t playback_handle);


/**
 * @brief 继续播放录像文件
 * @param[in] playback_handle
 *  播放句柄
 * @return
 *  成功  0
 *  失败  -1
 */
KP2P_API int kp2p_rec_play_continue(kp2p_rec_playback_handle_t playback_handle);


/**
 * @brief 停止播放录像文件
 * @param[in] playback_handle
 *  播放句柄
 * @return
 *  成功  0
 *  失败  -1
 */
KP2P_API int kp2p_rec_play_stop(kp2p_rec_playback_handle_t playback_handle);


/**
 * @brief kp2p_vop2p_call
 * @param p2p_handle
 * @param channel
 * @return
 */
KP2P_API int kp2p_vop2p_call(kp2p_handle_t p2p_handle, uint32_t channel);


/**
 * @brief 语音数据发送
 * @param[in] voice_data
 *  语音数据buffer
 * @param[in] voice_data_size
 *  语音数据长度
 * @param[in] time_stamp
 *  时间戳
 * @param[in] enc
 *  编码名称
 * @param[in] sample_rate
 *  采样率
 * @param[in] sample_width
 *  采样宽度
 * @param[in] channels
 *  采样通道总数
 * @param[in] compress_ratio
 *  压缩比率
 * @return
 *  成功  0
 *  失败  -1
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
 * @brief 语音挂断
 * @param[in] p2p_handle
 *  会话句柄
 * @return
 *  成功  0
 *  失败  -1
 */
KP2P_API int kp2p_vop2p_hangup(kp2p_handle_t p2p_handle);


/**
 * @brief 创建虚拟通道
 * @param[in] p2p_handle
 *  会话句柄
 * @param[in] channel
 *  通道名称
 * @return
 *  成功  有效的vcon句柄
 *  失败  NULL
 */
KP2P_API kp2p_vcon_handle_t kp2p_vcon_create(kp2p_handle_t p2p_handle, const char* channel);


/**
 * @brief vcon数据发送
 * @param[in] vcon_handle
 *  虚拟通道句柄
 * @param[in] buf
 *  数据缓冲
 * @param[in] buf_len
 *  缓冲长度
 * @return
 *  成功  0
 *  失败  -1
 */
KP2P_API int kp2p_vcon_send(kp2p_vcon_handle_t vcon_handle, const void *buf, size_t buf_len);


/**
 * @brief 销毁虚拟通道
 * @param[in] vcon_handle
 *  虚拟通道句柄
 * @return
 *  成功  0
 *  失败  -1
 */
KP2P_API int kp2p_vcon_destroy(kp2p_vcon_handle_t vcon_handle);


/**
 * @brief 远程设置
 * @param[in] p2p_handle
 *  连接句柄
 * @param[in] data
 *  数据
 * @param[in] data_size
 *  数据长度
 * @return
 *  成功  0
 *  失败  -1
 */
KP2P_API int kp2p_remote_setup(kp2p_handle_t p2p_handle, const void *data, size_t data_size);



#ifdef __cplusplus
}
#endif
#endif
