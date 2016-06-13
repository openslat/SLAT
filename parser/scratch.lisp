(let ((value ""))
  (dolist (k
	   '((31  105    2  721)
	     (32  105    4  721)
	     (33  105    6  721)
	     (34  105    8  721)
	     (35  105   10  721)
	     (36  105   12  721)
	     (37  105   14  721)
	     (38  105   16  721)
	     (39  105   18  721)
	     (40  105   20  721))
	   value)
    (setq value (format "%s\ncompgroup COMPGROUP_%d EDP_%d FRAG_%d LOSS_%d %d;"
			value
			(car k)
			(caddr k)
			(cadr k)
			(cadr k)
			(cadddr k)))))
"
compgroup COMPGROUP_31 EDP_2 FRAG_105 LOSS_105 721;
compgroup COMPGROUP_32 EDP_4 FRAG_105 LOSS_105 721;
compgroup COMPGROUP_33 EDP_6 FRAG_105 LOSS_105 721;
compgroup COMPGROUP_34 EDP_8 FRAG_105 LOSS_105 721;
compgroup COMPGROUP_35 EDP_10 FRAG_105 LOSS_105 721;
compgroup COMPGROUP_36 EDP_12 FRAG_105 LOSS_105 721;
compgroup COMPGROUP_37 EDP_14 FRAG_105 LOSS_105 721;
compgroup COMPGROUP_38 EDP_16 FRAG_105 LOSS_105 721;
compgroup COMPGROUP_39 EDP_18 FRAG_105 LOSS_105 721;
compgroup COMPGROUP_40 EDP_20 FRAG_105 LOSS_105 721;"



(let ((result "structure BUILDING"))
  (dotimes (i 40 result)
    (setq result (format "%s COMPGROUP_%d%s" result (+ 1 i) (if (eq i 39) ";" ",")))))
"structure BUILDING COMPGROUP_1, COMPGROUP_2, COMPGROUP_3, COMPGROUP_4, COMPGROUP_5, COMPGROUP_6, COMPGROUP_7, COMPGROUP_8, COMPGROUP_9, COMPGROUP_10, COMPGROUP_11, COMPGROUP_12, COMPGROUP_13, COMPGROUP_14, COMPGROUP_15, COMPGROUP_16, COMPGROUP_17, COMPGROUP_18, COMPGROUP_19, COMPGROUP_20, COMPGROUP_21, COMPGROUP_22, COMPGROUP_23, COMPGROUP_24, COMPGROUP_25, COMPGROUP_26, COMPGROUP_27, COMPGROUP_28, COMPGROUP_29, COMPGROUP_30, COMPGROUP_31, COMPGROUP_32, COMPGROUP_33, COMPGROUP_34, COMPGROUP_35, COMPGROUP_36, COMPGROUP_37, COMPGROUP_38, COMPGROUP_39, COMPGROUP_40;"


(let ((result ""))
  (dotimes (edp 21 result)
    (setq result
	  (format "%s\n\nrecorder edpim EDP_%d $linvalues --cols mean_x, sd_ln_x example1a_im_edp_%d.txt --new;"
		  result (+ 1 edp) (+ edp 1)))
    (setq result
	  (format "%s\nrecorder edprate EDP_%d $logvalues example1a_edp_%d_rate.txt --new;"
		  result (+ 1 edp) (+ edp 1)))))
"

recorder edpim EDP_1 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_1.txt --new;
recorder edprate EDP_1 $logvalues example1a_edp_1_rate.txt --new;

recorder edpim EDP_2 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_2.txt --new;
recorder edprate EDP_2 $logvalues example1a_edp_2_rate.txt --new;

recorder edpim EDP_3 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_3.txt --new;
recorder edprate EDP_3 $logvalues example1a_edp_3_rate.txt --new;

recorder edpim EDP_4 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_4.txt --new;
recorder edprate EDP_4 $logvalues example1a_edp_4_rate.txt --new;

