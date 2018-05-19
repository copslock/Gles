//#include "StdAfx.h"
#include <stdio.h>
#include <math.h>

#include "h265wh.h"

#ifndef __APPLE__
const unsigned char *avpriv_find_start_code(const unsigned char *p,
									  const unsigned char *end,
									  unsigned int *state)
{
	int i;

	if (p >= end)
		return end;

	for (i = 0; i < 3; i++) {
		unsigned int tmp = *state << 8;
		*state = tmp + *(p++);
		if (tmp == 0x100 || p == end)
			return p;
	}

	while (p < end) {
		if      (p[-1] > 1      ) p += 3;
		else if (p[-2]          ) p += 2;
		else if (p[-3]|(p[-1]-1)) p++;
		else {
			p++;
			break;
		}
	}

	p = FFMIN(p, end) - 4;

	return p + 4;
}

unsigned int Ue_265(unsigned char *pBuff, unsigned int nLen, unsigned int *nStartBit)
{
	//计算0bit的个数
	unsigned int nZeroNum = 0;
	while ((*nStartBit) < nLen * 8)
	{
		if (pBuff[(*nStartBit) / 8] & (0x80 >> ((*nStartBit) % 8))) //&:按位与，%取余
		{
			break;
		}
		nZeroNum++;
		(*nStartBit)++;
	}
	(*nStartBit) ++;

	//计算结果
	unsigned int dwRet = 0;
	unsigned int i;
	for (i=0; i<nZeroNum; i++)
	{
		dwRet <<= 1;
		if (pBuff[(*nStartBit) / 8] & (0x80 >> ((*nStartBit) % 8)))
		{
			dwRet += 1;
		}
		(*nStartBit)++;
	}
	return (1 << nZeroNum) - 1 + dwRet;
}

unsigned int u_265(unsigned int BitCount,unsigned char * buf,unsigned int *nStartBit)
{
	unsigned int dwRet = 0;
	unsigned int i;
	for (i=0; i<BitCount; i++)
	{
		dwRet <<= 1;
		if (buf[(*nStartBit) / 8] & (0x80 >> ((*nStartBit) % 8)))
		{
			dwRet += 1;
		}
		(*nStartBit)++;
	}
	return dwRet;
}

void profile_tier_level(unsigned char * buf,unsigned int ulLen,unsigned int *startbit,int sps_max_sub_layers_minus1)
{
	int i;
	(*startbit) += 96;
	char sub_layer_profile_present_flag[64];
	char sub_layer_level_present_flag[64];
	for (i = 0; i < sps_max_sub_layers_minus1 -1;i++)
	{
		sub_layer_profile_present_flag[i] = (char)u_265(1,buf,startbit);
		sub_layer_level_present_flag[i] = (char)u_265(1,buf,startbit);
	}

	if (sps_max_sub_layers_minus1 - 1 > 0)
	{
		for (i = sps_max_sub_layers_minus1 - 1; i < 8; i++)
		{
			(*startbit) += 2;
		}
	}

	for (i = 0; i < sps_max_sub_layers_minus1 - 1;i++)
	{
		if (sub_layer_profile_present_flag[i])
		{
			(*startbit) += 88;
		}
		if (sub_layer_level_present_flag[i])
		{
			(*startbit) += 8;
		}
	}
}

char sps_d(unsigned char * buf,unsigned int ulLen,int * nWidth,int *nHeight)
{
	unsigned int nstartBit = 0;
	unsigned int sps_video_parameter_set_id = u_265(4,buf,&nstartBit);
	if (sps_video_parameter_set_id >= 16)
	{
		return 0;
	}
	unsigned int sps_max_sub_layers_minus1 = u_265(3,buf,&nstartBit) + 1;
	if (sps_max_sub_layers_minus1 > 7)
	{
		return 0;
	}
	unsigned int sps_temporal_id_nesting_flag = u_265(1,buf,&nstartBit);
	profile_tier_level(buf,ulLen,&nstartBit,sps_max_sub_layers_minus1);
	unsigned int sps_seq_parameter_set_id = Ue_265(buf,ulLen,&nstartBit);
	if (sps_seq_parameter_set_id >= 32)
	{
		return 0;
	}
	unsigned int chroma_format_id = Ue_265(buf,ulLen,&nstartBit);
	if (chroma_format_id != 1)
	{
		return 0;
	}
	char separate_colour_plane_flag;
	if (3 == chroma_format_id)
	{
		separate_colour_plane_flag = (char)u_265(1,buf,&nstartBit);
	}
	*nWidth = Ue_265(buf,ulLen,&nstartBit);
	*nHeight = Ue_265(buf,ulLen,&nstartBit);
	return 1;
}

#else
extern const unsigned char *avpriv_find_start_code(const unsigned char *p,
									  const unsigned char *end,
									  unsigned int *state);
extern unsigned int Ue_265(unsigned char *pBuff, unsigned int nLen, unsigned int *nStartBit);
extern unsigned int u_265(unsigned int BitCount,unsigned char * buf,unsigned int *nStartBit);
extern void profile_tier_level(unsigned char * buf,unsigned int ulLen,unsigned int *startbit,int sps_max_sub_layers_minus1);
extern char sps_d(unsigned char * buf,unsigned int ulLen,int * nWidth,int *nHeight);
#endif
int H265GetWidthHeight(char *stream,int stream_len,int *width,int *height)
{
	int nul_unit_type;
	unsigned char * buf,* buf_end;
	buf = (unsigned char *)stream;
	buf_end = (unsigned char *)stream + stream_len - 1;
	int src_length;
	char bFound = 0;
	// 查找头
	while (1)
	{
		int stat = -1;
		buf = (unsigned char *)avpriv_find_start_code(buf,buf_end,(unsigned int *)&stat);
		if (--buf + 2 >= buf_end)
			break;
		nul_unit_type = (*buf >> 1) & 0x3f;
		int temporal_id = (*(buf + 1) & 0x07) - 1;
		src_length = buf_end - buf;

		if (33 == nul_unit_type)
		{
			bFound = 1;
			break;
		}
	}

	if (bFound)
	{
		char dBuffer[256];
		int nDSize = src_length > 256 ? 256 : src_length;
		unsigned int ndbt = 0;
		int i;
		for (i = 2;i < nDSize;i++)
		{
			if (i+2 < nDSize && 0x00 == buf[i] && 0x00 == buf[i + 1]
			&& 0x03 == buf[i + 2])
			{
				dBuffer[ndbt++] = buf[i];
				dBuffer[ndbt++] = buf[i + 1];
				i += 2;
			}
			else
			{
				dBuffer[ndbt++] = buf[i];
			}
		}
		if (sps_d((unsigned char *)dBuffer,ndbt,width,height))
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

