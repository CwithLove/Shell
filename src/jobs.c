#include "jobs.h"

jobs_t *jobs = NULL;
int current_job = -1;
int terminated_job = 0;

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
    int total_len = 0;
    char *cmd;

    for (int i = 0; seq[i] != NULL; i++) {
        for (int j = 0; seq[i][j] != NULL; j++) {
            total_len += strlen(seq[i][j]) + 1; // +1 for space or null terminator
        }
        if (seq[i + 1] != NULL) {
            total_len += 3; // space + '|' + space
        }
    }
    
    cmd = malloc(sizeof(char) * total_len);
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
    cmd[total_len - 1] = '\0';

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

    if (jobs->count != 0) {
        job->num = jobs->list->num + 1;
    } else {
        job->num = jobs->count + 1;
    }
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
        printf("En cours d'exécution");
        break;
    case STOPPED:
        printf("Stoppé");
        break;
    case TERMINATED:
        printf("Fini");
        break;
    }
    printf("    %s\n", job->cmd);
}

void list_jobs_print() {
    if (!jobs || jobs->count == 0 || jobs->list == NULL) {
        return;
    }
    job_t *current = jobs->list;
    while (current) {
        if (current->status == TERMINATED) {
            terminated_job = 1;
        }
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
    job_t *current = jobs->list;
    while (current) {
        if (current->num == num) {
            break;
        }
        current = current->next;
    }

    if (current == NULL) {
        fprintf(stderr, "fg: tâche inexistante\n");
        return;
    }

    if (current->status == STOPPED) {
        if (kill(-current->gpid, SIGCONT) == -1) {
            perror("kill");
        } else {
            current->status = RUNNING;
        }
        current_job = current->num;
    } else if (current->status == RUNNING) {
        current_job = current->num;
    }
    fprintf(stdout, "%s\n", current->cmd);
    wait_current_job();
}

void bg_job(int num) {
    if (jobs == NULL || jobs->count == 0) {
        fprintf(stderr, "bg: current: tâche inexistante\n");
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
        fprintf(stderr, "bg: %d: tâche inexistante\n", num);
        return;
    }

    if (current->status == STOPPED) {
        if (kill(-current->gpid, SIGCONT) == -1) {
            perror("kill");
        } else {
            fprintf(stdout, "[%d] %s\n", current->num, current->cmd);
            current->status = RUNNING;
        }
    } else if (current->status == RUNNING) {
        fprintf(stderr, "bg: la tâche %d est déjà en arrière plan\n", num);
        return;
    } else {
        fprintf(stderr, "bg: la tâche est déjà terminée\n");
        return;
    }
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
        fprintf(stderr, "Erreur: numéro de job invalide\n");
    } else if (current->status == RUNNING) {
        kill(-current->gpid, SIGTSTP);
        current->status = STOPPED;
    }
}

void wait_current_job() {
    job_t *prev = NULL;
    job_t *current = jobs->list;

    // Find the job in the list
    while (current != NULL && current->num != current_job) {
        prev = current;
        current = current->next;
    }

    // if the job is not found
    if (current == NULL) {
        current_job = -1;
        return;
    }

    // wait until the job is terminated
    while (current_job != -1) {
        if (current->status == TERMINATED) {
            break;
        }
        if (current->status == STOPPED) {
            current_job = -1;
            return;
        }
        sleep(1);
    }
    
    if (current->status == TERMINATED) {
        if (prev == NULL) {
            jobs->list = current->next;
        } else {
            prev->next = current->next;
        }
        jobs->count--;
        job_free(current);
    }

    current_job = -1;
}

void terminate_job() {
    if (jobs == NULL || jobs->list == NULL) {
        return;
    }
    
    job_t **pp = &jobs->list;
    while (*pp != NULL) {
        job_t *current = *pp;
        if (current->status == TERMINATED) {
            if (!terminated_job) {
                job_print(current);
            }
            *pp = current->next;
            job_free(current);
            jobs->count--;
        } else {
            pp = &((*pp)->next);
        }
    }
    terminated_job = 0;
}   