recorder edpim EDP_5 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_5.txt --new;
recorder edprate EDP_5 $logvalues example1a_edp_5_rate.txt --new;

recorder edpim EDP_6 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_6.txt --new;
recorder edprate EDP_6 $logvalues example1a_edp_6_rate.txt --new;

recorder edpim EDP_7 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_7.txt --new;
recorder edprate EDP_7 $logvalues example1a_edp_7_rate.txt --new;

recorder edpim EDP_8 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_8.txt --new;
recorder edprate EDP_8 $logvalues example1a_edp_8_rate.txt --new;

recorder edpim EDP_9 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_9.txt --new;
recorder edprate EDP_9 $logvalues example1a_edp_9_rate.txt --new;

recorder edpim EDP_10 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_10.txt --new;
recorder edprate EDP_10 $logvalues example1a_edp_10_rate.txt --new;

recorder edpim EDP_11 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_11.txt --new;
recorder edprate EDP_11 $logvalues example1a_edp_11_rate.txt --new;

recorder edpim EDP_12 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_12.txt --new;
recorder edprate EDP_12 $logvalues example1a_edp_12_rate.txt --new;

recorder edpim EDP_13 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_13.txt --new;
recorder edprate EDP_13 $logvalues example1a_edp_13_rate.txt --new;

recorder edpim EDP_14 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_14.txt --new;
recorder edprate EDP_14 $logvalues example1a_edp_14_rate.txt --new;

recorder edpim EDP_15 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_15.txt --new;
recorder edprate EDP_15 $logvalues example1a_edp_15_rate.txt --new;

recorder edpim EDP_16 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_16.txt --new;
recorder edprate EDP_16 $logvalues example1a_edp_16_rate.txt --new;

recorder edpim EDP_17 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_17.txt --new;
recorder edprate EDP_17 $logvalues example1a_edp_17_rate.txt --new;

recorder edpim EDP_18 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_18.txt --new;
recorder edprate EDP_18 $logvalues example1a_edp_18_rate.txt --new;

recorder edpim EDP_19 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_19.txt --new;
recorder edprate EDP_19 $logvalues example1a_edp_19_rate.txt --new;

recorder edpim EDP_20 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_20.txt --new;
recorder edprate EDP_20 $logvalues example1a_edp_20_rate.txt --new;

recorder edpim EDP_21 $linvalues --cols mean_x, sd_ln_x example1a_im_edp_21.txt --new;
recorder edprate EDP_21 $logvalues example1a_edp_21_rate.txt --new;"





    
(let ((result ""))
  (dotimes (cg 40 result)
    (setq result
	  (format "%s\n\nrecorder lossedp COMPGROUP_%d $lossedpvalues example1a_loss_%d_edp.txt --new;"
		  result (+ cg 1) (+ cg 1)))
    (setq result
	  (format "%s\nrecorder lossim COMPGROUP_%d $lossimvalues example1a_loss_%d_im.txt --new;"
		  result (+ 1 cg) (+ cg 1)))
    (setq result
	  (format "%s\n### recorder dsrate COMPGROUP_%d;" result (+ 1 cg)))
    (setq result
	  (format "%s\nrecorder dsedp COMPGROUP_%d 0.0:0.01:0.200 example1a_ds_edp_%d.txt --new;"
		  result (+ 1 cg) (+ cg 1)))
    (setq result
	  (format "%s\nrecorder lossrate COMPGROUP_%d 1E-4:4.8E-3:1.2 example1a_loss_rate_%d.txt --new;"
		  result (+ 1 cg) (+ cg 1)))))
"

recorder lossedp COMPGROUP_1 $lossedpvalues example1a_loss_1_edp.txt --new;
recorder lossim COMPGROUP_1 $lossimvalues example1a_loss_1_im.txt --new;
### recorder dsrate COMPGROUP_1;
recorder dsedp COMPGROUP_1 0.0:0.01:0.200 example1a_ds_edp_1.txt --new;
recorder lossrate COMPGROUP_1 1E-4:4.8E-3:1.2 example1a_loss_rate_1.txt --new;

