#! /usr/bin/env python3
import pyslat
import filecmp


IM1 = pyslat.ImportIMFn("IM_1", "imfunc.txt")
IM1.SetCollapse(pyslat.collapse(None, 0.9, 0.470))

def ResultsFile(name):
    return "py-results/{}".format(name)

def CheckResults(name):
    if not filecmp.cmp(ResultsFile(name), "results/{}".format(name)):
        print("MISMATCH: {}".format(name))
        exit()

logvalues = pyslat.logrange(0.01, 2.5, 199)
edpoddvalues = pyslat.logrange(0.05, 5.0, 199)
edpevenvalues = pyslat.logrange(0.001, 0.1, 199)
linvalues = pyslat.linrange(0.01, 2.5, 199)
lossimvalues = pyslat.linrange(0.01, 2.5, 199)
lossedpvalues1 = pyslat.linrange(0.001, 0.10, 149)
lossedpvalues2 = pyslat.linrange(0.001, 0.10, 199)
lossedpvalues3 = pyslat.linrange(0.05, 5.0, 199)
lossedpvalues4 = pyslat.linrange(0.001, 0.15, 149)
lossedpvalues5 = pyslat.linrange(0.001, 0.10, 149)

pyslat.recorder('imrate-rec', 'imrate', IM1,
                {'filename': ResultsFile("im_rate")},
                None, logvalues)

pyslat.recorder('collapse-rec', 'collapse', IM1,
                {'filename': ResultsFile("collapse.txt")},
                None, pyslat.linrange(0.01, 2.5, 199))

pyslat.recorder('collrate-rec', 'collrate', IM1,
                {'filename': ResultsFile("collrate.txt")},
                None, None)

N_EDPS=21
DATA_DIR = "/home/mag109/SLATv1.15_Public/example2_10storeybuilding"

for i in range(1, N_EDPS + 1):
    pyslat.ImportProbFn("EDP_FUNC_{}".format(i),
                        "{}/RB_EDP{}.txt".format(DATA_DIR, i))
    edp = pyslat.edp("EDP_{}".format(i), IM1,
                     pyslat.probfn.lookup("EDP_FUNC_{}".format(i)))
                                   
    pyslat.recorder('edpim-{}'.format(i), 'edpim', edp,
                    {'filename': ResultsFile("im_edp_{}.txt".format(i))},
                    ['mean_x', 'sd_ln_x'],
                    linvalues)
    
    if i==1:
        at = pyslat.logrange(0.001, 0.15, 149)
    elif i==2:
        at = pyslat.logrange(0.001, 0.10, 149)
    elif (i % 2) == 1:
        at = edpoddvalues
    else:
        at = edpevenvalues
        
    pyslat.recorder('edprate-{}'.format(i), 'edprate', edp,
                    {'filename': ResultsFile("edp_{}_rate.txt".format(i))},
                     None,
                    at)


# Fragility, loss, component group:
FRAG_DATA = [[2, [[0.005, 0.40], [0.010, 0.45], [0.030, 0.50], [0.060, 0.60]],
              [[1143, 0.42], [3214, 0.40], [4900, 0.37], [4900, 0.37]]],
             [3, [[0.004, 0.39], [0.0095, 0.25], [0.02, 0.62], [0.0428, 0.36]],
              [[590, 0.59], [2360, 0.63], [5900, 0.67], [5900, 0.67]]],
             [105, [[0.0039, 0.17], [0.0085, 0.23]],
              [[29.9, 0.2], [178.7, 0.2]]],
             [107, [[0.04, 0.36], [0.046, 0.33]],
              [ [131.7, 0.26], [131.7, 0.26]]],
             [203, [[0.55, 0.4], [1.0, 0.4]],
              [[46.73131, 0.4], [282.4967, 0.4]]],
             [211, [[32, 1.4]],
              [[900, 1.0]]],
             [208, [[1.2, 0.6]],
              [[1783.333, 0.4]]],
             [209, [[0.8, 0.5]],
              [[40000, 0.4]]],
             [205, [[1.6, 0.5]],
              [[196666.7, 0.6]]],
             [204, [[0.4, 0.3]],
              [[56000, 0.2]]],
             [106, [[0.0039, 0.17]],
              [[16.7, 0.2]]],
             [108, [[0.004, 0.5], [0.008, 0.5], [0.025, 0.5], [0.050, 0.5]],
              [[  250.0, 0.63], [ 1000.0, 0.63], [ 5000.0, 0.63], [10000.0, 0.63]]],
             [214, [[0.25, 0.6], [0.50, 0.6], [1.00, 0.6], [2.00, 0.6]],
              [[ 200.0, 0.63], [ 1200.0, 0.63], [ 3600.0, 0.63], [10000.0, 0.63]]]]

