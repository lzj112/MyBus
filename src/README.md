# 基于共享内存的进程间通信 MyBus

---

## 项目功能

- 实现本机和跨主机间的进程间通信

## 项目大概思路

每一个主机上运行有一个中转进程, 本机进程间通信直接写入共享内存, 跨主机的进程间通信, 先写入共享内存, 再通知本机中转进程, 中转进程将数据发往目标进程所在主机上的中转进程, 对端中转进程收到数据后写入共享内存再通知目标进程读取

## 头文件功能描述

|  头文件 | 功能  |
| ------------ | ------------ |
| BusInfo.h  | 主要用到的数据结构  |
| Epoll.h  |  epoll |
| MyBus.h  | 负责进程间通信的逻辑处理  |
| NetComm.h  |  负责中转进程的逻辑处理 |
| ProTabMgt.h  |  管理进程通道表 |
| RouTabMgt.h  |  管理路由表 |
| socketTCP.h  | 封装 socket API  |
| socketBus.h |  继承自 socketTCP, 处理数据的接收和发送	|

## 项目流程

- 首先通过 `BusCard* MyBus::initChannelControl()`初始化获得`MyBus`控制块`BusCard`

-  通过`void MyBus::prepareSocket()`初始化 TCPsocket 和 UDPsocket
> 跨主机通信时必需, 同主机间通信不用



- 通过`int MyBus::recvFromNetwork`   `void MyBus::sendByNetwork`  `int MyBus::recvFromLocal`  `int MyBus::sendToLocal` 来进行跨主机之间和本机之间的进程间通信

- ### 本机通信

获得`BusCard`中存储的本机通信使用的共享内存队列, 如果队列未满, 就将数据插入到队列尾部, 满则抛弃, 接收进程从同一个共享内存中读取数据
`MyBus`没有通知机制, 接收方自己负责何时从共享内存队列读取数据

- ### 跨主机通信

本机进程 A 先将数据写入本机共享内存队列, 写好后以 `UDP` 向本机中转进程 B 发送通知, B 拿到共享内存中的数据后建立 `TCP` 连接发往目标进程 D 所在主机的中转进程 C, C 拿到数据后将数据写入本机共享内存队列, 然后以 `UDP` 通知 D, D 再从共享内存中拿到数据

- ### 进程通道表和路由表

这两个表都是中转进程的 
1. 进程通道表存储上述的数据来源的进程 A 和目标进程 D 的对应关系,  区别不同通信双方的数据缓冲区
2. 路由表存储发来数据的中转进程的 IP 和 port, 在发现本机进程期望发向这个进程时, 不用建立新的`TCP`连接, 而是可以使用已有的连接

- ### 定时器处理

使用 `timerfd` 定时器并使用`epoll`监听, 进程通道表管理类和路由表管理类中都有定时器, 设置触发时间, 定期检查两个表有没有存满, 如果存储空间接近耗完, 就释放之前保存的


- ### 接受新连接

为防止连接淤积采用循环读取

```
    int connfd = -1;
    struct sockaddr_in client;
    socklen_t cliLength = sizeof(client);
    //防止连接淤积
    while (1)
    { 
        memset(&client, 0, cliLength);   
        connfd = accept(listenFd, (sockaddr *)&client, &cliLength); //读取新连接
        if (connfd <= 0) 
        {
            break;
        }
        else 
        {
            setNonblock(connfd);  //设置为非阻塞
            Add(connfd, EPOLLIN);       //将新的连接socketfd添加到合集
        }
    }
    
```

- ### 接收消息

防止没有读完
```
int NetComm::getMessage(int connfd, PacketBody* buffer, int length) 
{
    int count = length;
    int ret = 0;
    while (count > 0) //循环读取直到读完指定字节或出错
    {
        ret = recv(connfd, buffer, length, 0);
        if (ret == -1) 
        {
            if (errno == EINTR || errno == EWOULDBLOCK) 
            {
                ret = 0;
                continue;
            }
            else 
            { 
                perror("一个连接断开\n");
                return -1;
            }
        }
        if (ret == 0) 
        {
            myEpoll.Del(connfd);
            close(connfd);
            count = -1;
            break;
        }
        count -= ret;
        buffer += ret;
    }
    return count;
}
```

- ### 建立新连接

防止`connect`失败
```
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
```


---

# 项目反思

**设计上存在不合理之处 :**

1. 本机间进程通信, 如果该进程想和另一个本机进程通信就要重新创建`BusCard`, 因为里面存储的本机通信使用共享内存队列没有做到标示不同目标进程, 如果接收进程没有及时收数据可能就会有别的进程拿到这个数据了, 
2. 因为`BusCard` 存储在共享内存中, 我假设的是本机间通信双方都知道共享内存 ID, 这块可能可以改成使用配置文件好一点把, 如果是跨主机通信, 会使用`UDP`告知中转进程存放数据的共享内存 ID (以及中转进程收到数据后告知目标进程数据存放地点的共享内存 ID), 
3. 包括跨主机的进程间通信时, 要提供通信双方以及双方所在主机中转进程的 IP 和 port, 总计两对四元组, 参数过于复杂, 也应该换成配置文件的做法
4. 定时器到期如果存储空间满了我选择的简单的前移头指针, 控制访问到的位置来扩大存储区域, 但是这样一个是之前存储的也有可能是在频繁使用的一个连接或者进程通道, 最近存储的反而是不用的废弃连接和进程通道, 如果要改进的话, 考虑时间轮, 让他们以活跃程度排序
5. C++使用上的不足, 面向对象的特性把握的还是不够, 在对对象的封装上还有很多不合理之处, 比如`epoll`的封装
> 建议阅读未来知名业界大佬[娄神](https://github.com/hepangda)的代码以学习

