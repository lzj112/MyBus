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


    void* p = NULL;
    p = shmat(id, NULL, 0);
    if (p == (void *)-1) 
    {
        perror("****** : ");
    }


    Stu* ptr = (Stu *)p;

	printf("here is message : \n");

    for (int i = 0; i < 10; i++) 
    {
        printf("age = %d, name = %s\n", (ptr + i)->age, (ptr + i)->name);
    }

    shmdt(p);

    shmctl(id, IPC_RMID, 0);
   
   
    
    return 0;
}