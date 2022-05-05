/**
 * mini_memcheck
 * CS 241 - Spring 2022
 */
#include "mini_memcheck.h"
#include <stdio.h>
#include <string.h>

meta_data* head = (meta_data*)NULL;
size_t total_memory_requested = 0;
size_t total_memory_freed = 0;
size_t invalid_addresses = 0;

void *mini_malloc(size_t request_size, const char *filename,
                  void *instruction) {
    // your code here
    if (request_size <= 0 ) {
        return NULL;
    }

    meta_data* meta = malloc(sizeof(meta_data) + request_size);

    if (!meta) {
        return NULL;
    }

    meta->request_size = request_size;
    meta->filename = filename;
    meta->instruction = instruction;

    // Check if prior allocated memory or not
    if (!head) {
        head = meta;
    } else {
        meta->next = head;
        head = meta;
    }

    total_memory_requested += request_size;

    return (void*)(meta + 1);
}

void *mini_calloc(size_t num_elements, size_t element_size,
                  const char *filename, void *instruction) {
    // your code here
    void* ptr = mini_malloc(num_elements * element_size, filename, instruction);

    if (!ptr) {
        return NULL;
    }

    memset(ptr, 0, num_elements * element_size);

    return ptr;
}

void *mini_realloc(void *payload, size_t request_size, const char *filename,
                   void *instruction) {
    // your code here
    // Passing NULL or passing a size equal to zero
    if (!payload) {
        return mini_malloc(request_size, filename, instruction);
    } else if (request_size <= 0) {
        mini_free(payload);
    } else if (!head) {
        invalid_addresses += 1;
        return NULL;
    }
    // //Special case, head is payload
    // if (head + 1 == payload) {
    //     meta_data* ptr = realloc(head, sizeof(meta_data) + request_size);
    //     //Failed realloc
    //     if (!ptr) {
    //         return NULL;
    //     }

    //     //Handles whether size is increasing or decresing
    //     if (request_size > head->request_size) {
    //         total_memory_requested += request_size - head->request_size;
    //     } else if (request_size < head->request_size) {
    //         total_memory_freed += head->request_size - request_size;
    //     }

    //     return ptr + 1;
    // }

    // meta_data* previous = NULL;
    // for (previous = head; previous; previous = previous->next) {
    //     //Last object in the list
    //     if (!previous->next && previous + 1 != payload) {
    //         invalid_addresses += 1;
    //         return NULL;
    //     }

    //     //If payload is found after previous in the linkedlist
    //     if (previous->next + 1 == payload) {
    //         break;
    //     }
    // }


    // meta_data* ptr = realloc(previous->next, sizeof(meta_data) + request_size);
    // //Failed realloc
    // if (!ptr) {
    //     return NULL;
    // }

    // //Fix linkedlist
    // previous->next = ptr;

    // //Handles whether size is increasing or decresing
    // if (request_size > ptr->request_size) {
    //     total_memory_requested += request_size - ptr->request_size;
    // } else if (request_size < ptr->request_size) {
    //     total_memory_freed += ptr->request_size - request_size;
    // }

    // //Update meta_data
    // ptr->request_size = request_size;
    // ptr->filename = filename;
    // ptr->instruction = instruction;

    // return ptr + 1;
/**
 * @brief 
 * 
 */
    meta_data* prev = NULL;
    meta_data* curr = head;

    for (; curr; curr = curr->next) {
        if (curr + 1 == payload) {
            break;
        }
        //Last item in the list not payload
        if (!curr->next) {
            invalid_addresses += 1;
            return NULL;
        }
        prev = curr;
    }

    meta_data* new_ptr = realloc(curr, sizeof(meta_data) + request_size);
    //Failed realloc
    if (!new_ptr) {
        return NULL;
    }

    //Fix linkedlist
    if (prev) {
        prev->next = new_ptr;
    }

    //Handles whether size is increasing or decresing
    if (request_size > new_ptr->request_size) {
        total_memory_requested += request_size - new_ptr->request_size;
    } else if (request_size < new_ptr->request_size) {
        total_memory_freed += new_ptr->request_size - request_size;
    }

    //Update meta_data
    new_ptr->request_size = request_size;
    new_ptr->filename = filename;
    new_ptr->instruction = instruction;

    return new_ptr + 1;
}

void mini_free(void *payload) {
    // your code here
    // Nothing allocated yet
    // puts("free");
    if (!payload) {
        return;
    }
    if (!head) {
        // puts("!head");
        invalid_addresses += 1;
        return;
    }

    meta_data* prev = NULL;
    meta_data* curr = head;

    for (; curr; curr = curr->next) {
        //Found
        if (curr + 1 == payload) {
            break;
        }
        //Last item in the list not payload
        if (!curr->next) {
            invalid_addresses += 1;
            return;
        }
        prev = curr;
    }

    //Fix linkedlist
    if (prev) {
        prev->next = curr->next;
    } else {
        head = curr->next;
    }

    total_memory_freed += curr->request_size;
    free(curr);
/**
 * @brief 
 * 
 */
    // //Special case where head is the payload
    // if (head + 1 == payload) {
    //     // puts("head free");
    //     total_memory_freed += head->request_size;

    //     meta_data* tmp = head->next;
    //     free(head);
    //     head = tmp;
    //     return;
    // }

    // //Assert that head is not payload and there are more than one item
    // //Checks for invalid addresses and gets previous meta_data segment if found
    // meta_data* previous = NULL;
    // for (previous = head; previous; previous = previous->next) {
    //     //Last object in the list
    //     // puts("loop");
    //     if (!previous->next && previous + 1 != payload) {
    //         invalid_addresses += 1;
    //         return;
    //     }
    //     //If payload is found after this in the linkedlist
    //     if (previous->next + 1 == payload) {
    //         break;
    //     }
    // }
    // //ptr should point to the meta_data which ->next is the payload
    // // puts("out loop");

    // meta_data* toDelete = previous->next;
    // meta_data* following = toDelete->next;

    // previous->next = following;

    // total_memory_freed += ((meta_data*)payload - 1)->request_size;

    // free(toDelete);
    // if (!payload) {
    //     return;
    // } else if (!head) {
    //     invalid_addresses += 1;
    //     return;
    // }
}