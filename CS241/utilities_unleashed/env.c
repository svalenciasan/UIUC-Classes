/**
 * utilities_unleashed
 * CS 241 - Spring 2022
 */
#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>
#include <string.h>
#include <ctype.h>

#include "format.h"

char** getKeyValue(const char* arg) {
    // puts(arg);
    char* copystr = strdup(arg);
    char** keyValArr = malloc(sizeof(char*) * 2); //Stores 2 strings, key and value.
    
    keyValArr[0] = NULL;
    keyValArr[1] = NULL;

    char* ptr = strtok(copystr, "=");
    size_t i = 0;
    while(ptr != NULL) {
        keyValArr[i] = ptr;
        ++i;

		ptr = strtok(NULL, "=");
	}

    if (!keyValArr[0] || !keyValArr[1]) {
        print_env_usage();
    }
    
    // puts("getKeyValue Done");
    // puts(keyValArr[0]);
    // puts(keyValArr[1]);
    return keyValArr;
}


int main(int argc, char *argv[]) {
    // printf("WOWWOWOW\n");
    //Process name, x=y, --, cmd
    if (argc < 4 || !argv) {
        print_env_usage();
    }

    // puts(argv[0]);
    // puts(argv[1]);

    __pid_t child = fork();
    if (child < 0) {
        print_fork_failed();
        // exit(EXIT_FAILURE);
    } else if (child == 0) {
        char** arguments = NULL;

        for (size_t i = 1; i < (size_t)argc; ++i) {
            //Sets array of arguments
            if (strcmp(argv[i], "--") == 0) {
                if (i == 1) {
                    print_env_usage();
                }
                //No CMD
                if (i == (size_t)argc) {
                    print_env_usage();
                }
                arguments = argv + i + 1;
                break;
            }

            // puts("HELLOS");
            char** keyValue = getKeyValue((const char*)argv[i]);//TODO: FREE
            //TODO: check if both values exist
            // puts(keyValue[0]);
            // puts(keyValue[1]);
            
            //Sets ENV variables
            //TODO: CHECK FOR %
            char* val = keyValue[1];
            if (keyValue[1][0] == '%') {
                // puts("found %%");
                val = getenv(keyValue[1] + 1);
                if (!val) {
                    val = "";
                }
            }
            // puts(keyValue[0]);
            // puts(val);
            if (setenv(keyValue[0], val, 1) == -1) {
                print_environment_change_failed();
            }

            free(keyValue[0]);
            free(keyValue);
        }
        // puts("HELLOS");
        //Executes arguments
        execvp(arguments[0], arguments);
        print_exec_failed();
    } else {
        int wstatus;
        waitpid(child, &wstatus, 0);

        if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0) {

        }
    }

    return 0;
}