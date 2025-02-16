
#include "interface.h"

void prompt() {
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
}