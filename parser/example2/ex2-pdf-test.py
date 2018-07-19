#! /usr/bin/env python3

# This file is based on the second example in Brendon Bradley's original SLAT manual,
# but has been modified to user three different IM variants, to help assess how they
# affect the output.

import pyslat
import filecmp
from contextlib import redirect_stdout

pyslat.LogToStdErr(True)  # Log errors to stderr (on by default)
pyslat.SetLogFile("ex2-pdf-test.log") # Save errors in 'exercise2.log'

pyslat.set_input_directory("input")
pyslat.set_output_directory("pdf-test-results")

IM1 = pyslat.ImportIMFn("IM.1", "imfunc.csv")
IM2 = pyslat.ImportIMFn("IM.2", "imfunc_trunc_both.csv")
IM3 = pyslat.ImportIMFn("IM.3", "nzs.csv")

def ResultsFile(name):
    return "pdf-test-results/{}".format(name)

imvalues = pyslat.logrange(0.01, 3.0, 199)
pyslat.MakeRecorder('imrate-rec-1', 'imrate', IM1,
                {'filename': ResultsFile("im_rate-1")},
                None, imvalues)
pyslat.MakeRecorder('imrate-rec-2', 'imrate', IM2,
                {'filename': ResultsFile("im_rate-2")},
                None, imvalues)
pyslat.MakeRecorder('imrate-rec-3', 'imrate', IM3,
                {'filename': ResultsFile("im_rate-3")},
                None, imvalues)

N_EDPS=21

for i in range(1, N_EDPS + 1):
    pyslat.ImportProbFn("EDP_FUNC_{:>02}".format(i),
                        "RB_EDP{}.csv".format(i))
    pyslat.edp("EDP.{:>02}.1".format(i), IM1,
               pyslat.probfn.lookup("EDP_FUNC_{:>02}".format(i)))
    pyslat.edp("EDP.{:>02}.2".format(i), IM2,
               pyslat.probfn.lookup("EDP_FUNC_{:>02}".format(i)))
    pyslat.edp("EDP.{:>02}.3".format(i), IM3,
               pyslat.probfn.lookup("EDP_FUNC_{:>02}".format(i)))

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
                       {'mu': pyslat.LOGNORMAL_MU_TYPE.MEDIAN_X,
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
                       {'mu': pyslat.LOGNORMAL_MU_TYPE.MEDIAN_X,
                        'sd': pyslat.LOGNORMAL_SIGMA_TYPE.SD_LN_X},
                       frag)
    

    pyslat.bilevellossfn("COST_{:>03}".format(id), cost)

    pyslat.simplelossfn("DELAY_{:>03}".format(id), 
                  {'mu': pyslat.LOGNORMAL_MU_TYPE.MEAN_X,
                   'sd': pyslat.LOGNORMAL_SIGMA_TYPE.SD_LN_X},
                  delay)

