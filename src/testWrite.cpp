#include <assert.h>
#include <string.h>
#include <sys/types.h>

#include <iostream>

#include "MyBus.h" 

using namespace std;

int main() 
{

    MyBus station;
    key_t key = station.getKey(10);

    BusCard* cardPtr = station.initChannelControl(key);

    void* 
}