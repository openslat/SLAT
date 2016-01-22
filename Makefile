all: main unit_tests pyslat.so doc 

clean:
	rm -f *.so *.o main unit_tests

CC=g++
CFLAGS=-g -Wall -Werror -fbounds-check -Warray-bounds -std=gnu++11 -DBOOST_ALL_DYN_LINK -fPIC
LDFLAGS=-lgsl -lgslcblas -lm -lboost_log -lboost_thread -lboost_system -lpthread
# Uncomment the next line to add the current directory to the search path. This is *NOT*
# generally recommended, but saves on from specifying 'LD_LIBRARY_PATH=.':
#LDFLAGS+=-Wl,-rpath,.

LIBSRCS=functions.cpp relationships.cpp maq.cpp fragility.cpp
LIBOBJS=$(LIBSRCS:.cpp=.o)

UNIT_SRCS = functions_test.cpp relationships_test.cpp unit_test.cpp maq_test.cpp \
	fragility_test.cpp
UNIT_OBJS = $(UNIT_SRCS:.cpp=.o)

functions.o: functions.cpp functions.h replaceable.h
	g++ -c $(CFLAGS) -o $@ $<
relationships.o: relationships.cpp relationships.h functions.h maq.h replaceable.h
	g++ -c $(CFLAGS) -o $@ $<
maq.o: maq.cpp maq.h 
	g++ -c $(CFLAGS) -o $@ $<
fragility.o: fragility.cpp fragility.h 
	g++ -c $(CFLAGS) -o $@ $<
libslat.so: functions.o relationships.o maq.o fragility.o
	g++ -fPIC -shared -Wl,-soname,libslat.so -o libslat.so $(LIBOBJS) ${LDFLAGS}

main.o: main.cpp functions.h relationships.h maq.h libslat.so replaceable.h fragility.h
	g++ -c $(CFLAGS) -o $@ $<
main: main.o libslat.so
	g++ -fPIC main.o -L. -lslat -o main ${LDFLAGS}

functions_test.o: functions_test.cpp functions.h replaceable.h
	g++ -c $(CFLAGS) -o $@ $<
relationships_test.o: relationships_test.cpp relationships.h functions.h replaceable.h
	g++ -c $(CFLAGS) -o $@ $<
maq_test.o: maq_test.cpp maq.h relationships.h functions.h replaceable.h
	g++ -c $(CFLAGS) -o $@ $<
fragility_test.o: fragility_test.cpp fragility.h
	g++ -c $(CFLAGS) -o $@ $<
unit_test.o: unit_test.cpp
	g++ -c $(CFLAGS) -o $@ $<
unit_tests: unit_test.o $(UNIT_OBJS) libslat.so 
	g++ -fPIC $(UNIT_OBJS) -L. -lslat -o unit_tests ${LDFLAGS} -lboost_unit_test_framework

pyslat.o: pyslat.cpp functions.h relationships.h replaceable.h
	g++ -c $(CFLAGS) -o $@ $< -I/usr/include/python3.4m
pyslat.so: pyslat.o libslat.so
	g++ -fPIC -shared -Wl,-soname,pyslat.so -o pyslat.so pyslat.o ${LDFLAGS} -L. -lslat -lpython3.4m -lboost_python-py34

doc: $(OBJS) $(HEADERS)
	doxygen
