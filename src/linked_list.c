#include "linked_list.h"

linked_list_t *linked_list_init() {
    linked_list_t *list = (linked_list_t *)malloc(sizeof(linked_list_t));
    if (list == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    list->head = NULL;
    return list;
}

void linked_list_add(linked_list_t *list, pid_t pid) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (new_node == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    new_node->pid = pid;
    new_node->next = list->head;
    list->head = new_node;
}

void linked_list_remove(linked_list_t *list, pid_t pid) {
    node_t *current = list->head;
    node_t *previous = NULL;

    while (current != NULL && current->pid != pid) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        return;
    }

    if (previous == NULL) {
        list->head = current->next;
    } else {
        previous->next = current->next;
    }

    free(current);
}

int linked_list_is_empty(linked_list_t *list) {
    return list->head == NULL;
}

void linked_list_print(linked_list_t *list) {
    node_t *current = list->head;
    while (current != NULL) {
        printf("%d\n", current->pid);
        current = current->next;
    }
}

void linked_list_free(linked_list_t *list) {
    node_t *current = list->head;
    node_t *next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

pid_t get_last_pid(linked_list_t *list) {
    node_t *current = list->head;
    while (current->next != NULL) {
        current = current->next;
    }
    return current->pid;
}

void send_signal_to_all(linked_list_t *list, int signal) {
    node_t *current = list->head;
    while (current != NULL) {
        kill(current->pid, signal);
        current = current->next;
    }
}
