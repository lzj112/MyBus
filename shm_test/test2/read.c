#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
typedef struct
{
    char name[4];
    int age;
} people;

main(int argc, char **argv)
{
    int shm_id, i;
    key_t key;
    people *p_map;
    char *name = "/home/lzj/MyBus/shm_test";
    key = ftok(name, 0);
    if (key == -1)
    {
        perror("ftok error");
    }
    
    shm_id = shmget(key, 4096, IPC_CREAT);
    if (shm_id == -1)
    {
        perror("shmget error");
        return;
    }
    p_map = (people *)shmat(shm_id, NULL, 0);
    for (i = 0; i < 10; i++)
    {
        printf("name:%s\n", (*(p_map + i)).name);
        printf("age %d\n", (*(p_map + i)).age);
    }

    // int a;
    // printf("%d\n", a);
    int id = shmget(4567, 4096, IPC_CREAT);
    int* p = (int *)shmat(id, NULL, 0);
    printf("here is int : %d\n", *p);

    if (shmdt(id) == -1) 
    {

    }
    if (shmdt(p_map) == -1)
        perror(" detach error ");
}