/**
 * nonstop_networking
 * CS 241 - Spring 2022
 */
#include "format.h"
#include "common.h"
#include "vector.h"
#include "dictionary.h"
#include "queue.h"
#include "compare.h"
#include "callbacks.h"

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/stat.h>

// Constants
static size_t MAX_BUFFER_SIZE = 1024;
static const size_t MAX_CLIENTS = 100;
// static size_t HEADER_MAX = 1024;

static int server_socket = -1;// TODO: close
static int epoll_fd = -1;// TODO:close
static char tmp_directory[] = "XXXXXX";
static vector* files_vect = NULL;// TODO: FREE
static dictionary* client_state = NULL;// TODO:FREE

// Connection struct
typedef struct client_info {
	verb request_type;
	char filename[FILENAME_MAX];
    size_t file_size;
    size_t file_offset;
} client_info;

// Helper Functions
void start_server(char* port);
void run_server();
void close_server();

void handle_client(int client_fd);

void parse_verb(int client_fd);
void parse_filename(int client_fd);
bool parse_file_size(int client_fd);

bool is_valid_file(int client_fd);
bool get_local_file_size(int client_fd);
void get_LIST_size(int client_fd);
void add_file(int client_fd);

void handle_request(int client_fd);
void send_OK(int client_fd);
void send_ERROR(int client_fd, const char* error_message);

void GET_request(int client_fd);
void DELETE_request(int client_fd);
void LIST_request(int client_fd);
void PUT_request(int client_fd);

void cleanup_client(int client_fd);


void create_tmp_dir();
void close_tmp_dir();

int main(int argc, char **argv) {
    // Check if correct argv/argc
    if (argc != 2) {
        print_server_usage();
    }

    // Signal handler
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_handler = close_server;

    if (sigaction(SIGINT, &sig, NULL) == -1) {
        perror("sigaction:");
        exit(EXIT_FAILURE);
    }

    // Create tmp directory
    create_tmp_dir();

    // Create needed data structures
    client_state = int_to_shallow_dictionary_create();
    files_vect = string_vector_create();

    // Setup server and start listening
    // puts("Start");
    start_server(argv[1]);
    // puts("Run");
    run_server();

    close_server();

    return 0;
}

void start_server(char* port) {
    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    // Server socket set to reusable
    int optval = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int)) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    optval= 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    // Set up addrinfo
    struct addrinfo hints;
    struct addrinfo* result;// TODO: Free

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int return_state = getaddrinfo(NULL, port, &hints, &result);
    if (return_state != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(return_state));
        freeaddrinfo(result);// todo: needed?
        exit(EXIT_FAILURE);
    }

    // Connect to network port
    if (bind(server_socket, result->ai_addr, result->ai_addrlen) == -1) {
        perror(NULL);
        freeaddrinfo(result);
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);

    // Listen for clients
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
}

void close_server() {
    // Deal with clients
    vector* clients_vect = dictionary_keys(client_state);
    size_t num_clients = vector_size(clients_vect);
    for (size_t i = 0; i < num_clients; ++i) {
        cleanup_client(*(int*)vector_get(clients_vect, i));
    }
    vector_destroy(clients_vect);

    // Close epoll
    close(epoll_fd);

    // Close server
    if (shutdown(server_socket, SHUT_RDWR) != 0) {
        perror("shutdown: ");
    }
    close(server_socket);

    // Close tmp directory
    close_tmp_dir();

    // Free up memory
    dictionary_destroy(client_state);
    vector_destroy(files_vect);

    exit(EXIT_FAILURE);
}

void run_server() {
    // Create epoll
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1: ");
        exit(EXIT_FAILURE);
    }

    // Add server_socket event for listening for clients
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;
    event.data.fd = server_socket;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &event) == -1) {
        perror("epoll_ctl: ");
        exit(EXIT_FAILURE);
    }
    
    // Array for all client events
    struct epoll_event event_array[MAX_CLIENTS];

    // Main loop accepting clients
    while(true) {
        // Wait for clients
        // puts("Waiting");
        int num_events = epoll_wait(epoll_fd, event_array, MAX_CLIENTS, 1000);
        if (num_events == -1) {exit(EXIT_FAILURE);}

        // Look through all events
        for (int i = 0; i < num_events; ++i) {
            if (event_array[i].data.fd == server_socket) {
                // puts("New client");
                // New client connection
                int client_fd = accept(server_socket, NULL, NULL);
                if (client_fd < 0) {
                    perror("accept: ");
                    exit(EXIT_FAILURE);
                }

                // Add client to epoll
                struct epoll_event client_event;
                memset(&client_event, 0, sizeof(client_event));
                client_event.events = EPOLLIN;
                client_event.data.fd = client_fd;

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) == -1) {
                    perror("epoll_ctl: ");
                    exit(EXIT_FAILURE);
                }

                // Add client to dictionary
                client_info* info = calloc(1, sizeof(client_info));
                info->request_type = V_UNKNOWN;

                dictionary_set(client_state, &client_fd, info);
            } else {
                //  Client has sent a request
                handle_client(event_array[i].data.fd);
            }
        }
    }
}