recorder lossedp COMPGROUP_2 $lossedpvalues example1a_loss_2_edp.txt --new;
recorder lossim COMPGROUP_2 $lossimvalues example1a_loss_2_im.txt --new;
### recorder dsrate COMPGROUP_2;
recorder dsedp COMPGROUP_2 0.0:0.01:0.200 example1a_ds_edp_2.txt --new;
recorder lossrate COMPGROUP_2 1E-4:4.8E-3:1.2 example1a_loss_rate_2.txt --new;

recorder lossedp COMPGROUP_3 $lossedpvalues example1a_loss_3_edp.txt --new;
recorder lossim COMPGROUP_3 $lossimvalues example1a_loss_3_im.txt --new;
### recorder dsrate COMPGROUP_3;
recorder dsedp COMPGROUP_3 0.0:0.01:0.200 example1a_ds_edp_3.txt --new;
recorder lossrate COMPGROUP_3 1E-4:4.8E-3:1.2 example1a_loss_rate_3.txt --new;

recorder lossedp COMPGROUP_4 $lossedpvalues example1a_loss_4_edp.txt --new;
recorder lossim COMPGROUP_4 $lossimvalues example1a_loss_4_im.txt --new;
### recorder dsrate COMPGROUP_4;
recorder dsedp COMPGROUP_4 0.0:0.01:0.200 example1a_ds_edp_4.txt --new;
recorder lossrate COMPGROUP_4 1E-4:4.8E-3:1.2 example1a_loss_rate_4.txt --new;

recorder lossedp COMPGROUP_5 $lossedpvalues example1a_loss_5_edp.txt --new;
recorder lossim COMPGROUP_5 $lossimvalues example1a_loss_5_im.txt --new;
### recorder dsrate COMPGROUP_5;
recorder dsedp COMPGROUP_5 0.0:0.01:0.200 example1a_ds_edp_5.txt --new;
recorder lossrate COMPGROUP_5 1E-4:4.8E-3:1.2 example1a_loss_rate_5.txt --new;

recorder lossedp COMPGROUP_6 $lossedpvalues example1a_loss_6_edp.txt --new;
recorder lossim COMPGROUP_6 $lossimvalues example1a_loss_6_im.txt --new;
### recorder dsrate COMPGROUP_6;
recorder dsedp COMPGROUP_6 0.0:0.01:0.200 example1a_ds_edp_6.txt --new;
recorder lossrate COMPGROUP_6 1E-4:4.8E-3:1.2 example1a_loss_rate_6.txt --new;

recorder lossedp COMPGROUP_7 $lossedpvalues example1a_loss_7_edp.txt --new;
recorder lossim COMPGROUP_7 $lossimvalues example1a_loss_7_im.txt --new;
### recorder dsrate COMPGROUP_7;
recorder dsedp COMPGROUP_7 0.0:0.01:0.200 example1a_ds_edp_7.txt --new;
recorder lossrate COMPGROUP_7 1E-4:4.8E-3:1.2 example1a_loss_rate_7.txt --new;

recorder lossedp COMPGROUP_8 $lossedpvalues example1a_loss_8_edp.txt --new;
recorder lossim COMPGROUP_8 $lossimvalues example1a_loss_8_im.txt --new;
### recorder dsrate COMPGROUP_8;
recorder dsedp COMPGROUP_8 0.0:0.01:0.200 example1a_ds_edp_8.txt --new;
recorder lossrate COMPGROUP_8 1E-4:4.8E-3:1.2 example1a_loss_rate_8.txt --new;

recorder lossedp COMPGROUP_9 $lossedpvalues example1a_loss_9_edp.txt --new;
recorder lossim COMPGROUP_9 $lossimvalues example1a_loss_9_im.txt --new;
### recorder dsrate COMPGROUP_9;
recorder dsedp COMPGROUP_9 0.0:0.01:0.200 example1a_ds_edp_9.txt --new;
recorder lossrate COMPGROUP_9 1E-4:4.8E-3:1.2 example1a_loss_rate_9.txt --new;

