#include <cstdlib>
#include <cstring>
#include <net/if.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <iostream>

#include "MyBus.h"

MyBus::MyBus()
{
}

MyBus::~MyBus()
{
}

//获取当前工作目录
char *MyBus::getPath(char *buffer, size_t size)
{
    buffer = getcwd(buffer, size);

    return buffer; //getcwd失败返回的也是nullptr
}

//ftok获得key
key_t MyBus::getKey(int proj_id, char *in_case_path)
{
    char buffer[50];
    if (in_case_path != nullptr) //如果用户提供了文件目录
    {
        strcpy(buffer, in_case_path);
    }

    char *path = getPath(buffer, (size_t)50);
    
    key_t keyTmp = ftok(buffer, proj_id); //获得key
    if (keyTmp == -1)
    {
        perror("ftok failed : ");
        exit(1);
    }
    return keyTmp;
}


BusCard* MyBus::initChannelControl(int proj_id, const char* ip, int port) 
{
    key_t key = getKey(proj_id);
    //开辟存储控制块的共享内存
    int shmid = ShmManage::Get(key, sizeof(BusCard), IPC_CREAT | 0666);
    std::cout << "Channel Control shm_id = " << shmid << std::endl;

    //挂载到当前进程
    BusCard* cardPtr = static_cast<BusCard *> (ShmManage::At(shmid, nullptr, 0));

    cardPtr->shmSelfId = shmid;
    cardPtr->ftokKey = key;
    cardPtr->localQueue[0][2] = (int)getpid();
    
    //初始化通信通道
    initShmQueue(cardPtr);

    //初始化socket
    prepareSocket(ip, port);
    return cardPtr;
}

BusCard* MyBus::getChannelControl(int shmid) 
{
    //挂载到当前进程
    return static_cast<BusCard *>(ShmManage::At(shmid, nullptr, 0));
}


int MyBus::initShmQueue(BusCard* card) 
{
    if (card == nullptr) 
    {
        return -1;
    }

    for (int i = 0; i < 3; i++) 
    {
        int keyTmp = (card->ftokKey >> 16) + i;  //防止重复
        int key_ = getKey(keyTmp);
    
        int shmid = ShmManage::Get(keyTmp, sizeof(PacketBody) * QUEUESIZE, IPC_CREAT | 0666);
        if (i == 2) 
        {
            card->netQueue[0] = shmid;
            continue;
        }
        card->localQueue[0][i] = shmid; //存储两个队列的shmid
        
    }
    return 0;
}



void* MyBus::getLocalQueue(BusCard* cardPtr, int flag) //flag=0期望读队列,=1期望写队列
{
    if (cardPtr == nullptr || flag > 1 || flag < 0) 
    {
        return nullptr;
    }

    int shmid;
    int pid = (int)getpid();
    if (pid == cardPtr->localQueue[0][2]) //是创建队列的进程
    {
        shmid = cardPtr->localQueue[0][flag];
    }
    else 
    {
        //不是本机进程,队列相反
        shmid = (flag == 0) ? cardPtr->localQueue[0][1] : cardPtr->localQueue[0][0];
    }
    
    //将该队列挂载到当前进程
    return ShmManage::At(shmid, nullptr, 0);
}


void MyBus::prepareSocket(const char* ip, int port) 
{
    socketControl.initSocketfd();
    socketControl.Bind(ip, port);
}

int MyBus::getQueueFront(BusCard* cardPtr, int flag) 
{
    if (cardPtr == nullptr || flag > 1 || flag < 0 ) 
    {
        return -1;
    }
    
    int shmid;
    int front;
    int pid = (int)getpid();
    if (pid == cardPtr->localQueue[0][2]) //是创建队列的进程
    {
        front = cardPtr->localQueue[1][flag];
    }
    else 
    {
        //不是本机进程,队列相反
        front = (flag == 0) ? cardPtr->localQueue[1][1] : cardPtr->localQueue[1][0];
    }
    return front;
}

int MyBus::getQueueRear(BusCard* cardPtr, int flag) 
{
    if (cardPtr == nullptr || flag > 1 || flag < 0 ) 
    {
        return -1;
    }
    int shmid;
    int rear;
    int pid = (int)getpid();
    if (pid == cardPtr->localQueue[0][2]) //是创建队列的进程
    {
        rear = cardPtr->localQueue[2][flag];
    }
    else 
    {
        //不是本机进程,队列相反
        rear = (flag == 0) ? cardPtr->localQueue[2][1] : cardPtr->localQueue[2][0];
    }
    return rear;
}

int MyBus::addQueueFront(BusCard* cardPtr, int flag) 
{
    if (cardPtr == nullptr || flag > 1 || flag < 0 ) 
    {
        return -1;
    }
    
    int pid = (int)getpid();
    if (pid == cardPtr->localQueue[0][2]) //是创建队列的进程
    {
        cardPtr->localQueue[1][flag] = (cardPtr->localQueue[1][flag] + 1) % QUEUESIZE;
    }
    else 
    {
        //不是本机进程,队列相反
        if (flag == READ) 
        {
            //对端读队列头指针++
            cardPtr->localQueue[1][1] = (cardPtr->localQueue[1][1] + 1) % QUEUESIZE;
        }
        else 
        {
            //对端写队列头指针++
            cardPtr->localQueue[1][0] = (cardPtr->localQueue[1][0] + 1) % QUEUESIZE;
        }
    }
    return 1;
}

