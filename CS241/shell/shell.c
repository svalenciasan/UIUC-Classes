/**
 * shell
 * CS 241 - Spring 2022
 */
#include "format.h"
#include "shell.h"
#include "vector.h"
#include "sstring.h"

#include <ulimit.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <utime.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>


/**
 * @brief TODO:
 * 
 * Starting up a shell
 * Optional arguments when launching shell
 * Interaction
 * Built-in commands
 * Foreground external commands
 * Logical operators
 * SIGINT handling
 * Exiting
 */
//Contains full path of current directory.
static char* currentDir = NULL;

//Contains full path of history file.
static char* history_file = NULL;
//File type history
static FILE* historyF = NULL;

//Contains full path of command file.
static char* command_file = NULL;
//File type stream
static FILE* streamF = NULL;

//Contains raw input history. Stores deep copies of input.
static vector* history_vect = NULL;

//Contains list of process
static vector* process_vect = NULL;

typedef struct process {
    char *command;
    pid_t pid;
} process;

/**
 * @brief MY FUNCTIONS
 */
void printVector(vector* vect);

char* getCurrentDir(char* buff);

FILE* openHistoryRW();
void storeInHistoryFile(char* input, char* cmd);
void storeInHistoryVector(const char* input);

void openFileR();

vector* parseCommand(const char* line);

int runHashCommand(vector* parsedIn, const char* rawInput);
int runPrefixCommand(vector* parsedIn, const char* rawInput);

int runBuiltIn(vector* parsedIn, const char* rawInput);
int runExternal(vector* parsedIn, const char* input, bool isBackground);
int runAll(const char* input);

void handleInput();


void printVector(vector* vect) {
    printf("Vector size: %zx\n", vector_size(vect));
    for (size_t i = 0; i < vector_size(vect); ++i) {
        printf("%s-", (char*)vector_get(vect, i));
    }
    printf("\n");
    fflush(stdout);
}

char* getCurrentDir(char* buff) {
    if (!getcwd(buff, FILENAME_MAX)) {
        //  TODO: FORMAT ERROR?
    }
    return buff;
}

/**
 * @brief 
 * Background processes related
 */

void process_destructor(void* elem) {
    process* proc = (process*) elem;

    proc->pid = -1;
    free(proc->command); proc->command = NULL;
    free(proc); elem = NULL;
}

void addProcess(const char* cmd, pid_t pid) {
    process* process = malloc(sizeof(process));//FREE

    char* command_in = malloc(strlen(cmd) + 1);//Free
    strcpy(command_in, cmd);


    process->command = command_in;
    process->pid = pid;

    vector_push_back(process_vect, process);
}

void destroyProcess(pid_t pid) {
    for (size_t i = 0; i < vector_size(process_vect); ++i) {
        process* proc = (process*)vector_get(process_vect, i);
        if (pid == proc->pid) {
            vector_erase(process_vect, i);

            return;
        }
    }
}

process* getProcess(pid_t pid) {
    for (size_t i = 0; i < vector_size(process_vect); ++i) {
        process* proc = (process*)vector_get(process_vect, i);
        if (pid == proc->pid) {
            return proc;
        }
    }
    return NULL;
}

void signalHandler(int signal) {
    //Handles zombie and deletes pid from vector
    if (signal == SIGCHLD) {
        int status = 0;
        pid_t pid = waitpid(-1, &status, WNOHANG);
        while ( pid > 0) {
            destroyProcess(pid);
            pid = waitpid(-1, &status, WNOHANG);
        }
    }
}

/**
 * @brief HISTORY RELATED
 */
void loadHistoryFileContents() {
    //Check if history is being recorded/read
    if (history_file) {
        historyF = fopen(history_file, "r");//w+
        //Failed to open
        if (!historyF) {
            print_history_file_error();
            return;
        }
    } else {
        return;
    }

    //Loads
    char* input = NULL;//FREE
    size_t size = 0;
    while(getline(&input, &size, historyF) != -1 && strcmp(input, "exit\n")) {
        //Remove last \n
        input[strlen(input) - 1] = '\0';
        storeInHistoryVector(input);
    }

    fclose(historyF); historyF = NULL;
    free(input);
}

