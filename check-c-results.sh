#! /bin/bash
result=0
for f in parser/example2/c-results/*; do
    #echo $f;
    ./compare-values.awk $f ${f/c-results/expected-results}
    if [[ $? -ne 0 ]]; then
	echo Error in  $(basename $f)
	result=-1
    fi
done

for f in parser/example2/expected-results/*; do
    if [[ ! -e ${f/expected-results/c-results} ]]; then
	echo missing $(basename $f); 
	result=-1
    fi;
done
exit $result
