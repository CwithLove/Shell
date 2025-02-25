#include "cmd.h"

extern jobs_t *jobs;    // Référence globale à la structure de gestion des tâches.
extern int current_job; // Identifiant de la tâche en premier-plan.
linked_list_t *history; // Liste chaînée pour stocker l'historique des commandes.

int nb_cmd(struct cmdline *l) {
    int i = 0;
    while (l->seq[i] != NULL) {
        i++;
    }
    return i;
}

int cd(char **cmd) {
    // Allocation d'un buffer pour le chemin avec une taille maximale prédéfinie.
    char *path = malloc(sizeof(char) * MAXPATH + 1);
    if (path == NULL) {
        perror("malloc");
    }

    // Cas sans argument, on se positionne dans le répertoire HOME.
    if (cmd[1] == NULL) {
        char *tem = getenv("HOME");
        if (tem != NULL) {
            strncpy(path, tem, MAXPATH);
        }
    }
    // Cas où le chemin commence par '~' indiquant le répertoire HOME.
    else if (cmd[1][0] == '~') {
        if (cmd[1][1] == '/' || cmd[1][1] == '\0') {
            char *tem = getenv("HOME");
            if (tem != NULL) {
                snprintf(path, MAXPATH, "%s%s", tem, cmd[1] + 1);
            }
        } else {
            perror("cd");
        }
    } 
    // Cas général : utilisation directe du chemin fourni.
    else {
        strncpy(path, cmd[1], MAXPATH);
    }
    // Changement de répertoire et vérification de la réussite de l'opération.
    if (chdir(path) != 0) {
        perror("cd");
        return 1;
    }

    // Mise à jour de la variable d'environnement PWD avec le nouveau répertoire.
    char cwd[MAXPATH];
    getcwd(cwd, MAXPATH);
    setenv("PWD", cwd, 1);
    free(path);
    return 1;
}

int export(char **cmd) {
    if (cmd[1] == NULL) {
        fprintf(stderr, "Erreur: export VAR=value\n");
        return 1;
    }

    char *arg = cmd[1];
    char *eq_ptr = strchr(arg, '=');

    // Si le signe '=' n'est pas trouvé, on vérifie la présence de la variable et
    // on la crée avec une valeur vide si nécessaire.
    if (eq_ptr == NULL) {
        char *current_val = getenv(arg);
        if (current_val == NULL) {
            if (setenv(arg, "", 1) < 0) {
                perror("setenv");
                return 1;
            }
        } else {
            if (setenv(arg, current_val, 1) < 0) {
                perror("setenv");
                return 1;
            }
        }
    // Si '=' est présent, séparer le nom et la valeur et mettre à jour l'environnement.
    } else {
        *eq_ptr = '\0';    
        char *var_name = arg;
        char *var_value = eq_ptr + 1;
        if (setenv(var_name, var_value, 1) < 0) {
            perror("setenv");
            return 1;
        }
    }

    return 1;
}

