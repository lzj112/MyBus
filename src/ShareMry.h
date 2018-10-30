#ifndef _SHAREMRY_H_
#define _SHAREMRY_H_

/*
共享内存信息
*/

#include <sys/types.h>

struct ShareMry 
{
    key_t shm_Key;
    size_t shm_Size;
    int shm_Id;
};

#endif