recorder lossedp COMPGROUP_10 $lossedpvalues example1a_loss_10_edp.txt --new;
recorder lossim COMPGROUP_10 $lossimvalues example1a_loss_10_im.txt --new;
### recorder dsrate COMPGROUP_10;
recorder dsedp COMPGROUP_10 0.0:0.01:0.200 example1a_ds_edp_10.txt --new;
recorder lossrate COMPGROUP_10 1E-4:4.8E-3:1.2 example1a_loss_rate_10.txt --new;

recorder lossedp COMPGROUP_11 $lossedpvalues example1a_loss_11_edp.txt --new;
recorder lossim COMPGROUP_11 $lossimvalues example1a_loss_11_im.txt --new;
### recorder dsrate COMPGROUP_11;
recorder dsedp COMPGROUP_11 0.0:0.01:0.200 example1a_ds_edp_11.txt --new;
recorder lossrate COMPGROUP_11 1E-4:4.8E-3:1.2 example1a_loss_rate_11.txt --new;

recorder lossedp COMPGROUP_12 $lossedpvalues example1a_loss_12_edp.txt --new;
recorder lossim COMPGROUP_12 $lossimvalues example1a_loss_12_im.txt --new;
### recorder dsrate COMPGROUP_12;
recorder dsedp COMPGROUP_12 0.0:0.01:0.200 example1a_ds_edp_12.txt --new;
recorder lossrate COMPGROUP_12 1E-4:4.8E-3:1.2 example1a_loss_rate_12.txt --new;

recorder lossedp COMPGROUP_13 $lossedpvalues example1a_loss_13_edp.txt --new;
recorder lossim COMPGROUP_13 $lossimvalues example1a_loss_13_im.txt --new;
### recorder dsrate COMPGROUP_13;
recorder dsedp COMPGROUP_13 0.0:0.01:0.200 example1a_ds_edp_13.txt --new;
recorder lossrate COMPGROUP_13 1E-4:4.8E-3:1.2 example1a_loss_rate_13.txt --new;

recorder lossedp COMPGROUP_14 $lossedpvalues example1a_loss_14_edp.txt --new;
recorder lossim COMPGROUP_14 $lossimvalues example1a_loss_14_im.txt --new;
### recorder dsrate COMPGROUP_14;
recorder dsedp COMPGROUP_14 0.0:0.01:0.200 example1a_ds_edp_14.txt --new;
recorder lossrate COMPGROUP_14 1E-4:4.8E-3:1.2 example1a_loss_rate_14.txt --new;

recorder lossedp COMPGROUP_15 $lossedpvalues example1a_loss_15_edp.txt --new;
recorder lossim COMPGROUP_15 $lossimvalues example1a_loss_15_im.txt --new;
### recorder dsrate COMPGROUP_15;
recorder dsedp COMPGROUP_15 0.0:0.01:0.200 example1a_ds_edp_15.txt --new;
recorder lossrate COMPGROUP_15 1E-4:4.8E-3:1.2 example1a_loss_rate_15.txt --new;

recorder lossedp COMPGROUP_16 $lossedpvalues example1a_loss_16_edp.txt --new;
recorder lossim COMPGROUP_16 $lossimvalues example1a_loss_16_im.txt --new;
### recorder dsrate COMPGROUP_16;
recorder dsedp COMPGROUP_16 0.0:0.01:0.200 example1a_ds_edp_16.txt --new;
recorder lossrate COMPGROUP_16 1E-4:4.8E-3:1.2 example1a_loss_rate_16.txt --new;

