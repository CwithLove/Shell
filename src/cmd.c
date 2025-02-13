#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int nb_cmd(struct cmdline *l) {
    int i = 0;
    while (l->seq[i] != NULL) {
        i++;
    }
    return i;
}

int cd(char *path) {
    if (chdir(path) != 0) {
        perror("cd");
        return 1;
    }
    return 0;
}

int internal(char **cmd) {
    if (strcmp(cmd[0], "exit") == 0 || strcmp(cmd[0], "quit") == 0) {
        exit(0);
    } else if (strcmp(cmd[0], "cd") == 0) {
        if (cmd[1] == NULL) {
            fprintf(stderr, "cd: expected argument\n");
        } else {
            return cd(cmd[1]);
        }
        return 1;
    }
    return 0;
}

static int **create_pipes(int n_cmd) {
    int **pipes = (int **)malloc(sizeof(int *) * n_cmd - 1);
    if (pipes == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    pipes[0] = (int *)malloc(sizeof(int) * 2);
    if (pipes[0] == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipes[0]) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    return pipes;
}

static void connect_pipes(int **pipes) {

}

static void redirection_in_out(struct cmdline *l) {
    
}

void execution(struct cmdline *l) {
    if (l->seq[0] == NULL) {
        return;
    }

    int n_cmd = nb_cmd(l);
    int **pipes;

    if (n_cmd > 1) {
        pipes = create_pipes(n_cmd);
    }

    for (int i = 0; i < n_cmd; i++) {
        if (internal(l->seq[i])) {
            //
        } else {
            pid_t pid;
            pid = Fork();

            if (pid == 0) {
                // Fils
                // dup2()
                if (execvp(l->seq[i][0], l->seq[0]) == -1) {
                    fprintf(stderr, "%s : Command not found\n", l->seq[i][0]);
                    exit(3);
                }
            } else if (pid > 0) {
                pid = waitpid(pid, NULL, 0);
            } else {
                perror("Fork");
            }
        }
    }
}

