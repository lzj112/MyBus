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
    int id = shmget(1234, 8, 0);
    if (id == -1) 
    {
        perror("shmget "), exit(1);
    }

    Stu* p = (Stu *)shmat(id, NULL, 0);
   
    while (1) 
    {
        printf("age = %d, name = %s\n", p->age, p->name);
        sleep(2);
    }
    return 0;
}