TARGET=WIN
ANTLR=java -jar ~/antlr-4.5.3-complete.jar -Dlanguage=Python3


PYLIB=../python-win32

CC=/mingw64/bin/g++

CFLAGS=-g -O3 -fopenmp -Wall -Werror -std=gnu++11 -DBOOST_ALL_DYN_LINK
CFLAGS+=-I ../Python-3.5.2/PC -I../Python-3.5.2/Include
CFLAGS+=-DMS_WIN64

LDFLAGS=-L/mingw64lib -L/usr/local/lib
LDFLAGS+=-L../python-amd64 -lpython3.5m -L.
LDFLAGS+=-lboost_log-mt -lboost_thread-mt -lboost_system-mt -lpthread
LDFLAGS+=-lboost_filesystem-mt -lboost_log_setup-mt -lboost_unit_test_framework-mt
LDFLAGS+=-lpthread -fopenmp -lgsl

PYLIBFLAGS=-shared -Wl,--dll -Wl,--export-all-symbols -Wl,-out-implib,pyslatcore.1 
PYSLATCORE=_pyslatcore.pyd

include ../Makefile.common
