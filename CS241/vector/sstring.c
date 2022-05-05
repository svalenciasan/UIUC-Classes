/**
 * vector
 * CS 241 - Spring 2022
 */
#include "sstring.h"
#include "vector.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <string.h>

struct sstring {
    // Anything you want
    char* strArr;
    size_t size;
};

sstring *cstr_to_sstring(const char *input) {
    // your code goes here
    sstring* string = (sstring*) malloc(sizeof(sstring));  //FREE

    string->strArr = malloc(strlen(input) + 1);
    strcpy(string->strArr, input);

    string->size = strlen(string->strArr);

    return string;
}

char *sstring_to_cstr(sstring *input) {
    // your code goes here
    char* charStr = malloc(input->size + 1);
    strcpy(charStr, input->strArr);

    return charStr;
}

int sstring_append(sstring *this, sstring *addition) {
    // your code goes here
    this->strArr = realloc(this->strArr, this->size + addition->size + 1);

    strcpy(this->strArr + this->size, addition->strArr);

    this->size = strlen(this->strArr);

    return this->size;
}

vector *sstring_split(sstring *this, char delimiter) {
    // your code goes here
    vector* vect = string_vector_create();   //FREE
    
    // char* copyArr = malloc(this->size + 1);
    // strcpy(copyArr, this->strArr);

    char* beg = this->strArr;
    char* end = this->strArr;

    while (*end != '\0') {
        end = strchr(end, delimiter);

        if (!end) {
            vector_push_back(vect, beg);
            break;
        }

        char tmp = *end;
        *end = '\0';
        vector_push_back(vect, beg);
        *end = tmp;

        ++end;
        beg = end;
    }

    if (this->strArr[this->size - 1] == delimiter) {
        vector_push_back(vect, "\0");
    }

    // free(copyArr);

    return vect;
}

int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution) {
    // // your code goes here
    char* targetPtr = strstr(this->strArr + offset, target);

    if (!targetPtr) {
        return -1;
    }

    char* newArr = malloc(this->size - strlen(target) + strlen(substitution) + 1);    //Free old

    //Save what is after the target
    // char* afterTarget = targetPtr + strlen(target);
    // char* afterTmp = malloc(strlen(afterTarget) + 1);   //FREE
    // strcpy(afterTmp, afterTarget);

    //Copy the beginning until target start
    strncpy(newArr, this->strArr, targetPtr - this->strArr);
    //Copy the substitution in
    strcpy(newArr + (targetPtr - this->strArr), substitution);
    //Copy everything after the target
    strcpy(newArr + strlen(substitution) + (targetPtr - this->strArr), targetPtr + strlen(target));

    free(this->strArr);
    this->strArr = newArr;
    this->size = strlen(this->strArr);

    return 0;
}

char *sstring_slice(sstring *this, int start, int end) {
    // your code goes here
    char* slice = malloc(end - start + 1);

    strncpy(slice, this->strArr + start, end - start);

    slice[end - start] = '\0';

    return slice;
}

void sstring_destroy(sstring *this) {
    // your code goes here
    free(this->strArr); this->strArr = NULL;
    free(this); this = NULL;
}
