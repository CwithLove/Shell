#ifndef __CMD_H
#define __CMD_H

#include "csapp.h"
#include "readcmd.h"
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "jobs.h"

#define MAXPATH 100

/**
 * @brief 
 * 
 * @param l 
 */
void execution(struct cmdline *l);

#endif