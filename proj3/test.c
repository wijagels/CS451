#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mybarrier.h"

void *worker(void *args) {
    printf("Thread start\n");
    mybarrier_t *barrier = (mybarrier_t *)args;
    sleep(rand() % 3);
    printf("Thread wait\n");
    mybarrier_wait(barrier);
    printf("Thread exit\n");
    pthread_exit(NULL);
    return NULL;
}

int main() {
    setbuf(stdout, NULL);
    pthread_t threads[100];
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    mybarrier_t *barrier = mybarrier_init(100);
    srand(time(NULL));
    for (int i = 0; i < 100; i++) {
        pthread_create(&threads[i], &attr, worker, barrier);
    }
    mybarrier_destroy(barrier);
    for (int i = 0; i < 100; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_attr_destroy(&attr);
}
