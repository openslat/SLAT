(let ((value ""))
  (dolist (k
	   '((1    2    2   20)
	     (2    2    2   32)
	     (3    2    4   20)
	     (4    2    4   32)
	     (5    2    6   20)
	     (6    2    6   32)
	     (7    2    8   20)
	     (8    2    8   32)
	     (9    2   10   20)
	     (10    2   10   32)
	     (11    2   12   20)
	     (12    2   12   32)
	     (13    2   14   20)
	     (14    2   14   32)
	     (15    2   16   20)
	     (16    2   16   32)
	     (17    2   18   20)
	     (18    2   18   32)
	     (19    2   20   20)
	     (20    2   20   32)
	     (21    3    2   20)
	     (22    3    4   20)
	     (23    3    6   20)
	     (24    3    8   20)
	     (25    3   10   20)
	     (26    3   12   20)
	     (27    3   14   20)
	     (28    3   16   20)
	     (29    3   18   20)
	     (30    3   20   20)
	     (31  105    2  728)
	     (32  105    4  728)
	     (33  105    6  728)
	     (34  105    8  728)
	     (35  105   10  728)
	     (36  105   12  728)
	     (37  105   14  728)
	     (38  105   16  728)
	     (39  105   18  728)
	     (40  105   20  728)
	     (41  106    2  728)
	     (42  106    4  728)
	     (43  106    6  728)
	     (44  106    8  728)
	     (45  106   10  728)
	     (46  106   12  728)
	     (47  106   14  728)
	     (48  106   16  728)
	     (49  106   18  728)
	     (50  106   20  728)
	     (51  107    2  157)
	     (52  107    4  157)
	     (53  107    6  157)
	     (54  107    8  157)
	     (55  107   10  157)
	     (56  107   12  157)
	     (57  107   14  157)
	     (58  107   16  157)
	     (59  107   18  157)
	     (60  107   20  157)
	     (61  203    3  728)
	     (62  203    5  728)
	     (63  203    7  728)
	     (64  203    9  728)
	     (65  203   11  728)
	     (66  203   13  728)
	     (67  203   15  728)
	     (68  203   17  728)
	     (69  203   19  728)
	     (70  203   21  728)
	     (71  211    3   73)
	     (72  211    5   73)
	     (73  211    7   73)
	     (74  211    9   73)
	     (75  211   11   73)
	     (76  211   13   73)
	     (77  211   15   73)
	     (78  211   17   73)
	     (79  211   19   73)
	     (80  211   21   73)
	     (81  205   21    4)
	     (82  214    1   10)
	     (83  214    3   10)
	     (84  214    5   10)
	     (85  214    7   10)
	     (86  214    9   10)
	     (87  214   11   10)
	     (88  214   13   10)
	     (89  214   15   10)
	     (90  214   17   10)
	     (91  214   19   10)
	     (92  108    2   10)
	     (93  108    4   10)
	     (94  108    6   10)
	     (95  108    8   10)
	     (96  108   10   10)
	     (97  108   12   10)
	     (98  108   14   10)
	     (99  108   16   10)
	     (100  108   18   10)
	     (101  108   20   10)
	     (102  208    1   53)
	     (103  208    3   53)
	     (104  208    5   53)
	     (105  208    7   53)
	     (106  208    9   53)
	     (107  208   11   53)
	     (108  208   13   53)
	     (109  208   15   53)
	     (110  208   17   53)
	     (111  208   19   53)
	     (112  209    5   16)
	     (113  209   11   16)
	     (114  209   19   16)
	     (115  210    1   80)
	     (116  210    3   80)
	     (117  210    5   80)
	     (118  210    7   80)
	     (119  210    9   80)
	     (120  210   11   80)
	     (121  210   13   80)
	     (122  210   15   80)
	     (123  210   17   80)
	     (124  210   19   80)
	     (125  204    1    2))
	   value)
    (setq value (format "%s\ncompgroup COMPGROUP_%d EDP_%d FRAG_%d LOSS_%d %d;"
			value
			(car k)
			(caddr k)
			(cadr k)
			(cadr k)
			(cadddr k)))))
"
compgroup COMPGROUP_1 EDP_2 FRAG_2 LOSS_2 20;
compgroup COMPGROUP_2 EDP_2 FRAG_2 LOSS_2 32;
compgroup COMPGROUP_3 EDP_4 FRAG_2 LOSS_2 20;
compgroup COMPGROUP_4 EDP_4 FRAG_2 LOSS_2 32;
compgroup COMPGROUP_5 EDP_6 FRAG_2 LOSS_2 20;
compgroup COMPGROUP_6 EDP_6 FRAG_2 LOSS_2 32;
compgroup COMPGROUP_7 EDP_8 FRAG_2 LOSS_2 20;
compgroup COMPGROUP_8 EDP_8 FRAG_2 LOSS_2 32;
compgroup COMPGROUP_9 EDP_10 FRAG_2 LOSS_2 20;
compgroup COMPGROUP_10 EDP_10 FRAG_2 LOSS_2 32;
compgroup COMPGROUP_11 EDP_12 FRAG_2 LOSS_2 20;
compgroup COMPGROUP_12 EDP_12 FRAG_2 LOSS_2 32;
compgroup COMPGROUP_13 EDP_14 FRAG_2 LOSS_2 20;
compgroup COMPGROUP_14 EDP_14 FRAG_2 LOSS_2 32;
compgroup COMPGROUP_15 EDP_16 FRAG_2 LOSS_2 20;
compgroup COMPGROUP_16 EDP_16 FRAG_2 LOSS_2 32;
compgroup COMPGROUP_17 EDP_18 FRAG_2 LOSS_2 20;
compgroup COMPGROUP_18 EDP_18 FRAG_2 LOSS_2 32;
compgroup COMPGROUP_19 EDP_20 FRAG_2 LOSS_2 20;
compgroup COMPGROUP_20 EDP_20 FRAG_2 LOSS_2 32;
compgroup COMPGROUP_21 EDP_2 FRAG_3 LOSS_3 20;
compgroup COMPGROUP_22 EDP_4 FRAG_3 LOSS_3 20;
compgroup COMPGROUP_23 EDP_6 FRAG_3 LOSS_3 20;
compgroup COMPGROUP_24 EDP_8 FRAG_3 LOSS_3 20;
compgroup COMPGROUP_25 EDP_10 FRAG_3 LOSS_3 20;
compgroup COMPGROUP_26 EDP_12 FRAG_3 LOSS_3 20;
compgroup COMPGROUP_27 EDP_14 FRAG_3 LOSS_3 20;
compgroup COMPGROUP_28 EDP_16 FRAG_3 LOSS_3 20;
compgroup COMPGROUP_29 EDP_18 FRAG_3 LOSS_3 20;
compgroup COMPGROUP_30 EDP_20 FRAG_3 LOSS_3 20;
compgroup COMPGROUP_31 EDP_2 FRAG_105 LOSS_105 728;
compgroup COMPGROUP_32 EDP_4 FRAG_105 LOSS_105 728;
compgroup COMPGROUP_33 EDP_6 FRAG_105 LOSS_105 728;
compgroup COMPGROUP_34 EDP_8 FRAG_105 LOSS_105 728;
compgroup COMPGROUP_35 EDP_10 FRAG_105 LOSS_105 728;
compgroup COMPGROUP_36 EDP_12 FRAG_105 LOSS_105 728;
compgroup COMPGROUP_37 EDP_14 FRAG_105 LOSS_105 728;
compgroup COMPGROUP_38 EDP_16 FRAG_105 LOSS_105 728;
compgroup COMPGROUP_39 EDP_18 FRAG_105 LOSS_105 728;
compgroup COMPGROUP_40 EDP_20 FRAG_105 LOSS_105 728;
compgroup COMPGROUP_41 EDP_2 FRAG_106 LOSS_106 728;
compgroup COMPGROUP_42 EDP_4 FRAG_106 LOSS_106 728;
compgroup COMPGROUP_43 EDP_6 FRAG_106 LOSS_106 728;
compgroup COMPGROUP_44 EDP_8 FRAG_106 LOSS_106 728;
compgroup COMPGROUP_45 EDP_10 FRAG_106 LOSS_106 728;
compgroup COMPGROUP_46 EDP_12 FRAG_106 LOSS_106 728;
compgroup COMPGROUP_47 EDP_14 FRAG_106 LOSS_106 728;
compgroup COMPGROUP_48 EDP_16 FRAG_106 LOSS_106 728;
compgroup COMPGROUP_49 EDP_18 FRAG_106 LOSS_106 728;
compgroup COMPGROUP_50 EDP_20 FRAG_106 LOSS_106 728;
compgroup COMPGROUP_51 EDP_2 FRAG_107 LOSS_107 157;
compgroup COMPGROUP_52 EDP_4 FRAG_107 LOSS_107 157;
compgroup COMPGROUP_53 EDP_6 FRAG_107 LOSS_107 157;
compgroup COMPGROUP_54 EDP_8 FRAG_107 LOSS_107 157;
compgroup COMPGROUP_55 EDP_10 FRAG_107 LOSS_107 157;
compgroup COMPGROUP_56 EDP_12 FRAG_107 LOSS_107 157;
compgroup COMPGROUP_57 EDP_14 FRAG_107 LOSS_107 157;
compgroup COMPGROUP_58 EDP_16 FRAG_107 LOSS_107 157;
compgroup COMPGROUP_59 EDP_18 FRAG_107 LOSS_107 157;
compgroup COMPGROUP_60 EDP_20 FRAG_107 LOSS_107 157;
compgroup COMPGROUP_61 EDP_3 FRAG_203 LOSS_203 728;
compgroup COMPGROUP_62 EDP_5 FRAG_203 LOSS_203 728;
compgroup COMPGROUP_63 EDP_7 FRAG_203 LOSS_203 728;
compgroup COMPGROUP_64 EDP_9 FRAG_203 LOSS_203 728;
compgroup COMPGROUP_65 EDP_11 FRAG_203 LOSS_203 728;
compgroup COMPGROUP_66 EDP_13 FRAG_203 LOSS_203 728;
compgroup COMPGROUP_67 EDP_15 FRAG_203 LOSS_203 728;
compgroup COMPGROUP_68 EDP_17 FRAG_203 LOSS_203 728;
compgroup COMPGROUP_69 EDP_19 FRAG_203 LOSS_203 728;
compgroup COMPGROUP_70 EDP_21 FRAG_203 LOSS_203 728;
compgroup COMPGROUP_71 EDP_3 FRAG_211 LOSS_211 73;
compgroup COMPGROUP_72 EDP_5 FRAG_211 LOSS_211 73;
compgroup COMPGROUP_73 EDP_7 FRAG_211 LOSS_211 73;
compgroup COMPGROUP_74 EDP_9 FRAG_211 LOSS_211 73;
compgroup COMPGROUP_75 EDP_11 FRAG_211 LOSS_211 73;
compgroup COMPGROUP_76 EDP_13 FRAG_211 LOSS_211 73;
compgroup COMPGROUP_77 EDP_15 FRAG_211 LOSS_211 73;
compgroup COMPGROUP_78 EDP_17 FRAG_211 LOSS_211 73;
compgroup COMPGROUP_79 EDP_19 FRAG_211 LOSS_211 73;
compgroup COMPGROUP_80 EDP_21 FRAG_211 LOSS_211 73;
compgroup COMPGROUP_81 EDP_21 FRAG_205 LOSS_205 4;
compgroup COMPGROUP_82 EDP_1 FRAG_214 LOSS_214 10;
compgroup COMPGROUP_83 EDP_3 FRAG_214 LOSS_214 10;
compgroup COMPGROUP_84 EDP_5 FRAG_214 LOSS_214 10;
compgroup COMPGROUP_85 EDP_7 FRAG_214 LOSS_214 10;
compgroup COMPGROUP_86 EDP_9 FRAG_214 LOSS_214 10;
compgroup COMPGROUP_87 EDP_11 FRAG_214 LOSS_214 10;
compgroup COMPGROUP_88 EDP_13 FRAG_214 LOSS_214 10;
compgroup COMPGROUP_89 EDP_15 FRAG_214 LOSS_214 10;
compgroup COMPGROUP_90 EDP_17 FRAG_214 LOSS_214 10;
compgroup COMPGROUP_91 EDP_19 FRAG_214 LOSS_214 10;
compgroup COMPGROUP_92 EDP_2 FRAG_108 LOSS_108 10;
compgroup COMPGROUP_93 EDP_4 FRAG_108 LOSS_108 10;
compgroup COMPGROUP_94 EDP_6 FRAG_108 LOSS_108 10;
compgroup COMPGROUP_95 EDP_8 FRAG_108 LOSS_108 10;
compgroup COMPGROUP_96 EDP_10 FRAG_108 LOSS_108 10;
compgroup COMPGROUP_97 EDP_12 FRAG_108 LOSS_108 10;
compgroup COMPGROUP_98 EDP_14 FRAG_108 LOSS_108 10;
compgroup COMPGROUP_99 EDP_16 FRAG_108 LOSS_108 10;
compgroup COMPGROUP_100 EDP_18 FRAG_108 LOSS_108 10;
compgroup COMPGROUP_101 EDP_20 FRAG_108 LOSS_108 10;
compgroup COMPGROUP_102 EDP_1 FRAG_208 LOSS_208 53;
compgroup COMPGROUP_103 EDP_3 FRAG_208 LOSS_208 53;
compgroup COMPGROUP_104 EDP_5 FRAG_208 LOSS_208 53;
compgroup COMPGROUP_105 EDP_7 FRAG_208 LOSS_208 53;
compgroup COMPGROUP_106 EDP_9 FRAG_208 LOSS_208 53;
compgroup COMPGROUP_107 EDP_11 FRAG_208 LOSS_208 53;
compgroup COMPGROUP_108 EDP_13 FRAG_208 LOSS_208 53;
compgroup COMPGROUP_109 EDP_15 FRAG_208 LOSS_208 53;
compgroup COMPGROUP_110 EDP_17 FRAG_208 LOSS_208 53;
compgroup COMPGROUP_111 EDP_19 FRAG_208 LOSS_208 53;
compgroup COMPGROUP_112 EDP_5 FRAG_209 LOSS_209 16;
compgroup COMPGROUP_113 EDP_11 FRAG_209 LOSS_209 16;
compgroup COMPGROUP_114 EDP_19 FRAG_209 LOSS_209 16;
compgroup COMPGROUP_115 EDP_1 FRAG_210 LOSS_210 80;
compgroup COMPGROUP_116 EDP_3 FRAG_210 LOSS_210 80;
compgroup COMPGROUP_117 EDP_5 FRAG_210 LOSS_210 80;
compgroup COMPGROUP_118 EDP_7 FRAG_210 LOSS_210 80;
compgroup COMPGROUP_119 EDP_9 FRAG_210 LOSS_210 80;
compgroup COMPGROUP_120 EDP_11 FRAG_210 LOSS_210 80;
compgroup COMPGROUP_121 EDP_13 FRAG_210 LOSS_210 80;
compgroup COMPGROUP_122 EDP_15 FRAG_210 LOSS_210 80;
compgroup COMPGROUP_123 EDP_17 FRAG_210 LOSS_210 80;
compgroup COMPGROUP_124 EDP_19 FRAG_210 LOSS_210 80;
compgroup COMPGROUP_125 EDP_1 FRAG_204 LOSS_204 2;"