recorder lossedp COMPGROUP_17 $lossedpvalues example1a_loss_17_edp.txt --new;
recorder lossim COMPGROUP_17 $lossimvalues example1a_loss_17_im.txt --new;
### recorder dsrate COMPGROUP_17;
recorder dsedp COMPGROUP_17 0.0:0.01:0.200 example1a_ds_edp_17.txt --new;
recorder lossrate COMPGROUP_17 1E-4:4.8E-3:1.2 example1a_loss_rate_17.txt --new;

recorder lossedp COMPGROUP_18 $lossedpvalues example1a_loss_18_edp.txt --new;
recorder lossim COMPGROUP_18 $lossimvalues example1a_loss_18_im.txt --new;
### recorder dsrate COMPGROUP_18;
recorder dsedp COMPGROUP_18 0.0:0.01:0.200 example1a_ds_edp_18.txt --new;
recorder lossrate COMPGROUP_18 1E-4:4.8E-3:1.2 example1a_loss_rate_18.txt --new;

recorder lossedp COMPGROUP_19 $lossedpvalues example1a_loss_19_edp.txt --new;
recorder lossim COMPGROUP_19 $lossimvalues example1a_loss_19_im.txt --new;
### recorder dsrate COMPGROUP_19;
recorder dsedp COMPGROUP_19 0.0:0.01:0.200 example1a_ds_edp_19.txt --new;
recorder lossrate COMPGROUP_19 1E-4:4.8E-3:1.2 example1a_loss_rate_19.txt --new;

recorder lossedp COMPGROUP_20 $lossedpvalues example1a_loss_20_edp.txt --new;
recorder lossim COMPGROUP_20 $lossimvalues example1a_loss_20_im.txt --new;
### recorder dsrate COMPGROUP_20;
recorder dsedp COMPGROUP_20 0.0:0.01:0.200 example1a_ds_edp_20.txt --new;
recorder lossrate COMPGROUP_20 1E-4:4.8E-3:1.2 example1a_loss_rate_20.txt --new;

recorder lossedp COMPGROUP_21 $lossedpvalues example1a_loss_21_edp.txt --new;
recorder lossim COMPGROUP_21 $lossimvalues example1a_loss_21_im.txt --new;
### recorder dsrate COMPGROUP_21;
recorder dsedp COMPGROUP_21 0.0:0.01:0.200 example1a_ds_edp_21.txt --new;
recorder lossrate COMPGROUP_21 1E-4:4.8E-3:1.2 example1a_loss_rate_21.txt --new;

recorder lossedp COMPGROUP_22 $lossedpvalues example1a_loss_22_edp.txt --new;
recorder lossim COMPGROUP_22 $lossimvalues example1a_loss_22_im.txt --new;
### recorder dsrate COMPGROUP_22;
recorder dsedp COMPGROUP_22 0.0:0.01:0.200 example1a_ds_edp_22.txt --new;
recorder lossrate COMPGROUP_22 1E-4:4.8E-3:1.2 example1a_loss_rate_22.txt --new;

recorder lossedp COMPGROUP_23 $lossedpvalues example1a_loss_23_edp.txt --new;
recorder lossim COMPGROUP_23 $lossimvalues example1a_loss_23_im.txt --new;
### recorder dsrate COMPGROUP_23;
recorder dsedp COMPGROUP_23 0.0:0.01:0.200 example1a_ds_edp_23.txt --new;
recorder lossrate COMPGROUP_23 1E-4:4.8E-3:1.2 example1a_loss_rate_23.txt --new;

recorder lossedp COMPGROUP_24 $lossedpvalues example1a_loss_24_edp.txt --new;
recorder lossim COMPGROUP_24 $lossimvalues example1a_loss_24_im.txt --new;
### recorder dsrate COMPGROUP_24;
recorder dsedp COMPGROUP_24 0.0:0.01:0.200 example1a_ds_edp_24.txt --new;
recorder lossrate COMPGROUP_24 1E-4:4.8E-3:1.2 example1a_loss_rate_24.txt --new;

