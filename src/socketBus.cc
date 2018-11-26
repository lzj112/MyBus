#include <assert.h>
#include <memory.h>
#include <arpa/inet.h>

#include "socketBus.h" 
#include "AllocPort.h"

void socketBus::startListening(const char* ip, int port) 
{
    Bind(ip, port);
    Listen(10);
}

//向中转进程发送通知
int socketBus::sendTo(const char* ip, int port, Notice buffer) 
{  
std::cout << "中转进程ip = " << ip << "端口 : "  << port << std::endl;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd != -1);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);
    
    int on=1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_BROADCAST, &on, sizeof(on));

    int ret = sendto(sockfd, (void *)&buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1) 
    {
        perror("sendto");
    }
std::cout << "向中转进程发送通知fa song wanbi" << std::endl;
}


//跨物理器发送数据
int socketBus::sendTo(const PacketBody& str, int connfd) 
{
std::cout << "跨物理机发送" << std::endl;
    //使用的阻塞socket
    int res = send(connfd, (void *)&str, sizeof(str), 0);
    return res;
}

int socketBus::makeNewConn(const char* destIP, int destPort) 
{
    int ret = 0;
    int sockfd = 0;
    do 
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        assert(sockfd != -1);

        //保证给中转进程提供唯一端口号
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        inet_pton(AF_INET, destIP, &addr.sin_addr);
        addr.sin_port = htons(destPort);
        int res = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
        if (ret == -1) 
        {   
            perror("connect");
            close(sockfd);
        }

    } while (ret == -1);    //防止connect失败
    return sockfd;
}


void socketBus::inform(const char* ip, int port, int id) 
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd != -1);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    int buffer = id;
    sendto(sockfd, (void *)&buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, sizeof(addr));
}