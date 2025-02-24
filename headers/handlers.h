#ifndef HANDLERS_H
#define HANDLERS_H

#include "csapp.h"
#include "jobs.h"
#include "linked_list.h"
#include <signal.h>
#include <sys/wait.h>
/**
 * @brief Signal handler for SIGCHLD
 * 
 * @param sig 
 */
void sigchild_handler(int sig);

/**
 * @brief Signal handler for SIGINT and SIGTSTP
 * 
 * @param sig 
 */
void sigint_sigtstp_handler(int sig);

#endif