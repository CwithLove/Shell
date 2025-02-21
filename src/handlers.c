#ifndef __HANDLERS_H
#define __HANDLERS_H

#include "handlers.h"

extern jobs_t *jobs;
extern int current_job;

void sigchild_handler(int sig) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            job_t *current = jobs->list;
            while (current != NULL) {
                linked_list_remove(current->pids, pid);
                if (linked_list_is_empty(current->pids)) {
                    fprintf(stderr, "[%d] %s done\n", current->num, current->cmd);
                    job_free(current);
                    if (current_job == current->num) {
                        current_job = -1;
                    }
                    if (current == jobs->list) {
                        jobs->list = current->next;
                    } else {
                        job_t *prec = jobs->list;
                        while (prec->next != current) {
                            prec = prec->next;
                        }
                        prec->next = current->next;
                    }
                    jobs->count--;
                    break;
                }
                current = current->next;
            }
        } else if (WIFSTOPPED(status)) {
            job_t *current = jobs->list;
            while (current != NULL) {
                linked_list_remove(current->pids, pid);
                if (linked_list_is_empty(current->pids)) {
                    job_print(current);
                    current->status = STOPPED;
                    if (current_job == current->num) {
                        current_job = -1;
                    }
                    break;
                }
                current = current->next;
            }
        }
    }
    return;
}

void sigint_sigtstp_handler(int sig) {
    if (current_job != -1) {
        job_t *current = jobs->list;
        while (current != NULL) {
            if (current->num == current_job) {
                break;
            }
            current = current->next;
        }
        if (current != NULL) {
            kill(-current->gpid, sig);
        }
    }
    return;
}


#endif