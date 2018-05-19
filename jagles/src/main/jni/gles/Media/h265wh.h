#ifndef __H265WH_H__
#define __H265WH_H__

#define FFMIN(a,b) ((a) > (b) ? (b) : (a))

/*return: 0 正常从sps中拿到宽高信息, -1失败
 *param:  stream : 码流数据
 *		  stream_len : 码流长度(byte)，必须给定
 *        width/height:存放宽高信息
 *
 */
 #ifdef __cplusplus
extern "C" {
#endif
int H265GetWidthHeight(char *stream,int stream_len,int *width,int *height);
#ifdef __cplusplus
};
#endif
#endif