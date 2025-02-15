/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include "readcmd.h"
#include "csapp.h"
#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include "handlers.h"

int main()
{
	while (1) {
		setup_handlers();
		struct cmdline *l;

		// int i, j;
		
		/* Change username if you want */
		char *username = "Vania@Marangozova";


		char *home = getenv("HOME");
		char cwd[MAXPATH]; 
		getcwd(cwd, MAXPATH);
		if (home != NULL && strstr(cwd, home) == cwd) {
			printf("\033[0;32m%s\033[0m:[\033[0;34m~%s\033[0m]: ", username, cwd + strlen(home));
		} else {
			printf( "\033[0;32m%s\033[0m:[\033[0;34m%s\033[0m]: ", username, cwd);
		}
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
