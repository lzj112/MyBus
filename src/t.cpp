#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <net/if.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <sys/shm.h>
#include <time.h>
#include <cstdlib>
#include <assert.h>

#include <iostream>
#include <vector> 
#include <thread>
#include <mutex>

#include "AllocPort.h"


using namespace std;

const char* ip = "127.0.0.1";
int port = 5000;

struct a 
{
  int a;
  char b;

};
int main() 
{

  int shmid = shmget(1234, sizeof(struct a), IPC_CREAT | 06666);

  struct a* t = (struct a *)shmat(shmid, nullptr, 0);

  cout << t->a << " " << t->b << endl;

  shmdt(t);
  shmctl(shmid, IPC_RMID, nullptr);
}
