#include "linked_list.h"

linked_list_t *linked_list_init() {
    linked_list_t *list = (linked_list_t *)malloc(sizeof(linked_list_t));
    if (list == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    list->tail = NULL;
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
    new_node->cmd = NULL;
    list->head = new_node;

    if (list->tail == NULL) {
        list->tail = new_node;
    }
}

void linked_list_add_tail(linked_list_t *list, char* cmd) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (new_node == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    new_node->pid = -1;
    new_node->next = NULL;
    new_node->cmd = cmd;
    if (list->tail != NULL) {
        list->tail->next = new_node;
    }
    list->tail = new_node;
    if (list->head == NULL) {
        list->head = new_node;
    }
}

void linked_list_history(linked_list_t* list) {
    if (list == NULL) {
        return;
    }

    node_t *current = list->head;
    int history_num = 1;
    while (current != NULL) {
        printf("    %d. %s\n", history_num, current->cmd);
        current = current->next;
        history_num++;
    }
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

int linked_list_contains(linked_list_t *list, pid_t pid) {
    node_t *current = list->head;
    while (current != NULL) {
        if (current->pid == pid) {
            return 1;
        }
        current = current->next;
    }
    return 0;
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

