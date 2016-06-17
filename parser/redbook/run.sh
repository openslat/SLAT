#! /bin/bash

if LD_LIBRARY_PATH=../.. PYTHONPATH=../.. ../SlatInterpreter.py redbook.slat; then
    echo "Successful"
fi

