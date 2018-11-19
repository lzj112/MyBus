#include <iostream>
#include <vector>
#include <thread>

using namespace std;
class t  
{
public:
    vector<shared_ptr<thread> > t1;
public:
    t() 
    {
        for (int i = 0; i < 3; i++) 
        {
            t1.push_back(make_shared<thread>(t::Func, this)); //(1)
        }
    }
    void Func() 
    {
        cout << " hello world!" << endl;
    }
};
int main() 
{
    t tt;
    while (1){}
}