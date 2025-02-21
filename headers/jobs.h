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
    linked_list_t *pids; 
    
    /**
     * @brief Process group identifier
     */
    gid_t gpid;

    /**
     * @brief Status of the job (RUNNING, STOPPED, TERMINATED)
     */
    ProcessState status;

    /**
     * @brief Command of the job
     */
    char* cmd;

    /**
     * @brief Next job in the list
     */
    struct job* next;
} job_t;

typedef struct jobs
{
    /**
     * @brief Number of jobs
     */
    int count;

    /**
     * @brief List of jobs
     */
    job_t* list;
} jobs_t;

/**
 * @brief Initialize the jobs structure
 * 
 * This function initializes the jobs structure.
 * 
 * @return A pointer to the jobs structure
 */
jobs_t* jobs_init();

/**
 * @brief Get the cmd object
 * 
 * @param seq 
 * @return char* 
 */
char *get_cmd(char ***seq);

/**
 * @brief Add a job to the jobs structure
 * 
 * This function adds a job to the jobs structure.
 * 
 * @param jobs The jobs structure
 * @param gpid The process group identifier
 * @param cmd The command
 */
int jobs_add(linked_list_t *pid, gid_t gpid, char*** cmd);

/**
 * @brief print all jobs
 * 
 * @param job 
 */
void job_print(job_t* job);

/**
 * @brief print all jobs
 * 
 * @param jobs 
 */
void list_jobs_print();

/**
 * @brief Free the jobs structure
 * 
 * @param job 
 */
void job_free(job_t* job);

/**
 * @brief Free the jobs structure
 * 
 * @param jobs 
 */
void list_jobs_free();

/**
 * @brief Get a job by its number
 * 
 * @param jobs The jobs structure
 * @param num The number of the job
 * @return The job with the given number
 */
void fg_job(int num);

/**
 * @brief Start a job to the background
 * 
 * This function starts a job to the background.
 */
void bg_job(int num);

/**
 * @brief Stop a job
 * 
 * This function stops a job.
 * 
 * @param num 
 */
void stop_job(int num);

/**
 * @brief Wait for the current job
 * 
 * This function waits for the current job.
 * 
 */
void wait_current_job();

/**
 * @brief Terminate jobs in status TERMINATED
 * 
 * This function terminates a job.
 * 
 * @param num 
 */
void terminate_job();

#endif