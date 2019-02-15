#include <sys/shm.h>

#include "socketBus.h"
#include "ProTabMgt.h" 

int ProTabMgt::initProShmList(int id) //初始化中转通道
{
    key_t key = ftok(PATH, id + 2);
    if (key == -1) 
    {
        std::cout << "initShmListHead() is failed 2" << std::endl;
        return -1;
    }

    proTabList[0] = shmget(key, sizeof(proToNetqueue) * QUEUESIZE, IPC_CREAT | 0666);
    if (proTabList[0] == -1) 
    {
        proTabList[0] = shmget(key, 0, 0);
        if (proTabList[0] == -1)
        {
            perror("shmget in initshmlist");
        }
    }
    proTabList[1] = 0;
    proTabList[2] = 0;

    return 0;
}

int ProTabMgt::initTime(int firstTime, int interval) 
{
    return timing.startTimerfd(firstTime, interval);
}

int ProTabMgt::getTimerfd() 
{
    return timing.getTimerfd();
}

int ProTabMgt::creShmQueue(int proj_id) 
{
    key_t key = ftok(PATH, proj_id);

    int shmid = shmget(key, sizeof(PacketBody) * QUEUESIZE, IPC_CREAT | 0666);
    if (shmid == -1) 
    {
        shmid = shmget(key, 0, 0);
        if (shmid == -1) 
        {
            perror("shmget in creshmqueue");
        }
    }

    return shmid;
}

//进程通道表节点存储的是对端中转进程和发往的本端目标进程的ip port
int ProTabMgt::updateList(const struct ProComm& str) 
{
    std::lock_guard<std::mutex> locker(myLock);
    int rear = proTabList[2];
    proToNetqueue* tmpAddr = static_cast<proToNetqueue *> (shmat(proTabList[0], nullptr, 0)) + proTabList[2];
    if ((proTabList[2] + 1) % QUEUESIZE == proTabList[1])  //通道表满,放弃新的进程通信
    {
        return -1;
    }
    
    //赋值新节点 翻转,source端存自己,dest端存对面的
    tmpAddr->offset = proTabList[2];
    tmpAddr->destPort = str.sourcePort;
    tmpAddr->sourcePort = str.destPort;
    strcpy(tmpAddr->sourceIP, str.destIP);
    strcpy(tmpAddr->destIP, str.sourceIP);
    tmpAddr->readQueue[0] = creShmQueue(str.destPort | READ);
    tmpAddr->readQueue[1] = 0;
    tmpAddr->readQueue[2] = 0;

    
    proTabList[2] = (proTabList[2] + 1) % QUEUESIZE;   //尾加一

    shmdt(tmpAddr);
    return rear;
}

int ProTabMgt::getProShmQueue(const struct ProComm& str) 
{
    proToNetqueue* nodePtr = static_cast<proToNetqueue *> (shmat(proTabList[0], nullptr, 0)) + proTabList[1];
    if (nodePtr == (void *)-1) 
    {
        perror("shmat ");
        return -1;
    }
    int queueID = -1;
    for (int i = proTabList[1]; ;) 
    {
        if (i == proTabList[2]) 
        {
            break ;
        }
        if ((strcmp((nodePtr + i)->sourceIP, str.destIP) == 0) && 
            (strcmp((nodePtr + i)->destIP, str.sourceIP) == 0) &&
            (nodePtr + i)->sourcePort == str.destPort &&
            (nodePtr + i)->destPort == str.sourcePort)
        {
            queueID = (nodePtr + i)->offset;
            break;
        }
        i = (i + 1) % QUEUESIZE;
    }
    return queueID;
}

//将收到的数据写入共享内存缓冲区
void ProTabMgt::copy(PacketBody* ptr, const PacketBody& str) 
{
    ptr->head.type = str.head.type;
    strcpy(ptr->buffer, str.buffer);
    ptr->head.bodySzie = str.head.bodySzie;
    ptr->netQuaad.destPort = str.netQuaad.destPort;
    ptr->netQuaad.sourcePort = str.netQuaad.sourcePort;
    strcpy(ptr->netQuaad.sourceIP, str.netQuaad.sourceIP);
    strcpy(ptr->netQuaad.destIP, str.netQuaad.destIP);
}

//收到其他物理机发来的数据,将消息存入共享内存, shmid是进程通道表的对应节点的readqueueid
void ProTabMgt::saveMessage(int offset, const PacketBody& str) 
{
    proToNetqueue* tmpAddr = static_cast<proToNetqueue *> (shmat(proTabList[0], nullptr, 0)) + offset;
    if (tmpAddr == (void *)-1) 
    {
        perror("shmat in savemessage in savemessage");
        return ;
    }
    //读取队列是本机上进程通过此通道读取其他进程发来的信息
    //读取队列的头尾指针
    int front = tmpAddr->readQueue[1];
    int rear = tmpAddr->readQueue[2];

    //队列满 抛弃数据
    if ((rear + 1) % QUEUESIZE == front) 
    {
        return ;
    }
    
    //读队列是进程会用这个通道读取其他进程发来的数据
    PacketBody* queueAddr = (static_cast<PacketBody *> (shmat(tmpAddr->readQueue[0], nullptr, 0))) + rear;
    copy(queueAddr, str);
    tmpAddr->readQueue[2] = (tmpAddr->readQueue[2] + 1) % QUEUESIZE;
}

void ProTabMgt::check() 
{
    socketBus::readTime(timing.getTimerfd());
    if (((proTabList[2] - proTabList[1] + QUEUESIZE) % QUEUESIZE) >= 
        (QUEUESIZE - 10))   //如果路由表剩余空间只剩下不足十个位置
    {
        reOrder();
    }  
}

void ProTabMgt::reOrder() 
{
    std::lock_guard<std::mutex> locker(myLock);
   
    // proToNetqueue* tmpAddr = static_cast<proToNetqueue *> (shmat(proTabList[0], nullptr, 0)) + proTabList[1];
    int front = proTabList[1];
    int rear = proTabList[2];
    int size = ((rear - front + QUEUESIZE) % QUEUESIZE) / 2;
    // memset(tmpAddr, 0, size);
    proTabList[1] = (proTabList[1] + size) % QUEUESIZE;
}


void ProTabMgt::realseAll() 
{
    proToNetqueue* nodePtr = static_cast<proToNetqueue *> (shmat(proTabList[0], nullptr, 0)) + proTabList[1];
    
    for (int i = proTabList[1]; ;) 
    {
        if (i == proTabList[2]) 
        {
            break ;
        }
        shmctl((nodePtr + i)->readQueue[0], IPC_RMID, nullptr);
        i = (i + 1) % QUEUESIZE;
    }
    shmdt(nodePtr);
    shmctl(proTabList[0], IPC_RMID, nullptr);
}