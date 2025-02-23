#ifndef __HANDLERS_H
#define __HANDLERS_H

#include "handlers.h"

extern jobs_t *jobs;
extern int current_job;


void sigchild_handler(int sig) {
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        if (WIFSTOPPED(status)) {
            gid_t gid = getpgid(pid);
            job_t *job = jobs->list;
            while (job != NULL) {
                if (job->gpid == gid) {
                    if (job->status != STOPPED) {
                        job->status = STOPPED;
                    }
                    job_print(job);
                    current_job = -1; 
                    break;
                }
                job = job->next;
            }
        } else if (WIFEXITED(status) || WIFSIGNALED(status)) {
            job_t *job = jobs->list;
            while (job != NULL) {
                if (linked_list_contains(job->pids, pid)) {
                    linked_list_remove(job->pids, pid);
                    if (linked_list_is_empty(job->pids)) {
                        job->status = TERMINATED;
                        if (current_job == job->num) {
                            current_job = -1;
                        }
                        break;
                    }
                }
                job = job->next;
            }
        }
    }
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
            if (sig == SIGINT) {
                if (kill(-current->gpid, SIGINT) == -1) {
                    perror("kill");
                }
            } else if (sig == SIGTSTP) {
                if (kill(-current->gpid, SIGTSTP) == -1) {
                    perror("kill");
                }
            }
        } 
    }
    write(STDOUT_FILENO, "\n", 1);
}


#endif