/*
这个程序呢没有删除共享内存段,导致我写其他例子的时候,不能创建id为1234的共享内存段
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
       
typedef struct Stu 
{
    int age;
    char name[10];
} Stu;

int main() 
{
    Stu s;
    strcpy(s.name, "lvbai");

    //创建共享内存段
    int id = shmget(1234, 8, IPC_CREAT | 0644);
    /*
    IPC_CREAT定义的一个特殊比特位，同时必须和权限标志按位或才能创建一个新的共享内存段。
    */
    if (id == -1) 
    {
        perror("shmget"), exit(1);
    }
    //挂载到进程的地址空间
    Stu* p = (Stu *)shmat(id, NULL, 0);

    int i = 0;
    // while (1) 
    // {
        s.age = ++i;
        memcpy(p, &s, sizeof(Stu)); //写到共享共享段中
        // sleep(2);
    // }
    return 0;
}