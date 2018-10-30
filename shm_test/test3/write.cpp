#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <map>
#include <errno.h>

using namespace std;

class test 
{
public:
    test() 
    {
        x = 10;
        test_Tmp.insert(make_pair(1, "this is one"));
        test_Tmp.insert(make_pair(2, "this is two"));
        test_Tmp.insert(make_pair(3, "this is three"));
    }
    void show() 
    {
        cout << x << endl;;
    }
    void Insert(int x) 
    {
        test_Tmp.insert(make_pair(x, "this is x"));
    }
private:
    int x;
    map<int, string> test_Tmp;
};

int main() 
{
    test my_test;
    int shmid = shmget((key_t)1234, 1024, IPC_CREAT | 0644);
    assert(shmid != -1);

    void* pAddr = nullptr;
    pAddr = shmat(shmid, nullptr, 0);
    
}