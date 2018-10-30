#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

typedef struct 
{
    char name[4];
    int age;
} people;

int main() 
{
    int shm_id, i;
    key_t key;
    char temp;
    people* p_map;
    char* name = "/home/lzj/MyBus/shm_test";
    key = ftok(name, 0);
    assert(key != -1);

    shm_id = shmget(key, 4096, IPC_CREAT | 0644);
    assert(shm_id != -1);

    printf("shmid ===== %d\n", shm_id);

    p_map = (people *)shmat(shm_id, NULL, 0);
    temp = 'a';

    for (int i = 0; i < 10; i++) 
    {
        temp += 1;
        memcpy((*(p_map + i)).name, &temp, 1);
        (*(p_map + i)).age = 20 + i;    
    }
    // int* tmp = (int *)shmat(shm_id, NULL, 0);
    // *tmp = 100;

    // int shmId = shmget(4567, 4096, IPC_CREAT | 0644);
    // assert(shmId != -1);

    // int* intP = (int *)shmat(shmId, NULL, 0);
    // *intP = 10;

    // if (shmdt(intP) == -1) 
    // {

    // }
    if (shmdt(p_map) == -1) 
    {

    }

}