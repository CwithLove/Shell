#ifndef __HANDLERS_H
#define __HANDLERS_H

#include "csapp.h"
#include <signal.h>
#include <sys/wait.h>

void sigchild_handler(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            fprintf(stderr, "Process %d exited with status %d\n", pid, WEXITSTATUS(status));
        } else {
            fprintf(stderr, "Process %d exited abnormally\n", pid);
        }
    }
}

int setup_handlers() {
    Signal(SIGCHLD, sigchild_handler);
    return 0;
}

#endif