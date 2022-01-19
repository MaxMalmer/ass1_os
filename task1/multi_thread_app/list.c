#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "list.h"

/* list.c
 *  Max Malmer, c17mmr@cs.umu.se
 *  Laboration 1 5DV088 HT19
 *  Version 1.0, 2019-09-12
 *  A implementation of the datatype list as double linked list.
 *  Use list_free() to clear the memory after use. Give the input 1
 *  to the list to take control of the memory cleaning. Give it 0
 *  otherwise.
 */

// ===========INTERNAL DATA TYPES============

struct node {
    void *value;
    struct node *next;
    struct node *previous;
};

struct list {
    unsigned int is_struct;
    unsigned int num_elements;
    struct node *first;
    struct node *last;
};

// ===========INTERNAL FUNCTION IMPLEMENTATIONS============

/**
 * list_empty() - Creates a new empty list.
 * @boolean: The value given for taking control of memory.
 *
 * Returns: A pointer to a new list.
 */
list *list_empty(int boolean) {
    list *l = malloc(sizeof(list));
    l->first = malloc(sizeof(struct node));
    l->last = malloc(sizeof(struct node));
    memory_control(l->first);
    memory_control(l->last);
    l->is_struct = boolean;
    l->first->next = l->last;
    l->last->previous = l->first;
    l->num_elements = 0;
    return l;
}

/**
 * list_is_empty() - Checks if the list is empty.
 * @l: The specified list.
 *
 * Returns: A boolean if it's empty or not.
 */
bool list_is_empty(list *l) {

    if (l->first->next == l->last) {
        return true;
    } else {
        return false;
    }
}

/**
 * list_last() - Returns the last element of a given list.
 * @l: The specified list.
 *
 * Returns: The pointer to the endpointer.
 */
pos_node list_last(list *l) {
    return l->last;
}

/**
 * list_first() - Returns a position the first element of the list.
 * @l: The specified list.
 *
 * Returns: The pointer to the first element.
 */
pos_node list_first(list *l) {
    return l->first->next;
}

/**
 * list_next() - Gives the next node position of the node given.
 * @l: The specified list.
 * @n: The current node.
 *
 * Returns: The pointer to the next element.
 */
pos_node list_next(list *l, pos_node n) {

    if (list_is_empty(l)) {
        fprintf(stderr, "We're trying to iterate an empty list.\n");
    }
    return n->next;
}

/**
 * list_previous() - Gives the next node position of the node given.
 * @l: The specified list.
 * @n: The current node.
 *
 * Returns: The pointer to the next element.
 */
pos_node list_previous(list *l, pos_node n) {

    if (list_is_empty(l)) {
        fprintf(stderr, "We're trying to iterate an empty list.\n");
    }
    return n->previous;
}

/**
 * list_inspect() - Inspects the the value of the node given.
 * @l: The specified list.
 * @n: The current node.
 *
 * Returns: The pointer to the value of the node.
 */
void *list_inspect(list *l, pos_node n) {

    if (n == l->first) {
        fprintf(stderr, "We're trying to use the startpointer of the list.\n");
    }
    return n->value;
}

/**
 * list_number_of_elements() - Gives the sixe of the list in ints.
 * @l: The specified list.
 *
 * Returns: A pointer to an int which specifies the size of the list.
 */
int list_number_of_elements(list *l) {
    return l->num_elements;
}

/**
 * list_remove_node() - Removes the node given for a list from it.
 * @l: The specified list.
 * @n: The current node.
 *
 * Returns: A pointer to the value after the one deleted.
 */
pos_node list_remove_node(list *l, pos_node n) {

    if (n == list_last(l)) {
        fprintf(stderr, "We're trying to delete the endpointer.\n");
        return n;
    }

    if (n == l->first) {
        fprintf(stderr, "We're trying to delete the startpointer.\n");
        return n;
    }
    pos_node next_node = list_next(l, n);
    n->previous->next = n->next;
    n->next->previous = n->previous;

    if (!l->is_struct) {
        free(n->value);
    }
    free(n);
    l->num_elements--;
    return next_node;
}

/**
 * list_insert_node() - Inserts a node before the node specified.
 * @l: The specified list.
 * @n: The current node.
 * @v: The value to set for the new node.
 *
 * Returns: A pointer to the new node.
 */
pos_node list_insert_node(list *l, pos_node n, void *v) {
    pos_node new_node = malloc(sizeof(struct node));
    memory_control(new_node);
    new_node->value = v;
    new_node->next = n;
    new_node->previous = n->previous;
    n->previous = new_node;
    new_node->previous->next = new_node;
    l->num_elements++;
    return new_node;
}

/**
 * list_replace() - Replaces a given nodes value in the list.
 * @n: The node which value is to be replaced.
 * @v: The value to replace it.
 *
 * Returns: Nothing.
 */
void list_replace(pos_node n, void *v) {
    n->value = v;
}

/**
 * list_free() - Cleans the memory of a given list, it can't be used again.
 * @l: The list to be murdered.
 *
 * Returns: Nothing.
 */
void list_free(list *l) {

    if (!l->is_struct) {
        pos_node current_node = list_first(l);

        while (current_node != list_last(l)) {
            current_node = list_remove_node(l, current_node);
            current_node = list_next(l, current_node);
        }
    }
    free(l->first);
    free(l->last);
    free(l);
}

/**
 * list_print() - Prints the given list. 
 * @l: The list to be printed.
 *
 */
void list_print(list *l) {
    pos_node current_node = list_first(l);

    while (current_node != list_last(l)) {
            printf("[%p], ", list_inspect(l, current_node));
            current_node = list_next(l, current_node);
    }
    printf("\n");
}

// ===========HELP FUNCTIONS============

/**
 * memory_control() - Controls that memory is allocated.
 * @memory: Memory to be checked.
 *
 * Returns: Nothing if everything is okey. Otherwise exit is used and perror
 *          is used to specify the error.
 */
void memory_control(void *memory) {

    if (memory == NULL) {
        perror("Memory related error");
        exit(errno);
    }
}
