CXX=g++
CFLAGS=-std=c++11 -g -O2 -Wall -fPIC 

INC=-I../include
LIB=-L../lib -lreactor
OBJS=$(addsufffix .o, $(basename $(wildcard *.cpp)))

all:
	$(CXX) -o hello_rlbs $(CFLAGS) hello_rlbs.cpp $(INC) $(LIB)
	$(CXX) -o rlbs_reactor_test $(CFLAGS) rlbs_reactor.cpp $(INC) $(LIB)

.PHONY:clean

clean:
	rm -rf *.o hello_rlbs rlbs_reactor_test