int internal(char **cmd) {
    // Ignorer les commentaires.
    if (cmd[0][0] == '#') {
        return 1;
    }

    // Commande d'arrêt du shell
    if (!strcmp(cmd[0], "exit") || !strcmp(cmd[0], "quit")) {
        list_jobs_free(jobs); // Liberer pour eviter la fuite de memoire
        linked_list_free(history); // Liberer pour eviter la fuite de memoire
        int status;
        if (cmd[1] != NULL) {
            status = atoi(cmd[1]);
        } else {
            status = 0;
        }
        exit(status);
    } 
    // Changement de répertoire.
    else if (!strcmp(cmd[0], "cd")) { 
        return cd(cmd);
    } 
    // Exportation d'une variable d'environnement.
    else if (!strcmp(cmd[0], "export")) {
        return export(cmd);
    } 
    // Affichage de la liste des tâches.
    else if (!strcmp(cmd[0], "jobs")) {
        list_jobs_print(jobs);
        return 1;
    } 
    // Affichage de l'historique des commandes.
    else if (!strcmp(cmd[0], "history")) {
        if (cmd[1] != NULL && !strcmp(cmd[1], "-c")) {
            linked_list_free(history);
            history = linked_list_init();
            return 1;
        } else if (cmd[1] != NULL) {
            fprintf(stderr, "Erreur: Option -c implemented only\n");
            return 1;
        } else {
            linked_list_history(history);
            return 1;
        }
    }
    // Commandes de gestion des tâches (foreground, background, stop).
    else if (!strcmp(cmd[0], "fg") || !strcmp(cmd[0], "bg") || !strcmp(cmd[0], "stop")) {
        int num; 

        // Détermination du numéro de la tâche à partir des arguments.
        if (cmd[1] == NULL) {
            if (jobs->count != 0) {
                num = jobs->list->num;
            } else {
                num = -1;
            }
        } else {
            if (cmd[1][0] == '%') {
                num = atoi(cmd[1] + 1);
            } else {
                num = atoi(cmd[1]);
            }
        }

        // Exécution de la commande en fonction du premier caractère.
        switch (cmd[0][0])
        {
        case 'f': // fg
            fg_job(num);
            return 1;
        
        case 'b': // bg
            bg_job(num);
            return 1;

        case 's': // stop
            stop_job(num);
            return 1;
        }
    }
    
    // Retourne 0 si la commande n'est pas interne.
    return 0;
}

int **create_pipes(int n_cmd) {
    int **pipes = (int **)malloc(sizeof(int *) * (n_cmd - 1));
    if (pipes == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Création d'un pipe pour chaque paire de commandes consécutives.
    for (int i = 0; i < n_cmd - 1; i++) {
        pipes[i] = (int *)malloc(sizeof(int) * 2);
        if (pipes[i] == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }

        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }
    return pipes;
}

void connect_pipes(int **pipes, int rang_cmd, int n_cmd) {
    for (int i = 0; i < n_cmd - 1; i++) {
        if (i == rang_cmd) {
            // Redirige la sortie de la commande courante vers l'écriture du pipe
            close(pipes[i][0]);
            if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                perror("dup2 in connect");
                exit(EXIT_FAILURE);
            }
            close(pipes[i][1]);
        }

        else if (i == rang_cmd - 1) {
            // Redirige l'entrée de la commande courante depuis la lecture du pipe.
            close(pipes[i][1]);
            if (dup2(pipes[i][0], STDIN_FILENO) == -1) {
                perror("dup2 out pipe");
                exit(EXIT_FAILURE);
            }
            close(pipes[i][0]);
        } else {
            // Ferme les descripteurs inutilisés.
            close(pipes[i][0]);
            close(pipes[i][1]);
        }  
    } 
}

void free_pipes(int **pipes, int n_cmd) {
    for (int i = 0; i < n_cmd - 1; i++)
    {
        free(pipes[i]);
    }
    free(pipes);
}

void connect_in_out(struct cmdline* l, int rang_cmd, int n_cmd) 
{
    int fd;  
    // Gestion de la redirection d'entrée pour la première commande.
    if (rang_cmd == 0 && l->in != NULL) {
        fd = open(l->in, O_RDONLY);
        if (fd == -1) {
            perror(l->in);
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDIN_FILENO) == -1) {
            perror("dup2 in");
            exit(EXIT_FAILURE);
        }
        close(fd);
    } 
    // Gestion de la redirection de sortie pour la dernière commande.
    if (rang_cmd == n_cmd - 1 && l->out != NULL) {
        fd = open(l->out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            perror(l->out);
        }

        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2 out");
            exit(EXIT_FAILURE);
        }
        close(fd);
    }
}

