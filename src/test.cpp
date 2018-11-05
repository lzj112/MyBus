#include <sys/sem.h>

#include <iostream>

using namespace std;

int main() 
{
    void* t;
    int a = -1;
    t = &a;
    if (*(int *)t == -1) 
    {
        cout << "asasa" << endl;
    }    
}
