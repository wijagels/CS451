#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static void notifySetup(mqd_t *mqdp);
static void threadFunc(union sigval sv) {
    ssize_t numRead;
    mqd_t *mqdp;
    void *buffer;
    struct mq_attr attr;
    mqdp = sv.sival_ptr;
    mq_getattr(*mqdp, &attr);
    buffer = malloc(attr.mq_msgsize);
    notifySetup(mqdp);
    while ((numRead = mq_receive(*mqdp, buffer, attr.mq_msgsize, NULL)) >= 0)
        printf("Read %ld bytes\n", (long)numRead);
    free(buffer);
}
static void notifySetup(mqd_t *mqdp) {
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = threadFunc;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = mqdp;
    mq_notify(*mqdp, &sev);
}
int main(int argc, char *argv[]) {
    mqd_t mqd;
    mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    notifySetup(&mqd);
    pause();
    /* Wait for notifications via thread function */
}
