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


class lognormaldist:
    def __init__(self, dist):
        self._dist = dist

    def mean(self):
        return self._dist.get_mean_X()

    def median(self):
        return self._dist.get_median_X()

    def mean_ln(self):
        return self._dist.get_mu_lnX()

    def sd(self):
        return self._dist.get_sigma_X();
    
    def sd_ln(self):
        return self._dist.get_sigma_lnX();

    def __str__(self):
        return("Log Normal Distribution: mean: {}, sd_ln_x: {}.".format(
            self.mean(), self.sd_ln()))


class detfn:
    def __init__(self, id, type, parameters):
        if type == 'power law':
            fntype = FUNCTION_TYPE.PLC
        elif type == 'hyperbolic':
            fntype = FUNCTION_TYPE.NLH
        elif type == 'loglog':
            fntype = FUNCTION_TYPE.LOGLOG
        elif type == 'linear':
            fntype = FUNCTION_TYPE.LIN
        else:
            raise ValueError("Invalid detfn type: {}".format(type))
            
        self._id = id
        self._type = type
        self._parameters = parameters.copy()
        self._func = factory(fntype, parameters)

    def id(self):
        return self._id

    def function(self):
        return self._func

    def ValueAt(self, x):
        return self._func.ValueAt(x)
    
    def __str__(self):
        return("Deterministic Function '{}', of type '{}', from parameters {}.".format(self._id, self._type, self._parameters))



class probfn:
    def __init__(self, id, type, mu_func, sigma_func):
        self._id = id
        self._type = type
        self._mu_func = mu_func
        self._sigma_func = sigma_func
        self._func = MakeLogNormalProbabilisticFn({mu_func[0]: mu_func[1].function(),
                                                   sigma_func[0]: sigma_func[1].function()})
            
    def id(self):
        return self._id

    def function(self):
        return self._func
    
    def Mean(self, x):
        return self._func.Mean(x)

    def MeanLn(self, x):
        return self._func.MeanLn(x)

    def Median(self, x):
        return self._func.Median(x)

    def SD_ln(self, x):
        return self._func.SD_ln(x)

    def SD(self, x):
        return self._func.SD(x)

    def X_at_exceedence(self, x, y):
        return self._func.X_at_exceedence(x, y)

    def __str__(self):
        return(("Probabilistic function '{}' of type {}, using the deterministic function {} for {}, and " +
               "the deterministic function {} for {}.").format(
                   self._id, self._type,
                   self._mu_func[1].id(), self._mu_func[0],
                   self._sigma_func[1].id(), self._sigma_func[0]))

class collapse:
    def __init__(self, id, mu, sd):
        self._id = id
        self._mu = mu
        self._sd = sd
        self._func = MakeCollapse(mu, sd)
        self._collapse = None
        print(self)

    def func(self):
        return self._func

    def __str__(self):
        return("Collapse function '{}', mu={}, sd={}.".format(self._id, self._mu, self._sd))
        
class im:
    def __init__(self, id, detfn):
        self._id = id
        self._detfn = detfn
        self._func = MakeIM(detfn.function())

    def id(self):
        return self._id

    def function(self):
        return self._func

    def getlambda(self, x):
        return self._func.getlambda(x)

    def pCollapse(self, x):
        return self._func.pCollapse(x)

    def SetCollapse(self, collapse):
        self._collapse = collapse
        self._func.SetCollapse(collapse.func())

    def CollapseRate(self):
        return self._func.CollapseRate()

    def __str__(self):
        if self._collapse:
            return("Intensity measure '{}', based on the deterministic function '{}', with {}".format(self._id, self._detfn.id(), self._collapse))
        else:
            return("Intensity measure '{}', based on the deterministic function '{}'.".format(self._id, self._detfn.id()))

