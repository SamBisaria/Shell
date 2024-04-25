// PID: 730562988
// I pledge the COMP 211 honor code.

// All necessary libraries are included in shell.h
#include "shell.h"

void alloc_mem_for_argv(command_t *p_cmd) {
    int argc = p_cmd->argc;
    p_cmd->argv = (char **) malloc((argc + 1) * sizeof(char *));

    if (p_cmd->argv == NULL) {
        return;
    }

    for (int i = 0; i < argc; i++) {
        p_cmd->argv[i] = (char *) malloc(MAX_ARG_LEN * sizeof(char));

        if (p_cmd->argv[i] == NULL) {
            return;
        }
    }

    p_cmd->argv[argc] = NULL;
}

void cleanup(command_t *p_cmd) {
    if (p_cmd->argv != NULL) {
        for (int i = 0; p_cmd->argv[i] != NULL; i++) {
            free(p_cmd->argv[i]);
            p_cmd->argv[i] = NULL;
        }

        free(p_cmd->argv);
        p_cmd->argv = NULL;
    }
}

void parse(char *line, command_t *p_cmd) {
    if (line == NULL) {
        p_cmd->argc = 0;
        p_cmd->argv = (char **) malloc(sizeof(char *));
        p_cmd->argv[0] = NULL;
        return;
    }

    char *line_copy = strdup(line);
    if (line_copy == NULL) {
        return;
    }

    char *token;
    int argc = 0;
    token = strtok(line_copy, " ");
    while (token != NULL) {
        argc++;
        token = strtok(NULL, " ");
    }

    p_cmd->argc = argc;
    p_cmd->argv = (char **) malloc((argc + 1) * sizeof(char *));
    if (p_cmd->argv == NULL) {
        free(line_copy);
        return;
    }

    char *line_copy_ptr = line_copy;
    int i = 0;
    token = strtok(line_copy_ptr, " ");
    while (token != NULL) {
        p_cmd->argv[i] = strdup(token);
        if (p_cmd->argv[i] == NULL) {
            free(line_copy);
            return;
        }
        i++;
        token = strtok(NULL, " ");
    }
    p_cmd->argv[argc] = NULL;

    free(line_copy);
}

bool find_full_path(command_t *p_cmd) {
    char *path_env = getenv("PATH");
    if (path_env == NULL) {
        return false;
    }

    char *path_copy = strdup(path_env);
    if (path_copy == NULL) {
        return false;
    }

    char *dir_path = strtok(path_copy, ":");
    while (dir_path != NULL) {
        char *full_path = (char *) malloc(strlen(dir_path) + strlen(p_cmd->argv[0]) + 2);
        if (full_path == NULL) {
            free(path_copy);
            return false;
        }

        sprintf(full_path, "%s/%s", dir_path, p_cmd->argv[0]);
        if (access(full_path, F_OK) == 0) {
            free(p_cmd->argv[0]);
            p_cmd->argv[0] = full_path;
            free(path_copy);
            return true;
        }

        free(full_path);
        dir_path = strtok(NULL, ":");
    }

    free(path_copy);
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
    if (pid == -1) {
        perror("fork");
        return ERROR;
    } else if (pid == 0) {
        execv(p_cmd->argv[0], p_cmd->argv);
        printf("Command %s not found!\n", p_cmd->argv[0]);
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        return SUCCESS;
    }
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
