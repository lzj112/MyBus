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
using namespace std;

class a 
{
public:
    a(const char* p) 
    {
      strcpy(c, p);
    }
    int b;
    char c[8];
};

int main() 
{
    const char* p = "123456";
    struct a tmp(p);
    // struct a tmp;
    tmp.b = 1;
    // strcpy(tmp.c, p);
    cout << tmp.c << endl;
}