for f in FRAG_DATA:
    id = f[0]
    frag = f[1]
    loss = f[2]

    pyslat.fragfn_user("FRAG_{}".format(id), 
                       {'mu': pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X,
                        'sd': pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X},
                       frag)
    

    pyslat.lossfn("LOSS_{}".format(id), 
                  {'mu': pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X,
                   'sd': pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X},
                  loss)

COMPONENT_DATA = [[1, "EDP_2", "FRAG_2", "LOSS_2", 20],
                  [2, "EDP_2", "FRAG_2", "LOSS_2", 18],
                  [3, "EDP_4", "FRAG_2", "LOSS_2", 4],
                  [4, "EDP_4", "FRAG_2", "LOSS_2", 18],
                  [5, "EDP_6", "FRAG_2", "LOSS_2", 4],
                  [6, "EDP_6", "FRAG_2", "LOSS_2", 18],
                  [7, "EDP_8", "FRAG_2", "LOSS_2", 4],
                  [8, "EDP_8", "FRAG_2", "LOSS_2", 18],
                  [9, "EDP_10", "FRAG_2", "LOSS_2", 4],
                  [10, "EDP_10", "FRAG_2", "LOSS_2", 18],
                  [11, "EDP_12", "FRAG_2", "LOSS_2", 4],
                  [12, "EDP_12", "FRAG_2", "LOSS_2", 18],
                  [13, "EDP_14", "FRAG_2", "LOSS_2", 4],
                  [14, "EDP_14", "FRAG_2", "LOSS_2", 18],
                  [15, "EDP_16", "FRAG_2", "LOSS_2", 4],
                  [16, "EDP_16", "FRAG_2", "LOSS_2", 18],
                  [17, "EDP_18", "FRAG_2", "LOSS_2", 4],
                  [18, "EDP_18", "FRAG_2", "LOSS_2", 18],
                  [19, "EDP_20", "FRAG_2", "LOSS_2", 4],
                  [20, "EDP_20", "FRAG_2", "LOSS_2", 18],
                  [21, "EDP_2", "FRAG_3", "LOSS_3", 16],
                  [22, "EDP_4", "FRAG_3", "LOSS_3", 16],
                  [23, "EDP_6", "FRAG_3", "LOSS_3", 16],
                  [24, "EDP_8", "FRAG_3", "LOSS_3", 16],
                  [25, "EDP_10", "FRAG_3", "LOSS_3", 16],
                  [26, "EDP_12", "FRAG_3", "LOSS_3", 16],
                  [27, "EDP_14", "FRAG_3", "LOSS_3", 16],
                  [28, "EDP_16", "FRAG_3", "LOSS_3", 16],
                  [29, "EDP_18", "FRAG_3", "LOSS_3", 16],
                  [30, "EDP_20", "FRAG_3", "LOSS_3", 16],
                  [31, "EDP_2", "FRAG_105", "LOSS_105", 721],
                  [32, "EDP_4", "FRAG_105", "LOSS_105", 721],
                  [33, "EDP_6", "FRAG_105", "LOSS_105", 721],
                  [34, "EDP_8", "FRAG_105", "LOSS_105", 721],
                  [35, "EDP_10", "FRAG_105", "LOSS_105", 721],
                  [36, "EDP_12", "FRAG_105", "LOSS_105", 721],
                  [37, "EDP_14", "FRAG_105", "LOSS_105", 721],
                  [38, "EDP_16", "FRAG_105", "LOSS_105", 721],
                  [39, "EDP_18", "FRAG_105", "LOSS_105", 721],
                  [40, "EDP_20", "FRAG_105", "LOSS_105", 721],
                  [41, "EDP_2", "FRAG_107", "LOSS_107", 99],
                  [42, "EDP_4", "FRAG_107", "LOSS_107", 99],
                  [43, "EDP_6", "FRAG_107", "LOSS_107", 99],
                  [44, "EDP_8", "FRAG_107", "LOSS_107", 99],
                  [45, "EDP_10", "FRAG_107", "LOSS_107", 99],
                  [46, "EDP_12", "FRAG_107", "LOSS_107", 99],
                  [47, "EDP_14", "FRAG_107", "LOSS_107", 99],
                  [48, "EDP_16", "FRAG_107", "LOSS_107", 99],
                  [49, "EDP_18", "FRAG_107", "LOSS_107", 99],
                  [50, "EDP_20", "FRAG_107", "LOSS_107", 99],
                  [51, "EDP_3", "FRAG_203", "LOSS_203", 693],
                  [52, "EDP_5", "FRAG_203", "LOSS_203", 693],
                  [53, "EDP_7", "FRAG_203", "LOSS_203", 693],
                  [54, "EDP_9", "FRAG_203", "LOSS_203", 693],
                  [55, "EDP_11", "FRAG_203", "LOSS_203", 693],
                  [56, "EDP_13", "FRAG_203", "LOSS_203", 693],
                  [57, "EDP_15", "FRAG_203", "LOSS_203", 693],
                  [58, "EDP_17", "FRAG_203", "LOSS_203", 693],
                  [59, "EDP_19", "FRAG_203", "LOSS_203", 693],
                  [60, "EDP_21", "FRAG_203", "LOSS_203", 693],
                  [61, "EDP_3", "FRAG_211", "LOSS_211", 23],
                  [62, "EDP_5", "FRAG_211", "LOSS_211", 23],
                  [63, "EDP_7", "FRAG_211", "LOSS_211", 23],
                  [64, "EDP_9", "FRAG_211", "LOSS_211", 23],
                  [65, "EDP_11", "FRAG_211", "LOSS_211", 23],
                  [66, "EDP_13", "FRAG_211", "LOSS_211", 23],
                  [67, "EDP_15", "FRAG_211", "LOSS_211", 23],
                  [68, "EDP_17", "FRAG_211", "LOSS_211", 23],
                  [69, "EDP_19", "FRAG_211", "LOSS_211", 23],
                  [70, "EDP_21", "FRAG_211", "LOSS_211", 23],
                  [71, "EDP_1", "FRAG_208", "LOSS_208", 53],
                  [72, "EDP_3", "FRAG_208", "LOSS_208", 53],
                  [73, "EDP_5", "FRAG_208", "LOSS_208", 53],
                  [74, "EDP_7", "FRAG_208", "LOSS_208", 53],
                  [75, "EDP_9", "FRAG_208", "LOSS_208", 53],
                  [76, "EDP_11", "FRAG_208", "LOSS_208", 53],
                  [77, "EDP_13", "FRAG_208", "LOSS_208", 53],
                  [78, "EDP_15", "FRAG_208", "LOSS_208", 53],
                  [79, "EDP_17", "FRAG_208", "LOSS_208", 53],
                  [80, "EDP_19", "FRAG_208", "LOSS_208", 53],
                  [81, "EDP_5", "FRAG_209", "LOSS_209", 16],
                  [82, "EDP_11", "FRAG_209", "LOSS_209", 16],
                  [83, "EDP_19", "FRAG_209", "LOSS_209", 16],
                  [84, "EDP_21", "FRAG_205", "LOSS_205", 4],
                  [85, "EDP_1", "FRAG_204", "LOSS_204", 2],
                  [86, "EDP_2", "FRAG_106", "LOSS_106", 721],
                  [87, "EDP_4", "FRAG_106", "LOSS_106", 721],
                  [88, "EDP_6", "FRAG_106", "LOSS_106", 721],
                  [89, "EDP_8", "FRAG_106", "LOSS_106", 721],
                  [90, "EDP_10", "FRAG_106", "LOSS_106", 721],
                  [91, "EDP_12", "FRAG_106", "LOSS_106", 721],
                  [92, "EDP_14", "FRAG_106", "LOSS_106", 721],
                  [93, "EDP_16", "FRAG_106", "LOSS_106", 721],
                  [94, "EDP_18", "FRAG_106", "LOSS_106", 721],
                  [95, "EDP_20", "FRAG_106", "LOSS_106", 721],
                  [96, "EDP_2", "FRAG_108", "LOSS_108", 10],
                  [97, "EDP_4", "FRAG_108", "LOSS_108", 10],
                  [98, "EDP_6", "FRAG_108", "LOSS_108", 10],
                  [99, "EDP_8", "FRAG_108", "LOSS_108", 10],
                  [100, "EDP_10", "FRAG_108", "LOSS_108", 10],
                  [101, "EDP_12", "FRAG_108", "LOSS_108", 10],
                  [102, "EDP_14", "FRAG_108", "LOSS_108", 10],
                  [103, "EDP_16", "FRAG_108", "LOSS_108", 10],
                  [104, "EDP_18", "FRAG_108", "LOSS_108", 10],
                  [105, "EDP_20", "FRAG_108", "LOSS_108", 10],
                  [106, "EDP_1", "FRAG_214", "LOSS_214", 10],
                  [107, "EDP_3", "FRAG_214", "LOSS_214", 10],
                  [108, "EDP_5", "FRAG_214", "LOSS_214", 10],
                  [109, "EDP_7", "FRAG_214", "LOSS_214", 10],
                  [110, "EDP_9", "FRAG_214", "LOSS_214", 10],
                  [111, "EDP_11", "FRAG_214", "LOSS_214", 10],
                  [112, "EDP_13", "FRAG_214", "LOSS_214", 10],
                  [113, "EDP_15", "FRAG_214", "LOSS_214", 10],
                  [114, "EDP_17", "FRAG_214", "LOSS_214", 10],
                  [115, "EDP_19", "FRAG_214", "LOSS_214", 10]]

