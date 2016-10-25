import pyslatcore
import math
import numpy as np
from contextlib import redirect_stdout
import numbers
import sys



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
    NLH = pyslatcore.NLH
    PLC = pyslatcore.PLC
    LIN = pyslatcore.LIN
    LOGLOG = pyslatcore.LOGLOG

class LOGNORMAL_MU_TYPE:
    MEAN_X = pyslatcore.MEAN_X
    MEDIAN_X = pyslatcore.MEDIAN_X
    MEAN_LN_X = pyslatcore.MEAN_LN_X
    
class LOGNORMAL_SIGMA_TYPE:
    SD_X = pyslatcore.SD_X
    SD_LN_X = pyslatcore.SD_LN_X

def factory(t, params):
    return pyslatcore.factory(t, params)

def factory(t, param1, param2):
    return pyslatcore.factory(t, param1, param2)

def MakeLogNormalProbabilisticFn(mu_function, mu_type, sigma_function, sigma_type):
    return pyslatcore.MakeLogNormalProbabilisticFn(mu_function, mu_type, sigma_function, sigma_type)

def MakeIM(f):
    return pyslatcore.MakeIM(f)

def MakeEDP(base_rate, dependent_rate, name):
    return pyslatcore.MakeEDP(base_rate, dependent_rate, name)

def MakeFragilityFn(parameters):
    return pyslatcore.MakeFragilityFn(parameters)

def MakeLossFn(parameters):
    return pyslatcore.MakeLossFn(parameters)

def MakeCompGroup(edp, frag_fn, loss_fn, count, name):
    return pyslatcore.MakeCompGroup(edp, frag_fn, loss_fn, count, name)

def MakeStructure():
    return pyslatcore.MakeStructure()

def MakeLogNormalDist(mu, mu_type, sigma, sigma_type):
    return pyslatcore.MakeLogNormalDist(mu, mu_type, sigma, sigma_type)

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
    defs = dict()
    
    def __init__(self, id, type, parameters):
        #print("PARAMETERS: {}".format(parameters))
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
        #print("PARAMETERS: {}".format(self._parameters))
        if fntype == FUNCTION_TYPE.LOGLOG or fntype == FUNCTION_TYPE.LIN:
            self._func = factory(fntype, parameters[0], parameters[1])
        else:
            self._func = factory(fntype, parameters)

        if id != None:
            detfn.defs[id] = self

    def lookup(id):
        return detfn.defs.get(id)

    def id(self):
        return self._id

    def function(self):
        return self._func

    def ValueAt(self, x):
        return self._func.ValueAt(x)
    
    def __str__(self):
        return("Deterministic Function '{}', of type '{}', from parameters {}.".format(self._id, self._type, self._parameters))



class probfn:
    defs = dict()

    def lookup(id):
        return probfn.defs.get(id)
    
    def __init__(self, id, type, mu_func, sigma_func):
        self._id = id
        self._type = type
        self._mu_func = mu_func
        self._sigma_func = sigma_func
        self._func = MakeLogNormalProbabilisticFn(mu_func[1].function(), mu_func[0],
                                                  sigma_func[1].function(), sigma_func[0]) 
        if id != None:
            probfn.defs[id] = self
            
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

        
class im:
    defs = dict()
    
    def __init__(self, id, detfn):
        self._id = id
        self._detfn = detfn
        self._func = MakeIM(detfn.function())
        self._collapse = None
        self._demolition = None
        if id != None:
            im.defs[id] = self

    def lookup(id):
        return im.defs.get(id)

    def id(self):
        return self._id

    def function(self):
        return self._func

    def getlambda(self, x):
        return self._func.get_lambda(x)

    def pRepair(self, x):
        return self._func.pRepair(x)
    
    def pCollapse(self, x):
        return self._func.pCollapse(x)

    def SetCollapse(self, collapse):
        self._collapse = collapse
        self._func.SetCollapse(collapse)

    def CollapseRate(self):
        return self._func.CollapseRate()

    def pDemolition(self, x):
        return self._func.pDemolition(x)

    def SetDemolition(self, demolition):
        self._demolition = demolition
        self._func.SetDemolition(demolition)

    def DemolitionRate(self):
        return self._func.DemolitionRate()

    def __str__(self):
        if self._collapse:
            return("Intensity measure '{}', based on the deterministic function '{}', with {}".format(self._id, self._detfn, self._collapse))
        else:
            return("Intensity measure '{}', based on the deterministic function '{}'.".format(self._id, self._detfn.id()))

