#! /bin/bash

rm *.png example1a*.txt
export CLASSPATH=".:/usr/local/lib/antlr-4.5.2-complete.jar:$CLASSPATH"
java -jar /usr/local/lib/antlr-4.5.2-complete.jar -Dlanguage=Python3 slatLexer.g4
java -jar /usr/local/lib/antlr-4.5.2-complete.jar -Dlanguage=Python3 slatParser.g4

PALEO=/home/mag109/SLATv1.15_Public/example1a_collapse/results
if LD_LIBRARY_PATH=.. PYTHONPATH=.. ./SlatInterpreter.py test_cases/example1a.slat; then
    exit
    echo "Writing IM-Reate Relationship"
    graph -T png -l x -l y -C \
	  -L "IM-Rate Relationship" \
	  -X "IM" \
	  -Y "Rate" \
	  -m 1 -W 0.01 <(tail -n +4 $PALEO/im-rate.txt) \
	  -m 3 -W 0.005 <(tail -n +2 example1a_im_rate.txt) \
	  > im_rate.png

    echo "Writing EDP-IM Relationship"
    graph -T png -C \
	  -L "EDP-IM Relationship" \
	  -X "IM" \
	  -Y "EDP" \
	  -m 2 -W 0.01 <(tail -n +2 example1a_im_edp.txt | awk -e '{print $1, $4}') \
	  -m 3 -W 0.01 <(tail -n +2 example1a_im_edp.txt | awk -e '{print $1, $6}') \
	  -m 1 -W 0.01 <(tail -n +2 example1a_im_edp.txt | awk -e '{print $1, $2}') \
	  -m 4 -W 0.005 <(tail -n +4 $PALEO/edp-im-1 | awk -e '{print $1, $2}') \
	  > edp_im.png
    

    echo "Writing EDP-RATE Relationship"
    graph -T png -l x -l y -C \
	  -L "EDP-RATE Relationship" \
	  -X "EDP" \
	  -Y "RATE" \
	  -m 1 -W 0.01 <(tail -n +2 example1a_edp_rate.txt) \
	  -m 3 -W 0.005 <(tail -n +4 ../edp_rate.dat) \
	  > edp_rate.png

    echo "Writing DS-EDP Relationship"
    graph -T png -C \
	  -L "DS-EDP Relationship" \
	  -X "EDP" \
	  -Y "p(DS)" \
	  -W 0.01 <(tail -n +2 ../ds_edp.dat | awk -e '{print $1, $2}') \
	  -W 0.01 <(tail -n +2 ../ds_edp.dat | awk -e '{print $1, $3}') \
	  -W 0.01 <(tail -n +2 ../ds_edp.dat | awk -e '{print $1, $4}') \
	  -W 0.01 <(tail -n +2 ../ds_edp.dat | awk -e '{print $1, $5}') \
	  -W 0.005 <(tail -n +2 example1a_ds_edp.txt | awk -e '{print $1, $2}') \
	  -W 0.005 <(tail -n +2 example1a_ds_edp.txt | awk -e '{print $1, $3}') \
	  -W 0.005 <(tail -n +2 example1a_ds_edp.txt | awk -e '{print $1, $4}') \
	  -W 0.005 <(tail -n +2 example1a_ds_edp.txt | awk -e '{print $1, $5}') \
	  > ds_edp.png

    echo "Writing LOSS-EDP Relationship"
    graph -T png -C \
	  -L "LOSS-EDP Relationship" \
	  -X "EDP" \
	  -Y "LOSS" \
	  -m 1 -W 0.01 <(tail -n +2 ../loss_edp.dat | awk -e '{print $1, $2}') \
	  -m 3 -W 0.005 <(tail -n +3 example1a_loss_edp.txt | awk -e '{print $1, $2}') \
	  > loss_edp.png

    echo "Writing sigma LOSS-EDP Relationship"
    graph -T png -C \
	  -L "sigma LOSS-EDP Relationship" \
	  -X "EDP" \
	  -Y "LOSS" \
	  -m 1 -W 0.01 <(tail -n +2 ../loss_edp.dat | awk -e '{print $1, $3}') \
	  -m 3 -W 0.005 <(tail -n +3 example1a_loss_edp.txt | awk -e '{print $1, $3}') \
	  > sigma_loss_edp.png

    echo "Writing LOSS-IM Relationship"
        graph -T png -C \
	  -L "LOSS-IM Relationship" \
	  -X "IM" \
	  -Y "LOSS" \
	  -m 1 -W 0.01 <(tail -n +2 ../loss_im.dat | awk -e '{ if ($2 !~ /nan/) print $1, $2}') \
	  -m 3 -W 0.005 <(tail -n +3 example1a_loss_im.txt | awk -e '{print $1, $2}') \
	  > loss_im.png

	echo "Writing sigma LOSS-IM Relationship"
	graph -T png -C \
	  -L "sigma LOSS-IM Relationship" \
	  -X "IM" \
	  -Y "LOSS" \
	  -m 1 -W 0.01 <(tail -n +2 ../loss_im.dat | awk -e '{ if ($3 !~ /nan/) print $1, $3}') \
	  -m 3 -W 0.005 <(tail -n +3 example1a_loss_im.txt | awk -e '{print $1, $3}') \
	  > sigma_loss_im.png

	echo "Writing Annual Loss"
	graph -T png -C \
	  -L "Annual Loss" \
	  -X "Year" \
	  -Y "Loss" \
	  -m 1 -W 0.01 <(tail -n +2 ../annual_loss.dat | awk -e '{ if ($2 !~ /nan/) print $1, $2}') \
	  -m 3 -W 0.005 <(tail -n +2 example1a_annual_loss.txt | awk -e '{print $1, $2}') \
	  > annual_loss.png

	echo "Writing Loss Rate"
	graph -T png -l x -l y -C \
	  -L "Loss Rate" \
	  -X "Loss" \
	  -Y "Rate of Exceedence" \
	  -m 1 -W 0.01 <(tail -n +2 ../loss_rate.dat | awk -e '{ if ($2 !~ /nan/) print $1, $2}') \
	  -m 3 -W 0.005 <(tail -n +2 example1a_loss_rate.txt | awk -e '{print $1, $2}') \
	  > loss_rate.png

	qiv -Dft im_rate.png edp_im.png edp_rate.png ds_edp.png loss_edp.png sigma_loss_edp.png loss_im.png sigma_loss_im.png \
	annual_loss.png loss_rate.png
fi

