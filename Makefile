.PHONY = all clean
#Defining Macros
CC = gcc
FLAGS = -Wall -g0
FPIC = -fpic
SH = -shared

all: pollserver

#Creating Programs (tools)
pollserver: pollserver.c
	$(CC) $(FLAGS) pollserver.c -o pollserver

#Creating Shared Library
st_reactor: st_reactor.c st_reactor.h
	$(CC) $(FLAGS) -o st_reactor.so $(SH) $(FPIC) codecA.c

clean:
	rm -f *.so pollserver