class edp:
    defs = dict()
    def __init__(self, id, im, fn):
        self._id = id
        self._im = im
        self._fn = fn
        self._func = MakeEDP(im.function(), fn.function(), id)
        if id != None:
            edp.defs[id] = self

    def lookup(id):
        if not isinstance(id, pyslatcore.EDP):
            return edp.defs.get(id)
        else:
            for e in edp.defs.values():
                if (e.function().AreSame(id)):
                    return e
            return None
            
    def id(self):
        return self._id

    def get_IM(self):
        return self._im

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
        return self._func.get_lambda(x)

    def X_at_exceedence(self, x, y):
        return self._fn.X_at_exceedence(x, y)

    def __str__(self):
        return(("Engineering Demand Parameter '{}' using the intensity measure '{}', and " +
                "the probabilistic function {}.").format(self._id, self._im.id(), self._fn.id()))

class fragfn:
    defs = dict()
    
    def __init__(self, id):
        self._id = id
        self.func = None
        if id != None:
            fragfn.defs[id] = self

    def lookup(id):
        if not isinstance(id, pyslatcore.FragilityFn):
            return fragfn.defs.get(id)
        else:
            for f in fragfn.defs.values():
                if (f.function().AreSame(id)):
                    return f
            return None
        
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
            params.append(MakeLogNormalDist(s[0], options['mu'], s[1], options['sd']))
        self._func = MakeFragilityFn(params)

    def __str__(self):
        return("User-defined Fragility Function '{}', built from {} using options '{}'.".format(
            self._id,
            self._scalars,
            self._options))

    def size(self):
        return len(self._scalars)

class lossfn:
    defs = dict()
    
    def __init__(self, id, options, data):
        self._id = id
        self._options = options
        self._data = data

        params = []
        for d in data:
            params.append(MakeLogNormalDist(d[0], options['mu'], d[1], options['sd']))
        self._func = MakeLossFn(params)
        if id != None:
            lossfn.defs[id] = self

    def lookup(id):
        return lossfn.defs.get(id)

    def id(self):
        return(self._id)

    def function(self):
        return self._func

    def __str__(self):
        return("Loss Function '{}', from {} as {}.".format(self._id, self._data, self._options))

class compgroup:
    defs = dict()
    
    def __init__(self, id, edp, frag, loss, count):
        print("> compgroup: {}, {}, {}, {}, {}".format(id, edp, frag, loss, count))
        self._id = id
        self._edp = edp
        self._frag = frag
        self._loss = loss
        self._count = count
        self._func = MakeCompGroup(edp.function(),
                                   frag.function(),
                                   loss.function(),
                                   count, id)
        if id != None:
            compgroup.defs[id] = self

    def lookup(id):
        if not isinstance(id, pyslatcore.CompGroup):
            return compgroup.defs.get(id)
        else:
            for c in compgroup.defs.values():
                if (c.function().AreSame(id)):
                    return c
            return None
    
    def get_IM(self):
        return self._edp.get_IM()

    def fragfn(self):
        return self._frag

    def size(self):
        return self._frag.size()
        
    def function(self):
        return self._func

    def E_Loss_EDP(self, x):
        return self._func.E_Cost_EDP(x)

    def SD_ln_Loss_EDP(self, x):
        return self._func.SD_ln_cost_EDP(x)

    def E_Loss_IM(self, x):
        return self._func.E_Cost_IM(x)

    def SD_ln_Loss_IM(self, x):
        return self._func.SD_ln_Cost_IM(x)

    def E_loss(self, t, l):
        return self._func.E_cost(t, l)

    def E_annual_loss(self):
        return self._func.E_annual_cost()

    def lambda_loss(self, loss):
        return self._func.lambda_cost(loss)

    def pDS_IM(self, im):
        return self._func.pDS_IM(im)

    def Rate(self):
        return self._func.Rate()

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
    defs = dict()
    
    def __init__(self, id):
        super().__init__()
        self._structure = MakeStructure()
        self._im = None
        if id != None:
            structure.defs[id] = self

    def lookup(id):
        return structure.defs.get(id)

    def __str__(self):
        return "Structure"

    def get_IM(self):
        return self._im

    def AddCompGroup(self, group):
        self._structure.AddCompGroup(group.function())
        if self._im == None:
            self._im = group.get_IM()

    def setRebuildCost(self, cost):
        self._structure.setRebuildCost(cost)

    def getRebuildCost(self):
        return lognormaldist(self._structure.getRebuildCost())

    def setDemolitionCost(self, cost):
        self._structure.setDemolitionCost(cost)

    def getDemolitionCost(self):
        return lognormaldist(self._structure.getDemolitionCost())

    def Loss(self, im, consider_collapse):
        return lognormaldist(self._structure.Cost(im, consider_collapse))

    def TotalLoss(self, im):
        return lognormaldist(self._structure.TotalCost(im))

    def LossesByFate(self, im):
        result = self._structure.CostsByFate(im)
        return [lognormaldist(result[0]),
                lognormaldist(result[1]),
                lognormaldist(result[2])]

    def AnnualLoss(self):
        return lognormaldist(self._structure.AnnualCost())

    def DeaggregatedLoss(self, im):
        return self._structure.DeaggregatedCost(im)

    def ComponentsByEDP(self):
        return self._structure.ComponentsByEDP()

    def ComponentsByFragility(self):
        return self._structure.ComponentsByFragility()
    
