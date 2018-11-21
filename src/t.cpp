#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
using namespace std;

#define read 0


int main()
{
    int flag = 0;
    if (flag & read) 
    {
        cout << "as" << endl;
    }
    else 
    {
        cout << "sa" << endl;
    }
}
