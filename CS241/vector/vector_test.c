/**
 * vector
 * CS 241 - Spring 2022
 */
#include "vector.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//Primitive tests
bool test(vector* vect, int* arr, size_t size, size_t capacity) {
    if (vector_size(vect) != size) {
        return false;
    } else if (vector_capacity(vect) != capacity) {
        return false;
    }
    //Shallow Copy
    // for (size_t i = 0; i < size; i++) {
    //     if (vector_get(vect, i) != arr[i]) {
    //         return false;
    //     }
    // }
    for (size_t i = 0; i < size; i++) {
        if (*(int*)vector_get(vect, i) != arr[i]) {
            return false;
        }
    }
    return true;
}

void fillVector(vector* vect, int* arr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        vector_push_back(vect, &arr[i]);
    }
}

int main(int argc, char *argv[]) {
    // Write your test cases here

    /**
     * @brief Vector push_back tests + reserve
     * 
     */

    vector* vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    if (!test(vect, (int*){NULL}, 0, 8)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    fillVector(vect, (int[]){0,1}, 2);
    if (!test(vect, (int[]){0,1}, 2, 8)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;
   

    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    fillVector(vect, (int[]){0}, 1);
    if (!test(vect, (int[]){0}, 1, 8)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    fillVector(vect, (int[]){0, 1, 2, 3, 4, 5, 6}, 7);
    if (!test(vect, (int[]){0, 1, 2, 3, 4, 5, 6}, 7, 8)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    fillVector(vect, (int[]){0, 1, 2, 3, 4, 5, 6, 7}, 8);
    if (!test(vect, (int[]){0, 1, 2, 3, 4, 5, 6, 7}, 8, 8)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    fillVector(vect, (int[]){0, 1, 2, 3, 4, 5, 6, 7, 8}, 9);
    if (!test(vect, (int[]){0, 1, 2, 3, 4, 5, 6, 7, 8}, 9, 16)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;

    /**
     * @brief Vector resize tests
     * 
     */

    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    fillVector(vect, (int[]){0, 1, 2}, 3);
    vector_resize(vect, 2);
    if (!test(vect, (int[]){0, 1}, 2, 8)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    fillVector(vect, (int[]){0, 1}, 2);
    vector_resize(vect, 1);
    if (!test(vect, (int[]){0}, 1, 8)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    fillVector(vect, (int[]){0}, 1);
    vector_resize(vect, 0);
    if (!test(vect, (int[]){}, 0, 8)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    fillVector(vect, (int[]){0}, 1);
    vector_resize(vect, 1);
    if (!test(vect, (int[]){0}, 1, 8)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size_t size = 3;
    size_t capacity = 8;
    fillVector(vect, (int[]){0, 1, 2}, size);
    vector_resize(vect, 4);
    if (!test(vect, (int[]){0, 1, 2, 0}, size + 1, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 3;
    capacity = 8;
    fillVector(vect, (int[]){0, 1, 2}, size);
    vector_resize(vect, 5);
    if (!test(vect, (int[]){0, 1, 2, 0, 0}, size + 2, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 3;
    capacity = 8;
    fillVector(vect, (int[]){0, 1, 2}, size);
    vector_resize(vect, 7);
    if (!test(vect, (int[]){0, 1, 2, 0, 0, 0, 0}, 7, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }


    vector_destroy(vect); vect = NULL;
    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 3;
    capacity = 8;
    fillVector(vect, (int[]){0, 1, 2}, size);
    vector_resize(vect, 8);
    if (!test(vect, (int[]){0, 1, 2, 0, 0, 0, 0, 0}, 8, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 3;
    capacity = 16;
    fillVector(vect, (int[]){0, 1, 2}, size);
    vector_resize(vect, 9);
    if (!test(vect, (int[]){0, 1, 2, 0, 0, 0, 0, 0, 0}, 9, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;

    /**
     * @brief vector_pop_back tests
     * 
     */

    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 0;
    capacity = 8;

    fillVector(vect, (int[]){}, 0);
    vector_pop_back(vect);

    if (!test(vect, (int[]){}, size, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 0;
    capacity = 8;

    fillVector(vect, (int[]){0}, 1);
    vector_pop_back(vect);

    if (!test(vect, (int[]){}, size, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 7;
    capacity = 8;

    fillVector(vect, (int[]){0, 1, 2, 3, 4, 5, 6, 7}, 8);
    vector_pop_back(vect);

    if (!test(vect, (int[]){0, 1, 2, 3, 4, 5, 6}, size, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 8;
    capacity = 16;

    fillVector(vect, (int[]){0, 1, 2, 3, 4, 5, 6, 7, 8}, 9);
    vector_pop_back(vect);

    if (!test(vect, (int[]){0, 1, 2, 3, 4, 5, 6, 7}, size, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;

    /**
     * @brief vector_erase tests
     * 
     */
    //ASSERTION FAILS
    // vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    // size = 0;
    // capacity = 8;

    // fillVector(vect, (int[]){}, 0);
    // vector_erase(vect, 0);

    // if (!test(vect, (int[]){}, size, capacity)) {
    //     puts("Failed");
    //     // return false;
    // } else {
    //     puts("Passed");  
    // }
    // vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 0;
    capacity = 8;

    fillVector(vect, (int[]){0}, 1);
    vector_erase(vect, 0);

    if (!test(vect, (int[]){}, size, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;

    //ASSERTION FAILS
    // vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    // size = 1;
    // capacity = 8;

    // fillVector(vect, (int[]){0}, 1);
    // vector_erase(vect, 1);

    // if (!test(vect, (int[]){0}, size, capacity)) {
    //     puts("Failed");
    //     // return false;
    // } else {
    //     puts("Passed");  
    // }
    // vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 7;
    capacity = 8;

    fillVector(vect, (int[]){0, 1, 2, 3, 4, 5, 6, 7}, 8);
    vector_erase(vect, 3);

    if (!test(vect, (int[]){0, 1, 2, 4, 5, 6}, size, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 8;
    capacity = 16;

    fillVector(vect, (int[]){0, 1, 2, 3, 4, 5, 6, 7, 8}, 9);
    vector_erase(vect, 8);

    if (!test(vect, (int[]){0, 1, 2, 3, 4, 5, 6, 7}, size, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;

    /**
     * @brief vector_clear
     * 
     */

    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 0;
    capacity = 8;

    fillVector(vect, (int[]){0}, 1);
    vector_clear(vect);

    if (!test(vect, (int[]){}, size, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 0;
    capacity = 8;

    fillVector(vect, (int[]){0, 1, 2, 3, 4, 5, 6, 7}, 8);
    vector_clear(vect);

    if (!test(vect, (int[]){}, size, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 0;
    capacity = 16;

    fillVector(vect, (int[]){0, 1, 2, 3, 4, 5, 6, 7, 8}, 9);
    vector_clear(vect);

    if (!test(vect, (int[]){}, size, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;

    /**
     * @brief vector_set test
     * 
     */
    // Assert FAILED
    // vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    // size = 1;
    // capacity = 8;

    int a = 0;
    // fillVector(vect, (int[]){}, 0);
    // vector_set(vect, 0, &a);

    // if (!test(vect, (int[]){0}, size, capacity)) {
    //     puts("Failed");
    //     // return false;
    // } else {
    //     puts("Passed");  
    // }
    // vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 1;
    capacity = 8;

    a = 1;

    fillVector(vect, (int[]){1}, 1);
    vector_set(vect, 0, &a);

    if (!test(vect, (int[]){1}, size, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 3;
    capacity = 8;

    a = 3;

    fillVector(vect, (int[]){0,1,2}, 3);
    vector_set(vect, 2, &a);

    if (!test(vect, (int[]){0, 1, 3}, size, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 8;
    capacity = 8;

    a = 10;

    fillVector(vect, (int[]){0, 1, 2, 3, 4, 5, 6, 7}, 8);
    vector_set(vect, 3, &a);

    if (!test(vect, (int[]){0, 1, 2, 10, 4, 5, 6, 7}, size, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;


    vect = vector_create(int_copy_constructor, int_destructor, int_default_constructor);
    size = 9;
    capacity = 16;

    fillVector(vect, (int[]){0, 1, 2, 3, 4, 5, 6, 7, 8}, 9);
    vector_set(vect, 0, &a);

    if (!test(vect, (int[]){10, 1, 2, 3, 4, 5, 6, 7, 8}, size, capacity)) {
        puts("Failed");
        // return false;
    } else {
        puts("Passed");  
    }
    vector_destroy(vect); vect = NULL;   

    

    return 0;
}
