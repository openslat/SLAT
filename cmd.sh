g++ -c -std=gnu++11 \
    -o pyslat.o \
    -DMS_WIN64 \
    -I/c/MingW/include/ \
    -I/home/mag109/boost_1_60_0 \
    -I/c/Python34/include \
    -I/c/Program\ Files\ \(x86\)/GnuWin32/include \
    pyslat.cpp 

g++ -shared \
    -o pyslat.dll pyslat.o \
    -L. -lslat \
    -L/c/Python34/libs \
    -L/home/mag109/boost_1_60_0/stage/lib \
    -lboost_python3-mgw49-mt-1_60 \
    -lboost_python-mgw49-mt-1_60 \
    -lboost_system-mgw49-mt-1_60 \
    -lboost_thread-mgw49-mt-1_60 \
    -lboost_log-mgw49-mt-1_60 \
    -lboost_log_setup-mgw49-mt-1_60 \
    -lboost_container-mgw49-mt-1_60 \
    -lboost_context-mgw49-mt-1_60 \
    -lboost_filesystem-mgw49-mt-1_60 \
    -lpython3