void storeAllInHistoryFile() {
    //Check if history is being recorded/read
    if (history_file) {
        historyF = fopen(history_file, "w");//w+
        //Failed to open
        if (!historyF) {
            print_history_file_error();
            exit(1);
        }
    } else {
        return;
    }

    for (size_t i = 0; i < vector_size(history_vect); ++i) {
        if (fprintf(historyF,"%s\n" , (char*)vector_get(history_vect, i)) < 0) {
            print_history_file_error();
        }
    }

    fclose(historyF); historyF = NULL;
}

void storeInHistoryVector(const char* input) {
    //Parse input into command and arguments. String vector.
    vector* parsed = parseCommand(input);//FREE
    char* cmd = (char*)vector_get(parsed, 0);

    //Checks for commands to not store in vector
    if (cmd && (!strcmp(cmd, "!history") || cmd[0] == '#' || cmd[0] == '!')) {
        //MEMORY CLEANUP
        vector_destroy(parsed);

        return;
    } else {
        //Store raw input in history vector
        vector_push_back(history_vect, (void*)input);
        //MEMORY CLEANUP
        vector_destroy(parsed);
    }
}

/**
 * @brief Command file related
 */
void openFileR() {
    //Checks to see if using command file.
    if (command_file) {
        streamF = fopen(command_file, "r");
        //Failed to open
        if (!streamF) {
            print_script_file_error();
            exit(1);
        }
    } else {
        // streamF = stdin;
        streamF = NULL;
    }
    return;
}

vector* parseCommand(const char* line) {
    //Copy to avoid changing input
    char * copy = malloc(strlen(line) + 1); 
    strcpy(copy, line);//TODO could fail

    vector* parsedInput = string_vector_create();  //Contains command and arguments FREE

    //Separate string by spaces and store
    char* delim = " \t\n\v\f\r";
    char* token = strtok(copy, delim);
    while (token) {
        vector_push_back(parsedInput, token);
        token = strtok(NULL, delim);
    }

    free(copy);

    return parsedInput;
}

char** parseVectorToArgv(vector* vect) {
    char** argv = malloc(sizeof(char*) * (vector_size(vect) + 1));//Free

    for (size_t i = 0; i < vector_size(vect); ++i) {
        size_t size = strlen(vector_get(vect, i));
        char* arg = malloc(size + 1);//Free extra for null terminating

        strncpy(arg, (char*)vector_get(vect, i), size);//Could fail

        arg[size] = '\0';

        argv[i] = arg;
    }
    //Null terminate array
    // char** endArr = argv + vector_size(vect);
    // endArr = NULL;
    argv[vector_size(vect)] = NULL;

    return argv;
}

void freeArgv(char** argv) {
    for (size_t i = 0; argv[i]; ++i) {
        free(argv[i]); argv[i] = NULL;
    }
    free(argv); argv = NULL;
}

/**
 * @brief 
 * Commands
 */

void andOperator(const char* input) {
    //Separate raw input
    char* inputCopy = (char*)malloc(strlen(input));//Free
    strcpy(inputCopy, input);

    char* operator = strstr(inputCopy, "&&");
    char* firstIn = inputCopy;
    char* secondIn = operator + strlen("&&");
    memcpy(operator , "\0\0", strlen("&&"));

    if (runAll(firstIn) == 0) {
        runAll(secondIn);
    }

    free(inputCopy);
}

void orOperator(const char* input) {
    //Separate raw input
    char* inputCopy = (char*)malloc(strlen(input));//Free
    strcpy(inputCopy, input);

    char* operator = strstr(inputCopy, "||");
    char* firstIn = inputCopy;
    char* secondIn = operator + strlen("||");
    memcpy(operator , "\0\0", strlen("||"));

    if (runAll(firstIn) != 0) {
        runAll(secondIn);
    }

    free(inputCopy);
}

