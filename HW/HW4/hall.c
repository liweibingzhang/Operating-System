#include "hall.h"
#include <stdio.h>
#include <pthread.h>   // for pthreads
#include <stdlib.h>    // for exit
#include <unistd.h>    
static pthread_mutex_t mutex;
static pthread_cond_t hall_cond;
static char* hall_state;
void initMonitor(int n){
    // Allocate memory for the hall state
    hall_state = malloc((n + 1) * sizeof(char)); // +1 for null-terminator
    
    // Initialize the hall state to all free spaces
    for (int i = 0; i < n; i++) {
        hall_state[i] = '*';
    }
    hall_state[n] = '\0'; // null-terminator
    
    // Initialize the mutex and condition variable
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&hall_cond, NULL);
}
void destroyMonitor(){
    free(hall_state);
    // Destroy the mutex and condition variable
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&hall_cond);
}
int allocateSpace( char const *name, int width ){
    // Acquire the mutex to enter the monitor
    pthread_mutex_lock(&mutex);
    int start = -1;
    while (start == -1) {
        // Search for a contiguous block of free spaces of the required width
        for (int i = 0; i < width; i++) {
            if (hall_state[i] == '*') {
                int j;
                for (j = i + 1; j < i + width; j++) {
                    if (hall_state[j] != '*') {
                        break;
                    }
                }
                if (j == i + width) {
                    // Found a block of free spaces
                    start = i;
                    break;
                }
            }
        }
        if (start == -1) {
            // No free space available, wait for a signal
            printf("%s waiting: %s\n", name, hall_state);
            pthread_cond_wait(&hall_cond, &mutex);
        }
    }
    // Mark the allocated spaces as taken by the organization
    for (int i = start; i < start + width; i++) {
        hall_state[i] = name[0];
    }
    // Print the allocation report
    printf("%s allocated: %s\n", name, hall_state);
    // Release the mutex
    pthread_mutex_unlock(&mutex);

    return start;
}

/** Relese the allocated spaces from index start up to (and including)
    index start + width - 1. */
void freeSpace( char const *name, int start, int width ){
    // Acquire the mutex to enter the monitor
    pthread_mutex_lock(&mutex);

    // Mark the freed spaces as available
    for (int i = start; i < start + width; i++) {
        hall_state[i] = '*';
    }

    // Print the allocation report
    printf("%s freed: %s\n", name, hall_state);

    // Signal all waiting threads that a space has become available
    pthread_cond_broadcast(&hall_cond);

    // Release the mutex
    pthread_mutex_unlock(&mutex);
}