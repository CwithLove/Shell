#ifndef __HANDLERS_H
#define __HANDLERS_H

#include "handlers.h"

extern jobs_t *jobs;
extern int current_job;
void sigchild_handler(int sig) {
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        // job_t *prev = NULL;
        job_t *current = jobs->list;

        while (current != NULL) {
            // Remove the finished process from job's process list
            linked_list_remove(current->pids, pid);

            if (WIFEXITED(status) || WIFSIGNALED(status)) { // Case 1: Process terminated normally or by a signal
                if (linked_list_is_empty(current->pids)) {
                    current->status = TERMINATED;
                    break; // Exit loop as the job has been freed
                }
            } else if (WIFSTOPPED(status)) { // Case 2: Process was stopped
                // gid_t gid = getpgid(pid);
                // job_t *job = jobs->list;

                // while (job) {
                //     if (job->gpid == gid) {
                //         job->status = STOPPED;
                //         printf("\n");
                //         job_print(job);
                //         current_job = -1; // Reset current foreground job
                //     }
                //     job = job->next;
                // }
                current->status = STOPPED;
                job_print(current);
                current_job = -1; // Reset current foreground job
                return;
            }
            // prev = current;
            current = current->next;
        }

        // fprintf(stderr, "Process %d terminated\n", pid);
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