.PHONY = all clean
#Defining Macros
CC = gcc
FLAGS = -Wall -g0
FPIC = -fpic
SH = -shared

all: pollserver st_reactor react_server

#Creating Programs (tools)
pollserver: pollserver.c
	$(CC) $(FLAGS) pollserver.c -o pollserver
react_server: react_server.c
	$(CC) $(FLAGS) react_server.c -o react_server

#Creating Shared Library
st_reactor: st_reactor.c st_reactor.h
	$(CC) $(FLAGS) -o st_reactor.so $(SH) $(FPIC) st_reactor.c

clean:
	rm -f *.so pollserver st_reactor react_server