#ifndef PRIME_FACTOR_H
#define PRIME_FACTOR_H

/**
 * @defgroup prime_factor_h Prime Factor.
 * 
 * @brief A program that factors a given number for primes.
 * 
 * A program that prime factors random numbers for a given number of threads.
 * The given number of threads is also the number the program uses as upper bound
 * for the random generations of numbers for the prime factorisation.
 * 
 * use: prime_factor [-p num_threads] [-s scheduler]
 * 
 * @author Max Malmer
 * @since  2022-02-02
 * 
 * @{
 */

#define _POSIX_C_SOURCE 200809L
#define	_XOPEN_SOURCE

typedef struct {
    int num;
    bool sched_manual;
    int num_to_factor;
} pthread_data;

/**
 * @brief Prints the correct usage of the program.
 * 
 * @param -
 * @return -
 */
void print_usage(void);

/**
 * @brief Calculates the prime factors of a given number.
 * 
 * @param num       a pthread_data struct with the settings of the thread. 
 * @return          -
 */
void *prime_factors(void *data);

/**
 * @brief Sleep a thread for a given number of miliseconds.
 * 
 * @param milisec   The number of miliseconds a thread should sleep.
 */
int msleep(long milisec);

#endif /* PRIME_FACTOR_H */

/**
 * @}
 */