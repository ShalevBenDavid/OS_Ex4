// Created by Shalev Ben David and Ron Shuster.

#ifndef OS_EX4_ST_REACTOR_H
#define OS_EX4_ST_REACTOR_H

#include <pthread.h>
#include <vector>
#include <poll.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "9034" // Port we're listening on.
#define DATA_LEN 512 //  The data length we are willing to transfer.

typedef void (*handler_t) (void*, int);

// Struct for file descriptor.
typedef struct {
    int file_descriptor; // File descriptor.
    handler_t event_handler; // Pointer to a function to handle fd.
} FD, *P_FD;

// Struct to hold the reactor info.
typedef struct R {
    std :: vector <P_FD> file_descriptors; // Vector of file descriptors.
    std :: vector <pollfd> poll_fds; // Vector of poll for file descriptors.
    pthread_t thread_id; // Hold the reactor's thread id.
    bool is_alive; // Is the reactor thread alive.
} Reactor, *P_Reactor;

void* createReactor ();
void stopReactor (void*);
void startReactor (void*);
void addFd (void*, int, handler_t);
void WaitFor (void*);
void deleteFD (void*, int);
void* runServer(void*);
void wipe (void*);

#endif //OS_EX4_ST_REACTOR_H
