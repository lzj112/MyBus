#include <sys/shm.h>

#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>

using namespace std;



int main() 
{
    void* tmp = shmat(123, nullptr, 0);
    if (tmp == (void *)-1) 
    {
        cout << "asasasas" << endl;
    }
}