void semiColonOperator(const char* input) {
    char* inputCopy = (char*)malloc(strlen(input));//Free
    strcpy(inputCopy, input);

    char* operator = strstr(inputCopy, ";");
    char* firstIn = inputCopy;
    char* secondIn = operator + strlen(";");
    memcpy( operator , "\0", strlen(";") );

    runAll(firstIn);
    runAll(secondIn);

    free(inputCopy);
}

int runCdCommand(vector* parsedIn, const char* rawInput) {
    // char* cmd = (char*)vector_get(parsedIn, 0);
    size_t size = vector_size(parsedIn);

    if (size != 2) {
        print_invalid_command(rawInput);
    } else if (chdir((char*)vector_get(parsedIn, 1))) {
        print_no_directory(vector_get(parsedIn, 1));
        return 1;
    }
    //Update directory
    currentDir = getCurrentDir(currentDir);
    return 0;
}

int runHistoryCommand(vector* parsedIn, const char* rawInput) {
    // char* cmd = (char*)vector_get(parsedIn, 0);
    size_t size = vector_size(parsedIn);

    if (size != 1) {
        print_invalid_command(rawInput);
        return 1;
    }

    for (size_t i = 0; i < vector_size(history_vect); ++i) {
        print_history_line(i, vector_get(history_vect, i));
    }

    return 0;
}

int runHashCommand(vector* parsedIn, const char* rawInput) {
    char* cmd = (char*)vector_get(parsedIn, 0);
    size_t size = vector_size(parsedIn);

    if (size > 1 || strlen(cmd) < 2) {
        print_invalid_command(rawInput);
        return 1;
    } else {
        //"OTHER VALUES OF N WILL NOT BE TESTED"
        int index = atoi(cmd + 1);
        if (index < 0 || index >= (int)vector_size(history_vect)) {
            print_invalid_index();
            return 1;
        }
        char* found = vector_get(history_vect, index);
        
        print_command(found);

        runAll(found);
        storeInHistoryVector(found);//JUST ADDED TODO

        return 0;
    }
}

int runPrefixCommand(vector* parsedIn, const char* rawInput) {
    if (vector_size(history_vect) < 1) {
        print_no_history_match();
        return 1;
    }

    char find = '!';
    char* found = strchr(rawInput, find);

    if (strlen(found) < 2) {
        char* found = vector_get(history_vect, vector_size(history_vect) - 1);

        print_command(found);

        runAll(found);
        storeInHistoryVector(found);//JUST ADDED TODO

        return 0; 
    }

    for (size_t i = vector_size(history_vect); i > 0; --i) {
        if (!strncmp(found + 1, vector_get(history_vect, i-1), strlen(found + 1))) {
            char* found = vector_get(history_vect, i-1);

            print_command(found);

            runAll(found);
            storeInHistoryVector(found);//JUST ADDED TODO

            return 0;
        }
    }
    print_no_history_match();
    return 1;
}

/**
 * @brief 
 * Functions related to PS command
 */
long int getNthreads(pid_t pid) {
    char filePath[50];
    sprintf(filePath,  "/proc/%ld/status", (long)pid);
    FILE* file = fopen(filePath, "r");
    if (!file) {
        puts("Couldn't open file");//TODO
        return -1;
    }

    char* line = NULL;//FREE
    size_t size = 0;
    while(getline(&line, &size, file) != -1) {
        if (!strncmp("Threads:", line, 8)) {
            long int value = 0;
            
            if (sscanf(line, "%*s%ld", &value) <= 0) {
                puts("scan failed");//TODO
            }

            fclose(file);
            free(line);

            return value;
        }
    }
    puts("x not found");//TODO

    fclose(file);
    free(line);

    return 0;
}

