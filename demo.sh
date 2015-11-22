#! /bin/bash

rm *.png *.dat
if make && ./main; then
    graph -T png -l x -l y <(tail -n +2 im_rate.dat) \
	  -L "IM-Rate Relationship" \
	  -X "IM" \
	  -Y "Rate" > im_rate.png

    graph -T png -C \
	  -L "EDP-IM Relationship" \
	  -X "IM" \
	  -Y "EDP" \
	  <(tail -n +2 im_edp.dat | awk -e '{print $1, $2}') \
	  <(tail -n +2 im_edp.dat | awk -e '{print $1, $3}') \
	  <(tail -n +2 im_edp.dat | awk -e '{print $1, $4}') \
	  > edp_im.png

    graph -T png -l x -l y \
	  -L "EDP-RATE Relationship" \
	  -X "EDP" \
	  -Y "RATE" \
	  <(tail -n +2 edp_rate.dat) \
	  > edp_rate.png

    qiv -Dft im_rate.png edp_im.png edp_rate.png 
fi
