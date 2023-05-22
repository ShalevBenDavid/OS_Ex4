// Created by Shalev Ben David and Ron Shuster.

#include "st_reactor.hpp"

using namespace std;

void* runServer (void* obj);

char buf[DATA_LEN]; // Global buffer to hold data from clients.
bool keep_alive = true; // Global bool to know when to stopn loop.

/**
 * Creates a reactor.
 * @return - A pointer to the new allocated reactor.
 */
void* createReactor () {
    P_Reactor reac = new Reactor();

    // Initialize values.
    reac -> is_alive = false;
    reac -> thread_id = 0;
    // Return pointer to the reactor.
    return reac;
}

/**
 * Stops the reactor if it's running. Else, does nothings.
 * @param obj - Pointer to the reactor.
 */
void stopReactor (void* obj) {
    P_Reactor reac = (P_Reactor) obj;

    // Check if the reactor is active.
    if (reac -> is_alive) {
        //Sending cancel request to thread.
        pthread_cancel(reac -> thread_id);
        // Wait for the reactor's thread to finish.
        pthread_join(reac -> thread_id, NULL);
        // Set alive status to false;
        reac -> is_alive = false;
        // Deleting all locations.
        wipe(reac);

        cout << "(+) Stopped reactor." << endl;
    }
}

/**
 * Starts a thread for the reactor. The thread will run in infinite loop with poll.
 * @param obj - Pointer to the reactor.
 */
void startReactor (void* obj) {
    P_Reactor reac = (P_Reactor) obj;

    // If reactor already running, return.
    if (reac -> is_alive) {
        cout << "(-) Reactor already running!" << endl;
        return;
    }
    // Begin single thread.
    int result = pthread_create(&reac -> thread_id, NULL, runServer, reac);
    // If running thread failed, exit.
    if (result) {
        cout << "(-) Creating thread failed!" << endl;
        return;
    } else {
        cout << "(+) Started reactor successfully." << endl;
    }
    // Set alive status to true;
    reac -> is_alive = true;
    // Call wait on thread.
    WaitFor(reac);
}

/**
 * Add a file descriptor to the reactor.
 * @param obj - A pointer to the reactor.
 * @param fd - The file descriptor we want to add.
 * @param handler - The function we match to this file descriptor.
 */
void addFd (void* obj, int fd, handler_t handler) {
    P_Reactor reac = (P_Reactor) obj;

    // Create allocation for the file descriptor.
    P_FD new_fd = new FD();

    // Set the fd and event handler to the fd pointer.
    new_fd -> file_descriptor = fd;
    new_fd -> event_handler = handler;

    // Create the poll struct for this fd.
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN; // Report ready to read on incoming connection

    // Update the reactor (insert fd and poll).
    reac -> file_descriptors.push_back(new_fd);
    reac -> poll_fds.push_back(pfd);
}

/**
 * Deletes a file descriptor from reactor, if exist.
 * @param obj - Pointer to the reactor.
 * @param fd - The file descriptor we want to remove.
 */
void deleteFD (void* obj, int fd) {
    P_Reactor reac = (P_Reactor) obj;

    // Only if alive.
    if (reac -> is_alive) {
        // Locate and delete file descriptors.
        for (size_t i = 0; i < reac -> file_descriptors.size(); i++) {
            if (reac -> file_descriptors.at(i) -> file_descriptor == fd) {
                delete reac -> file_descriptors.at(i);
                reac -> file_descriptors.erase(reac -> file_descriptors.begin() + i);
                reac -> poll_fds.erase(reac -> poll_fds.begin() + i);
            }
        }
    }
}

/**
 * Wait on pthread_join(3) until the reactor's thread will end.
 * @param obj - Pointer to the reactor.
 */
void WaitFor (void* obj) {
    P_Reactor reac = (P_Reactor) obj;

    // Check first that the reactor's thread is alive.
    if (reac -> is_alive) {
        cout << "(*) Waiting for thread..." << endl;
        // Wait for the reactor's thread to finish.
        pthread_join(reac -> thread_id, NULL);
    }
}

// ----------------------> My Functions <-------------------------

/**
 * @param sa -The socket address struct.
 * @return - A sockaddr (IPv4 or IPv6).
 */
void* get_in_addr (struct sockaddr* sa) {
    if (sa -> sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa) -> sin_addr);
    }
    return &(((struct sockaddr_in6*) sa) -> sin6_addr);
}

