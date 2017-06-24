#include "common.h"

int main() {
    int semid, shmid, xfrs, bytes;
    struct shmseg *shmp;
    semid = semget(SEM_KEY, 0, 0);
    shmid = shmget(SHM_KEY, 0, 0);
    shmp = shmat(shmid, NULL, SHM_RDONLY);
    for (xfrs = 0, bytes = 0;; xfrs++) {
        reserveSem(semid, READ_SEM);
        if (shmp->cnt == 0) break;
        bytes += shmp->cnt;
        write(STDOUT_FILENO, shmp->buf, shmp->cnt);
        releaseSem(semid, WRITE_SEM);
    }
    shmdt(shmp);
    releaseSem(semid, WRITE_SEM);
    printf("Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
