#include <cstring>
#include <sys/sem.h>

#include <iostream>

using namespace std;
int main() 
{
    int tmp = 0;
    {
        int tmp;
        tmp = 10;
    }
    cout << tmp << endl;
}