/**
 * extreme_edge_cases
 * CS 241 - Spring 2022
 */
#include "camelCaser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

void printArr(const char** input) {
    if (!input) {
        return;
    }
    size_t lastIndex = 0;
    for(size_t i = 0; input[i]; i++) {
        printf("\"%s\"\n", input[i]);
        lastIndex++;
    }
    if (!input[lastIndex]) {
        printf("%p\n", input[lastIndex]);
    }
}

void lowerCaseString(char* input_string) {
    if (!input_string) {
        return;
    }
    for (size_t i = 0; input_string[i] != '\0'; i++) {
        input_string[i] = tolower(input_string[i]);
    }
}

int countPunct(const char* input_str) {
    int count = 0;
    if (!input_str) {
        return 0;
    }
    
    for (size_t i = 0; i < strlen(input_str); i++) {
        if (ispunct(input_str[i])) {
            count++;
        }
    }
    
    return count;
}

void separateStrings(char* input_str, char** arr) {
    size_t begSentence = 0;
    size_t arrIndex = 0;
    for (size_t i = 0; input_str[i] != '\0'; i++) {
        if (ispunct(input_str[i])) {
            if (begSentence == i) {
                arr[arrIndex] = input_str + begSentence;
                arrIndex++;

                begSentence++;

                input_str[i] = '\0';
                continue;
            }

            arr[arrIndex] = input_str + begSentence;
            arrIndex++;

            begSentence = i + 1;

            input_str[i] = '\0';
        }
    }
    arr[arrIndex] = NULL;
}

//ignore leading spaces
//ignore illegal chars still capitalize first alpha
void upperString(char* input_string) {
    //Ignore leading zeros
    size_t i = 0;
    for (; input_string[i] != '\0'; i++) {
        if (!isspace(input_string[i])) {
            break;
        }
    }
    
    bool canUpper = false;
    for (size_t j = i; input_string[j] != '\0'; j++) {
        if (canUpper && isalpha(input_string[j])) {
            input_string[j] = toupper(input_string[j]);
            canUpper = false;
        }
        if (isspace(input_string[j])) {
            canUpper = true;
            continue;
        }
    }
}

void upperArr(char** arr) {
    if (!arr) {
        return;
    }
    for (size_t i = 0; arr[i]; i++) {
        upperString(arr[i]);
    }
}

char* removeSpace(char* input_str) {
    if (!input_str) {
        return NULL;
    }

    char* newStr = (char*) malloc(strlen(input_str));

    size_t j = 0;
    for (size_t i = 0; input_str[i] != '\0'; i++) {
        if (!isspace(input_str[i])) {
            newStr[j] = input_str[i];
            j++;
        }
    }
    newStr[j] = '\0';

    return newStr;
}

void removeSpacesArr(char** arr) {
    if (!arr) {
        return;
    }
    for (size_t i = 0; arr[i]; i++) {
        // printf("Before remSpace: %s\n", arr[i]);
        arr[i] = removeSpace(arr[i]);
        // printf("After remSpace: %s\n", arr[i]);
    }
}

char **camel_caser(const char *input_str) {
    if (!input_str) {
        return NULL;
    }
    //Allocate heap memory
    char* copyStr = NULL;
    asprintf(&copyStr, "%s", input_str);                                    //Free

    //Lowercase string
    lowerCaseString(copyStr);

    //Allocate memory for array
    char** arr = (char**) malloc(sizeof(char*) * (countPunct(copyStr) + 1));  //Free

    //Separate string by '.' and store pointers in arr
    separateStrings(copyStr, arr);
    // printArr((const char**)arr);

    //Uppercase after space all strings
    upperArr(arr);

    //Remove spaces
    removeSpacesArr(arr);
    // printArr((const char**)arr);

    //Clear up old copy
    free(copyStr);

    return arr;
}

void destroy(char **result) {
    // TODO: Implement me!
    if (!result) {
        return;
    }

    for (size_t i = 0; result[i]; i++) {
        free(result[i]);
    }

    free(result);

    return;
}