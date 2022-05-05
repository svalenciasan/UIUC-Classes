/**
 * deepfried_dd
 * CS 241 - Spring 2022
 */
#include "format.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <argp.h>
#include <stdbool.h> 

static bool sig1 = false;

void signal_handler(int signal);
FILE* openFile(char* file_name, char* mode);

int main(int argc, char **argv) {
    // ./dd -i input.dat -o output.dat -b 4096 -c 32 -p 2 -k 10
    signal(SIGUSR1, signal_handler);

    FILE* input_fd = stdin;
    FILE* output_fd = stdout;
    size_t block_size = 512;
    size_t num_blocks_to_copy = 0;
    size_t block_skip_input = 0;
    size_t block_skip_output = 0;

    // Signal handling
    int opt;
    while((opt = getopt(argc, argv, "i:o:b:c:p:k:")) != -1) { 
        switch(opt) {
            case 'i':
                // Input file
                input_fd = fopen(optarg, "r");
                if (!input_fd) {
                    print_invalid_input(optarg);
                    exit(1);
                }
                break;
            case 'o':
                // Output file
                output_fd = fopen(optarg, "w");
                if (!output_fd) {
                    print_invalid_input(optarg);
                    exit(1);
                }
                break; 
            case 'b':
                // Block size
                block_size = (size_t)atol(optarg);

                break;
            case 'c':
                // Blocks copied
                num_blocks_to_copy = (size_t)atol(optarg);

                break;
            case 'p':
                // Blocks to skip input
                block_skip_input = (size_t)atol(optarg);

                break;
            case 'k':
                // Blocks to skip output
                block_skip_output = (size_t)atol(optarg);

                break;
            default:
                break;
        }
    }

    // Skip input/output file
    fseek(input_fd, block_skip_input * block_size, SEEK_SET);
    fseek(output_fd, block_skip_output * block_size, SEEK_SET);

    // Start tracking time
    struct timespec starting_time;
    if (clock_gettime(CLOCK_MONOTONIC, &starting_time) == -1) {
        //TODO: handle error
        exit(1);
    }

    size_t full_blocks_in = 0;
    size_t partial_blocks_in = 0;
    size_t total_bytes_copied = 0;

    while (true) {
        // Check if signal
        if (sig1) {
            // Get current time
            struct timespec curr_time;
            if (clock_gettime(CLOCK_MONOTONIC, &curr_time) == -1) {
                //TODO: handle error
                exit(1);
            }
            // Calculate time_elapsed
            double time_elapsed = ((curr_time.tv_nsec - starting_time.tv_nsec) / 1e9) + (double)(curr_time.tv_sec - starting_time.tv_sec);

            print_status_report(full_blocks_in, partial_blocks_in, full_blocks_in, partial_blocks_in, total_bytes_copied, time_elapsed);
            // Reset signal
            sig1 = false;
        }

        // Check if EOF
        if (feof(input_fd)) {
            break;
        } else if (num_blocks_to_copy != 0 && full_blocks_in >= num_blocks_to_copy) {
            break;
        }

        char read_block[block_size];
        size_t num_read = fread(read_block, 1, block_size, input_fd);
        // Error/EOF
        if (num_read == 0) {
            // Check if EOF
            if (feof(input_fd)) {
                break;
            } else {
                // Error
                exit(1);
            }
        }

        if (fwrite(read_block, 1, num_read, output_fd) == 0) {
            exit(1);
        }

        total_bytes_copied += num_read;

        if (num_read >= block_size) {
            ++full_blocks_in;
        } else {
            ++partial_blocks_in;
        }
    }

    // Get current time
    struct timespec curr_time;
    if (clock_gettime(CLOCK_MONOTONIC, &curr_time) == -1) {
        //TODO: handle error
        exit(1);
    }

    // Calculate time_elapsed
    double time_elapsed = ((curr_time.tv_nsec - starting_time.tv_nsec) / 1e9) + (double)(curr_time.tv_sec - starting_time.tv_sec);
    
    // Print final status report
    print_status_report(full_blocks_in, partial_blocks_in, full_blocks_in, partial_blocks_in, total_bytes_copied, time_elapsed);

    return 0;
}

void signal_handler(int signal) {
    if (signal == SIGUSR1) {
        sig1 = true;
    }
}