COMPONENT_DATA = [[1, "EDP.02", "FRAG.002", "COST_002", "DELAY_002", 20, 1.0, 1.0],
                  [2, "EDP.02", "FRAG.002", "COST_002", "DELAY_002", 18, 1.0, 1.0],
                  [3, "EDP.04", "FRAG.002", "COST_002", "DELAY_002", 4, 1.0, 1.0],
                  [4, "EDP.04", "FRAG.002", "COST_002", "DELAY_002", 18, 1.0, 1.0],
                  [5, "EDP.06", "FRAG.002", "COST_002", "DELAY_002", 4, 1.0, 1.0],
                  [6, "EDP.06", "FRAG.002", "COST_002", "DELAY_002", 18, 1.0, 1.0],
                  [7, "EDP.08", "FRAG.002", "COST_002", "DELAY_002", 4, 1.0, 1.0],
                  [8, "EDP.08", "FRAG.002", "COST_002", "DELAY_002", 18, 1.0, 1.0],
                  [9, "EDP.10", "FRAG.002", "COST_002", "DELAY_002", 4, 1.0, 1.0],
                  [10, "EDP.10", "FRAG.002", "COST_002", "DELAY_002", 18, 1.0, 1.0],
                  [11, "EDP.12", "FRAG.002", "COST_002", "DELAY_002", 4, 1.0, 1.0],
                  [12, "EDP.12", "FRAG.002", "COST_002", "DELAY_002", 18, 1.0, 1.0],
                  [13, "EDP.14", "FRAG.002", "COST_002", "DELAY_002", 4, 1.0, 1.0],
                  [14, "EDP.14", "FRAG.002", "COST_002", "DELAY_002", 18, 1.0, 1.0],
                  [15, "EDP.16", "FRAG.002", "COST_002", "DELAY_002", 4, 1.0, 1.0],
                  [16, "EDP.16", "FRAG.002", "COST_002", "DELAY_002", 18, 1.0, 1.0],
                  [17, "EDP.18", "FRAG.002", "COST_002", "DELAY_002", 4, 1.0, 1.0],
                  [18, "EDP.18", "FRAG.002", "COST_002", "DELAY_002", 18, 1.0, 1.0],
                  [19, "EDP.20", "FRAG.002", "COST_002", "DELAY_002", 4, 1.0, 1.0],
                  [20, "EDP.20", "FRAG.002", "COST_002", "DELAY_002", 18, 1.0, 1.0],
                  [21, "EDP.02", "FRAG.003", "COST_003", "DELAY_003", 16, 1.0, 1.0],
                  [22, "EDP.04", "FRAG.003", "COST_003", "DELAY_003", 16, 1.0, 1.0],
                  [23, "EDP.06", "FRAG.003", "COST_003", "DELAY_003", 16, 1.0, 1.0],
                  [24, "EDP.08", "FRAG.003", "COST_003", "DELAY_003", 16, 1.0, 1.0],
                  [25, "EDP.10", "FRAG.003", "COST_003", "DELAY_003", 16, 1.0, 1.0],
                  [26, "EDP.12", "FRAG.003", "COST_003", "DELAY_003", 16, 1.0, 1.0],
                  [27, "EDP.14", "FRAG.003", "COST_003", "DELAY_003", 16, 1.0, 1.0],
                  [28, "EDP.16", "FRAG.003", "COST_003", "DELAY_003", 16, 1.0, 1.0],
                  [29, "EDP.18", "FRAG.003", "COST_003", "DELAY_003", 16, 1.0, 1.0],
                  [30, "EDP.20", "FRAG.003", "COST_003", "DELAY_003", 16, 1.0, 1.0],
                  [31, "EDP.02", "FRAG.105", "COST_105", "DELAY_105", 721, 1.0, 1.0],
                  [32, "EDP.04", "FRAG.105", "COST_105", "DELAY_105", 721, 1.0, 1.0],
                  [33, "EDP.06", "FRAG.105", "COST_105", "DELAY_105", 721, 1.0, 1.0],
                  [34, "EDP.08", "FRAG.105", "COST_105", "DELAY_105", 721, 1.0, 1.0],
                  [35, "EDP.10", "FRAG.105", "COST_105", "DELAY_105", 721, 1.0, 1.0],
                  [36, "EDP.12", "FRAG.105", "COST_105", "DELAY_105", 721, 1.0, 1.0],
                  [37, "EDP.14", "FRAG.105", "COST_105", "DELAY_105", 721, 1.0, 1.0],
                  [38, "EDP.16", "FRAG.105", "COST_105", "DELAY_105", 721, 1.0, 1.0],
                  [39, "EDP.18", "FRAG.105", "COST_105", "DELAY_105", 721, 1.0, 1.0],
                  [40, "EDP.20", "FRAG.105", "COST_105", "DELAY_105", 721, 1.0, 1.0],
                  [41, "EDP.02", "FRAG.107", "COST_107", "DELAY_107", 99, 1.0, 1.0],
                  [42, "EDP.04", "FRAG.107", "COST_107", "DELAY_107", 99, 1.0, 1.0],
                  [43, "EDP.06", "FRAG.107", "COST_107", "DELAY_107", 99, 1.0, 1.0],
                  [44, "EDP.08", "FRAG.107", "COST_107", "DELAY_107", 99, 1.0, 1.0],
                  [45, "EDP.10", "FRAG.107", "COST_107", "DELAY_107", 99, 1.0, 1.0],
                  [46, "EDP.12", "FRAG.107", "COST_107", "DELAY_107", 99, 1.0, 1.0],
                  [47, "EDP.14", "FRAG.107", "COST_107", "DELAY_107", 99, 1.0, 1.0],
                  [48, "EDP.16", "FRAG.107", "COST_107", "DELAY_107", 99, 1.0, 1.0],
                  [49, "EDP.18", "FRAG.107", "COST_107", "DELAY_107", 99, 1.0, 1.0],
                  [50, "EDP.20", "FRAG.107", "COST_107", "DELAY_107", 99, 1.0, 1.0],
                  [51, "EDP.03", "FRAG.203", "COST_203", "DELAY_203", 693, 1.0, 1.0],
                  [52, "EDP.05", "FRAG.203", "COST_203", "DELAY_203", 693, 1.0, 1.0],
                  [53, "EDP.07", "FRAG.203", "COST_203", "DELAY_203", 693, 1.0, 1.0],
                  [54, "EDP.09", "FRAG.203", "COST_203", "DELAY_203", 693, 1.0, 1.0],
                  [55, "EDP.11", "FRAG.203", "COST_203", "DELAY_203", 693, 1.0, 1.0],
                  [56, "EDP.13", "FRAG.203", "COST_203", "DELAY_203", 693, 1.0, 1.0],
                  [57, "EDP.15", "FRAG.203", "COST_203", "DELAY_203", 693, 1.0, 1.0],
                  [58, "EDP.17", "FRAG.203", "COST_203", "DELAY_203", 693, 1.0, 1.0],
                  [59, "EDP.19", "FRAG.203", "COST_203", "DELAY_203", 693, 1.0, 1.0],
                  [60, "EDP.21", "FRAG.203", "COST_203", "DELAY_203", 693, 1.0, 1.0],
                  [61, "EDP.03", "FRAG.211", "COST_211", "DELAY_211", 23, 1.0, 1.0],
                  [62, "EDP.05", "FRAG.211", "COST_211", "DELAY_211", 23, 1.0, 1.0],
                  [63, "EDP.07", "FRAG.211", "COST_211", "DELAY_211", 23, 1.0, 1.0],
                  [64, "EDP.09", "FRAG.211", "COST_211", "DELAY_211", 23, 1.0, 1.0],
                  [65, "EDP.11", "FRAG.211", "COST_211", "DELAY_211", 23, 1.0, 1.0],
                  [66, "EDP.13", "FRAG.211", "COST_211", "DELAY_211", 23, 1.0, 1.0],
                  [67, "EDP.15", "FRAG.211", "COST_211", "DELAY_211", 23, 1.0, 1.0],
                  [68, "EDP.17", "FRAG.211", "COST_211", "DELAY_211", 23, 1.0, 1.0],
                  [69, "EDP.19", "FRAG.211", "COST_211", "DELAY_211", 23, 1.0, 1.0],
                  [70, "EDP.21", "FRAG.211", "COST_211", "DELAY_211", 23, 1.0, 1.0],
                  [71, "EDP.01", "FRAG.208", "COST_208", "DELAY_208", 53, 1.0, 1.0],
                  [72, "EDP.03", "FRAG.208", "COST_208", "DELAY_208", 53, 1.0, 1.0],
                  [73, "EDP.05", "FRAG.208", "COST_208", "DELAY_208", 53, 1.0, 1.0],
                  [74, "EDP.07", "FRAG.208", "COST_208", "DELAY_208", 53, 1.0, 1.0],
                  [75, "EDP.09", "FRAG.208", "COST_208", "DELAY_208", 53, 1.0, 1.0],
                  [76, "EDP.11", "FRAG.208", "COST_208", "DELAY_208", 53, 1.0, 1.0],
                  [77, "EDP.13", "FRAG.208", "COST_208", "DELAY_208", 53, 1.0, 1.0],
                  [78, "EDP.15", "FRAG.208", "COST_208", "DELAY_208", 53, 1.0, 1.0],
                  [79, "EDP.17", "FRAG.208", "COST_208", "DELAY_208", 53, 1.0, 1.0],
                  [80, "EDP.19", "FRAG.208", "COST_208", "DELAY_208", 53, 1.0, 1.0],
                  [81, "EDP.05", "FRAG.209", "COST_209", "DELAY_209", 16, 1.0, 1.0],
                  [82, "EDP.11", "FRAG.209", "COST_209", "DELAY_209", 16, 1.0, 1.0],
                  [83, "EDP.19", "FRAG.209", "COST_209", "DELAY_209", 16, 1.0, 1.0],
                  [84, "EDP.21", "FRAG.205", "COST_205", "DELAY_205", 4, 1.0, 1.0],
                  [85, "EDP.01", "FRAG.204", "COST_204", "DELAY_204", 2, 1.0, 1.0],
                  [86, "EDP.02", "FRAG.106", "COST_106", "DELAY_106", 721, 1.0, 1.0],
                  [87, "EDP.04", "FRAG.106", "COST_106", "DELAY_106", 721, 1.0, 1.0],
                  [88, "EDP.06", "FRAG.106", "COST_106", "DELAY_106", 721, 1.0, 1.0],
                  [89, "EDP.08", "FRAG.106", "COST_106", "DELAY_106", 721, 1.0, 1.0],
                  [90, "EDP.10", "FRAG.106", "COST_106", "DELAY_106", 721, 1.0, 1.0],
                  [91, "EDP.12", "FRAG.106", "COST_106", "DELAY_106", 721, 1.0, 1.0],
                  [92, "EDP.14", "FRAG.106", "COST_106", "DELAY_106", 721, 1.0, 1.0],
                  [93, "EDP.16", "FRAG.106", "COST_106", "DELAY_106", 721, 1.0, 1.0],
                  [94, "EDP.18", "FRAG.106", "COST_106", "DELAY_106", 721, 1.0, 1.0],
                  [95, "EDP.20", "FRAG.106", "COST_106", "DELAY_106", 721, 1.0, 1.0],
                  [96, "EDP.02", "FRAG.108", "COST_108", "DELAY_108", 10, 1.0, 1.0],
                  [97, "EDP.04", "FRAG.108", "COST_108", "DELAY_108", 10, 1.0, 1.0],
                  [98, "EDP.06", "FRAG.108", "COST_108", "DELAY_108", 10, 1.0, 1.0],
                  [99, "EDP.08", "FRAG.108", "COST_108", "DELAY_108", 10, 1.0, 1.0],
                  [100, "EDP.10", "FRAG.108", "COST_108", "DELAY_108", 10, 1.0, 1.0],
                  [101, "EDP.12", "FRAG.108", "COST_108", "DELAY_108", 10, 1.0, 1.0],
                  [102, "EDP.14", "FRAG.108", "COST_108", "DELAY_108", 10, 1.0, 1.0],
                  [103, "EDP.16", "FRAG.108", "COST_108", "DELAY_108", 10, 1.0, 1.0],
                  [104, "EDP.18", "FRAG.108", "COST_108", "DELAY_108", 10, 1.0, 1.0],
                  [105, "EDP.20", "FRAG.108", "COST_108", "DELAY_108", 10, 1.0, 1.0],
                  [106, "EDP.01", "FRAG.214", "COST_214", "DELAY_214", 10, 1.0, 1.0],
                  [107, "EDP.03", "FRAG.214", "COST_214", "DELAY_214", 10, 1.0, 1.0],
                  [108, "EDP.05", "FRAG.214", "COST_214", "DELAY_214", 10, 1.0, 1.0],
                  [109, "EDP.07", "FRAG.214", "COST_214", "DELAY_214", 10, 1.0, 1.0],
                  [110, "EDP.09", "FRAG.214", "COST_214", "DELAY_214", 10, 1.0, 1.0],
                  [111, "EDP.11", "FRAG.214", "COST_214", "DELAY_214", 10, 1.0, 1.0],
                  [112, "EDP.13", "FRAG.214", "COST_214", "DELAY_214", 10, 1.0, 1.0],
                  [113, "EDP.15", "FRAG.214", "COST_214", "DELAY_214", 10, 3.5, 1.0],
                  [114, "EDP.17", "FRAG.214", "COST_214", "DELAY_214", 10, 1.0, 2.5],
                  [115, "EDP.19", "FRAG.214", "COST_214", "DELAY_214", 10, 2.78, 3.14]]

