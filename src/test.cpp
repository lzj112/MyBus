#include <iostream>
#include <string>

class APP 
{
public:
    APP(const std::string &ffff) 
    {
        std::cout << "APP(): " << ffff << std::endl;
        // std::cout << "here is APP() \n";
    }

    APP(int i)
    {
        std::cout << "APP(): " << i << std::endl;
    }

    void run() 
    {
        std::cout << "???" << std::endl;
    }
};

int main() 
{
    static_cast<APP>("Lv").run();
    APP("ZiJian").run();
    ((APP)222222).run();
}


/*
...
static_cast
类型转换的实质:
调用了这个类型的单参构造函数

*/