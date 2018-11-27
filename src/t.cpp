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

int main() 
{
    cout << sizeof(PacketBody) << endl;
}