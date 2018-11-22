#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
using namespace std;

#define read 0


//进程间通信需要的地址
struct ProComm 
{
    char sourceIP[8];
    int sourcePort;
    int destPort;
    char destIP[8];
};

//进程间通信的结构
struct PacketHead  
{
    int type;
    int bodySzie;
    char unused[4]; //用于对齐 无意义
};

struct PacketBody
{
    struct PacketHead head;
    struct ProComm netQuaad;    //四元组 确定对端IP port
    char buffer[256];
};

//通知中转进程
struct Notice 
{
    struct PacketHead head;
    struct ProComm netQueaad;
    int shmid;
};
int main()
{
   cout << "head = " << sizeof(PacketHead) << endl;
   cout << "notice = " <<  sizeof(Notice) << endl;
   cout << "notice - 12 = " << sizeof(Notice) - 12 << endl;
   cout << "32" << endl;
}
