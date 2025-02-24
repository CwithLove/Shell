/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include "readcmd.h"
#include "csapp.h"
#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include "handlers.h"
#include "jobs.h"

extern jobs_t *jobs; // Référence globale à la structure de gestion des tâches.

int main() {
	// Initialisation de la structure globale de gestion des tâches.
	jobs = jobs_init();
	
	// Setup handlers
	Signal(SIGCHLD, sigchild_handler);
	Signal(SIGINT, sigint_sigtstp_handler);
	Signal(SIGTSTP, sigint_sigtstp_handler);

	struct cmdline *l;
	
	while (1) {
		// Libération des tâches terminées avant chaque nouvelle commande.
		terminate_job();

		// Vous pouvez changer le nom d'utilisateur ici pour correspondre à votre profil.
		char *username = "Vania@Marangozova";
		char *home = getenv("HOME");
		char cwd[MAXPATH]; 
		
		getcwd(cwd, MAXPATH);
		if (home != NULL && strstr(cwd, home) == cwd) {
			printf("\033[0;32m%s\033[0m:[\033[0;34m~%s\033[0m]: ", username, cwd + strlen(home));
		} else {
			printf( "\033[0;32m%s\033[0m:[\033[0;34m%s\033[0m]: ", username, cwd);
		}
		fflush(stdout);
	
		l = readcmd();

		/* If input stream closed, normal termination */
		if (!l) {
			printf("bye bye\n");
			exit(0);
		}

		if (l->err) {
			/* Syntax error, read another command */
			printf("erreur: %s\n", l->err);
			continue;
		}

		execution(l);
		// Attente de la fin d'exécution de la tâche en avant-plan avant de reprendre l'invite.
		wait_current_job();
	}
}
