all: main unit_tests pyslat.so doc 

clean:
	rm -f *.so *.o main unit_tests

CC=g++
CFLAGS=-g -Wall -Werror -fbounds-check -Warray-bounds -std=gnu++11 -DBOOST_ALL_DYN_LINK -fPIC
LDFLAGS=-lgsl -lgslcblas -lm -lboost_log -lboost_thread -lboost_system -lpthread
# Uncomment the next line to add the current directory to the search path. This is *NOT*
# generally recommended, but saves on from specifying 'LD_LIBRARY_PATH=.':
#LDFLAGS+=-Wl,-rpath,.

LIBSRCS=functions.cpp relationships.cpp maq.cpp fragility.cpp lognormaldist.cpp loss_functions.cpp \
	comp_group.cpp caching.cpp
LIBOBJS=$(LIBSRCS:.cpp=.o)

UNIT_SRCS = unit_test.cpp functions_test.cpp relationships_test.cpp maq_test.cpp \
	fragility_test.cpp lognormaldist_test.cpp comp_group_test.cpp
UNIT_OBJS = $(UNIT_SRCS:.cpp=.o)

caching.o: caching.cpp caching.h 
	$(CC) -c $(CFLAGS) -o $@ $<
functions.o: functions.cpp functions.h replaceable.h
	$(CC) -c $(CFLAGS) -o $@ $<
relationships.o: relationships.cpp relationships.h functions.h maq.h replaceable.h caching.h
	$(CC) -c $(CFLAGS) -o $@ $<
maq.o: maq.cpp maq.h 
	$(CC) -c $(CFLAGS) -o $@ $<
fragility.o: fragility.cpp fragility.h 
	$(CC) -c $(CFLAGS) -o $@ $<
lognormaldist.o: lognormaldist.cpp lognormaldist.h 
	$(CC) -c $(CFLAGS) -o $@ $<
loss_functions.o: loss_functions.cpp loss_functions.h 
	$(CC) -c $(CFLAGS) -o $@ $<
comp_group.o: comp_group.cpp comp_group.h 
	$(CC) -c $(CFLAGS) -o $@ $<
libslat.so: functions.o relationships.o maq.o fragility.o lognormaldist.o loss_functions.o comp_group.o caching.o
	$(CC) -fPIC -shared -Wl,-soname,libslat.so -o libslat.so $(LIBOBJS) ${LDFLAGS}

main.o: main.cpp functions.h relationships.h maq.h libslat.so replaceable.h fragility.h lognormaldist.h loss_functions.h
	$(CC) -c $(CFLAGS) -o $@ $<
main: main.o libslat.so
	$(CC) -fPIC main.o -L. -lslat -o main ${LDFLAGS}

functions_test.o: functions_test.cpp functions.h replaceable.h
	$(CC) -c $(CFLAGS) -o $@ $<
relationships_test.o: relationships_test.cpp relationships.h functions.h replaceable.h
	$(CC) -c $(CFLAGS) -o $@ $<
maq_test.o: maq_test.cpp maq.h relationships.h functions.h replaceable.h
	$(CC) -c $(CFLAGS) -o $@ $<
fragility_test.o: fragility_test.cpp fragility.h
	$(CC) -c $(CFLAGS) -o $@ $<
comp_group_test.o: comp_group_test.cpp comp_group.h 
	$(CC) -c $(CFLAGS) -o $@ $<
unit_test.o: unit_test.cpp
	$(CC) -c $(CFLAGS) -o $@ $<
unit_tests: unit_test.o $(UNIT_OBJS) libslat.so 
	$(CC) -fPIC $(UNIT_OBJS) -L. -lslat -o unit_tests ${LDFLAGS} -lboost_unit_test_framework

pyslat.o: pyslat.cpp functions.h relationships.h replaceable.h
	$(CC) -c $(CFLAGS) -o $@ $< -I/usr/include/python3.4m
pyslat.so: pyslat.o libslat.so
	$(CC) -fPIC -shared -Wl,-soname,pyslat.so -o pyslat.so pyslat.o ${LDFLAGS} -L. -lslat -lpython3.4m -lboost_python-py34

doc: $(OBJS) $(HEADERS)
	doxygen
