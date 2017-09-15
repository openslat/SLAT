#! /usr/bin/env python3

import pyslat
import filecmp
from contextlib import redirect_stdout

pyslat.LogToStdErr(True)  # Log errors to stderr (on by default)
pyslat.SetLogFile("exercise2.log") # Save errors in 'exercise2.log'

pyslat.set_input_directory("input")
pyslat.set_output_directory("py-results")

IM1 = pyslat.ImportIMFn("IM.1", "imfunc.csv")
IM1.SetCollapse(pyslat.MakeLogNormalDist(
    1.2, pyslat.LOGNORMAL_MU_TYPE.MEAN_X,
    0.470, pyslat.LOGNORMAL_SIGMA_TYPE.SD_LN_X))
IM1.SetDemolition(pyslat.MakeLogNormalDist(
    0.9, pyslat.LOGNORMAL_MU_TYPE.MEAN_X,
    0.470, pyslat.LOGNORMAL_SIGMA_TYPE.SD_LN_X)) 

def ResultsFile(name):
    return "py-results/{}".format(name)

imvalues = pyslat.logrange(0.01, 3.0, 199)
edpoddvalues = pyslat.logrange(0.05, 5.0, 199)
edpevenvalues = pyslat.logrange(0.001, 0.1, 199)
linvalues = pyslat.linrange(0.01, 3.0, 199)
costimvalues = pyslat.linrange(0.01, 3.0, 199)
costedpvalues1 = pyslat.linrange(0.001, 0.10, 199)
costedpvalues2 = pyslat.linrange(0.001, 0.10, 199)
costedpvalues3 = pyslat.linrange(0.05, 5.0, 199)
costedpvalues4 = pyslat.linrange(0.032, 0.0325, 199)
costedpvalues5 = pyslat.linrange(0.001, 0.10, 199)

pyslat.MakeRecorder('imrate-rec', 'imrate', IM1,
                {'filename': ResultsFile("im_rate")},
                None, imvalues)

pyslat.MakeRecorder('imrate-lin-rec', 'imrate', IM1,
                {'filename': ResultsFile("im_rate_lin")},
                    None, linvalues)

pyslat.MakeRecorder('collapse-rec', 'collapse', IM1,
                {'filename': ResultsFile("collapse")},
                None, pyslat.linrange(0.01, 3.0, 199))

pyslat.MakeRecorder('collrate-rec', 'collrate', IM1,
                {'filename': ResultsFile("collrate")},
                None, None)

N_EDPS=21

for i in range(1, N_EDPS + 1):
    pyslat.ImportProbFn("EDP_FUNC_{:>02}".format(i),
                        "RB_EDP{}.csv".format(i))
    edp = pyslat.edp("EDP.{:>02}".format(i), IM1,
                     pyslat.probfn.lookup("EDP_FUNC_{:>02}".format(i)))
                                   
    pyslat.MakeRecorder('edpim-{:>02}'.format(i), 'edpim', edp,
                    {'filename': ResultsFile("im_edp_{}".format(i))},
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
                    {'filename': ResultsFile("edp_{}_rate".format(i))},
                     None,
                    at)


# Fragility, cost, component group:
SIMPLE_FRAG_DATA = [[3, [[0.004, 0.39], [0.0095, 0.25], [0.02, 0.62], [0.0428, 0.36]],
                     [[590, 0.59], [2360, 0.63], [5900, 0.67], [5900, 0.67]],
                     [[None, None], [None, None], [None, None], [None, None]]],
                    [105, [[0.0039, 0.17], [0.0085, 0.23]],
                     [[29.9, 0.2], [178.7, 0.2]],
                     [[None, None], [None, None]]],
                    [107, [[0.04, 0.36], [0.046, 0.33]],
                     [ [131.7, 0.26], [131.7, 0.26]],
                     [ [None, None], [None, None]]],
                    [211, [[32, 1.4]],
                     [[900, 1.0]],
                     [[None, None]]],
                    [106, [[0.0039, 0.17]],
                     [[16.7, 0.2]],
                     [[None, None]]],
                    [108, [[0.004, 0.5], [0.008, 0.5], [0.025, 0.5], [0.050, 0.5]],
                     [[  250.0, 0.63], [ 1000.0, 0.63], [ 5000.0, 0.63], [10000.0, 0.63]],
                     [[  None, None], [ None, None], [ None, None], [None, None]]],
                    [214, [[0.25, 0.6], [0.50, 0.6], [1.00, 0.6], [2.00, 0.6]],
                     [[ 200.0, 0.63], [ 1200.0, 0.63], [ 3600.0, 0.63], [10000.0, 0.63]],
                     [[ None, None], [ None, None], [ None, None], [None, None]]]]

