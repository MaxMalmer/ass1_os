#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

char newline[] = "\n";
#define MM 1000000

int main(){
	for (int k = 0; k < 100000; k++) {
		char filename[20];
		sprintf(filename, "junkfile%d", k);		

		//File stuff
		if (access(filename, F_OK) != 0) {
			FILE* fp = fopen(filename, "w");
			//printf("%s NOT found, generating one... please wait kindly\n\n", filename);
			for (int i = 0; i < 10; i++) {
				char str[20];
				sprintf(str, "%d", i);
				for (int j = 0; j < 100; j++) {
					fwrite(str, 1, strlen(str), fp);				
				}
				fwrite(newline, 1, strlen(newline), fp);				
			}
			fclose(fp);
		} else {
		//	printf("%s found, running tests...",filename);
		}
	}
	//printf("Done with file generation...\n\n");
	//Tests
	int* rnumbers = malloc(sizeof(int)*MM);
	int* snumbers = malloc(sizeof(int)*MM);
	srandom(47);

	for (int i = 0; i < MM; i++) {
		double r = (((double) rand()) / ((double)RAND_MAX));
		rnumbers[i] =(int) (r*MM / 1001);
	}
	for (int i = 0; i < 1000; i++) {
		for (int j = 0; j < 1000; j++) {
			snumbers[i*1000 + j] = i;
		}
	}
	
	int sequential = 1;
	//FILE* fp;
	int ptr;
	int readsize = 1000;
	char linee[readsize];

	if (sequential) {
		struct timeval starttime;
		gettimeofday(&(starttime), NULL);
		struct timeval t0 = starttime;
		struct timeval t1, dt;
		
		for (int j = 0; j < 1000; j++) {

			for (int i = 0; i < 1000; i++) {
				int index = snumbers[i];
				char str[20];
				sprintf(str, "junkfile%d", index);
				//fp = fopen(str, "r");
				ptr = open(str, O_RDONLY);
				//fread(linee, 1, readsize, fp);
				read(ptr, linee, readsize);
				//fclose(fp);
				close(ptr);
			}
		}
		gettimeofday(&t1, NULL);
		timersub(&t1, &t0, &dt);

		fprintf(stdout, "%ld.%06ld\n", dt.tv_sec, dt.tv_usec);
	} else {
		for (int i = 0; i < MM; i++) {
			int index = rnumbers[i];
			char str[20];
			sprintf(str, "junkfile%d", index);
			//fp = fopen(str, "r");
			//fread(linee, 1, readsize, fp);
			//fclose(fp);
			ptr = open(str, O_RDONLY);
			read(ptr, linee, readsize);
			close(ptr);
		}
	}
}
