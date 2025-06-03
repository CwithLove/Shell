#ifndef __LINKED_LIST_H
#define __LINKED_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include "csapp.h"


/**
 *  @typedef node_t
 *  @brief Structure representing a node in a linked list
 * 
 *  This structure represents a node in a linked list. 
 *  It contains a pid_t value and a pointer to the next node.
 */
typedef struct node {
    char *cmd;
    pid_t pid;
    struct node *next;
} node_t;

/**
 *  @brief Structure representing a linked list
 * 
 *  This structure represents a linked list.
 */
typedef struct linked_list {
    node_t *head;
    node_t *tail;
} linked_list_t;

/**
 *  @brief Initialize a linked list
 * 
 *  This function initializes a linked list.
 * 
 *  @return A pointer to the linked list
 */
linked_list_t *linked_list_init();

/**
 *  @brief Add a node to the linked list
 * 
 *  This function adds a node to the linked list.
 * 
 *  @param list The linked list
 *  @param pid The pid to add
 */
void linked_list_add(linked_list_t *list, pid_t pid);

/**
 * @brief Add a node to the tail of the linked list
 * 
 * This function adds a node to the tail of the linked list.
 * 
 * @param list
 * @param cmd
 */
void linked_list_add_tail(linked_list_t *list, char* cmd);


/**
 * @brief Print the history of the linked list
 * 
 * This function prints the history.
 * 
 * @param list
 */
void linked_list_history(linked_list_t* list);

/**
 *  @brief Remove a node from the linked list
 * 
 *  This function removes a node from the linked list.
 * 
 *  @param list The linked list
 *  @param pid The pid to remove
 */
void linked_list_remove(linked_list_t *list, pid_t pid);

/**
 *  @brief Check if the linked list is empty
 * 
 *  This function checks if the linked list is empty.
 * 
 *  @param list The linked list
 *  @return 1 if the linked list is empty, 0 otherwise
 */
int linked_list_is_empty(linked_list_t *list);

/**
 * @brief Check if the linked list contains a pid
 * 
 * @param list 
 * @param pid 
 * @return  return 1 if the linked list contains the pid, 0 otherwise
 */
int linked_list_contains(linked_list_t *list, pid_t pid);

/**
 *  @brief Print the linked list
 * 
 *  This function prints the linked list.
 * 
 *  @param list The linked list
 */
void linked_list_print(linked_list_t *list);

/**
 *  @brief Free the linked list
 * 
 *  This function frees the linked list.
 * 
 *  @param list The linked list
 */
void linked_list_free(linked_list_t *list);

#endif