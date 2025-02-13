#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int nb_commande(struct cmdline *l) {
    return 1;
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

void execution(struct cmdline *l) {
    int nb_cmd = nb_commande(l);
    for (int i = 0; i < nb_cmd; i++) {
        if (internal(l->seq[i])) {
            //
        } else {
            pid_t pid;
            pid = Fork();

            if (pid == 0) {
                // Fils
                // char *envp = getenv("PATH");

                execvp(l->seq[0][0], l->seq[0]);
                exit(0);
            } else if (pid > 0) {
                pid = waitpid(pid, NULL, 0);
            } else { // Case -1
                perror("Fork");
            }
        }
    }
}

