//
// Created by LiHong on 16/3/3.
// Copyright (c) 2016 LiHong. All rights reserved.
//

#ifndef IOS_JATEXT_H
#define IOS_JATEXT_H


class JAText {
public:
    JAText();
    ~JAText();
    void LoadFont(char *path);
    unsigned char *GenTextbuffer(char *txt,unsigned int color,int pw,int ph);

private:
    unsigned char *ascbuffer;
    unsigned char *hzbuffer;
    void putpixels(int x,int y,unsigned int color,unsigned char *buf,int w,int h);
    void PutText(char *pStr,unsigned int color,unsigned char *buf,int w,int h);
};


#endif //IOS_JATEXT_H
