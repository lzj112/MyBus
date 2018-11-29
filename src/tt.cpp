#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <net/if.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/shm.h>
#include <time.h>
#include <cstdlib>
#include <assert.h>

#include <iostream>
#include <vector> 
#include <thread>
#include <mutex>

#include "BusInfo.h"
using namespace std;

const char* ip = "127.0.0.1";
int port = 4100;

int main() 
{
    int udpfd = socket(AF_INET, SOCK_DGRAM, 0);


    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    // int optval = 1;
    // setsockopt(fdTCP, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    int res = bind(udpfd, (struct sockaddr*)&addr, sizeof(addr));
    if (res == -1) 
    {
        perror("bind fdudp");
    }

    printf("udpfd = = %d\n", udpfd);
    printf("brfore the recvfrom\n");

    int length = sizeof(char) * 10;
    char buf[10];
    
    int ret = recvfrom(udpfd, (void *)&buf, length, 0, nullptr, nullptr);
    perror("emmmm");
    if (ret < 0) 
    {
        perror("recvfrom is wrong");
    }
    else 
    {
        printf("recvfrom got something = %s\n", buf);
    }
}