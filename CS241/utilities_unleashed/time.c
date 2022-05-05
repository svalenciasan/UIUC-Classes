/**
 * utilities_unleashed
 * CS 241 - Spring 2022
 */
// #pragma once

#define _POSIX_C_SOURCE 199309L

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>
#include <string.h>

#include "format.h"

int main(int argc, char *argv[]) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // printf("WOWWOWOW\n");
    if (argc < 2 || !argv) {
        print_time_usage();
    }
    
    __pid_t child = fork();
    if (child < 0) {
        print_fork_failed();
        // exit(EXIT_FAILURE);
    } else if (child == 0) {
        //Start timer
        execvp(argv[1], argv + 1);
        print_exec_failed();
    } else {
        int wstatus;
        waitpid(child, &wstatus, 0);

        if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 0) {
            clock_gettime(CLOCK_MONOTONIC, &end);

            double time_taken;
            time_taken = (end.tv_nsec - start.tv_nsec) / 1e9;
            time_taken += (end.tv_sec - start.tv_sec);

            display_results(argv, time_taken);
        }
    }

    return 0;
}