(let ((result "structure BUILDING"))
  (dotimes (i 125 result)
    (setq result (format "%s COMPGROUP_%d%s" result (+ 1 i) (if (eq i 144) ";" ",")))))
"structure BUILDING COMPGROUP_1, COMPGROUP_2, COMPGROUP_3, COMPGROUP_4, COMPGROUP_5, COMPGROUP_6, COMPGROUP_7, COMPGROUP_8, COMPGROUP_9, COMPGROUP_10, COMPGROUP_11, COMPGROUP_12, COMPGROUP_13, COMPGROUP_14, COMPGROUP_15, COMPGROUP_16, COMPGROUP_17, COMPGROUP_18, COMPGROUP_19, COMPGROUP_20, COMPGROUP_21, COMPGROUP_22, COMPGROUP_23, COMPGROUP_24, COMPGROUP_25, COMPGROUP_26, COMPGROUP_27, COMPGROUP_28, COMPGROUP_29, COMPGROUP_30, COMPGROUP_31, COMPGROUP_32, COMPGROUP_33, COMPGROUP_34, COMPGROUP_35, COMPGROUP_36, COMPGROUP_37, COMPGROUP_38, COMPGROUP_39, COMPGROUP_40, COMPGROUP_41, COMPGROUP_42, COMPGROUP_43, COMPGROUP_44, COMPGROUP_45, COMPGROUP_46, COMPGROUP_47, COMPGROUP_48, COMPGROUP_49, COMPGROUP_50, COMPGROUP_51, COMPGROUP_52, COMPGROUP_53, COMPGROUP_54, COMPGROUP_55, COMPGROUP_56, COMPGROUP_57, COMPGROUP_58, COMPGROUP_59, COMPGROUP_60, COMPGROUP_61, COMPGROUP_62, COMPGROUP_63, COMPGROUP_64, COMPGROUP_65, COMPGROUP_66, COMPGROUP_67, COMPGROUP_68, COMPGROUP_69, COMPGROUP_70, COMPGROUP_71, COMPGROUP_72, COMPGROUP_73, COMPGROUP_74, COMPGROUP_75, COMPGROUP_76, COMPGROUP_77, COMPGROUP_78, COMPGROUP_79, COMPGROUP_80, COMPGROUP_81, COMPGROUP_82, COMPGROUP_83, COMPGROUP_84, COMPGROUP_85, COMPGROUP_86, COMPGROUP_87, COMPGROUP_88, COMPGROUP_89, COMPGROUP_90, COMPGROUP_91, COMPGROUP_92, COMPGROUP_93, COMPGROUP_94, COMPGROUP_95, COMPGROUP_96, COMPGROUP_97, COMPGROUP_98, COMPGROUP_99, COMPGROUP_100, COMPGROUP_101, COMPGROUP_102, COMPGROUP_103, COMPGROUP_104, COMPGROUP_105, COMPGROUP_106, COMPGROUP_107, COMPGROUP_108, COMPGROUP_109, COMPGROUP_110, COMPGROUP_111, COMPGROUP_112, COMPGROUP_113, COMPGROUP_114, COMPGROUP_115, COMPGROUP_116, COMPGROUP_117, COMPGROUP_118, COMPGROUP_119, COMPGROUP_120, COMPGROUP_121, COMPGROUP_122, COMPGROUP_123, COMPGROUP_124, COMPGROUP_125,"




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
  (dotimes (cg 125 result)
    (setq result
	  (format "%s\n\nrecorder lossedp COMPGROUP_%d $lossedpvalues results/loss_%d_edp.txt --new;"
		  result (+ cg 1) (+ cg 1)))
    (setq result
	  (format "%s\nrecorder lossim COMPGROUP_%d $lossimvalues results/loss_%d_im.txt --new;"
		  result (+ 1 cg) (+ cg 1)))
    (setq result
	  (format "%s\n### recorder dsrate COMPGROUP_%d;" result (+ 1 cg)))
    (setq result
	  (format "%s\nrecorder dsedp COMPGROUP_%d 0.0:0.01:0.200 results/ds_edp_%d.txt --new;"
		  result (+ 1 cg) (+ cg 1)))
    (setq result
	  (format "%s\nrecorder lossrate COMPGROUP_%d 1E-4:4.8E-3:1.2 results/loss_rate_%d.txt --new;"
		  result (+ 1 cg) (+ cg 1)))))
