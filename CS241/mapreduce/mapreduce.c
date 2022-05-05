/**
 * mapreduce
 * CS 241 - Spring 2022
 */
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    if (argc != 6) {
      print_usage();
      return 1;
    }

    char* input_file = argv[1];
    char* output_file = argv[2];
    char* mapper_executable = argv[3];
    char* reducer_executable = argv[4];
    char* mapper_count = argv[5];
    int mapper_count_int = atoi(mapper_count);

    // printf("%s, %s, %s, %s, %s\n", input_file, output_file, mapper_executable, reducer_executable, mapper_count);

    // Create an input pipe for each mapper.
    int* pipe_mapper[mapper_count_int];// Free
    for(int i = 0; i < mapper_count_int; ++i) {
        pipe_mapper[i] = calloc(2, sizeof(int));// Read/Write ends
        pipe(pipe_mapper[i]);
    }

    // Create one input pipe for the reducer.
    int pipe_reducer[2];
    pipe(pipe_reducer);

    // Open the output file.
    int fd_output_file = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);

    // Start a splitter process for each mapper.
    pid_t splitter_pid[mapper_count_int];
    for(int i = 0; i < mapper_count_int; ++i) {
        // Int to char*
        char i_str[10];
        sprintf(i_str, "%d", i);
        // puts(i_str);
        // pid_t child = fork();
        splitter_pid[i] = fork();
        // printf("%d\n", splitter_pid[i]);

        if(splitter_pid[i] == -1) {
            // Error
            exit(1);
        } else if(splitter_pid[i] == 0) {
            // Child
            // puts("Child SPLIT");
            close(pipe_mapper[i][0]);// Close read end mapper
            dup2(pipe_mapper[i][1], STDOUT_FILENO);// stdout is write mapper pipe

            execl("./splitter", "./splitter", input_file, mapper_count, i_str, NULL);

            exit(1);
        } else {
            // Parent
            close(pipe_mapper[i][1]);// Close write end mapper

            // wait(NULL);
        }
    }

    // Start all the mapper processes.
    pid_t mapper_pid[mapper_count_int];
    for(int i = 0; i < mapper_count_int; ++i) {
        // Int to char*
        mapper_pid[i] = fork();

        if(mapper_pid[i] == -1) {
            // Error
            exit(1);
        } else if(mapper_pid[i] == 0) {
            // Child
            // puts("Child MAP");
            // close(pipe_mapper[i][1]);// Close write end mapper
            dup2(pipe_mapper[i][0], STDIN_FILENO);// stdin is read mapper

            close(pipe_reducer[0]);// Close read end reducer TODO FIX
            dup2(pipe_reducer[1], STDOUT_FILENO);// stdout is write reducer

            execl(mapper_executable, mapper_executable, NULL);

            exit(1);
        } else {
            // Parent
            // close(pipe_mapper[i][0]);// Close read end

            // close(pipe_reducer[0]);// Close read end
            // close(pipe_reducer[1]);// Close write end reducer todo:remove
            // wait(NULL);
        }
    }
    close(pipe_reducer[1]);

    // Start the reducer process.
    pid_t reducer_pid = fork();

    if(reducer_pid == -1) {
        // Error
        exit(1);
    } else if(reducer_pid == 0) {
        // Child
        // puts("Child REDUCE");
        // close(pipe_reducer[1]);// Close write end reducer

        dup2(pipe_reducer[0], STDIN_FILENO);// Stdin is read reducer
        dup2(fd_output_file, STDOUT_FILENO);// Stdout is output

        execl(reducer_executable, reducer_executable, NULL);

        exit(1);
    } else {
        // Parent
        // wait(NULL);
        close(pipe_reducer[0]);// Close read end reducer
        close(fd_output_file);
    }

    // Wait for the reducer to finish.
    // Splitter
    for(int i = 0; i < mapper_count_int; ++i) {
        int status;
        waitpid(splitter_pid[i], &status, 0);
    }
    // Mapper
    for(int i = 0; i < mapper_count_int; ++i) {
        close(pipe_mapper[i][0]);// Close read end mapper

        int status;
        waitpid(mapper_pid[i], &status, 0);
    }
    // Reducer
    int status;
    waitpid(reducer_pid, &status, 0);

    // Print nonzero subprocess exit codes.
    if(status != 0) {
        print_nonzero_exit_status(reducer_executable, status);
    }

    // Count the number of lines in the output file.
    print_num_lines(output_file);

    // Cleanup
    for (int i = 0; i < mapper_count_int; ++i) {
      free(pipe_mapper[i]);
    }

    return 0;
}