#! /usr/bin/env python3
import pyslat
import filecmp


IM1 = pyslat.ImportIMFn("IM.1", "imfunc.txt")
IM1.SetCollapse(pyslat.MakeLogNormalDist(
    {pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X: 1.2,
     pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X: 0.470}))
IM1.SetDemolition(pyslat.MakeLogNormalDist(
    {pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X: 0.9,
     pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X: 0.470}))

def ResultsFile(name):
    return "py-results/{}".format(name)

imvalues = pyslat.logrange(0.01, 3.0, 199)
edpoddvalues = pyslat.logrange(0.05, 5.0, 199)
edpevenvalues = pyslat.logrange(0.001, 0.1, 199)
linvalues = pyslat.linrange(0.01, 3.0, 199)
lossimvalues = pyslat.linrange(0.01, 3.0, 199)
lossedpvalues1 = pyslat.linrange(0.001, 0.10, 199)
lossedpvalues2 = pyslat.linrange(0.001, 0.10, 199)
lossedpvalues3 = pyslat.linrange(0.05, 5.0, 199)
lossedpvalues4 = pyslat.linrange(0.032, 0.0325, 199)
lossedpvalues5 = pyslat.linrange(0.001, 0.10, 199)

pyslat.MakeRecorder('imrate-rec', 'imrate', IM1,
                {'filename': ResultsFile("im_rate")},
                None, imvalues)

pyslat.MakeRecorder('imrate-lin-rec', 'imrate', IM1,
                {'filename': ResultsFile("im_rate_lin")},
                    None, linvalues)

pyslat.MakeRecorder('collapse-rec', 'collapse', IM1,
                {'filename': ResultsFile("collapse.txt")},
                None, pyslat.linrange(0.01, 3.0, 199))

pyslat.MakeRecorder('collrate-rec', 'collrate', IM1,
                {'filename': ResultsFile("collrate.txt")},
                None, None)

N_EDPS=21
DATA_DIR = "."