"

recorder lossedp COMPGROUP_1 $lossedpvalues results/loss_1_edp.txt --new;
recorder lossim COMPGROUP_1 $lossimvalues results/loss_1_im.txt --new;
### recorder dsrate COMPGROUP_1;
recorder dsedp COMPGROUP_1 0.0:0.01:0.200 results/ds_edp_1.txt --new;
recorder lossrate COMPGROUP_1 1E-4:4.8E-3:1.2 results/loss_rate_1.txt --new;

recorder lossedp COMPGROUP_2 $lossedpvalues results/loss_2_edp.txt --new;
recorder lossim COMPGROUP_2 $lossimvalues results/loss_2_im.txt --new;
### recorder dsrate COMPGROUP_2;
recorder dsedp COMPGROUP_2 0.0:0.01:0.200 results/ds_edp_2.txt --new;
recorder lossrate COMPGROUP_2 1E-4:4.8E-3:1.2 results/loss_rate_2.txt --new;

recorder lossedp COMPGROUP_3 $lossedpvalues results/loss_3_edp.txt --new;
recorder lossim COMPGROUP_3 $lossimvalues results/loss_3_im.txt --new;
### recorder dsrate COMPGROUP_3;
recorder dsedp COMPGROUP_3 0.0:0.01:0.200 results/ds_edp_3.txt --new;
recorder lossrate COMPGROUP_3 1E-4:4.8E-3:1.2 results/loss_rate_3.txt --new;

recorder lossedp COMPGROUP_4 $lossedpvalues results/loss_4_edp.txt --new;
recorder lossim COMPGROUP_4 $lossimvalues results/loss_4_im.txt --new;
### recorder dsrate COMPGROUP_4;
recorder dsedp COMPGROUP_4 0.0:0.01:0.200 results/ds_edp_4.txt --new;
recorder lossrate COMPGROUP_4 1E-4:4.8E-3:1.2 results/loss_rate_4.txt --new;

recorder lossedp COMPGROUP_5 $lossedpvalues results/loss_5_edp.txt --new;
recorder lossim COMPGROUP_5 $lossimvalues results/loss_5_im.txt --new;
### recorder dsrate COMPGROUP_5;
recorder dsedp COMPGROUP_5 0.0:0.01:0.200 results/ds_edp_5.txt --new;
recorder lossrate COMPGROUP_5 1E-4:4.8E-3:1.2 results/loss_rate_5.txt --new;

recorder lossedp COMPGROUP_6 $lossedpvalues results/loss_6_edp.txt --new;
recorder lossim COMPGROUP_6 $lossimvalues results/loss_6_im.txt --new;
### recorder dsrate COMPGROUP_6;
recorder dsedp COMPGROUP_6 0.0:0.01:0.200 results/ds_edp_6.txt --new;
recorder lossrate COMPGROUP_6 1E-4:4.8E-3:1.2 results/loss_rate_6.txt --new;

recorder lossedp COMPGROUP_7 $lossedpvalues results/loss_7_edp.txt --new;
recorder lossim COMPGROUP_7 $lossimvalues results/loss_7_im.txt --new;
### recorder dsrate COMPGROUP_7;
recorder dsedp COMPGROUP_7 0.0:0.01:0.200 results/ds_edp_7.txt --new;
recorder lossrate COMPGROUP_7 1E-4:4.8E-3:1.2 results/loss_rate_7.txt --new;

recorder lossedp COMPGROUP_8 $lossedpvalues results/loss_8_edp.txt --new;
recorder lossim COMPGROUP_8 $lossimvalues results/loss_8_im.txt --new;
### recorder dsrate COMPGROUP_8;
recorder dsedp COMPGROUP_8 0.0:0.01:0.200 results/ds_edp_8.txt --new;
recorder lossrate COMPGROUP_8 1E-4:4.8E-3:1.2 results/loss_rate_8.txt --new;

recorder lossedp COMPGROUP_9 $lossedpvalues results/loss_9_edp.txt --new;
recorder lossim COMPGROUP_9 $lossimvalues results/loss_9_im.txt --new;
### recorder dsrate COMPGROUP_9;
recorder dsedp COMPGROUP_9 0.0:0.01:0.200 results/ds_edp_9.txt --new;
recorder lossrate COMPGROUP_9 1E-4:4.8E-3:1.2 results/loss_rate_9.txt --new;

recorder lossedp COMPGROUP_10 $lossedpvalues results/loss_10_edp.txt --new;
recorder lossim COMPGROUP_10 $lossimvalues results/loss_10_im.txt --new;
### recorder dsrate COMPGROUP_10;
recorder dsedp COMPGROUP_10 0.0:0.01:0.200 results/ds_edp_10.txt --new;
recorder lossrate COMPGROUP_10 1E-4:4.8E-3:1.2 results/loss_rate_10.txt --new;

recorder lossedp COMPGROUP_11 $lossedpvalues results/loss_11_edp.txt --new;
recorder lossim COMPGROUP_11 $lossimvalues results/loss_11_im.txt --new;
### recorder dsrate COMPGROUP_11;
recorder dsedp COMPGROUP_11 0.0:0.01:0.200 results/ds_edp_11.txt --new;
recorder lossrate COMPGROUP_11 1E-4:4.8E-3:1.2 results/loss_rate_11.txt --new;

recorder lossedp COMPGROUP_12 $lossedpvalues results/loss_12_edp.txt --new;
recorder lossim COMPGROUP_12 $lossimvalues results/loss_12_im.txt --new;
### recorder dsrate COMPGROUP_12;
recorder dsedp COMPGROUP_12 0.0:0.01:0.200 results/ds_edp_12.txt --new;
recorder lossrate COMPGROUP_12 1E-4:4.8E-3:1.2 results/loss_rate_12.txt --new;

recorder lossedp COMPGROUP_13 $lossedpvalues results/loss_13_edp.txt --new;
recorder lossim COMPGROUP_13 $lossimvalues results/loss_13_im.txt --new;
### recorder dsrate COMPGROUP_13;
recorder dsedp COMPGROUP_13 0.0:0.01:0.200 results/ds_edp_13.txt --new;
recorder lossrate COMPGROUP_13 1E-4:4.8E-3:1.2 results/loss_rate_13.txt --new;

recorder lossedp COMPGROUP_14 $lossedpvalues results/loss_14_edp.txt --new;
recorder lossim COMPGROUP_14 $lossimvalues results/loss_14_im.txt --new;
### recorder dsrate COMPGROUP_14;
recorder dsedp COMPGROUP_14 0.0:0.01:0.200 results/ds_edp_14.txt --new;
recorder lossrate COMPGROUP_14 1E-4:4.8E-3:1.2 results/loss_rate_14.txt --new;

recorder lossedp COMPGROUP_15 $lossedpvalues results/loss_15_edp.txt --new;
recorder lossim COMPGROUP_15 $lossimvalues results/loss_15_im.txt --new;
### recorder dsrate COMPGROUP_15;
recorder dsedp COMPGROUP_15 0.0:0.01:0.200 results/ds_edp_15.txt --new;
recorder lossrate COMPGROUP_15 1E-4:4.8E-3:1.2 results/loss_rate_15.txt --new;

recorder lossedp COMPGROUP_16 $lossedpvalues results/loss_16_edp.txt --new;
recorder lossim COMPGROUP_16 $lossimvalues results/loss_16_im.txt --new;
### recorder dsrate COMPGROUP_16;
recorder dsedp COMPGROUP_16 0.0:0.01:0.200 results/ds_edp_16.txt --new;
recorder lossrate COMPGROUP_16 1E-4:4.8E-3:1.2 results/loss_rate_16.txt --new;

recorder lossedp COMPGROUP_17 $lossedpvalues results/loss_17_edp.txt --new;
recorder lossim COMPGROUP_17 $lossimvalues results/loss_17_im.txt --new;
### recorder dsrate COMPGROUP_17;
recorder dsedp COMPGROUP_17 0.0:0.01:0.200 results/ds_edp_17.txt --new;
recorder lossrate COMPGROUP_17 1E-4:4.8E-3:1.2 results/loss_rate_17.txt --new;

recorder lossedp COMPGROUP_18 $lossedpvalues results/loss_18_edp.txt --new;
recorder lossim COMPGROUP_18 $lossimvalues results/loss_18_im.txt --new;
### recorder dsrate COMPGROUP_18;
recorder dsedp COMPGROUP_18 0.0:0.01:0.200 results/ds_edp_18.txt --new;
recorder lossrate COMPGROUP_18 1E-4:4.8E-3:1.2 results/loss_rate_18.txt --new;

recorder lossedp COMPGROUP_19 $lossedpvalues results/loss_19_edp.txt --new;
recorder lossim COMPGROUP_19 $lossimvalues results/loss_19_im.txt --new;
### recorder dsrate COMPGROUP_19;
recorder dsedp COMPGROUP_19 0.0:0.01:0.200 results/ds_edp_19.txt --new;
recorder lossrate COMPGROUP_19 1E-4:4.8E-3:1.2 results/loss_rate_19.txt --new;

recorder lossedp COMPGROUP_20 $lossedpvalues results/loss_20_edp.txt --new;
recorder lossim COMPGROUP_20 $lossimvalues results/loss_20_im.txt --new;
### recorder dsrate COMPGROUP_20;
recorder dsedp COMPGROUP_20 0.0:0.01:0.200 results/ds_edp_20.txt --new;
recorder lossrate COMPGROUP_20 1E-4:4.8E-3:1.2 results/loss_rate_20.txt --new;

recorder lossedp COMPGROUP_21 $lossedpvalues results/loss_21_edp.txt --new;
recorder lossim COMPGROUP_21 $lossimvalues results/loss_21_im.txt --new;
### recorder dsrate COMPGROUP_21;
recorder dsedp COMPGROUP_21 0.0:0.01:0.200 results/ds_edp_21.txt --new;
recorder lossrate COMPGROUP_21 1E-4:4.8E-3:1.2 results/loss_rate_21.txt --new;

