/**
 * mini_memcheck
 * CS 241 - Spring 2022
 */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    // Your tests here using malloc and free
    void *p1 = malloc(30);
    // puts("1");
    void *p2 = malloc(40);
    // puts("2");
    void *p3 = malloc(50);
    // puts("3");
    free(p2);
    free(p1);
    free(p3);
    // puts("4");

    p1 = calloc(10, sizeof(char*) * 10);
    p2 = calloc(8, sizeof(char) * 10);
    p3 = calloc(4, sizeof(int) * 10);
    void* p4 = realloc(NULL, 4);

    free(p1);

    void* tmp = realloc(p2, 70);
    // printf("%p\n", tmp);
    void* tmp3 = realloc(tmp, 100);
    void* tmp2 = realloc(tmp3, 100);
    // printf("%p\n", tmp3);
    // printf("%p\n", tmp2);
    // puts("hello");
    // printf("%p\n", tmp);
    tmp = realloc(tmp, 0);

    free(p4);
    // puts("hello");
    free(tmp2);
    // puts("hello");
    free(p3);
    // puts("hello");


    return 0;
}