unsigned long int getVsize(pid_t pid) {
    char filePath[50];
    sprintf(filePath,  "/proc/%ld/status", (long)pid);
    FILE* file = fopen(filePath, "r");//Close
    if (!file) {
        puts("Couldn't open file");//TODO
        return -1;
    }

    char* line = NULL;//FREE
    size_t size = 0;
    while(getline(&line, &size, file) != -1) {
        if (!strncmp("VmSize:", line, 7)) {
            unsigned long int value = 0;
            
            if (sscanf(line, "%*s%ld", &value) <= 0) {
                puts("scan failed");//TODO
            }

            fclose(file);
            free(line);

            return value;
        }
    }
    puts("x not found");//TODO

    fclose(file);
    free(line);

    return 0;
}

char getState(pid_t pid) {
    char filePath[50];
    sprintf(filePath,  "/proc/%ld/status", (long)pid);
    FILE* file = fopen(filePath, "r");//CLOSE
    if (!file) {
        puts("Couldn't open file");//TODO
        return -1;
    }

    char* line = NULL;//FREE
    size_t size = 0;
    while(getline(&line, &size, file) != -1) {
        if (!strncmp("State:\t", line, 7)) {
            char value = 0;
            
            if (sscanf(line, "State:\t%c%*s", &value) <= 0) {
                puts("scan failed");//TODO
            }

            fclose(file);
            free(line);

            return value;
        }
    }
    puts("x not found");//TODO

    fclose(file);
    free(line);

    return 0;
}

