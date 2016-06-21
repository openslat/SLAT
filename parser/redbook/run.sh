#! /bin/bash

mkdir -p results

if LD_LIBRARY_PATH=../.. PYTHONPATH=../.. ../SlatInterpreter.py redbook.slat; then
    echo "Successful"
fi