// Return a listening socket
int get_listener_socket (void* obj) {
    P_Reactor reac = (P_Reactor) obj;

    int listener = -1; // Listening socket descriptor
    int yes = 1;  // For setsockopt() SO_REUSEADDR, below
    int rv = 0;

    struct addrinfo hints = {};
    struct addrinfo *ai, *p = nullptr;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai))) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        wipe(reac);
        exit(EXIT_FAILURE);
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

/**
 * Receive message on listener socket from clients.
 * @param obj - Pointer to the reactor.
 * @param fd - The client socket on which we receive message.
 */
void handle_recv (void* obj, int fd) {
    P_Reactor reac = (P_Reactor) obj;

    // Receive message from client.
    int received_bytes = recv(fd, buf, DATA_LEN, 0);
    buf[received_bytes] = '\0';

    // Got error or connection closed by client.
    if (received_bytes <= 0) {
        if (received_bytes == 0) {
            cout << "(=) Socket " << fd << " hung up!" << endl;
        } else {
            cout << "(-) Failed to receive message on socket " << fd << "." << endl;
        }

        // Either way, we'll close the connection and remove client.
        close(fd);
        deleteFD(reac, fd);
    }
    // We got data from the client.
    else {
        // Iterate over clients.
        for (size_t j = 1; j < reac -> file_descriptors.size(); j++) {
            int dest_fd = reac -> file_descriptors.at(j) -> file_descriptor;
            // Send to everyone except the listener and the sender.
            if (dest_fd != fd) {
                if (send(dest_fd, buf, received_bytes, 0) == -1) {
                    cout << "(-) Error sending message." << endl;
                }
            }
        }
    }
}

/**
 * Receive a new connection on reactor.
 * @param obj - Pointer to the reactor.
 * @param fd - The listener socket on which we get a new connection.
 */
void handle_new_connection (void* obj, int fd) {
    // Client address.
    struct sockaddr_storage remote_addr = {};
    socklen_t addr_len = sizeof(remote_addr);
    char remoteIP[INET6_ADDRSTRLEN] = {0};

    // Newly accepted socket descriptor.
    int new_fd = accept(fd, (struct sockaddr *)&remote_addr, &addr_len);
    if (new_fd == -1) {
        cout << "(-) Failed to accept new client." << endl;
        return;
    }

    // Add socket descriptor to reactor.
    addFd(obj, new_fd, handle_recv);
    cout << "(=) New connection from " << inet_ntop(remote_addr.ss_family, get_in_addr((struct sockaddr*)&remote_addr),
            remoteIP, INET6_ADDRSTRLEN) << " on socket " << new_fd << "." << endl;
}

/**
 * Run busy loop to handle clients.
 */
void* runServer (void* obj) {
    P_Reactor reac = (P_Reactor) obj;

    // Set up and get a listening socket descriptor.
    int listener = get_listener_socket(reac);
    if (listener == -1) {
        fprintf(stderr, "(-) Error in getting listening socket!\n");
        wipe(reac);
        cout << "(+) Stopped reactor." << endl;
        return obj;
    }
    // Add the listener to set
    addFd(reac, listener, handle_new_connection);

    while (keep_alive) {
        int numEvents = poll(reac -> poll_fds.data(), reac -> poll_fds.size(), -1);
        if (numEvents == -1) {
            cout << "(-) Poll() failed.\n";
            keep_alive = false;
        }

        // Run through the existing connections looking for data to read
        for (size_t i = 0; i < reac -> file_descriptors.size(); i++) {
            // Check if someone's ready to read
            if (reac -> poll_fds.at(i).revents & POLLIN) {
                // Call appropriate function (new connection/recv).
                reac -> file_descriptors.at(i) ->
                    event_handler(reac, reac -> file_descriptors.at(i) -> file_descriptor);
                if (!keep_alive) { break; }
            }
        }
    }
    return obj;
}

/**
 * Wipe allocations for the reactor struct.
 * @param obj - Pointer to the reactor.
 */
void wipe (void* obj) {
    P_Reactor reac = (P_Reactor) obj;
    cout << "(*) Wiping reactor." << endl;

    // Clear file_descriptors.
    for (size_t i = 0; i < reac -> file_descriptors.size(); i++) {
        delete reac -> file_descriptors.at(i);
    }
    // Delete reactor.
    delete reac;
}