recorder lossedp COMPGROUP_22 $lossedpvalues results/loss_22_edp.txt --new;
recorder lossim COMPGROUP_22 $lossimvalues results/loss_22_im.txt --new;
### recorder dsrate COMPGROUP_22;
recorder dsedp COMPGROUP_22 0.0:0.01:0.200 results/ds_edp_22.txt --new;
recorder lossrate COMPGROUP_22 1E-4:4.8E-3:1.2 results/loss_rate_22.txt --new;

recorder lossedp COMPGROUP_23 $lossedpvalues results/loss_23_edp.txt --new;
recorder lossim COMPGROUP_23 $lossimvalues results/loss_23_im.txt --new;
### recorder dsrate COMPGROUP_23;
recorder dsedp COMPGROUP_23 0.0:0.01:0.200 results/ds_edp_23.txt --new;
recorder lossrate COMPGROUP_23 1E-4:4.8E-3:1.2 results/loss_rate_23.txt --new;

recorder lossedp COMPGROUP_24 $lossedpvalues results/loss_24_edp.txt --new;
recorder lossim COMPGROUP_24 $lossimvalues results/loss_24_im.txt --new;
### recorder dsrate COMPGROUP_24;
recorder dsedp COMPGROUP_24 0.0:0.01:0.200 results/ds_edp_24.txt --new;
recorder lossrate COMPGROUP_24 1E-4:4.8E-3:1.2 results/loss_rate_24.txt --new;

recorder lossedp COMPGROUP_25 $lossedpvalues results/loss_25_edp.txt --new;
recorder lossim COMPGROUP_25 $lossimvalues results/loss_25_im.txt --new;
### recorder dsrate COMPGROUP_25;
recorder dsedp COMPGROUP_25 0.0:0.01:0.200 results/ds_edp_25.txt --new;
recorder lossrate COMPGROUP_25 1E-4:4.8E-3:1.2 results/loss_rate_25.txt --new;

recorder lossedp COMPGROUP_26 $lossedpvalues results/loss_26_edp.txt --new;
recorder lossim COMPGROUP_26 $lossimvalues results/loss_26_im.txt --new;
### recorder dsrate COMPGROUP_26;
recorder dsedp COMPGROUP_26 0.0:0.01:0.200 results/ds_edp_26.txt --new;
recorder lossrate COMPGROUP_26 1E-4:4.8E-3:1.2 results/loss_rate_26.txt --new;

recorder lossedp COMPGROUP_27 $lossedpvalues results/loss_27_edp.txt --new;
recorder lossim COMPGROUP_27 $lossimvalues results/loss_27_im.txt --new;
### recorder dsrate COMPGROUP_27;
recorder dsedp COMPGROUP_27 0.0:0.01:0.200 results/ds_edp_27.txt --new;
recorder lossrate COMPGROUP_27 1E-4:4.8E-3:1.2 results/loss_rate_27.txt --new;

recorder lossedp COMPGROUP_28 $lossedpvalues results/loss_28_edp.txt --new;
recorder lossim COMPGROUP_28 $lossimvalues results/loss_28_im.txt --new;
### recorder dsrate COMPGROUP_28;
recorder dsedp COMPGROUP_28 0.0:0.01:0.200 results/ds_edp_28.txt --new;
recorder lossrate COMPGROUP_28 1E-4:4.8E-3:1.2 results/loss_rate_28.txt --new;

recorder lossedp COMPGROUP_29 $lossedpvalues results/loss_29_edp.txt --new;
recorder lossim COMPGROUP_29 $lossimvalues results/loss_29_im.txt --new;
### recorder dsrate COMPGROUP_29;
recorder dsedp COMPGROUP_29 0.0:0.01:0.200 results/ds_edp_29.txt --new;
recorder lossrate COMPGROUP_29 1E-4:4.8E-3:1.2 results/loss_rate_29.txt --new;

recorder lossedp COMPGROUP_30 $lossedpvalues results/loss_30_edp.txt --new;
recorder lossim COMPGROUP_30 $lossimvalues results/loss_30_im.txt --new;
### recorder dsrate COMPGROUP_30;
recorder dsedp COMPGROUP_30 0.0:0.01:0.200 results/ds_edp_30.txt --new;
recorder lossrate COMPGROUP_30 1E-4:4.8E-3:1.2 results/loss_rate_30.txt --new;

recorder lossedp COMPGROUP_31 $lossedpvalues results/loss_31_edp.txt --new;
recorder lossim COMPGROUP_31 $lossimvalues results/loss_31_im.txt --new;
### recorder dsrate COMPGROUP_31;
recorder dsedp COMPGROUP_31 0.0:0.01:0.200 results/ds_edp_31.txt --new;
recorder lossrate COMPGROUP_31 1E-4:4.8E-3:1.2 results/loss_rate_31.txt --new;

recorder lossedp COMPGROUP_32 $lossedpvalues results/loss_32_edp.txt --new;
recorder lossim COMPGROUP_32 $lossimvalues results/loss_32_im.txt --new;
### recorder dsrate COMPGROUP_32;
recorder dsedp COMPGROUP_32 0.0:0.01:0.200 results/ds_edp_32.txt --new;
recorder lossrate COMPGROUP_32 1E-4:4.8E-3:1.2 results/loss_rate_32.txt --new;

recorder lossedp COMPGROUP_33 $lossedpvalues results/loss_33_edp.txt --new;
recorder lossim COMPGROUP_33 $lossimvalues results/loss_33_im.txt --new;
### recorder dsrate COMPGROUP_33;
recorder dsedp COMPGROUP_33 0.0:0.01:0.200 results/ds_edp_33.txt --new;
recorder lossrate COMPGROUP_33 1E-4:4.8E-3:1.2 results/loss_rate_33.txt --new;

recorder lossedp COMPGROUP_34 $lossedpvalues results/loss_34_edp.txt --new;
recorder lossim COMPGROUP_34 $lossimvalues results/loss_34_im.txt --new;
### recorder dsrate COMPGROUP_34;
recorder dsedp COMPGROUP_34 0.0:0.01:0.200 results/ds_edp_34.txt --new;
recorder lossrate COMPGROUP_34 1E-4:4.8E-3:1.2 results/loss_rate_34.txt --new;

recorder lossedp COMPGROUP_35 $lossedpvalues results/loss_35_edp.txt --new;
recorder lossim COMPGROUP_35 $lossimvalues results/loss_35_im.txt --new;
### recorder dsrate COMPGROUP_35;
recorder dsedp COMPGROUP_35 0.0:0.01:0.200 results/ds_edp_35.txt --new;
recorder lossrate COMPGROUP_35 1E-4:4.8E-3:1.2 results/loss_rate_35.txt --new;

recorder lossedp COMPGROUP_36 $lossedpvalues results/loss_36_edp.txt --new;
recorder lossim COMPGROUP_36 $lossimvalues results/loss_36_im.txt --new;
### recorder dsrate COMPGROUP_36;
recorder dsedp COMPGROUP_36 0.0:0.01:0.200 results/ds_edp_36.txt --new;
recorder lossrate COMPGROUP_36 1E-4:4.8E-3:1.2 results/loss_rate_36.txt --new;

recorder lossedp COMPGROUP_37 $lossedpvalues results/loss_37_edp.txt --new;
recorder lossim COMPGROUP_37 $lossimvalues results/loss_37_im.txt --new;
### recorder dsrate COMPGROUP_37;
recorder dsedp COMPGROUP_37 0.0:0.01:0.200 results/ds_edp_37.txt --new;
recorder lossrate COMPGROUP_37 1E-4:4.8E-3:1.2 results/loss_rate_37.txt --new;

recorder lossedp COMPGROUP_38 $lossedpvalues results/loss_38_edp.txt --new;
recorder lossim COMPGROUP_38 $lossimvalues results/loss_38_im.txt --new;
### recorder dsrate COMPGROUP_38;
recorder dsedp COMPGROUP_38 0.0:0.01:0.200 results/ds_edp_38.txt --new;
recorder lossrate COMPGROUP_38 1E-4:4.8E-3:1.2 results/loss_rate_38.txt --new;

recorder lossedp COMPGROUP_39 $lossedpvalues results/loss_39_edp.txt --new;
recorder lossim COMPGROUP_39 $lossimvalues results/loss_39_im.txt --new;
### recorder dsrate COMPGROUP_39;
recorder dsedp COMPGROUP_39 0.0:0.01:0.200 results/ds_edp_39.txt --new;
recorder lossrate COMPGROUP_39 1E-4:4.8E-3:1.2 results/loss_rate_39.txt --new;

recorder lossedp COMPGROUP_40 $lossedpvalues results/loss_40_edp.txt --new;
recorder lossim COMPGROUP_40 $lossimvalues results/loss_40_im.txt --new;
### recorder dsrate COMPGROUP_40;
recorder dsedp COMPGROUP_40 0.0:0.01:0.200 results/ds_edp_40.txt --new;
recorder lossrate COMPGROUP_40 1E-4:4.8E-3:1.2 results/loss_rate_40.txt --new;

recorder lossedp COMPGROUP_41 $lossedpvalues results/loss_41_edp.txt --new;
recorder lossim COMPGROUP_41 $lossimvalues results/loss_41_im.txt --new;
### recorder dsrate COMPGROUP_41;
recorder dsedp COMPGROUP_41 0.0:0.01:0.200 results/ds_edp_41.txt --new;
recorder lossrate COMPGROUP_41 1E-4:4.8E-3:1.2 results/loss_rate_41.txt --new;

recorder lossedp COMPGROUP_42 $lossedpvalues results/loss_42_edp.txt --new;
recorder lossim COMPGROUP_42 $lossimvalues results/loss_42_im.txt --new;
### recorder dsrate COMPGROUP_42;
recorder dsedp COMPGROUP_42 0.0:0.01:0.200 results/ds_edp_42.txt --new;
recorder lossrate COMPGROUP_42 1E-4:4.8E-3:1.2 results/loss_rate_42.txt --new;

