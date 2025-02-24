#ifndef __HANDLERS_H
#define __HANDLERS_H

#include "handlers.h"

// Déclaration des variables globales utilisées pour la gestion des tâches.
extern jobs_t *jobs;    // Pointeur vers la structure globale contenant la liste des tâches.
extern int current_job; // Identifiant de la tâche actuellement en premier-plan.
extern int stopped_job; // Flag indiquant si une tâche a été stoppée par la fonction stop_fg.


void sigchild_handler(int sig) {
    pid_t pid;
    int status;
    // Boucle pour traiter tous les processus zombis.
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        // Si le processus a été arrêté (par exemple via SIGTSTP ou SIGSTOP).
        if (WIFSTOPPED(status)) {
            gid_t gid = getpgid(pid); // Récupère l'identifiant du groupe de processus.
            job_t *job = jobs->list;
            // Parcours de la liste des tâches pour trouver celle correspondant au groupe.
            while (job != NULL) {
                if (job->gpid == gid) {
                     // Mise à jour de l'état de la tâche si ce n'est pas déjà stoppé.
                    if (job->status != STOPPED) {
                        job->status = STOPPED;
                    }
                    // Affichage conditionnel de la tâche stoppée selon le flag.
                    if (!stopped_job) {
                        job_print(job);
                    } else {
                        stopped_job = 0; // Réinitialisation du flag.
                    }
                    current_job = -1; // Réinitialisation de la tâche en premier-plan.
                    return; // Sortie de la fonction.
                }
                job = job->next;
            }
        } 
         // Si le processus s'est terminé normalement ou a été interrompu par un signal.
        else if (WIFEXITED(status) || WIFSIGNALED(status)) {
            job_t *job = jobs->list;
            // Parcours de la liste pour identifier la tâche associée au PID concerné.
            while (job != NULL) {
                // Vérifie si le PID fait partie de la liste des processus de la tâche.
                if (linked_list_contains(job->pids, pid)) {
                    // Retire le PID de la liste des processus de la tâche.
                    linked_list_remove(job->pids, pid);
                     // Si aucun processus ne reste actif dans la tâche, marque la tâche comme terminée.
                    if (linked_list_is_empty(job->pids)) {
                        job->status = TERMINATED;
                        // Réinitialisation de la tâche en premier-plan si elle correspond à la tâche terminée.
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

    // Gestion des erreurs de waitpid, hormis le cas normal d'absence de processus enfants. (EINTR eg)
    if (pid == -1 && errno != ECHILD) {
        unix_error("waitpid");
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
        // Si la tâche est trouvée, envoie le signal approprié au groupe de processus.
        if (current != NULL) {
            if (sig == SIGINT) {
                // Envoie de SIGINT pour interrompre la tâche.
                if (kill(-current->gpid, SIGINT) == -1) {
                    perror("kill");
                }
            } else if (sig == SIGTSTP) {
                // Envoie de SIGTSTP pour suspendre la tâche.
                if (kill(-current->gpid, SIGTSTP) == -1) {
                    perror("kill");
                }
            }
        } 
    }
    // Ajoute une nouvelle ligne à la sortie standard pour un affichage propre.
    write(STDOUT_FILENO, "\n", 1);
}


#endif