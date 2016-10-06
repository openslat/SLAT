CC=/mingw64/bin/g++
CFLAGS=-g -O3 -fopenmp -Wall -std=gnu++11 -DBOOST_ALL_DYN_LINK
CFLAGS+=-I ../Python-3.5.2/PC -I../Python-3.5.2/Include
CFLAGS+=-DMS_WIN64

DLLFLAGS=-Wl,--dll -Wl,--export-all-symbols -Wl,--out-implib,_example.a 
LDFLAGS=-L/opt/mxe/usr/bin/x86_64-w64-mingw32.shared/bin -L/opt/mxe/usr/bin/x86_64-w64-mingw32.shared/lib 
LDFLAGS+=-L../python-amd64 -lpython35 -L.
LDFLAGS+=-lboost_log-mt -lboost_thread-mt -lboost_system-mt -lpthread
LDFLAGS+=-lboost_filesystem-mt -lboost_log_setup-mt -lboost_unit_test_framework-mt
LDFLAGS+=-lpthread -fopenmp -lgsl


all: main.exe unit_tests interp example2.exe _pyslatcore.pyd

clean:
	rm -f *.pyd *.a *.o *.so main.exe unit_tests total_loss_debug example2.exe

ANTLR=java -jar /usr/local/lib/antlr-4.5.2-complete.jar -Dlanguage=Python3

LIBSRCS=../functions.cpp ../relationships.cpp ../maq.cpp ../fragility.cpp ../lognormaldist.cpp ../loss_functions.cpp \
	../comp_group.cpp ../caching.cpp ../structure.cpp
LIBHDRS=$(LIBSRCS:.cpp=.h)
LIBOBJS=$(LIBSRCS:.cpp=.o)

UNIT_SRCS = unit_test.cpp functions_test.cpp relationships_test.cpp maq_test.cpp \
	fragility_test.cpp lognormaldist_test.cpp comp_group_test.cpp structure_test.cpp
UNIT_OBJS = $(UNIT_SRCS:.cpp=.o)

%.o : ../%.cpp
	echo $(PLATFORM)
	$(CC) -c $(CFLAGS) $< -o $@
caching.o: ../caching.cpp ../caching.h 
functions.o: ../functions.cpp ../functions.h ../replaceable.h
relationships.o: ../relationships.cpp ../relationships.h ../functions.h ../maq.h ../replaceable.h ../caching.h
maq.o: ../maq.cpp ../maq.h 
fragility.o: ../fragility.cpp ../fragility.h 
lognormaldist.o: ../lognormaldist.cpp ../lognormaldist.h 
loss_functions.o: ../loss_functions.cpp ../loss_functions.h 
comp_group.o: ../comp_group.cpp ../comp_group.h ../caching.h
structure.o: ../structure.cpp ../structure.h

libslat.dll: functions.o relationships.o maq.o fragility.o lognormaldist.o loss_functions.o comp_group.o caching.o structure.o
	$(CC) -shared \
	-Wl,--dll \
	-Wl,--export-all-symbols \
	-Wl,--out-implib,libslat.a \
	-o libslat.dll \
        functions.o relationships.o maq.o fragility.o lognormaldist.o loss_functions.o comp_group.o caching.o structure.o \
	$(LDFLAGS) \
#	$(LIBOBJS)

main.o: ../main.cpp ../functions.h ../relationships.h ../maq.h ../replaceable.h ../fragility.h ../lognormaldist.h ../loss_functions.h
total_loss_debug.o: ../total_loss_debug.cpp ../functions.h ../relationships.h ../maq.h ../replaceable.h ../fragility.h ../lognormaldist.h ../loss_functions.h

main.exe: main.o libslat.dll 
	$(CC) main.o -L. -o main.exe \
	-lslat \
	$(CFLAGS) \
	$(LDFLAGS) 

example2.exe: example2.o libslat.dll
	$(CC) example2.o -L. -o example2.exe \
	-lslat \
	 $(CFLAGS) \
	$(LDFLAGS)

functions_test.o: ../functions_test.cpp ../functions.h ../replaceable.h
relationships_test.o: ../relationships_test.cpp ../relationships.h ../functions.h ../replaceable.h
maq_test.o: ../maq_test.cpp ../maq.h ../relationships.h ../functions.h ../replaceable.h
fragility_test.o: ../fragility_test.cpp ../fragility.h
comp_group_test.o: ../comp_group_test.cpp ../comp_group.h 
structure_test.o: ../structure_test.cpp ../structure.h
unit_test.o: ../unit_test.cpp

unit_tests: unit_test.o $(UNIT_OBJS) libslat.dll
	$(CC) $(UNIT_OBJS) -L. -lslat -o unit_tests ${LDFLAGS} -lboost_unit_test_framework-mt

../wrap_pyslatcore.cpp: ../pyslatcore.h ../pyslatcore.i 
	swig -python -c++ -py3  -modern \
	-fastdispatch \
	-nosafecstrings \
	-fvirtual \
	-noproxydel \
	-fastproxy \
	-nofastinit \
	-fastunpack \
	-fastquery \
	-modernargs \
	-castmode \
	-nobuildnone \
	-o ../wrap_pyslatcore.cpp ../pyslatcore.i
wrap_pyslatcore.o: ../wrap_pyslatcore.cpp
pyslatcore.o: ../pyslatcore.cpp $(LIBHDRS)
	$(CC) -c $(CFLAGS) -dD -o $@ $<
_pyslatcore.pyd: pyslatcore.o wrap_pyslatcore.o libslat.dll
	$(CC) -shared wrap_pyslatcore.o pyslatcore.o \
	-shared -o _pyslatcore.pyd \
	-Wl,--dll \
	-Wl,--export-all-symbols \
	-Wl,--out-implib,pyslatcore.a \
	-L. -lslat \
	${LDFLAGS} \
        -L$(PYLIB) ../\
	-lpython3.5

doc: $(OBJS) ../$(HEADERS)
	doxygen

interp: ../parser/slatLexer.py ../parser/slatParser.py

../parser/slatParser.py: ../parser/slatParser.g4
	cd ../parser && $(ANTLR) slatParser.g4

../parser/slatLexer.py: ../parser/slatLexer.g4
	cd ../parser && $(ANTLR) slatLexer.g4

example2.o: ../example2.cpp ../functions.h ../relationships.h ../maq.h ../replaceable.h ../fragility.h ../lognormaldist.h ../loss_functions.h
cachetest.o: ../../cachetest.cpp ../caching.h
cachetest: ../cachetest.o ../caching.o
	$(CC)  ../cachetest.o ../caching.o -o ../cachetest ${LDFLAGS}
