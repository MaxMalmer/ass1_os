#ifndef TASK2_H
#define TASK2_H

/**
 * @defgroup task2_h Task 2.
 * 
 * @brief A program that creates a bunch of junkfiles and reads from them.
 * 
 * This program is buildt to test the function 
 *
 * @author Max Malmer
 * @since  2022-02-02
 * 
 * @{
 */

#define _POSIX_C_SOURCE 200809L
#define	_XOPEN_SOURCE

/**
 * @brief A thread-function that reads a lot from a amount of given files.
 * 
 * @param -
 * @return -
 */
void *read_function(void *placeholder);

#endif /* TASK2_H */

/**
 * @}
 */