recorder lossedp COMPGROUP_43 $lossedpvalues results/loss_43_edp.txt --new;
recorder lossim COMPGROUP_43 $lossimvalues results/loss_43_im.txt --new;
### recorder dsrate COMPGROUP_43;
recorder dsedp COMPGROUP_43 0.0:0.01:0.200 results/ds_edp_43.txt --new;
recorder lossrate COMPGROUP_43 1E-4:4.8E-3:1.2 results/loss_rate_43.txt --new;

recorder lossedp COMPGROUP_44 $lossedpvalues results/loss_44_edp.txt --new;
recorder lossim COMPGROUP_44 $lossimvalues results/loss_44_im.txt --new;
### recorder dsrate COMPGROUP_44;
recorder dsedp COMPGROUP_44 0.0:0.01:0.200 results/ds_edp_44.txt --new;
recorder lossrate COMPGROUP_44 1E-4:4.8E-3:1.2 results/loss_rate_44.txt --new;

recorder lossedp COMPGROUP_45 $lossedpvalues results/loss_45_edp.txt --new;
recorder lossim COMPGROUP_45 $lossimvalues results/loss_45_im.txt --new;
### recorder dsrate COMPGROUP_45;
recorder dsedp COMPGROUP_45 0.0:0.01:0.200 results/ds_edp_45.txt --new;
recorder lossrate COMPGROUP_45 1E-4:4.8E-3:1.2 results/loss_rate_45.txt --new;

recorder lossedp COMPGROUP_46 $lossedpvalues results/loss_46_edp.txt --new;
recorder lossim COMPGROUP_46 $lossimvalues results/loss_46_im.txt --new;
### recorder dsrate COMPGROUP_46;
recorder dsedp COMPGROUP_46 0.0:0.01:0.200 results/ds_edp_46.txt --new;
recorder lossrate COMPGROUP_46 1E-4:4.8E-3:1.2 results/loss_rate_46.txt --new;

recorder lossedp COMPGROUP_47 $lossedpvalues results/loss_47_edp.txt --new;
recorder lossim COMPGROUP_47 $lossimvalues results/loss_47_im.txt --new;
### recorder dsrate COMPGROUP_47;
recorder dsedp COMPGROUP_47 0.0:0.01:0.200 results/ds_edp_47.txt --new;
recorder lossrate COMPGROUP_47 1E-4:4.8E-3:1.2 results/loss_rate_47.txt --new;

recorder lossedp COMPGROUP_48 $lossedpvalues results/loss_48_edp.txt --new;
recorder lossim COMPGROUP_48 $lossimvalues results/loss_48_im.txt --new;
### recorder dsrate COMPGROUP_48;
recorder dsedp COMPGROUP_48 0.0:0.01:0.200 results/ds_edp_48.txt --new;
recorder lossrate COMPGROUP_48 1E-4:4.8E-3:1.2 results/loss_rate_48.txt --new;

recorder lossedp COMPGROUP_49 $lossedpvalues results/loss_49_edp.txt --new;
recorder lossim COMPGROUP_49 $lossimvalues results/loss_49_im.txt --new;
### recorder dsrate COMPGROUP_49;
recorder dsedp COMPGROUP_49 0.0:0.01:0.200 results/ds_edp_49.txt --new;
recorder lossrate COMPGROUP_49 1E-4:4.8E-3:1.2 results/loss_rate_49.txt --new;

recorder lossedp COMPGROUP_50 $lossedpvalues results/loss_50_edp.txt --new;
recorder lossim COMPGROUP_50 $lossimvalues results/loss_50_im.txt --new;
### recorder dsrate COMPGROUP_50;
recorder dsedp COMPGROUP_50 0.0:0.01:0.200 results/ds_edp_50.txt --new;
recorder lossrate COMPGROUP_50 1E-4:4.8E-3:1.2 results/loss_rate_50.txt --new;

recorder lossedp COMPGROUP_51 $lossedpvalues results/loss_51_edp.txt --new;
recorder lossim COMPGROUP_51 $lossimvalues results/loss_51_im.txt --new;
### recorder dsrate COMPGROUP_51;
recorder dsedp COMPGROUP_51 0.0:0.01:0.200 results/ds_edp_51.txt --new;
recorder lossrate COMPGROUP_51 1E-4:4.8E-3:1.2 results/loss_rate_51.txt --new;

recorder lossedp COMPGROUP_52 $lossedpvalues results/loss_52_edp.txt --new;
recorder lossim COMPGROUP_52 $lossimvalues results/loss_52_im.txt --new;
### recorder dsrate COMPGROUP_52;
recorder dsedp COMPGROUP_52 0.0:0.01:0.200 results/ds_edp_52.txt --new;
recorder lossrate COMPGROUP_52 1E-4:4.8E-3:1.2 results/loss_rate_52.txt --new;

recorder lossedp COMPGROUP_53 $lossedpvalues results/loss_53_edp.txt --new;
recorder lossim COMPGROUP_53 $lossimvalues results/loss_53_im.txt --new;
### recorder dsrate COMPGROUP_53;
recorder dsedp COMPGROUP_53 0.0:0.01:0.200 results/ds_edp_53.txt --new;
recorder lossrate COMPGROUP_53 1E-4:4.8E-3:1.2 results/loss_rate_53.txt --new;

recorder lossedp COMPGROUP_54 $lossedpvalues results/loss_54_edp.txt --new;
recorder lossim COMPGROUP_54 $lossimvalues results/loss_54_im.txt --new;
### recorder dsrate COMPGROUP_54;
recorder dsedp COMPGROUP_54 0.0:0.01:0.200 results/ds_edp_54.txt --new;
recorder lossrate COMPGROUP_54 1E-4:4.8E-3:1.2 results/loss_rate_54.txt --new;

recorder lossedp COMPGROUP_55 $lossedpvalues results/loss_55_edp.txt --new;
recorder lossim COMPGROUP_55 $lossimvalues results/loss_55_im.txt --new;
### recorder dsrate COMPGROUP_55;
recorder dsedp COMPGROUP_55 0.0:0.01:0.200 results/ds_edp_55.txt --new;
recorder lossrate COMPGROUP_55 1E-4:4.8E-3:1.2 results/loss_rate_55.txt --new;

recorder lossedp COMPGROUP_56 $lossedpvalues results/loss_56_edp.txt --new;
recorder lossim COMPGROUP_56 $lossimvalues results/loss_56_im.txt --new;
### recorder dsrate COMPGROUP_56;
recorder dsedp COMPGROUP_56 0.0:0.01:0.200 results/ds_edp_56.txt --new;
recorder lossrate COMPGROUP_56 1E-4:4.8E-3:1.2 results/loss_rate_56.txt --new;

recorder lossedp COMPGROUP_57 $lossedpvalues results/loss_57_edp.txt --new;
recorder lossim COMPGROUP_57 $lossimvalues results/loss_57_im.txt --new;
### recorder dsrate COMPGROUP_57;
recorder dsedp COMPGROUP_57 0.0:0.01:0.200 results/ds_edp_57.txt --new;
recorder lossrate COMPGROUP_57 1E-4:4.8E-3:1.2 results/loss_rate_57.txt --new;

recorder lossedp COMPGROUP_58 $lossedpvalues results/loss_58_edp.txt --new;
recorder lossim COMPGROUP_58 $lossimvalues results/loss_58_im.txt --new;
### recorder dsrate COMPGROUP_58;
recorder dsedp COMPGROUP_58 0.0:0.01:0.200 results/ds_edp_58.txt --new;
recorder lossrate COMPGROUP_58 1E-4:4.8E-3:1.2 results/loss_rate_58.txt --new;

recorder lossedp COMPGROUP_59 $lossedpvalues results/loss_59_edp.txt --new;
recorder lossim COMPGROUP_59 $lossimvalues results/loss_59_im.txt --new;
### recorder dsrate COMPGROUP_59;
recorder dsedp COMPGROUP_59 0.0:0.01:0.200 results/ds_edp_59.txt --new;
recorder lossrate COMPGROUP_59 1E-4:4.8E-3:1.2 results/loss_rate_59.txt --new;

recorder lossedp COMPGROUP_60 $lossedpvalues results/loss_60_edp.txt --new;
recorder lossim COMPGROUP_60 $lossimvalues results/loss_60_im.txt --new;
### recorder dsrate COMPGROUP_60;
recorder dsedp COMPGROUP_60 0.0:0.01:0.200 results/ds_edp_60.txt --new;
recorder lossrate COMPGROUP_60 1E-4:4.8E-3:1.2 results/loss_rate_60.txt --new;

recorder lossedp COMPGROUP_61 $lossedpvalues results/loss_61_edp.txt --new;
recorder lossim COMPGROUP_61 $lossimvalues results/loss_61_im.txt --new;
### recorder dsrate COMPGROUP_61;
recorder dsedp COMPGROUP_61 0.0:0.01:0.200 results/ds_edp_61.txt --new;
recorder lossrate COMPGROUP_61 1E-4:4.8E-3:1.2 results/loss_rate_61.txt --new;

recorder lossedp COMPGROUP_62 $lossedpvalues results/loss_62_edp.txt --new;
recorder lossim COMPGROUP_62 $lossimvalues results/loss_62_im.txt --new;
### recorder dsrate COMPGROUP_62;
recorder dsedp COMPGROUP_62 0.0:0.01:0.200 results/ds_edp_62.txt --new;
recorder lossrate COMPGROUP_62 1E-4:4.8E-3:1.2 results/loss_rate_62.txt --new;

