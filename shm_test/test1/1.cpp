#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <memory>
#include <iostream>
using namespace std;

typedef struct Stu 
{
    int age;
    char name[10];
} Stu;

int main() 
{
    int id = shmget((key_t)1234, sizeof(Stu) * 10, IPC_CREAT | 0644);
    // Stu tmp[10];
    // char tmp[15] = "here is shm";

    void* p = shmat(id, nullptr, 0);
    Stu* pp = (Stu *)p;
    shared_ptr<Stu *> tmpPtr(&pp);
    for (int i = 0; i < 5; i++) 
    {
        (*tmpPtr + i)->age = i;
        strcpy((*tmpPtr).name, "lvbai");
    }

    shmdt(t);

    // shmctl(id, IPC_RMID, 0);
 
    return 0;
}