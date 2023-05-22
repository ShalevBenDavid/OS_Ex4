In this readme.txt we will explain how to run this exercise.

Instructions:
First of all, run the command "make" or "make all" in the terminal to generate all executable files.
In order to clean the files, run the command "make clean".
Since we compile the shared library "st_reactor.so" in the makefile,
please before running the program run the command: "export LD_LIBRARY_PATH=."
Same as beej, all message that are sent to the server are transfer to the client
and only system logs, errors, clients connecting and disconnecting and info as such will be shown in the server.

Running:
Please run first the server by entering the command: "./react_server".
For each client you wish to connect to the server, open a new terminal window and run the command:
"telnet 127.0.0.1 9034" or "nc 127.0.0.1 9034" or with the client from exercise 3 using "./stnc -c 127.0.0.1 9034"
If you wish to disconnect a specific client, shut down his terminal window.
If you wish to close the server, press ctrl+z or ctrl+c in the server window.

* Our server runs on port 9034 (same as beej's server).
* You may run with valgrind since all allocation were freed (unconditional jump may appear).

Questions we have asked ChatGPT:
-> "how can you cancel a thread?"
-> "what pthread_t holds?"
-> "how to close sockets and free space when force exiting program in c (ctrl+c or ctrl+z)?"
-> "how to delete a specific item from a vector?