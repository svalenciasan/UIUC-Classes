/**
 * perilous_pointers
 * CS 241 - Spring 2022
 */
#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {
    // your code here
    first_step(81);
    
    int step2 = 132;
    second_step(&step2);

    step2 = 8942;
    int* doubleStep = &step2;
    double_step(&doubleStep);

    int strangeInt = 15;
    strange_step((char*)&strangeInt - 5);

    char* empty = "012";
    empty_step(empty);

    char* two = "012u";
    two_step(two, two);

    char* first = "01234";
    char* second = first + 2;
    char* third = second + 2;
    three_step(first, second, third);

    first = "AA";
    second = "III";
    third = "QQQQ";
    step_step_step(first, second, third);

    char a = 65;
    int b = 65;
    it_may_be_odd(&a, b);

    // char *a = strtok(str, ",");
    // a = strtok(NULL, ",");
    // if (strcmp(a, "CS241") == 0) {

    char str[10] = "a,CS241";
    tok_step(str);

    // (orange != NULL && orange == blue && ((char *)blue)[0] == 1 && *((int *)orange) % 3 == 0)
    char blue[5];
    blue[0] = 1;
    blue[1] = 2;
    blue[2] = 0;
    blue[3] = 0;
    blue[4] = 0;
    char* orange = blue;
    the_end(orange, blue);

    return 0;
}