char *get_each_cmd(char **seq) {
    char *cmd;
    int total_len = 0;
    int i = 0;
    // Calcul de la longueur totale requise pour la commande.
    while (seq[i] != NULL) {
        total_len += strlen(seq[i]) + 1;
        i++;
    }
    cmd = malloc(sizeof(char) * total_len);
    if (cmd == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    cmd[0] = '\0';
    // Concaténation de chaque segment de commande avec un espace entre eux.
    for (int j = 0; seq[j] != NULL; j++) {
        strcat(cmd, seq[j]);
        if (seq[j + 1] != NULL) {
            strcat(cmd, " ");
        }
    }
    return cmd;
}

void execution(struct cmdline *l) {
    if (l->seq[0] == NULL) {
        return;
    }

    // Ajout de la commande à l'historique.
    char *history_cmd = get_cmd(l->seq);
    linked_list_add_tail(history, history_cmd);
    
    int n_cmd = nb_cmd(l); // Nombre total de commandes dans la séquence.
    linked_list_t *pids = linked_list_init(); // Initialisation de la liste des PID.
    int **pipes;
    pid_t pid;
    gid_t gpid = -1;
    
    // Création des pipes s'il y a au moins une commande.
    if (n_cmd > 0) {
        pipes = create_pipes(n_cmd);
    }
    
    // Compteur pour les commandes internes.
    int n_internal = 0;
    // Boucle de traitement pour chaque commande de la séquence.
    for (int cmd = 0; cmd < n_cmd; cmd++) {
        // Si la commande est interne, on l'exécute immédiatement.
        if (internal(l->seq[cmd])) {
            n_internal++;
            continue;
        } 
        
        // Vérifie si le nombre maximal de jobs est atteint.
        if (jobs->count >= MAXJOBS) {
            errno = EAGAIN; // Erreur -> maximale de jobs possible
            // fprintf(stderr, "Erreur : nombre maximal de jobs atteint\n");
            perror("Erreur");
            return;
        }

        // Construction d'une chaîne de commande pour expansion via wordexp.
        char *cmd_str = get_each_cmd(l->seq[cmd]);
        wordexp_t p;
        int ret;

        ret = wordexp(cmd_str, &p, 0);
        if (ret != 0) {
            perror("wordexp");
            return;
        }

        pid = fork();
        if (pid == -1) {
            perror("Fork");

        } else if (pid > 0) { // Parent
            
            if (gpid == -1) {
                gpid = pid;
            }
            setpgid(pid, gpid);
            linked_list_add(pids, pid);

        } else { // Fils
            connect_in_out(l,cmd,n_cmd); // Configure les redirections d'entrée/sortie.
            connect_pipes(pipes, cmd,n_cmd); // Connecte les pipes pour cette commande.
            free_pipes(pipes, n_cmd); // Libère les ressources allouées pour les pipes.
            
            // Exécute la commande en se basant sur l'expansion effectuée par wordexp.
            if (execvp(p.we_wordv[0], p.we_wordv) == -1) {
                if (l->bg) {
                    l->bg = 0;
                }
                fprintf(stderr, "%s : Command not found\n", l->seq[cmd][0]);
                exit(3);
            }
        }
        wordfree(&p); // Libère les ressources allouées par wordexp.
    }
    
    // Ferme les pipes non utilisés après la boucle de création des processus.
    connect_pipes(pipes, -1, n_cmd);
    free_pipes(pipes, n_cmd);

    // Si toutes les commandes étaient internes, on ne crée pas de job.
    if (n_internal == n_cmd) {
        return;
    }

    // Création du job en ajoutant la liste des PID et le groupe de processus associé.
    int job_num = jobs_add(pids, gpid, l->seq);
    // Si la commande n'est pas en arrière-plan, on affecte le job en premier-plan.
    if (!l->bg) {
        current_job = job_num;
    } else {
        // Affiche le numéro du job et le dernier PID pour les commandes en background.
        printf("[%d] %d\n", job_num, pids->head->pid);
    }
}
