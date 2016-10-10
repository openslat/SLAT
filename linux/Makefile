SRCDIR=../src
OBJDIR=obj
BINDIR=bin
LIBDIR=lib
SCRIPTDIR=scripts
ANTLR=java -jar /usr/local/lib/antlr-4.5.2-complete.jar -Dlanguage=Python3

CFLAGS=-g -O3 -fopenmp -Wall -Werror -fbounds-check -Warray-bounds -std=gnu++11 -DBOOST_ALL_DYN_LINK
CFLAGS+=`pkg-config --cflags python3`
CC=g++
CFLAGS +=  -fPIC `pkg-config --cflags gsl`
LDFLAGS=-lboost_log -lboost_thread -lboost_system -lpthread 
LDFLAGS += `pkg-config --libs gsl`
LDFLAGS += -lpthread -fopenmp
PYLIBFLAGS=-fPIC -shared -Wl,-soname,_pyslatcore.so `pkg-config --libs python3` `pkg-config --libs gsl`

# Uncomment the next line to add the current directory to the search path. This is *NOT*
# generally recommended, but saves on from specifying 'LD_LIBRARY_PATH=.':
#LDFLAGS+=-Wl,-rpath,.

LIBSRCS=functions.cpp relationships.cpp maq.cpp fragility.cpp lognormaldist.cpp loss_functions.cpp \
	comp_group.cpp caching.cpp structure.cpp
LIBHDRS=$(addprefix $(SRCDIR)/,$(LIBSRCS:.cpp=.h))
LIBOBJS=$(addprefix $(OBJDIR)/,$(LIBSRCS:.cpp=.o))

UNIT_SRCS = unit_test.cpp functions_test.cpp relationships_test.cpp maq_test.cpp \
	fragility_test.cpp lognormaldist_test.cpp comp_group_test.cpp structure_test.cpp
UNIT_OBJS = $(addprefix $(OBJDIR)/,$(UNIT_SRCS:.cpp=.o))

all: $(OBJDIR) $(BINDIR) $(LIBDIR) $(SCRIPTDIR) \
	$(BINDIR)/main $(BINDIR)/unit_tests $(LIBDIR)/_pyslatcore.so \
	$(BINDIR)/example2 interp $(LIBDIR)/pyslatcore.py

$(OBJDIR):
	mkdir $(OBJDIR)

$(BINDIR):
	mkdir $(BINDIR)

$(LIBDIR):
	mkdir $(LIBDIR)

$(SCRIPTDIR):
	mkdir $(SCRIPTDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR) $(LIBDIR) $(SCRIPTDIR)


$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/caching.o: $(SRCDIR)/caching.cpp $(SRCDIR)/caching.h 
$(OBJDIR)/functions.o: $(SRCDIR)/functions.cpp $(SRCDIR)/functions.h $(SRCDIR)/replaceable.h
$(OBJDIR)/relationships.o: $(SRCDIR)/relationships.cpp $(SRCDIR)/relationships.h $(SRCDIR)/functions.h $(SRCDIR)/maq.h $(SRCDIR)/replaceable.h $(SRCDIR)/caching.h
$(OBJDIR)/maq.o: $(SRCDIR)/maq.cpp $(SRCDIR)/maq.h 
$(OBJDIR)/fragility.o: $(SRCDIR)/fragility.cpp $(SRCDIR)/fragility.h 
$(OBJDIR)/lognormaldist.o: $(SRCDIR)/lognormaldist.cpp $(SRCDIR)/lognormaldist.h 
$(OBJDIR)/loss_functions.o: $(SRCDIR)/loss_functions.cpp $(SRCDIR)/loss_functions.h 
$(OBJDIR)/comp_group.o: $(SRCDIR)/comp_group.cpp $(SRCDIR)/comp_group.h $(SRCDIR)/caching.h
$(OBJDIR)/structure.o: $(SRCDIR)/structure.cpp $(SRCDIR)/structure.h

$(LIBDIR)/libslat.so: $(LIBOBJS)
	$(CC) -fPIC -shared -Wl,-soname,libslat.so -o $(LIBDIR)/libslat.so $(LIBOBJS) ${LDFLAGS}

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp $(SRCDIR)/functions.h $(SRCDIR)/relationships.h $(SRCDIR)/maq.h $(SRCDIR)/replaceable.h $(SRCDIR)/fragility.h $(SRCDIR)/lognormaldist.h $(SRCDIR)/loss_functions.h
$(OBJDIR)/total_loss_debug.o: $(SRCDIR)/total_loss_debug.cpp $(SRCDIR)/functions.h $(SRCDIR)/relationships.h $(SRCDIR)/maq.h $(SRCDIR)/replaceable.h $(SRCDIR)/fragility.h $(SRCDIR)/lognormaldist.h $(SRCDIR)/loss_functions.h

$(BINDIR)/main: $(OBJDIR)/main.o $(LIBDIR)/libslat.so
	$(CC) -fPIC $(OBJDIR)/main.o -L$(LIBDIR) -lslat -o $(BINDIR)/main ${LDFLAGS}
