#ifndef _ROUTABMGT_H_
#define _ROUTABMGT_H_

#include <mutex>

#include "Timer.h"
#include "BusInfo.h"

class RouTabMgt 
{
public:
    ~RouTabMgt();
    //初始化定时器
    int initTime();
    int getTimerfd();
    //初始化路由表
    int initNetShmList(int id);
    //创建共享内存
    int creShmQueue(int proj_id);
    //更新路由表
    int updateList(int sockfd, const char* ip, int port);
    int isThereConn(const char* ip, int port);
    void check();
    void delNode(int connfd);
    void reOrder();

private:
    Timer timing;  //定时器
    int rouTabList[3];   //依次存储shmid 头尾指针
    std::mutex myLock;
};

#endif