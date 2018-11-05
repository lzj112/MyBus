#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    int id = shmget((key_t)1234, sizeof(Stu) * 10, IPC_CREAT | 0666);
    if (id == -1) 
    {
        perror("shmget "), exit(1);
    }
    // Stu tmp[10];
    // char tmp[15] = "here is shm";
    Stu* t = (Stu *)shmat(id, NULL, 0);
    for (int i = 0; i < 5; i++) 
    {
        (t + i)->age = i;
        strcpy((t + i)->name, "lvbai");
    }

    // strcpy(t, tmp);

    shmdt(t);

    // shmctl(id, IPC_RMID, 0);
   
   key_t key = ftok("/home/lzj/MyBus", 2);
   printf("%d************\n", (key >> 16 )  /*+1*/);
    
    return 0;
}