#include <assert.h>
#include <memory.h>
#include <arpa/inet.h>

#include "socketBus.h" 
#include <AllocPort.h>

void socketBus::startListening(const char* ip, int port) 
{
    initSocketfd();
    Bind(ip, port);
    Listen(10);
}

//向中转进程发送通知
int socketBus::sendTo(const char* ip, int port, Notice buffer) 
{  
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd != -1);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    sendto(sockfd, (void *)&buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, sizeof(addr));

}

int socketBus::makeNewConn(const PacketBody& str) 
{
    int ret = 0;
    int sockfd = 0;
    do 
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        assert(sockfd != -1);

        //保证给中转进程提供唯一端口号
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        inet_pton(AF_INET, str.netQuaad.sourceIP, &addr.sin_addr);
        int port; 
        int res = 0;
        do 
        {
            AllocPort t;
            port = t.getPort();
            addr.sin_port = htons(port);
            res = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
        } while (res == -1);    //防止port被占用

        ret = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
        if (ret == -1) 
        {
            close(sockfd);
        }
    } while (ret == -1);    //防止connect失败

    return sockfd;
}

//跨物理器发送数据
int socketBus::sendTo(const PacketBody& str, int connfd) 
{
    //使用的阻塞socket
    int res = send(connfd, (void *)&str, sizeof(str), 0);
    return res;
}