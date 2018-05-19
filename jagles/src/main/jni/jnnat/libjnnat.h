#ifndef  __LIBJAP2P_H__
#define  __LIBJAP2P_H__


typedef unsigned long JAP2P_HANDLE;

#ifdef WIN32
typedef unsigned __int64 JAP2P_UINT64;
#ifdef LIBJNNAT_EXPORTS
#define P2PSDKClientAPI __declspec(dllexport)
#else
#define P2PSDKClientAPI __declspec(dllimport)
#endif
#else
#include <stdint.h>
#include <time.h>
typedef uint64_t JAP2P_UINT64;
#define  P2PSDKClientAPI
#endif 



/**
 * this is a fucking bad design
 * to let the vcon_handle's detail comes
 * maybe oneday, the lib will remove this
 * if all the lib's dependency agree jni, ios etc
 */
typedef struct{
    int vcon_id;				//虚拟通道ID
    JAP2P_HANDLE p2p_handle;		//P2P会话句柄
}VCON_HANDLE,*PVCON_HANDLE;

#ifdef __cplusplus
extern "C"{
#endif

/**
 *  云台控制动作
 *  标识无效的命令请不要使用
 *  命令							描述				param1					param2
 *  JA_PTZ_CMD_UP				控制云台向上运动	运动速度，取值0~7			无
 *  JA_PTZ_CMD_DOWN				控制云台向下运动	运动速度，取值0~7			无
 *  JA_PTZ_CMD_LEFT				控制云台向左运动	运动速度，取值0~7			无
 *  JA_PTZ_CMD_RIGHT,			控制云台向右运动	运动速度，取值0~7			无
 *  JA_PTZ_CMD_AUTOPAN,			自动水平旋转		开启或停止自动，取值1或者0	无
 *  JA_PTZ_CMD_IRIS_OPEN,		控制光圈打开		镜头速度，取值0~7			无
 *  JA_PTZ_CMD_IRIS_CLOSE,		控制光圈关闭		镜头速度，取值0~7			无
 *  JA_PTZ_CMD_ZOOM_IN,			调整远焦			镜头速度，取值0~7			无
 *  JA_PTZ_CMD_ZOOM_OUT,		调整近焦			镜头速度，取值0~7			无
 *  JA_PTZ_CMD_FOCUS_FAR,		调整变倍大		镜头速度，取值0~7			无
 *  JA_PTZ_CMD_FOCUS_NEAR		调整变倍小		镜头速度，取值0~7			无
 *  JA_PTZ_CMD_STOP,			停止云台运动		无						无
 *  JA_PTZ_CMD_AUX1,			辅助开关1		    开关号0					打开或关闭，取值1或0
 *  JA_PTZ_CMD_AUX2,			辅助开关2			开关号1					打开或关闭，取值1或0
 *  JA_PTZ_CMD_PRESET_SET,		设置预置点		预置点,取值1~?（相同，则覆盖）无
 *  JA_PTZ_CMD_PRESET_CLEAR,	清除预置点		预置点,取值1~？			无
 *  JA_PTZ_CMD_PRESET_GOTO,		转到预置点		预置点,取值1~？			无
 */
enum  JA_PTZ_CMD {
    JA_PTZ_CMD_NULL = -1,
    JA_PTZ_CMD_UP,
    JA_PTZ_CMD_DOWN,
    JA_PTZ_CMD_LEFT,
    JA_PTZ_CMD_RIGHT,
    JA_PTZ_CMD_LEFT_UP,			//无效命令
    JA_PTZ_CMD_RIGHT_UP,		//无效命令
    JA_PTZ_CMD_LEFT_DOWN,		//无效命令
    JA_PTZ_CMD_RIGHT_DOWN,		//无效命令
    JA_PTZ_CMD_AUTOPAN,
    JA_PTZ_CMD_IRIS_OPEN,
    JA_PTZ_CMD_IRIS_CLOSE,
    JA_PTZ_CMD_ZOOM_IN,
    JA_PTZ_CMD_ZOOM_OUT,
    JA_PTZ_CMD_FOCUS_FAR,
    JA_PTZ_CMD_FOCUS_NEAR,
    JA_PTZ_CMD_STOP,
    JA_PTZ_CMD_WIPPER_ON,		//无效命令
    JA_PTZ_CMD_WIPPER_OFF,		//无效命令
    JA_PTZ_CMD_LIGHT_ON,		//无效命令
    JA_PTZ_CMD_LIGHT_OFF,		//无效命令
    JA_PTZ_CMD_POWER_ON,		//无效命令
    JA_PTZ_CMD_POWER_OFF,		//无效命令
    JA_PTZ_CMD_GOTO_PRESET,		//无效命令
    JA_PTZ_CMD_SET_PRESET,		//无效命令
    JA_PTZ_CMD_CLEAR_PRESET,	//无效命令
    JA_PTZ_CMD_TOUR,			//无效命令
    JA_PTZ_CMD_AUX1,
    JA_PTZ_CMD_AUX2,
    JA_PTZ_CMD_PRESET_SET,
    JA_PTZ_CMD_PRESET_CLEAR,
    JA_PTZ_CMD_PRESET_GOTO,
    JA_PTZ_CMD_CNT,
};

/* 音频头，兼容bubble协议中的数据传输方式  */
#define DEFAULT_AUDIO_PACKSIZE 320
#pragma pack(4)
typedef struct AUDIO_BUF_ATTR
{
    int entries;  		/*音频包个数，一般为1 */
	int packsize; 		/*音频包字节长度，一般320 */
	JAP2P_UINT64 pts; 	/*时间戳：微秒 */
	int gtime;			/*时间戳：秒 */
	char encode[8];		/*编码格式,字符串 */
	int samplerate;		/*音频采样率  */
	int samplewidth;	/*音频采样位宽 */
}AUDIO_BUF_ATTR_t;
#pragma pack()



/*媒体类型  */
#define JA_MT_AUDIO		0
#define JA_MT_H264		1
/*帧类型  */
#define JA_FT_AUDIO		0
#define JA_FT_IFRAME	1
#define JA_FT_PFRAME	2
#define JA_FT_OOB       15

/*事件OnDisconnect  ret对应值 */
#define	JA_RET_OFFLINE		1     //设备离线
#define	JA_RET_TIMEOUT	   	 2   //连接超时
#define	JA_RET_FAILED		3   //连接失败
#define	JA_RET_CLOSE		4  //主动关闭,
#define   JA_RET_CLOSEBYPEER   5  //对端断开


#define FRMAE_SIZE 1024*1024


/**
 * 报警信息属性头, 用于报警业务
 */
 
enum P2PAlarmMsgType{
    P2P_ALARMMSG_TYPE_TEXT = 0,
    P2P_ALARMMSG_TYPE_AUDIO = 1,
    P2P_ALARMMSG_TYPE_SMALL_IMG = 2,
    P2P_ALARMMSG_TYPE_IMG = 3,
    P2P_ALARMMSG_TYPE_LARGE_IMG = 4,
    P2P_ALARMMSG_TYPE_VIDEO = 5,
};
    
struct P2PAlarmMsgData{
    int type;       //消息类型, 参考 enum P2PAlarmMsgType
    int chn;        //消息对应通道
    time_t ticket;  //消息对应时间
    union {
        // 图片
        struct {
            char src[64];           // 图片源，如，src.jpg
            char enc[8];            // 图片编码 JPEG....
            unsigned int fps;       // 图片帧率，一般为 1,保留做gif 用
            unsigned int width;     // 图片宽
            unsigned int height;    // 图片高
        }i;
        // 视频
        struct {
            char src[64];           // 视频源， 如，src.mp4
            char enc[8];            // 视频编码 H264/H265
            unsigned int fps;       // 视频帧率
            unsigned int width;     // 视频宽
            unsigned int height;    // 视频高
        }v;
        // 音频
        struct {
            char src[64];               //音频源， 如，src.aac
            char enc[8];                //音频编码 如，G711A/AAC
            unsigned int samplerate;    //音频采样率
            unsigned int samplewidth;   //音频采样位宽
            unsigned int channles;      //音频通道
            float compress_ratio;       //音频采样率
        }a;
    };
    int totalLen;   // 消息总长度
    int curLen;     // 当前消息包长度
    void *data;     // 数据，totalLen != curLen 就得拼包了
};


/* SDK回调函数结构体,用户必须自己实现,并传入初始化SDK接口进行初始化   */
typedef struct _ja_callback {
    /*连接过程中产生的成功连接事件*/
    void (*OnConnect)(JAP2P_HANDLE p2p_handle, const char* conn_type_msg);
    /*断开连接事件,连接失败，及关闭连接的时候会有此事件
     **/
    void (*OnDisconnect)(JAP2P_HANDLE p2p_handle, int ret);
    /*
     *p2p会话收到音/视频帧事件，对应帧数据如下
     *音频帧：		AUDIO_BUF_ATTR_t的头 + raw_g711音频
     *视频帧：		h264的裸数据
     *p2p_handle:	对应p2p会话句柄
     *media_type:	0:音频;1:视频
     *frame_type:	0:音频帧;1:关键帧;2:P帧
     *frame:		视频帧数据
     *frame_len:	视频帧长度
     */
    void (*OnRecvFrame)(
            JAP2P_HANDLE p2p_handle,
            unsigned short media_type,
            unsigned short frame_type,
            void* frame,
            int frame_len
    );
    void (*OnRecvFrameEx)(
            JAP2P_HANDLE p2p_handle,
            unsigned short frame_type,
            void *frame,
            int frame_len,
            JAP2P_UINT64 uiTimeStampMs,
            unsigned int uiWidth,
            unsigned int uiHeight,
            unsigned int uiSamplerate,
            unsigned int uiSamplewidth,
            unsigned int uiChannels,
            char *avEnc
    );
    

    
    /*
     *p2p 用户校验结果事件
     *ret: [0：成功;1:校验超时;2:用户名密码错误]
     */
    void (*OnLoginResult)(JAP2P_HANDLE p2p_handle, int ret);
    /*p2p云台控制结果事件
     */
    void (*OnPtzResult)(JAP2P_HANDLE p2p_handle, int ret);
    /*虚拟通道数据接收事件，注意区别于音视频接收事件回调* */
    void (*OnVconRecv)(JAP2P_HANDLE p2p_handle, void *buf, int len);
    void (*OnVconEvent)(JAP2P_HANDLE p2p_handle, char *event);
    void (*OnP2PError)(JAP2P_HANDLE p2p_handle, char *error_msg, int len);
    /**
     * 新增虚拟通道接收事件，只仍出对应vcon_handle,
     * 注意:只需要设置OnVconRecv/OnVconRecv2 中一个回调*
     */
    void (*OnVconRecv2)(VCON_HANDLE* vcon_handle, void *buf, int len);
    
    #define PROTO_CONNECTREQ_BUBBLE    0
    #define PROTO_CONNECTREQ_HOLE      1
    #define PROTO_CONNECTREQ_TURN      2
    
    //hole
    #define PROTO_CONNECTREQ_HOLE_REQ  10
    #define PROTO_CONNECTREQ_HOLE_REQ_ACK 11
    #define PROTO_CONNECTREQ_HOLE_READY 12
    #define PROTO_CONNECTREQ_HOLE_PUNCH 13

    void (*OnConnectReq)(JAP2P_HANDLE p2p_handle,
                         int proto_type,
                         int action,
                         unsigned int ip,
                         unsigned short port,
                         unsigned int random);
    
    void (*OnOpenChannel)(JAP2P_HANDLE p2p_handle,
                          int nChannelId,
                          int nStreamId,
                          const char *camDes);
    /**
     * 注意有可能一次回调的数据不是完整的一张图或一段视频
     */
    void (*OnRecvAlarmMsg)(JAP2P_HANDLE p2p_handle,
                           struct P2PAlarmMsgData *msgData);
}TJACallBack,*PJACallBack;


/**
 *  用事件处理回调初始化SDK
 *
 *  @param callback    事件处理回调实现
 *
 *  @return 0			成功
 *			-1			失败
 */
P2PSDKClientAPI int ja_p2p_init(PJACallBack callback);


/**
 *  初始化tutk
 *
 *
 *  @return 0			成功
 *	    1		        已经初始化过
 */
P2PSDKClientAPI int ja_p2p_init_tutk();

/**
 *  获取P2P的版本号
 *  @return 	P2P版本号字符
 *						
 */
P2PSDKClientAPI const char* ja_p2p_GetVersion(void);

/**
 *  释放SDK资源，当所有连接断开时
 *
 *  @return 0:	成功
 *			-1:	失败
 */
    
P2PSDKClientAPI int ja_p2p_exit(void);

/**
 *  设置调试信息级别
 *
 *  @param level 调试级别，值越在，信息越多
 */
P2PSDKClientAPI void ja_p2p_set_debug_level(int level);

/**
 * 设置是否使用onVconRecv回调（不设置默认true）
 *
 * @param bRet false,否；true,是
 * @return -1 不成功，0 成功
 */
 P2PSDKClientAPI int ja_p2p_set_onVconRecv(JAP2P_HANDLE p2p_handle, bool bRet);
/**
 *  连接指定ID的设备
 *
 *  @param devid    易视网ID
 *  @param connType 已经废弃,指定连接方式,之前是按0,1,2顺序连接[0:直连;1：穿透;2:转发]
 *
 *  @return !0			成功连接P2P句柄(Plat android, will got a negative int)
 *			0			连接失败
 */
P2PSDKClientAPI JAP2P_HANDLE ja_p2p_connect(const char* devid, int connType);

/**
 *  创建p2phandle,用于ja_p2p_connect3连接时候用
 *
 *  @return !0  返回创建的p2p_handle
 *          0   创建失败
 */
P2PSDKClientAPI JAP2P_HANDLE ja_p2p_create_p2phandle(void);


/**
 *  通过分享校验字段连接指定ID的设备，不需要再进行p2p_login操作
 *
 *  @param  p2p_handle  创建的p2p_handle
 *  @param  devid       易视网ID
 *  @param  ipaddr      设备ip地址信息
 *  @param  port        设备端口信息
 *  @param verify_str 从服务器获取到的分享校验字段
 *
 *  @return     0			成功连接
 *				1:			校验超时
 *				2:			用户密码错误
 *              -1:         无效校验字符串/解码校验字串失败
 *              -2:         内部p2p连接失败
 */
P2PSDKClientAPI int  ja_p2p_connect2(JAP2P_HANDLE  p2p_handle,
                                     const char* devid,
                                     const char *ipaddr,
                                     unsigned short port,
                                     const char* verify_str);




/**
 *  使用创建的p2p_handle连接制定的设备，可指定id  或者(ip/port)两种方式中一个
 *
 *  @param p2p_handle 已经创建的p2p_handle
 *  @param devid      易视网id
 *  @param ipaddr     设备ip地址
 *  @param port       设备端口
 *
 *  @return 0          连接成功
 *          -1         连接失败
 *          -8          解析P2P服务器失败
 *          -9          设备离线（服务器数据）
 *          -10         穿透请求超时
 *          -11         穿透失败（无打洞包）
 *          -12         穿透失败（没收到ready）
 */
P2PSDKClientAPI int ja_p2p_connect3(JAP2P_HANDLE  p2p_handle,
                                    const char* devid,
                                    const char *ipaddr,
                                    unsigned short port);
    
/**
 *  获取当前连接的连接协议类型名
 *
 *  @param p2p_handle 已经创建的p2p_handle
 *
 *  @return “BUBBLE" "HOLE" "TRANSFER"
 */
P2PSDKClientAPI const char* ja_p2p_GetConnectProto(JAP2P_HANDLE p2p_handle);
    
/**
 *  获取对应协议连接错误信息
 *
 *  @param p2p_handle 已经创建的p2p_handle
 *  @param proto_name "BUBBLE", "HOLE", "TURN"
 *
 *  @return a string to explain what steps when connect failed
 */
P2PSDKClientAPI const char* ja_p2p_GetConnectStepStr(JAP2P_HANDLE p2p_handle,
                                                     const char* proto_name);


/**
 *  关闭一个连接
 *
 *  @param p2p_handle 已经连接的P2P句柄
 */
P2PSDKClientAPI void ja_p2p_close(JAP2P_HANDLE p2p_handle);

/**
 *  P2P用户校验
 *
 *  @param p2p_handle 已经连接的P2P句柄
 *  @param user       用户名
 *  @param pwd        用户密码
 *
 *  @return 0:			校验成功
 *			1:			校验超时
 *			2:			用户密码错误
 */
P2PSDKClientAPI int ja_p2p_login(JAP2P_HANDLE p2p_handle,
                                 const char* user,
                                 const char* pwd);

    
/**
 *  打开/关闭正在连接设备的码流/切换码流(先停止/再打开另个码流)
 *
 *  @param p2p_handle 已经连接的P2P句柄
 *  @param nChannelId 通道号,从零起
 *  @param nStreamId  码流号,0,主码流;1,子码流;2,网络摄像机的第三个码流
 *  @param bOpen      1,打开码流;0,停止码流
 *
 *  @return 0:			成功
 *			!0:         失败
 */
P2PSDKClientAPI int ja_p2p_open_channel(JAP2P_HANDLE p2p_handle,
                                        int nChannelId,
                                        int nStreamId,
                                        int bOpen);



/**
 *  P2P云台控制
 *
 *  @param p2p_handle 已经连接的P2P句柄
 *  @param nChannel   云台操作通道号
 *  @param act        云台操作动作
 *  @param bStart     1,云台动作开始;0,云台操作停止
 *  @param param1     云台操作参数1
 *  @param param2     云台操作参数2
 *
 *  @return 0:		操作成功
 *			非零：	操作超时
 */

P2PSDKClientAPI int ja_p2p_ptz_ctrl(JAP2P_HANDLE p2p_handle,
                                    int nChannel,
                                    int act,
                                    int bStart,
                                    int param1,
                                    int param2);


/**
 *  P2P获取设备通道数
 *
 *  @param p2p_handle 已经连接的P2P句柄
 *
 *  @return 返回设备的最大通道数
 */
P2PSDKClientAPI int ja_p2p_getChannelCount(JAP2P_HANDLE p2p_handle);


/**
 *  在已经连接的p2p会话上创建虚拟通道
 *
 *  @param p2p_handle 已经连接的p2p句柄
 *  @param sAppname   设备端对应app服务名,例如："spook"
 *         vcon_bufsz:	已经废弃,指定设备虚拟通道接收缓存大小
 *
 *  @return >0:			返回成功创建的虚拟通道句柄
 *			0：			创建虚拟通道失败
 */
P2PSDKClientAPI VCON_HANDLE* ja_p2p_vcon_create(JAP2P_HANDLE p2p_handle,
                                                const char *sAppname);


/**
 *  在成功创建的虚拟通道上发送数据
 *
 *  @param vcon_handle 成功创建的虚拟通道句柄
 *  @param len         数据的长度(字节)
 *  @param buf         待发送数据
 *
 *  @return
 */
P2PSDKClientAPI int ja_p2p_vcon_send(VCON_HANDLE* vcon_handle,
                                     int  len,
                                     void *buf);


/**
 *  销毁已经不在使用的虚拟通道
 *
 *  @param vcon_handle 成功创建的虚拟通道句柄
 *
 *  @return
 */
P2PSDKClientAPI int ja_p2p_vcon_destroy(VCON_HANDLE* vcon_handle);











///////////////////////////////////////////////////////////////////////////////
/////远程回放搜索录像接口
///////////////////////////////////////////////////////////////////////////////
typedef void *PlaybackSearchTaskHandle;


/**
 *  启动p2p的远程回放搜索任务
 *
 *  @param p2p_handle        已经连接的P2P句柄
 *  @param chn               将要搜索的通道集合
 *  @param chn_count         chn[]通道集合元素个数,
 *							例如同时搜索0,1通道, chn[0] = 0,
 *							                chn[1] = 1,
 *							                chn_count = 2
 *  @param recfile_starttime 搜索录像的起始时间/秒，GMT时间
 *  @param recfile_endtime   搜索录像的结止时间/秒，GMT时间
 *  @param recfile_type      录像类型,其中第0位表示定时录像，第1位表示移动录像，
 *                           第2位表示报警录像，第3位表示手动录像
 *
 *  @return 非零:				返回搜索任务句柄
 *			NULL:				任务启动失败
 */

P2PSDKClientAPI PlaybackSearchTaskHandle ja_p2p_rec_FindFileTaskStart(
                                        JAP2P_HANDLE p2p_handle,
                                        char chn[],
                                        int chn_count,
                                        int recfile_starttime,
                                        int recfile_endtime,
                                        int recfile_type);


/**
 *  获取对应搜索任务的录像记录数
 *
 *  @param search_task_handle 搜索任务句柄
 *
 *  @return >0:				   录像记录条数
 *			=0:				   无录像记录
 *          -1:                搜索超时
 *          -2:                参数有误
 */
P2PSDKClientAPI int ja_p2p_rec_FindFileCount(
                    PlaybackSearchTaskHandle search_task_handle);

typedef struct FileRecord{
    int chn;   //录像文件对应通道
    int type;  //录像文件类型
    long starttime;//录像开始时间/秒,GMT格式
    long endtime;  //录像结束时间/秒,GMT格式
}stFileRecord;

//2000 for new device has large recordsss
#define FILE_RECORDS_MAX_CNT 200 // p2p(bubble/stcp/rudp/udx/udx2) but tcpt will use this value
//records -> records(point,dynamic malloc the mem, not the FILE_RECORDS_MAX_CNT any more
typedef struct FileRecords{
    int records_cnt;
    stFileRecord records[FILE_RECORDS_MAX_CNT];
}stFileRecords;

/**
 *  获取录像列表
 *
 *  @param search_task_handle 搜索任务句柄
 *
 *  @return record_count:		0:  无录像记录/没有更多录像记录
 *								>0: 记录条数
 *			stFileRecord[]:		录像记录
 */
P2PSDKClientAPI stFileRecords* ja_p2p_rec_FindFileNext(
                               PlaybackSearchTaskHandle search_task_handle);



/**
 *  关闭对应搜索任务
 *
 *  @param search_task_handle 搜索任务句柄
 *
 *  @return 0:	关闭搜索任务成功
 *			-1:	关闭搜索任务失败
 */
P2PSDKClientAPI int ja_p2p_rec_FindFileTaskClose(
                    PlaybackSearchTaskHandle *search_task_handle);

///////////////////////////////////////////////////////////////////////////////
/////////录像点播接口
///////////////////////////////////////////////////////////////////////////////
typedef void* PlaybackHandle;

/**
 *  录像点播音视频数据回调接口
 *
 *  @param	frame:音视频帧
 *  @param	len:帧长度
 *  @param  type:0:音频，1:I帧 2:P帧
 *  @param	vWidth:视频宽度
 *  @param	vHeight:视频高度
 *  @param	avencode:编码类型,视频：H264/H265,音频：G711
 *  @param	aSampleRate:音频采样率
 *  @param	aSampleWidth:音频采样宽度
 *  @param	aSampleChn:音频采样通道
 *  @param	chn:音视频通道来源
 *  @param	timestamp:音视频时间戳,毫秒
 *  @param	gentime:录像时间/秒,GMT格式
 *  @param	ctx:上下文
 */

typedef void (*fOnRecvRecFrame)(void *frame,
                                int len,
                                int type,
                                int vWidth,
                                int vHeight,
                                const char* avencode,
                                int aSampleRate,
                                int aSampleWidth,
                                int aSampleChn,
                                int chn,
                                JAP2P_UINT64 timestamp,
                                unsigned int gentime,
                                void *ctx);

//TODO 完整属性的回调函数（framerate）

/**
 *  开始回放点播任务
 *
 *  @param p2p_handle        已经连接的P2P句柄
 *  @param chn               回放通道集合
 *  @param chn_count         chn[]通道集合元素个数
 *							 例如同时点播0,1通道, chn[0] = 0,
 *							                   chn[1] = 1,
 *							                   chn_count = 2
 *  @param recfile_starttime 搜索录像的起始时间/秒，GMT时间
 *  @param recfile_endtime   搜索录像的结止时间/秒，GMT时间
 *  @param recfile_type      录像类型,其中第0位表示定时录像，
 *                                  第1位表示移动录像，
 *                                  第2位表示报警录像，
 *                                  第3位表示手动录像
 *  @param OnRecvRecFrame    录像点播音视频数据回调接口
 *  @param ctx               上下文
 *
 *  @return NZ      回放点播句柄
 *          NULL    回放点播失败
 */
P2PSDKClientAPI PlaybackHandle ja_p2p_rec_PlaybackStart(
                                JAP2P_HANDLE p2p_handle,
                                char chn[],
                                int chn_count,
                                int recfile_starttime,
                                int recfile_endtime,
                                int recfile_type,
                                fOnRecvRecFrame OnRecvRecFrame,
                                void *ctx);

/**
 *  开始回放点播任务
 *
 *  @param p2p_handle        已经连接的P2P句柄
 *  @param chn               回放通道集合
 *  @param chn_count         chn[]通道集合元素个数
 *							 例如同时点播0,1通道, chn[0] = 0,
 *							                   chn[1] = 1,
 *							                   chn_count = 2
 *  @param recfile_starttime 搜索录像的起始时间/秒，GMT时间
 *  @param recfile_endtime   搜索录像的结止时间/秒，GMT时间
 *  @param recfile_type      录像类型,其中第0位表示定时录像，
 *                                  第1位表示移动录像，
 *                                  第2位表示报警录像，
 *                                  第3位表示手动录像
 *  @param download          指定是否是下载录像。
 *  @param OnRecvRecFrame    录像点播音视频数据回调接口
 *  @param ctx               上下文
 *
 *  @return NZ      回放点播句柄
 *          NULL    回放点播失败
 */
P2PSDKClientAPI PlaybackHandle ja_p2p_rec_PlaybackStartV2(JAP2P_HANDLE p2p_handle,
                                          char chn[],
                                          int chn_count,
                                          int starttime,
                                          int endtime,
                                          int type,
                                          bool download,
                                          fOnRecvRecFrame OnRecvRecFrame,
                                          void* ctx);


/**
 *  暂停回放点播任务
 *
 *  @param playback_handle 回放点播句柄
 *
 *  @return 0:		成功
 */
P2PSDKClientAPI int ja_p2p_rec_PlaybackPause(PlaybackHandle playback_handle);

/**
 *  继续回放任务
 *
 *  @param playback_handle 回放点播句柄
 *
 *  @return 0:		成功
 */
P2PSDKClientAPI int ja_p2p_rec_PlaybackContinue(PlaybackHandle playback_handle);
    
/**
 *  跳转到新的时间开始点播业务
 *
 *  @param playback_handle   回放点播句柄
 *  @param recfile_starttime 跳转录像的开始时间
 *  @param recfile_endtime   跳转录像的结束时间
 *  @param recfile_type      跳转录像的结束时间
 *
 *  @return 0   成功
 */
P2PSDKClientAPI int ja_p2p_rec_PlaybackSeek(PlaybackHandle playback_handle,
                                            int recfile_starttime,
                                            int recfile_endtime,
                                            int recfile_type);
    
/**
 *  停止回放任务
 *
 *  @param playback_handle 回放点播句柄
 *
 *  @return 0   成功
 */
P2PSDKClientAPI int ja_p2p_rec_PlaybackStop(PlaybackHandle playback_handle);


/**
 *  搜索设备的结果
 *
 *  @member   esee_id:    易视网ID(ipc为空)
 *  @member   ip:         设备ip地址
 *  @member   port:       设备端口
 *  @member   max_ch:     设备最大通道数
 *  @member   dev_model:  设备类型：空/DVR/NVR/IPCAM...
 */
struct SearchDeviceResult{
    char esee_id[32];
    char ip[32];
    unsigned short port;
    unsigned int max_ch;
    char dev_model[32];
};
    
/**
 *  搜索设备回调函数
 *
 *  @param   pResult:    搜索结果
 *  @param   ctx：       上下文
 */
typedef void (*fOnSearchDeviceResult)(struct SearchDeviceResult *pResult,
                                      void *ctx);


    
/**
 *  手机端设备搜索接口,传入搜索设备回调函数，
 *  开始搜索，直到接口调用完毕, 即搜索完毕,如非必要无需重复调用
 *
 *  @param OnSearchDeviceResult 搜索设备回调接口
 *  @param ctx                  上下文
 *
 *  @return 0                       搜索完毕
 *          -1                      搜索任务已经开启
 */

P2PSDKClientAPI int ja_p2p_search_device(
                    fOnSearchDeviceResult OnSearchDeviceResult,
                    void *ctx);


/**
 *  语音对讲拔打接口
 *
 *  @param p2p_handle 已经连接的p2p句柄
 *  @param talkChn    设备端对应的对讲通道号（直播通道号）
 *  @return 0 拔打成功
 *          -1 拔打失败
 *          -2 设备正忙
 */
P2PSDKClientAPI int ja_p2p_vop2p_call(JAP2P_HANDLE p2p_handle, int talkChn);

/**
 *  语音对讲挂断接口
 *
 *  @param p2p_handle 已经连接的p2p句柄
 *
 *  @return 0   成功
 *          -1  失败
 */
P2PSDKClientAPI int ja_p2p_vop2p_hangup(JAP2P_HANDLE p2p_handle);

/**
 *  语音对讲数据发送接口
 *
 *  @param p2p_handle    已经连接的p2p句柄
 *  @param voiceData     语音数据
 *  @param dataSz        语音数据大小
 *  @param tsMs          语音数据时间戳（ms）
 *  @param enc           语音数据编码（G711A)
 *  @param samplerate    语音数据采样率(8k)
 *  @param samplewidth   语音数据采样带宽（8bit)
 *  @param channels      语音数据采样通道（一般为1，单声道）
 *  @param compressRatio 压缩率（2.0)
 *
 *  @return 0   成功
 *          -1  失败
 */
P2PSDKClientAPI int ja_p2p_vop2p_talk_send(JAP2P_HANDLE p2p_handle,
                                           void *voiceData,
                                           size_t dataSz,
                                           JAP2P_UINT64 tsMs,
                                           const char *enc,
                                           int samplerate,
                                           int samplewidth,
                                           int channels,
                                           float compressRatio);
    
    
    


/**
 * 警报拉取接口
 *  @param p2p_handle    已经连接的p2p句柄
 *  @param ch            警报消息对应通道
 *  @param ticket        警报消息时间戳
 *  @param type          警报消息类型
 *
 *  @return 0   成功
 *          -1  未找到资源
 *          -2  其它错误
 */

// alarmMsgTypo
//enum P2PAlarmMsgType{
//    P2P_ALARMMSG_TYPE_TEXT = 0,
//    P2P_ALARMMSG_TYPE_AUDIO = 1,
//    P2P_ALARMMSG_TYPE_SMALL_IMG = 2,
//    P2P_ALARMMSG_TYPE_IMG = 3,
//    P2P_ALARMMSG_TYPE_LARGE_IMG = 4,
//    P2P_ALARMMSG_TYPE_VIDEO = 5,
//};

P2PSDKClientAPI int ja_p2p_pull_alarmmsg(JAP2P_HANDLE p2p_handle,
                                         int type,
                                         int ch,
                                         time_t ticket);


/**
 *  clientType, eg: "EseeCloud.Android/iOS", "VRCam.Android/iOS" "CMS", "EseeWeb" .....
 *  识别不同客户端名字，其实就是bundleid别名
 *
 * IMEI: 手机识别码/UDID
 *
 *  ISP: 移动服务提供商
 *
 *  BundleId: 客户ODM识别码。
 */
/* ja_p2p_set_clienttype 已经废弃， 请使用 ja_p2P_set_clientinfo_type 设置客户端类型名。 */
P2PSDKClientAPI int ja_p2p_set_clienttype(int clientType);
P2PSDKClientAPI int ja_p2p_set_clientinfo_type(const char* clientType);
P2PSDKClientAPI int ja_p2p_set_clientinfo_imei(const char *imei);
P2PSDKClientAPI int ja_p2p_set_clientinfo_isp(const char *isp);
P2PSDKClientAPI int ja_p2p_set_clientinfo_bundleid(const char *bundleid);
P2PSDKClientAPI int ja_p2p_set_clientinfo_version(const char *version);


/**
 * @brief 设置tcpt转发服务器，针对连接。
 * @param addr tcpt服务器地址
 * @param port tcpt服务器端口
 */
P2PSDKClientAPI int ja_p2p_set_tcpt_server(const char* addr, unsigned short port);

/**
 * 用于android/ios 等手机平台切换网络时，重置底层网络。
 * @return 0 成功
 *          -1  失败
 */
P2PSDKClientAPI int ja_p2p_reset_transfer(void);


/**
 * TUTK 在线状态回调
 *  0 设备在线
 *  -90 设备离线
 *  -13 超时，一般时间设置太短
 *  其它 查询失败
 *
 */

typedef void( *fOnCheckTUTKDevOnline)(int result, void * ctx);

/**
 * 检测tutk ID 在线状态
 * @param tutkid    TUTKUID
 * @param timeout   检测超时时间/毫秒
 * @param on        检测事件
 * @param ctx       用户上下文
 * @return
 */
P2PSDKClientAPI int ja_p2p_check_tutk_dev_online(const char* tutkid,
                                                 unsigned int timeout,
                                                 fOnCheckTUTKDevOnline on,
                                                 void* ctx);


/**
 * 设置TUTK 模块日志路径
 * @param iotc_log_path     IOTC 日志路径
 * @param max_iotc_log_sz   最大日志大小/Bytes, 0为不限制大小
 * @param rdt_log_path      RDT 日志路径
 * @param max_rdt_log_sz    最大日志大小/Bytes, 0为不限制大小
 * @return 0 成功
 *          -1 失败，参数有错
 *              ja_p2p_set_tutk_log_path("/tmp/iotc.log", 0, "/tmp/rdt.log", 0);
 *
 */
P2PSDKClientAPI int ja_p2p_set_tutk_log_path(const char* iotc_log_path,
                             int max_iotc_log_sz,
                             const char* rdt_log_path,
                             int max_rdt_log_sz);

    
#ifdef __cplusplus
};
#endif

#endif
