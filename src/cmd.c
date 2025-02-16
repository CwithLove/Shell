#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern gid_t gpid;

static int nb_cmd(struct cmdline *l) {
    int i = 0;
    while (l->seq[i] != NULL) {
        i++;
    }
    return i;
}

static int cd(char **cmd) {
    char *path = malloc(sizeof(char) * MAXPATH + 1);
    if (path == NULL) {
        perror("malloc");
    }

    if (cmd[1] == NULL) {
        char *tem = getenv("HOME");
        if (tem != NULL) {
            strncpy(path, tem, MAXPATH);
        }

    } else if (cmd[1][0] == '~') {
        if (cmd[1][1] == '/' || cmd[1][1] == '\0') {
            char *tem = getenv("HOME");
            if (tem != NULL) {
                snprintf(path, MAXPATH, "%s%s", tem, cmd[1] + 1);
            }
        } else {
            perror("cd");
        }
    } else {
        strncpy(path, cmd[1], MAXPATH);
    }
    
    if (chdir(path) != 0) {
        perror("cd");
        return 1;
    }

    char cwd[MAXPATH];
    getcwd(cwd, MAXPATH);
    setenv("PWD", cwd, 1);
    free(path);
    return 1;
}

int internal(char **cmd) {
    if (!strcmp(cmd[0], "exit") || !strcmp(cmd[0], "quit")) {
        exit(0);
    } else if (!strcmp(cmd[0], "cd")) { 
        return cd(cmd);
    } else if (!strcmp(cmd[0], "sleep")) {
        if (cmd[1] == NULL) {
            fprintf(stderr, "sleep: expected argument\n");
        } else {
            sleep(atoi(cmd[1]));
        }
        return 1;
    } else if (!strcmp(cmd[0], "kill")) {
        if (cmd[1] == NULL) {
            fprintf(stderr, "kill: expected argument\n");
        } else {
            kill(atoi(cmd[1]), SIGKILL);
        }
        return 1;
    } else if (!strcmp(cmd[0], "fg")) {
        /* code */
    } else if (!strcmp(cmd[0], "bg")) {
        /* code */
    }
    
    return 0;
}

int **create_pipes(int n_cmd) {
    int **pipes = (int **)malloc(sizeof(int *) * (n_cmd - 1));
    if (pipes == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n_cmd - 1; i++) {
        pipes[i] = (int *)malloc(sizeof(int) * 2);
        if (pipes[i] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }
    return pipes;
}

void connect_pipes(int **pipes, int rang_cmd, int n_cmd) {
    for (int i = 0; i < n_cmd - 1; i++) {
        if (i == rang_cmd) {
            close(pipes[i][0]);
            if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                perror("dup2 in connect");
                exit(EXIT_FAILURE);
            }
            close(pipes[i][1]);
        }

        else if (i == rang_cmd - 1) {
            close(pipes[i][1]);
            if (dup2(pipes[i][0], STDIN_FILENO) == -1) {
                perror("dup2 out pipe");
                exit(EXIT_FAILURE);
            }
            close(pipes[i][0]);
        } else {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }  
    } 
}

void free_pipes(int **pipes, int n_cmd) {
    for (int i = 0; i < n_cmd - 1; i++)
    {
        free(pipes[i]);
    }
    free(pipes);
}

void connect_in_out(struct cmdline* l, int rang_cmd, int n_cmd) 
{
    int fd;  
    if (rang_cmd == 0 && l->in != NULL) {
        fd = open(l->in, O_RDONLY);
        if (fd == -1) {
            perror(l->in);
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDIN_FILENO) == -1) {
            perror("dup2 in");
            exit(EXIT_FAILURE);
        }
        close(fd);
    } if (rang_cmd == n_cmd - 1 && l->out != NULL) {
        fd = open(l->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            perror(l->out);
        }

        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2 out");
            exit(EXIT_FAILURE);
        }
        close(fd);
    }
}

void execution(struct cmdline *l) {
    if (l->seq[0] == NULL) {
        return;
    }

    int n_cmd = nb_cmd(l);
    int **pipes;
    int nb_child = 0;
    pid_t pid;
    gpid = -1;

    if (n_cmd > 0) {
        pipes = create_pipes(n_cmd);
    }

    pid_t *child_pids = malloc(sizeof(pid_t) * n_cmd);
    if (child_pids == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (int cmd = 0; cmd < n_cmd; cmd++) {
        if (internal(l->seq[cmd])) {
            continue;
        } 
    
        pid = fork();
        if (pid == -1) {
            perror("Fork");
        } else if (pid > 0) {
            // Pere
            child_pids[nb_child++] = pid;
            if (gpid == -1) {
                gpid = pid;
                
            }
            setpgid(pid, gpid);
        } else {
            // Fils
            connect_in_out(l,cmd,n_cmd);
            connect_pipes(pipes, cmd,n_cmd);
            free_pipes(pipes, n_cmd);
            if (execvp(l->seq[cmd][0], l->seq[cmd]) == -1) {
                fprintf(stderr, "%s : Command not found\n", l->seq[cmd][0]);
                exit(3);
            }
        }
    }

    connect_pipes(pipes, -1, n_cmd);
    free_pipes(pipes, n_cmd);

    for (int i = 0; i < nb_child; i++) {
        if (waitpid(child_pids[i], NULL, 0) == -1) {
            perror("waitpid");
        }
    }

}

