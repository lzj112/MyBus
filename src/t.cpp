#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
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

int main() 
{
    int port;
    do 
    {
        AllocPort tmp;
        port = tmp.getPort();
        cout << port << endl;
    } while (port != 2048);


}