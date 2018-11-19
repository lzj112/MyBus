#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/sem.h>
#include <sys/shm.h>
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

/*
void MyBus::initShmPlane() 
{
    plane.initList();
} 

void MyBus::initPlaneSocket(const char* ip, int port) 
{
    if (ip == nullptr) 
    {
        std::cout << "ip == nullptr in initplanesocket" << std::endl;
        return ;
    }
    plane.socketControl.startListening(ip, port);
}
*/

BusCard* MyBus::initChannelControl(int proj_id) 
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
    
    //初始化两个通信通道
    initShmQueue(cardPtr);

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

    for (int i = 0; i < 2; i++) 
    {
        int keyTmp = (card->ftokKey >> 16) + i;  //防止重复
        int key_ = getKey(keyTmp);
    
        int shmid = ShmManage::Get(keyTmp, sizeof(PacketBody) * QUEUESIZE, IPC_CREAT | 0666);
        
        card->localQueue[0][i] = shmid; //存储两个队列的shmid
        
        shmid = 0;
        keyTmp = 0;
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

/*
int MyBus::getListenFd() 
{
    return plane.socketControl.getMysockfd();
}
*/

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
        ++cardPtr->localQueue[1][flag];
    }
    else 
    {
        //不是本机进程,队列相反
        if (flag == 0) 
        {
            ++cardPtr->localQueue[1][1];    //对端读队列尾指针++
        }
        else 
        {
            ++cardPtr->localQueue[1][0];    //对端写队列尾指针++
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
        ++cardPtr->localQueue[2][flag];
    }
    else 
    {
        //不是本机进程,队列相反
        if (flag == 0) 
        {
            ++cardPtr->localQueue[2][1];
        }
        else 
        {
            ++cardPtr->localQueue[2][0];
        }
    }
    return 1;
}

int MyBus::sendToLocal(BusCard* cardPtr, void* shmMapAddr, const char* buffer, int length) 
{
    if (cardPtr == nullptr) 
    {
        std::cout << "sendtolocal 1" << std::endl;
        return -1;
    }

    int queueFront = getQueueFront(cardPtr, 1); //获得写队列头尾指针
    int queueRear = getQueueRear(cardPtr, 1);
    if ((queueRear + 1) % QUEUESIZE == queueFront) //队列满,牺牲了队列中一个元素保持控来判断队满
    {
        std::cout << "sendtolocal 2" << std::endl;
        return -1;
    }

    {
        std::lock_guard<std::mutex> locker(my_lock);    //上锁
        PacketBody* destPtr = static_cast<PacketBody *> (shmMapAddr);
        if (length >= PacketBodyBufferSize) 
        {
            std::cout << "sendtolocal 3" << std::endl;
            return -1;
        }
        destPtr->head.bodySzie = length;
        strncpy(destPtr->buffer, buffer, length);
    
        addQueueRear(cardPtr, 1);   //移动队尾指针
    }
}

int MyBus::recvFromLocal(BusCard* cardPtr, void* shmMadAddr, char* buffer, int length) 
{
    if (cardPtr == nullptr) 
    {
        std::cout << "recvlocal 1" << std::endl;
        return -1;
    }

    int queueFront = getQueueFront(cardPtr, 0); //获得读队列头尾指针
    int queueRear = getQueueRear(cardPtr, 0);
    if (queueFront == queueRear) //queue is empty
    {
        std::cout << "recvlocal 2" << std::endl;
        return -1;
    }

    {
        std::lock_guard<std::mutex> locker(my_lock);    //上锁

        PacketBody* sourcePtr = static_cast<PacketBody *> (shmMadAddr);
        strncpy(buffer, (sourcePtr + queueFront)->buffer, length);

        addQueueFront(cardPtr, 1);  //移动队头指针
    }
}
