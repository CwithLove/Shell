#include "jobs.h"

jobs_t *jobs;
int current_job;

jobs_t *jobs_init() {
    jobs_t *j = malloc(sizeof(jobs_t));
    if (j == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    j->count = 0;
    j->list = NULL;
    return j;
}

char *get_cmd(char ***seq) {
    int cmd_len = 0;
    for (int i = 0; seq[i] != NULL; i++) {
        for (int j = 0; seq[i][j] != NULL; j++) {
            cmd_len += strlen(seq[i][j]) + 1;
        }
    }
    char *cmd = malloc(sizeof(char) * cmd_len);
    if (cmd == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int cmd_i = 0;
    int i = 0; int j = 0; int k = 0;
    while (seq[i] != NULL) {
        j = 0;
        while (seq[i][j] != NULL) {
            k = 0;
            while (seq[i][j][k] != '\0') {
                cmd[cmd_i++] = seq[i][j][k++];
            }
            if (seq[i][j + 1] != NULL) {
                cmd[cmd_i++] = ' ';
            }
            j++;
        }
        if (seq[i + 1] != NULL) {
            cmd[cmd_i++] = ' ';
            cmd[cmd_i++] = '|';
            cmd[cmd_i++] = ' ';
        }
        i++;
    }
    cmd[cmd_len - 1] = '\0';

    return cmd;
}

int jobs_add(linked_list_t *pids, gid_t gpid, char ***seq) {
    job_t *job = (job_t *)malloc(sizeof(job_t));
    if (job == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    job->pids = pids;
    job->gpid = gpid;
    job->status = RUNNING;
    job->cmd = get_cmd(seq);
    /* A COMPLETER 
    CONDITION LE JOB > 10
    */
    job->num = jobs->count + 1;
    job->next = jobs->list;

    jobs->list = job;
    jobs->count++;
    return job->num;
}

void job_print(job_t *job) {
    if (job == NULL) {
        return;
    }
    printf("[%d] ", job->num);
    switch (job->status)
    {
    case RUNNING:
        printf("Running");
        break;
    case STOPPED:
        printf("Suspended");
        break;
    case TERMINATED:
        printf("Done");
        break;
    }
    printf("    %s\n", job->cmd);
}

void list_jobs_print() {
    if (!jobs) {
        return;
    }
    job_t *current = jobs->list;
    while (current) {
        job_print(current);
        current = current->next;
    }
}

void job_free(job_t *job) {
    if (job == NULL) {
        return;
    }
    free(job->cmd);
    linked_list_free(job->pids);
    free(job);
}

void list_jobs_free() {
    if (jobs == NULL) {
        return;
    }
    job_t *current = jobs->list;
    while (current) {
        job_t *next = current->next;
        job_free(current);
        current = next;
        jobs->count--;
    }
    free(jobs);
}

void fg_job(int num) {
    // if (jobs == NULL) {
    //     // fprintf(stderr, "no such jobs\n"); // A VERIFIER
    //     perror("fg");
    //     return;
    // }

    // job_t *current = jobs->list;
    // while (current) {
    //     if (current->num == num) {
    //         break;
    //     }
    //     current = current->next;
    // }

    // if (current == NULL) {
    //     // fprintf(stderr, "no such jobs\n"); // A VERIFIER
    //     perror("fg");
    //     return;
    // } else {
    //     current_job = current->num;
    //     kill(-jobs->list->gpid, SIGCONT);
    // }
}

void bg_job(int num) {
    // if (jobs == NULL) {
    //     perror("bg");
    //     return;
    // } 

    // job_t *current = jobs->list;
    // while (current) {
    //     if (current->num == num) {
    //         break;
    //     }
    //     current = current->next;
    // }
    
    // if (current == NULL) {
    //     perror("bg");
    //     return;
    // }
    // kill(-jobs->list->gpid, SIGCONT);
}

void stop_job(int num) {    
    job_t *current = jobs->list;
    while (current) {
        if (current->num == num) {
            break;
        }
        current = current->next;
    }

    if (num >= MAXJOBS || jobs <= 0 || current == NULL) {
        fprintf(stderr, "Error: invalid job number\n");
    } else if (current->status == RUNNING) {
        kill(-current->gpid, SIGTSTP);
        current->status = STOPPED;
    }
}

void wait_current_job() {
    job_t *prec;
    job_t *current;
    if (current_job != -1) {
        prec = NULL;
        current = jobs->list;
        while (current) {
            if (current->num == current_job) {
                break;
            }
            prec = current;
            current = current->next;
        }    
    }

    while (current_job != -1 && (current->status != TERMINATED)) {
        sleep(1);
    }

    if (current_job != -1) {
        if (prec == NULL) {
            jobs->list = current->next;
        } else {
            prec->next = current->next;
        }
        job_free(current);
    }

    current_job = -1;
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