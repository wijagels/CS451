#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define report(message) \
    printf("[PID:%d|T+%luus]: " message "\n", getpid(), timediff())

struct timespec begin;
struct timespec cur;

long timediff() {
    clock_gettime(CLOCK_REALTIME, &cur);
    return ((cur.tv_nsec - begin.tv_nsec) / 1e3) +
           (cur.tv_sec - begin.tv_sec) * 1e6;
}

int main() {
    clock_gettime(CLOCK_REALTIME, &begin);
    pid_t pid;
    report("Spawned grandparent");
    pid = fork();
    if (pid) {
        sleep(6);
        waitpid(pid, NULL, 0);
        report("wait() returned");
        exit(0);
    } else {
        report("Spawned parent");
        pid = fork();
        if (pid) {
            sleep(3);
            report("Terminating");
            _exit(0);
        } else {
            report("Spawned child");
            // This fails horribly on modern OSes with systemd
            while (getppid() != 1)
                ;
            report("Adopted by init");
            _exit(0);
        }
    }
}
