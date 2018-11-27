#include <assert.h>
#include <arpa/inet.h>
#include <sys/shm.h>
#include <memory.h>

#include "socketBus.h" 
#include "AllocPort.h"

void socketBus::startListening(const char* ip, int port) 
{
    Bind(ip, port);
    Listen(10);
}

//向本机中转进程发送通知
int socketBus::sendTo(const char* ip, int port, Notice buffer) 
{  
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
}


//跨物理器发送数据
int socketBus::sendTo(const PacketBody& str, int connfd) 
{
std::cout << "向另一个物理机转发=" << str.buffer << std::endl;
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
        // setNonBlock(sockfd);

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

printf("海牙!\n");
    proToNetqueue* t = static_cast<proToNetqueue *> (shmat(id, nullptr, 0));
    if (t == (void *)-1) 
    {
        perror("asasasasas");
    }
    int id = t->netQueue[0];
    printf("向本机通知%s已经到位\n", t->buffer);
}