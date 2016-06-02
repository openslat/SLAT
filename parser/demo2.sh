#! /bin/bash

rm *.png example2*.txt
export CLASSPATH=".:/usr/local/lib/antlr-4.5.2-complete.jar:$CLASSPATH"
java -jar /usr/local/lib/antlr-4.5.2-complete.jar -Dlanguage=Python3 slat.g4

if LD_LIBRARY_PATH=.. PYTHONPATH=.. ./SlatInterpreter.py test_cases/example2.slat; then
    echo "Successful"
fi

