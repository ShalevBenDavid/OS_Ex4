// Created by Shalev Ben David and Ron Shuster.

#include "st_reactor.hpp"
#include <signal.h>
using namespace std;

P_Reactor reactor; // Global reactor.

// Signal handler function to clean up.
void signalHandler(int signal_num) {
    cout << "\n(*) Cleaning up..." << endl;
    // Stop the reactor.
    stopReactor(reactor);

    cout << "(*) Exiting." << endl;
    // Exit the program
    exit(signal_num);
}

int main() {
    // Register signal handlers for when exiting program.
    signal(SIGINT, signalHandler);   // Ctrl+C
    signal(SIGTSTP, signalHandler);  // Ctrl+Z

    // Create our reactor.
    reactor = (P_Reactor) createReactor();
    // Begin our thread to listen for clients.
    startReactor(reactor);

    cout << "(*) Exiting." << endl;

    return 0;
}