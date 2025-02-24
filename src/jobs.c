#include "jobs.h"

// Global variables
jobs_t *jobs = NULL; // Liste des jobs
int current_job = -1; // Numéro du job courant

int terminated_job = 0; // Flag indique si un job a été nettoyé par la fonction terminate_job
                        // on ne veut pas ré-afficher le message quand on tappe la commande jobs

int stopped_job = 0; // Flag indique si un job a été stoppé par la fonction stop_job on ne veut
                     // pas ré-afficher le message quand le shell recoit le signal SIGTSTP

jobs_t *jobs_init() {
    jobs_t *j = malloc(sizeof(jobs_t));
    if (j == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    j->count = 0; // Initialisation du compteur de tâches à zéro.
    j->list = NULL; // La liste des tâches est initialement vide.
    return j;
}

char *get_cmd(char ***seq) {
    int total_len = 0;
    char *cmd;

    // Calculer la longueur totale de la commande
    for (int i = 0; seq[i] != NULL; i++) {
        for (int j = 0; seq[i][j] != NULL; j++) {
            total_len += strlen(seq[i][j]) + 1; // +1 pour l'espace ou le caractère nul.
        }
        if (seq[i + 1] != NULL) {
            total_len += 3; // Ajout pour " | " entre les segments de pipeline.
        }
    }
    
    // Allouer de la mémoire pour la commande
    cmd = malloc(sizeof(char) * total_len);
    if (cmd == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Construction de la chaine de commande en concaténant les segments.
    int cmd_i = 0;
    int i = 0; int j = 0; int k = 0;
    while (seq[i] != NULL) {
        j = 0;
        while (seq[i][j] != NULL) {
            k = 0;
            // Copie caractère par caractère du segment courant. 
            while (seq[i][j][k] != '\0') {
                cmd[cmd_i++] = seq[i][j][k++];
            }
            // Ajout d'un espace entre les arguments, sauf pour le dernier.
            if (seq[i][j + 1] != NULL) {
                cmd[cmd_i++] = ' ';
            }
            j++;
        }
        // Insertion de la syntaxe du pipeline " | " si un autre segment suit.
        if (seq[i + 1] != NULL) {
            cmd[cmd_i++] = ' ';
            cmd[cmd_i++] = '|';
            cmd[cmd_i++] = ' ';
        }
        i++;
    }
    cmd[total_len - 1] = '\0'; // Terminaison de la chaine par le caractère nul.

    return cmd;
}

int jobs_add(linked_list_t *pids, gid_t gpid, char ***seq) {
    // Allocation de la mémoire pour une nouvelle tâche.
    job_t *job = (job_t *)malloc(sizeof(job_t));
    if (job == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    job->pids = pids;
    job->gpid = gpid;
    job->status = RUNNING; // La tâche démarre en état d'exécution.
    job->cmd = get_cmd(seq); // Reconstruction de la commande complète.

    // Attribution d'un numéro de tâche, incrémental en fonction des tâches existantes.
    if (jobs->count != 0) {
        job->num = jobs->list->num + 1;
    } else {
        job->num = jobs->count + 1;
    }

    // Insertion de la nouvelle tâche en tête de liste.
    job->next = jobs->list;
    jobs->list = job;
    jobs->count++;
    return job->num;
}

void job_print(job_t *job) {
    if (job == NULL) {
        return;
    }
    // Affichage du numéro de la tâche.
    printf("[%d] ", job->num);

    // Affichage du statut de la tâche dans un format lisible.
    switch (job->status) {
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
    // Affichage de la commande d'origine associée à la tâche.
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
    free(job->cmd);              // Libération de la chaîne de commande
    linked_list_free(job->pids); // Libération de la liste des PID.
    free(job);                   // Libération de la structure de la tâche.
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
    free(jobs); // Libération de la structure globale.
}

void fg_job(int num) {
    if (num == -1) {
        fprintf(stderr, "fg: courant: tâche inexistante\n");
        return;
    }

    // Recherche de la tâche correspondante dans la liste.
    job_t *current = jobs->list;
    while (current) {
        if (current->num == num) {
            break;
        }
        current = current->next;
    }

    if (current == NULL) {
        fprintf(stderr, "fg: %d: tâche inexistante\n", num);
        return;
    }

    // Si la tâche est stoppée, envoi d'un signal pour la reprendre.
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

    // Affichage de la commande associée à la tâche en premier-plan.
    fprintf(stdout, "%s\n", current->cmd);
    wait_current_job();   // Attente de la fin d'exécution ou du stoppage de la tâche.
}

void bg_job(int num) {
    if (num == -1) {
        fprintf(stderr, "bg: courant: tâche inexistante\n");
        return;
    }

    // Recherche de la tâche dans la liste.
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

    // Si la tâche est stoppée, envoi d'un signal pour la relancer en arrière-plan.
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
    if (num == -1) {
        fprintf(stderr, "stop: courant: tâche inexistante\n");
        return;
    }
    
    // Recherche de la tâche à stopper.
    job_t *current = jobs->list;
    while (current) {
        if (current->num == num) {
            break;
        }
        current = current->next;
    }
    
    if (current == NULL) {
        fprintf(stderr, "stop: %d: tâche inexistante\n", num);
        return;
    }
    
    stopped_job = 1;
    if (current->status == RUNNING) {
        kill(-current->gpid, SIGTSTP); // Envoi du signal pour suspendre la tâche.
        current->status = STOPPED;
        fprintf(stdout, "[%d] Stoppé    %s\n", current->num, current->cmd);
    } else if (current->status == STOPPED) {
        fprintf(stderr, "stop: la tâche %d est déjà stoppée\n", num);
        return;
    } else {
        fprintf(stderr, "stop: la tâche est déjà terminée\n");
        return;
    }
}

void wait_current_job() {
    job_t *prev = NULL;
    job_t *current = jobs->list;

    // Recherche de la tâche dans la liste.
    while (current != NULL && current->num != current_job) {
        prev = current;
        current = current->next;
    }

    // Si la tâche n'est pas trouvée, réinitialisation du numéro de tâche en avant-plan.
    if (current == NULL) {
        current_job = -1;
        return;
    }

    // Boucle d'attente tant que la tâche est active.
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
    
    // Si la tâche est terminée, suppression de celle-ci de la liste et libération de la mémoire.
    if (current->status == TERMINATED) {
        if (prev == NULL) {
            jobs->list = current->next;
        } else {
            prev->next = current->next;
        }
        jobs->count--;
        job_free(current);
    }

    current_job = -1; // Réinitialisation du suivi de la tâche en premier-plan.
}

void terminate_job() {
    if (jobs == NULL || jobs->list == NULL) {
        return;
    }
    
    job_t **pp = &jobs->list;
    // Itération sur la liste et suppression des tâches terminées.
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
    terminated_job = 0; // Réinitialisation du flag de terminaison.
}   