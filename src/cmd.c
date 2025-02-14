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
    if (strcmp(cmd[0], "exit") == 0 || strcmp(cmd[0], "quit") == 0) {
        exit(0);
    } else if (strcmp(cmd[0], "cd") == 0) { 
        return cd(cmd);
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
    } else if (strcmp(cmd[0], "fg") == 0) {
        /* code */
    } else if (strcmp(cmd[0], "bg") == 0) {
        /* code */
    }
    
    return 0;
}

// static void redirection_in_out(struct cmdline *l) {
//     if (l->in != NULL) {
//         int fd = open(l->in, O_RDONLY);
        
//         if (fd == -1) {
//             perror("open");
//         }
        
//         if (dup2(fd, STDIN_FILENO) == -1) {
//             perror("dup2");
//         }
//     }

//     if (l->out != NULL) {
//         int fd = open(l->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        
//         if (fd == -1) {
//             perror("open");
//         }

//         if (dup2(fd, STDOUT_FILENO) == -1) {
//             perror("dup2");
//         }
//     }
// }

int **create_pipes(int n_cmd) {
    int **pipes = malloc(sizeof(int *) * (n_cmd - 1));
    for (int i = 0; i < n_cmd - 1; i++) {
        pipes[i] = malloc(sizeof(int) * 2);
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(2);
        }
    }
    return pipes;
}

/**
 * @brief Connects the pipes between the commands
 * 
 * @param pipes 
 * @param n_cmd 
 * @param rank_cmd 
 * 
 * Si rank_cmd = 0 
 */

void free_pipes(int **pipes, int n_cmd) {
    for (int i = 0; i < n_cmd - 1; i++)
    {
        free(pipes[i]);
    }
    free(pipes);
}

void connect_pipes(int **pipes, int rank_cmd, int n_cmd) {
    // si rank_cmd = -1
    // Fermer les tubes
    // Fils dup2(tube[0],STDOUT)Tube0(close)  (close)tube1(close) (close)tube2(close)
    // (close)tube[]dup2(tube[1],SDTIN) -> Fils2 ->  ()tube1(close)

    // Boucle i = 0 à n_cmd - 1:
    // Si i = rank_cmd
    // close()
    // ---
    // sinon
    // Fermer les tubes 
}

void connect_in_out(struct cmdline* l, int rank_cmd, int n_cmd) {
    // dup2(l->in, STDIN_FILENO) in -> Fils tube Fils2 tube2 Fils -> out dup2(l->out, STDOUT_FILENO)    
}

void execution(struct cmdline *l) {
    if (l->seq[0] == NULL) {
        return;
    }

    int n_cmd = nb_cmd(l);
    // int **pipes;


    if (n_cmd > 1) {
        // pipes = create_pipes(n_cmd); n_cmd - 1 
    }

    for (int i = 0; i < n_cmd; i++) {
        if (internal(l->seq[i])) {
            //
        } else {
            pid_t pid;
            pid = fork();

            if (pid == 0) {
                // Fils

                // connect_in_out(l, n_cmd);
                // connect_pipes(pipes, n_cmd, i);
                if (execvp(l->seq[i][0], l->seq[0]) == -1) {
                    fprintf(stderr, "%s : Command not found\n", l->seq[i][0]);
                    exit(3);
                }
            } else if (pid > 0) {
                // Pere
                // pid = waitpid(pid, NULL, 0);
                // if (pid == -1) {
                //     perror("Waitpid");
                // }
            } else {
                perror("Fork");
            }

            // connection_pipes(pipes, -1, n_cmd);
            // free_pipes(pipes, n_cmd);
        }
    }
}

