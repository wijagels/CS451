#include "mapreduce.h"
#include <ctype.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "common.h"

static int* fd_arr;
static sem_t reduce_sem;

void sigusr1_handler(int sig);
void worker(MAPREDUCE_SPEC*, DATA_SPLIT*, int, char);

void mapreduce(MAPREDUCE_SPEC* spec, MAPREDUCE_RESULT* result) {
    sem_init(&reduce_sem, 0, -1);
    signal(SIGUSR1, &sigusr1_handler);

    struct timeval start, end;

    if (NULL == spec || NULL == result) {
        EXIT_ERROR(ERROR, "%s", "NULL pointer!\n");
    }

    gettimeofday(&start, NULL);

    /* Get size */
    FILE* fp = fopen(spec->input_data_filepath, "r");
    fseek(fp, 0, SEEK_END);
    int64_t sz = ftell(fp);
    rewind(fp);
    DEBUG_MSG("File size: %ld\n", sz);

    int64_t boundary = sz / spec->split_num;
    DEBUG_MSG("Boundary size: %ld\n", boundary);
    char part_name[FILENAME_MAX];
    fd_arr = malloc(spec->split_num * sizeof(int));
    int* pid_arr = result->map_worker_pid;
    if (!fd_arr) EXIT_ERROR(1, "%s", "Fatal malloc error");
    for (int i = 0; i < spec->split_num; i++) {
        snprintf(part_name, FILENAME_MAX, "mr-%d.itm", i);
        fd_arr[i] =
            open(part_name, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    }
    char special_snowflake = 1;

    for (int i = 0; i < spec->split_num - 1; i++) {
        int64_t part_sz = boundary - (ftell(fp) - boundary * i);
        DEBUG_MSG("Part_sz = %ld\n", part_sz);
        fseek(fp, part_sz, SEEK_CUR);
        ++part_sz;
        char c = fgetc(fp);
        /* Stop as soon as we hit a line break */
        while (c != '\n') {
            ++part_sz;
            c = fgetc(fp);
        }
        int64_t offset = ftell(fp) - part_sz - 1;
        pid_t pid = fork();
        if (!pid) {
            free(pid_arr);
            int fd = open(spec->input_data_filepath, O_RDONLY);
            lseek(fd, offset, SEEK_SET);
            DATA_SPLIT ds = {fd, part_sz, spec->usr_data};
            worker(spec, &ds, fd_arr[i], special_snowflake);
            return;
        }
        pid_arr[i] = pid;
        special_snowflake = 0;
    }
    int64_t offset = ftell(fp) - 1;
    pid_t pid = fork();
    if (!pid) {
        free(pid_arr);
        int fd = open(spec->input_data_filepath, O_RDONLY);
        lseek(fd, offset, SEEK_SET);
        DATA_SPLIT ds = {fd, sz - offset, spec->usr_data};
        worker(spec, &ds, fd_arr[spec->split_num - 1], special_snowflake);
        return;
    }
    pid_arr[spec->split_num - 1] = pid;

    int exit_status;
    for (int i = 1; i < spec->split_num; i++) {
        waitpid(pid_arr[i], &exit_status, 0);
        if (exit_status != 0)
            EXIT_ERROR(1, "Worker %d terminated abnormally\n", pid_arr[i]);
    }
    sem_wait(&reduce_sem);
    DEBUG_MSG("%s\n", "Children complete, sending SIGUSR1");
    kill(pid_arr[0], SIGUSR1);
    waitpid(pid_arr[0], &exit_status, 0);
    if (exit_status != 0)
        EXIT_ERROR(1, "Reduce worker %d terminated abnormally\n", pid_arr[0]);

    gettimeofday(&end, NULL);

    result->processing_time = (end.tv_sec - start.tv_sec) * US_PER_SEC +
                              (end.tv_usec - start.tv_usec);
    result->map_worker_pid = pid_arr;
    result->reduce_worker_pid = pid_arr[0];
    result->filepath = "mr.rst";
    fclose(fp);
    free(fd_arr);
}

void sigusr1_handler(int sig) {
    DEBUG_MSG("Catching signal %d\n", sig);
    if (sig == SIGUSR1) sem_post(&reduce_sem);
}

void worker(MAPREDUCE_SPEC* spec, DATA_SPLIT* ds, int fd,
            char special_snowflake) {
    sigset_t sigmask;
    if (special_snowflake) {
        DEBUG_MSG("In here %d\n", getpid());
        sigemptyset(&sigmask);
        sigaddset(&sigmask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &sigmask, NULL);
        signal(SIGUSR1, &sigusr1_handler);
    }

    int rval = spec->map_func(ds, fd);
    if (rval) {
        _EXIT_ERROR(rval, "map_func failed on %d failed\n", getpid());
    }

    if (special_snowflake) {
        sigprocmask(SIG_UNBLOCK, &sigmask, NULL);
        unlink("mr.rst");
        int rst_fd =
            open("mr.rst", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
        DEBUG_MSG("Worker %d waiting on siblings\n", getpid());
        kill(getppid(), SIGUSR1);
        sem_wait(&reduce_sem);
        DEBUG_MSG("Worker %d moving to reduce phase\n", getpid());
        rval = spec->reduce_func(fd_arr, spec->split_num, rst_fd);
    }
    free(fd_arr);
    DEBUG_MSG("Worker %d completed\n", getpid());
    _exit(rval);
}