/**
 * Client related
 */

void handle_client(int client_fd) {
    client_info* info = dictionary_get(client_state, &client_fd);
    // puts("handle");
    if (info->request_type == V_UNKNOWN) {
        // Should only run the first time
        parse_verb(client_fd);
        // printf("type:%u\n", info->request_type);
        switch (info->request_type) {
            case GET:
                parse_filename(client_fd);

                if (!get_local_file_size(client_fd)) {
                    // File doesn't exist
                    send_ERROR(client_fd, err_no_such_file);
                    cleanup_client(client_fd);
                    return;
                }

                send_OK(client_fd);

                // Write size to socket
                write_all_to_socket(client_fd, (char*)(&info->file_size), sizeof(size_t));
                break;
            case DELETE:
                parse_filename(client_fd);

                break;
            case PUT:
                parse_filename(client_fd);
                // puts("parsed");
                if (!parse_file_size(client_fd)) {return;}
                add_file(client_fd);

                break;
            case LIST:
                get_LIST_size(client_fd);

                send_OK(client_fd);
                // Write size to socket
                write_all_to_socket(client_fd, (char*)(&info->file_size), sizeof(size_t));

                break;
            case V_UNKNOWN:
                // TODO: ERROR close client connection
                send_ERROR(client_fd, err_bad_request);
                cleanup_client(client_fd);
                print_invalid_response();
                return;
        }
    }
    handle_request(client_fd);
}

void parse_verb(int client_fd) {
    // printf("%zu\n", (size_t)dictionary_contains(client_state, &client_fd));
    client_info* info = dictionary_get(client_state, &client_fd);

    const ssize_t partial_size = 4;
    char request[partial_size];//GET_

    // Read first 4 bytes to tell the type of request
    if (read_all_from_socket(client_fd, request, partial_size) < partial_size) {
        send_ERROR(client_fd, err_bad_request);
        cleanup_client(client_fd);
        return;
    }
    // printf("PARSED VERB:%s|\n", request);
    
    if (strncmp(request, "LIST", partial_size) == 0) {
        // Read remaining part of verb: "\n"
        if (read_all_from_socket(client_fd, request, 1) < 1) {
            send_ERROR(client_fd, err_bad_request);
            cleanup_client(client_fd);
            return;
        }
        // printf("REMAINING:%s|\n", request);
        info->request_type = LIST;

    } else if (strncmp(request, "GET ", partial_size) == 0) {
        info->request_type = GET;

    } else if (strncmp(request, "DELE", partial_size) == 0) {
        // Read remaining part of verb: "te "
        if (read_all_from_socket(client_fd, request, 3) < 3) {
            send_ERROR(client_fd, err_bad_request);
            cleanup_client(client_fd);
            return;
        }
        info->request_type = DELETE;

    } else if (strncmp(request, "PUT ", partial_size) == 0) {
        info->request_type = PUT;

    } else {
        // Not a valid Method
        info->request_type = V_UNKNOWN;

    }
}

void parse_filename(int client_fd) {
    client_info* info = dictionary_get(client_state, &client_fd);

    for (size_t i = 0; i < FILENAME_MAX; ++i) {
        if (read_all_from_socket(client_fd, info->filename + i, 1) == -1) {exit(EXIT_FAILURE);}
        // If reached \n then this is the end of file name
        if (info->filename[i] == '\n') {info->filename[i] = '\0'; break;}
    }
}

bool parse_file_size(int client_fd) {
    client_info* info = dictionary_get(client_state, &client_fd);
    if (read_all_from_socket(client_fd, (char*)(&info->file_size), sizeof(size_t)) < (ssize_t)sizeof(size_t)) {
        send_ERROR(client_fd, err_bad_request);
        cleanup_client(client_fd);
        return false;
    }
    return true;
}

bool is_valid_file(int client_fd) {
    client_info* info = dictionary_get(client_state, &client_fd);

    struct stat file_stat;
    if (stat(info->filename, &file_stat) == -1) {
        perror("stat: ");
        return false;
    }
    // send_ERROR(client_fd, err_no_such_file);
    return true;
}

