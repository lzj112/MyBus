#include <sys/shm.h>
#include <assert.h>

#include "RouTabMgt.h" 
#include "socketBus.h"


RouTabMgt::~RouTabMgt() 
{
    shmctl(rouTabList[0], IPC_RMID, nullptr);
}

int RouTabMgt::initTime() 
{
    return timing.startTimerfd();
}

int RouTabMgt::getTimerfd() 
{
    return timing.getTimerfd();
}

int RouTabMgt::initNetShmList(int id)  //初始化路由表
{
    key_t key = ftok(PATH, id + 1);
    if (key == -1) 
    {
        std::cout << "initShmListHead() is failed 1" << std::endl;
        return -1;
    }
    rouTabList[0] = shmget(key, sizeof(RoutingTable) * QUEUESIZE, IPC_CREAT | 0666);
    if (rouTabList[0] == -1) 
    {
        rouTabList[0] = shmget(key, 0, 0);
        if (rouTabList[0] == -1) 
        {
            perror("shmget in initshmlist");
            return -1;
        }
    }
    rouTabList[1] = 0;
    rouTabList[2] = 0;
    return 0;
}
//创建共享内存
int RouTabMgt::creShmQueue(int proj_id) 
{
    key_t key = ftok(PATH, proj_id);
    assert(key != -1);
    int shmid = shmget(key, sizeof(PacketBody) * QUEUESIZE, IPC_CREAT | 0666);
    if (shmid == -1) 
    {
        shmid = shmget(key, 0, 0);
        if (shmid == -1) 
        {
            perror("shmget in creshmrouTabList");
        }
    }
    return shmid;  
}
//更新路由表
int RouTabMgt::updateList(int sockfd, const char* ip, int port) 
{
    RoutingTable* tmpAddr = static_cast<RoutingTable *> (shmat(rouTabList[0], nullptr, 0)) + rouTabList[2];
    if ((rouTabList[2] + 1) % QUEUESIZE == rouTabList[1]) 
    {
        return -1;
    }
    memset(tmpAddr, 0 ,sizeof(RoutingTable));

    tmpAddr->sockfd = sockfd;
    tmpAddr->port = port;
    strcpy(tmpAddr->IP, ip);
    tmpAddr->isReadable = true;

    rouTabList[2]  = (rouTabList[2] + 1) % QUEUESIZE;   //尾加一
    shmdt(tmpAddr);
    return 0;
}

//寻找是否已有连接
int RouTabMgt::isThereConn(const char* ip, int port) 
{
    RoutingTable* nodePtr = static_cast<RoutingTable *> (shmat(rouTabList[0], nullptr, 0)) + rouTabList[1];

    int connfd = -1;
    for (int i = rouTabList[1];;) 
    {
        if (rouTabList[2] == i) 
        {
            break;
        }   
        //找到这个连接且这个连接没有修改
        if ((strcmp((nodePtr + i)->IP, ip) == 0) && 
            (nodePtr + i)->port == port && nodePtr->isReadable == true) 
        {
            connfd = (nodePtr + i)->sockfd;
            break;
        }         
        i = (i + 1) % QUEUESIZE;
    }

    return connfd;
}

void RouTabMgt::delNode(int connfd) 
{
    std::lock_guard<std::mutex> locker(myLock);
    RoutingTable* nodePtr = static_cast<RoutingTable *> (shmat(rouTabList[0], nullptr, 0)) + rouTabList[1];

    for (int i = rouTabList[1];;) 
    {
        if (rouTabList[2] == i) 
        {
            break;
        }   
        //找到这个连接且这个连接没有修改
        if ((nodePtr + i)->sockfd == connfd && nodePtr->isReadable == true) 
        {
            connfd = -1;
            (nodePtr + i)->isReadable = false;
            break;
        }         
        i = (i + 1) % QUEUESIZE;
    }
}

void RouTabMgt::check() 
{

    socketBus::readTime(timing.getTimerfd());
    if (((rouTabList[2] - rouTabList[1] + QUEUESIZE) % QUEUESIZE) >= 
        (QUEUESIZE - 10))   //如果路由表剩余空间只剩下不足十个位置
    {
        reOrder();
    }    
}

//释放保存的前一半连接
void RouTabMgt::reOrder() 
{
    
    std::lock_guard<std::mutex> locker(myLock);
   
    RoutingTable* tmpAddr = static_cast<RoutingTable *> (shmat(rouTabList[0], nullptr, 0)) + rouTabList[1];
    int front = rouTabList[1];
    int rear = rouTabList[2];
    int size = ((rear - front + QUEUESIZE) % QUEUESIZE) / 2;
    memset(tmpAddr, 0, size);
    rouTabList[1] = (rouTabList[1] + size) % QUEUESIZE;

    printf("释放旧连接\n");
}
