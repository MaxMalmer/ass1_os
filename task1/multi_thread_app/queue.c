#include "queue.h"

/* queue.c
 *  Max Malmer, c17mmr@cs.umu.se
 *  Laboration 4 5DV088 HT19
 *  Version 1.0, 2019-11-21
 *  A implementation of the datatype queue with a double linked list.
 *  Use queue_free() to clear the memory after use. Give the input 1(true)
 *  to the queue to take control of the memory cleaning. Give it 0(false)
 *  otherwise.
 */

// ===========INTERNAL DATA TYPES============

struct queue {
    struct list *l;
};

// ===========INTERNAL FUNCTION IMPLEMENTATIONS============

/**
 * queue_empty() - Creates a new empty queue.
 * @boolean: The value given for taking control of memory.
 *
 * Returns: A pointer to a new queue.
 */
queue *queue_empty(int boolean) {
    queue *q = malloc(sizeof(queue));
    memory_control(q);
    q->l = list_empty(boolean);
    return q;
}

/**
 * list_is_empty() - Checks if the queue is empty.
 * @q: The specified queue.
 *
 * Returns: A boolean if it's empty or not.
 */
bool queue_is_empty(queue *q) {

    if (list_is_empty(q->l)) {
        return true;
    } else {
        return false;
    }
}

/**
 * queue_push() - Adds a element to the queue.
 * @q: The specified queue.
 * @v: The element to be added.
 *
 * Returns: The queue.
 */
queue *queue_push(queue *q, void *v) {
    list_insert_node(q->l, list_last(q->l), v);
    return q;
}

/**
 * queue_pop() - Removes the top element in the queue.
 * @q: The specified queue.
 *
 * Returns: The queue.
 */
queue *queue_pop(queue *q) {

    if (queue_is_empty(q)) {
        fprintf(stderr, "Trying to empty an empty queue\n");
    }
    list_remove_node(q->l, list_first(q->l));
    return q;
}

/**
 * queue_inspect() - Inspects the top element of the queue
 * @q: The specified queue.
 *
 * Returns: A pointer to the element.
 */
void *queue_inspect(queue *q) {

    if (queue_is_empty(q)) {
        fprintf(stderr, "Trying to inspect an empty queue\n");
    }
    return list_inspect(q->l, list_first(q->l));
}

/**
 * queue_free() - Removes the queue and it's elements from memory.
 * @q: The specified queue.
 *
 */
void queue_free(queue *q) {
    list_free(q->l);
    free(q);
}

/**
 * queue_print() - Prints the queue. 
 * @q: The specified queue.
 *
 */
void queue_print(queue *q) {
    list_print(q->l);
}
