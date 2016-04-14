all: main unit_tests pyslat.pyd doc 

clean:
	rm -f *.a *.o main unit_tests

CC=g++
#GSLROOT=/c/Program\ Files\ \(x86\)/GnuWIN32
GSLROOT=/usr/local
CFLAGS=-g -Wall -fbounds-check -Warray-bounds -std=gnu++11 -DBOOST_ALL_DYN_LINK \
	-shared-libgcc \
	-I/usr/local/include \
	-I$(GSLROOT)/include \
	-I/usr/local/include/boost-1_60 \
	-I/c/Python34/include

LDFLAGS=-L/usr/local/lib \
	-L$(GSLROOT)/lib \
	-L. \
	-lgsl.dll -lgslcblas.dll \
	-lgcc_s \
	-lboost_log \
	-lboost_thread \
	-lboost_system \
	-lboost_filesystem \
	-lboost_unit_test_framework \
	-L/c/windows \
	-L/c/Python34/libs \
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
	$(CC) -g -Wall -Werror -fbounds-check -Warray-bounds -std=gnu++11 \
	-static \
	gsl_test.cpp \
	-I$(GSLROOT)/include \
	-L$(GSLROOT)/lib \
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
	-L$(GSLROOT)/lib -lgsl.dll \
	-L. -lhello \

main.o: main.cpp functions.h relationships.h maq.h replaceable.h fragility.h lognormaldist.h loss_functions.h
	$(CC) -c $(CFLAGS) -o $@ $<
main: main.o libslat.dll
	$(CC) main.o -L. -o main \
	-lslat \
	-L/usr/local/lib \
	 $(CFLAGS) \
	-L$(GSLROOT)/lib -lgsl \
	-lboost_system \
	-lboost_log \
	-lboost_filesystem \
	-lboost_log_setup \
	-lpthread -lm \
	-lboost_thread \
        -lboost_unit_test_framework

main2: main.cpp $(LIBSRCS) $(LIBHDRS)
	$(CC) -static main.cpp $(LIBSRCS) \
	-o main2 \
	-Wl,--dll-verbose \
	-g -fbounds-check -Warray-bounds -std=gnu++11 \
	-I/usr/local/include \
	-I$(GSLROOT)/include \
	-I/usr/local/include/boost-1_60 \
	-I/c/Python34/include \
	-L. \
	-L$(GSLROOT)/lib \
	-lboost_system \
	-lboost_log \
	-lboost_filesystem \
	-lboost_log_setup \
	-lpthread -lm \
	-lboost_thread \
	 -lgsl -lgslcblas 


temp.o: temp.cpp
	$(CC) -c -g -fbounds-check -Warray-bounds -std=gnu++11 \
	-DBOOST_ALL_DYN_LINK \
	-DBOOST_LOG_USE_NATIVE_SYSLOG \
	-I/usr/local/include \
	-I/usr/local/include/boost-1_60 \
	-I/c/Python34/include \
	-o $@ $< \

temp: temp.o
	$(CC)  -o temp 	temp.o \
	-L. \
	-L/usr/local/lib \
	-lboost_log \
	-lboost_log_setup \
	-lboost_thread \
	-lboost_system \
	-lboost_filesystem \
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
	$(CC) $(UNIT_OBJS) -L. -lslat -o unit_tests ${LDFLAGS} -lboost_unit_test_framework

pyslat.o: pyslat.cpp $(LIBHDRS)
	$(CC) -c $(CFLAGS) -o $@ $<
pyslat.pyd: pyslat.o libslat.dll
	$(CC) -shared pyslat.o \
	-shared -o pyslat.pyd \
	-I/usr/include/python3.4m \
	-Wl,--dll \
	-Wl,--export-all-symbols \
	-Wl,--out-implib,pyslat.a \
	-L. -lslat \
	${LDFLAGS} \
	-lpython34 -lboost_python3
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

