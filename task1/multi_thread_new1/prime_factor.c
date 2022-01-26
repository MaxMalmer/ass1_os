#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <getopt.h>
#include <sched.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include "prime_factor.h"

bool sched_manual = false;
int num_to_factor = 0;

int main(int argc, char **argv) {
    char option = 0;
    int option_index = 0;
    int nrthr = 1;
    char sched_type = 0;
    bool sched_specified = false;
    
    static struct option long_options[] = {
        {"pthread", optional_argument, NULL, 'p'},
        {NULL, 0, NULL, 0}
    };

    while ((option = getopt_long(argc, argv, "p:s:", 
                                long_options, &option_index)) != -1) {

        if (option != 0) {

            if (option == 'p') {

                if (sscanf(optarg,"%d", &nrthr) == EOF) {
                    print_usage();
                }

                if (nrthr < 1) {
                    print_usage();
                }
            }

            if (option == 's') {
                sched_specified = true;
                sched_type = *optarg;
            }
        } else {
            print_usage();
        }
    }

    if (sched_specified) {

        if (sched_type != 'f' && sched_type != 'r' && sched_type != 'm') {
            print_usage();
        }
    }

    if (optind == argc - 1) {
        print_usage();
    }

    if (sched_type == 'f') {

        if ((sched_setscheduler(0, SCHED_FIFO, NULL)) > 0) {
            perror("scheduler:");
        }

    } else if (sched_type == 'r') {

        if ((sched_setscheduler(0, SCHED_RR, NULL)) > 0) {
            perror("scheduler:");
        }

    } else if (sched_type == 'm') {
        sched_manual = true;
    }

    pthread_t threads[nrthr + 1];
    threads[0] = pthread_self();
    int num_subthreads = nrthr - 1;
    num_to_factor = nrthr;

    // while tills knapp trycks eller t.ex. 200 iterationer
    for (int i = 0; i < num_subthreads; i++) {

        // Randomisera ett tal mellan 0 och num_to_factor
        num_to_factor = num_to_factor - i;
        
        if (pthread_create(&threads[i + 1], NULL, &prime_factors, &num_to_factor) != 0) {
            perror("pthread:");
        }

        // check all threads if done from static global array
        msleep(5);
    }

    /*for (int i = 0; i < num_subthreads; i++) {

        num_to_factor = num_to_factor - i;
        
        if (pthread_join(threads[i + 1], NULL) != 0) {
            perror("pthread:");
        }
    }*/

    return 0;
}

void print_usage(void) {
    fprintf(stderr, 
            "ERROR use: prime_factor [-p start_prime] [-s scheduler]\n");
    exit(1);
}

void *prime_factors(void *prime) {
    clock_t start_t, end_t;
    start_t = clock();
    int n = *((int*)prime);

    if (sched_manual) {

        struct sched_param param;
        param.sched_priority = num_to_factor - n;

        if (param.sched_priority > sched_get_priority_min(SCHED_RR)) {
            param.sched_priority = sched_get_priority_min(SCHED_RR);
        }

        if (pthread_setschedparam(pthread_self(), SCHED_OTHER, &param) != 0) {
            perror("pthread_setscheduler:");
        }
    }

    while (n % 2 == 0) {
        // One factor 2 found
        n = n / 2;
    }
 
    for (int i = 3; i <= sqrt(n); i = i + 2) {

        while (n % i == 0) {
            // Factor found
            n = n / i;
        }
    }

    //if (n > 2)
        // Factor found
        
    end_t = clock();
    fprintf(stdout, "%ld\n", end_t - start_t);

    return 0;
}

int msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}