#include <sys/sem.h>

#include <iostream>

using namespace std;

class A{
public:
    void b(int a = 10);
    void c(int a);
};

void A::b(int a) 
{
    cout << a << endl;
}

void A::c(int a = 10) 
{
    cout << a << endl;
}

int main() 
{
    // A a;
    // a.b(12);
    // a.c(12);
    cout << IPC_CREAT << ' ' << IPC_RMID << ' '
     << IPC_SET << ' ' << IPC_STAT << endl;
}