bool get_local_file_size(int client_fd) {
    client_info* info = dictionary_get(client_state, &client_fd);

    struct stat file_stat;
    if (stat(info->filename, &file_stat) == -1) {
        perror("stat: ");
        // send_ERROR(client_fd, err_no_such_file);
        // cleanup_client(client_fd);
        // exit(EXIT_FAILURE);
        return false;
    }
    info->file_size = file_stat.st_size;
    return true;
}

void get_LIST_size(int client_fd) {
    client_info* info = dictionary_get(client_state, &client_fd);

    size_t num_files = vector_size(files_vect);
    // Size = 0
    if (num_files == 0) {info->file_size = 0; return;}

    for (size_t i = 0; i < num_files - 1; ++i) {
        info->file_size += strlen(vector_get(files_vect, i)) + 1;
    }

    info->file_size += strlen(vector_get(files_vect, num_files - 1));
}

void add_file(int client_fd) {
    client_info* info = dictionary_get(client_state, &client_fd);

    // Make/overwrite file
    FILE* local_file = fopen(info->filename, "w");
    if (!local_file) {exit(EXIT_FAILURE);}
    fclose(local_file);

    // Handle global file list
    for (size_t i = 0; i < vector_size(files_vect); ++i) {
        if (strcmp(info->filename, vector_get(files_vect, i)) == 0) {
            // File already exists
            return;
        }
    }
    // New file, add it to list
    vector_push_back(files_vect, info->filename);
}

void handle_request(int client_fd) {
    client_info* info = dictionary_get(client_state, &client_fd);
    switch (info->request_type) {
        case GET:
            GET_request(client_fd);
            break;
        case DELETE:
            DELETE_request(client_fd);
            break;
        case LIST:
            LIST_request(client_fd);
            break;
        case PUT:
            PUT_request(client_fd);
            break;
        case V_UNKNOWN:
        default:
            puts("SHOULD NEVER BE HERE");
            exit(EXIT_FAILURE);
            break;
    }
}

void GET_request(int client_fd) {
    client_info* info = dictionary_get(client_state, &client_fd);

    // Open for reading
    FILE* local_file = fopen(info->filename, "r");//TODO:Close
    if (!local_file) {exit(EXIT_FAILURE);}

    // printf("File size: %ld\n", info->file_size);
    // Move read pointer
    if (fseek(local_file, info->file_offset, SEEK_SET) == -1) {perror("fseek: "); exit(EXIT_FAILURE);}

    if (info->file_offset < info->file_size) {
        // Remaining bytes
        size_t buffer_size = info->file_size - info->file_offset;
        // Cap buffer if remaining bytes are larger than MAX_BUFFER_SIZE
        buffer_size = buffer_size <= MAX_BUFFER_SIZE ? buffer_size : MAX_BUFFER_SIZE;

        char read_buffer[buffer_size];
        fread(read_buffer, 1, buffer_size, local_file);

        size_t result = write_all_to_socket(client_fd, read_buffer, buffer_size);
        if (result < buffer_size) {exit(EXIT_FAILURE);}

        info->file_offset += result;
    }
    fclose(local_file);

    // printf("Sent: %ld\n", info->file_offset);

    // Need to send more data
    if (info->file_offset < info->file_size) {return;}

    // Finished GET
    cleanup_client(client_fd);
}

void DELETE_request(int client_fd) {
    client_info* info = dictionary_get(client_state, &client_fd);

    if (remove(info->filename) < 0) {
        // File not in directory
        send_ERROR(client_fd, err_no_such_file);
        cleanup_client(client_fd);
        return;
    }

    for (size_t i = 0; i < vector_size(files_vect); ++i) {
        if (strcmp(info->filename, vector_get(files_vect, i)) == 0) {
            vector_erase(files_vect, i);
            send_OK(client_fd);
            cleanup_client(client_fd);
            return;
        }
    }

    // TODO: SHOULD NEVER REACH THIS POINT
    send_ERROR(client_fd, err_no_such_file);
    cleanup_client(client_fd);
    return;
}

void LIST_request(client_fd) {
    client_info* info = dictionary_get(client_state, &client_fd);

    // Size = 0
    if (info->file_size == 0) {
        cleanup_client(client_fd);
        return;
    }

    for (size_t i = 0; i < vector_size(files_vect) - 1; ++i) {
        char* filename = vector_get(files_vect, i);
        // Write filename
        if (write_all_to_socket(client_fd, filename, strlen(filename)) == -1) {exit(EXIT_FAILURE);}
        // Write \n
        if (write_all_to_socket(client_fd, "\n", 1) == -1) {exit(EXIT_FAILURE);}
    }

    // Write the last filename without \n
    char* filename = vector_get(files_vect, vector_size(files_vect) - 1);
    if (write_all_to_socket(client_fd, filename, strlen(filename)) == -1) {exit(EXIT_FAILURE);}

    cleanup_client(client_fd);
}

