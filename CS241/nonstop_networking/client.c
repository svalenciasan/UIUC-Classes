/**
 * nonstop_networking
 * CS 241 - Spring 2022
 */
#include "format.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"

#include <netdb.h>
#include <arpa/inet.h>

// Global Variables
static size_t CAP_SIZE = 1024;

static int socket_fd = -1;// TODO: close
static char** parsed = NULL;// {host, port, method, remote, local, NULL} TODO: FREE

// Given Helpers
char **parse_args(int argc, char **argv);
verb check_args(char **args);

// Made Helpers
void send_request(verb request_type);
void receive_response(verb request_type);
int check_response_status();
int connect_to_server(const char *host, const char *port);


int main(int argc, char **argv) {
    // Good luck!
    parsed = parse_args(argc, argv);
    verb request_type = check_args(parsed);//todo: argv?

    // Connect Client
    socket_fd = connect_to_server(parsed[0], parsed[1]);

    // Send REQUEST
    printf("Sending request: %u\n", request_type);
    send_request(request_type);

    // Close write
    shutdown(socket_fd, SHUT_WR);

    // Recieve Response
    puts("Receiving server response");
    receive_response(request_type);

    // Close read
    shutdown(socket_fd, SHUT_RD);

    // Close socket
    close(socket_fd);

    // Cleanup
    free(parsed);
    puts("Close server");
}

// MADE HELPERS
void send_request(verb request_type) {
    char* buffer;// TODO: FREE
    switch (request_type) {
        case GET:
        case PUT:
        case DELETE:
            buffer = calloc(sizeof(char), strlen(parsed[2]) + strlen(parsed[3]) + 3);// 3 = space \n \0
            sprintf(buffer, "%s %s\n", parsed[2], parsed[3]);
            break;
        case LIST:
            buffer = calloc(sizeof(char), strlen(parsed[2]) + 2);// \n \0
            sprintf(buffer, "%s\n", parsed[2]);
            break;
        case V_UNKNOWN:
        default:
            // print_invalid_response();
            exit(1);//TODO: remove?
            break;
    }
    
    // Write to server socket and check for errors
    ssize_t bytes_written = write_all_to_socket(socket_fd, buffer, strlen(buffer));
    if (bytes_written < (ssize_t)strlen(buffer)) {
        print_connection_closed();

        free(buffer);

        exit(-1);
    }
    free(buffer);

    // Sending file for PUT request
    if (request_type == PUT) {
        // Open for reading
        FILE* local_file = fopen(parsed[4], "r");//TODO:Close
        if (!local_file) { exit( 1 ); }

        // Get the size of the file
        if (fseek(local_file, 0, SEEK_END) != 0) { fclose(local_file); perror(NULL); exit(1); }
        ssize_t file_size = ftell(local_file);
        if (file_size == -1) { fclose(local_file); perror("Fseek: "); exit(1); }
        rewind(local_file);

        printf("File size: %ld\n", file_size);

        // Write size to socket
        write_all_to_socket(socket_fd, (char*)(&file_size), sizeof(size_t));//TODO: size_t? fail?

        size_t bytes_written = 0;
        while (bytes_written < (size_t)file_size) {
            // Cap the size to write at once
            size_t buffer_size = file_size - bytes_written;
            buffer_size = buffer_size <= CAP_SIZE ? buffer_size : CAP_SIZE;

            char read_buffer[buffer_size];
            fread(read_buffer, 1, buffer_size, local_file);

            size_t result = write_all_to_socket(socket_fd, read_buffer, buffer_size);

            bytes_written += result;
        }

        printf("Sent: %ld\n", file_size);

        fclose(local_file);
    }
}

