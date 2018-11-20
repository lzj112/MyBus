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

char* GetLocalIp()
{
    int MAXINTERFACES = 16;
    char* ip = NULL;
    int fd, intrface, retn = 0;
    struct ifreq buf[MAXINTERFACES];
    struct ifconf ifc;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t)buf;
        if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc))
        {
            intrface = ifc.ifc_len / sizeof(struct ifreq);

            while (intrface-- > 0)
            {
                if (!(ioctl(fd, SIOCGIFADDR, (char *)&buf[intrface])))
                {
                    ip = (inet_ntoa(((struct sockaddr_in *)(&buf[intrface].ifr_addr))->sin_addr));
                    break;
                }
            }
        }
        close(fd);
        return ip;
    }
}
int main()
{
    cout << GetLocalIp() << endl;
}
