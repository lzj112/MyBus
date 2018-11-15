#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
using namespace std;

struct tmp 
{
    int value;
    int shmidNext;
};

struct ttmp 
{
    int a;
    char c;
    string b;
};

template<class T>
void isSame(const T& str) 
{
    if (typeid(str) == typeid(struct tmp))
    {
        cout << "111111111" << endl;
        cout << T.value;
    }
    else if (typeid(str) == typeid(struct ttmp)) 
    {
        cout << "22222" << endl;
    }
 
}

int main() 
{
    struct tmp a;
    isSame(a);
    struct ttmp b;
    isSame(b);
}
