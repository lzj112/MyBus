#include <sys/shm.h>

#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>

using namespace std;


struct a 
{
    int a;
    int b;
    char unused[4];
};
int main() 
{
  struct a A;
  cout << sizeof(A) << endl;
}
