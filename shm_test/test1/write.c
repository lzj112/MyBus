#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <errno.h>
#include <assert.h>
#include "shmdata.h"

int main() 
{
    int running = 1;
    void* shm = NULL;
    struct shared_use_st* shared = NULL;
    char buffer[BUFSIZ + 1];    //用以保存输入的文本    常量BUFSIZ,值为8192
    int shmid;

    //创建共享内存
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), IPC_CREAT | 0666);
    // assert(shmid != -1);
    printf("shmid == %d\n", shmid);
    if (shmid == -1) 
    {
        perror("shmid == -1 : ");
    }

    //将共享内存连接到当前进程的地址空间
    shm = shmat(shmid, NULL, 0);
    assert(shm != -1);
    printf("Memory attached at %X\n", (int)shm);

    //设置共享内存
    shared = (struct shared_use_st *)shm;
    while (running) //向共享内存中写数据
    {
        //数据还没有被读取,则等待数据被读取,不能向共享内存中写入文本
        while (shared->written == 1) 
        {
            sleep(1);
            printf("Waiting...\n");
        }

        //向共享内存中写入数据
        printf("Enter some text : ");
        fgets(buffer, BUFSIZ, stdin);
        strncpy(shared->text, buffer, TEXT_SZ);

        //写完数据,设置written使共享内存段可读
        shared->written = 1;

        //输入了end,退出循环
        if (strncmp(buffer, "end", 3) == 0) 
        {
            running = 0;
        }
    }

    //把共享内存从当前进程中分离
    int ret = shmdt(shm);
    assert(ret != -1);

    sleep(2);
    exit(EXIT_SUCCESS);
}