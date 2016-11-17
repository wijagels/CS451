#include "mapreduce.h"
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "common.h"

static int* fd_arr;
static pid_t* pid_arr;
static pthread_mutex_t reduce_lock = PTHREAD_MUTEX_INITIALIZER;

void worker(MAPREDUCE_SPEC* spec, DATA_SPLIT* ds, int fd,
            char special_snowflake);

void mapreduce(MAPREDUCE_SPEC* spec, MAPREDUCE_RESULT* result) {
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
    pid_arr = malloc(spec->split_num * sizeof(pid_t));
    if (!fd_arr) EXIT_ERROR(1, "%s", "Fatal malloc error");
    for (int i = 0; i < spec->split_num; i++) {
        snprintf(part_name, FILENAME_MAX, "mr-%d.itm", i);
        unlink(part_name);
        fd_arr[i] = open(part_name, O_CREAT | O_RDWR, S_IRWXU);
    }
    char special_snowflake = 1;

    for (int i = 0; i < spec->split_num - 1; i++) {
        size_t part_sz = boundary;
        fseek(fp, boundary, SEEK_CUR);
        ++part_sz;
        char c = fgetc(fp);
        /* Stop as soon as we hit a non-letter */
        while (isalpha(c)) {
            ++part_sz;
            c = fgetc(fp);
        }
        int64_t offset = ftell(fp) - part_sz - 1;
        pid_t pid = fork();
        if (!pid) {
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
        int fd = open(spec->input_data_filepath, O_RDONLY);
        lseek(fd, offset, SEEK_SET);
        DATA_SPLIT ds = {fd, sz - offset, spec->usr_data};
        worker(spec, &ds, fd_arr[spec->split_num - 1], special_snowflake);
        return;
    }
    pid_arr[spec->split_num - 1] = pid;

    for (int i = 1; i < spec->split_num; i++) waitpid(pid_arr[i], NULL, 0);
    kill(pid_arr[0], SIGUSR1);
    waitpid(pid_arr[0], NULL, 0);

    gettimeofday(&end, NULL);

    result->processing_time = (end.tv_sec - start.tv_sec) * US_PER_SEC +
                              (end.tv_usec - start.tv_usec);
    result->map_worker_pid = pid_arr;
    result->reduce_worker_pid = pid_arr[0];
    result->filepath = "mr.rst";
    free(fd_arr);
}

void sigusr1_handler(int sig) {
    if (sig == SIGUSR1) pthread_mutex_unlock(&reduce_lock);
}

void worker(MAPREDUCE_SPEC* spec, DATA_SPLIT* ds, int fd,
            char special_snowflake) {
    sigset_t sigmask;
    if (special_snowflake) {
        sigemptyset(&sigmask);
        sigaddset(&sigmask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &sigmask, NULL);

        pthread_mutex_lock(&reduce_lock);
        signal(SIGUSR1, &sigusr1_handler);
    }

    int rval = spec->map_func(ds, fd);

    if (special_snowflake) {
        sigprocmask(SIG_UNBLOCK, &sigmask, NULL);
        unlink("mr.rst");
        int rst_fd = open("mr.rst", O_CREAT | O_WRONLY, S_IRWXU);
        pthread_mutex_lock(&reduce_lock);
        spec->reduce_func(fd_arr, spec->split_num, rst_fd);
    }
    _exit(rval);
}
