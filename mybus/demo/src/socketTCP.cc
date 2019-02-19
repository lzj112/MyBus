#include <fcntl.h>
#include <assert.h>
#include <memory.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <iostream>

#include "socketTCP.h" 

int socketTCP::setNonBlock(int fd) 
{
    //设置文件描述符为非阻塞
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);

    return 0;
}

void socketTCP::initSocketTCP() 
{
    fdTCP = socket(AF_INET, SOCK_STREAM, 0);
    if (fdTCP == -1) 
    {
        perror("socket fdtcp");
    }
    setNonBlock(fdTCP);
}

void socketTCP::initSocketUDP() 
{
    fdUDP = socket(AF_INET, SOCK_DGRAM, 0);
    if (fdUDP == -1) 
    {
        perror("socket fdudp");
    }
}

int socketTCP::Bind(const char* ip, int port) 
{
    if (ip == nullptr) 
    {
        std::cout << "ip is nullptr in Bind" << std::endl;
        return -1;
    }
    printf("bind ip == %s port = %d\n", ip, port);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);
    int optval = 1;
    setsockopt(fdTCP, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    int res = bind(fdTCP, (struct sockaddr*)&addr, sizeof(addr));
    if (res == -1) 
    {
        perror("bind fdtcp");
    }
 
    res = bind(fdUDP, (struct sockaddr*)&addr, sizeof(addr));
    if (res == -1) 
    {
        perror("fdudp bind");
    }
    
    return 0;
}


int socketTCP::Listen(int backlog) 
{
    int res = listen(fdTCP, backlog);
    if (res == -1) 
    {
        std::cout << "Listen is failed" << std::endl;
    }
    return 0;
}

int socketTCP::Connect(const char* ip, int port) 
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    int res = connect(fdTCP, (struct sockaddr*)&addr, sizeof(addr));
    
    //失败后原来的不可再用了
    if (res == -1) 
    {
        Close(fdTCP);
        initSocketTCP();
        return -1;
    }
    
    return 0;
}

int socketTCP::Accept() 
{
    //对对端的客户协议地址不感兴趣
    int connfd = accept(fdTCP, nullptr, nullptr);
    if (connfd == -1) 
    {
        std::cout << "Accept is failed" << std::endl;
    }

    return connfd;
}

int socketTCP::Close(int fd) 
{
    int res = close(fd);
    if (res == -1) 
    {
        std::cout << "Close is failed";
        return -1;
    }
    return 0;
}

int socketTCP::Shutdown(int fd, int how) 
{
    int res = shutdown(fd, how);
    if (res == -1) 
    {
        std::cout << "Shutdown is failed" << std::endl;
    }

    return 0;
}