#include <fcntl.h>
#include <assert.h>
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

int socketTCP::initSocketfd(int domain, int type, int protocol) 
{
    this->my_sockfd = socket(domain, type, protocol);
    if (my_sockfd == -1) 
    {
        std::cout << "initSocketfd is failed" << std::endl;
        return -1;
    }
    return 0;
}

int socketTCP::Bind(const char* ip, int port) 
{
    if (ip == nullptr) 
    {
        std::cout << "ip is nullptr in Bind" << std::endl;
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    int res = bind(my_sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if (res == -1) 
    {
        std::cout << "Bind is failed" << std::endl;
        return -1;
    }
    
    return 0;
}

int socketTCP::Listen(int backlog) 
{
    int res = listen(my_sockfd, backlog);
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

    int res = connect(my_sockfd, (struct sockaddr*)&addr, sizeof(addr));
    
    //失败后原来的不可再用了
    if (res == -1) 
    {
        Close(my_sockfd);
        initSocketfd();
        return -1;
    }
    
    return 0;
}

int socketTCP::Accept() 
{
    if (my_sockfd == -1) 
    {
        return -1;
    }

    //对对端的客户协议地址不感兴趣
    int connfd = accept(my_sockfd, nullptr, nullptr);
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