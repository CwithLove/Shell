#ifndef __JOBS_H
#define __JOBS_H

#include "csapp.h"
#include "linked_list.h"

/**
 * @def MAXJOBS
 * 
 * @brief Maximum number of jobs
 */
#define MAXJOBS 10

#define DEFINE_PROCESS_STATES \
    typedef enum {         \
        TERMINATED,       \
        STOPPED,          \
        RUNNING           \
    } ProcessState;

DEFINE_PROCESS_STATES

typedef struct job
{
    /**
     * @brief Unique identifier of the job
     */
    int num;

    /**
     * @brief Process group identifier
     */
    linked_list_t pids; 
    
    /**
     * @brief Process group identifier
     */
    gid_t gpid;

    /**
     * @brief State of the job (RUNNING, STOPPED, TERMINATED)
     */
    ProcessState state;

    char*** cmd;
} job_t;

typedef job_t* jobs_t;


#endif