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

/**
 *  @brief Get the pid of the last node in the linked list
 * 
 *  This function gets the pid of the last node in the linked list.
 * 
 *  @param list The linked list
 *  @return The pid of the last node in the linked list
 */
pid_t get_last_pid(linked_list_t *list);

/**
 *  @brief Send a signal to all nodes in the linked list
 * 
 *  This function sends a signal to all nodes in the linked list.
 * 
 *  @param list The linked list
 *  @param signal The signal to send
 */
void send_signal_to_all(linked_list_t *list, int signal);


#endif