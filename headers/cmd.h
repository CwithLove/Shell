#ifndef __CMD_H
#define __CMD_H

#include "csapp.h"
#include "readcmd.h"
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "linked_list.h"
#include "jobs.h"
#include <wordexp.h>

/**
 * @def MAXPATH 
 * @brief Defines the maximum allowed length for a path name.
 * 
 * This macro defines the maximum number of characters allowed in a pathname
 * on the system. It has a value of 100, which is a common maximum length
 * across different operating systems. However, it is important to note that
 * the actual maximum pathname length may vary depending on the specific
 * system configuration.
 */
#define MAXPATH 100

/**
 * @brief Counts the number of commands in a command line
 * 
 * This function takes a pointer to a `cmdline` structure and
 * returns the number of commands in the sequence.
 *  
 * @param l 
 * @return The number of commands in the sequence 
 */
int nb_cmd(struct cmdline *l);


/**
 * @brief Handle the "cd", "fg", "bg" command
 * 
 * This function checks if the given command is an internal command 
 * and executes the corresponding
 * 
 * @param cmd
 *  
 * @return 1 if the command is internal, 0 otherwise
 */
int internal(char **cmd);

/**
 * @brief Execute a parsed command line
 * 
 * This function takes a pointer to a `cmdline` structure and
 * executes the corresponding command.
 * 
 * @param l  
 */
void execution(struct cmdline *l);

#endif