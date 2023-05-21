// Created by Shalev Ben David and Ron Shuster.

// Signal handler function to clean up.
void signalHandler(int signal_num) {
    printf("(*) Cleaning up...\n");


    // Exit the program
    exit(signal_num);
}

int main() {
//     // Load the dynamic library.
//     char* lib_name = "./st_reactor.so";
//     void* lib = dlopen(lib_name, RTLD_NOW);
//     // Print in case of an error.
//     if (!(lib)) {
//         printf("(-) Error in opening library: %s\n", dlerror());
//         exit(EXIT_FAILURE);
//     }


    // Register signal handlers for when exiting program.
    signal(SIGINT, signalHandler);   // Ctrl+C
    signal(SIGTSTP, signalHandler);  // Ctrl+Z
}