ifeq ($(shell uname), Linux)
all: main unit_tests pyslat.so doc total_loss_debug interp

clean:
	rm -f *.a *.o *.so main unit_tests total_loss_debug

ANTLR=java -jar /usr/local/lib/antlr-4.5.2-complete.jar -Dlanguage=Python3
else
all: main unit_tests pyslat.pyd doc interp

clean:
	rm -f *.a *.o *.dll main unit_tests
ANTLR=java -jar ../../antlr-4.5.3-complete.jar -Dlanguage=Python3
endif

CFLAGS=-g -Wall -Werror -fbounds-check -Warray-bounds -std=gnu++11 -DBOOST_ALL_DYN_LINK

ifeq ($(shell uname), Linux)
	# Linux Build
	CC=g++
	CFLAGS +=  -fPIC `pkg-config --cflags gsl`

	LDFLAGS=-lboost_log -lboost_thread -lboost_system -lpthread 
	LDFLAGS += `pkg-config --libs gsl`
else
	# MING build (presumably)
	PYVER=3.5m
	PYINC=/mingw64/include/python$(PYVER)
	PYLIB=python$(PYVER)

	CC=/mingw64/bin/g++

	CFLAGS += -shared-libgcc \
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
endif

# Uncomment the next line to add the current directory to the search path. This is *NOT*
# generally recommended, but saves on from specifying 'LD_LIBRARY_PATH=.':
#LDFLAGS+=-Wl,-rpath,.

LIBSRCS=functions.cpp relationships.cpp maq.cpp fragility.cpp lognormaldist.cpp loss_functions.cpp \
	comp_group.cpp caching.cpp structure.cpp
LIBHDRS=$(LIBSRCS:.cpp=.h)
LIBOBJS=$(LIBSRCS:.cpp=.o)

UNIT_SRCS = unit_test.cpp functions_test.cpp relationships_test.cpp maq_test.cpp \
	fragility_test.cpp lognormaldist_test.cpp comp_group_test.cpp structure_test.cpp
UNIT_OBJS = $(UNIT_SRCS:.cpp=.o)

%.o : %.cpp
	echo $(PLATFORM)
	$(CC) -c $(CFLAGS) $< -o $@
caching.o: caching.cpp caching.h 
functions.o: functions.cpp functions.h replaceable.h
relationships.o: relationships.cpp relationships.h functions.h maq.h replaceable.h caching.h
maq.o: maq.cpp maq.h 
fragility.o: fragility.cpp fragility.h 
lognormaldist.o: lognormaldist.cpp lognormaldist.h 
loss_functions.o: loss_functions.cpp loss_functions.h 
comp_group.o: comp_group.cpp comp_group.h
structure.o: structure.cpp structure.h

ifeq ($(shell uname), Linux)
libslat.so: functions.o relationships.o maq.o fragility.o lognormaldist.o loss_functions.o comp_group.o caching.o structure.o
	$(CC) -fPIC -shared -Wl,-soname,libslat.so -o libslat.so $(LIBOBJS) ${LDFLAGS}
else
libslat.dll: functions.o relationships.o maq.o fragility.o lognormaldist.o loss_functions.o comp_group.o caching.o structure.o
	$(CC) -shared \
	-Wl,--dll \
	-Wl,--export-all-symbols \
	-Wl,--out-implib,libslat.a \
	-o libslat.dll \
	$(LIBOBJS) \
	$(LDFLAGS)
endif

main.o: main.cpp functions.h relationships.h maq.h replaceable.h fragility.h lognormaldist.h loss_functions.h
total_loss_debug.o: total_loss_debug.cpp functions.h relationships.h maq.h replaceable.h fragility.h lognormaldist.h loss_functions.h

ifeq ($(shell uname), Linux)
main: main.o libslat.so
	$(CC) -fPIC main.o -L. -lslat -o main ${LDFLAGS}
total_loss_debug: total_loss_debug.o libslat.so
	$(CC) -fPIC total_loss_debug.o -L. -lslat -o total_loss_debug ${LDFLAGS}
else
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
endif

functions_test.o: functions_test.cpp functions.h replaceable.h
relationships_test.o: relationships_test.cpp relationships.h functions.h replaceable.h
maq_test.o: maq_test.cpp maq.h relationships.h functions.h replaceable.h
fragility_test.o: fragility_test.cpp fragility.h
comp_group_test.o: comp_group_test.cpp comp_group.h 
structure_test.o: structure_test.cpp structure.h
unit_test.o: unit_test.cpp

ifeq ($(shell uname), Linux)
unit_tests: unit_test.o $(UNIT_OBJS) libslat.so 
	$(CC) -fPIC $(UNIT_OBJS) -L. -lslat -o unit_tests ${LDFLAGS} -lboost_unit_test_framework
else
unit_tests: unit_test.o $(UNIT_OBJS) libslat.dll
	$(CC) $(UNIT_OBJS) -L. -lslat -o unit_tests ${LDFLAGS} -lboost_unit_test_framework-mt
endif


ifeq ($(shell uname), Linux)
pyslat.o: pyslat.cpp functions.h relationships.h replaceable.h
	$(CC) -c $(CFLAGS) `pkg-config --cflags python3` -o $@ $<
pyslat.so: pyslat.o libslat.so
	$(CC) -fPIC -shared -Wl,-soname,pyslat.so -o pyslat.so pyslat.o ${LDFLAGS} -L. -lslat \
	`pkg-config --libs python3` -lboost_python-py34
else
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
endif

# pyslat.pyd: pyslat.o libslat.dll
# 	$(CC) -shared pyslat.o \
# 	-shared -o pyslat.pyd \
# 	-Wl,--dll \
# 	-Wl,--export-all-symbols \
# 	-Wl,--out-implib,pyslat.a \
# 	-L. -lslat \
# 	${LDFLAGS} \
#         -L$(PYLIB) \
# 	-lpython3.5 -lboost_python3-mt

doc: $(OBJS) $(HEADERS)
	doxygen

interp: parser/slatLexer.py parser/slatParser.py

parser/slatParser.py: parser/slatParser.g4
	cd parser && $(ANTLR) slatParser.g4

parser/slatLexer.py: parser/slatLexer.g4
	cd parser && $(ANTLR) slatLexer.g4
