/**
 * vector
 * CS 241 - Spring 2022
 */
#include "sstring.h"

int main(int argc, char *argv[]) {
    // TODO create some tests
    sstring* str = cstr_to_sstring("..ab..cd ef.gh.");
    // sstring* str = cstr_to_sstring("........");
    // sstring* str2 = cstr_to_sstring("..ab..cd ef.gh..");
    vector* vect = sstring_split(str, '.');
    printf("[");
    for (void** itr = vector_begin(vect); itr != vector_end(vect); ++itr) {
        printf("\'%s\', ", *(char**)itr);
    }
    printf("]\n");

    // printf("%d\n", sstring_append(str, str2));

    char* sliced = sstring_slice(str, 2, 5);
    printf("%s\n", sliced);

    sstring_substitute(str, 2, "..", "SUBSTITUTION");
    vector* vect2 = sstring_split(str, '\0');
    printf("[");
    for (void** itr = vector_begin(vect2); itr != vector_end(vect2); ++itr) {
        printf("\'%s\', ", *(char**)itr);
    }
    printf("]\n");

    return 0;
}
