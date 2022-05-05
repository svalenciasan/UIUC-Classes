/**
 * malloc
 * CS 241 - Spring 2022
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

typedef struct meta_data {
    size_t size;
    bool free;//0(in use/False) or 1(available/True)

    struct meta_data* next;
    struct meta_data* prev;

    struct meta_data* next_free;
    struct meta_data* prev_free;

    void* ptr;
} meta_data;

static const size_t meta_data_size = sizeof(meta_data);
static meta_data* tail = NULL;

static meta_data* free_tail = NULL;
// static size_t freeMem = 0;//Alternative we could store the largest piece of free memory

/**
 * @brief 
 * Add to free list
 */
void addFree(meta_data* meta_ptr) {
    assert(meta_ptr->free);
    // meta_ptr->free = true;

    meta_ptr->next_free = NULL;
    meta_ptr->prev_free = free_tail;

    if (free_tail) {
        free_tail->next_free = meta_ptr;
    }

    free_tail = meta_ptr;
}

void removeFree(meta_data* meta_ptr) {
    assert(free_tail);//At least one item in free list
    assert(meta_ptr->free == false);
    // meta_ptr->free;

    meta_data* pFree = meta_ptr->prev_free;
    meta_data* nFree = meta_ptr->next_free;

    if (pFree) {
        pFree->next_free = nFree;
    }
    if (nFree) {
        nFree->prev_free = pFree;
    }

    if (free_tail == meta_ptr) {
        free_tail = pFree;
    }

    //SANITY
    meta_ptr->prev_free = NULL;
    meta_ptr->next_free = NULL;
}

/**
 * @brief 
 * JOIN BLOCK HELPERS
 */

void joinPrevious(meta_data** meta_ptr) {
    meta_data* entry = *meta_ptr;
    //Join with previous block
    meta_data* previous = entry->prev;
    if (previous && previous->free) {
        previous->size += entry->size + meta_data_size;
        previous->next = entry->next;

        //Fix metadata from entry next
        if (previous->next) {
            previous->next->prev = previous;
        }

        //Fix tail pointer
        if (tail == entry) {
            tail = previous;
        }

        if (entry->free) {
            //Fix free list
            entry->free = false;//TODO
            removeFree(entry);

            // addFree(previous);
        } else {
            entry->free = false;//todo remove?
            //Copy data from entry into previous block
            memcpy(previous->ptr, entry->ptr, entry->size);//TODO: fix SIGFAULT
            
            previous->free = false;
            removeFree(previous);
        }

        *meta_ptr = previous;//Does this change outside pointer?
    }
}

void joinFollowing(meta_data** meta_ptr) {//TODO: turn back
    meta_data* entry = *meta_ptr;
    //Join with block ahead
    meta_data* next = entry->next;
    if (next && next->free) {
        entry->size += next->size + meta_data_size;
        entry->next = next->next;
        //Fix metadata from new next entry
        if (entry->next) {
            entry->next->prev = entry;
        }

        //Fix tail pointer
        if (tail == next) {
            tail = entry;
        }

        //Fix free list
        next->free = false;//TODO
        removeFree(next);
    }
}

void joinBlock(meta_data** meta_ptr) {
    //Join with previous block
    joinPrevious(meta_ptr);
    //Join with block ahead
    joinFollowing(meta_ptr);
}

//Splits the block creating a new block in front of the entry. Does not change entry* or entry->ptr.
//Size is size of entry after the split
void splitBlock(meta_data* entry, size_t size) {
    assert(!entry->free);

    if (entry->size <= meta_data_size + size) {
        return;
    }

    meta_data* split = (meta_data*) ((void*)entry + meta_data_size + size);
    split->ptr = split + 1;
    split->size = entry->size - meta_data_size - size;
    split->free = true;
    split->prev = entry;
    split->next = entry->next;

    //Fix metadata from new next entry 
    if (split->next) {
        split->next->prev = split;
    }

    //Fix tail pointer
    if (tail == entry) {
        tail = split;
    }

    entry->next = split;
    entry->size = size;

    //Free list
    addFree(split);
    // freeMem += split->size;//todo change
}

/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
    // implement calloc!
    void* newMem = malloc(size * num);

    //Set memory to zero
    if (newMem) {
        memset(newMem, 0, size * num);
    }
    return newMem;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {
    // implement malloc!
    if (size == 0) { return NULL; }

    meta_data* curr = free_tail;
    meta_data* chosen = NULL;

    //Looking for free blocks
    while(curr) {
        //Free block found
        assert(curr->free);
        if (curr->size >= size) {
            chosen = curr;

            chosen->free = false;
            removeFree(chosen);

            splitBlock(chosen, size);

            return chosen->ptr;
        }
        curr = curr->prev_free;
    }

    //No free blocks with size found
    void* block = sbrk(meta_data_size + size);
    if (!block) { return NULL; }

    chosen = (meta_data*) block;
    // chosen->ptr = block + meta_data_size;//chosen + 1
    chosen->ptr = chosen + 1;

    chosen->size = size;
    chosen->free = false;
    chosen->next = NULL;
    chosen->prev = tail;
    chosen->next_free = NULL;
    chosen->prev_free = NULL;

    if (tail) {
        tail->next = chosen;
    }

    tail = chosen;

    return chosen->ptr;
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {
    // implement free!
    if(!ptr) {
        return;
    }

    meta_data* curr = ptr - meta_data_size;
    assert(curr->ptr == ptr);
    assert(curr->free == false);

    // freeMem += curr->size;
    curr->free = true;
    addFree(curr);

    joinBlock(&curr);
}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
    // implement realloc!
    if (!ptr) { return malloc(size); }
    if (size == 0) { free(ptr); return NULL; }

    meta_data* curr = ptr - meta_data_size;
    assert(curr->ptr == ptr);
    assert(curr->free == false);//TODO:MIGHT REMOVE

    if (curr->size >= size) {//TODO: CHANGE TO ==
        //Same size no change
        return curr->ptr;
    } else if (curr->size < size) {
        // void* newMem = malloc(size);
        // if (!newMem) {
        //     return NULL;
        // }

        // memcpy(newMem, curr->ptr, curr->size);
        // free(curr->ptr);

        // return newMem;

        // Checks if there is adjacent memory available to realloc larger size
        size_t previousSpace = 0;
        if (curr->prev && curr->prev->free) {
            previousSpace += curr->prev->size + meta_data_size;
        }
        size_t followingSpace = 0;
        if (curr->next && curr->next->free) {
            followingSpace += curr->next->size  + meta_data_size;
        }

        //Grabs the largest block adjacent TODO:CHANGE
        if (previousSpace >= size && previousSpace > followingSpace) {
            joinPrevious(&curr);
        } else if (followingSpace >= size && followingSpace >= previousSpace) {
            joinFollowing(&curr);
        } else if (previousSpace + followingSpace >= size) {
            joinBlock(&curr);
        } else {
            void* newMem = malloc(size);
            if (!newMem) {
                return NULL;
            }

            memcpy(newMem, curr->ptr, curr->size);
            free(curr->ptr);

            return newMem;
        }
    }
    //If new size was allocated from adjacent blocks this still runs to split off any extra size that is left over.
    //Else curr->size > size
    splitBlock(curr, size);
    return curr->ptr;
}
