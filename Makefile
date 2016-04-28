all: main unit_tests pyslat.pyd doc 

clean:
	rm -f *.a *.o main unit_tests
#PYVER=35
#PYPATH=/c/Users/mag109/AppData/Local/Programs/Python/Python35
#PYINC=$(PYPATH)/include
#PYLIB=$(PYLIB)/lib
PYVER=3.5m
PYINC=/mingw64/include/python$(PYVER)
PYLIB=python$(PYVER)
CC=/mingw64/bin/g++
#GSLROOT=/c/Program\ Files\ \(x86\)/GnuWIN32
GSLROOT=/usr/local
CFLAGS= -g -Wall -fbounds-check -Warray-bounds -std=gnu++11 -DBOOST_ALL_DYN_LINK \
	-shared-libgcc \
	-D__x86_64__ \
	-I$(PYINC) \
	-I/mingw64/include \
	-I/usr/local/include

LDFLAGS=-L$(PYLIB) \
	-L/mingw64/lib \
	-lpython$(PYVER) \
	-L/usr/local/lib \
	-L. \
	-lgsl.dll -lgslcblas.dll \
	-lboost_log-mt \
	-lboost_thread-mt \
	-lboost_system-mt \
	-lboost_filesystem-mt \
	-lboost_unit_test_framework-mt \
	-L/c/windows \
	 -lpthread -lm 
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
	-o libslat.dll \
	$(LIBOBJS) \
	$(LDFLAGS)

gsl_test: gsl_test.cpp 
	$(CC) -g -Wall -fbounds-check -Warray-bounds -std=gnu++11 \
	-static -D__int64=__int64_t \
	gsl_test.cpp \
	-I$(PYINC) \
        -lgsl.dll -lm \
	 -o gsl_test

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
	-lgsl.dll \
	-L. -lhello \

main.o: main.cpp functions.h relationships.h maq.h replaceable.h fragility.h lognormaldist.h loss_functions.h
	$(CC) -c $(CFLAGS) -o $@ $<
main: main.o libslat.dll
	$(CC) main.o -L. -o main \
	-lslat \
	-L/usr/local/lib \
	 $(CFLAGS) \
	-lgsl \
	-lboost_system-mt \
	-lboost_log-mt \
	-lboost_filesystem-mt \
	-lboost_log_setup-mt \
	-lpthread -lm \
	-lboost_thread-mt \
        -lboost_unit_test_framework-mt

main2: main.cpp $(LIBSRCS) $(LIBHDRS)
	$(CC) -static main.cpp $(LIBSRCS) \
	-o main2 \
	-Wl,--dll-verbose \
	-g -fbounds-check -Warray-bounds -std=gnu++11 \
	-I/usr/local/include \
	-I/usr/local/include/boost-1_60 \
	-I$(PYINC) \
	-L. \
	-lboost_system \
	-lboost_log \
	-lboost_filesystem \
	-lboost_log_setup \
	-lpthread -lm \
	-lboost_thread \
	 -lgsl -lgslcblas 


temp.o: temp.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

temp: temp.o
	$(CC)  -o temp 	temp.o \
	-L. \
	-L/mingw64/lib \
	-L/mingw64/lib/python3.5 \
	-L/usr/local/lib \
	-lboost_log-mt \
	-lboost_log_setup-mt \
	-lboost_thread-mt \
	-lboost_system-mt \
	-lboost_filesystem-mt \
	-lpython3.5 \
	# -L/usr/local/lib \
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
	$(CC) $(UNIT_OBJS) -L. -lslat -o unit_tests ${LDFLAGS} -lboost_unit_test_framework-mt

pyslat.o: pyslat.cpp $(LIBHDRS)
	$(CC) -c $(CFLAGS) -dD -o $@ $<
pyslat.pyd: pyslat.o libslat.dll
	$(CC) -shared pyslat.o \
	-shared -o pyslat.pyd \
	-Wl,--dll \
	-Wl,--export-all-symbols \
	-Wl,--out-implib,pyslat.a \
	-L. -lslat \
	${LDFLAGS} \
        -L$(PYLIB) \
	-lpython3.5 -lboost_python3-mt
# pyslat.o: 
# pyslat.pyd: pyslat.cpp $(LIBSRCS) $(LIBHDRS)
# 	$(CC) -shared $(CFLAGS) pyslat.cpp \
# 	$(LIBOBJS) \
# 	-I/usr/include/python3.4m \
# 	-shared -o pyslat.pyd \
# 	${LDFLAGS} \
# 	-Wl,--dll \
# 	-Wl,--export-all-symbols \
# 	-Wl,--out-implib,pyslat.a \
# 	-lpython34 -lboost_python3

doc: $(OBJS) $(HEADERS)
	doxygen

