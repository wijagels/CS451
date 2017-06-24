#include "common.h"

int main(int argc, char *argv[]) {
    int semid, shmid, bytes, xfrs;

    struct shmseg *shmp;

    union semun dummy;

    semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);

    initSemAvailable(semid, WRITE_SEM);

    initSemInUse(semid, READ_SEM);

    shmid =
        shmget(SHM_KEY, sizeof(struct shmseg), IPC_CREAT | S_IRUSR | S_IWUSR)

            shmp = shmat(shmid, NULL, 0);

    for (xfrs = 0, bytes = 0;; xfrs++, bytes += shmp->cnt) {
        reserveSem(semid, WRITE_SEM);

        shmp->cnt = read(STDIN_FILENO, shmp->buf, BUF_SIZE);

        releaseSem(semid, READ_SEM);

        if (shmp->cnt == 0) break;
    }

    reserveSem(semid, WRITE_SEM);

    semctl(semid, 0, IPC_RMID, dummy);
    shmdt(shmp);
    shmctl(shmid, IPC_RMID, 0);
    printf("Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
