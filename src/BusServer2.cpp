#include <iostream>

#include "NetComm.h"

using namespace std;

const char* ip = "127.0.0.1";
int port = 4097;

int main() 
{
    NetComm plane;

    plane.initList();
    plane.socketControl.startListening(ip, port);
    plane.runMyEpoll();

}