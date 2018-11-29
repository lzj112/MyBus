#include <cstdlib>
#include <cstring>
#include <net/if.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ipc.h>
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
void MyBus::getPath(char *buffer, size_t size)
{
    buffer = getcwd(buffer, size);
}

//ftok获得key
key_t MyBus::getKey(int proj_id, char *in_case_path)
{
    char buffer[50];
    if (in_case_path != nullptr) //如果用户提供了文件目录
    {
        strcpy(buffer, in_case_path);
    }

    getPath(buffer, (size_t)50);
    
    key_t keyTmp = ftok(buffer, proj_id); //获得key
    if (keyTmp == -1)
    {
        perror("ftok failed : ");
        exit(1);
    }
    return keyTmp;
}


BusCard* MyBus::initChannelControl(int proj_id) 
{
    key_t key = getKey(proj_id);
    //开辟存储控制块的共享内存
    int shmid = shmget(key, sizeof(BusCard), IPC_CREAT | 0666);
    if (shmid == -1) 
    {
        //是否是已经创建,尝试获取
        shmid = shmget(key, 0, 0);
        if (shmid != -1) 
        {
            std::cout << "shmid == " << shmid << std::endl;
        }
    }

    //挂载到当前进程
    BusCard* cardPtr = static_cast<BusCard *> (shmat(shmid, nullptr, 0));
    memset(cardPtr, 0, sizeof(BusCard));

    cardPtr->shmSelfId = shmid;
    cardPtr->ftokKey = key;
    cardPtr->localQueue[0][2] = (int)getpid();

    //初始化通信通道
    initShmQueue(cardPtr);

    return cardPtr;
}

BusCard* MyBus::getChannelControl(int shmid) 
{
    //挂载到当前进程
    return static_cast<BusCard *>(shmat(shmid, nullptr, 0));
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
        int key = getKey(keyTmp);
        int shmid = shmget(key, sizeof(PacketBody) * QUEUESIZE, IPC_CREAT | 0666);
        if (shmid == -1) 
        {
            shmid = shmget(keyTmp, 0, 0);
            if (shmid == -1)
            {
                perror("shmget error");
            }
        }
        PacketBody* tmpAddr = static_cast<PacketBody *> (shmat(shmid, nullptr, 0));
        if (tmpAddr == (void *)-1) 
        {
            perror("shmat is error");
        }
        memset(tmpAddr, 0, sizeof(PacketBody) * QUEUESIZE);
        shmdt(tmpAddr);
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
    return shmat(shmid, nullptr, 0);
}


void MyBus::prepareSocket(const char* ip, int port) 
{
    socketControl.initSocketTCP();
    socketControl.initSocketUDP();
    socketControl.startListening(ip, port);
}