recorder lossedp COMPGROUP_63 $lossedpvalues results/loss_63_edp.txt --new;
recorder lossim COMPGROUP_63 $lossimvalues results/loss_63_im.txt --new;
### recorder dsrate COMPGROUP_63;
recorder dsedp COMPGROUP_63 0.0:0.01:0.200 results/ds_edp_63.txt --new;
recorder lossrate COMPGROUP_63 1E-4:4.8E-3:1.2 results/loss_rate_63.txt --new;

recorder lossedp COMPGROUP_64 $lossedpvalues results/loss_64_edp.txt --new;
recorder lossim COMPGROUP_64 $lossimvalues results/loss_64_im.txt --new;
### recorder dsrate COMPGROUP_64;
recorder dsedp COMPGROUP_64 0.0:0.01:0.200 results/ds_edp_64.txt --new;
recorder lossrate COMPGROUP_64 1E-4:4.8E-3:1.2 results/loss_rate_64.txt --new;

recorder lossedp COMPGROUP_65 $lossedpvalues results/loss_65_edp.txt --new;
recorder lossim COMPGROUP_65 $lossimvalues results/loss_65_im.txt --new;
### recorder dsrate COMPGROUP_65;
recorder dsedp COMPGROUP_65 0.0:0.01:0.200 results/ds_edp_65.txt --new;
recorder lossrate COMPGROUP_65 1E-4:4.8E-3:1.2 results/loss_rate_65.txt --new;

recorder lossedp COMPGROUP_66 $lossedpvalues results/loss_66_edp.txt --new;
recorder lossim COMPGROUP_66 $lossimvalues results/loss_66_im.txt --new;
### recorder dsrate COMPGROUP_66;
recorder dsedp COMPGROUP_66 0.0:0.01:0.200 results/ds_edp_66.txt --new;
recorder lossrate COMPGROUP_66 1E-4:4.8E-3:1.2 results/loss_rate_66.txt --new;

recorder lossedp COMPGROUP_67 $lossedpvalues results/loss_67_edp.txt --new;
recorder lossim COMPGROUP_67 $lossimvalues results/loss_67_im.txt --new;
### recorder dsrate COMPGROUP_67;
recorder dsedp COMPGROUP_67 0.0:0.01:0.200 results/ds_edp_67.txt --new;
recorder lossrate COMPGROUP_67 1E-4:4.8E-3:1.2 results/loss_rate_67.txt --new;

recorder lossedp COMPGROUP_68 $lossedpvalues results/loss_68_edp.txt --new;
recorder lossim COMPGROUP_68 $lossimvalues results/loss_68_im.txt --new;
### recorder dsrate COMPGROUP_68;
recorder dsedp COMPGROUP_68 0.0:0.01:0.200 results/ds_edp_68.txt --new;
recorder lossrate COMPGROUP_68 1E-4:4.8E-3:1.2 results/loss_rate_68.txt --new;

recorder lossedp COMPGROUP_69 $lossedpvalues results/loss_69_edp.txt --new;
recorder lossim COMPGROUP_69 $lossimvalues results/loss_69_im.txt --new;
### recorder dsrate COMPGROUP_69;
recorder dsedp COMPGROUP_69 0.0:0.01:0.200 results/ds_edp_69.txt --new;
recorder lossrate COMPGROUP_69 1E-4:4.8E-3:1.2 results/loss_rate_69.txt --new;

recorder lossedp COMPGROUP_70 $lossedpvalues results/loss_70_edp.txt --new;
recorder lossim COMPGROUP_70 $lossimvalues results/loss_70_im.txt --new;
### recorder dsrate COMPGROUP_70;
recorder dsedp COMPGROUP_70 0.0:0.01:0.200 results/ds_edp_70.txt --new;
recorder lossrate COMPGROUP_70 1E-4:4.8E-3:1.2 results/loss_rate_70.txt --new;

recorder lossedp COMPGROUP_71 $lossedpvalues results/loss_71_edp.txt --new;
recorder lossim COMPGROUP_71 $lossimvalues results/loss_71_im.txt --new;
### recorder dsrate COMPGROUP_71;
recorder dsedp COMPGROUP_71 0.0:0.01:0.200 results/ds_edp_71.txt --new;
recorder lossrate COMPGROUP_71 1E-4:4.8E-3:1.2 results/loss_rate_71.txt --new;

recorder lossedp COMPGROUP_72 $lossedpvalues results/loss_72_edp.txt --new;
recorder lossim COMPGROUP_72 $lossimvalues results/loss_72_im.txt --new;
### recorder dsrate COMPGROUP_72;
recorder dsedp COMPGROUP_72 0.0:0.01:0.200 results/ds_edp_72.txt --new;
recorder lossrate COMPGROUP_72 1E-4:4.8E-3:1.2 results/loss_rate_72.txt --new;

recorder lossedp COMPGROUP_73 $lossedpvalues results/loss_73_edp.txt --new;
recorder lossim COMPGROUP_73 $lossimvalues results/loss_73_im.txt --new;
### recorder dsrate COMPGROUP_73;
recorder dsedp COMPGROUP_73 0.0:0.01:0.200 results/ds_edp_73.txt --new;
recorder lossrate COMPGROUP_73 1E-4:4.8E-3:1.2 results/loss_rate_73.txt --new;

recorder lossedp COMPGROUP_74 $lossedpvalues results/loss_74_edp.txt --new;
recorder lossim COMPGROUP_74 $lossimvalues results/loss_74_im.txt --new;
### recorder dsrate COMPGROUP_74;
recorder dsedp COMPGROUP_74 0.0:0.01:0.200 results/ds_edp_74.txt --new;
recorder lossrate COMPGROUP_74 1E-4:4.8E-3:1.2 results/loss_rate_74.txt --new;

recorder lossedp COMPGROUP_75 $lossedpvalues results/loss_75_edp.txt --new;
recorder lossim COMPGROUP_75 $lossimvalues results/loss_75_im.txt --new;
### recorder dsrate COMPGROUP_75;
recorder dsedp COMPGROUP_75 0.0:0.01:0.200 results/ds_edp_75.txt --new;
recorder lossrate COMPGROUP_75 1E-4:4.8E-3:1.2 results/loss_rate_75.txt --new;

recorder lossedp COMPGROUP_76 $lossedpvalues results/loss_76_edp.txt --new;
recorder lossim COMPGROUP_76 $lossimvalues results/loss_76_im.txt --new;
### recorder dsrate COMPGROUP_76;
recorder dsedp COMPGROUP_76 0.0:0.01:0.200 results/ds_edp_76.txt --new;
recorder lossrate COMPGROUP_76 1E-4:4.8E-3:1.2 results/loss_rate_76.txt --new;

recorder lossedp COMPGROUP_77 $lossedpvalues results/loss_77_edp.txt --new;
recorder lossim COMPGROUP_77 $lossimvalues results/loss_77_im.txt --new;
### recorder dsrate COMPGROUP_77;
recorder dsedp COMPGROUP_77 0.0:0.01:0.200 results/ds_edp_77.txt --new;
recorder lossrate COMPGROUP_77 1E-4:4.8E-3:1.2 results/loss_rate_77.txt --new;

recorder lossedp COMPGROUP_78 $lossedpvalues results/loss_78_edp.txt --new;
recorder lossim COMPGROUP_78 $lossimvalues results/loss_78_im.txt --new;
### recorder dsrate COMPGROUP_78;
recorder dsedp COMPGROUP_78 0.0:0.01:0.200 results/ds_edp_78.txt --new;
recorder lossrate COMPGROUP_78 1E-4:4.8E-3:1.2 results/loss_rate_78.txt --new;

recorder lossedp COMPGROUP_79 $lossedpvalues results/loss_79_edp.txt --new;
recorder lossim COMPGROUP_79 $lossimvalues results/loss_79_im.txt --new;
### recorder dsrate COMPGROUP_79;
recorder dsedp COMPGROUP_79 0.0:0.01:0.200 results/ds_edp_79.txt --new;
recorder lossrate COMPGROUP_79 1E-4:4.8E-3:1.2 results/loss_rate_79.txt --new;

recorder lossedp COMPGROUP_80 $lossedpvalues results/loss_80_edp.txt --new;
recorder lossim COMPGROUP_80 $lossimvalues results/loss_80_im.txt --new;
### recorder dsrate COMPGROUP_80;
recorder dsedp COMPGROUP_80 0.0:0.01:0.200 results/ds_edp_80.txt --new;
recorder lossrate COMPGROUP_80 1E-4:4.8E-3:1.2 results/loss_rate_80.txt --new;

recorder lossedp COMPGROUP_81 $lossedpvalues results/loss_81_edp.txt --new;
recorder lossim COMPGROUP_81 $lossimvalues results/loss_81_im.txt --new;
### recorder dsrate COMPGROUP_81;
recorder dsedp COMPGROUP_81 0.0:0.01:0.200 results/ds_edp_81.txt --new;
recorder lossrate COMPGROUP_81 1E-4:4.8E-3:1.2 results/loss_rate_81.txt --new;

recorder lossedp COMPGROUP_82 $lossedpvalues results/loss_82_edp.txt --new;
recorder lossim COMPGROUP_82 $lossimvalues results/loss_82_im.txt --new;
### recorder dsrate COMPGROUP_82;
recorder dsedp COMPGROUP_82 0.0:0.01:0.200 results/ds_edp_82.txt --new;
recorder lossrate COMPGROUP_82 1E-4:4.8E-3:1.2 results/loss_rate_82.txt --new;

recorder lossedp COMPGROUP_83 $lossedpvalues results/loss_83_edp.txt --new;
recorder lossim COMPGROUP_83 $lossimvalues results/loss_83_im.txt --new;
### recorder dsrate COMPGROUP_83;
recorder dsedp COMPGROUP_83 0.0:0.01:0.200 results/ds_edp_83.txt --new;
recorder lossrate COMPGROUP_83 1E-4:4.8E-3:1.2 results/loss_rate_83.txt --new;

