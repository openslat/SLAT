import pyslatcore
import math
import numpy as np

def frange(start, stop, step):
    i = start
    while i <= stop + step/2:
        yield i
        i += step

def logrange(start, end, count):
    return(list(map(math.exp, 
             math.log(start) + np.arange(0, count, 1)
             * math.log(end/start)/(count - 1))))

def linrange(start, end, count):
    return(list(frange(start, end, (end - start)/(count - 1))))



class FUNCTION_TYPE:
    NLH = pyslatcore.FUNCTION_TYPE.NLH
    PLC = pyslatcore.FUNCTION_TYPE.PLC
    LIN = pyslatcore.FUNCTION_TYPE.LIN
    LOGLOG = pyslatcore.FUNCTION_TYPE.LOGLOG

class LOGNORMAL_PARAM_TYPE:
    MEAN_X = pyslatcore.LOGNORMAL_PARAM_TYPE.MEAN_X
    MEDIAN_X = pyslatcore.LOGNORMAL_PARAM_TYPE.MEDIAN_X
    MEAN_LN_X = pyslatcore.LOGNORMAL_PARAM_TYPE.MEAN_LN_X
    SD_X = pyslatcore.LOGNORMAL_PARAM_TYPE.SD_X
    SD_LN_X = pyslatcore.LOGNORMAL_PARAM_TYPE.SD_LN_X

def factory(t, params):
    return pyslatcore.factory(t, params)

def MakeLogNormalProbabilisticFn(parameters):
    return pyslatcore.MakeLogNormalProbabilisticFn(parameters)

def MakeIM(f):
    return pyslatcore.MakeIM(f)

def MakeCollapse(mu, sd):
    return pyslatcore.MakeCollapse(mu, sd)

def MakeEDP(base_rate, dependent_rate):
    return pyslatcore.MakeEDP(base_rate, dependent_rate)

def MakeFragilityFn(parameters):
    return pyslatcore.MakeFragilityFn(parameters)

def MakeLossFn(parameters):
    return pyslatcore.MakeLossFn(parameters)

def MakeCompGroup(edp, frag_fn, loss_fn, count):
    return pyslatcore.MakeCompGroup(edp, frag_fn, loss_fn, count)

def MakeStructure():
    return pyslatcore.MakeStructure()

def MakeLogNormalDist(parameters):
    return pyslatcore.MakeLogNormalDist(parameters)

def IntegrationSettings(tolerance, max_evals):
    return pyslatcore.IntegrationSettings(tolerance, max_evals)







