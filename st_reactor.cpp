// Created by Shalev Ben David and Ron Shuster.

#include "st_reactor.hpp"
#include <stdlib.h>
#include <stdio.h>

void* runServer (void* obj);

/**
 * Creates a reactor.
 * @return - A pointer to the new allocated reactor.
 */
void* createReactor () {
    // Create reactor.
    P_Reactor reac = new Reactor();
    // Initialize values.
    reac -> is_alive = true;
    reac ->thread_id = 0;
    // Return pointer to the reactor.
    return reac;
}

/**
 * Stops the reactor if it's running. Else, does nothings.
 */
void stopReactor (void* obj) {
    P_Reactor reac = (P_Reactor) obj;
    // Check if the reactor is active.
    if (reac -> is_alive) {
        //Sending cancel request to thread.
        pthread_cancel(reac -> thread_id);
        // Wait for the thread to terminate.
        pthread_join(reac -> thread_id, NULL);
        // Set alive status to false;
        reac -> is_alive = false;

        printf("(+) Stopped reactor.\n");
    }
}

/**
 * Starts a thread for the reactor. The thread will run in infinite loop with poll.
 */
void startReactor (void* obj) {
    P_Reactor reac = (P_Reactor) obj;
    // If reactor already running, return.
    if (reac -> is_alive) {
        printf("(-) Reactor already running!\n");
        return;
    }
    // Begin single thread.
    int result = pthread_create(&reac -> thread_id, NULL, runServer, reac);
    // If running thread failed, exit.
    if (result) {
        printf("(-) Creating thread failed!\n");
        return;
    } else {
        printf("(+) Started reactor successfully.\n");
    }
    // Set alive status to true;
    reac -> is_alive = true;
    // Call wait on thread.
    WaitFor(reac);
}

/**
 *
 */
void addFd (void* obj, int fd, handler_t handler) {
    P_Reactor reac = (P_Reactor) obj;

    // Create allocation for the file descriptor.
    P_FD new_fd = new FD();

    // Set the fd and event handler to the fd pointer.
    new_fd -> file_descriptor = fd;
    new_fd -> event_handler = handler;

    // Create the poll struct for this fd.
    struct pollfd *pfd = new pollfd();
    pfd -> fd = fd;
    pfd -> events = POLLIN; // Report ready to read on incoming connection

    // Update the reactor (insert fd and poll).
    reac -> file_descriptors.push_back(new_fd);
    reac -> poll_fds.push_back(pfd);
}

/**
 * Wait on pthread_join(3) until the reactor's thread will end.
 */
void WaitFor (void* obj) {
    P_Reactor reac = (P_Reactor) obj;
    // Check first that the reactor's thread is alive.
    if (reac -> is_alive) {
        printf("(*) Waiting for thread...\n");
        // Wait for the reactor's thread to finish.
        pthread_join(reac -> thread_id, NULL);
    }
}

// ---------------------- My Functions -------------------------

// Get sockaddr, IPv4 or IPv6:
void* get_in_addr (struct sockaddr* sa) {
    if (sa -> sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa) -> sin_addr);
    }
    return &(((struct sockaddr_in6*) sa) -> sin6_addr);
}

// Return a listening socket
int get_listener_socket (void) {
    int listener;     // Listening socket descriptor
    int yes = 1;        // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai))) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }
    for(p = ai; p != NULL; p = p -> ai_next) {
        listener = socket(p -> ai_family, p -> ai_socktype, p -> ai_protocol);
        if (listener < 0) {
            continue;
        }
        // Lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (bind(listener, p -> ai_addr, p -> ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        break;
    }
    freeaddrinfo(ai); // All done with this
    // If we got here, it means we didn't get bound
    if (p == NULL) {
        return -1;
    }
    // Listen
    if (listen(listener, 10) == -1) {
        return -1;
    }
    return listener;
}

void handle_recv (void* obj, int fd) {

}

void handle_new_connection (void* obj, int fd) {
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof(remoteaddr);
    // Newly accept()ed socket descriptor
    int newfd = accept(fd, (struct sockaddr *)&remoteaddr, &addrlen);

    if (newfd == -1) {
        perror("accept");
    }

    addFd(obj, newfd, handle_recv);
}

/**
 *  Run busy loop to handle clients.
 * @return
 */
void* runServer (void* obj) {
    P_Reactor reac = (P_Reactor) obj;

    // Listening socket descriptor
    int listener;
    int newfd;
    // Client address
    socklen_t addrlen;

    // Buffer for client data.
    char buf[DATA_LEN];

    char remoteIP[INET6_ADDRSTRLEN];

    // Set up and get a listening socket
    listener = get_listener_socket();
    if (listener == -1) {
        fprintf(stderr, "(-) Error in getting listening socket!\n");
        exit(EXIT_FAILURE);
    }

    // Add the listener to set
    addFd(reac, listener, handle_new_connection);

    return obj;
}
