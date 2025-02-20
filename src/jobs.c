#include "jobs.h"


jobs_t *jobs_init() {
    jobs_t *jobs = malloc(sizeof(jobs_t));
    if (jobs == NULL) {
        perror("malloc");
    }
    jobs->count = 0;
    jobs->list = NULL;
    return jobs;
}

char *get_cmd(char ***seq) {
    char *cmd;

    /*not implemented*/
    return "hello";
    
}

int jobs_add(linked_list_t *pids, gid_t gpid, char ***seq) {
    job_t *job = malloc(sizeof(job_t));
    if (job == NULL) {
        perror("malloc");
        return -1;
    }
    job->gpid = gpid;
    job->cmd = get_cmd(seq);
    job->pids = pids;
    job->status = RUNNING;
    job->next = jobs->list;
    jobs->list = job;
    return 0;
}

void job_print(job_t *job) {
    if (job == NULL) {
        return;
    }
    char *status;
    switch (job->status)
    {
    case RUNNING:
        
        status = "RUNNING";
        break;
    case STOPPED:
        
        status = "STOPPED";
        break;
    case TERMINATED:

        status = "TERMINATED";
        break;
    default:
        break;
    }
    printf("[%d] %s %s\n", job->num, status, job->cmd);
}

void list_jobs_print(jobs_t *jobs) {
    if (!jobs) {
        return;
    }
    job_t *current = jobs->list;
    while (current) {
        job_print(current);
        current = current->next;
    }
}

void job_free(job_t *jobs) {
    if (jobs == NULL) {
        return;
    }
    free(jobs->cmd);
    linked_list_free(jobs->pids);
    free(jobs);
}

void list_jobs_free(jobs_t *jobs) {
    if (jobs == NULL) {
        return;
    }
    job_t *current = jobs->list;
    while (current) {
        job_t *next = current->next;
        job_free(current);
        current = next;
    }
    free(jobs);
}

void fg_job(int num) {
    if (jobs == NULL) {
        // fprintf(stderr, "no such jobs\n"); // A VERIFIER
        perror("fg");
        return;
    }

    job_t *current = jobs->list;
    while (current) {
        if (current->num == num) {
            break;
        }
        current = current->next;
    }

    if (current == NULL) {
        // fprintf(stderr, "no such jobs\n"); // A VERIFIER
        perror("fg");
        return;
    } else {
        current_job = current->num;
        kill(-jobs->list->gpid, SIGCONT);
    }
}

void bg_job(int num) {
    if (jobs == NULL) {
        perror("bg");
        return;
    } 

    job_t *current = jobs->list;
    while (current) {
        if (current->num == num) {
            break;
        }
        current = current->next;
    }
    
    if (current == NULL) {
        perror("bg");
        return;
    }
    kill(-jobs->list->gpid, SIGCONT);
}

void stop_job(int num) {
    if (jobs == NULL) {
        perror("stop");
        return;
    }

    job_t *current = jobs->list;
    while (current) {
        if (current->num == num) {
            break;
        }
        current = current->next;
    }

    if (current == NULL) {
        perror("stop");
        return;
    }
    kill(-jobs->list->gpid, SIGTSTP);
}

void wait_current_job() {
    while (current_job != -1) {
        sleep(1);
    }
}

void terminate_job() {
    if (jobs == NULL) {
        return;
    }

    job_t *prec = NULL;
    job_t *current = jobs->list;
    if (current == NULL) {
        return;
    } else {
        while (current) {
            if (current->status == TERMINATED) {
                if (prec == NULL) {
                    jobs->list = current->next;
                } else {
                    prec->next = current->next;
                }
                job_free(current);
            }
            current = current->next;
        }
    }
}