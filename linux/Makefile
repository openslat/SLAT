TARGET=LINUX
ANTLR=java -jar /usr/local/lib/antlr-4.5.2-complete.jar -Dlanguage=Python3

CC=g++
export LIB=linux/lib
CFLAGS=-g -O3 -fopenmp -Wall -Werror -fbounds-check -Warray-bounds -std=gnu++11 -DBOOST_ALL_DYN_LINK
CFLAGS+=`pkg-config --cflags python3`
CFLAGS +=  -fPIC `pkg-config --cflags gsl`

LDFLAGS=-lboost_log -lboost_thread -lboost_system -lpthread -lboost_unit_test_framework -lboost_filesystem
LDFLAGS += `pkg-config --libs gsl`
LDFLAGS += -lpthread -fopenmp

PYLIBFLAGS=-fPIC -shared -Wl,-soname,_pyslatcore.so `pkg-config --libs python3` `pkg-config --libs gsl`
PYSLATCORE=_pyslatcore.so
# Uncomment the next line to add the current directory to the search path. This is *NOT*
# generally recommended, but saves on from specifying 'LD_LIBRARY_PATH=.':
#LDFLAGS+=-Wl,-rpath,.

include ../Makefile.common
