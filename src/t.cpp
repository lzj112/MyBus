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

bool check()
{
    union t 
    {
        int a;
        char b;
    };
    t tmp;
    tmp.a = 0x12345678;
    if (tmp.b == 0x78) 
    {
        cout << "little end" << endl;
    }
}

int main() 
{
    check();
}