for f in SIMPLE_FRAG_DATA:
    id = f[0]
    frag = f[1]
    cost = f[2]
    delay = f[3]
    
    pyslat.fragfn_user("FRAG.{:>03}".format(id), 
                       {'mu': pyslat.LOGNORMAL_MU_TYPE.MEAN_X,
                        'sd': pyslat.LOGNORMAL_SIGMA_TYPE.SD_LN_X},
                       frag)
    

    pyslat.simplelossfn("COST_{:>03}".format(id), 
                  {'mu': pyslat.LOGNORMAL_MU_TYPE.MEAN_X,
                   'sd': pyslat.LOGNORMAL_SIGMA_TYPE.SD_LN_X},
                  cost)

    pyslat.simplelossfn("DELAY_{:>03}".format(id), 
                  {'mu': pyslat.LOGNORMAL_MU_TYPE.MEAN_X,
                   'sd': pyslat.LOGNORMAL_SIGMA_TYPE.SD_LN_X},
                  delay)

BILEVEL_FRAG_DATA = [[2, [[0.005, 0.40], [0.010, 0.45], [0.030, 0.50], [0.060, 0.60]],
                      [[6, 12, 1143, 1143, 0.42], 
                       [6, 12, 3214, 3214, 0.40], 
                       [6, 12, 4900, 4900, 0.37],
                       [6, 12, 4900, 4900, 0.37]],
                      [[57.772, 0.39], [ 91.2312, 0.3097], [ 108.65175, 0.2964], [ 108.65175, 0.2964]]],
                    [203, [[0.55, 0.4], [1.0, 0.4]],
                     [[9, 900, 58.4, 43.2, 0.4], 
                      [9, 900, 297.9, 277.8,  0.4]],
                     [[None, None], [None, None]]],
                    [204, [[0.4, 0.3]],
                     [[3, 5, 56000, 33600, 0.2]],
                     [[None, None]]],
                    [208, [[1.2, 0.6]],
                     [[10, 100, 2500, 1000, 0.4]],
                     [[None, None]]],
                    [209, [[0.8, 0.5]],
                     [[2, 6, 50000, 40000, 0.4]],
                     [[None, None]]],
                    [205, [[1.6, 0.5]],
                     [[2, 8, 220000, 150000, 0.6]],
                     [[None, None]]],
]

for f in BILEVEL_FRAG_DATA:
    id = f[0]
    frag = f[1]
    cost = f[2]
    delay = f[3]
    
    pyslat.fragfn_user("FRAG.{:>03}".format(id), 
                       {'mu': pyslat.LOGNORMAL_MU_TYPE.MEAN_X,
                        'sd': pyslat.LOGNORMAL_SIGMA_TYPE.SD_LN_X},
                       frag)
    

    pyslat.bilevellossfn("COST_{:>03}".format(id), cost)

    pyslat.simplelossfn("DELAY_{:>03}".format(id), 
                  {'mu': pyslat.LOGNORMAL_MU_TYPE.MEAN_X,
                   'sd': pyslat.LOGNORMAL_SIGMA_TYPE.SD_LN_X},
                  delay)

