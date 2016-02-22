#! /bin/bash

rm *.png *.dat
 if make && LD_LIBRARY_PATH=. ./main; then
    graph -T png -l x -l y -C \
	  -L "IM-Rate Relationship" \
	  -X "IM" \
	  -Y "Rate" \
	  -m 1 -W 0.01 <(tail -n +2 im_rate.dat) \
	  -m 3 -W 0.005 <(tail -n +4 ~/Downloads/SLATv1.15_Public/example1_output_gcc/example1_IM-rate-1) \
	  > im_rate.png

    graph -T png -C \
	  -L "EDP-IM Relationship" \
	  -X "IM" \
	  -Y "EDP" \
	  -m 1 -W 0.01 <(tail -n +2 im_edp.dat | awk -e '{print $1, $2}') \
	  -m 2 <(tail -n +2 im_edp.dat | awk -e '{print $1, $3}') \
	  -m 3 <(tail -n +2 im_edp.dat | awk -e '{print $1, $4}') \
	  -m 4 -W 0.005 <(tail -n +4 ~/Downloads/SLATv1.15_Public/example1_output_gcc/example1_EDP-IM-1 | awk -e '{print $1, $2}') \
	  > edp_im.png

    graph -T png -l x -l y -C \
	  -L "EDP-RATE Relationship" \
	  -X "EDP" \
	  -Y "RATE" \
	  -m 1 -W 0.01 <(tail -n +2 edp_rate.dat) \
	  -m 3 -W 0.005 <(tail -n +4 ~/Downloads/SLATv1.15_Public/example1_output_gcc/example1_EDP-rate-1) \
	  > edp_rate.png

    graph -T png -C \
	  -L "DS-EDP Relationship" \
	  -X "EDP" \
	  -Y "p(DS)" \
	  <(tail -n +2 ds_edp.dat | awk -e '{print $1, $2}') \
	  <(tail -n +2 ds_edp.dat | awk -e '{print $1, $3}') \
	  <(tail -n +2 ds_edp.dat | awk -e '{print $1, $4}') \
	  <(tail -n +2 ds_edp.dat | awk -e '{print $1, $5}') \
	  > ds_edp.png

    graph -T png -C \
	  -L "LOSS-EDP Relationship" \
	  -X "EDP" \
	  -Y "LOSS" \
	  -m 1 -W 0.01 <(tail -n +2 loss_edp.dat | awk -e '{print $1, $2}') \
	  -m 3 -W 0.005 <(tail -n +4 ~/Downloads/SLATv1.15_Public/example1_output_gcc/example1_Loss-EDP-1 | awk -e '{print $1, $2}') \
	  > loss_edp.png

    graph -T png -C \
	  -L "sigma LOSS-EDP Relationship" \
	  -X "EDP" \
	  -Y "LOSS" \
	  -m 1 -W 0.01 <(tail -n +2 loss_edp.dat | awk -e '{print $1, $3}') \
	  -m 3 -W 0.005 <(tail -n +4 ~/Downloads/SLATv1.15_Public/example1_output_gcc/example1_Loss-EDP-1 | awk -e '{print $1, $3}') \
	  > sigma_loss_edp.png

        graph -T png -C \
	  -L "LOSS-IM Relationship" \
	  -X "IM" \
	  -Y "LOSS" \
	  -m 1 -W 0.01 <(tail -n +2 loss_im.dat | awk -e '{ if ($2 !~ /nan/) print $1, $2}') \
	  -m 3 -W 0.005 <(tail -n +4 ~/Downloads/SLATv1.15_Public/example1_output_gcc/example1_Loss-IM-1 | awk -e '{print $1, $2}') \
	  > loss_im.png

	graph -T png -C \
	  -L "sigma LOSS-IM Relationship" \
	  -X "IM" \
	  -Y "LOSS" \
	  -m 1 -W 0.01 <(tail -n +2 loss_im.dat | awk -e '{ if ($3 !~ /nan/) print $1, $3}') \
	  -m 3 -W 0.005 <(tail -n +4 ~/Downloads/SLATv1.15_Public/example1_output_gcc/example1_Loss-IM-1 | awk -e '{print $1, $3}') \
	  > sigma_loss_im.png

    qiv -Dft im_rate.png edp_im.png edp_rate.png ds_edp.png loss_edp.png sigma_loss_edp.png loss_im.png sigma_loss_im.png
fi

LD_LIBRARY_PATH=. ./unit_tests
