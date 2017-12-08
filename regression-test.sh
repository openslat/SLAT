#! /bin/bash

export LD_LIBRARY_PATH=$PWD/linux/lib
export PYTHONPATH=$PWD/linux/lib

pushd parser/example2
rm -rf slat_resutls py-results c-results
../../linux/scripts/SlatInterpreter.py example2.slat
if ! $(diff --recursive --exclude=slat.log expected-results slat_results); then
    echo 'SLAT script failed.'
    popd
    exit -1
fi

./example2.py
if ! $(diff --recursive --exclude=slat.log expected-results py-results); then
    echo 'Python script failed.'
    popd
    exit -1
fi

if ! ../../linux/bin/example2; then
    echo 'C++ program failed to run.'
    popd
    exit -1
fi

popd
if ! ./check-c-results.sh; then
    echo 'C++ results failed.'
    exit -1
fi

exit 0

