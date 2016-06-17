#! /bin/bash

if LD_LIBRARY_PATH=../.. PYTHONPATH=../.. ../SlatInterpreter.py example2.slat; then
    echo "Successful"
fi