recorder lossedp COMPGROUP_25 $lossedpvalues example1a_loss_25_edp.txt --new;
recorder lossim COMPGROUP_25 $lossimvalues example1a_loss_25_im.txt --new;
### recorder dsrate COMPGROUP_25;
recorder dsedp COMPGROUP_25 0.0:0.01:0.200 example1a_ds_edp_25.txt --new;
recorder lossrate COMPGROUP_25 1E-4:4.8E-3:1.2 example1a_loss_rate_25.txt --new;

recorder lossedp COMPGROUP_26 $lossedpvalues example1a_loss_26_edp.txt --new;
recorder lossim COMPGROUP_26 $lossimvalues example1a_loss_26_im.txt --new;
### recorder dsrate COMPGROUP_26;
recorder dsedp COMPGROUP_26 0.0:0.01:0.200 example1a_ds_edp_26.txt --new;
recorder lossrate COMPGROUP_26 1E-4:4.8E-3:1.2 example1a_loss_rate_26.txt --new;

recorder lossedp COMPGROUP_27 $lossedpvalues example1a_loss_27_edp.txt --new;
recorder lossim COMPGROUP_27 $lossimvalues example1a_loss_27_im.txt --new;
### recorder dsrate COMPGROUP_27;
recorder dsedp COMPGROUP_27 0.0:0.01:0.200 example1a_ds_edp_27.txt --new;
recorder lossrate COMPGROUP_27 1E-4:4.8E-3:1.2 example1a_loss_rate_27.txt --new;

recorder lossedp COMPGROUP_28 $lossedpvalues example1a_loss_28_edp.txt --new;
recorder lossim COMPGROUP_28 $lossimvalues example1a_loss_28_im.txt --new;
### recorder dsrate COMPGROUP_28;
recorder dsedp COMPGROUP_28 0.0:0.01:0.200 example1a_ds_edp_28.txt --new;
recorder lossrate COMPGROUP_28 1E-4:4.8E-3:1.2 example1a_loss_rate_28.txt --new;

recorder lossedp COMPGROUP_29 $lossedpvalues example1a_loss_29_edp.txt --new;
recorder lossim COMPGROUP_29 $lossimvalues example1a_loss_29_im.txt --new;
### recorder dsrate COMPGROUP_29;
recorder dsedp COMPGROUP_29 0.0:0.01:0.200 example1a_ds_edp_29.txt --new;
recorder lossrate COMPGROUP_29 1E-4:4.8E-3:1.2 example1a_loss_rate_29.txt --new;

recorder lossedp COMPGROUP_30 $lossedpvalues example1a_loss_30_edp.txt --new;
recorder lossim COMPGROUP_30 $lossimvalues example1a_loss_30_im.txt --new;
### recorder dsrate COMPGROUP_30;
recorder dsedp COMPGROUP_30 0.0:0.01:0.200 example1a_ds_edp_30.txt --new;
recorder lossrate COMPGROUP_30 1E-4:4.8E-3:1.2 example1a_loss_rate_30.txt --new;

recorder lossedp COMPGROUP_31 $lossedpvalues example1a_loss_31_edp.txt --new;
recorder lossim COMPGROUP_31 $lossimvalues example1a_loss_31_im.txt --new;
### recorder dsrate COMPGROUP_31;
recorder dsedp COMPGROUP_31 0.0:0.01:0.200 example1a_ds_edp_31.txt --new;
recorder lossrate COMPGROUP_31 1E-4:4.8E-3:1.2 example1a_loss_rate_31.txt --new;

recorder lossedp COMPGROUP_32 $lossedpvalues example1a_loss_32_edp.txt --new;
recorder lossim COMPGROUP_32 $lossimvalues example1a_loss_32_im.txt --new;
### recorder dsrate COMPGROUP_32;
recorder dsedp COMPGROUP_32 0.0:0.01:0.200 example1a_ds_edp_32.txt --new;
recorder lossrate COMPGROUP_32 1E-4:4.8E-3:1.2 example1a_loss_rate_32.txt --new;

