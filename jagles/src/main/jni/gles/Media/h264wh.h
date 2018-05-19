#ifndef __H264WH_H__
#define __H264WH_H__
/*return: 0 ������sps���õ������Ϣ, -1ʧ��
 *param:  stream : ��������
 *		  stream_len : ��������(byte)���������
 *        width/height:��ſ����Ϣ
 *
 */
#ifdef __cplusplus
extern "C" {
#endif
int GetWidthHeight(char *stream,int stream_len,int *width,int *height);
int h264_decode_seq_parameter_set(unsigned char  * buf,unsigned int nLen,int *Width,int *Height);
    
#ifdef __cplusplus
};
#endif
#endif