void PUT_request(int client_fd) {
    client_info* info = dictionary_get(client_state, &client_fd);
    FILE* local_file = fopen(info->filename, "a");//TODO:Close
    if (!local_file) {exit(EXIT_FAILURE);}

    // printf("File size: %ld\n", info->file_size);
    // printf("offset:%zu, size:%zu", info->file_offset, info->file_size);
    if (info->file_offset < info->file_size) {
        // Remaining bytes
        size_t buffer_size = info->file_size - info->file_offset;
        // Cap buffer if remaining bytes are larger than MAX_BUFFER_SIZE
        buffer_size = buffer_size <= MAX_BUFFER_SIZE ? buffer_size : MAX_BUFFER_SIZE;

        char read_buffer[buffer_size];
        ssize_t read_result = read_all_from_socket(client_fd, read_buffer, buffer_size);
        if (read_result == -1) {exit(EXIT_FAILURE);}
        // Too little data sent
        if (read_result < (ssize_t)buffer_size) {
           send_ERROR(client_fd, err_bad_file_size);

            // Remove file
            remove(info->filename);
            // Handle global file list
            for (size_t i = 0; i < vector_size(files_vect); ++i) {
                if (strcmp(info->filename, vector_get(files_vect, i)) == 0) {
                    // File already exists
                    vector_erase(files_vect, i);
                }
            }

            // Finished GET
            cleanup_client(client_fd);
            return;
        }

        fwrite(read_buffer, 1, buffer_size, local_file);

        info->file_offset += read_result;
    }
    fclose(local_file);

    // printf("Sent: %ld\n", info->file_offset);

    // Need to send more data
    if (info->file_offset < info->file_size) {return;}

    // Check if too much data recieved
    char check_buff[2];
    if (read_all_from_socket(client_fd, check_buff, 1 ) > 0) {
        send_ERROR(client_fd, err_bad_file_size);

        // Remove file
        remove(info->filename);
        // Handle global file list
        for (size_t i = 0; i < vector_size(files_vect); ++i) {
            if (strcmp(info->filename, vector_get(files_vect, i)) == 0) {
                // File already exists
                vector_erase(files_vect, i);
            }
        }
    }

    // Finished GET
    send_OK(client_fd);
    cleanup_client(client_fd);
}

void send_OK(int client_fd) {
    char* error = "OK\n";
    if (write_all_to_socket(client_fd, error, strlen(error)) < (ssize_t)strlen(error)){exit(EXIT_FAILURE);}
}

void send_ERROR(int client_fd, const char* error_message) {
    char* error = "ERROR\n";
    char message[strlen(error) + strlen(error_message)];
    sprintf(message, "%s%s", error, error_message);

    if (write_all_to_socket(client_fd, message, strlen(message)) < (ssize_t)strlen(message)){exit(EXIT_FAILURE);}
}

void cleanup_client(int client_fd) {
    // Remove client from epoll
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) == -1) {
        perror("epoll_ctl: ");
        exit(EXIT_FAILURE);
    }

    free(dictionary_get(client_state, &client_fd));
    dictionary_remove(client_state, &client_fd);

    close(client_fd);
}

/**
 * Directory related
 */

void create_tmp_dir() {
    // Create temp directory
    // TODO: fix
    if (mkdtemp(tmp_directory) == NULL) {
        perror("mkdtemp: ");
        exit(EXIT_FAILURE);
    }

    print_temp_directory(tmp_directory);

    // Change process working directory
    if (chdir(tmp_directory) == -1) {
        perror ("chdir: ");
        exit (EXIT_FAILURE);
    }
}

void close_tmp_dir() {
    for (size_t i = 0; i < vector_size(files_vect); ++i) {
        if (remove(vector_get(files_vect, i)) == -1) {
            perror("rmdir file: ");
            exit(EXIT_FAILURE);
        }
    }

    // Change process working directory
    if (chdir("..") == -1) {
        perror ("chdir: ");
        exit (EXIT_FAILURE);
    }

    if (remove(tmp_directory) == -1) {
        perror("rmdir directory: ");
        exit(EXIT_FAILURE);
    }

    // TODO: call unlink
}