COMPONENT_DATA = [[1, "EDP.02", "FRAG.002", "COST_002", "DELAY_002", 20],
                  [2, "EDP.02", "FRAG.002", "COST_002", "DELAY_002", 18],
                  [3, "EDP.04", "FRAG.002", "COST_002", "DELAY_002", 4],
                  [4, "EDP.04", "FRAG.002", "COST_002", "DELAY_002", 18],
                  [5, "EDP.06", "FRAG.002", "COST_002", "DELAY_002", 4],
                  [6, "EDP.06", "FRAG.002", "COST_002", "DELAY_002", 18],
                  [7, "EDP.08", "FRAG.002", "COST_002", "DELAY_002", 4],
                  [8, "EDP.08", "FRAG.002", "COST_002", "DELAY_002", 18],
                  [9, "EDP.10", "FRAG.002", "COST_002", "DELAY_002", 4],
                  [10, "EDP.10", "FRAG.002", "COST_002", "DELAY_002", 18],
                  [11, "EDP.12", "FRAG.002", "COST_002", "DELAY_002", 4],
                  [12, "EDP.12", "FRAG.002", "COST_002", "DELAY_002", 18],
                  [13, "EDP.14", "FRAG.002", "COST_002", "DELAY_002", 4],
                  [14, "EDP.14", "FRAG.002", "COST_002", "DELAY_002", 18],
                  [15, "EDP.16", "FRAG.002", "COST_002", "DELAY_002", 4],
                  [16, "EDP.16", "FRAG.002", "COST_002", "DELAY_002", 18],
                  [17, "EDP.18", "FRAG.002", "COST_002", "DELAY_002", 4],
                  [18, "EDP.18", "FRAG.002", "COST_002", "DELAY_002", 18],
                  [19, "EDP.20", "FRAG.002", "COST_002", "DELAY_002", 4],
                  [20, "EDP.20", "FRAG.002", "COST_002", "DELAY_002", 18],
                  [21, "EDP.02", "FRAG.003", "COST_003", "DELAY_003", 16],
                  [22, "EDP.04", "FRAG.003", "COST_003", "DELAY_003", 16],
                  [23, "EDP.06", "FRAG.003", "COST_003", "DELAY_003", 16],
                  [24, "EDP.08", "FRAG.003", "COST_003", "DELAY_003", 16],
                  [25, "EDP.10", "FRAG.003", "COST_003", "DELAY_003", 16],
                  [26, "EDP.12", "FRAG.003", "COST_003", "DELAY_003", 16],
                  [27, "EDP.14", "FRAG.003", "COST_003", "DELAY_003", 16],
                  [28, "EDP.16", "FRAG.003", "COST_003", "DELAY_003", 16],
                  [29, "EDP.18", "FRAG.003", "COST_003", "DELAY_003", 16],
                  [30, "EDP.20", "FRAG.003", "COST_003", "DELAY_003", 16],
                  [31, "EDP.02", "FRAG.105", "COST_105", "DELAY_105", 721],
                  [32, "EDP.04", "FRAG.105", "COST_105", "DELAY_105", 721],
                  [33, "EDP.06", "FRAG.105", "COST_105", "DELAY_105", 721],
                  [34, "EDP.08", "FRAG.105", "COST_105", "DELAY_105", 721],
                  [35, "EDP.10", "FRAG.105", "COST_105", "DELAY_105", 721],
                  [36, "EDP.12", "FRAG.105", "COST_105", "DELAY_105", 721],
                  [37, "EDP.14", "FRAG.105", "COST_105", "DELAY_105", 721],
                  [38, "EDP.16", "FRAG.105", "COST_105", "DELAY_105", 721],
                  [39, "EDP.18", "FRAG.105", "COST_105", "DELAY_105", 721],
                  [40, "EDP.20", "FRAG.105", "COST_105", "DELAY_105", 721],
                  [41, "EDP.02", "FRAG.107", "COST_107", "DELAY_107", 99],
                  [42, "EDP.04", "FRAG.107", "COST_107", "DELAY_107", 99],
                  [43, "EDP.06", "FRAG.107", "COST_107", "DELAY_107", 99],
                  [44, "EDP.08", "FRAG.107", "COST_107", "DELAY_107", 99],
                  [45, "EDP.10", "FRAG.107", "COST_107", "DELAY_107", 99],
                  [46, "EDP.12", "FRAG.107", "COST_107", "DELAY_107", 99],
                  [47, "EDP.14", "FRAG.107", "COST_107", "DELAY_107", 99],
                  [48, "EDP.16", "FRAG.107", "COST_107", "DELAY_107", 99],
                  [49, "EDP.18", "FRAG.107", "COST_107", "DELAY_107", 99],
                  [50, "EDP.20", "FRAG.107", "COST_107", "DELAY_107", 99],
                  [51, "EDP.03", "FRAG.203", "COST_203", "DELAY_203", 693],
                  [52, "EDP.05", "FRAG.203", "COST_203", "DELAY_203", 693],
                  [53, "EDP.07", "FRAG.203", "COST_203", "DELAY_203", 693],
                  [54, "EDP.09", "FRAG.203", "COST_203", "DELAY_203", 693],
                  [55, "EDP.11", "FRAG.203", "COST_203", "DELAY_203", 693],
                  [56, "EDP.13", "FRAG.203", "COST_203", "DELAY_203", 693],
                  [57, "EDP.15", "FRAG.203", "COST_203", "DELAY_203", 693],
                  [58, "EDP.17", "FRAG.203", "COST_203", "DELAY_203", 693],
                  [59, "EDP.19", "FRAG.203", "COST_203", "DELAY_203", 693],
                  [60, "EDP.21", "FRAG.203", "COST_203", "DELAY_203", 693],
                  [61, "EDP.03", "FRAG.211", "COST_211", "DELAY_211", 23],
                  [62, "EDP.05", "FRAG.211", "COST_211", "DELAY_211", 23],
                  [63, "EDP.07", "FRAG.211", "COST_211", "DELAY_211", 23],
                  [64, "EDP.09", "FRAG.211", "COST_211", "DELAY_211", 23],
                  [65, "EDP.11", "FRAG.211", "COST_211", "DELAY_211", 23],
                  [66, "EDP.13", "FRAG.211", "COST_211", "DELAY_211", 23],
                  [67, "EDP.15", "FRAG.211", "COST_211", "DELAY_211", 23],
                  [68, "EDP.17", "FRAG.211", "COST_211", "DELAY_211", 23],
                  [69, "EDP.19", "FRAG.211", "COST_211", "DELAY_211", 23],
                  [70, "EDP.21", "FRAG.211", "COST_211", "DELAY_211", 23],
                  [71, "EDP.01", "FRAG.208", "COST_208", "DELAY_208", 53],
                  [72, "EDP.03", "FRAG.208", "COST_208", "DELAY_208", 53],
                  [73, "EDP.05", "FRAG.208", "COST_208", "DELAY_208", 53],
                  [74, "EDP.07", "FRAG.208", "COST_208", "DELAY_208", 53],
                  [75, "EDP.09", "FRAG.208", "COST_208", "DELAY_208", 53],
                  [76, "EDP.11", "FRAG.208", "COST_208", "DELAY_208", 53],
                  [77, "EDP.13", "FRAG.208", "COST_208", "DELAY_208", 53],
                  [78, "EDP.15", "FRAG.208", "COST_208", "DELAY_208", 53],
                  [79, "EDP.17", "FRAG.208", "COST_208", "DELAY_208", 53],
                  [80, "EDP.19", "FRAG.208", "COST_208", "DELAY_208", 53],
                  [81, "EDP.05", "FRAG.209", "COST_209", "DELAY_209", 16],
                  [82, "EDP.11", "FRAG.209", "COST_209", "DELAY_209", 16],
                  [83, "EDP.19", "FRAG.209", "COST_209", "DELAY_209", 16],
                  [84, "EDP.21", "FRAG.205", "COST_205", "DELAY_205", 4],
                  [85, "EDP.01", "FRAG.204", "COST_204", "DELAY_204", 2],
                  [86, "EDP.02", "FRAG.106", "COST_106", "DELAY_106", 721],
                  [87, "EDP.04", "FRAG.106", "COST_106", "DELAY_106", 721],
                  [88, "EDP.06", "FRAG.106", "COST_106", "DELAY_106", 721],
                  [89, "EDP.08", "FRAG.106", "COST_106", "DELAY_106", 721],
                  [90, "EDP.10", "FRAG.106", "COST_106", "DELAY_106", 721],
                  [91, "EDP.12", "FRAG.106", "COST_106", "DELAY_106", 721],
                  [92, "EDP.14", "FRAG.106", "COST_106", "DELAY_106", 721],
                  [93, "EDP.16", "FRAG.106", "COST_106", "DELAY_106", 721],
                  [94, "EDP.18", "FRAG.106", "COST_106", "DELAY_106", 721],
                  [95, "EDP.20", "FRAG.106", "COST_106", "DELAY_106", 721],
                  [96, "EDP.02", "FRAG.108", "COST_108", "DELAY_108", 10],
                  [97, "EDP.04", "FRAG.108", "COST_108", "DELAY_108", 10],
                  [98, "EDP.06", "FRAG.108", "COST_108", "DELAY_108", 10],
                  [99, "EDP.08", "FRAG.108", "COST_108", "DELAY_108", 10],
                  [100, "EDP.10", "FRAG.108", "COST_108", "DELAY_108", 10],
                  [101, "EDP.12", "FRAG.108", "COST_108", "DELAY_108", 10],
                  [102, "EDP.14", "FRAG.108", "COST_108", "DELAY_108", 10],
                  [103, "EDP.16", "FRAG.108", "COST_108", "DELAY_108", 10],
                  [104, "EDP.18", "FRAG.108", "COST_108", "DELAY_108", 10],
                  [105, "EDP.20", "FRAG.108", "COST_108", "DELAY_108", 10],
                  [106, "EDP.01", "FRAG.214", "COST_214", "DELAY_214", 10],
                  [107, "EDP.03", "FRAG.214", "COST_214", "DELAY_214", 10],
                  [108, "EDP.05", "FRAG.214", "COST_214", "DELAY_214", 10],
                  [109, "EDP.07", "FRAG.214", "COST_214", "DELAY_214", 10],
                  [110, "EDP.09", "FRAG.214", "COST_214", "DELAY_214", 10],
                  [111, "EDP.11", "FRAG.214", "COST_214", "DELAY_214", 10],
                  [112, "EDP.13", "FRAG.214", "COST_214", "DELAY_214", 10],
                  [113, "EDP.15", "FRAG.214", "COST_214", "DELAY_214", 10],
                  [114, "EDP.17", "FRAG.214", "COST_214", "DELAY_214", 10],
                  [115, "EDP.19", "FRAG.214", "COST_214", "DELAY_214", 10]]

