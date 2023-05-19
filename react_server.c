// Created by Shalev Ben David and Ron Shuster.

// Signal handler function to clean up.
void signalHandler(int signal_num) {
    printf("(*) Cleaning up...\n");


    // Exit the program
    exit(signal_num);
}

int main() {

    // Register signal handlers for when exiting program.
    signal(SIGINT, signalHandler);   // Ctrl+C
    signal(SIGTSTP, signalHandler);  // Ctrl+Z
}