building1 = pyslat.structure("building1")
building2 = pyslat.structure("building2")
building3 = pyslat.structure("building3")

for comp in COMPONENT_DATA:
    id = comp[0]
    edp = comp[1]
    frag = comp[2]
    cost = comp[3]
    delay = comp[4]
    count = comp[5]
    cost_adj = comp[6]
    delay_adj = comp[7]

    for variant in [1, 2, 3]:
        
        cg = pyslat.compgroup(
            "COMPGROUP_{:>03}".format(id),
            pyslat.edp.lookup("{}.{}".format(edp, variant)),
            pyslat.fragfn.lookup(frag),
            pyslat.lossfn.lookup(cost),
            pyslat.lossfn.lookup(delay),
            count,
            cost_adj,
            delay_adj)
        if variant == 1:
            print(id, edp, frag, cost, count, cost_adj, delay_adj)

        if variant == 1:
            building1.AddCompGroup(cg)
        if variant == 2:
            building2.AddCompGroup(cg)
        if variant == 3:
            building3.AddCompGroup(cg)


pyslat.MakeRecorder("PDF_REC-1",
                "pdf", 
                pyslat.structure.lookup("building1"),
                {'filename': ResultsFile("pdf-1"), 'append': False},
                None, 
                pyslat.linrange(0.00, 1.5, 200))
