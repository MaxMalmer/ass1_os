#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <getopt.h>
#include "task2.h"

/*
 * Operating Systems UMU
 * Spring 22
 * Assignment 1
 *
 * File:         task2.c
 * Description:  A program that writes a lot of files and reads from them.
 * Author:       Oscar Kamf, Max Malmer
 * CS username:  oi17okf, maxmal
 * Date:         2022-02-10
 * Input:        -
 * Output:       A long execution time.
 * Limitations:  -
 */

char newline[] = "\n";
int type_specified = O_RDONLY;
struct timeval t0;
#define MM 1000000

int main(int argc, char **argv) {
	char option = 0;
    int option_index = 0;
	int read_type = 0;
	srand(0);

	static struct option long_options[] = {
        {"pthread", optional_argument, NULL, 'r'},
        {NULL, 0, NULL, 0}
    };

    while ((option = getopt_long(argc, argv, "r:", 
                                long_options, &option_index)) != -1) {

        if (option != 0) {

            if (option == 'r') {

                if (sscanf(optarg,"%d", &read_type) == EOF) {
                    print_usage();
                }
            }
        } else {
            print_usage();
        }
    }

	if (read_type == 0) {
		type_specified = O_RDONLY;
	} else if (read_type == 1) {
		type_specified = O_SYNC;
	} else if (read_type == 2) {
		type_specified = __O_DIRECT;
	}

	for (int k = 0; k < 10000; k++) {
		char filename[20];
		sprintf(filename, "junkfile%d", k);		

		if (access(filename, F_OK) != 0) {
			FILE* fp = fopen(filename, "w");
			for (int i = 0; i < 10; i++) {
				char str[20];
				sprintf(str, "%d", i);
				for (int j = 0; j < 100; j++) {
					fwrite(str, 1, strlen(str), fp);				
				}
				fwrite(newline, 1, strlen(newline), fp);				
			}
			fclose(fp);
		}
	}
	int sequential = 0;
	int ptr;
	int readsize = 1000;
	char linee[readsize];
	int nrthr = 100;
	int num_subthreads = nrthr - 1;

	if (!sequential) {
		pthread_t threads[nrthr + 1];
		threads[0] = pthread_self();
		gettimeofday(&t0, NULL);

		for (int i = 0; i < num_subthreads; i++) {

			if (pthread_create(&threads[i + 1], NULL, &read_function, NULL) != 0) {
				perror("pthread:");
			}
		}

		for (int i = 0; i < num_subthreads; i++) {

			if (pthread_join(threads[i + 1], NULL) != 0) {
				perror("pthread:");
			} 
		}

	} else {

		int* rnumbers = malloc(sizeof(int)*MM);
		int* snumbers = malloc(sizeof(int)*MM);

		for (int i = 0; i < MM; i++) {
			double r = (((double) rand()) / ((double)RAND_MAX));
			rnumbers[i] =(int) (r*MM / 1001);
		}
		for (int i = 0; i < 1000; i++) {
			for (int j = 0; j < 1000; j++) {
				snumbers[i*1000 + j] = i;
			}
		}

		for (int i = 0; i < 100000; i++) {
			int index = rnumbers[i];
			char str[20];
			sprintf(str, "junkfile%d", index);
			ptr = open(str, type_specified);
			read(ptr, linee, readsize);
			close(ptr);
		}
	}
	struct timeval t1, dt;
    gettimeofday(&t1, NULL);
    timersub(&t1, &t0, &dt);
    double throughput = (double)num_subthreads / (dt.tv_sec);
    fprintf(stdout, "%lf", throughput);

	return 0;
}

void *read_function(void *placeholder) {

	struct timeval t1, dt;
	gettimeofday(&t1, NULL);
	timersub(&t1, &t0, &dt);
	fprintf(stdout, "%ld.%06ld\n", dt.tv_sec, dt.tv_usec);

	int* rnumbers = malloc(sizeof(int)*MM);
	int* snumbers = malloc(sizeof(int)*MM);

	for (int i = 0; i < MM; i++) {
		double r = (((double) rand()) / ((double)RAND_MAX));
		rnumbers[i] =(int) (r*MM / 1001);
	}
	for (int i = 0; i < 1000; i++) {
		for (int j = 0; j < 1000; j++) {
			snumbers[i*1000 + j] = i;
		}
	}

	int ptr;
	int readsize = 1000;
	char linee[readsize];

	for (int i = 0; i < 1000; i++) {
		int index = snumbers[i];
		char str[20];
		sprintf(str, "junkfile%d", index);
		ptr = open(str, type_specified);
		read(ptr, linee, readsize);
		close(ptr);
	}
	
	return 0;
}

void print_usage(void) {
    fprintf(stderr, 
            "ERROR use: task2 [-p caching type] 1 O_RDONLY 2 O_SYNC 3 O_DIRECT\n");
    exit(1);
}

