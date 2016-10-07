SRCDIR=../src
OBJDIR=obj
BINDIR=bin
ANTLR=java -jar /usr/local/lib/antlr-4.5.2-complete.jar -Dlanguage=Python3

CFLAGS=-g -O3 -fopenmp -Wall -Werror -fbounds-check -Warray-bounds -std=gnu++11 -DBOOST_ALL_DYN_LINK
CFLAGS+=`pkg-config --cflags python3`
CC=g++
CFLAGS +=  -fPIC `pkg-config --cflags gsl`
LDFLAGS=-lboost_log -lboost_thread -lboost_system -lpthread 
LDFLAGS += `pkg-config --libs gsl`
LDFLAGS += -lpthread -fopenmp

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

all: $(OBJDIR) $(BINDIR) $(BINDIR)/main $(BINDIR)/unit_tests $(BINDIR)/_pyslatcore.so \
	$(BINDIR)/example2 interp $(BINDIR)/pyslatcore.py

$(OBJDIR):
	mkdir $(OBJDIR)

$(BINDIR):
	mkdir $(BINDIR)

clean:
	rm -rf $(OBJDIR) $(BINDIR)


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

$(BINDIR)/libslat.so: $(OBJDIR)/functions.o $(OBJDIR)/relationships.o $(OBJDIR)/maq.o $(OBJDIR)/fragility.o $(OBJDIR)/lognormaldist.o $(OBJDIR)/loss_functions.o $(OBJDIR)/comp_group.o $(OBJDIR)/caching.o $(OBJDIR)/structure.o $(BINDIR)
	$(CC) -fPIC -shared -Wl,-soname,libslat.so -o $(BINDIR)/libslat.so $(LIBOBJS) ${LDFLAGS}

$(OBJDIR)/main.o: $(SRCDIR)/main.cpp $(SRCDIR)/functions.h $(SRCDIR)/relationships.h $(SRCDIR)/maq.h $(SRCDIR)/replaceable.h $(SRCDIR)/fragility.h $(SRCDIR)/lognormaldist.h $(SRCDIR)/loss_functions.h
$(OBJDIR)/total_loss_debug.o: $(SRCDIR)/total_loss_debug.cpp $(SRCDIR)/functions.h $(SRCDIR)/relationships.h $(SRCDIR)/maq.h $(SRCDIR)/replaceable.h $(SRCDIR)/fragility.h $(SRCDIR)/lognormaldist.h $(SRCDIR)/loss_functions.h

$(BINDIR)/main: $(OBJDIR)/main.o $(BINDIR)/libslat.so
	$(CC) -fPIC $(OBJDIR)/main.o -L$(BINDIR) -lslat -o $(BINDIR)/main ${LDFLAGS}
$(BINDIR)/example2: $(OBJDIR)/example2.o $(BINDIR)/libslat.so
	$(CC) -fPIC $(OBJDIR)/example2.o -L$(BINDIR) -lslat -o $(BINDIR)/example2 ${LDFLAGS}

$(OBJDIR)/functions_test.o: $(SRCDIR)/functions_test.cpp $(SRCDIR)/functions.h $(SRCDIR)/replaceable.h
$(SRCDIR)/relationships_test.o: $(SRCDIR)/relationships_test.cpp $(SRCDIR)/relationships.h $(SRCDIR)/functions.h $(SRCDIR)/replaceable.h
$(OBJDIR)/maq_test.o: $(SRCDIR)/maq_test.cpp $(SRCDIR)/maq.h $(SRCDIR)/relationships.h $(SRCDIR)/functions.h $(SRCDIR)/replaceable.h
$(OBJDIR)/fragility_test.o: $(SRCDIR)/fragility_test.cpp $(SRCDIR)/fragility.h
$(OBJDIR)/comp_group_test.o: $(SRCDIR)/comp_group_test.cpp $(SRCDIR)/comp_group.h 
$(OBJDIR)/structure_test.o: $(SRCDIR)/structure_test.cpp $(SRCDIR)/structure.h
$(OBJDIR)/unit_test.o: $(SRCDIR)/unit_test.cpp

$(BINDIR)/unit_tests: $(OBJDIR)/unit_test.o $(UNIT_OBJS) $(BINDIR)/libslat.so 
	$(CC) -fPIC $(UNIT_OBJS) -L$(BINDIR) -lslat -o $(BINDIR)/unit_tests ${LDFLAGS} -lboost_unit_test_framework
$(OBJDIR)/pyslatcore.o: $(SRCDIR)/pyslatcore.cpp $(SRCDIR)/functions.h $(SRCDIR)/relationships.h $(SRCDIR)/replaceable.h
	$(CC) -c $(CFLAGS) `pkg-config --cflags python3` -o $@ $<
doc: $(OBJS) $(HEADERS)
	doxygen

interp: $(BINDIR)/slatLexer.py $(BINDIR)/slatParser.py

$(BINDIR)/slatParser.py: $(SRCDIR)/slatParser.g4
	cd $(SRCDIR) &&	$(ANTLR) -o $(realpath $(BINDIR)) slatParser.g4

$(BINDIR)/slatLexer.py: $(SRCDIR)/slatLexer.g4
	cd $(SRCDIR) && $(ANTLR) -o $(realpath $(BINDIR)) slatLexer.g4

$(OBJDIR)/example2.o: $(SRCDIR)/example2.cpp $(SRCDIR)/functions.h $(SRCDIR)/relationships.h $(SRCDIR)/maq.h $(SRCDIR)/replaceable.h $(SRCDIR)/fragility.h $(SRCDIR)/lognormaldist.h $(SRCDIR)/loss_functions.h

$(SRCDIR)/cachetest.o: $(SRCDIR)/cachetest.cpp $(SRCDIR)/caching.h
cachetest: $(SRCDIR)/cachetest.o $(OBJxDIR)/caching.o
	$(CC) -fPIC $(SRCDIR)/cachetest.o $(SRCDIR)/caching.o -o cachetest ${LDFLAGS}

$(BINDIR)/pyslatcore.py: $(OBJDIR)/pyslatcore.py
	cp $(OBJDIR)/pyslatcore.py $(BINDIR)

$(SRCDIR)/wrap_pyslatcore.cpp: $(SRCDIR)/pyslatcore.i $(SRCDIR)/pyslatcore.h
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

$(BINDIR)/_pyslatcore.so: $(OBJDIR)/wrap_pyslatcore.o $(OBJDIR)/pyslatcore.o 
	$(CC) -fPIC -shared -Wl,-soname,_pyslatcore.so $(OBJDIR)/wrap_pyslatcore.o $(OBJDIR)/pyslatcore.o -o $@ `pkg-config --libs python3` `pkg-config --libs gsl` -L$(BINDIR) -lslat 