$(BINDIR)/example2: $(OBJDIR)/example2.o $(LIBDIR)/libslat.so
	$(CC) -fPIC $(OBJDIR)/example2.o -L$(LIBDIR) -lslat -o $(BINDIR)/example2 ${LDFLAGS}

$(OBJDIR)/functions_test.o: $(SRCDIR)/functions_test.cpp $(SRCDIR)/functions.h $(SRCDIR)/replaceable.h
$(SRCDIR)/relationships_test.o: $(SRCDIR)/relationships_test.cpp $(SRCDIR)/relationships.h $(SRCDIR)/functions.h $(SRCDIR)/replaceable.h
$(OBJDIR)/maq_test.o: $(SRCDIR)/maq_test.cpp $(SRCDIR)/maq.h $(SRCDIR)/relationships.h $(SRCDIR)/functions.h $(SRCDIR)/replaceable.h
$(OBJDIR)/fragility_test.o: $(SRCDIR)/fragility_test.cpp $(SRCDIR)/fragility.h
$(OBJDIR)/comp_group_test.o: $(SRCDIR)/comp_group_test.cpp $(SRCDIR)/comp_group.h 
$(OBJDIR)/structure_test.o: $(SRCDIR)/structure_test.cpp $(SRCDIR)/structure.h
$(OBJDIR)/unit_test.o: $(SRCDIR)/unit_test.cpp

$(BINDIR)/unit_tests: $(OBJDIR)/unit_test.o $(UNIT_OBJS) $(LIBDIR)/libslat.so 
	$(CC) -fPIC $(UNIT_OBJS) -L$(LIBDIR) -lslat -o $(BINDIR)/unit_tests ${LDFLAGS} -lboost_unit_test_framework
$(OBJDIR)/pyslatcore.o: $(SRCDIR)/pyslatcore.cpp $(SRCDIR)/functions.h $(SRCDIR)/relationships.h $(SRCDIR)/replaceable.h
	$(CC) -c $(CFLAGS) `pkg-config --cflags python3` -o $@ $<
doc: $(OBJS) $(HEADERS)
	doxygen

interp: $(LIBDIR)/slatLexer.py $(LIBDIR)/slatParser.py $(SCRIPTDIR)/SlatInterpreter.py $(LIBDIR)/pyslat.py

$(LIBDIR)/slatParser.py: $(SRCDIR)/slatParser.g4
	cd $(SRCDIR) &&	$(ANTLR) -o $(realpath $(LIBDIR)) slatParser.g4

$(LIBDIR)/slatLexer.py: $(SRCDIR)/slatLexer.g4
	cd $(SRCDIR) && $(ANTLR) -o $(realpath $(LIBDIR)) slatLexer.g4

$(SCRIPTDIR)/SlatInterpreter.py: $(SRCDIR)/SlatInterpreter.py
	cp $(SRCDIR)/SlatInterpreter.py $(SCRIPTDIR)

$(LIBDIR)/pyslat.py: $(SRCDIR)/pyslat.py
	cp $(SRCDIR)/pyslat.py $(LIBDIR)

$(OBJDIR)/example2.o: $(SRCDIR)/example2.cpp $(SRCDIR)/functions.h $(SRCDIR)/relationships.h $(SRCDIR)/maq.h $(SRCDIR)/replaceable.h $(SRCDIR)/fragility.h $(SRCDIR)/lognormaldist.h $(SRCDIR)/loss_functions.h

$(SRCDIR)/cachetest.o: $(SRCDIR)/cachetest.cpp $(SRCDIR)/caching.h
cachetest: $(SRCDIR)/cachetest.o $(OBJxDIR)/caching.o
	$(CC) -fPIC $(SRCDIR)/cachetest.o $(SRCDIR)/caching.o -o cachetest ${LDFLAGS}

$(LIBDIR)/pyslatcore.py: $(OBJDIR)/pyslatcore.py
	cp $(OBJDIR)/pyslatcore.py $(LIBDIR)

$(OBJDIR)/wrap_pyslatcore.cpp: $(SRCDIR)/pyslatcore.i $(SRCDIR)/pyslatcore.h
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
	-o $(OBJDIR)/wrap_pyslatcore.cpp $(SRCDIR)/pyslatcore.i
$(OBJDIR)/wrap_pyslatcore.o: $(OBJDIR)/wrap_pyslatcore.cpp $(SRCDIR)/functions.h
	$(CC) -c $(CFLAGS) -I$(SRCDIR) $< -o $@

$(LIBDIR)/_pyslatcore.so: $(OBJDIR)/wrap_pyslatcore.o $(OBJDIR)/pyslatcore.o 
	$(CC) $(PYLIBFLAGS) -fPIC -shared -Wl,-soname,_pyslatcore.so $(OBJDIR)/wrap_pyslatcore.o $(OBJDIR)/pyslatcore.o -o $@  -L$(LIBDIR) -lslat 
