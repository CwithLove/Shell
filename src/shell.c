/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include "readcmd.h"
#include "csapp.h"
#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include "handlers.h"
#include "interface.h"

extern int refresh_prompt;

int main()
{
	Signal(SIGCHLD, sigchild_handler);
	Signal(SIGINT, sigint_sigtstp_handler);
	Signal(SIGTSTP, sigint_sigtstp_handler);
	while (1) {
		struct cmdline *l;
		
		prompt();
		fflush(stdout);
		// int i, j;
	
		l = readcmd();

		/* If input stream closed, normal termination */
		if (!l) {
			printf("exit\n");
			exit(0);
		}

		if (l->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in) printf("in: %s\n", l->in);
		if (l->out) printf("out: %s\n", l->out);

		/* Display each command of the pipe */
		// for (i=0; l->seq[i]!=0; i++) {
		// 	char **cmd = l->seq[i];
		// 	printf("seq[%d]: ", i);
		// 	for (j=0; cmd[j]!=0; j++) {
		// 		printf("%s ", cmd[j]);
		// 	}
		// 	printf("\n");
		// }

		// fprintf(stderr, "Execution cmd...\n");
		execution(l);
	}
}
