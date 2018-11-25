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

void t() 
{
    static int a= 0;
    int b = 0;
    printf("%d %d\n", ++a, ++b);
    printf("address  ==  %p\n", &a);
}

int main() 
{
    char x = -2, y = 3;
    char t = (++x) | (y++);
    printf("%d %d %d\n", x, y ,t);
    t = (++x) || (y++);
    // cout << x << ' ' << y << ' ' << t << endl;
    printf("%d %d %d\n", x, y ,t);
}
