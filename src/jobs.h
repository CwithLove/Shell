#ifndef JOBS_H
#define JOBS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define MAXJOBS 10

#define DEFINE_PROCESS_STATES \
    typedef enum {         \
        TERMINATED,       \
        STOPPED,          \
        RUNNING           \
    } ProcessState;

DEFINE_PROCESS_STATES

typedef struct jobs
{
    int num;
    pid_t pid[4]; // changer en dynamique
    ProcessState state;
    char*** cmd;
    //struct jobs *next;
}*jobs_t;

jobs_t jobs_init();
void jobs_add(jobs_t jobs, int pid);
void jobs_remove(jobs_t jobs, int pid);
void jobs_free(jobs_t jobs);
void jobs_print(jobs_t jobs);
void jobs_set_gpid(jobs_t jobs, int gpid);

#endif