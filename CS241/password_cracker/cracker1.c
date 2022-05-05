/**
 * password_cracker
 * CS 241 - Spring 2022
 */
#include "cracker1.h"
#include "format.h"
#include "utils.h"
#include "includes/queue.h"
#include "thread_status.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <crypt.h>
#include <stdbool.h> 

const size_t USERNAME_MAX_LENGTH = 9;
const size_t HASH_MAX_LENGTH = 14;
const size_t PASSWORD_MAX_LENGTH = 9;//18?
const size_t NUM_LETTERS_ALPHABET = 26;
const char* SALT = "xx";

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
queue* tasks_q;
size_t successful_cracks;
size_t unsuccessful_cracks;

typedef struct task_t {
    char* username;
    char* hash;
    char* password;
} task_t;

void free_task(task_t* task) {
    free(task->username);
    free(task->hash);
    free(task->password);
    free(task); task = NULL;
}

void* crack_thread(void* input) {
    threadStatusSet("started");
    // int threadId = pthread_self();
    int threadId = (long)input;
    
    task_t* task = NULL;
    while((task = queue_pull(tasks_q))) {
        threadStatusSet("got task");
        v1_print_thread_start(threadId, task->username);//Print

        double start_time = getThreadCPUTime();

        int prefix_length = getPrefixLength(task->password);
        int suffix_length = strlen(task->password) - prefix_length;

        setStringPosition(task->password + prefix_length, 0);//Set '.' to 'a'

        struct crypt_data cdata;
        cdata.initialized = 0;

        bool found = suffix_length == 0;//Did not have any unknowns
        int hashCount = 0;
        int num_combinations = (int)pow((double)NUM_LETTERS_ALPHABET, (double)suffix_length);
        for (int i = 0; !found && i < num_combinations; ++i) {
            ++hashCount;
            if (strcmp(crypt_r(task->password, SALT, &cdata), task->hash) == 0) {
                threadStatusSet("lock");
                pthread_mutex_lock(&lock);
                ++successful_cracks;
                pthread_mutex_unlock(&lock);
                threadStatusSet("unlock");
                found = true;

                break;
            }
            incrementString(task->password);
        }

        if (!found) { ++unsuccessful_cracks; }

        v1_print_thread_result(threadId, task->username, task->password, hashCount, getThreadCPUTime() - start_time, !found);//Print

        free_task(task);
    }
    queue_push(tasks_q, NULL);//Prevents blocking

    return NULL;
}

int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads

    //Create tasks and add them to queue
    tasks_q = queue_create(0);
    char* line = NULL;
    size_t size = 0;

    while (getline(&line, &size, stdin) != -1) {
        task_t* task = malloc(sizeof(task_t));

        char* username = malloc(USERNAME_MAX_LENGTH);
        char* hash = malloc(HASH_MAX_LENGTH);
        char* password = malloc(PASSWORD_MAX_LENGTH);

        sscanf(line, "%s %s %s", username, hash, password);

        task->username = username;
        task->hash = hash;
        task->password = password;

        queue_push(tasks_q, task);
    }
    free(line);
    queue_push(tasks_q, NULL);

    //Startup threads
    pthread_t threads[thread_count];
    for (size_t i = 0; i < thread_count; ++i) {
        pthread_create(threads + i, NULL, crack_thread, (void*)(i + 1));
    }

    //Join threads
    threadStatusSet("Starting join");
    for(size_t i = 0; i < thread_count; ++i){
		pthread_join(threads[i], NULL);
	}
    threadStatusSet("Finished join");

    v1_print_summary(successful_cracks, unsuccessful_cracks);

    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
