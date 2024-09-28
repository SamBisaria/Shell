// PID: 730562988
// I pledge the COMP 211 honor code.

// All necessary libraries are included in shell.h
#include "shell.h"

void alloc_mem_for_argv(command_t *p_cmd) {
    p_cmd->argv = malloc((p_cmd->argc + 1) * sizeof(char *));
    for (int i = 0; i < p_cmd->argc; i++) {
        p_cmd->argv[i] = malloc(MAX_ARG_LEN * sizeof(char));
    }
    p_cmd->argv[p_cmd->argc] = NULL;
}

void cleanup(command_t *p_cmd) {
    for (int i = 0; i < p_cmd->argc; i++) {
        free(p_cmd->argv[i]);
        p_cmd->argv[i] = NULL;
    }
    free(p_cmd->argv);
    p_cmd->argv = NULL;
}

void parse(char *line, command_t *p_cmd) {
    char *clone = strdup(line);
    char *token = strtok(clone, " ");
    int argc = 0;
    while (token != NULL) {
        argc++;
        token = strtok(NULL, " ");
    }
    p_cmd->argc = argc;
    alloc_mem_for_argv(p_cmd);
    clone = strdup(line);
    token = strtok(clone, " ");
    for (int i = 0; i < argc; i++) {
        strcpy(p_cmd->argv[i], token);
        token = strtok(NULL, " ");
    }
    free(clone);
}

bool find_full_path(command_t *p_cmd) {
    char *path = getenv("PATH");
    char *clone = strdup(path);
    char *token = strtok(clone, ":");
    while (token != NULL) {
        char full_path[1024];
        strcpy(full_path, token);
        strcat(full_path, "/");
        strcat(full_path, p_cmd->argv[0]);
        if (access(full_path, F_OK) == 0) {
            strcpy(p_cmd->argv[0], full_path);
            free(clone);
            return true;
        }
        token = strtok(NULL, ":");
    }
    free(clone);
    return false;
}

int execute(command_t *p_cmd) {
    if (is_builtin(p_cmd)) {
        return do_builtin(p_cmd);
    }
    if (!find_full_path(p_cmd)) {
        printf("Command %s not found!\n", p_cmd->argv[0]);
        return ERROR;
    }
    pid_t pid = fork();
    if (pid == 0) {
        execv(p_cmd->argv[0], p_cmd->argv);
        exit(ERROR);
    } else {
        wait(NULL);
    }
    return SUCCESS;
}
bool is_builtin(command_t *p_cmd) {
    // Do not modify
    char *executable = p_cmd->argv[0];
    if (strcmp(executable, "cd") == 0 || strcmp(executable, "exit") == 0) {
        return true;
    }
    return false;
}

int do_builtin(command_t *p_cmd) {
    // Do not modify
    if (strcmp(p_cmd->argv[0], "exit") == 0) {
        exit(SUCCESS);
    }

    // cd
    if (p_cmd->argc == 1) {// cd with no arguments
        return chdir(getenv("HOME"));
    } else if (p_cmd->argc == 2) {// cd with 1 arg
        return chdir(p_cmd->argv[1]);
    } else {
        fprintf(stderr, "cd: Too many arguments\n");
        return ERROR;
    }
}
