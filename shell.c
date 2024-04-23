// PID: 9DigitPidNoSpacesOrDashes
// I pledge the COMP 211 honor code.

// All necessary libraries are included in shell.h
#include "shell.h"

void alloc_mem_for_argv(command_t* p_cmd) {
    // TODO:
}

void cleanup(command_t* p_cmd) {
    // TODO:
}

void parse(char* line, command_t* p_cmd) {
    // TODO:
}

bool find_full_path(command_t* p_cmd) {
    // TODO:
    return true;
}

int execute(command_t* p_cmd) {
    // TODO:
    return 0;
}

bool is_builtin(command_t* p_cmd) {
    // Do not modify
    char* executable = p_cmd->argv[0];
    if (strcmp(executable, "cd") == 0 || strcmp(executable, "exit") == 0) {
        return true;
    }
    return false;
}

int do_builtin(command_t* p_cmd) {
    // Do not modify
    if (strcmp(p_cmd->argv[0], "exit") == 0) {
        exit(SUCCESS);
    }

    // cd
    if (p_cmd->argc == 1) {  // cd with no arguments
        return chdir(getenv("HOME"));
    } else if (p_cmd->argc == 2) {  // cd with 1 arg
        return chdir(p_cmd->argv[1]);
    } else {
        fprintf(stderr, "cd: Too many arguments\n");
        return ERROR;
    }
}