class recorder:
    defs = dict()
    
    def __init__(self, id, type, function, options, columns, at):
        super().__init__()

        self._type = type
        self._function = function
        self._options = options
        self._at = at
        if at != None:
            self._at = list(at)

        if not type == 'dsrate' and not type == 'collrate' \
           and not type == 'structloss' \
           and at==None:
            raise ValueError('MUST PROVIDE ''AT'' CLAUSE')

        if (type =='dsedp' or type == 'dsim') and columns == None:
            columns = []
            for i in range(function.size()):
                columns.append("DS{}".format(i + 1))
            columns.append("E(DS)")
        elif (type == 'probfn' or type == 'edpim') and columns == None:
            columns = ['mean_ln_x', 'sd_ln_x']
        elif (type == 'lossedp' or type =='lossim' \
              or type == 'totalloss') \
             and columns == None:
            columns = ['mean_x', 'sd_ln_x']
        self._columns = columns
        if id != None:
            recorder.defs[id] = self
            
        print(self)

    def lookup(id):
        return recorder.defs.get(id)

    def all():
        return recorder.defs.values()

    def __str__(self):
        if self._at != None:
            values = list(self._at)
            if len(values) > 4:
                atprint = "[{}, {}, ... {}]".format(values[0], values[1], values[-1])
            else:
                atprint = values
        else:
            atprint = "(no values)"

        return "Recorder: {} {} {} {} {}".format(self._type, self._function, self._options, self._columns, atprint)

    def generate_output(self):
        #print(self)
        if self._type == 'dsrate':
            rates = self._function.Rate()
            line1 = ""
            line2 = ""
            for i in range(len(rates)):
                 label = "DS{}".format(i + 1)
                 line1 = "{}{:>20}".format(line1, label)
                 line2 = "{}{:>20.6}".format(line2, rates[i])
            line1 = "{}{:>20}".format(line1, "E(DS)")
            line2 = "{}{:>20.6}".format(line2, sum(rates))
            print(line1)
            print(line2)
        else:
            labels = {'detfn': ['x', 'y'],
                      'probfn': ['x', None],
                      'imrate': ['IM', 'lambda'],
                      'edpim': ['IM', None],
                      'edprate': ['EDP', 'lambda'],
                      'dsedp': ['EDP', None],
                      'dsim': ['IM', None],
                      'lossds': ['DS', None],
                      'lossedp': ['EDP', None],
                      'lossim': ['IM', None],
                      'annloss': ['t', ["E[ALt]"]],
                      'lossrate': ['t', 'Rate'],
                      'totalloss': ['IM', None],
                      'collapse': ['IM', ['p(Demolition)', 'p(Collapse)']],
                      'deagg': ['IM', ['mean_nc', 'sd_nc', 'mean_c', 'sd_c']]}
        
            x_label = labels[self._type][0]
            if x_label == 'IM':
                if isinstance(self._function, im):
                    x_label = self._function.id()
                else:
                    x_label = self._function.get_IM().id()
            y_label = labels[self._type][1]

            if y_label:
                if not isinstance(y_label, list):
                    y_label = [y_label];
            else:
                y_label = self._columns
                      
            line = "{:>15}".format(x_label)
            for y in y_label:
                    line = "{}{:>15}".format(line, y)
            print(line)

            for x in self._at:
                line = "{:>15.6}".format(x)
                if self._type == 'dsedp':
                    yvals = self._function.fragfn().pExceeded(x)
                    for y in yvals:
                        line = "{}{:>15.6}".format(line, y)
                    line = "{}{:>15.6}".format(line, sum(yvals))
                elif self._type == 'dsim':
                    yvals = self._function.pDS_IM(x)
                    for y in yvals:
                        line = "{}{:>15.6}".format(line, y)
                    line = "{}{:>15.6}".format(line, sum(yvals))
                elif self._type == 'annloss':
                    annual_loss = self._function.E_loss(int(x), self._options['lambda'])
                    line = "{}{:>15.6}".format(line, annual_loss)
                elif self._type == 'lossrate':
                    loss_rate = self._function.lambda_loss(x)
                    line = "{}{:>15.6}".format(line, loss_rate)
                elif self._type == 'collapse':
                    p = self._function.pDemolition(x)
                    line = "{}{:>15.6}".format(line, p)
                    p = self._function.pCollapse(x)
                    line = "{}{:>15.6}".format(line, p)
                elif self._type == 'deagg':
                    values = self._function.DeaggregatedLoss(x)
                    nc = values[0]
                    c = values[1]
                    line = "{}{:>15.6}{:>15.6}{:>15.6}{:>15.6}".format(
                        x, 
                        nc.get_mean_X(), nc.get_sigma_lnX(), 
                        c.get_mean_X(), c.get_sigma_lnX())
                elif not self._columns == None:
                    line = "{:>15.6}".format(x)
                    for y in self._columns:
                        if isinstance(y, numbers.Number):
                            if isinstance(self._function, probfn):
                                yval = self._function.function().X_at_exceedence(x, y)
                            elif isinstance(self._function, edp):
                                yval = self._function.X_at_exceedence(x, y)
                            else:
                                yval = "----"
                        elif y == 'mean_x':
                            if self._type == 'lossedp':
                                yval = self._function.E_Loss_EDP(x)
                            elif self._type == 'lossim':
                                yval = self._function.E_Loss_IM(x)
                            elif self._type == 'totalloss':
                                yval = self._function.TotalLoss(x).mean()
                            else:
                                yval = self._function.Mean(x)
                        elif y == 'mean_ln_x':
                            if self._type == 'totalloss':
                                yval = self._function.TotalLoss(x).mean_ln()
                            else:
                                yval = self._function.MeanLn(x)
                        elif y == 'median_x':
                            if self._type == 'totalloss':
                                yval = self._function.TotalLoss(x).median()
                            else:
                                yval = self._function.Median(x)
                        elif y == 'sd_ln_x':
                            if self._type == 'lossedp':
                                yval = self._function.SD_ln_Loss_EDP(x)
                            elif self._type == 'lossim':
                                yval = self._function.SD_ln_Loss_IM(x)
                            elif self._type == 'totalloss':
                                yval = self._function.TotalLoss(x).sd_ln()
                            else:
                                yval = self._function.SD_ln(x)
                        elif y == 'sd_x':
                            if self._type == 'totalloss':
                                yval = self._function.TotalLoss(x).sd()
                            else:
                                yval = self._function.SD(x)
                        else:
                            yval = "+++++++++"
                        line = "{}{:>15.6}".format(line, yval)
                else:
                    if isinstance(self._function, detfn):
                        yval = self._function.ValueAt(x)
                    elif (isinstance(self._function, im) or
                          isinstance(self._function, edp) or
                          isinstance(self._function, IM) or
                          isinstance(self._function, EDP)):
                          yval = self._function.getlambda(x)
                    elif isinstance(self._function, compgroup):
                          yval = self._function.E_Loss_EDP(x)
                    else:
                        yval = "*****"
                    line = "{}{:>15.6}".format(line, yval)
                print(line)
                
            
    def run(self):
        #print("RUN {}".format(self))
        destination = self._options.get('filename')
        if destination != None:
            if self._options.get('append'):
                f = open(destination, "a")
            else:
                f = open(destination, "w")
            with redirect_stdout(f):
                self.generate_output()

        else:
            self.generate_output()

