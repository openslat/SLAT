#! /bin/bash

rm *.png example2*.txt
export CLASSPATH=".:/usr/local/lib/antlr-4.5.2-complete.jar:$CLASSPATH"
java -jar /usr/local/lib/antlr-4.5.2-complete.jar -Dlanguage=Python3 slat.g4

if LD_LIBRARY_PATH=.. PYTHONPATH=.. ./SlatInterpreter.py test_cases/example2.slat; then
    echo "Writing IM-Reate Relationship"
    graph -T png -l x -l y -C \
	  -L "IM-Rate Relationship" \
	  -X "IM" \
	  -Y "Rate" \
	  -m 3 -W 0.005 <(tail -n +3 example2_im_rate.txt) \
	  > im_rate_2.png

#	qiv -Dft im_rate_2.png 
fi

