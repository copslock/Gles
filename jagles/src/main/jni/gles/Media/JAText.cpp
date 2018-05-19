//
// Created by LiHong on 16/3/3.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "JAText.h"

JAText::JAText()
{
    hzbuffer = NULL;
    ascbuffer = NULL;
}

JAText::~JAText()
{
    if(hzbuffer)
    {
        free(hzbuffer);
        hzbuffer =NULL;
    }
    if(ascbuffer)
    {
        free(ascbuffer);
        ascbuffer = NULL;
    }
    
}

void JAText::LoadFont(char *path)
{
    FILE *fp;
    char fhz[255];
    char fasc[255];
    sprintf(fasc,"%s/asc24.font",path);
    sprintf(fhz,"%s/hzk24.font",path);
    fp = fopen(fasc, "rb");
    if(fp==NULL)
    {
        printf("error open file %s\n",fasc);
        return;
    }
    fseek(fp,0, SEEK_END);
    long size = ftell(fp);
    fseek(fp,0,SEEK_SET);
    ascbuffer = (unsigned char *)malloc(size);
    fread((void*)ascbuffer, size, 1, fp);
    fclose(fp);
    fp = fopen(fhz, "rb");
    if(fp==NULL)
    {
        printf("error open file %s\n",fhz);
        return;
    }
    fseek(fp,0, SEEK_END);
    size = ftell(fp);
    fseek(fp,0,SEEK_SET);
    hzbuffer = (unsigned char *)malloc(size);
    fread((void*)hzbuffer, size, 1, fp);
    fclose(fp);
}

unsigned char *JAText::GenTextbuffer(char *txt,unsigned int color,int w,int h)
{
    unsigned char *txtbuf;
	if (!txt)
		return NULL;
    txtbuf = (unsigned char *)calloc(1,w*h*4);
//    for(int y=0;y<h;y++)
//        for(int x=0;x<w;x++)
//            putpixels(x,y,0xffffff,txtbuf,w,h);
    PutText(txt,color,txtbuf,w,h);
    return txtbuf;
}

void JAText::putpixels(int x,int y,unsigned int color,unsigned char *buf,int w,int h)
{
    if(x<0||x>w||y<0||y>h)
        return;
    unsigned int *lbuf =(unsigned int *) buf;
    *(lbuf+y*w+x) = color;
}

void JAText::PutText(char *pStr,unsigned int color,unsigned char *buf,int pw,int ph)
{
    unsigned char *p;
    int hzpos;
    int q,w;
    unsigned char *hzbuf;
    int i,j,m;
    unsigned char s,e;
    unsigned int lbuf;
    i=0;
    int x = 0;
	if (!ascbuffer || !hzbuffer)
		return;
	int llen = strlen(pStr);
	if (llen > 40)
		llen = 40;
    while(i<llen)
    {
        p=(unsigned char *)&pStr[i];
        if (*p>0xA0)
        {
            q=(*p & 0x7f)-32;
            w=(*(p+1) & 0x7f)-32;
            hzpos=((q-1)*94+w-1);
            hzpos=hzpos*72;
            hzbuf=&hzbuffer[hzpos];
            for(j=0;j<24;j++)
            {
                lbuf=(hzbuf[j*3] << 16) |  ((unsigned char)hzbuf[j*3+1] << 8) | (unsigned char)hzbuf[j*3+2];
                for(m=0;m<24;m++)
                {
                    if (((lbuf >> (23-m)) & 1)==1)
                        putpixels(x+m,j,color,buf,pw,ph);
                    else
                        putpixels(x+m,j, 0x66666666,buf,pw,ph);
                }
            }
            i=i+2;
            x=x+24;
        }
        else
        {
            hzpos=*p * 36;
            hzbuf=&ascbuffer[hzpos];
            for(j=0;j<24;j++)
            {
                for(m=0;m<12;m++)
                {
                    if ((hzbuf[(j*12+m)/8] >> (7-(j*12+m)%8))&0x01==0x01)
                        putpixels(x+m,j,color,buf,pw,ph);
                    else
                        putpixels(x+m,j,0x66666666,buf,pw,ph);
                }
            }
            i++;
            x+=12;
        }
    }
}

