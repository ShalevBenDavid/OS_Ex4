.PHONY = all clean
#Defining Macros
CXX = g++
FLAGS = -Wall -g0
FPIC = -fpic
SH = -shared

all: st_reactor react_server

#Creating Programs (tools)
react_server: react_server.cpp
	$(CXX) $(FLAGS) react_server.cpp -L. -l st_reactor -o react_server

#Creating Shared Library
st_reactor: st_reactor.cpp st_reactor.hpp
	$(CXX) $(FLAGS) -o libst_reactor.so $(SH) $(FPIC) st_reactor.cpp

clean:
	rm -f *.so st_reactor react_server