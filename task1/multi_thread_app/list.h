#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

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

typedef struct list list;

typedef struct node *pos_node;

// ===========INTERNAL FUNCTION IMPLEMENTATIONS============

/**
 * list_empty() - Creates a new empty list.
 * @boolean: The value given for taking control of memory.
 *
 * Returns: A pointer to a new list.
 */
list *list_empty(int boolean);

/**
 * list_is_empty() - Checks if the list is empty.
 * @l: The specified list.
 *
 * Returns: A boolean if it's empty or not.
 */
bool list_is_empty(list *l);

/**
 * list_last() - Returns the last element of a given list.
 * @l: The specified list.
 *
 * Returns: The pointer to the endpointer.
 */
pos_node list_last(list *l);

/**
 * list_first() - Returns a position the first element of the list.
 * @l: The specified list.
 *
 * Returns: The pointer to the first element.
 */
pos_node list_first(list *l);

/**
 * list_next() - Gives the next node position of the node given.
 * @l: The specified list.
 * @n: The current node.
 *
 * Returns: The pointer to the next element.
 */
pos_node list_next(list *l, pos_node n);

/**
 * list_previous() - Gives the next node position of the node given.
 * @l: The specified list.
 * @n: The current node.
 *
 * Returns: The pointer to the next element.
 */
pos_node list_previous(list *l, pos_node n);

/**
 * list_inspect() - Inspects the the value of the node given.
 * @l: The specified list.
 * @n: The current node.
 *
 * Returns: The pointer to the value of the node.
 */
void *list_inspect(list *l, pos_node n);

/**
 * list_number_of_elements() - Gives the sixe of the list in ints.
 * @l: The specified list.
 *
 * Returns: A pointer to an int which specifies the size of the list.
 */
int list_number_of_elements(list *l);

/**
 * list_remove_node() - Removes the node given for a list from it.
 * @l: The specified list.
 * @n: The current node.
 *
 * Returns: A pointer to the value after the one deleted.
 */
pos_node list_remove_node(list *l, pos_node n);

/**
 * list_insert_node() - Inserts a node before the node specified.
 * @l: The specified list.
 * @n: The current node.
 * @v: The value to set for the new node.
 *
 * Returns: A pointer to the new node.
 */
pos_node list_insert_node(list *l, pos_node n, void *v);

/**
 * list_replace() - Replaces a given nodes value in the list.
 * @n: The node which value is to be replaced.
 * @v: The value to replace it.
 *
 * Returns: Nothing.
 */
void list_replace(pos_node n, void *v);

/**
 * list_free() - Cleans the memory of a given list, it can't be used again.
 * @l: The list to be murdered.
 *
 * Returns: Nothing.
 */
void list_free(list *l);

/**
 * list_print() - Prints the given list. 
 * @l: The list to be printed.
 *
 */
void list_print(list *l);

// ===========HELP FUNCTIONS============

/**
 * memory_control() - Controls that memory is allocated.
 * @memory: Memory to be checked.
 *
 * Returns: Nothing if everything is okey. Otherwise exit is used and perror
 *          is used to specify the error.
 */
void memory_control(void *memory);
