#include <cstring>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <iostream>

#include "Parsing.h"
#include "cJSON.h"

Parsing::~Parsing()
{
    for (auto x : addr) 
    {
        delete x.first;
    }
}

void Parsing::getConfigFile(const char* fileName) 
{
    struct stat s;
    if ((stat(fileName, &s)) < 0) 
    {
        perror("stat");
        return ;
    }

    int fileFd = open(fileName, O_RDONLY, 0);
    assert(fileFd != -1);

    char* mapping = (char *)mmap(nullptr, s.st_size, PROT_READ, MAP_PRIVATE, fileFd, 0);
    assert(mapping != (void *)-1);

    cJSON* root = cJSON_Parse(mapping);
    assert(root != nullptr);

    addSource(root);
    addSourcePass(root);
    addDestPass(root);
    addDest(root);
    
    for (auto x : addr) 
    {
        printf("ip%s port%d\n", x.first, x.second);
    }

    int res = munmap(mapping, s.st_size);
    assert(res != -1);
}

void Parsing::addSource(cJSON* t) 
{
    cJSON* sip = cJSON_GetObjectItem(t, "sourceIP");
printf("addsourceip %s\n", sip->valuestring);
    std::pair<char* ,int> item;
    item.first = new char[10];
    strcpy(item.first, sip->valuestring);
    
    cJSON* sport = cJSON_GetObjectItem(t, "sourcePort");
printf("----%d\n", sport->valueint);
    item.second = sport->valueint;
    this->addr.push_back(item);
}

void Parsing::addSourcePass(cJSON* t) 
{
    cJSON* spIP = cJSON_GetObjectItem(t, "passIP");
    std::pair<char*, int> item;
    item.first = new char[10];
    strcpy(item.first, spIP->valuestring);

    cJSON* spPort = cJSON_GetObjectItem(t, "passPort");
    item.second = spPort->valueint;
    this->addr.push_back(item);
}

void Parsing::addDest(cJSON* t) 
{
    cJSON* sip = cJSON_GetObjectItem(t, "destIP");
    std::pair<char* ,int> item;
    item.first = new char[10];
    strcpy(item.first, sip->valuestring);
    
    cJSON* sport = cJSON_GetObjectItem(t, "destPort");
    item.second = sport->valueint;
    this->addr.push_back(item);
}

void Parsing::addDestPass(cJSON* t) 
{
    cJSON* sip = cJSON_GetObjectItem(t, "destPassIP");
    std::pair<char* ,int> item;
    item.first = new char[10];
    strcpy(item.first, sip->valuestring);
    
    cJSON* sport = cJSON_GetObjectItem(t, "destPassPort");
    item.second = sport->valueint;
    this->addr.push_back(item);
}

char* Parsing::getSourceIP() 
{
    return addr[0].first;
} 

char* Parsing::getSourcePassIP() 
{
    return addr[1].first;
}

char* Parsing::getdestPassIP() 
{
    return addr[2].first;
}

char* Parsing::getdestIP() 
{
    return addr[3].first;
}

int Parsing::getSourcePort() 
{
    return addr[0].second;
}

int Parsing::getSourcePassPort() 
{
    return addr[1].second;
}

int Parsing::getDestPassPort() 
{
    return addr[2].second;
}

int Parsing::getDestPort() 
{
    return addr[3].second;
}