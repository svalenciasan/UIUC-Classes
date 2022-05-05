/**
 * extreme_edge_cases
 * CS 241 - Spring 2022
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"

bool test(char **(*camelCaser)(const char *), void (*destroy)(char **), const char* input, char** expected) {
    char** output = (*camelCaser)(input);

    if (!output && !expected) {
        // puts("Passed test");
        return true;
    } else if (!output || !expected) {
        // puts("Failed test");
        return false;
    }

    //Test array size
    if (sizeof(output)/sizeof(char*) != sizeof(expected)/sizeof(char*)) {
        return false;
    }

    for (size_t i = 0; expected[i]; i++) {
        if (strcmp(output[i], expected[i]) != 0) {
            destroy(output);
            // puts("Failed test");
            return false;
        }
    }
    destroy(output);
    // puts("Passed test");
    return true;
}

int test_camelCaser(char **(*camelCaser)(const char *),
                    void (*destroy)(char **)) {
    // TODO: Implement me!
    // puts("Test 1: Empty string");
    if(!test(camel_caser, destroy, "", (char*[]){NULL})) {
        return false;
    }

    // puts("Test 2: NULL");
    if(!test(camel_caser, destroy, NULL, NULL)) {
        return false;
    }

    // puts("Test 3: Spaces");
    if(!test(camel_caser, destroy, "   ", (char*[]){NULL})) {
        return false;
    }

    // puts("Test 4: Punctuation");
    if(!test(camel_caser, destroy, "!\"#%%&\'()*,./:;?@[\\]^_`{|}~", (char*[]){"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", NULL})) {
        return false;
    }

    // puts("Test 5: Random Char");
    if(!test(camel_caser, destroy, "~`!@#$\%^&*()_+=:;'\"?/><\\", (char*[]){"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", NULL})) {
        return false;
    }

    // puts("Test 6: Numbers");
    if(!test(camel_caser, destroy, "abc1 23abc.", (char*[]){"abc123Abc", NULL})) {
        return false;
    }

    // puts("Test 7: Not ending with punctuation");
    if(!test(camel_caser, destroy, "abc1 23abc", (char*[]){NULL})) {
        return false;
    }

    // puts("Test 8: Leading, in between, trailing spaces");
    if(!test(camel_caser, destroy, "   abc.  a bc   . ", (char*[]){"abc", "aBc", NULL})) {
        return false;
    }

    // puts("Test 9: Non printable chars");
    if(!test(camel_caser, destroy, "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F\x20\x7F", (char*[]){NULL})) {
        return false;
    }

    // puts("Test 10: Null string");
    if(!test(camel_caser, destroy, "\0Hello", (char*[]){NULL})) {
        return false;
    }

    if(!test(camel_caser, destroy, "Hello.", (char*[]){"hello", NULL})) {
        return false;
    }

    if(!test(camel_caser, destroy, "HELLO WORLD.", (char*[]){"helloWorld", NULL})) {
        return false;
    }

    if(!test(camel_caser, destroy, "h\ne\fll\ro\twor\vld.", (char*[]){"hELlOWorLd", NULL})) {
        return false;
    }

    if(!test(camel_caser, destroy, "..Hello.There buddy. hi", (char*[]){"", "", "hello", "thereBuddy", NULL})) {
        return false;
    }

    if(!test(camel_caser, destroy, ".   .Hello   . There  buddy. hi", (char*[]){"", "", "hello", "thereBuddy", NULL})) {
        return false;
    }

    if(!test(camel_caser, destroy, ".", (char*[]){"", NULL})) {
        return false;
    }

    if(!test(camel_caser, destroy, "12k5jn23 45p5oi.", (char*[]){"12k5jn2345P5oi", NULL})) {
        return false;
    }

    if(!test(camel_caser, destroy, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.", (char*[]){"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz", NULL})) {
        return false;
    }

    if(!test(camel_caser, destroy, "  .. abc def..  ", (char*[]){"", "", "abcDef", "", NULL})) {
        return false;
    }

    if(!test(camel_caser, destroy, "The Heisenbug is an incredible creature. Facenovel servers get their power from its indeterminism. Code smell can be ignored with INCREDIBLE use of air freshener. God objects are the new religion.", (char*[]){"theHeisenbugIsAnIncredibleCreature",
"facenovelServersGetTheirPowerFromItsIndeterminism",
"codeSmellCanBeIgnoredWithIncredibleUseOfAirFreshener",
"godObjectsAreTheNewReligion",
NULL})) {
        return false;
    }

    return true;
}