char* getStart_str(pid_t pid) {
    //startTime
    char filePath[50];
    sprintf(filePath,  "/proc/%ld/stat", (long)pid);

    FILE* file = fopen(filePath, "r");//Close
    if (!file) {
        puts("Couldn't open file");//TODO
        return NULL;
    }

    long long unsigned startTime = 0;

    fscanf(file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld %*ld %llu", &startTime);
    fclose(file);

    //btime
    sprintf(filePath,  "/proc/stat");

    file = fopen(filePath, "r");//Close
    if (!file) {
        puts("Couldn't open file");//TODO
        return NULL;
    }

    char* line = NULL;//FREE
    size_t size = 0;

    long long unsigned btime = 0;

    while(getline(&line, &size, file) != -1) {
        if (!strncmp("btime", line, 5)) {
            if (sscanf(line, "btime %llu", &btime) <= 0) {
                puts("scan failed");//TODO
            }
            break;
        }
    }

    fclose(file);
    free(line);

    //Time
    // time_t timeC = (time_t)((btime + startTime) / sysconf(_SC_CLK_TCK));
    time_t timeC = (time_t)btime + (startTime / sysconf(_SC_CLK_TCK));

    struct tm* tm_info = localtime(&timeC);

    char* buffer = malloc(10);//Free
    time_struct_to_string(buffer, 10, tm_info);

    // free(tm_info);

    return buffer;
}

char* getTime_str(pid_t pid) {
    char filePath[50];
    sprintf(filePath,  "/proc/%ld/stat", (long)pid);//TODO: could change to snprintf
    FILE* file = fopen(filePath, "r");//Close
    if (!file) {
        puts("Couldn't open file");//TODO
        return NULL;
    }

    unsigned long utimeVal = 0;
    unsigned long stimeVal = 0;

    fscanf(file, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu", &utimeVal, &stimeVal);

    unsigned long runTime = (utimeVal + stimeVal) / sysconf(_SC_CLK_TCK);
    size_t minutes = runTime / 60;//TODO might need to change this
    size_t seconds = runTime;

    char* buffer = malloc(10);//Free
    execution_time_to_string(buffer, 10, minutes, seconds);

    fclose(file);

    return buffer;
}

int runPsCommand(vector* parsedIn, const char* rawInput) {
    if (vector_size(parsedIn) != 1) {
        print_invalid_command(rawInput);
        return 1;
    }
    print_process_info_header();
    for (size_t i = 0; i < vector_size(process_vect); ++i) {
        process* proc = vector_get(process_vect, i);

        process_info* process_inf = malloc(sizeof(process_info));//Free
        process_inf->pid = proc->pid;
        process_inf->nthreads = getNthreads(proc->pid);
        process_inf->vsize = getVsize(proc->pid);
        process_inf->state = getState(proc->pid);
        process_inf->start_str = getStart_str(proc->pid);//Free
        process_inf->time_str = getTime_str(proc->pid);//Free
        process_inf->command = proc->command;//Free

        print_process_info(process_inf);

        free(process_inf->start_str);
        free(process_inf->time_str);
        free(process_inf);
    }
    return 0;
}

int killChild(vector* parsedIn, const char* rawInput) {
    size_t size = vector_size(parsedIn);
    if (size != 2) {
        print_invalid_command(rawInput);
        return 1;
    }

    char* pid = (char*)vector_get(parsedIn, 1);
    process* proc = getProcess((pid_t)atoi(pid));

    if (!proc) {
        print_no_process_found(proc->pid);
        return 1;
    }

    
    kill(proc->pid, SIGKILL);
    print_killed_process(proc->pid, proc->command);
    return 0;
}

int stopChild(vector* parsedIn, const char* rawInput) {
    size_t size = vector_size(parsedIn);
    if (size != 2) {
        print_invalid_command(rawInput);
        return 1;
    }

    char* pid = (char*)vector_get(parsedIn, 1);
    process* proc = getProcess((pid_t)atoi(pid));

    if (!proc) {
        print_no_process_found(proc->pid);
        return 1;
    }

    
    kill(proc->pid, SIGSTOP);
    print_stopped_process(proc->pid, proc->command);
    return 0;
}

int contChild(vector* parsedIn, const char* rawInput) {
    size_t size = vector_size(parsedIn);
    if (size != 2) {
        print_invalid_command(rawInput);
        return 1;
    }

    char* pid = (char*)vector_get(parsedIn, 1);
    process* proc = getProcess((pid_t)atoi(pid));

    if (!proc) {
        print_no_process_found(proc->pid);
        return 1;
    }

    
    kill(proc->pid, SIGCONT);
    print_continued_process(proc->pid, proc->command);
    return 0;
}

/**
 * @brief 
 * 
 * @param parsedIn 
 * @param rawInput 
 * @return int -1 if not a builtIn 0 if executed properly 1 if otherwise
 */
int runBuiltIn(vector* parsedIn, const char* rawInput) {
    if (vector_size(parsedIn) < 1) {
        return -1;
    }

    char* cmd = (char*)vector_get(parsedIn, 0);

    //User did not input a command
    if (!cmd) {
        return -1;
    }

    if (!strcmp(cmd, "cd")) {
        return runCdCommand(parsedIn, rawInput);
    } else if (!strcmp(cmd, "!history")) {
        return runHistoryCommand(parsedIn, rawInput);
    } else if (cmd[0] == '#') {
        return runHashCommand(parsedIn, rawInput);
    } else if (cmd[0] == '!') {
        return runPrefixCommand(parsedIn, rawInput);
    } else if (!strcmp(cmd, "ps")) {
        return runPsCommand(parsedIn, rawInput);
    } else if (!strcmp(cmd, "kill")) {
        return killChild(parsedIn, rawInput);
    } else if (!strcmp(cmd, "stop")) {
        return stopChild(parsedIn, rawInput);
    } else if (!strcmp(cmd, "cont")) {
        return contChild(parsedIn, rawInput);
    }

    return -1;
}

char* isRedirection(vector* parsedIn) {
    if (vector_size(parsedIn) < 3) {
        return NULL;
    }

    size_t operatorIndex = vector_size(parsedIn) - 2;
    size_t size = strlen(vector_get(parsedIn, operatorIndex));
    char* operator = malloc(size + 1);//Free
    strncpy(operator, (char*)vector_get(parsedIn, operatorIndex), size);//Could fail
    operator[size] = '\0';

    if (!strcmp(operator, ">") || !strcmp(operator, ">>") || !strcmp(operator, "<")) {
        vector_erase(parsedIn, operatorIndex);

        return operator;//Free
    }

    free(operator);
    return NULL;
}

int runExternal(vector* parsedIn, const char* input, bool isBackground) {
    //Checks if the process is a background process and removes the &
    if (isBackground) {
        vector_pop_back(parsedIn);
    }

    if (vector_size(parsedIn) < 1) {
        return 1;
    }

    //Check if redirection
    char* operator = isRedirection(parsedIn);//Might need freeing
    int fd = -1;
    int savedOut = -1;
    if (operator && !strcmp(operator, ">")) {
        fd = open(vector_get(parsedIn, vector_size(parsedIn) - 1),  O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);

        if (fd < 0) {
            print_redirection_file_error();//TODO FIX
        }

        savedOut = dup(STDOUT_FILENO);
        vector_pop_back(parsedIn);
    } else if (operator && !strcmp(operator, ">>")) {
        fd = open(vector_get(parsedIn, vector_size(parsedIn) - 1),  O_CREAT | O_RDWR | O_APPEND, S_IRWXU);

        if (fd < 0) {
            print_redirection_file_error();
        }

        savedOut = dup(STDOUT_FILENO);
        vector_pop_back(parsedIn);
    } else if ((operator && !strcmp(operator, "<"))) {
        fd = open(vector_get(parsedIn, vector_size(parsedIn) - 1),  O_RDONLY, S_IRWXU);

        if (fd < 0) {
            print_redirection_file_error();
        }

        savedOut = dup(STDIN_FILENO);
        // printVector(parsedIn);
        vector_pop_back(parsedIn);
        // printVector(parsedIn);
    }

    fflush(stdout);
    __pid_t child = fork();
    if (child < 0) {
        print_fork_failed();
        //todo:kill
        return 1;
    } else if (child == 0) {
        if (operator && (!strcmp(operator, ">>") || !strcmp(operator, ">")) && fd != -1) {
            dup2(fd, STDOUT_FILENO);//1
        } else if (operator && !strcmp(operator, "<") && fd != -1) {
            dup2(fd, STDIN_FILENO);
        }

        //Run Process
        char** argv = parseVectorToArgv(parsedIn);//Free
        // printf("argv0:%s\n", argv[0]);
        // printf("argv1:%s\n", argv[1]);
        // printf("fd:%d\n", fd);
        if (streamF) {
            fclose(streamF);//SHOULD OPEN AGAIN?
        }
        execvp(argv[0], argv);

        print_exec_failed(input);

        freeArgv(argv);

        fflush(stdout);
        exit(EXIT_FAILURE);//TODO cHANGE
    } else {
        print_command_executed(child);

        int status = 0;

        if (isBackground) {
            addProcess(input, child);
        } else if (waitpid(child, &status, 0) == -1) {
            if (operator && (!strcmp(operator, ">>") || !strcmp(operator, ">"))) {
                dup2(savedOut, STDOUT_FILENO);
            } else if (operator && (!strcmp(operator, "<"))) {
                dup2(savedOut, STDIN_FILENO);
            }

            close(savedOut);
            free(operator);

            print_wait_failed();
            return 1;
        }

        if (operator && (!strcmp(operator, ">>") || !strcmp(operator, ">"))) {
            dup2(savedOut, STDOUT_FILENO);
        } else if (operator && (!strcmp(operator, "<"))) {
            dup2(savedOut, STDIN_FILENO);
        }
        
        close(savedOut);
        free(operator);

        return status;
    }
}

int runAll(const char* input) {
    //Parse input into command and arguments. String vector.
    vector* parsed = parseCommand(input);//FREE

    bool isBackground = false;
    for (size_t i = 0; i < vector_size(parsed); ++i) {
        char* check = (char*)vector_get(parsed, i);
        if (!strcmp(check, "&&")) {
            andOperator(input);
            vector_destroy(parsed);
            return -1;
        } else if (!strcmp(check, "||")) {
            orOperator(input);
            vector_destroy(parsed);
            return -1;
        } else if (check[strlen(check) - 1] == ';') {
            semiColonOperator(input);
            vector_destroy(parsed);
            return -1;
        } else if (!strcmp(check, "&")) {
            isBackground = true;
            // break;
        }
    }
    int builtOut = runBuiltIn(parsed, input);
    // if (!isBackground) {
    //     int builtOut = runBuiltIn(parsed, input);
    // }
    int externalOut = -1;

    if (builtOut == -1) {
        externalOut = runExternal(parsed, input, isBackground);

        //MEMORY CLEANUP
        vector_destroy(parsed);

        return externalOut;
    }

    //MEMORY CLEANUP
    vector_destroy(parsed);

    return builtOut;
}

void handleInput() {
    //No file being read.
    // if (!streamF) {
    //     streamF = stdin;
    // }

    //Reading input
    if (!streamF) {
        char* input = NULL;//FREE
        size_t size = 0;
        print_prompt(getCurrentDir(currentDir), getpid());
        while(getline(&input, &size, stdin) != -1 && strcmp(input, "exit\n")) {
            char* inputCopy = malloc(strlen(input)); //Free
            strcpy(inputCopy, input);//Could return NULL

            //Remove last \n
            inputCopy[strlen(input) - 1] = '\0';

            //Runs commands.
            runAll(inputCopy);

            //Store raw input in history vector
            storeInHistoryVector(inputCopy);
            // printVector(history_vect);

            print_prompt(getCurrentDir(currentDir), getpid());

            free(inputCopy);
        }

        free(input);
    } else if (streamF) {
        char* input = NULL;//FREE
        size_t size = 0;

        while(getline(&input, &size, streamF) != -1 && strcmp(input, "exit\n")) {
            char* inputCopy = malloc(strlen(input) + 1); //Free
            strcpy(inputCopy, input);//Could return NULL
            inputCopy[strlen(input)] = '\0';

            print_prompt(getCurrentDir(currentDir), getpid());
            
            //Remove last \n
            if (strlen(inputCopy) >= 1 && inputCopy[strlen(inputCopy) - 1] == '\n'){
                inputCopy[strlen(inputCopy) - 1] = '\0';
            }

            //New line format needed for using files
            printf("%s\n", inputCopy);

            //Runs commands.
            runAll(inputCopy);

            //Store raw input in history vector
            storeInHistoryVector(inputCopy);
            // printVector(history_vect);

            free(inputCopy);
        }
        free(input);
    }
}

int shell(int argc, char *argv[]) {
    // TODO: This is the entry point for your shell.
    signal(SIGINT, signalHandler);
    signal(SIGCHLD, signalHandler);

    currentDir = malloc(FILENAME_MAX);  //TODO: FREE
    history_vect = string_vector_create();  // TODO: FREE
    process_vect = vector_create(NULL, process_destructor, NULL);
    // process_vect = shallow_vector_create();//TODo:FREE

    int opt;
    opterr = 0;
    while((opt = getopt(argc, argv, "h:f:")) != -1) {
        switch(opt) {
            case 'h':
                history_file = get_full_path(optarg);   // TODO: FREE
                break;
            case 'f': 
                command_file = get_full_path(optarg);   // TODO: FREE
                break;
            default:
                print_usage();
                exit(1);    // TODO: REMOVE?
                break; 
        }
    }
    //File related
    openFileR();
    loadHistoryFileContents();

    //Adds ./shell to processes vector
    addProcess("./shell", getpid());

    //Contains main loop
    handleInput();

    //Stores in history file
    storeAllInHistoryFile();
    
    //CLEANUP
    if (streamF) {
        fclose(streamF);
    }

    free(currentDir);
    free(history_file);
    free(command_file);

    vector_destroy(history_vect);
    vector_destroy(process_vect);//TODO properly

    return 0;
}
