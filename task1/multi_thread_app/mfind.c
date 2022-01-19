#include "mfind.h"

/* mfind.c
 *  Max Malmer, c17mmr@cs.umu.se
 *  Laboration 4 5DV088 HT19
 *  Version 2.0, 2019-11-21
 *  A quite limited implementation of the unix program find. The usage is:
 *  mfind [-t type] [-p nrthr] start1 [start2 ...] name
 */


// ===========INTERNAL DATA TYPES============

struct dir_record {
    char *path;
};

char *goal;
char t = 0;
pthread_mutex_t path_mutex;
pthread_mutex_t result_mutex;
int nrthr;
queue *path;
queue *result;
queue *free_queue;
int semaphore;
int exit_status;

// ===========INTERNAL FUNCTION IMPLEMENTATIONS============

/**
 * main() - The main of the program which starts and end threads. It also reads
 *  the instructions given to the program. 
 * @argc: Number of arguments.
 * @argv: Array of arguments.
 *
 * Returns: 1 if something whilst running couldn't be read. If everything went 
 * without any issues it returns 0.
 */
int main(int argc, char **argv) {
    char option = 0;
    nrthr = 0;
    bool type_specified = false;
    path = queue_empty(true);
    result = queue_empty(true);
    free_queue = queue_empty(true);
    bool path_added = false;

    if (pthread_mutex_init(&path_mutex, NULL) != 0) {
        fprintf(stderr, "The pathmutex couldn't be initiated");
    }
    
    if (pthread_mutex_init(&result_mutex, NULL) != 0) {
        fprintf(stderr, "The resultmutex couldn't be initiated");
    }

    if ((semaphore = semget(1001, 1, 0600 | IPC_CREAT)) < 0) {
        perror("semaphore:");
        exit(1);
    }
    int option_index = 0;
    static struct option long_options[] = {
        {"pthread", optional_argument, NULL, 'p'},
        {"type", optional_argument, NULL, 't'},
        {NULL, 0, NULL, 0}
    };

    while ((option = getopt_long(argc, argv, "t:p:", 
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
            
            if (option == 't') {
                type_specified = true;
                t = *optarg;
            }
        } else {
            print_usage();
        }

    }
    pthread_t threads[nrthr + 1];
    threads[0] = pthread_self();
    int num_subthreads = nrthr - 1;

    if (type_specified) {

        if (t != 'f' && t != 'd' && t != 'l') {
            print_usage();
        }
    } else {
        t = 0;
    }

    if (optind >= argc) {
        print_usage();
    }

    if (optind == argc - 1) {
        print_usage();
    }

    //Parses the command from the first non option argument.
    for (int i = optind; i < argc; i++) {
        struct stat stat_dir;

        if (strncmp(argv[i], "-p", 2) == 0) {
            continue;
        }
        
        if (strncmp(argv[i], "-t", 2) == 0) {
            continue;
        }

        if (stat(argv[i], &stat_dir) < 0) {

            if (path_added) {
                goal = strdup(argv[i]);
                memory_control(goal);
                break;
            }
            perror("");
            exit(1);
        }

        if (!S_ISDIR(stat_dir.st_mode)) {
            fprintf(stderr, "%s is not a directory or a link to a directory.\n"
                    ,argv[i]);
            exit(1);
        }
        queue_path(argv[i]);
        path_added = true;

        struct sembuf semaphore_signal = {0, 1, 0};

        if (semop(semaphore, &semaphore_signal, 1) < 0) {
            perror("semaphore:");
        }
    }

    //Initialize threads.
    for (int i = 0; i < num_subthreads; i++) {
        
        if (pthread_create(&threads[i + 1], NULL, &init_search, NULL) != 0) {
            perror("pthread:");
        }
    }

    //Initialize thread function for main thread.
    init_search(NULL);

    //Join threads after execution of code.
    for (int i = 0; i < num_subthreads; i++) {

        if (pthread_join(threads[i + 1], NULL) != 0) {
            perror("pthread:");
        } 
    }

    //Print result and end program.
    while (!queue_is_empty(result)) {
        dir_record *temp_record = (dir_record*)queue_inspect(result);
        printf("%s\n", temp_record->path);

        if (!queue_is_empty(result)) {
            dir_record *temp_record2 = (dir_record*)queue_inspect(result);
            free_queue = queue_push(free_queue, temp_record2);
            result = queue_pop(result);
        }
    }

    if (semctl(semaphore, 1, IPC_RMID) < 0) {
        perror("semaphore:");
    }
    free(goal);
    free_directory(free_queue);
    free_directory(path);
    free_directory(result);
    return exit_status;
}

/**
 * print_usage() - Prints the usage information of the program if the user would 
 * fail on input. It exits the program with the value of 1. 
 */
void print_usage(void) {
    fprintf(stderr, 
            "ERROR use: mfind [-t type] [-p nrthr] start1 [start2 ...] name\n");
    exit(1);
}

/**
 * queue_path() - Queues another path on the the parth_record queue.
 * @path_dir: The directory to be added.
 *
 */
void queue_path(char *path_dir) {

    char *this_dir = strdup(path_dir);
    memory_control(this_dir);
    dir_record *path_record = malloc(sizeof(dir_record));
    memory_control(path_record);
    path_record->path = this_dir;

    if (pthread_mutex_lock(&path_mutex) != 0) {
        fprintf(stderr, "Mutex couldn't be locked");
    }
    path = queue_push(path, path_record);

    if (pthread_mutex_unlock(&path_mutex) != 0) {
        fprintf(stderr, "Mutex couldn't be unlocked"); 
    }
}

/**
 * queue_result() - Queues another result path on the the result_record queue.
 * @result_dir: The directory to be added.
 *
 */
void queue_result(char *result_dir) {

    char *this_dir = strdup(result_dir);
    memory_control(this_dir);
    dir_record *result_record = malloc(sizeof(dir_record));
    memory_control(result_record);
    result_record->path = this_dir;

    if (pthread_mutex_lock(&result_mutex) != 0) {
        fprintf(stderr, "Mutex couldn't be locked");
    }
    result = queue_push(result, result_record);

    if (pthread_mutex_unlock(&result_mutex) != 0) {
       fprintf(stderr, "Mutex couldn't be unlocked"); 
    }
}

/**
 * init_search() - The function that starts the search for each started thread.
 * @placeholder: NULL
 *
 * Returns: NULL
 */
void *init_search(void *placeholder) {
    int reads = 0;

    //While the semaphore isn't full, read directories.
    do {
        int sem_status = 0;
        struct sembuf semaphore_wait = {0, -1, 0};

        //Wait for path to be put in queue. 
        if ((sem_status = semop(semaphore, &semaphore_wait, 1)) == -1) {
            perror("semaphore:");
        }

        //If there's data available in the semanphore start dir_handler.
        if (sem_status != 11) {
            exit_status = directory_handler();
            reads++;
        }
    } while(semctl(semaphore, 0, GETVAL) > 0);

    //Thread is finnished, print and signal for other threads to continue.
    printf("Thread: %d Reads: %d\n", (int)pthread_self(), reads);
    struct sembuf semaphore_signal = {0, 1, 0};

    if (semop(semaphore, &semaphore_signal, 1) == -1) {
        perror("sempahore:");
    }
    return placeholder;
}

/**
 * directory_handler() - The function that iterates though a new directory and
 * searches for the specified file, directory or link. 
 * 
 * Returns: 1 if a directory couldn't be opened. 0 if no problems occured.
 */
int directory_handler(void) {

    if (pthread_mutex_lock(&path_mutex) != 0) {
        fprintf(stderr, "Mutex couldn't be locked");
    }

    if (queue_is_empty(path)) {

        if (pthread_mutex_unlock(&path_mutex) != 0) {
            fprintf(stderr, "Mutex couldn't be unlocked");
        }
        return 0;
    }
    dir_record *current_record;
    current_record = queue_inspect(path);
    dir_record *temp_record = (dir_record*)queue_inspect(path);
    free_queue = queue_push(free_queue, temp_record);
    path = queue_pop(path);

    if (pthread_mutex_unlock(&path_mutex) != 0) {
        fprintf(stderr, "Mutex couldn't be unlocked");
    }
    char *current_path = current_record->path;
    DIR *this_dir;
    struct dirent *directory_pointer;
    struct stat stat_dir;

    //Check if we can access and open dir.
    if (access(current_path, R_OK|X_OK) < 0) {
        fprintf(stderr, "'%s': user lacks permission\n", current_path);
        return 1;
    }

    if ((this_dir = opendir(current_path)) == NULL) {
        pthread_mutex_lock(&path_mutex);
        fprintf(stderr, "Cannot open directory %s\n", current_path);
        pthread_mutex_unlock(&path_mutex);
        return 1;
    }

    //Search the directory for information.
    while ((directory_pointer = readdir(this_dir)) != NULL) {

        if (strcmp(directory_pointer->d_name, ".") == 0 
            || strcmp(directory_pointer->d_name, "..") == 0) {
            continue;
        }
        bool is_goal = false;
        unsigned int path_len = strlen(current_path);
        char *complete_path = malloc(sizeof(char*) * 4096 * (nrthr + 1));
        memory_control(complete_path);
        strcpy(complete_path, current_path);
        complete_path[path_len++] = '/';
        strcpy(&complete_path[path_len], directory_pointer->d_name);

        if (lstat(complete_path, &stat_dir) < 0) {
            perror("");
        }

        if (strcmp(goal, directory_pointer->d_name) == 0) {
            is_goal = true;
        }
        
        if (S_ISDIR(stat_dir.st_mode)) {

            if ((t == 0 || t == 'd') && is_goal) {
                queue_result(complete_path);
            }

            //Add new dir to queue and signal for the next thread to enter.
            queue_path(complete_path);
            struct sembuf semaphore_signal = {0, 1, 0};

            if (semop(semaphore, &semaphore_signal, 1) == -1) {
                perror("");
            }
        }

        if (S_ISLNK(stat_dir.st_mode)) {

            if ((t == 0 || t == 'l') && is_goal) {
                queue_result(complete_path);
            }
        }

        if (S_ISREG(stat_dir.st_mode)) {

            if ((t == 0 || t == 'f') && is_goal) {
                queue_result(complete_path);
            }
        }
        free(complete_path);
    }

    if (closedir(this_dir) < 0) {
        perror("");
    }
    return 0;
}

/**
 * free_directory() - Releases the memory for a given queue.
 * @q: The queue to be free'd. 
 *
 */
void free_directory(queue *q) {

    while (!queue_is_empty(q)) {
        dir_record *temp_record = (dir_record*)queue_inspect(q);
        free(temp_record->path);
        free(temp_record);
        q = queue_pop(q);
    }
    queue_free(q);
}