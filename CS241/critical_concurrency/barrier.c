/**
 * critical_concurrency
 * CS 241 - Spring 2022
 */
#include "barrier.h"

// The returns are just for errors if you want to check for them.
int barrier_destroy(barrier_t *barrier) {
    int error = 0;

    barrier->count = 0;
    pthread_cond_destroy(&barrier->cv);
    pthread_mutex_destroy(&barrier->mtx);
    barrier->n_threads = 0;
    barrier->times_used = 0;

    return error;
}

int barrier_init(barrier_t *barrier, unsigned int num_threads) {
    int error = 0;

    barrier->count = 0;//TODO: or numthreads
    pthread_cond_init(&barrier->cv, NULL);
    pthread_mutex_init(&barrier->mtx, NULL);
    barrier->n_threads = num_threads;
    barrier->times_used = 0;

    return error;
}

int barrier_wait(barrier_t *barrier) {
    unsigned int used = barrier->times_used;
    pthread_mutex_lock(&barrier->mtx);
    //Another iteration
    if (barrier->count == barrier->n_threads) {
        barrier->count = 0;
    }
    barrier->count++;
    // pthread_mutex_unlock(&barrier->mtx);
    if (barrier->count == barrier->n_threads) {
        pthread_cond_broadcast(&barrier->cv);//Barrier is full
        barrier->times_used++;
    } else {
        while (barrier->count != barrier->n_threads && used == barrier->times_used) {
            pthread_cond_wait(&barrier->cv, &barrier->mtx);
        }
    }
    pthread_mutex_unlock(&barrier->mtx);
    return 0;
}
