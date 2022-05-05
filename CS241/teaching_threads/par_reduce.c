/**
 * teaching_threads
 * CS 241 - Spring 2022
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "reduce.h"
#include "reducers.h"

/* You might need a struct for each task ... */
typedef struct reduce_t {
    int* list;
    size_t listLen;
    reducer reduceFunction;
    int baseCase;
} reduce_t;

/* You should create a start routine for your threads. */

void* reduce_routine(void* input) {
    reduce_t* reduceIn = (reduce_t*)input;
    int* output = malloc(sizeof(int));//FREE
    *output = reduce(reduceIn->list, reduceIn->listLen, reduceIn->reduceFunction, reduceIn->baseCase);
    return output;
}

int par_reduce(int *list, size_t list_len, reducer reduce_func, int base_case,
               size_t num_threads) {
    /* Your implementation goes here */
    /**
     * list[10] = [1,2,3,4,5,6,7,8,9,10]
     * list1[3] = [1,2,3]
     * list2[3] = [4,5,6]
     * list3[4] = [7,8,9,10]
     */
    // 10, 3
    //Works for even list numbers TODO: odd
    if (num_threads > list_len / 2) {
        num_threads = list_len / 2;
    }

    //Create threads
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    reduce_t* splitLists = malloc(num_threads * sizeof(reduce_t));

    size_t elementsThread = (list_len / num_threads);//Should round down?
    size_t elementsLeft = list_len % num_threads;
    // printf("%zu\n", elementsThread);

	for(size_t i = 0; i < num_threads; ++i){
        //TODO: Add an extra element if odd and last split list
        size_t odd = 0;
        if (i == num_threads - 1) {
            odd = elementsLeft;
        }
        //Creates split list
        // int* tmpList = malloc(elementsThread * sizeof(int));
        //Copies values from original list
        // memcpy(tmpList, list + (elementsThread * i), sizeof(int) * (elementsThread + odd));

        splitLists[i] = (reduce_t){list + (elementsThread * i), (elementsThread + odd), reduce_func, base_case};

		pthread_create(threads + i, NULL, reduce_routine, splitLists + i);
	}

    int* new_list = malloc(num_threads * sizeof(int));
    for(size_t i = 0; i < num_threads; ++i){
        void* threadReturn = NULL;
		pthread_join(threads[i], &threadReturn);

        new_list[i] = (*(int*)threadReturn);

        free(threadReturn);
	}


    int output = reduce(new_list, num_threads, reduce_func, base_case);

    free(new_list);
    free(threads);
    free(splitLists);

    return output;
}