building = pyslat.structure("building")
building.setRebuildCost(pyslat.MakeLogNormalDist({pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X:14E6,
                                                  pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X:0.35}))
for comp in COMPONENT_DATA:
    id = comp[0]
    edp = comp[1]
    frag = comp[2]
    loss = comp[3]
    count = comp[4]

    cg = pyslat.compgroup(id, pyslat.edp.lookup(edp),
                            pyslat.fragfn.lookup(frag),
                            pyslat.lossfn.lookup(loss),
                            count)
    print(id, edp, frag, loss, count)
    if id in [1, 2, 86, 96]:
        at_values = lossedpvalues1
    elif id in [21, 31, 41]:
        at_values = lossedpvalues5
    elif id in [71, 85, 106]: 
        at_values = lossedpvalues4
    elif id in [86, 87]:
        at_values = lossedpvalues2
    elif (id >= 51 and id < 87) or (id >= 107):
        at_values = lossedpvalues3
    else:
        at_values = lossedpvalues2
        
    pyslat.recorder("LOSSEDP_{}_REC".format(id), 'lossedp', cg,
                    {'filename': ResultsFile("loss_{}_edp.txt".format(id))},
                    None, 
                    at_values).run()
    CheckResults("loss_{}_edp.txt".format(id))

    pyslat.recorder("LOSSIM_{}_REC".format(id), 'lossim', cg,
                    {'filename': ResultsFile("loss_{}_im.txt".format(id))},
                    None, 
                    lossimvalues).run()
    CheckResults("loss_{}_im.txt".format(id))

    pyslat.recorder("DSEDP_{}_REC".format(id), 'dsedp', cg,
                    {'filename': ResultsFile("ds_edp_{}.txt".format(id))},
                    None, 
                    pyslat.frange(0.0, 0.200, 0.01)).run()
    CheckResults("ds_edp_{}.txt".format(id))

    pyslat.recorder("LOSSRATE_{}_REC".format(id), 'lossrate', cg,
                    {'filename': ResultsFile("loss_rate_{}.txt".format(id))},
                    None, 
                    pyslat.frange(1E-4, 1.2, 4.8E-3)).run()
    CheckResults("loss_rate_{}.txt".format(id))

    building.AddCompGroup(cg)


pyslat.recorder("lossnc", 
                "structloss", 
                pyslat.structure.lookup("building"),
                {'collapse': False, 'filename': "py-results/loss_nc_total", 'append': False},
                None, 
                pyslat.linrange(0.01, 2.5, 199))

pyslat.recorder("lossc", 
                "structloss", 
                pyslat.structure.lookup("building"),
                {'collapse': True, 'filename': "py-results/loss_c_total", 'append': False},
                None, 
                pyslat.linrange(0.01, 2.5, 199))

pyslat.recorder("deagg", 
                "deagg", 
                pyslat.structure.lookup("building"),
                {'filename': "py-results/deagg", 'append': False},
                None, 
                pyslat.linrange(0.01, 2.5, 199))

pyslat.IntegrationSettings(1.0E-6, 1024)
for r in pyslat.recorder.all():
    r.run()
    

    
