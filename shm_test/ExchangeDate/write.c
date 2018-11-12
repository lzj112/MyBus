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
 
    int id = shmget((key_t)1234, sizeof(Stu) * 10, IPC_CREAT | 0644);
    if (id == -1) 
    {
        perror("shmget "), exit(1);
    }
    printf("%d\n", id);
    Stu* t = (Stu *)shmat(id, NULL, 0);
    for (int i = 0; i < 5; i++) 
    {
        (t + i)->age = i;
        strcpy((t + i)->name, "lvbai");
    }

    shmdt(t);
 
    return 0;
}