recorder lossedp COMPGROUP_84 $lossedpvalues results/loss_84_edp.txt --new;
recorder lossim COMPGROUP_84 $lossimvalues results/loss_84_im.txt --new;
### recorder dsrate COMPGROUP_84;
recorder dsedp COMPGROUP_84 0.0:0.01:0.200 results/ds_edp_84.txt --new;
recorder lossrate COMPGROUP_84 1E-4:4.8E-3:1.2 results/loss_rate_84.txt --new;

recorder lossedp COMPGROUP_85 $lossedpvalues results/loss_85_edp.txt --new;
recorder lossim COMPGROUP_85 $lossimvalues results/loss_85_im.txt --new;
### recorder dsrate COMPGROUP_85;
recorder dsedp COMPGROUP_85 0.0:0.01:0.200 results/ds_edp_85.txt --new;
recorder lossrate COMPGROUP_85 1E-4:4.8E-3:1.2 results/loss_rate_85.txt --new;

recorder lossedp COMPGROUP_86 $lossedpvalues results/loss_86_edp.txt --new;
recorder lossim COMPGROUP_86 $lossimvalues results/loss_86_im.txt --new;
### recorder dsrate COMPGROUP_86;
recorder dsedp COMPGROUP_86 0.0:0.01:0.200 results/ds_edp_86.txt --new;
recorder lossrate COMPGROUP_86 1E-4:4.8E-3:1.2 results/loss_rate_86.txt --new;

recorder lossedp COMPGROUP_87 $lossedpvalues results/loss_87_edp.txt --new;
recorder lossim COMPGROUP_87 $lossimvalues results/loss_87_im.txt --new;
### recorder dsrate COMPGROUP_87;
recorder dsedp COMPGROUP_87 0.0:0.01:0.200 results/ds_edp_87.txt --new;
recorder lossrate COMPGROUP_87 1E-4:4.8E-3:1.2 results/loss_rate_87.txt --new;

recorder lossedp COMPGROUP_88 $lossedpvalues results/loss_88_edp.txt --new;
recorder lossim COMPGROUP_88 $lossimvalues results/loss_88_im.txt --new;
### recorder dsrate COMPGROUP_88;
recorder dsedp COMPGROUP_88 0.0:0.01:0.200 results/ds_edp_88.txt --new;
recorder lossrate COMPGROUP_88 1E-4:4.8E-3:1.2 results/loss_rate_88.txt --new;

recorder lossedp COMPGROUP_89 $lossedpvalues results/loss_89_edp.txt --new;
recorder lossim COMPGROUP_89 $lossimvalues results/loss_89_im.txt --new;
### recorder dsrate COMPGROUP_89;
recorder dsedp COMPGROUP_89 0.0:0.01:0.200 results/ds_edp_89.txt --new;
recorder lossrate COMPGROUP_89 1E-4:4.8E-3:1.2 results/loss_rate_89.txt --new;

recorder lossedp COMPGROUP_90 $lossedpvalues results/loss_90_edp.txt --new;
recorder lossim COMPGROUP_90 $lossimvalues results/loss_90_im.txt --new;
### recorder dsrate COMPGROUP_90;
recorder dsedp COMPGROUP_90 0.0:0.01:0.200 results/ds_edp_90.txt --new;
recorder lossrate COMPGROUP_90 1E-4:4.8E-3:1.2 results/loss_rate_90.txt --new;

recorder lossedp COMPGROUP_91 $lossedpvalues results/loss_91_edp.txt --new;
recorder lossim COMPGROUP_91 $lossimvalues results/loss_91_im.txt --new;
### recorder dsrate COMPGROUP_91;
recorder dsedp COMPGROUP_91 0.0:0.01:0.200 results/ds_edp_91.txt --new;
recorder lossrate COMPGROUP_91 1E-4:4.8E-3:1.2 results/loss_rate_91.txt --new;

recorder lossedp COMPGROUP_92 $lossedpvalues results/loss_92_edp.txt --new;
recorder lossim COMPGROUP_92 $lossimvalues results/loss_92_im.txt --new;
### recorder dsrate COMPGROUP_92;
recorder dsedp COMPGROUP_92 0.0:0.01:0.200 results/ds_edp_92.txt --new;
recorder lossrate COMPGROUP_92 1E-4:4.8E-3:1.2 results/loss_rate_92.txt --new;

recorder lossedp COMPGROUP_93 $lossedpvalues results/loss_93_edp.txt --new;
recorder lossim COMPGROUP_93 $lossimvalues results/loss_93_im.txt --new;
### recorder dsrate COMPGROUP_93;
recorder dsedp COMPGROUP_93 0.0:0.01:0.200 results/ds_edp_93.txt --new;
recorder lossrate COMPGROUP_93 1E-4:4.8E-3:1.2 results/loss_rate_93.txt --new;

recorder lossedp COMPGROUP_94 $lossedpvalues results/loss_94_edp.txt --new;
recorder lossim COMPGROUP_94 $lossimvalues results/loss_94_im.txt --new;
### recorder dsrate COMPGROUP_94;
recorder dsedp COMPGROUP_94 0.0:0.01:0.200 results/ds_edp_94.txt --new;
recorder lossrate COMPGROUP_94 1E-4:4.8E-3:1.2 results/loss_rate_94.txt --new;

recorder lossedp COMPGROUP_95 $lossedpvalues results/loss_95_edp.txt --new;
recorder lossim COMPGROUP_95 $lossimvalues results/loss_95_im.txt --new;
### recorder dsrate COMPGROUP_95;
recorder dsedp COMPGROUP_95 0.0:0.01:0.200 results/ds_edp_95.txt --new;
recorder lossrate COMPGROUP_95 1E-4:4.8E-3:1.2 results/loss_rate_95.txt --new;

recorder lossedp COMPGROUP_96 $lossedpvalues results/loss_96_edp.txt --new;
recorder lossim COMPGROUP_96 $lossimvalues results/loss_96_im.txt --new;
### recorder dsrate COMPGROUP_96;
recorder dsedp COMPGROUP_96 0.0:0.01:0.200 results/ds_edp_96.txt --new;
recorder lossrate COMPGROUP_96 1E-4:4.8E-3:1.2 results/loss_rate_96.txt --new;

recorder lossedp COMPGROUP_97 $lossedpvalues results/loss_97_edp.txt --new;
recorder lossim COMPGROUP_97 $lossimvalues results/loss_97_im.txt --new;
### recorder dsrate COMPGROUP_97;
recorder dsedp COMPGROUP_97 0.0:0.01:0.200 results/ds_edp_97.txt --new;
recorder lossrate COMPGROUP_97 1E-4:4.8E-3:1.2 results/loss_rate_97.txt --new;

recorder lossedp COMPGROUP_98 $lossedpvalues results/loss_98_edp.txt --new;
recorder lossim COMPGROUP_98 $lossimvalues results/loss_98_im.txt --new;
### recorder dsrate COMPGROUP_98;
recorder dsedp COMPGROUP_98 0.0:0.01:0.200 results/ds_edp_98.txt --new;
recorder lossrate COMPGROUP_98 1E-4:4.8E-3:1.2 results/loss_rate_98.txt --new;

recorder lossedp COMPGROUP_99 $lossedpvalues results/loss_99_edp.txt --new;
recorder lossim COMPGROUP_99 $lossimvalues results/loss_99_im.txt --new;
### recorder dsrate COMPGROUP_99;
recorder dsedp COMPGROUP_99 0.0:0.01:0.200 results/ds_edp_99.txt --new;
recorder lossrate COMPGROUP_99 1E-4:4.8E-3:1.2 results/loss_rate_99.txt --new;

recorder lossedp COMPGROUP_100 $lossedpvalues results/loss_100_edp.txt --new;
recorder lossim COMPGROUP_100 $lossimvalues results/loss_100_im.txt --new;
### recorder dsrate COMPGROUP_100;
recorder dsedp COMPGROUP_100 0.0:0.01:0.200 results/ds_edp_100.txt --new;
recorder lossrate COMPGROUP_100 1E-4:4.8E-3:1.2 results/loss_rate_100.txt --new;

recorder lossedp COMPGROUP_101 $lossedpvalues results/loss_101_edp.txt --new;
recorder lossim COMPGROUP_101 $lossimvalues results/loss_101_im.txt --new;
### recorder dsrate COMPGROUP_101;
recorder dsedp COMPGROUP_101 0.0:0.01:0.200 results/ds_edp_101.txt --new;
recorder lossrate COMPGROUP_101 1E-4:4.8E-3:1.2 results/loss_rate_101.txt --new;

recorder lossedp COMPGROUP_102 $lossedpvalues results/loss_102_edp.txt --new;
recorder lossim COMPGROUP_102 $lossimvalues results/loss_102_im.txt --new;
### recorder dsrate COMPGROUP_102;
recorder dsedp COMPGROUP_102 0.0:0.01:0.200 results/ds_edp_102.txt --new;
recorder lossrate COMPGROUP_102 1E-4:4.8E-3:1.2 results/loss_rate_102.txt --new;

recorder lossedp COMPGROUP_103 $lossedpvalues results/loss_103_edp.txt --new;
recorder lossim COMPGROUP_103 $lossimvalues results/loss_103_im.txt --new;
### recorder dsrate COMPGROUP_103;
recorder dsedp COMPGROUP_103 0.0:0.01:0.200 results/ds_edp_103.txt --new;
recorder lossrate COMPGROUP_103 1E-4:4.8E-3:1.2 results/loss_rate_103.txt --new;

recorder lossedp COMPGROUP_104 $lossedpvalues results/loss_104_edp.txt --new;
recorder lossim COMPGROUP_104 $lossimvalues results/loss_104_im.txt --new;
### recorder dsrate COMPGROUP_104;
recorder dsedp COMPGROUP_104 0.0:0.01:0.200 results/ds_edp_104.txt --new;
recorder lossrate COMPGROUP_104 1E-4:4.8E-3:1.2 results/loss_rate_104.txt --new;

