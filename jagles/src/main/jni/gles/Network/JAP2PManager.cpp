//
//  JAP2PManager.cpp
//  iOS
//
//  Created by Chan Jason on 2017/5/6.
//  Copyright © 2017年 LiHong. All rights reserved.
//

#include "JAP2PManager.h"
#include "list.h"
pthread_mutex_t g_listLock = PTHREAD_MUTEX_INITIALIZER;
List <JAP2PManager *> g_ListManager;

bool g_InitP2P_V1 = false;
bool g_InitP2P_V2 = false;

kp2p_callback_t g_Callback_V2;
TJACallBack g_Callback_V1;

bool GetP2PConnector(JAP2PManager *mgn)
{
    //return true;
    for(int i=0;i<g_ListManager.length();i++)
        if(g_ListManager.at(i) == mgn) {
            pthread_mutex_unlock(&g_listLock);
            return true;
        }
    return false;
}

JAP2PManager *GetP2PConnector(long handle)
{
    pthread_mutex_lock(&g_listLock);
    for(int i=0;i<g_ListManager.length();i++)
        if(g_ListManager.at(i)->GetHandle() == handle) {
            pthread_mutex_unlock(&g_listLock);
            return g_ListManager.at(i);
        }
    pthread_mutex_unlock(&g_listLock);
    return NULL;
}

bool JAP2PManager::CompareConnectStr(const char *devid, const char *ipaddr, unsigned short port)
{
    if(strlen(mDeviceID)==0 && strlen(mIP)==0)
        return false;
    if(strlen(mDeviceID)==0)
        return strcasecmp(ipaddr,mIP)==0 && mPort == port;
    else
        return strcasecmp(devid,mDeviceID)==0;
}

void JAP2PManager::UpdateProperty(char *devid, char *ipaddr, int port) {
    if(devid)
        strncpy(mDeviceID,devid,20);
    if(ipaddr)
        strncpy(mIP,ipaddr,80);
    if(port)
        mPort = port;
}
