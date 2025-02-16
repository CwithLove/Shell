#ifndef HANDLERS_H
#define HANDLERS_H

#include "csapp.h"
#include "interface.h"

void sigchild_handler(int sig);

void sigint_sigtstp_handler(int sig);

#endif