pyslat.MakeRecorder("PDF_REC-2",
                "pdf", 
                pyslat.structure.lookup("building2"),
                {'filename': ResultsFile("pdf-2"), 'append': False},
                None, 
                pyslat.linrange(0.00, 1.5, 200))
pyslat.MakeRecorder("PDF_REC-3",
                "pdf", 
                pyslat.structure.lookup("building3"),
                {'filename': ResultsFile("pdf-3"), 'append': False},
                None, 
                pyslat.linrange(0.00, 1.5, 200))

pyslat.MakeRecorder("NORM_PDF_REC-1",
                "pdf", 
                pyslat.structure.lookup("building1"),
                {'filename': ResultsFile("norm_pdf-1"), 'append': False, 'normalise': True},
                None, 
                pyslat.linrange(0.0, 1.5, 200))
pyslat.MakeRecorder("NORM_PDF_REC-2",
                "pdf", 
                pyslat.structure.lookup("building2"),
                {'filename': ResultsFile("norm_pdf-2"), 'append': False, 'normalise': True},
                None, 
                pyslat.linrange(0.0, 1.5, 200))
pyslat.MakeRecorder("NORM_PDF_REC-3",
                "pdf", 
                pyslat.structure.lookup("building3"),
                {'filename': ResultsFile("norm_pdf-3"), 'append': False, 'normalise': True},
                None, 
                pyslat.linrange(0.0, 1.5, 200))

