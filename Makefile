all: main unit_tests pyslat.a doc 

clean:
	rm -f *.a *.o main unit_tests

CC=g++
CFLAGS=-g -fbounds-check -Warray-bounds -std=gnu++11 -DBOOST_ALL_DYN_LINK \
	-I/c/Program\ Files\ \(x86\)/GnuWin32/include \
	-I/usr/local/include/boost-1_60 \
	-I/c/Python34/include
LDFLAGS=-L/usr/local/lib \
	-lboost_log-mgw49-mt-1_60 \
	-lboost_thread-mgw49-mt-1_60 \
	 -lboost_system-mgw49-mt-1_60 \
	-lboost_filesystem-mgw49-mt-1_60 \
	-lboost_unit_test_framework-mgw49-mt-1_60 \
	-L/c/Program\ Files\ \(x86\)/GnuWin32/lib \
	-L/c/windows \
	-L/c/Python34/libs \
	 -lpthread -lm \
	-lgsl -lgslcblas 
# Uncomment the next line to add the current directory to the search path. This is *NOT*
# generally recommended, but saves on from specifying 'LD_LIBRARY_PATH=.':
#LDFLAGS+=-Wl,-rpath,.

LIBSRCS=functions.cpp relationships.cpp maq.cpp fragility.cpp lognormaldist.cpp loss_functions.cpp \
	comp_group.cpp caching.cpp
LIBHDRS=$(LIBSRCS:.cpp=.h)
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
libslat.dll: functions.o relationships.o maq.o fragility.o lognormaldist.o loss_functions.o comp_group.o caching.o
	$(CC) -shared \
	-Wl,--dll \
	-Wl,--export-all-symbols \
	-Wl,--out-implib,libslat.a \
	-o libslat.dll $(LIBOBJS) $(LDFLAGS)


hello.o: hello.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

libhello.dll: hello.o
	$(CC) -shared \
	-Wl,--dll \
	-Wl,--export-all-symbols \
	-Wl,--out-implib,libhello.a \
	-o libhello.dll hello.o $(LDFLAGS)
hellomain.o: hellomain.cpp hello.h
	$(CC) -c $(CFLAGS) -o $@ $<
hellomain: hellomain.o libhello.dll
	$(CC) hellomain.o -o hellomain \
	-L. -lhello \
	-L/c/Program\ Files\ \(x86\)/GnuWin32/lib -lgsl -lgslcblas

main.o: main.cpp functions.h relationships.h maq.h replaceable.h fragility.h lognormaldist.h loss_functions.h
	$(CC) -c $(CFLAGS) -o $@ $<
main: main.o libslat.dll
	$(CC) main.o -L. -o main \
	-lslat \
	-L/usr/local/lib \
	-L/c/Program\ Files\ \(x86\)/GnuWin32/lib \
	-lboost_system-mgw49-mt-1_60 \
	-lboost_log-mgw49-mt-1_60 \
	-lboost_filesystem-mgw49-mt-1_60 \
	-lboost_log_setup-mgw49-mt-1_60 \
	-lpthread -lm \
	-lboost_thread-mgw49-mt-1_60 \
	 -lboost_unit_test_framework-mgw49-mt-1_60 \
	 -lgsl -lgslcblas 

main2: main.cpp $(LIBSRCS) $(LIBHDRS)
	$(CC) -static main.cpp $(LIBSRCS) \
	-o main2 \
	-Wl,--dll-verbose \
	-g -fbounds-check -Warray-bounds -std=gnu++11 \
	-I/c/Program\ Files\ \(x86\)/GnuWin32/include \
	-I/usr/local/include/boost-1_60 \
	-I/c/Python34/include \
	-L. \
	-L/usr/local/lib \
	-L/c/Program\ Files\ \(x86\)/GnuWin32/lib \
	-lboost_system-mgw49-mt-s-1_60 \
	-lboost_log-mgw49-mt-s-1_60 \
	-lboost_filesystem-mgw49-mt-s-1_60 \
	-lboost_log_setup-mgw49-mt-s-1_60 \
	-lpthread -lm \
	-lboost_thread-mgw49-mt-s-1_60 \
	 -lgsl -lgslcblas 


temp.o: temp.cpp
	$(CC) -c -g -fbounds-check -Warray-bounds -std=gnu++11 \
	-DBOOST_ALL_DYN_LINK \
	-DBOOST_LOG_USE_NATIVE_SYSLOG \
	-I/c/Program\ Files\ \(x86\)/GnuWin32/include \
	-I/usr/local/include/boost-1_60 \
	-I/c/Python34/include \
	-o $@ $< \

temp: temp.o
	$(CC)  -o temp 	temp.o \
	-L. \
	-L/usr/local/lib \
	-lboost_log-mgw49-mt-1_60 \
	-lboost_log_setup-mgw49-mt-1_60 \
	-lboost_thread-mgw49-mt-1_60 \
	-lboost_system-mgw49-mt-1_60 \
	-lboost_filesystem-mgw49-mt-1_60 \
	# -L/c/Program\ Files\ \(x86\)/GnuWin32/lib \
	#   -lpthread -lm \
	#  -lgsl -lgslcblas 


functions_test.o: functions_test.cpp functions.h replaceable.h
	$(CC) -c $(CFLAGS) -o $@ $<
lognormaldist_test.o: lognormaldist_test.cpp lognormaldist.h 
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
unit_tests: unit_test.o $(UNIT_OBJS) libslat.a 
	$(CC) $(UNIT_OBJS) -L. -lslat -o unit_tests ${LDFLAGS} -lboost_unit_test_framework-mgw49-mt-1_60

pyslat.o: 
pyslat.dll: pyslat.cpp $(LIBSRCS) $(LIBHDRS)
	$(CC) -shared $(CFLAGS) pyslat.cpp $(LIBSRCS) -I/usr/include/python3.4m \
	-shared -o pyslat.dll \
	${LDFLAGS} \
	-Wl,--dll \
	-Wl,--export-all-symbols \
	-Wl,--out-implib,pyslat.a \
	-L. -lpython34 -lboost_python3-mgw49-mt-1_60

doc: $(OBJS) $(HEADERS)
	doxygen

