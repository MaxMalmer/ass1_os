#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>

#include "queue.h"

/* mfind.c
 *  Max Malmer, c17mmr@cs.umu.se
 *  Laboration 4 5DV088 HT19
 *  Version 2.0, 2019-11-21
 *  A quite limited implementation of the unix program find. The usage is:
 *  mfind [-t type] [-p nrthr] start1 [start2 ...] name
 */

#define _POSIX_C_SOURCE 200809L
#define	_XOPEN_SOURCE

// ===========INTERNAL DATA TYPES============

typedef struct dir_record dir_record;

// ===========INTERNAL FUNCTION IMPLEMENTATIONS============

/**
 * print_usage() - Prints the usage information of the program if the user would 
 * fail on input. It exits the program with the value of 1. 
 */
void print_usage(void);

/**
 * queue_path() - Queues another path on the the parth_record queue.
 * @path_dir: The directory to be added.
 *
 */
void queue_path(char *path_dir);

/**
 * queue_result() - Queues another result path on the the result_record queue.
 * @result_dir: The directory to be added.
 *
 */
void queue_result(char *result_dir);

/**
 * init_search() - The function that starts the search for each started thread.
 * @placeholder: NULL
 *
 * Returns: NULL
 */
void *init_search(void *placeholder);

/**
 * directory_handler() - The function that iterates though a new directory and
 * searches for the specified file, directory or link. 
 * 
 * Returns: 1 if a directory couldn't be opened. 0 if no problems occured.
 */
int directory_handler(void);

/**
 * free_directory() - Releases the memory for a given queue.
 * @q: The queue to be free'd. 
 *
 */
void free_directory(queue *q);