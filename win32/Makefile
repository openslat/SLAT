TARGET=WIN
ANTLR=java -jar /usr/local/lib/antlr-4.7.1-complete.jar -Dlanguage=Python3

PYLIB=../python-win32

CC=/opt/mxe/usr/bin/i686-w64-mingw32.shared-g++

CFLAGS=-g -O3 -fopenmp -Wall -Werror -std=gnu++11 -DBOOST_ALL_DYN_LINK
CFLAGS+=-I ../Python-3.5.2/PC -I../Python-3.5.2/Include

LDFLAGS=-L/opt/mxe/usr/bin/i686-w64-mingw32.shared/bin -L/opt/mxe/usr/bin/i686-w64-mingw32.shared/lib 
LDFLAGS+=-L../python-win32 -lpython35 -L.
LDFLAGS+=-lboost_log-mt -lboost_thread_win32-mt -lboost_system-mt -lpthread
LDFLAGS+=-lboost_filesystem-mt -lboost_log_setup-mt -lboost_unit_test_framework-mt
LDFLAGS+=-lpthread -fopenmp -lgsl

PYLIBFLAGS=-shared -Wl,--dll -Wl,--export-all-symbols -Wl,-out-implib,pyslatcore.1 
PYSLATCORE=_pyslatcore.pyd

export LIB=win32/lib
include ../Makefile.common