recorder lossedp COMPGROUP_33 $lossedpvalues example1a_loss_33_edp.txt --new;
recorder lossim COMPGROUP_33 $lossimvalues example1a_loss_33_im.txt --new;
### recorder dsrate COMPGROUP_33;
recorder dsedp COMPGROUP_33 0.0:0.01:0.200 example1a_ds_edp_33.txt --new;
recorder lossrate COMPGROUP_33 1E-4:4.8E-3:1.2 example1a_loss_rate_33.txt --new;

recorder lossedp COMPGROUP_34 $lossedpvalues example1a_loss_34_edp.txt --new;
recorder lossim COMPGROUP_34 $lossimvalues example1a_loss_34_im.txt --new;
### recorder dsrate COMPGROUP_34;
recorder dsedp COMPGROUP_34 0.0:0.01:0.200 example1a_ds_edp_34.txt --new;
recorder lossrate COMPGROUP_34 1E-4:4.8E-3:1.2 example1a_loss_rate_34.txt --new;

recorder lossedp COMPGROUP_35 $lossedpvalues example1a_loss_35_edp.txt --new;
recorder lossim COMPGROUP_35 $lossimvalues example1a_loss_35_im.txt --new;
### recorder dsrate COMPGROUP_35;
recorder dsedp COMPGROUP_35 0.0:0.01:0.200 example1a_ds_edp_35.txt --new;
recorder lossrate COMPGROUP_35 1E-4:4.8E-3:1.2 example1a_loss_rate_35.txt --new;

recorder lossedp COMPGROUP_36 $lossedpvalues example1a_loss_36_edp.txt --new;
recorder lossim COMPGROUP_36 $lossimvalues example1a_loss_36_im.txt --new;
### recorder dsrate COMPGROUP_36;
recorder dsedp COMPGROUP_36 0.0:0.01:0.200 example1a_ds_edp_36.txt --new;
recorder lossrate COMPGROUP_36 1E-4:4.8E-3:1.2 example1a_loss_rate_36.txt --new;

recorder lossedp COMPGROUP_37 $lossedpvalues example1a_loss_37_edp.txt --new;
recorder lossim COMPGROUP_37 $lossimvalues example1a_loss_37_im.txt --new;
### recorder dsrate COMPGROUP_37;
recorder dsedp COMPGROUP_37 0.0:0.01:0.200 example1a_ds_edp_37.txt --new;
recorder lossrate COMPGROUP_37 1E-4:4.8E-3:1.2 example1a_loss_rate_37.txt --new;

recorder lossedp COMPGROUP_38 $lossedpvalues example1a_loss_38_edp.txt --new;
recorder lossim COMPGROUP_38 $lossimvalues example1a_loss_38_im.txt --new;
### recorder dsrate COMPGROUP_38;
recorder dsedp COMPGROUP_38 0.0:0.01:0.200 example1a_ds_edp_38.txt --new;
recorder lossrate COMPGROUP_38 1E-4:4.8E-3:1.2 example1a_loss_rate_38.txt --new;

recorder lossedp COMPGROUP_39 $lossedpvalues example1a_loss_39_edp.txt --new;
recorder lossim COMPGROUP_39 $lossimvalues example1a_loss_39_im.txt --new;
### recorder dsrate COMPGROUP_39;
recorder dsedp COMPGROUP_39 0.0:0.01:0.200 example1a_ds_edp_39.txt --new;
recorder lossrate COMPGROUP_39 1E-4:4.8E-3:1.2 example1a_loss_rate_39.txt --new;

recorder lossedp COMPGROUP_40 $lossedpvalues example1a_loss_40_edp.txt --new;
recorder lossim COMPGROUP_40 $lossimvalues example1a_loss_40_im.txt --new;
### recorder dsrate COMPGROUP_40;
recorder dsedp COMPGROUP_40 0.0:0.01:0.200 example1a_ds_edp_40.txt --new;
recorder lossrate COMPGROUP_40 1E-4:4.8E-3:1.2 example1a_loss_rate_40.txt --new;"