pyslat.MakeRecorder("anncost-1", 
                "structcost", 
                pyslat.structure.lookup("building1"),
                {'filename': ResultsFile("ann_cost-1"), 'append': False, 'structcost-type': 'annual'},
                ['mean_x'], 
                None)

pyslat.MakeRecorder("anncost-2", 
                "structcost", 
                pyslat.structure.lookup("building2"),
                {'filename': ResultsFile("ann_cost-2"), 'append': False, 'structcost-type': 'annual'},
                ['mean_x'], 
                None)

pyslat.MakeRecorder("anncost-3", 
                "structcost", 
                pyslat.structure.lookup("building3"),
                {'filename': ResultsFile("ann_cost-3"), 'append': False, 'structcost-type': 'annual'},
                ['mean_x'],
                None)

linvalues = pyslat.linrange(0.01, 3.0, 199)
pyslat.MakeRecorder("TOTALCOST_COMP_REC-1", 
                "totalcost", 
                pyslat.structure.lookup("building1"),
                {'filename': ResultsFile("total_cost-1"), 'append': False},
                ['mean_x'], 
                linvalues)
pyslat.MakeRecorder("TOTALCOST_COMP_REC-2", 
                "totalcost", 
                pyslat.structure.lookup("building2"),
                {'filename': ResultsFile("total_cost-2"), 'append': False},
                ['mean_x'], 
                linvalues)
pyslat.MakeRecorder("TOTALCOST_COMP_REC-3", 
                "totalcost", 
                pyslat.structure.lookup("building3"),
                {'filename': ResultsFile("total_cost-3"), 'append': False},
                ['mean_x'], 
                linvalues)

pyslat.Set_Integration_Tolerance(1.0E-3)
pyslat.Set_Integration_Eval_Limit(2048)
pyslat.Set_Integration_Search_Limit(2048)
pyslat.Set_Integration_Method(pyslat.INTEGRATION_METHOD.DIRECTED)


for r in pyslat.recorder.all():
    r.run()