void receive_response(verb request_type) {
    // Print error message
    if (check_response_status() == 1) { return; }
    // Check all types of request_type
    if (request_type == PUT || request_type == DELETE) {
        print_success();
    } else if (request_type == GET) {
        // Get size of file
        size_t file_size = 0;
        if (read_all_from_socket(socket_fd, (char*)(&file_size), sizeof(size_t)) <= 0) { exit(1); }

        // Open file
        FILE* write_file = fopen(parsed[4], "w");
        if (!write_file) { exit( 1 ); }

        // Write to file
        size_t bytes_written = 0;
        while (bytes_written < file_size) {
            // Cap the size to write at once
            size_t buffer_size = file_size - bytes_written;
            buffer_size = buffer_size <= CAP_SIZE ? buffer_size : CAP_SIZE;

            // Read from server file
            char buffer[buffer_size];
            size_t result = read_all_from_socket(socket_fd, buffer, buffer_size);
            if (result < 0) { exit(1); }
            // Read less than file_size
            if (result == 0) { break; }

            // Write to local file
            fwrite(buffer, 1, buffer_size, write_file);

            bytes_written += result;
        }
        fclose(write_file);  

        // Check if too little data sent
        if (bytes_written < file_size) {
            print_too_little_data();
            exit( 1 );
        }
        // Check if too much data sent
        char check_buff[5];
        if (read_all_from_socket(socket_fd, check_buff, 1 ) > 0) {
            print_received_too_much_data();
            exit( 1 );
        }

        // print_success();
    } else if (request_type == LIST) {
        //TODO add loop like GET
        // Get size of data
        size_t list_size = 0;
        if (read_all_from_socket(socket_fd, (char*)(&list_size), sizeof(size_t)) <= 0) { exit(1); }

        // Read all data
        char read_buffer[list_size + 1];
        size_t bytes_read = read_all_from_socket(socket_fd, read_buffer, list_size);//todo: check if fail
        read_buffer[list_size] = 0;

        puts(read_buffer);

        // Check if too little data sent
        if (bytes_read < list_size) {
            print_too_little_data();
            exit(1);
        } else if (bytes_read > list_size) {
            // Check if too much data sent
            print_received_too_much_data();
            exit(1);
        }
    } else {
        // print_invalid_response();
        exit(1);
    }
}

int check_response_status() {
    const char OK[] = "OK\n";
    const char ERROR[] = "ERROR\n";

    char* buffer = calloc(sizeof(char), strlen(OK) + 1);// Free
    ssize_t read_bytes = read_all_from_socket(socket_fd, buffer, strlen(OK));
    if (read_bytes <= 0) { exit(1); }
    // printf("buff:%s\n", buffer);
    if (!strcmp(buffer, OK)) {
        // Response OK
        printf("%s", OK);

        free(buffer);

        return 0;
    } else {
        // Move reading pointer past ERROR\n
        printf("%s", ERROR);
        char* error = realloc(buffer, sizeof(ERROR));// Free
        if (!error) { free(buffer); exit(1); }
        buffer = error; error = NULL;

        ssize_t bytes_left = read_all_from_socket(socket_fd, buffer, strlen(ERROR) - strlen(OK));
        if (bytes_left == -1) { free(buffer); exit(1); }
        // printf("buff:%s\n", buffer);
        free(buffer); buffer = NULL;

        // Response ERROR
        char* err_message = calloc(sizeof(char), 100);// Free
        if (!err_message) { exit(1); }

        // Get ERROR message
        ssize_t read_bytes = read_all_from_socket(socket_fd, err_message, 100);
        if (read_bytes <= 0) { free(err_message); exit(1); }
        // printf("%lu", read_bytes);
        // printf("Error Message:%s\n", err_message);
        print_error_message(err_message);

        free(err_message);

        return 1;
    }
}

/**
 * Sets up a connection to a chatroom server and returns
 * the file descriptor associated with the connection.
 *
 * host - Server to connect to.
 * port - Port to connect to server on.
 *
 * Returns integer of valid file descriptor, or exit(1) on failure.
 */
int connect_to_server(const char *host, const char *port) {
    // Set up addrinfo
    struct addrinfo hints;
    struct addrinfo* result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int s = getaddrinfo(host, port, &hints, &result);
    if (s != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
      freeaddrinfo(result);
      exit(1);
    }

    int sock_fd = socket(result->ai_family, result->ai_socktype, 0);
    if (sock_fd == -1) {
      perror(NULL);
      exit(1);
    }

    int ok = connect(sock_fd, result->ai_addr, result->ai_addrlen);
    if (ok == -1) {
        perror(NULL);
        exit(1);
    }

    freeaddrinfo(result);

    return sock_fd;
}

// GIVEN HELPERS

/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}
