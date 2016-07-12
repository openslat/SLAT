#! /bin/bash
for f in parser/example2/c-results/*; do
    #echo $f;
    ./compare-values.awk $f ${f/c-results/results}
    if [[ $? -ne 0 ]]; then
	echo Error in  $(basename $f)
	exit -1
    fi
done

for f in parser/example2/results//*; do
    if [[ ! -e ${f/results/c-results} ]]; then
	echo missing $(basename $f); 
	exit -1
    fi;
done