recorder lossedp COMPGROUP_105 $lossedpvalues results/loss_105_edp.txt --new;
recorder lossim COMPGROUP_105 $lossimvalues results/loss_105_im.txt --new;
### recorder dsrate COMPGROUP_105;
recorder dsedp COMPGROUP_105 0.0:0.01:0.200 results/ds_edp_105.txt --new;
recorder lossrate COMPGROUP_105 1E-4:4.8E-3:1.2 results/loss_rate_105.txt --new;

recorder lossedp COMPGROUP_106 $lossedpvalues results/loss_106_edp.txt --new;
recorder lossim COMPGROUP_106 $lossimvalues results/loss_106_im.txt --new;
### recorder dsrate COMPGROUP_106;
recorder dsedp COMPGROUP_106 0.0:0.01:0.200 results/ds_edp_106.txt --new;
recorder lossrate COMPGROUP_106 1E-4:4.8E-3:1.2 results/loss_rate_106.txt --new;

recorder lossedp COMPGROUP_107 $lossedpvalues results/loss_107_edp.txt --new;
recorder lossim COMPGROUP_107 $lossimvalues results/loss_107_im.txt --new;
### recorder dsrate COMPGROUP_107;
recorder dsedp COMPGROUP_107 0.0:0.01:0.200 results/ds_edp_107.txt --new;
recorder lossrate COMPGROUP_107 1E-4:4.8E-3:1.2 results/loss_rate_107.txt --new;

recorder lossedp COMPGROUP_108 $lossedpvalues results/loss_108_edp.txt --new;
recorder lossim COMPGROUP_108 $lossimvalues results/loss_108_im.txt --new;
### recorder dsrate COMPGROUP_108;
recorder dsedp COMPGROUP_108 0.0:0.01:0.200 results/ds_edp_108.txt --new;
recorder lossrate COMPGROUP_108 1E-4:4.8E-3:1.2 results/loss_rate_108.txt --new;

recorder lossedp COMPGROUP_109 $lossedpvalues results/loss_109_edp.txt --new;
recorder lossim COMPGROUP_109 $lossimvalues results/loss_109_im.txt --new;
### recorder dsrate COMPGROUP_109;
recorder dsedp COMPGROUP_109 0.0:0.01:0.200 results/ds_edp_109.txt --new;
recorder lossrate COMPGROUP_109 1E-4:4.8E-3:1.2 results/loss_rate_109.txt --new;

recorder lossedp COMPGROUP_110 $lossedpvalues results/loss_110_edp.txt --new;
recorder lossim COMPGROUP_110 $lossimvalues results/loss_110_im.txt --new;
### recorder dsrate COMPGROUP_110;
recorder dsedp COMPGROUP_110 0.0:0.01:0.200 results/ds_edp_110.txt --new;
recorder lossrate COMPGROUP_110 1E-4:4.8E-3:1.2 results/loss_rate_110.txt --new;

recorder lossedp COMPGROUP_111 $lossedpvalues results/loss_111_edp.txt --new;
recorder lossim COMPGROUP_111 $lossimvalues results/loss_111_im.txt --new;
### recorder dsrate COMPGROUP_111;
recorder dsedp COMPGROUP_111 0.0:0.01:0.200 results/ds_edp_111.txt --new;
recorder lossrate COMPGROUP_111 1E-4:4.8E-3:1.2 results/loss_rate_111.txt --new;

recorder lossedp COMPGROUP_112 $lossedpvalues results/loss_112_edp.txt --new;
recorder lossim COMPGROUP_112 $lossimvalues results/loss_112_im.txt --new;
### recorder dsrate COMPGROUP_112;
recorder dsedp COMPGROUP_112 0.0:0.01:0.200 results/ds_edp_112.txt --new;
recorder lossrate COMPGROUP_112 1E-4:4.8E-3:1.2 results/loss_rate_112.txt --new;

recorder lossedp COMPGROUP_113 $lossedpvalues results/loss_113_edp.txt --new;
recorder lossim COMPGROUP_113 $lossimvalues results/loss_113_im.txt --new;
### recorder dsrate COMPGROUP_113;
recorder dsedp COMPGROUP_113 0.0:0.01:0.200 results/ds_edp_113.txt --new;
recorder lossrate COMPGROUP_113 1E-4:4.8E-3:1.2 results/loss_rate_113.txt --new;

recorder lossedp COMPGROUP_114 $lossedpvalues results/loss_114_edp.txt --new;
recorder lossim COMPGROUP_114 $lossimvalues results/loss_114_im.txt --new;
### recorder dsrate COMPGROUP_114;
recorder dsedp COMPGROUP_114 0.0:0.01:0.200 results/ds_edp_114.txt --new;
recorder lossrate COMPGROUP_114 1E-4:4.8E-3:1.2 results/loss_rate_114.txt --new;

recorder lossedp COMPGROUP_115 $lossedpvalues results/loss_115_edp.txt --new;
recorder lossim COMPGROUP_115 $lossimvalues results/loss_115_im.txt --new;
### recorder dsrate COMPGROUP_115;
recorder dsedp COMPGROUP_115 0.0:0.01:0.200 results/ds_edp_115.txt --new;
recorder lossrate COMPGROUP_115 1E-4:4.8E-3:1.2 results/loss_rate_115.txt --new;

recorder lossedp COMPGROUP_116 $lossedpvalues results/loss_116_edp.txt --new;
recorder lossim COMPGROUP_116 $lossimvalues results/loss_116_im.txt --new;
### recorder dsrate COMPGROUP_116;
recorder dsedp COMPGROUP_116 0.0:0.01:0.200 results/ds_edp_116.txt --new;
recorder lossrate COMPGROUP_116 1E-4:4.8E-3:1.2 results/loss_rate_116.txt --new;

recorder lossedp COMPGROUP_117 $lossedpvalues results/loss_117_edp.txt --new;
recorder lossim COMPGROUP_117 $lossimvalues results/loss_117_im.txt --new;
### recorder dsrate COMPGROUP_117;
recorder dsedp COMPGROUP_117 0.0:0.01:0.200 results/ds_edp_117.txt --new;
recorder lossrate COMPGROUP_117 1E-4:4.8E-3:1.2 results/loss_rate_117.txt --new;

recorder lossedp COMPGROUP_118 $lossedpvalues results/loss_118_edp.txt --new;
recorder lossim COMPGROUP_118 $lossimvalues results/loss_118_im.txt --new;
### recorder dsrate COMPGROUP_118;
recorder dsedp COMPGROUP_118 0.0:0.01:0.200 results/ds_edp_118.txt --new;
recorder lossrate COMPGROUP_118 1E-4:4.8E-3:1.2 results/loss_rate_118.txt --new;

recorder lossedp COMPGROUP_119 $lossedpvalues results/loss_119_edp.txt --new;
recorder lossim COMPGROUP_119 $lossimvalues results/loss_119_im.txt --new;
### recorder dsrate COMPGROUP_119;
recorder dsedp COMPGROUP_119 0.0:0.01:0.200 results/ds_edp_119.txt --new;
recorder lossrate COMPGROUP_119 1E-4:4.8E-3:1.2 results/loss_rate_119.txt --new;

recorder lossedp COMPGROUP_120 $lossedpvalues results/loss_120_edp.txt --new;
recorder lossim COMPGROUP_120 $lossimvalues results/loss_120_im.txt --new;
### recorder dsrate COMPGROUP_120;
recorder dsedp COMPGROUP_120 0.0:0.01:0.200 results/ds_edp_120.txt --new;
recorder lossrate COMPGROUP_120 1E-4:4.8E-3:1.2 results/loss_rate_120.txt --new;

recorder lossedp COMPGROUP_121 $lossedpvalues results/loss_121_edp.txt --new;
recorder lossim COMPGROUP_121 $lossimvalues results/loss_121_im.txt --new;
### recorder dsrate COMPGROUP_121;
recorder dsedp COMPGROUP_121 0.0:0.01:0.200 results/ds_edp_121.txt --new;
recorder lossrate COMPGROUP_121 1E-4:4.8E-3:1.2 results/loss_rate_121.txt --new;

recorder lossedp COMPGROUP_122 $lossedpvalues results/loss_122_edp.txt --new;
recorder lossim COMPGROUP_122 $lossimvalues results/loss_122_im.txt --new;
### recorder dsrate COMPGROUP_122;
recorder dsedp COMPGROUP_122 0.0:0.01:0.200 results/ds_edp_122.txt --new;
recorder lossrate COMPGROUP_122 1E-4:4.8E-3:1.2 results/loss_rate_122.txt --new;

recorder lossedp COMPGROUP_123 $lossedpvalues results/loss_123_edp.txt --new;
recorder lossim COMPGROUP_123 $lossimvalues results/loss_123_im.txt --new;
### recorder dsrate COMPGROUP_123;
recorder dsedp COMPGROUP_123 0.0:0.01:0.200 results/ds_edp_123.txt --new;
recorder lossrate COMPGROUP_123 1E-4:4.8E-3:1.2 results/loss_rate_123.txt --new;

recorder lossedp COMPGROUP_124 $lossedpvalues results/loss_124_edp.txt --new;
recorder lossim COMPGROUP_124 $lossimvalues results/loss_124_im.txt --new;
### recorder dsrate COMPGROUP_124;
recorder dsedp COMPGROUP_124 0.0:0.01:0.200 results/ds_edp_124.txt --new;
recorder lossrate COMPGROUP_124 1E-4:4.8E-3:1.2 results/loss_rate_124.txt --new;

recorder lossedp COMPGROUP_125 $lossedpvalues results/loss_125_edp.txt --new;
recorder lossim COMPGROUP_125 $lossimvalues results/loss_125_im.txt --new;
### recorder dsrate COMPGROUP_125;
recorder dsedp COMPGROUP_125 0.0:0.01:0.200 results/ds_edp_125.txt --new;
recorder lossrate COMPGROUP_125 1E-4:4.8E-3:1.2 results/loss_rate_125.txt --new;"