class CollRateRecorder(recorder):
    def __init__(self, id, type, function, options, columns, at):
        super().__init__(id, type, function, options, columns, at)

    def generate_output(self):
        print("{:>15}{:>30}{:>30}".format("IM", "rate(Demolition)", "rate(Collapse)"))
        print("{:>15}{:>30}{:>30}".format(self._function.id(), self._function.DemolitionRate(),
                                          self._function.CollapseRate()))

class StructLossRecorder(recorder):
    def __init__(self, id, type, function, options, columns, at):
        if  (not options['structloss-type'] == 'annual') \
            and at==None:
            raise ValueError('MUST PROVIDE ''AT'' CLAUSE')

        if columns == None or len(columns) == 0:
            columns = ['mean_x']
            
        super().__init__(id, type, function, options, columns, at)


    def generate_output(self):
        if self._options['structloss-type'] == 'annual':
            line1 = ""
            line2 = ""
            annual_loss = self._function.AnnualLoss()
            for y_label in self._columns:
                line1 = "{}{:>15}".format(line1, y_label)
                if y_label=='mean_x':
                    value = annual_loss.mean()
                elif y_label=='mean_ln_x':
                    value = annual_loss.mean_ln()
                elif y_label=='median_x':
                    value = annual_loss.median()
                elif y_label=='sd_ln_x':
                    value = annual_loss.sd_ln()
                elif y_label=='sd_x':
                    value = annual_loss.sd()
                line2 = "{}{:>15.6}".format(line2, value)
            print(line1)
            print(line2)
        elif self._options['structloss-type'] == 'by-edp':
            components = self._function.ComponentsByEDP();
            groups = dict()
            for c in components:
                groups[edp.lookup(c[0].get_EDP()).id()] = c

            x_label = self._function.get_IM().id()
            y_label = self._columns
            categories = list(groups.keys())
            categories.sort()

            # Print column headers:
            line = "{:>15}".format(x_label)
            for c in categories:
                for y in y_label:
                    line = "{}{:>15}".format(line, "{}.{}".format(c, y))
            print(line)

            for x in self._at:
                line = "{:>15.6}".format(x)
                for c in categories:
                    dists = []
                    for cg in groups[c]:
                        mean = cg.E_Cost_IM(x)
                        sd_ln = cg.SD_ln_Cost_IM(x)

                        dists.append(pyslatcore.MakeLogNormalDist(mean, LOGNORMAL_MU_TYPE.MEAN_X,
                                                                  sd_ln, LOGNORMAL_SIGMA_TYPE.SD_LN_X))
                    dist = lognormaldist(pyslatcore.AddDistributions(dists))
                    
                    for y in self._columns:
                        if y == 'mean_x':
                            yval = dist.mean()
                        elif y== 'mean_ln':
                            yval = dist.mean_ln()
                        elif y== 'median':
                            yval = dist.median()
                        elif y == 'sd_ln_x':
                            yval = dist.sd_ln()
                        elif y == 'sd_x':
                            yval = dist.sd()
                        else:
                            yval = None
                        
                        line = "{}{:>15.6}".format(line, yval)
                print(line)
            
                
        elif self._options['structloss-type'] == 'by-frag':
            components = self._function.ComponentsByFragility();
            groups = dict()
            for c in components:
                groups[compgroup.lookup(c[0]).fragfn().id()] = c

            x_label = self._function.get_IM().id()
            y_label = self._columns
            categories = list(groups.keys())
            categories.sort()
            
            # Print column headers:
            line = "{:>15}".format(x_label)
            for c in categories:
                for y in y_label:
                    line = "{}{:>20}".format(line, "{}.{}".format(c, y))
            print(line)

            for x in self._at:
                line = "{:>15.6}".format(x)
                for c in categories:
                    dists = []
                    for cg in groups[c]:
                        mean = cg.E_Cost_IM(x)
                        sd_ln = cg.SD_ln_Cost_IM(x)

                        dists.append(pyslatcore.MakeLogNormalDist(mean, LOGNORMAL_MU_TYPE.MEAN_X,
                                                                  sd_ln, LOGNORMAL_SIGMA_TYPE.SD_LN_X))
                    dist = lognormaldist(pyslatcore.AddDistributions(dists))
                    
                    for y in self._columns:
                        if y == 'mean_x':
                            yval = dist.mean()
                        elif y== 'mean_ln':
                            yval = dist.mean_ln()
                        elif y== 'median':
                            yval = dist.median()
                        elif y == 'sd_ln_x':
                            yval = dist.sd_ln()
                        elif y == 'sd_x':
                            yval = dist.sd()
                        else:
                            yval = None
                        
                        line = "{}{:>20.6}".format(line, yval)
                print(line)
                

        elif self._options['structloss-type'] == 'by-fate':
            x_label = self._function.get_IM().id()
            y_label = self._columns
            fates = ['repair', 'demo', 'coll']

            line = "{:>15}".format(x_label)
            for f in fates:
                for y in y_label:
                    line = "{}{:>15}".format(line, "{}.{}".format(f, y))
            print(line)

            for x in self._at:
                line = "{:>15.6}".format(x)
                for f in fates:
                    if f == 'repair':
                        dist = self._function.LossesByFate(x)[0]
                    elif f == 'demo':
                        dist = self._function.LossesByFate(x)[1]
                    elif f == 'coll':
                        dist = self._function.LossesByFate(x)[2]
                    else:
                        dist = None

                    for y in self._columns:
                        if y == 'mean_x':
                            yval = dist.mean()
                        elif y== 'mean_ln':
                            yval = dist.mean_ln()
                        elif y== 'median':
                            yval = dist.median()
                        elif y == 'sd_ln_x':
                            yval = dist.sd_ln()
                        elif y == 'sd_x':
                            yval = dist.sd()
                        else:
                            yval = None
                        
                        line = "{}{:>15.6}".format(line, yval)
                print(line)
                        
