#! /usr/bin/env python3
import pyslat

IM1 = pyslat.ImportIMFn("IM_1", "imfunc.txt")
pyslat.recorder('pre-imrate-rec', 'imrate', IM1,
                {'filename': "py-results/im_rate_0"},
                None, [0.01, 0.02, 0.04, 0.06, 0.08, 0.10, 0.20, 0.30, 0.40, 0.50, 0.60, 0.70, 0.80, 0.90, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0])
IM1.SetCollapse(pyslat.collapse(None, 0.9, 0.470))

def ResultsFile(name):
    return "py-results/{}".format(name)


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
                None, None);



for i in pyslat.frange(0.0, 1.5, 0.1):
    print(i, pyslat.im.lookup("IM_1").pCollapse(i))

N_EDPS=0
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
                  [0, "EDP_10", "FRAG_2", "LOSS_2", 18],
                  [1, "EDP_12", "FRAG_2", "LOSS_2", 4],
                  [2, "EDP_12", "FRAG_2", "LOSS_2", 18],
                  [3, "EDP_14", "FRAG_2", "LOSS_2", 4],
                  [4, "EDP_14", "FRAG_2", "LOSS_2", 18],
                  [5, "EDP_16", "FRAG_2", "LOSS_2", 4],
                  [6, "EDP_16", "FRAG_2", "LOSS_2", 18],
                  [7, "EDP_18", "FRAG_2", "LOSS_2", 4],
                  [8, "EDP_18", "FRAG_2", "LOSS_2", 18],
                  [9, "EDP_20", "FRAG_2", "LOSS_2", 4],
                  [0, "EDP_20", "FRAG_2", "LOSS_2", 18],
                  [1, "EDP_2", "FRAG_3", "LOSS_3", 16],
                  [2, "EDP_4", "FRAG_3", "LOSS_3", 16],
                  [3, "EDP_6", "FRAG_3", "LOSS_3", 16],
                  [4, "EDP_8", "FRAG_3", "LOSS_3", 16],
                  [5, "EDP_10", "FRAG_3", "LOSS_3", 16],
                  [6, "EDP_12", "FRAG_3", "LOSS_3", 16],
                  [7, "EDP_14", "FRAG_3", "LOSS_3", 16],
                  [8, "EDP_16", "FRAG_3", "LOSS_3", 16],
                  [9, "EDP_18", "FRAG_3", "LOSS_3", 16],
                  [0, "EDP_20", "FRAG_3", "LOSS_3", 16],
                  [1, "EDP_2", "FRAG_105", "LOSS_105", 721],
                  [2, "EDP_4", "FRAG_105", "LOSS_105", 721],
                  [3, "EDP_6", "FRAG_105", "LOSS_105", 721],
                  [4, "EDP_8", "FRAG_105", "LOSS_105", 721],
                  [5, "EDP_10", "FRAG_105", "LOSS_105", 721],
                  [6, "EDP_12", "FRAG_105", "LOSS_105", 721],
                  [7, "EDP_14", "FRAG_105", "LOSS_105", 721],
                  [8, "EDP_16", "FRAG_105", "LOSS_105", 721],
                  [9, "EDP_18", "FRAG_105", "LOSS_105", 721],
                  [0, "EDP_20", "FRAG_105", "LOSS_105", 721],
                  [1, "EDP_2", "FRAG_107", "LOSS_107", 99],
                  [2, "EDP_4", "FRAG_107", "LOSS_107", 99],
                  [3, "EDP_6", "FRAG_107", "LOSS_107", 99],
                  [4, "EDP_8", "FRAG_107", "LOSS_107", 99],
                  [5, "EDP_10", "FRAG_107", "LOSS_107", 99],
                  [6, "EDP_12", "FRAG_107", "LOSS_107", 99],
                  [7, "EDP_14", "FRAG_107", "LOSS_107", 99],
                  [8, "EDP_16", "FRAG_107", "LOSS_107", 99],
                  [9, "EDP_18", "FRAG_107", "LOSS_107", 99],
                  [0, "EDP_20", "FRAG_107", "LOSS_107", 99],
                  [1, "EDP_3", "FRAG_203", "LOSS_203", 693],
                  [2, "EDP_5", "FRAG_203", "LOSS_203", 693],
                  [3, "EDP_7", "FRAG_203", "LOSS_203", 693],
                  [4, "EDP_9", "FRAG_203", "LOSS_203", 693],
                  [5, "EDP_11", "FRAG_203", "LOSS_203", 693],
                  [6, "EDP_13", "FRAG_203", "LOSS_203", 693],
                  [7, "EDP_15", "FRAG_203", "LOSS_203", 693],
                  [8, "EDP_17", "FRAG_203", "LOSS_203", 693],
                  [9, "EDP_19", "FRAG_203", "LOSS_203", 693],
                  [0, "EDP_21", "FRAG_203", "LOSS_203", 693],
                  [1, "EDP_3", "FRAG_211", "LOSS_211", 23],
                  [2, "EDP_5", "FRAG_211", "LOSS_211", 23],
                  [3, "EDP_7", "FRAG_211", "LOSS_211", 23],
                  [4, "EDP_9", "FRAG_211", "LOSS_211", 23],
                  [5, "EDP_11", "FRAG_211", "LOSS_211", 23],
                  [6, "EDP_13", "FRAG_211", "LOSS_211", 23],
                  [7, "EDP_15", "FRAG_211", "LOSS_211", 23],
                  [8, "EDP_17", "FRAG_211", "LOSS_211", 23],
                  [9, "EDP_19", "FRAG_211", "LOSS_211", 23],
                  [0, "EDP_21", "FRAG_211", "LOSS_211", 23],
                  [1, "EDP_1", "FRAG_208", "LOSS_208", 53],
                  [2, "EDP_3", "FRAG_208", "LOSS_208", 53],
                  [3, "EDP_5", "FRAG_208", "LOSS_208", 53],
                  [4, "EDP_7", "FRAG_208", "LOSS_208", 53],
                  [5, "EDP_9", "FRAG_208", "LOSS_208", 53],
                  [6, "EDP_11", "FRAG_208", "LOSS_208", 53],
                  [7, "EDP_13", "FRAG_208", "LOSS_208", 53],
                  [8, "EDP_15", "FRAG_208", "LOSS_208", 53],
                  [9, "EDP_17", "FRAG_208", "LOSS_208", 53],
                  [0, "EDP_19", "FRAG_208", "LOSS_208", 53],
                  [1, "EDP_5", "FRAG_209", "LOSS_209", 16],
                  [2, "EDP_11", "FRAG_209", "LOSS_209", 16],
                  [3, "EDP_19", "FRAG_209", "LOSS_209", 16],
                  [4, "EDP_21", "FRAG_205", "LOSS_205", 4],
                  [5, "EDP_1", "FRAG_204", "LOSS_204", 2],
                  [6, "EDP_2", "FRAG_106", "LOSS_106", 721],
                  [7, "EDP_4", "FRAG_106", "LOSS_106", 721],
                  [8, "EDP_6", "FRAG_106", "LOSS_106", 721],
                  [9, "EDP_8", "FRAG_106", "LOSS_106", 721],
                  [0, "EDP_10", "FRAG_106", "LOSS_106", 721],
                  [1, "EDP_12", "FRAG_106", "LOSS_106", 721],
                  [2, "EDP_14", "FRAG_106", "LOSS_106", 721],
                  [3, "EDP_16", "FRAG_106", "LOSS_106", 721],
                  [4, "EDP_18", "FRAG_106", "LOSS_106", 721],
                  [5, "EDP_20", "FRAG_106", "LOSS_106", 721],
                  [6, "EDP_2", "FRAG_108", "LOSS_108", 10],
                  [7, "EDP_4", "FRAG_108", "LOSS_108", 10],
                  [8, "EDP_6", "FRAG_108", "LOSS_108", 10],
                  [9, "EDP_8", "FRAG_108", "LOSS_108", 10],
                  [0, "EDP_10", "FRAG_108", "LOSS_108", 10],
                  [1, "EDP_12", "FRAG_108", "LOSS_108", 10],
                  [2, "EDP_14", "FRAG_108", "LOSS_108", 10],
                  [3, "EDP_16", "FRAG_108", "LOSS_108", 10],
                  [4, "EDP_18", "FRAG_108", "LOSS_108", 10],
                  [5, "EDP_20", "FRAG_108", "LOSS_108", 10],
                  [6, "EDP_1", "FRAG_214", "LOSS_214", 10],
                  [7, "EDP_3", "FRAG_214", "LOSS_214", 10],
                  [8, "EDP_5", "FRAG_214", "LOSS_214", 10],
                  [9, "EDP_7", "FRAG_214", "LOSS_214", 10],
                  [0, "EDP_9", "FRAG_214", "LOSS_214", 10],
                  [1, "EDP_11", "FRAG_214", "LOSS_214", 10],
                  [2, "EDP_13", "FRAG_214", "LOSS_214", 10],
                  [3, "EDP_15", "FRAG_214", "LOSS_214", 10],
                  [4, "EDP_17", "FRAG_214", "LOSS_214", 10],
                  [5, "EDP_19", "FRAG_214", "LOSS_214", 10]]

pyslat.structure("building")
if False:
    for comp in COMPONENT_DATA:
        id = comp[0]
        edp = comp[1]
        frag = comp[2]
        loss = comp[3]
        count = comp[4]
        
        pyslat.compgroup(id, pyslat.edp.lookup(edp),
                         pyslat.fragfn.lookup(frag),
                         pyslat.lossfn.lookup(loss),
                         count)
        pyslat.structure.lookup("building").AddCompGroup(pyslat.compgroup.lookup(id))



if False:
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

#pyslat.recorder.lookup("deagg").run()
for r in pyslat.recorder.all():
    #print(r)
    r.run()
    
print(IM1)
                

    
