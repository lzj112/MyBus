#include <iostream>

#include "src/MyBus.h"
#include "src/NetComm.h"

using namespace std;

const char* ip = "127.0.0.1";
int port = 4096;

int main() 
{
    NetComm plane;

    plane.initList(1);
    plane.prepareSocket(ip, port);
    plane.runMyEpoll();

}