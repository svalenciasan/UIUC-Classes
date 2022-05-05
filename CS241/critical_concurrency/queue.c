/**
 * critical_concurrency
 * CS 241 - Spring 2022
 */
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * This queue is implemented with a linked list of queue_nodes.
 */
typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node;

struct queue {
    /* queue_node pointers to the head and tail of the queue */
    queue_node *head, *tail;

    /* The number of elements in the queue */
    ssize_t size;

    /**
     * The maximum number of elements the queue can hold.
     * max_size is non-positive if the queue does not have a max size.
     */
    ssize_t max_size;

    /* Mutex and Condition Variable for thread-safety */
    pthread_cond_t cv;
    pthread_mutex_t m;
};

queue *queue_create(ssize_t max_size) {
    /* Your code here */
    queue* new_queue = malloc(sizeof(queue));//TODO: Can Fail

    pthread_cond_init(&new_queue->cv, NULL);
    pthread_mutex_init(&new_queue->m, NULL);

    new_queue->head = NULL;
    new_queue->tail = NULL;
    new_queue->size = 0;
    new_queue->max_size = max_size;

    return new_queue;
}

void queue_destroy(queue *this) {
    /* Your code here */
    if (!this) {
        return;
    }

    queue_node* node = this->head;
    while(node) {
        queue_node* tmp = node;
        node = node->next;

        free(tmp);
    }

    pthread_cond_destroy(&this->cv);
    pthread_mutex_destroy(&this->m);
    free(this);
}

void queue_push(queue *this, void *data) {
    /* Your code here */
    pthread_mutex_lock(&this->m);
    //Max_size non negative and at max capacity
    while(this->max_size > 0 && this->size == this->max_size) {// >=?
        pthread_cond_wait(&this->cv, &this->m);
    }
    //Done or no need to wait
    queue_node* new_node = malloc(sizeof(queue_node));
    new_node->data = data;
    new_node->next = NULL;

    //Empty means we should set head and tail to first element
    if (this->size == 0) {
        this->head = new_node;
        this->tail = new_node;
    } else {
        this->tail->next = new_node;
        this->tail = new_node;
    }

    this->size++;

    pthread_cond_broadcast(&this->cv);
    pthread_mutex_unlock(&this->m);
}

void *queue_pull(queue *this) {
    /* Your code here */
    pthread_mutex_lock(&this->m);
    //
    while(this->size == 0) {// >=?
        pthread_cond_wait(&this->cv, &this->m);
    }

    queue_node* remove = this->head;
    if (this->size == 1) {
        this->tail = NULL;
        this->head = NULL;
    } else {
        this->head = remove->next;
    }

    this->size--;

    void* data = remove->data;
    
    free(remove);

    pthread_cond_broadcast(&this->cv);
    pthread_mutex_unlock(&this->m);
    return data;
}
