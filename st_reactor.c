// Created by Shalev Ben David and Ron Shuster.

#include "st_reactor.h"
#include <stdlib.h>

void* runServer (void* this);

/**
 * Creates a reactor.
 * @return - A pointer to the new allocated reactor.
 */
void* createReactor () {
    P_Reactor reac = (P_Reactor) calloc (sizeof (Reactor), 1);
    // If calloc failed, exit.
    if (!reac) {
        exit(EXIT_FAILURE);
    }
    // Allocate memory (initialize values).
    reac -> size = 0;
    reac -> is_alive = false;
    reac -> thread_id = NULL;
    reac -> file_descriptors = (p_fd*) calloc (sizeof (fd), 1);
    // If calloc failed, exit.
    if (!reac -> file_descriptors) {
        free(reac -> file_descriptors);
        free(reac);
        exit(EXIT_FAILURE);
    }
    // Return pointer to the reactor.
    return reac;
}

/**
 * Stops the reactor if it's running. Else, does nothings.
 */
void stopReactor (void* this) {
    P_Reactor reac = (P_Reactor) this;
    // Check if the reactor is active.
    if (reac -> is_alive) {
        //Sending cancel request to thread.
        pthread_cancel(reac -> thread_id);
        // Wait for the thread to terminate.
        pthread_join(reac -> thread_id, NULL);
        // Set alive status to false;
        reac -> is_alive = false;

        printf("(+) Stopped reactor.\n")
    }
}

/**
 * Starts a thread for the reactor. The thread will run in infinite loop with poll.
 */
void startReactor (void* this) {
    P_Reactor reac = (P_Reactor) this;
    // If reactor already running, return.
    if (reac -> is_alive) {
        printf("(-) Reactor already running!\n");
        return;
    }
    // Begin single thread.
    int result = pthread_create(&reac -> thread_id, NULL, runServer(this), NULL);
    // If running thread failed, exit.
    if (result) {
        printf("(-) Creating thread failed!\n");
        return;
    } else {
        printf("(+) Started reactor successfully.\n");
    }
    // Set alive status to true;
    reac -> is_alive = true;
}

/**
 *
 */
void addFd (void* this, int fd, handler_t handler) {

}

/**
 * Wait on pthread_join(3) until the reactor's thread will end.
 */
void WaitFor (void* this) {
    P_Reactor reac = (P_Reactor) this;
    // Check first that the reactor's thread is alive.
    if (reac -> is_alive) {
        printf("(*) Waiting for thread...\n");
        // Wait for the reactor's thread to finish.
        pthread_join(reac -> thread_id, NULL);
    }
}

void* runServer (void* this) {
    P_Reactor reac = (P_Reactor) this;

}