def MakeRecorder(id, type, function, options, columns, at):
    if (type == 'collrate'):
        return CollRateRecorder(id, type, function, options, columns, at)
    elif (type == 'structloss'):
        return StructLossRecorder(id, type, function, options, columns, at)
    else:
        return recorder(id, type, function, options, columns, at)
    

def ImportProbFn(id, filename):
    data = np.loadtxt(filename, skiprows=2)
    # Add a point at 0, 0 to support interpolation down to zero
    x = [0]
    mu = [0]
    sigma = [0]
    C = [0]
    for d in data:
        x.append(d[0])
        values = []
        collapse = 0
        for y in d[1:]:
            if y==0:
                collapse = collapse + 1
            else:
                values.append(y)
        mu.append(np.mean(values))
        sigma.append(np.std(values, ddof=1))
        C.append(collapse / (len(d) - 1))
        # Add points beyond the data provided to support interpolation beyond
        # the given data, up to a point:
        #N = len(x)
        #x.append(x[N - 1] * 1.5)
        #mu.append(mu[N - 2] + (mu[N-1] - mu[N-2]) * (x[N] - x[N-1]) / (x[N-1] - x[N-2]))
        #sigma.append(sigma[N - 2] + (sigma[N-1] - sigma[N-2]) * (x[N] - x[N-1]) / (x[N-1] - x[N-2]))
    mu_func = detfn(None, 'linear', [x.copy(), mu.copy()])
    sigma_func = detfn(None, 'linear', [x.copy(), sigma.copy()])
    return(probfn(id, 'lognormal', 
                  [LOGNORMAL_MU_TYPE.MEAN_X, mu_func],
                  [LOGNORMAL_SIGMA_TYPE.SD_X,  sigma_func]))

def ImportIMFn(id, filename):
    data = np.loadtxt(filename, skiprows=2)
    x = []
    y = []
    for d in data:
        x.append(d[0])
        y.append(d[1])
    im_func = detfn(None, 'loglog', [x.copy(), y.copy()])
    return(im(id, im_func))