int MyBus::getQueueFront(BusCard* cardPtr, int flag) 
{
    if (cardPtr == nullptr || flag > 1 || flag < 0 ) 
    {
        return -1;
    }
    
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

int MyBus::sendToLocal(BusCard* cardPtr, const char* buffer, int length) 
{
    int queueFront, queueRear;
    {   
        //获得写队列头尾指针
        std::lock_guard<std::mutex> locker(my_lock);
        queueFront = getQueueFront(cardPtr, WRITE); 
        queueRear = getQueueRear(cardPtr, WRITE);
    }
    if ((queueRear + 1) % QUEUESIZE == queueFront) //队列满,牺牲了队列中一个元素保持空来判断队满
    {
        return -1;
    }
    
    PacketBody* destPtr = (static_cast<PacketBody *> (getLocalQueue(cardPtr, WRITE))) + queueRear;
    if (length >= PacketBodyBufferSize) 
    {
        return -2;
    }
    destPtr->head.bodySzie = length;
    strncpy(destPtr->buffer, buffer, length);

    {
        std::lock_guard<std::mutex> locker(my_lock);
        addQueueRear(cardPtr, WRITE);   //移动队尾指针
    }
    shmdt(destPtr);
    return 0;
}

int MyBus::recvFromLocal(BusCard* cardPtr, char* buffer, int length) 
{
    int queueFront, queueRear;
    {   
        //获得写队列头尾指针
        std::lock_guard<std::mutex> locker(my_lock);
        queueFront = getQueueFront(cardPtr, READ); 
        queueRear = getQueueRear(cardPtr, READ);
    }
    if (queueFront == queueRear) 
    {
        std::cout << "queue is empty" << std::endl;
        return -1;
    }

    PacketBody* sourcePtr = (static_cast<PacketBody *> (getLocalQueue(cardPtr, READ))) + queueFront;
    strncpy(buffer, sourcePtr->buffer, length);
    memset(sourcePtr->buffer, 0, sizeof(sourcePtr->buffer));
    {
        std::lock_guard<std::mutex> locker(my_lock);    
        addQueueFront(cardPtr, READ);  //移动队头指针

    }
    shmdt(sourcePtr);
    return 0;
}

void MyBus::saveLocalMessage(BusCard* card, const char* buffer) 
{
    int queueFront = card->netQueue[1];
    int queueRear = card->netQueue[2];
    if ((queueRear + 1) % QUEUESIZE == queueFront) //队列满,牺牲了队列中一个元素保持空来判断队满
    {
        std::cout << "netqueue is full" << std::endl;
        return ;
    }
    PacketBody* destPtr = (static_cast<PacketBody *> (shmat(card->netQueue[0], nullptr, 0)) + queueRear);
    if (destPtr == (void *)-1) 
    {
        perror("shmat");
    }

    memset(destPtr->buffer, 0, sizeof(destPtr->buffer));
    strcpy(destPtr->buffer, buffer);
    
    card->netQueue[2] = (card->netQueue[2] + 1) % QUEUESIZE; 

    shmdt(destPtr);
}


void MyBus::sendByNetwork(BusCard* card, const ProComm& str, const char* buffer, int length)
{
    //存储进共享内存发送缓冲区
    saveLocalMessage(card, buffer);

    Notice tmp;
    tmp.head.type = READY;
    tmp.netQueaad = str;
    tmp.shmid = card->shmSelfId;
    tmp.head.bodySzie = length;

    //向中转进程发送通知
    socketControl.sendTo(str.sourcePassIP, str.sourcePassPort, tmp);

}


int MyBus::recvFromNetwork(/*const char* ip, int port,*/char* buffer) 
{
    int buf[2] = {0};
    socketControl.recvFrom(buf, sizeof(buf));
    proToNetqueue* tmpAddr = static_cast<proToNetqueue *> (shmat(buf[0], nullptr, 0)) + buf[1];
    if (tmpAddr == (void *)-1) 
    {
        perror("shmat in recvfrom");
        return 0;
    }

    //empty queue
    if (tmpAddr->readQueue[2] == tmpAddr->readQueue[1]) 
    {
        shmdt(tmpAddr);
    }
    else 
    {
    printf("queue is not empty : %d\n", tmpAddr->readQueue[0]);
        PacketBody* tmpBuf = static_cast<PacketBody *> (shmat(tmpAddr->readQueue[0], nullptr, 0));
        if (tmpBuf == (void *)-1) perror("readqueue id is wrong\n");
        strcpy(buffer, tmpBuf->buffer);
        tmpAddr->readQueue[1] = (tmpAddr->readQueue[1] + 1) % QUEUESIZE;
        shmdt(tmpBuf);
        shmdt(tmpAddr);
    }
    return 0;
}

void MyBus::release(int shmid) 
{
    int res = shmctl(shmid, IPC_RMID, nullptr);
    if (res == -1) 
    {
        perror("release is wrong");
    }
}

void MyBus::releaseAll(BusCard* card) 
{
    int id[4];
    id[0] = card->shmSelfId;
    id[1] = card->localQueue[0][0];
    id[2] = card->localQueue[0][1];
    id[3] = card->netQueue[0];
    for (int i = 3; i >= 0; i--) 
    {
        int res = shmctl(id[i], IPC_RMID, nullptr);
        if (res == -1) 
        {
            perror("releaseAll ");
        }
    }
}