for i in range(1, N_EDPS + 1):
    pyslat.ImportProbFn("EDP_FUNC_{:>02}".format(i),
                        "{}/RB_EDP{}.txt".format(DATA_DIR, i))
    edp = pyslat.edp("EDP.{:>02}".format(i), IM1,
                     pyslat.probfn.lookup("EDP_FUNC_{:>02}".format(i)))
                                   
    pyslat.MakeRecorder('edpim-{:>02}'.format(i), 'edpim', edp,
                    {'filename': ResultsFile("im_edp_{}.txt".format(i))},
                    ['mean_x', 'sd_ln_x'],
                    linvalues)
    
    if i==1:
        at = pyslat.logrange(0.032, 0.0325, 199)
    elif i==2:
        at = pyslat.logrange(0.001, 0.10, 199)
    elif (i % 2) == 1:
        at = edpoddvalues
    else:
        at = edpevenvalues
        
    pyslat.MakeRecorder('edprate-{:>02}'.format(i), 'edprate', edp,
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

    pyslat.fragfn_user("FRAG.{:>03}".format(id), 
                       {'mu': pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X,
                        'sd': pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X},
                       frag)
    

    pyslat.lossfn("LOSS_{:>03}".format(id), 
                  {'mu': pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X,
                   'sd': pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X},
                  loss)

COMPONENT_DATA = [[1, "EDP.02", "FRAG.002", "LOSS_002", 20],
                  [2, "EDP.02", "FRAG.002", "LOSS_002", 18],
                  [3, "EDP.04", "FRAG.002", "LOSS_002", 4],
                  [4, "EDP.04", "FRAG.002", "LOSS_002", 18],
                  [5, "EDP.06", "FRAG.002", "LOSS_002", 4],
                  [6, "EDP.06", "FRAG.002", "LOSS_002", 18],
                  [7, "EDP.08", "FRAG.002", "LOSS_002", 4],
                  [8, "EDP.08", "FRAG.002", "LOSS_002", 18],
                  [9, "EDP.10", "FRAG.002", "LOSS_002", 4],
                  [10, "EDP.10", "FRAG.002", "LOSS_002", 18],
                  [11, "EDP.12", "FRAG.002", "LOSS_002", 4],
                  [12, "EDP.12", "FRAG.002", "LOSS_002", 18],
                  [13, "EDP.14", "FRAG.002", "LOSS_002", 4],
                  [14, "EDP.14", "FRAG.002", "LOSS_002", 18],
                  [15, "EDP.16", "FRAG.002", "LOSS_002", 4],
                  [16, "EDP.16", "FRAG.002", "LOSS_002", 18],
                  [17, "EDP.18", "FRAG.002", "LOSS_002", 4],
                  [18, "EDP.18", "FRAG.002", "LOSS_002", 18],
                  [19, "EDP.20", "FRAG.002", "LOSS_002", 4],
                  [20, "EDP.20", "FRAG.002", "LOSS_002", 18],
                  [21, "EDP.02", "FRAG.003", "LOSS_003", 16],
                  [22, "EDP.04", "FRAG.003", "LOSS_003", 16],
                  [23, "EDP.06", "FRAG.003", "LOSS_003", 16],
                  [24, "EDP.08", "FRAG.003", "LOSS_003", 16],
                  [25, "EDP.10", "FRAG.003", "LOSS_003", 16],
                  [26, "EDP.12", "FRAG.003", "LOSS_003", 16],
                  [27, "EDP.14", "FRAG.003", "LOSS_003", 16],
                  [28, "EDP.16", "FRAG.003", "LOSS_003", 16],
                  [29, "EDP.18", "FRAG.003", "LOSS_003", 16],
                  [30, "EDP.20", "FRAG.003", "LOSS_003", 16],
                  [31, "EDP.02", "FRAG.105", "LOSS_105", 721],
                  [32, "EDP.04", "FRAG.105", "LOSS_105", 721],
                  [33, "EDP.06", "FRAG.105", "LOSS_105", 721],
                  [34, "EDP.08", "FRAG.105", "LOSS_105", 721],
                  [35, "EDP.10", "FRAG.105", "LOSS_105", 721],
                  [36, "EDP.12", "FRAG.105", "LOSS_105", 721],
                  [37, "EDP.14", "FRAG.105", "LOSS_105", 721],
                  [38, "EDP.16", "FRAG.105", "LOSS_105", 721],
                  [39, "EDP.18", "FRAG.105", "LOSS_105", 721],
                  [40, "EDP.20", "FRAG.105", "LOSS_105", 721],
                  [41, "EDP.02", "FRAG.107", "LOSS_107", 99],
                  [42, "EDP.04", "FRAG.107", "LOSS_107", 99],
                  [43, "EDP.06", "FRAG.107", "LOSS_107", 99],
                  [44, "EDP.08", "FRAG.107", "LOSS_107", 99],
                  [45, "EDP.10", "FRAG.107", "LOSS_107", 99],
                  [46, "EDP.12", "FRAG.107", "LOSS_107", 99],
                  [47, "EDP.14", "FRAG.107", "LOSS_107", 99],
                  [48, "EDP.16", "FRAG.107", "LOSS_107", 99],
                  [49, "EDP.18", "FRAG.107", "LOSS_107", 99],
                  [50, "EDP.20", "FRAG.107", "LOSS_107", 99],
                  [51, "EDP.03", "FRAG.203", "LOSS_203", 693],
                  [52, "EDP.05", "FRAG.203", "LOSS_203", 693],
                  [53, "EDP.07", "FRAG.203", "LOSS_203", 693],
                  [54, "EDP.09", "FRAG.203", "LOSS_203", 693],
                  [55, "EDP.11", "FRAG.203", "LOSS_203", 693],
                  [56, "EDP.13", "FRAG.203", "LOSS_203", 693],
                  [57, "EDP.15", "FRAG.203", "LOSS_203", 693],
                  [58, "EDP.17", "FRAG.203", "LOSS_203", 693],
                  [59, "EDP.19", "FRAG.203", "LOSS_203", 693],
                  [60, "EDP.21", "FRAG.203", "LOSS_203", 693],
                  [61, "EDP.03", "FRAG.211", "LOSS_211", 23],
                  [62, "EDP.05", "FRAG.211", "LOSS_211", 23],
                  [63, "EDP.07", "FRAG.211", "LOSS_211", 23],
                  [64, "EDP.09", "FRAG.211", "LOSS_211", 23],
                  [65, "EDP.11", "FRAG.211", "LOSS_211", 23],
                  [66, "EDP.13", "FRAG.211", "LOSS_211", 23],
                  [67, "EDP.15", "FRAG.211", "LOSS_211", 23],
                  [68, "EDP.17", "FRAG.211", "LOSS_211", 23],
                  [69, "EDP.19", "FRAG.211", "LOSS_211", 23],
                  [70, "EDP.21", "FRAG.211", "LOSS_211", 23],
                  [71, "EDP.01", "FRAG.208", "LOSS_208", 53],
                  [72, "EDP.03", "FRAG.208", "LOSS_208", 53],
                  [73, "EDP.05", "FRAG.208", "LOSS_208", 53],
                  [74, "EDP.07", "FRAG.208", "LOSS_208", 53],
                  [75, "EDP.09", "FRAG.208", "LOSS_208", 53],
                  [76, "EDP.11", "FRAG.208", "LOSS_208", 53],
                  [77, "EDP.13", "FRAG.208", "LOSS_208", 53],
                  [78, "EDP.15", "FRAG.208", "LOSS_208", 53],
                  [79, "EDP.17", "FRAG.208", "LOSS_208", 53],
                  [80, "EDP.19", "FRAG.208", "LOSS_208", 53],
                  [81, "EDP.05", "FRAG.209", "LOSS_209", 16],
                  [82, "EDP.11", "FRAG.209", "LOSS_209", 16],
                  [83, "EDP.19", "FRAG.209", "LOSS_209", 16],
                  [84, "EDP.21", "FRAG.205", "LOSS_205", 4],
                  [85, "EDP.01", "FRAG.204", "LOSS_204", 2],
                  [86, "EDP.02", "FRAG.106", "LOSS_106", 721],
                  [87, "EDP.04", "FRAG.106", "LOSS_106", 721],
                  [88, "EDP.06", "FRAG.106", "LOSS_106", 721],
                  [89, "EDP.08", "FRAG.106", "LOSS_106", 721],
                  [90, "EDP.10", "FRAG.106", "LOSS_106", 721],
                  [91, "EDP.12", "FRAG.106", "LOSS_106", 721],
                  [92, "EDP.14", "FRAG.106", "LOSS_106", 721],
                  [93, "EDP.16", "FRAG.106", "LOSS_106", 721],
                  [94, "EDP.18", "FRAG.106", "LOSS_106", 721],
                  [95, "EDP.20", "FRAG.106", "LOSS_106", 721],
                  [96, "EDP.02", "FRAG.108", "LOSS_108", 10],
                  [97, "EDP.04", "FRAG.108", "LOSS_108", 10],
                  [98, "EDP.06", "FRAG.108", "LOSS_108", 10],
                  [99, "EDP.08", "FRAG.108", "LOSS_108", 10],
                  [100, "EDP.10", "FRAG.108", "LOSS_108", 10],
                  [101, "EDP.12", "FRAG.108", "LOSS_108", 10],
                  [102, "EDP.14", "FRAG.108", "LOSS_108", 10],
                  [103, "EDP.16", "FRAG.108", "LOSS_108", 10],
                  [104, "EDP.18", "FRAG.108", "LOSS_108", 10],
                  [105, "EDP.20", "FRAG.108", "LOSS_108", 10],
                  [106, "EDP.01", "FRAG.214", "LOSS_214", 10],
                  [107, "EDP.03", "FRAG.214", "LOSS_214", 10],
                  [108, "EDP.05", "FRAG.214", "LOSS_214", 10],
                  [109, "EDP.07", "FRAG.214", "LOSS_214", 10],
                  [110, "EDP.09", "FRAG.214", "LOSS_214", 10],
                  [111, "EDP.11", "FRAG.214", "LOSS_214", 10],
                  [112, "EDP.13", "FRAG.214", "LOSS_214", 10],
                  [113, "EDP.15", "FRAG.214", "LOSS_214", 10],
                  [114, "EDP.17", "FRAG.214", "LOSS_214", 10],
                  [115, "EDP.19", "FRAG.214", "LOSS_214", 10]]

building = pyslat.structure("building")
building.setRebuildCost(pyslat.MakeLogNormalDist({pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X:14E6,
                                                  pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X:0.35}))
building.setRebuildCost(pyslat.MakeLogNormalDist({pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X:14E6,
                                                  pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X:0.35}))
building.setDemolitionCost(pyslat.MakeLogNormalDist({pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X: 14E6,
                                                     pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X: 0.35}))
for comp in COMPONENT_DATA:
    id = comp[0]
    edp = comp[1]
    frag = comp[2]
    loss = comp[3]
    count = comp[4]

    cg = pyslat.compgroup(
        "COMPGROUP_{:>03}".format(id),
        pyslat.edp.lookup(edp),
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
        
    pyslat.MakeRecorder("LOSSEDP_{:>03}_REC".format(id), 'lossedp', cg,
                    {'filename': ResultsFile("loss_{}_edp.txt".format(id))},
                    None, 
                    at_values)

    pyslat.MakeRecorder("LOSSIM_{:>03}_REC".format(id), 'lossim', cg,
                    {'filename': ResultsFile("loss_{}_im.txt".format(id))},
                    None, 
                    lossimvalues)

    pyslat.MakeRecorder("DSEDP_{:>03}_REC".format(id), 'dsedp', cg,
                    {'filename': ResultsFile("ds_edp_{}.txt".format(id))},
                    None, 
                    pyslat.frange(0.0, 0.200, 0.01))

    pyslat.MakeRecorder("LOSSRATE_{:>03}_REC".format(id), 'lossrate', cg,
                    {'filename': ResultsFile("loss_rate_{}.txt".format(id))},
                    None, 
                    pyslat.frange(1E-4, 1.2, 4.8E-3))

    pyslat.MakeRecorder("ANNLOSS_{:>03}_REC".format(id), "annloss", cg,
                    {'filename': ResultsFile("annual_loss_{}.txt".format(id)),
                     "lambda": 0.06},
                    None, 
                    pyslat.frange(1.0, 100.0, 1.0))
                
    building.AddCompGroup(cg)


#pyslat.MakeRecorder("deagg", 
#                "deagg", 
#                pyslat.structure.lookup("building"),
#                {'filename': "py-results/deagg", 'append': False},
#                None, 
#                linvalues)

pyslat.MakeRecorder("annloss", 
                "structloss", 
                pyslat.structure.lookup("building"),
                {'filename': "py-results/ann_loss", 'append': False, 'structloss-type': 'annual'},
                ['mean_x', 'sd_ln_x'], 
                None)

pyslat.MakeRecorder("STRUCTLOSS_FATE_REC", 
                "structloss", 
                pyslat.structure.lookup("building"),
                {'filename': "py-results/loss_by_fate", 'append': False, 'structloss-type': 'by-fate'},
                None, 
                linvalues)

pyslat.MakeRecorder("STRUCTLOSS_EDP_REC", 
                "structloss", 
                pyslat.structure.lookup("building"),
                {'filename': "py-results/loss_by_edp", 'append': False, 'structloss-type': 'by-edp'},
                None, 
                linvalues)

pyslat.MakeRecorder("STRUCTLOSS_COMP_REC", 
                "structloss", 
                pyslat.structure.lookup("building"),
                {'filename': "py-results/loss_by_frag", 'append': False, 'structloss-type': 'by-frag'},
                None, 
                linvalues)

pyslat.IntegrationSettings(1.0E-6, 1024)
for r in pyslat.recorder.all():
    r.run()
    

    