class edp:
    def __init__(self, id, im, fn):
        self._id = id
        self._im = im
        self._fn = fn
        self._func = MakeEDP(im.function(), fn.function())
            
    def id(self):
        return self._id

    def function(self):
        return self._func
    
    def Mean(self, x):
        return self._func.Mean(x)

    def MeanLn(self, x):
        return self._func.MeanLn(x)

    def Median(self, x):
        return self._func.Median(x)

    def SD_ln(self, x):
        return self._func.SD_ln(x)

    def SD(self, x):
        return self._func.SD(x)

    def getlambda(self, x):
        return self._func.getlambda(x)

    def X_at_exceedence(self, x, y):
        return self._fn.X_at_exceedence(x, y)

    def __str__(self):
        return(("Engineering Demand Parameter '{}' using the intensity measure '{}', and " +
                "the probabilistic function {}.").format(self._id, self._im.id(), self._fn.id()))

class fragfn:
    def __init__(self, id):
        self._id = id
        self.func = None
        
    def id(self):
        return(self._id)

    def function(self):
        return self._func

    def pExceeded(self, x):
        return self._func.pExceeded(x)

class fragfn_db(fragfn):
    def __init__(self, id, db_params):
        super().__init__(id)
        self._db_params = db_params

    def __str__(self):
        return("Fragility Function '{}', from database: {}.".format(self._id, self._db_params))

class fragfn_user(fragfn):
    def __init__(self, id, options, scalars):
        super().__init__(id)
        self._options = options
        self._scalars = scalars
        params = []
        for s in scalars:
            params.append({options['mu']: s[0], options['sd']: s[1]})
        self._func = MakeFragilityFn(params)

    def __str__(self):
        return("User-defined Fragility Function '{}', built from {} using options '{}'.".format(
            self._id,
            self._scalars,
            self._options))

    def size(self):
        return len(self._scalars)

class lossfn:
    def __init__(self, id, options, data):
        self._id = id
        self._options = options
        self._data = data

        params = []
        for d in data:
            params.append({options['mu']: d[0], options['sd']: d[1]})
        self._func = MakeLossFn(params)

    def id(self):
        return(self._id)

    def function(self):
        return self._func

    def __str__(self):
        return("Loss Function '{}', from {} as {}.".format(self._id, self._data, self._options))

class compgroup:
    def __init__(self, id, edp, frag, loss, count):
        self._id = id
        self._edp = edp
        self._frag = frag
        self._loss = loss
        self._count = count
        self._func = MakeCompGroup(edp.function(),
                                   frag.function(),
                                   loss.function(),
                                   count)
        
    def fragfn(self):
        return self._frag

    def size(self):
        return self._frag.size()
        
    def function(self):
        return self._func

    def E_Loss_EDP(self, x):
        return self._func.E_Loss_EDP(x)

    def SD_ln_Loss_EDP(self, x):
        return self._func.SD_ln_loss_EDP(x)

    def E_Loss_IM(self, x):
        return self._func.E_Loss_IM(x)

    def SD_ln_Loss_IM(self, x):
        return self._func.SD_ln_loss_IM(x)

    def E_loss(self, t, l):
        return self._func.E_loss(t, l)

    def E_annual_loss(self):
        return self._func.E_annual_loss()

    def lambda_loss(self, loss):
        return self._func.lambda_loss(loss)

    def id(self):
        return(self._id)
        
    def __str__(self):
        return("Component Group '{}' using {}, {}, and {} ({} component(s))".format(
            self._id, 
            self._edp.id(),
            self._frag.id(),
            self._loss.id(),
            self._count))

class structure:
    def __init__(self):
        super().__init__()
        self._structure = MakeStructure()

    def __str__(self):
        return "Structure"

    def AddCompGroup(self, group):
        self._structure.AddCompGroup(group.function())

    def setRebuildCost(self, cost):
        self._structure.setRebuildCost(cost)

    def getRebuildCost(self):
        return lognormaldist(self._structure.getRebuildCost())

    def Loss(self, im, consider_collapse):
        return lognormaldist(self._structure.Loss(im, consider_collapse))
                     
    