building = pyslat.structure("building")
building.setRebuildCost(pyslat.MakeLogNormalDist(14E6, pyslat.LOGNORMAL_MU_TYPE.MEAN_X,
                                                  0.35, pyslat.LOGNORMAL_SIGMA_TYPE.SD_LN_X))
building.setRebuildCost(pyslat.MakeLogNormalDist(14E6, pyslat.LOGNORMAL_MU_TYPE.MEAN_X,
                                                  0.35, pyslat.LOGNORMAL_SIGMA_TYPE.SD_LN_X))
building.setDemolitionCost(pyslat.MakeLogNormalDist(14E6, pyslat.LOGNORMAL_MU_TYPE.MEAN_X,
                                                  0.35, pyslat.LOGNORMAL_SIGMA_TYPE.SD_LN_X))
for comp in COMPONENT_DATA:
    id = comp[0]
    edp = comp[1]
    frag = comp[2]
    cost = comp[3]
    delay = comp[4]
    count = comp[5]

    cg = pyslat.compgroup(
        "COMPGROUP_{:>03}".format(id),
        pyslat.edp.lookup(edp),
        pyslat.fragfn.lookup(frag),
        pyslat.lossfn.lookup(cost),
        pyslat.lossfn.lookup(delay),
        count)
    print(id, edp, frag, cost, count)
    if id in [1, 2, 86, 96]:
        at_values = costedpvalues1
    elif id in [21, 31, 41]:
        at_values = costedpvalues5
    elif id in [71, 85, 106]: 
        at_values = costedpvalues4
    elif id in [86, 87]:
        at_values = costedpvalues2
    elif (id >= 51 and id < 87) or (id >= 107):
        at_values = costedpvalues3
    else:
        at_values = costedpvalues2
        
    pyslat.MakeRecorder("COSTEDP_{:>03}_REC".format(id), 'costedp', cg,
                    {'filename': ResultsFile("cost_{}_edp".format(id))},
                    None, 
                    at_values)

    pyslat.MakeRecorder("COSTIM_{:>03}_REC".format(id), 'costim', cg,
                    {'filename': ResultsFile("cost_{}_im".format(id))},
                    None, 
                    costimvalues)

    if id == 1:
        pyslat.MakeRecorder("DELAYEDP_{:>03}_REC".format(id), 'delayedp', cg,
                            {'filename': ResultsFile("delay_{}_edp".format(id))},
                            None, 
                            at_values)

        pyslat.MakeRecorder("DELAYIM_{:>03}_REC".format(id), 'delayim', cg,
                            {'filename': ResultsFile("delay_{}_im".format(id))},
                            None, 
                            costimvalues)
    
    pyslat.MakeRecorder("DSEDP_{:>03}_REC".format(id), 'dsedp', cg,
                    {'filename': ResultsFile("ds_edp_{}".format(id))},
                    None, 
                    pyslat.frange(0.0, 0.200, 0.01))

    pyslat.MakeRecorder("DSIM_{:>03}_REC".format(id), 'dsim', cg,
                    {'filename': ResultsFile("ds_im_{}".format(id))},
                    None, 
                    linvalues)

    pyslat.MakeRecorder("DSRATE_{:>03}_REC".format(id), 'dsrate', cg,
                    {'filename': ResultsFile("ds_rate_{:>03}".format(id))},
                    None, 
                    None)

    pyslat.MakeRecorder("COSTRATE_{:>03}_REC".format(id), 'costrate', cg,
                    {'filename': ResultsFile("cost_rate_{}".format(id))},
                    None, 
                    pyslat.frange(1E-4, 1.2, 4.8E-3))

    building.AddCompGroup(cg)


