//
// Created by afei9 on 2017-7-5.
//

#ifndef ANDROID_JADEFINE_H
#define ANDROID_JADEFINE_H

#ifdef _MSC_VER
#include <windows.h>
#else
#include <unistd.h>
#endif

#define JA_CONNECT_ERROR_SUCCESS 0
#define JA_CONNECT_ERROR_TIMEOUT 1
#define JA_CONNECT_ERROR_USER 2
#define JA_CONNECT_ERROR_UNKNOW 100

#define JA_CONNECT_LOGIN_TIMEOUT 20

void ja_sleep(int ms)
{
#ifdef _MSC_VER
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}
#endif //ANDROID_JADEFINE_H
