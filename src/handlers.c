#ifndef __HANDLERS_H
#define __HANDLERS_H

#include "csapp.h"
#include <signal.h>
#include <sys/wait.h>
#include "jobs.h"

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
    if (jobs == NULL || jobs->list == NULL) {
        return;
    }

    job_t *current = jobs->list;
    while (current != NULL) {
        if (current->num == current_job) {
            break;
        }
        current = current->next;
    }    
    kill(-current->gpid, sig);
}


#endif