#pyslat.MakeRecorder("deagg", 
#                "deagg", 
#                pyslat.structure.lookup("building"),
#                {'filename': "py-results/deagg", 'append': False},
#                None, 
#                linvalues)

pyslat.MakeRecorder("anncost", 
                "structcost", 
                pyslat.structure.lookup("building"),
                {'filename': "py-results/ann_cost", 'append': False, 'structcost-type': 'annual'},
                ['mean_x', 'sd_ln_x'], 
                None)

pyslat.MakeRecorder("STRUCTCOST_FATE_REC", 
                "structcost", 
                pyslat.structure.lookup("building"),
                {'filename': "py-results/cost_by_fate", 'append': False, 'structcost-type': 'by-fate'},
                None, 
                linvalues)

pyslat.MakeRecorder("STRUCTCOST_EDP_REC", 
                "structcost", 
                pyslat.structure.lookup("building"),
                {'filename': "py-results/cost_by_edp", 'append': False, 'structcost-type': 'by-edp'},
                None, 
                linvalues)

pyslat.MakeRecorder("STRUCTCOST_COMP_REC", 
                "structcost", 
                pyslat.structure.lookup("building"),
                {'filename': "py-results/cost_by_frag", 'append': False, 'structcost-type': 'by-frag'},
                None, 
                linvalues)

