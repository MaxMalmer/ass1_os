#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "list.h"

/* queue.c
 *  Max Malmer, c17mmr@cs.umu.se
 *  Laboration 4 5DV088 HT19
 *  Version 1.0, 2019-10-24
 *  A implementation of the datatype queue with a double linked list.
 *  Use queue_free() to clear the memory after use. Give the input 1
 *  to the queue to take control of the memory cleaning. Give it 0
 *  otherwise.
 */

// ===========INTERNAL DATA TYPES============

typedef struct queue queue;

// ===========INTERNAL FUNCTION IMPLEMENTATIONS============

/**
 * queue_empty() - Creates a new empty queue.
 * @boolean: The value given for taking control of memory.
 *
 * Returns: A pointer to a new queue.
 */
queue *queue_empty(int boolean);

/**
 * list_is_empty() - Checks if the queue is empty.
 * @q: The specified queue.
 *
 * Returns: A boolean if it's empty or not.
 */
bool queue_is_empty(queue *q);

/**
 * queue_push() - Adds a element to the queue.
 * @q: The specified queue.
 * @v: The element to be added.
 *
 * Returns: The queue.
 */
queue *queue_push(queue *q, void *v);

/**
 * queue_pop() - Removes the top element in the queue.
 * @q: The specified queue.
 *
 * Returns: The queue.
 */
queue *queue_pop(queue *q);

/**
 * queue_inspect() - Inspects the top element of the queue
 * @q: The specified queue.
 *
 * Returns: A pointer to the element.
 */
void *queue_inspect(queue *q);

/**
 * queue_free() - Removes the queue and it's elements from memory.
 * @q: The specified queue.
 *
 */
void queue_free(queue *q);

/**
 * queue_print() - Prints the queue. 
 * @q: The specified queue.
 *
 */
void queue_print(queue *q);