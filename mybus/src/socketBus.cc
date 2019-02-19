#include <assert.h>
#include <arpa/inet.h>
#include <sys/shm.h>
#include <memory.h>

#include "socketBus.h" 

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
        return -1;
    }
    close(sockfd);
    return 0;
}


//跨物理器发送数据 使用阻塞套接字
int socketBus::sendTo(const PacketBody& str, int connfd) 
{
std::cout << "向另一个物理机转发=" << str.buffer << std::endl;
    int res;
    do 
    {
        res = send(connfd, (void *)&str, sizeof(str), 0);
        if (res == -1) 
        {
            if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) 
            {
                continue;
            }
            else 
            {
                break;
            }
        }
        else 
        {
            break;
        }
    } while (1);
    return res;
}



//接收收到跨物理机发送的消息后中转udp通知本进程
void socketBus::recvFrom(void* buf, int length) 
{
    int udpfd = getMysockUDP(); 

    errno = 0;
    recvfrom(udpfd, buf, length, 0, nullptr, nullptr);
    perror("recvfrom ");
}

void socketBus::readTime(int connfd) 
{
    uint64_t exp;
    read(connfd, &exp, sizeof(uint64_t));
}

int socketBus::makeNewConn(const char* destIP, int destPort) 
{
    int res = 0;
    int sockfd = 0;
    do 
    {
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        assert(sockfd != -1);
        // setNonBlock(sockfd);
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        inet_pton(AF_INET, destIP, &addr.sin_addr);
        addr.sin_port = htons(destPort);
        int res = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
        if (res == -1) 
        {   
            perror("connect");
            close(sockfd);
        }

    } while (res == -1);    //防止connect失败
    return sockfd;
}


void socketBus::inform(const char* ip, int port, int* tmp) 
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    int buffer[2];
    buffer[0] = tmp[0];
    buffer[1] = tmp[1];

    sendto(sockfd, (void *)&buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, sizeof(addr));

    close(sockfd);
}