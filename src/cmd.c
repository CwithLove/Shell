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
    } else if (strcmp(cmd[0], "sleep") == 0) {
        if (cmd[1] == NULL) {
            fprintf(stderr, "sleep: expected argument\n");
        } else {
            sleep(atoi(cmd[1]));
        }
        return 1;
    } else if (strcmp(cmd[0], "kill") == 0) {
        if (cmd[1] == NULL) {
            fprintf(stderr, "kill: expected argument\n");
        } else {
            kill(atoi(cmd[1]), SIGKILL);
        }
        return 1;
    } else
    {
        /* code */
    }
    
    return 0;
}

static void redirection_in_out(struct cmdline *l) {
    if (l->in != NULL) {
        int fd = open(l->in, O_RDONLY);
        
        if (fd == -1) {
            perror("open");
        }
        
        if (dup2(fd, STDIN_FILENO) == -1) {
            perror("dup2");
        }
    }

    if (l->out != NULL) {
        int fd = open(l->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        
        if (fd == -1) {
            perror("open");
        }

        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2");
        }
    }
}

void execution(struct cmdline *l) {
    if (l->seq[0] == NULL) {
        return;
    }

    int n_cmd = nb_cmd(l);
    // int **pipes;

    if (n_cmd > 1) {
        // pipes = create_pipes(n_cmd);
    }

    for (int i = 0; i < n_cmd; i++) {
        if (internal(l->seq[i])) {
            //
        } else {
            pid_t pid;
            pid = fork();

            if (pid == 0) {
                // Fils
                redirection_in_out(l);
                if (execvp(l->seq[i][0], l->seq[0]) == -1) {
                    fprintf(stderr, "%s : Command not found\n", l->seq[i][0]);
                    exit(3);
                }
            } else if (pid > 0) {
                // Pere
                pid = waitpid(pid, NULL, 0);
                if (pid == -1) {
                    perror("Waitpid");
                }
            } else {
                perror("Fork");
            }
        }
    }
}

