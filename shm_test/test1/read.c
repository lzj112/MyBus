#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include <assert.h>
#include <errno.h>
#include "shmdata.h"

int main()
{
    int running = 1;              //程序是否继续运行的标志
    void *shm = NULL;             //分配的共享内存的原始首地址
    struct shared_use_st *shared; //指向shm
    int shmid;                    //共享内存标识符

    //创建共享内存
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), IPC_CREAT | 0666);
    // assert(shmid != -1);
    printf("shmid == %d\n", shmid);
    if (shmid == -1) 
    {
        perror("shmid == -1 : ");
    }

    //将共享内存连接/挂载到当前进程的地址空间
    shm = shmat(shmid, NULL, 0);
    assert(shm != (void *)-1);

    printf("Memory attached at %X\n", (int)shm);

    //设置共享内存
    shared = (struct shared_use_st *)shm;
    shared->written = 0;

    while (running) //读取共享内存中的数据
    {
        //没有进程向共享内存内存数据, 无数据可读
        if (shared->written != 0)
        {
            printf("You wrote: %s\n", shared->text);
            sleep(rand() % 3);

            //读取完数据,设置written使共享内存内存段可写
            shared->written = 0;

            //输入了end,退出循环
            if (strncmp(shared->text, "end", 3) == 0)
            {
                running = 0;
            }
        }
        else
        {
            //有其他进程在写数据, 不能读取数据
            sleep(1);
        }
    }
    //把共享内存从当前进程分离
    int ret = shmdt(shm);
    assert(ret != -1);

    //删除共享内存
    ret = shmctl(shmid, IPC_RMID, 0);
    assert(ret != -1);

    exit(EXIT_SUCCESS);
}