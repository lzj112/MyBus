#include <assert.h>
// #include <sys/types.h>
#include <arpa/inet.h>
// #include <netinet/in.h>

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
    Connect(ip, port);
    int count = 0;
    int ret = 0;
    int length = sizeof(buffer);
    
    int sockfd = getMysockfd();
    while (count < length) 
    {
        ret = send(sockfd, (void *)&buffer, sizeof(buffer), 0);
        if (ret == -1 && errno != EAGAIN) 
        {
            break;
        }
        count += ret;
    }
    return count;
}

int socketBus::makeNewConn(const PacketBody& str) 
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
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
    } while (res == -1);

    res = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    
}

//跨物理器发送数据
int socketBus::sendTo(const PacketBody& str) 
{

}