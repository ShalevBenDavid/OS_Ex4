// Created by Shalev Ben David and Ron Shuster.

#include "st_reactor.hpp"
#include <signal.h>

// Signal handler function to clean up.
void signalHandler(int signal_num, P_Reactor reactor) {
    printf("(*) Cleaning up...\n");
    // Stop the reactor.
    stopReactor(reactor);
    // Exit the program
    exit(signal_num);
}

int main() {
    // Create our reactor.
    P_Reactor reactor = createReactor();
    // Begin our thread to listen for clients.
    startReactor (reactor);
    // Register signal handlers for when exiting program.
    signal(SIGINT, signalHandler);   // Ctrl+C
    signal(SIGTSTP, signalHandler);  // Ctrl+Z
}