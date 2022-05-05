/**
 * password_cracker
 * CS 241 - Spring 2022
 */
#include "cracker2.h"
#include "format.h"
#include "utils.h"
#include "includes/queue.h"
#include "thread_status.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <crypt.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct task_t {
    char* username;
    char* hash;
    char* password;
    //Thread change
    bool found;
    int hashCount;
    char* cracked_password;
} task_t;

//Thread status
enum exit_status{FOUND, CANCELLED, END};

const size_t USERNAME_MAX_LENGTH = 9;
const size_t HASH_MAX_LENGTH = 14;
const size_t PASSWORD_MAX_LENGTH = 9;//18?
const size_t NUM_LETTERS_ALPHABET = 26;
const char* SALT = "xx";

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier;

size_t thread_c = 0;
bool finished = false;//Flag for when all passwords have been cracked
task_t* task = NULL;

void free_task(task_t* task) {
    free(task->username);
    free(task->hash);
    free(task->password);
    free(task->cracked_password);
    free(task); task = NULL;
}

task_t* create_task(char* username, char* hash, char* password) {
    task_t* task = malloc(sizeof(task_t));
    task->username = username;
    task->hash = hash;
    task->password = password;

    task->found = false;
    task->hashCount = 0;
    task->cracked_password = NULL;

    return task;
}

void* crack_thread(void* input) {
    int thread_id = (long)input;
    // Outer loop runs once every task
    while(true) {
        //Wait for task to be ready. Waiting on main.
        threadStatusSet("Thread waiting for task");
        pthread_barrier_wait(&barrier);
        if (finished) { break; }//Should trigger after last password is read

        int prefix_length = getPrefixLength(task->password);//TODO can be changed for speed up
        int suffix_length = strlen(task->password) - prefix_length;
        long start_index, count;
        getSubrange(suffix_length, thread_c, thread_id, &start_index, &count);

        //Setting starting position
        char* startPassword = strdup(task->password);//FREE
        setStringPosition(startPassword + prefix_length, start_index);//Set '.' to 'a'

        v2_print_thread_start(thread_id, task->username, start_index, startPassword);
        //Start solving
        enum exit_status status = END;
        int hashCount = 0;
        struct crypt_data cdata;
        cdata.initialized = 0;

        // printf("%ld\n", count);
        for(int i = 0; i < count; ++i) {
            // printf("%d", hashCount);
            // puts(startPassword);
            threadStatusSet("Thread solving");
            ++hashCount;
            if (strcmp(crypt_r(startPassword, SALT, &cdata), task->hash) == 0) {
                status = FOUND;

                //TODO: alert other threads
                pthread_mutex_lock(&lock);
                task->found = true;
                task->cracked_password = strdup(startPassword);//FREE
                pthread_mutex_unlock(&lock);

                break;
            }
            //Another thread has cracked the password
            if (task->found) {
                status = CANCELLED;
                break;
            }

            incrementString(startPassword);
        }
        threadStatusSet("Thread finished solving");
        pthread_mutex_lock(&lock);
        task->hashCount += hashCount;//TODO check edge case where count = 0
        pthread_mutex_unlock(&lock);

        free(startPassword);

        v2_print_thread_result(thread_id, hashCount, status);

        //Wait for all threads to be finished
        threadStatusSet("Thread waiting for all threads to finish");
        pthread_barrier_wait(&barrier);
    }
    // puts("THREAD DONE");
    threadStatusSet("Thread finished");
    return NULL;
}

int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads
    //Setup
    pthread_barrier_init(&barrier, NULL, thread_count + 1);//+1
    thread_c = thread_count;

    //Startup threads
    pthread_t threads[thread_count];
    for (size_t i = 0; i < thread_count; ++i) {
        pthread_create(threads + i, NULL, crack_thread, (void*)(i + 1));
    }

    //Read in tasks
    char* line = NULL;
    size_t size = 0;
    while (getline(&line, &size, stdin) != -1) {
        //Create task
        threadStatusSet("Main is creating task");
        char* username = malloc(USERNAME_MAX_LENGTH);
        char* hash = malloc(HASH_MAX_LENGTH);
        char* password = malloc(PASSWORD_MAX_LENGTH);
        sscanf(line, "%s %s %s", username, hash, password);
        task = create_task(username, hash, password);//TODO: Free

        v2_print_start_user(username);

        //Tracking time
        double start_time = getTime();
        double cpu_start_time = getCPUTime();

        //Task is ready. Threads may proceed
        threadStatusSet("Main new task is ready");
        pthread_barrier_wait(&barrier);
        //Waiting for threads to finish cracking
        threadStatusSet("Main waiting for threads to finish with task");
        pthread_barrier_wait(&barrier);
        //Threads are waiting on main
        threadStatusSet("Main is printing results");
        v2_print_summary(task->username, task->cracked_password, task->hashCount, getTime() - start_time, getCPUTime() - cpu_start_time, !(task->found));
        free_task(task); task = NULL;
    }
    free(line);
    finished = true;

    threadStatusSet("Main is releasing all threads");
    pthread_barrier_wait(&barrier);//Unlocks threads when last password has been attempted

    //Join threads
    for(size_t i = 0; i < thread_count; ++i){
		pthread_join(threads[i], NULL);
	}
    // puts("MAIN DONE");

    pthread_barrier_destroy(&barrier);
    pthread_mutex_destroy(&lock);

    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
