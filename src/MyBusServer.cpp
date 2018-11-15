#include "MyBus.h" 

const char* ip = "127.0.0.1";
int port = 4097;
int main() 
{
    MyBus station;
    station.initShmPlane();
    station.initPlaneSocket(ip, port); 
}