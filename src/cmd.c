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
    // path = getenv("PWD");
    // path = "/home/n/nguyenc/L3_2025/SR/L3_SR/Shell/headers";
    // printf("%s\n", path);
    // setenv(path);
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
    if (rank_cmd == -1) {
        for (int i = 0; i < n_cmd - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }
    } else {
        for (int i = 0; i < n_cmd - 1; i++) {
            if (i == rank_cmd) {
                close(pipes[i][1]);
                dup2(pipes[i][0], STDIN_FILENO);
            if (i == rank_cmd - 1) {
                close(pipes[i][0]);
                dup2(pipes[i][1], STDOUT_FILENO);
            } else {
                close(pipes[i][0]);
                close(pipes[i][1]);
            }
        }
    }
    
}
}
void connect_in_out(struct cmdline* l, int rank_cmd, int n_cmd) 
{
    // dup2(l->in, STDIN_FILENO) in -> Fils tube Fils2 tube2 Fils -> out dup2(l->out, STDOUT_FILENO)  
    if (rank_cmd == 0) {
        if (l->in != NULL) {
            int fd = open(l->in, O_RDONLY);
            if (fd == -1) {
                perror("open");
            }
            if (dup2(fd, STDIN_FILENO) == -1) {
                perror("dup2");
            }
        }
    } else if (rank_cmd == n_cmd - 1) {
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
}

void execution(struct cmdline *l) {
    if (l->seq[0] == NULL) {
        return;
    }

    int n_cmd = nb_cmd(l);
    int **pipes;


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