int MyBus::addQueueRear(BusCard* cardPtr, int flag) 
{
    if (cardPtr == nullptr || flag > 1 || flag < 0 ) 
    {
        return -1;
    }
    
    int pid = (int)getpid();
    if (pid == cardPtr->localQueue[0][2]) //是创建队列的进程
    {
        cardPtr->localQueue[2][flag] = (cardPtr->localQueue[2][flag] + 1) % QUEUESIZE;
    }
    else 
    {
        //不是本机进程,队列相反
        if (flag == READ) 
        {
            cardPtr->localQueue[2][1] = (cardPtr->localQueue[2][1] + 1) % QUEUESIZE;
        }
        else 
        {
            cardPtr->localQueue[2][0] = (cardPtr->localQueue[2][0] + 1) % QUEUESIZE;
        }
    }
    return 1;
}

char* MyBus::getLocalIP() 
{
    int MAXINTERFACES = 16;
    char* ip = NULL;
    int fd, intrface, retn = 0;
    struct ifreq buf[MAXINTERFACES];
    struct ifconf ifc;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)buf;
        if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
        {
            intrface = ifc.ifc_len / sizeof(struct ifreq);

            while (intrface-- > 0)
            {
                if (!(ioctl(fd, SIOCGIFADDR, (char *)&buf[intrface])))
                {
                    ip = (inet_ntoa(((struct sockaddr_in *)(&buf[intrface].ifr_addr))->sin_addr));
                    break;
                }
            }
        }
        close(fd);
        return ip;
    }
}

int MyBus::sendToLocal(BusCard* cardPtr, const char* buffer, int length) 
{
    if (cardPtr == nullptr) 
    {
        std::cout << "sendtolocal 1" << std::endl;
        return -1;
    }

    int queueFront, queueRear;
    {   
        //获得写队列头尾指针
        std::lock_guard<std::mutex> locker(my_lock);
        queueFront = getQueueFront(cardPtr, WRITE); 
        queueRear = getQueueRear(cardPtr, WRITE);
    }
    if ((queueRear + 1) % QUEUESIZE == queueFront) //队列满,牺牲了队列中一个元素保持空来判断队满
    {
        std::cout << "sendtolocal 2" << std::endl;
        return -1;
    }
    
    PacketBody* destPtr = (static_cast<PacketBody *> (getLocalQueue(cardPtr, WRITE))) + queueRear;
    if (length >= PacketBodyBufferSize) 
    {
        std::cout << "sendtolocal 3" << std::endl;
        return -1;
    }
    destPtr->head.bodySzie = length;
    strncpy(destPtr->buffer, buffer, length);

    {
        std::lock_guard<std::mutex> locker(my_lock);
        addQueueRear(cardPtr, WRITE);   //移动队尾指针
    }
    ShmManage::Dt(destPtr);
}

int MyBus::recvFromLocal(BusCard* cardPtr, char* buffer, int length) 
{
    if (cardPtr == nullptr) 
    {
        std::cout << "recvlocal 1" << std::endl;
        return -1;
    }

    int queueFront, queueRear;
    {   
        //获得写队列头尾指针
        std::lock_guard<std::mutex> locker(my_lock);
        queueFront = getQueueFront(cardPtr, READ); 
        queueRear = getQueueRear(cardPtr, READ);
        std::cout << "before " << queueFront << ' ' << queueRear;
    }
    if (queueFront == queueRear) //queue is empty
    {
        std::cout << "recvlocal 2" << std::endl;
        return -1;
    }

    PacketBody* sourcePtr = (static_cast<PacketBody *> (getLocalQueue(cardPtr, READ))) + queueFront;
    strncpy(buffer, (sourcePtr + queueFront)->buffer, length);

    {
        std::lock_guard<std::mutex> locker(my_lock);    
        addQueueFront(cardPtr, READ);  //移动队头指针

    }
    ShmManage::Dt(sourcePtr);
}

int MyBus::saveLocalMessage(BusCard* card, const char* buffer) 
{
    if (card == nullptr) 
    {
        std::cout << "sendtolocal 1" << std::endl;
        return ;
    }

    int queueFront = card->netQueue[1];
    int queueRear = card->netQueue[2];
    
    if ((queueRear + 1) % QUEUESIZE == queueFront) //队列满,牺牲了队列中一个元素保持空来判断队满
    {
        std::cout << "snetqueue is full" << std::endl;
        return ;
    }
    
    PacketBody* destPtr = (static_cast<PacketBody *> (ShmManage::At(card->netQueue[0], nullptr, 0))) + queueRear;
    
    strcpy(destPtr->buffer, buffer);

    card->netQueue[2] = (card->netQueue[2] + 1) % QUEUESIZE; 

    ShmManage::Dt(destPtr);

    return queueFront;
}


int MyBus::sendByNetwork(BusCard* card, const char* passIP, int passPort,int sourcePort, 
                        const char* destIP, int destPort, const char* p, int length)
{
    //存储进共享内存发送缓冲区
    int front = saveLocalMessage(card, p);

    Notice tmp;
    tmp.head.type = READY;
    const char* sourceIP = getLocalIP();
    strcmp(tmp.netQueaad.sourceIP, sourceIP);
    strcmp(tmp.netQueaad.destIP, destIP);
    tmp.netQueaad.destPort = destPort;
    tmp.netQueaad.sourcePort = sourcePort;
    tmp.shmid = card->netQueue[0];
    tmp.front = front;

    //向中转进程发送通知
    socketControl.sendTo(destIP, destPort, tmp);
}