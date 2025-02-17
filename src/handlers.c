#ifndef __HANDLERS_H
#define __HANDLERS_H

#include "csapp.h"
#include <signal.h>
#include <sys/wait.h>

gid_t gpid = -1;

void sigchild_handler(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        if (WIFEXITED(status)) {
            fprintf(stderr, "Process %d exited with status %d\n", pid, WEXITSTATUS(status));
        } else {
            fprintf(stderr, "Process %d exited abnormally\n", pid);
        }
    }
}

void sigint_sigtstp_handler(int sig) {
    if (gpid != -1) {
        kill(-gpid, sig);
        gpid = -1;
    } else {
        write(STDOUT_FILENO, "\n", 1);
    }
}


#endif