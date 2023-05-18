// Created by Shalev Ben David and Ron Shuster.

#include "st_reactor.h"
#include <stdlib.h>
#include <pthread.h>

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
    reac -> file_descriptors = (p_fd*) calloc (sizeof (p_fd), 1);
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
        reac -> is_alive = false;
    }
}

/**
 * Starts a thread for the reactor. The thread will run in infinite loop with poll.
 */
void startReactor (void* this) {


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

}