pyslat.MakeRecorder("STRUCTCOST_NPV_REC", 
                "structcost", 
                pyslat.structure.lookup("building"),
                {'filename': "py-results/expected_loss", 'append': False, 'structcost-type': 'npv', 'discount-rate': 0.06},
                None, 
                list(range(1, 100)))

pyslat.MakeRecorder("TOTALCOST_COMP_REC", 
                "totalcost", 
                pyslat.structure.lookup("building"),
                {'filename': "py-results/total_cost", 'append': False},
                None, 
                linvalues)

pyslat.MakeRecorder("PDF_REC",
                "pdf", 
                pyslat.structure.lookup("building"),
                {'filename': "py-results/pdf", 'append': False},
                None, 
                pyslat.linrange(0.00, 1.5, 200))

pyslat.MakeRecorder("NORM_PDF_REC",
                "pdf", 
                pyslat.structure.lookup("building"),
                {'filename': "py-results/norm_pdf", 'append': False, 'normalise': True},
                None, 
                pyslat.linrange(0.0, 1.5, 200))

pyslat.Set_Integration_Tolerance(1.0E-3)
pyslat.Set_Integration_Eval_Limit(2048)
pyslat.Set_Integration_Search_Limit(2048)
pyslat.Set_Integration_Method(pyslat.INTEGRATION_METHOD.DIRECTED)

#for r in pyslat.recorder.all():
#    r.run()
    

    
print(pyslat.Format_Statistics())

with redirect_stdout(open("cg_costs.txt", "w")):
    # Dump expected cost by component group
    edp_ids = list(pyslat.edp.defs.keys())
    edp_ids.sort()
    for edp_id in edp_ids:
        print(edp_id)
        cg_ids = []
        for cg in pyslat.compgroup.defs.values():
            if edp_id == cg._edp.id():
                cg_ids.append(cg.id())
        cg_ids.sort()
        for cg_id in cg_ids:
            cg = pyslat.compgroup.lookup(cg_id)

            print("{:10} {:10} {:5} {:>15.6}".format(
                cg_id,
                cg.fragfn().id(),
                cg._count,
                cg.E_annual_cost()))
        print()

    print(building.AnnualCost())
