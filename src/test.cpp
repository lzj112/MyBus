#include <sys/sem.h>

#include <iostream>

using namespace std;

struct test 
{
    int a;
    test() 
    {
        cout << "Asasa" << endl;
    }
    ~test() 
    {
        cout << "asqqqqqqqqqqq" << endl;
    }
};
int main() 
{
    test* tmp = new test();
    delete tmp;    
}
