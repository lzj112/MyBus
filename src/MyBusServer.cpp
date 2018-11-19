// #include <iostream>

#include "MyBus.h" 
using namespace std;

const char* ip = "127.0.0.1";
int port = 4097;
int main() 
{
    // MyBus station;
    NetComm plane;

    plane.initList();
    plane.socketControl.startListening(ip, port);

    plane.myEpoll.Create(plane.getListenFd());
    plane.myEpoll.Run();
}