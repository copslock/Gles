#ifndef __H265WH_H__
#define __H265WH_H__

#define FFMIN(a,b) ((a) > (b) ? (b) : (a))

/*return: 0 ������sps���õ������Ϣ, -1ʧ��
 *param:  stream : ��������
 *		  stream_len : ��������(byte)���������
 *        width/height:��ſ����Ϣ
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