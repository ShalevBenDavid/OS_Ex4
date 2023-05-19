// Created by Shalev Ben David and Ron Shuster.

#ifndef OS_EX4_ST_REACTOR_H
#define OS_EX4_ST_REACTOR_H

#include "stdbool.h"
#include <pthread.h>

typedef void (*handler_t) (int);

// Struct for file descriptor.
typedef struct {
    int file_descriptor; // File descriptor.
    handler_t event_handler; // Pointer to a function to handle fd.
} fd, *p_fd;

// Struct to hold the reactor info.
typedef struct {
    p_fd* file_descriptors; // Array of file descriptors.
    int size; // Number of file descriptors.
    pthread_t thread_id; // Hold the reactor's thread id.
    bool is_alive; // Is the reactor thread alive.
} Reactor, *P_Reactor;

void* createReactor ();
void stopReactor (void*);
void startReactor (void*);
void addFd (void*, int, handler_t);
void WaitFor (void*);

#endif //OS_EX4_ST_REACTOR_H
