all: main unit_tests doc

clean:
	rm -f *.o main

CC=g++
CFLAGS=-g -fbounds-check -Warray-bounds -std=gnu++11 -DBOOST_ALL_DYN_LINK
LDFLAGS=-lgsl -lgslcblas -lm -lboost_log -lboost_thread ./-lboost_system -lpthread

SRCS = main.cpp functions.cpp relationships.cpp maq.cpp
HEADERS = functions.h
OBJS = $(SRCS:.cpp=.o)
UNIT_SRCS = functions_test.cpp functions.cpp relationships_test.cpp relationships.cpp \
    maq.cpp unit_test.cpp maq_test.cpp
UNIT_OBJS = $(UNIT_SRCS:.cpp=.o)

main: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 

functions.o: functions.cpp functions.h

maq.o: maq.cpp maq.h

relationships.o: relationships.cpp relationships.h functions.h maq.h

.SUFFIXES: .cpp

.cpp.o:
	$(CC) $(CFLAGS) -c -o $@ $<

unit_tests: $(UNIT_OBJS)
	$(CC) -o $@ $(UNIT_OBJS) $(LDFLAGS) -lboost_unit_test_framework

doc: $(OBJS) $(HEADERS)
	doxygen


