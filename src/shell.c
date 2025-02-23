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

extern jobs_t *jobs;

int main()
{
	jobs = jobs_init();
	Signal(SIGCHLD, sigchild_handler);
	Signal(SIGINT, sigint_sigtstp_handler);
	Signal(SIGTSTP, sigint_sigtstp_handler);
	while (1) {
		list_jobs_print();
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
	
		terminate_job();

		struct cmdline *l;
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

		execution(l);
		wait_current_job();
	}
}
