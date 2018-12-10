
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <vector>
#include <iostream>
#include "cJSON.h"

int main() 
{
    void* a = nullptr;
    int res = cJSON_GetArraySize(static_cast<cJSON *> (a));
    perror("asasa");
    std::cout << a << std::endl;
}