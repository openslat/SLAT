#! /bin/bash

rm *.png *.txt
if LD_LIBRARY_PATH=.. PYTHONPATH=.. ./SlatInterpreter.py test_cases/example1.slat; then
    graph -T png -l x -l y -C \
	  -L "IM-Rate Relationship" \
	  -X "IM" \
	  -Y "Rate" \
	  -m 1 -W 0.01 <(tail -n +2 ../im_rate.dat) \
	  -m 3 -W 0.005 <(tail -n +3 example1_im_rate.txt) \
	  > im_rate.png

    graph -T png -C \
	  -L "EDP-IM Relationship" \
	  -X "IM" \
	  -Y "EDP" \
	  -m 1 -W 0.01 <(tail -n +2 example1_im_edp.txt | awk -e '{print $1, $2}') \
	  -m 2 -W 0.01 <(tail -n +2 example1_im_edp.txt | awk -e '{print $1, $4}') \
	  -m 3 -W 0.01 <(tail -n +2 example1_im_edp.txt | awk -e '{print $1, $6}') \
	  -m 4 -W 0.005 <(tail -n +2 ../im_edp.dat | awk -e '{print $1, $2}') \
	  -m 5 -W 0.005 <(tail -n +2 ../im_edp.dat | awk -e '{print $1, $3}') \
	  -m 6 -W 0.005 <(tail -n +2 ../im_edp.dat | awk -e '{print $1, $4}') \
	  > edp_im.png

    graph -T png -l x -l y -C \
	  -L "EDP-RATE Relationship" \
	  -X "EDP" \
	  -Y "RATE" \
	  -m 1 -W 0.01 <(tail -n +2 example1_edp_rate.txt) \
	  -m 3 -W 0.005 <(tail -n +4 ../edp_rate.dat) \
	  > edp_rate.png

    graph -T png -C \
	  -L "DS-EDP Relationship" \
	  -X "EDP" \
	  -Y "p(DS)" \
	  -W 0.01 <(tail -n +2 ../ds_edp.dat | awk -e '{print $1, $2}') \
	  -W 0.01 <(tail -n +2 ../ds_edp.dat | awk -e '{print $1, $3}') \
	  -W 0.01 <(tail -n +2 ../ds_edp.dat | awk -e '{print $1, $4}') \
	  -W 0.01 <(tail -n +2 ../ds_edp.dat | awk -e '{print $1, $5}') \
	  -W 0.005 <(tail -n +2 example1_ds_edp.txt | awk -e '{print $1, $2}') \
	  -W 0.005 <(tail -n +2 example1_ds_edp.txt | awk -e '{print $1, $3}') \
	  -W 0.005 <(tail -n +2 example1_ds_edp.txt | awk -e '{print $1, $4}') \
	  -W 0.005 <(tail -n +2 example1_ds_edp.txt | awk -e '{print $1, $5}') \
	  > ds_edp.png

    graph -T png -C \
	  -L "LOSS-EDP Relationship" \
	  -X "EDP" \
	  -Y "LOSS" \
	  -m 1 -W 0.01 <(tail -n +2 ../loss_edp.dat | awk -e '{print $1, $2}') \
	  -m 3 -W 0.005 <(tail -n +3 example1_loss_edp.txt | awk -e '{print $1, $2}') \
	  > loss_edp.png

    graph -T png -C \
	  -L "sigma LOSS-EDP Relationship" \
	  -X "EDP" \
	  -Y "LOSS" \
	  -m 1 -W 0.01 <(tail -n +2 ../loss_edp.dat | awk -e '{print $1, $3}') \
	  -m 3 -W 0.005 <(tail -n +3 example1_loss_edp.txt | awk -e '{print $1, $3}') \
	  > sigma_loss_edp.png

        graph -T png -C \
	  -L "LOSS-IM Relationship" \
	  -X "IM" \
	  -Y "LOSS" \
	  -m 1 -W 0.01 <(tail -n +2 ../loss_im.dat | awk -e '{ if ($2 !~ /nan/) print $1, $2}') \
	  -m 3 -W 0.005 <(tail -n +3 example1_loss_im.txt | awk -e '{print $1, $2}') \
	  > loss_im.png

	graph -T png -C \
	  -L "sigma LOSS-IM Relationship" \
	  -X "IM" \
	  -Y "LOSS" \
	  -m 1 -W 0.01 <(tail -n +2 ../loss_im.dat | awk -e '{ if ($3 !~ /nan/) print $1, $3}') \
	  -m 3 -W 0.005 <(tail -n +3 example1_loss_im.txt | awk -e '{print $1, $3}') \
	  > sigma_loss_im.png

#	graph -T png -C \
#	  -L "Annual Loss" \
#	  -X "Year" \
#	  -Y "Loss" \
#	  -m 1 -W 0.01 <(tail -n +2 annual_loss.dat | awk -e '{ if ($2 !~ /nan/) print $1, $2}') \
#	  -m 3 -W 0.005 <(tail -n +4 ~/Downloads/SLATv1.15_Public/example1_output_gcc/example1_ExpectedLoss-1 | awk -e '{print $1, $2}') \
#	  > annual_loss.png
#
#	qiv -ft annual_loss.png
#	graph -T png -l x -l y -C \
#	  -L "Loss Rate" \
#	  -X "Loss" \
#	  -Y "Rate of Exceedence" \
#	  -m 1 -W 0.01 <(tail -n +2 loss_rate.dat | awk -e '{ if ($2 !~ /nan/) print $1, $2}') \
#	  -m 3 -W 0.005 <(tail -n +4 ~/Downloads/SLATv1.15_Public/example1_output_gcc/example1_Loss-rate-1 | awk -e '{print $1, $2}') \
#	  > loss_rate.png
#
	qiv -Dft im_rate.png edp_im.png edp_rate.png ds_edp.png loss_edp.png sigma_loss_edp.png loss_im.png sigma_loss_im.png
	#annual_loss.png loss_rate.png
fi

