#include "mybarrier.h"
#include <stdio.h>
#include <stdlib.h>

mybarrier_t* mybarrier_init(unsigned int count) {
    mybarrier_t* barrier = malloc(sizeof(*barrier));

    if (NULL == barrier) {
        return NULL;
    }

    barrier->count = count;

    pthread_mutexattr_t attrs;
    pthread_mutexattr_init(&attrs);
    pthread_mutex_init(&barrier->mutex, &attrs);
    pthread_mutexattr_destroy(&attrs);

    pthread_condattr_t condattrs;
    pthread_condattr_init(&condattrs);
    pthread_cond_init(&barrier->cond, &condattrs);
    pthread_condattr_destroy(&condattrs);

    pthread_rwlockattr_t rwattrs;
    pthread_rwlockattr_init(&rwattrs);
    pthread_rwlockattr_setkind_np(&rwattrs, PTHREAD_RWLOCK_PREFER_READER_NP);
    pthread_rwlock_init(&barrier->dtor_lock, &rwattrs);
    pthread_rwlockattr_destroy(&rwattrs);

    return barrier;
}

int mybarrier_destroy(mybarrier_t* barrier) {
    int ret = 0;

    pthread_mutex_lock(&barrier->mutex);
    while (barrier->count > 0) {
        ret = pthread_cond_wait(&barrier->cond, &barrier->mutex);
    }
    pthread_mutex_unlock(&barrier->mutex);
    pthread_cond_broadcast(&barrier->cond);

    pthread_rwlock_wrlock(&barrier->dtor_lock);
    pthread_mutex_destroy(&barrier->mutex);
    pthread_cond_destroy(&barrier->cond);
    pthread_rwlock_destroy(&barrier->dtor_lock);
    free(barrier);

    return ret;
}

int mybarrier_wait(mybarrier_t* barrier) {
    int ret = 0;

    pthread_rwlock_rdlock(&barrier->dtor_lock);
    pthread_mutex_lock(&barrier->mutex);
    --barrier->count;
    if (barrier->count <= 0) {
        ret = pthread_cond_broadcast(&barrier->cond);
    } else {
        while (barrier->count > 0) {
            ret = pthread_cond_wait(&barrier->cond, &barrier->mutex);
        }
        pthread_cond_broadcast(&barrier->cond);
    }
    pthread_mutex_unlock(&barrier->mutex);
    pthread_rwlock_unlock(&barrier->dtor_lock);

    return ret;
}
