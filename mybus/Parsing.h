#ifndef _PARSING_H
#define _PARSING_H

#include <vector>

#include "cJSON.h"

class Parsing 
{
public:
    ~Parsing();
    void getConfigFile(const char* fileName);
    void addSource(cJSON* t);
    void addSourcePass(cJSON* t);
    void addDest(cJSON* t);
    void addDestPass(cJSON* t);
    char* getSourceIP();
    char* getSourcePassIP();
    char* getdestPassIP();
    char* getdestIP();
    int getSourcePort();
    int getSourcePassPort();
    int getDestPassPort();
    int getDestPort();

private:
    std::vector<std::pair<char*, int> > addr;
};

#endif