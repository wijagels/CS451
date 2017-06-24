#include <errno.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#define SHM_KEY 0x1234
#define SEM_KEY 0x5678
#define WRITE_SEM 0
#define READ_SEM 1
#define BUF_SIZE 1024
union semun { /* Used in calls to semctl() */
  int val;
  struct semid_ds* buf;
  unsigned short* array;
#if defined(__linux__)
  struct seminfo* __buf;
#endif
};
struct shmseg {
  int cnt;
  char buf[BUF_SIZE];
};
/* Initialize semaphore to 1 (i.e., "available") */
int initSemAvailable(int semId, int semNum) {
  union semun arg;
  arg.val = 1;
  return semctl(semId, semNum, SETVAL, arg);
}
/* Initialize semaphore to 0 (i.e., "in use") */
int initSemInUse(int semId, int semNum) {
  union semun arg;
  arg.val = 0;
  return semctl(semId, semNum, SETVAL, arg);
}
/* Reserve semaphore - decrement it by 1 */
int reserveSem(int semId, int semNum) {
  struct sembuf sops;
  sops.sem_num = semNum;
  sops.sem_op = -1;
  sops.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;
  while (semop(semId, &sops, 1) == -1)
    if (errno != EINTR || !bsRetryOnEintr) return -1;
  return 0;
}
/* Release semaphore - increment it by 1 */
int releaseSem(int semId, int semNum) {
  struct sembuf sops;
  sops.sem_num = semNum;
  sops.sem_op = 1;
  sops.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;
  return semop(semId, &sops, 1);
}
