/**
 * nonstop_networking
 * CS 241 - Spring 2022
 */
#include "common.h"

ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
    size_t bytes_read = 0;
    while (bytes_read < count) {
        ssize_t num_bytes = read(socket, buffer + bytes_read, count - bytes_read);
        if (num_bytes == 0) {
            return bytes_read;
        }
        else if (num_bytes > 0) {
            bytes_read += num_bytes;
        }
        else if (num_bytes == -1 && errno == EINTR) {
            continue;
        }
        else {
            return -1;
        }
    }

    return bytes_read;
}

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count) {
    size_t bytes_read = 0;
    while (bytes_read < count) {
        ssize_t num_bytes = write(socket, buffer + bytes_read, count - bytes_read);
        if (num_bytes == 0) {
            return bytes_read;
        }
        else if (num_bytes > 0) {
            bytes_read += num_bytes;
        }
        else if (num_bytes == -1 && errno == EINTR) {
            continue;
        }
        else {
            return -1;
        }
    }

    return bytes_read;
}