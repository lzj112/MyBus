#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <net/if.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <time.h>
#include <cstdlib>
#include <assert.h>

#include <iostream>
#include <vector> 
#include <thread>
#include <mutex>

#include "AllocPort.h"


using namespace std;

const char* ip = "127.0.0.1";
int port = 5000;

int main() 
{

    int fdTCP = socket(AF_INET, SOCK_STREAM, 0);
    int fdUDP = socket(AF_INET, SOCK_DGRAM, 0);
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
