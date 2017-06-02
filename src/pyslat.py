##@file
# Python interface to the C++ libslat library.

##@package pyslat
#@brief libslat interface.
#
#This module provides access to libslat from Python.

import pyslatcore
import math
import numpy as np
from contextlib import redirect_stdout
import numbers
import sys
import os.path

pyslatcore.Initialise()

_DEFAULT_INPUT_EXTENSION = ".csv"
_DEFAULT_INPUT_DIRECTORY = "."
_Input_directory = None

def set_input_directory(path):
    global _Input_directory
    _Input_directory = path

def full_input_path(path):
    if not os.path.dirname(path):
        path = os.path.join(_Input_directory or _DEFAULT_INPUT_DIRECTORY, path)

    if not os.path.splitext(path)[1]:
        return path + _DEFAULT_INPUT_EXTENSION
    else:
        return path

_DEFAULT_OUTPUT_EXTENSION = ".csv"
_DEFAULT_OUTPUT_DIRECTORY = "."
_Output_directory = None

def set_output_directory(path):
    global _Output_directory
    _Output_directory = path
    if not os.path.exists(_Output_directory):
        os.makedirs(_Output_directory)    

def full_output_path(path):
    if not os.path.dirname(path):
        path = os.path.join(_Output_directory or _DEFAULT_OUTPUT_DIRECTORY, path)

    if not os.path.splitext(path)[1]:
        return path + _DEFAULT_OUTPUT_EXTENSION
    else:
        return path
    
## Create a generate for a range of values.
# @param start The initial value.
# @param stop The ending value.
# @param step The increment.
def frange(start, stop, step):
    i = start
    while i <= stop + step/2:
        yield i
        i += step

## Create a list of values, spaced logarithmicly.
# @param start The initial value (*not* the log of the initial value).
# @param stop The ending value.
# @param step The increment.
def logrange(start, end, count):
    return(list(map(math.exp, 
             math.log(start) + np.arange(0, count, 1)
             * math.log(end/start)/(count - 1))))

## Create a list of values, spaced linearly.
# @param start The initial value.
# @param stop The ending value.
# @param step The increment.
def linrange(start, end, count):
    return(list(frange(start, end, (end - start)/(count - 1))))


## Deterministic function types
class FUNCTION_TYPE:
    ## Non-linear hyperbolic function
    NLH = pyslatcore.NLH

    ## Power-law curve function
    PLC = pyslatcore.PLC

    ## Linear interpolated function
    LIN = pyslatcore.LIN

    ## Log-log interpolation function
    LOGLOG = pyslatcore.LOGLOG

## The meaning of 'mu' for a lognormal distribution.
class LOGNORMAL_MU_TYPE:
    ## mu specifies mean(x).
    MEAN_X = pyslatcore.MEAN_X
    
    ## mu specifies median(x).
    MEDIAN_X = pyslatcore.MEDIAN_X
    
    ## mu specifies mean(ln(x)).
    MEAN_LN_X = pyslatcore.MEAN_LN_X

## The meaning of 'sigma' for a lognormal distribution.    
class LOGNORMAL_SIGMA_TYPE:
    ## sigma specifies the standard deviation(x).
    SD_X = pyslatcore.SD_X

    ## sigma specifies the standard deviation(ln(x)).
    SD_LN_X = pyslatcore.SD_LN_X

class INTEGRATION_METHOD:
    BINARY_SUBDIVISION = pyslatcore.BINARY_SUBDIVISION

    REVERSE_BINARY_SUBDIVISION = pyslatcore.REVERSE_BINARY_SUBDIVISION

    LOW_FIRST_REVERSE_BINARY_SUBDIVISION = pyslatcore.LOW_FIRST_REVERSE_BINARY_SUBDIVISION

    SCATTERED = pyslatcore.SCATTERED

    DIRECTED = pyslatcore.DIRECTED

## Create a formula-based deterministic function.
# This functions wraps the library function SLAT::factory(). It returns
# a deterministic function (subclass FormulaFn), given a formula type
# and set of parameters.
#
# @param t Function type (from the FUNCTION_TYPE class). This function
#          handles non-interpolated functions.
# @param params A vector of numbers, specifying the control parameters
#  of the function.
def factory2(t, params):
    return pyslatcore.factory(t, params)

## Create an interpolated deterministic function.
# This function wraps the library function SLAT::factory(). It returns
# an interpolated deterministic function, given sets of x and y values.
#
# @param t Function type (from the FUNCTION_TYPE class). This function
#          handles interpolated functions.
# @param params A vector of numbers, specifying the x and y values for
#          the interpolation.
def factory3(t, param1, param2):
    return pyslatcore.factory(t, param1, param2)

## Create a log normal function.
# This function wraps the library function SLAT::MakeLogNormalProbabilisticFn().
# It returns a log normal probabilistic function, given values for mu 
# and sigma, and LOGNORMAL_MU_TYPE and LOGNORMAL_SIGMA_TYPE constants
# indiciating how to interpret the parameters.
def MakeLogNormalProbabilisticFn(mu_function, mu_type, sigma_function, sigma_type):
    return pyslatcore.MakeLogNormalProbabilisticFn(mu_function, mu_type, sigma_function, sigma_type)

## Create an Intensity Measure from a deterministic function.
# This function wraps the SLAT::IM.
#
# @param f A deterministic function object.
# @todo Add a 'name' parameter to the function.
def MakeIM(f):
    return pyslatcore.MakeIM(f)

## Create an Engineering Demand Parameter
# This function wraps the library function SLAT::MakeEDP(). It returns
# an EDP, given an IM, a probabilistic function, and a name for the EDP.
def MakeEDP(base_rate, dependent_rate, name):
    return pyslatcore.MakeEDP(base_rate, dependent_rate, name)

## Create a fragility function.
# This function wraps the library function SLAT::MakeFragilityFn(). It
# returns a fragility function, given a list of log normal distributions.
#
# @param parameters A list of log normal distributions (one per damage state).
# @todo Add a 'name' parameter.
def MakeFragilityFn(parameters):
    return pyslatcore.MakeFragilityFn(parameters)

## Create a loss function.
# This function wraps the library function SLAT::MakeLossFn(). It
# returns a loss function, given a list of log normal distributions.
#
# @param parameters A list of log normal distributions (one per damage state).
# @todo Add a 'name' parameter.
def MakeLossFn(parameters):
    return pyslatcore.MakeLossFn(parameters)

## Create a component group
# This function wraps SLAT::MakeCompGroup(). It returns a component group,
# given an EDP, fragility function, optional delay function, number of
# components, and the name of the component group.
# @param edp The EDP driving the components
# @param frag_fn The fragility function of the component
# @param cost_fn A loss function representing the repair cost
# @param delay_fn An (optional) loss funtion representing the time
#                 to repair the component
# @param count The number of components
# @param name  The name of the component group
def MakeCompGroup(edp, frag_fn, cost_fn, delay_fn, count, name):
    print("MakeCompGroup: {} {} {} {} {} {}".format(edp, frag_fn, cost_fn, delay_fn, count, name))
    if (delay_fn == None):
        delay_fn = MakeLossFn([DefaultLogNormalDist()])
    return pyslatcore.MakeCompGroup(edp, frag_fn, cost_fn, delay_fn, count, name)

## Create a structure
# This function wraps SLAT::MakeStructure(), which returns an empty structure.
# Data for the structure should be provided by calling its member functions 
# (e.g., AddCompGroup(), setRebuildCost(), etc.).
def MakeStructure():
    return pyslatcore.MakeStructure()

## Create a log normal distribution.
# This wraps the library function SLAT::MakeLogNormalDist(). It returns
# a log normal distribution object, given values for mu 
# and sigma, and LOGNORMAL_MU_TYPE and LOGNORMAL_SIGMA_TYPE constants
# indiciating how to interpret the parameters.
def MakeLogNormalDist(mu, mu_type, sigma, sigma_type):
    return pyslatcore.MakeLogNormalDist(mu, mu_type, sigma, sigma_type)

## Create a default log normal distribution.
# This wraps the library function SLAT::MakeLogNormalDist(). It returns
# a log normal distribution using the default parameters. See the
# documentation for pyslatcore.MakeLogNormalDist() for details.
def DefaultLogNormalDist():
    return pyslatcore.MakeLogNormalDist()

## Set the default integration parameters.
#  This wraps the library function SLAT::IntegrationSettings().
#
# @param tolerance The tolerance at which to consider an integration
#        complete.
# @param integration_eval_limit The maximum number of evaluations to allow when performing
#        an integration.
# @todo Separate search and integration limits.
def IntegrationSettings(tolerance, integration_eval_limit):
    return pyslatcore.IntegrationSettings(tolerance, integration_eval_limit)

def Set_Integration_Tolerance(tolerance):
    return pyslatcore.Set_Integration_Tolerance(tolerance)

def Set_Integration_Eval_Limit(limit):
    return pyslatcore.Set_Integration_Eval_Limit(int(limit))

def Set_Integration_Search_Limit(limit):
    pyslatcore.Set_Integration_Search_Limit(int(limit))

def Set_Integration_Method(type):
    return pyslatcore.Set_Integration_Method(type)

def Reset_Statistics():
    pyslatcore.reset_statistics()

def Format_Statistics():
    return pyslatcore.format_statistics()


## Enable/disable logging to stderr
#  This wraps the library function SLAT::LogToStdErr().
#
# @param flag True to enable logging
def LogToStdErr(flag):
    return pyslatcore.LogToStdErr(flag)

## Set log file name.
#  If a file is open from an earlier call to this funciton, that file 
#  will be closed.
#
#  This wraps the library function SLAT::SetLogFile().
#
# @param path The path to the log file. It will be created or emptied as necessary.
#
# @param flag True to enable logging
def SetLogFile(path):
    return pyslatcore.SetLogFile(path)

## Represents a log normal distribution.
#
# Wraps a pyslatcore::LogNormalDist.
class lognormaldist:
    ## Constructor 
    # @param dist A pyslatcore::LogNormalDist
    def __init__(self, dist):
        self._dist = dist

    ## Return mean(X) for the distribution.
    def mean(self):
        return self._dist.get_mean_X()

    ## Return median(X) for the distribution.
    def median(self):
        return self._dist.get_median_X()

    ## Return mean(ln(X)) for the distribution.
    def mean_ln(self):
        return self._dist.get_mu_lnX()

    ## Return standard deviation(X) for the distribution.
    def sd(self):
        return self._dist.get_sigma_X();
    
    ## Return standard deviation(ln(X)) for the distribution.
    def sd_ln(self):
        return self._dist.get_sigma_lnX();

    ## Return a string describing the distribution, for debugging,
    # warning, and error messages.
    def __str__(self):
        return("Log Normal Distribution: mean: {}, sd_ln_x: {}.".format(
            self.mean(), self.sd_ln()))

## Represents a deterministic function.
#
# Wraps a pyslatcore::DeterministicFn.
class detfn:
    ## A dictionary of all detfn objects, indexed by name.
    defs = dict()

    ## @var _id 
    # A programmer-supplied string identifying the object.
    # This is used to access the detfn by id, and included in 
    # debugging, warning, and error messages.

    ## @var _type
    # A FUNCTION_TYPE value, indicating the type of function
    # represented by the object. Not needed outside the 
    # constructor, but saved to assist in debugging.

    ## @var _parameters
    #  A copy of the parameters used to create the function.
    # Not needed outside the constructor, but saved to assist
    # in debugging.

    ## Constructor
    #
    # @param id An identifier for the object (we use strings, but
    #        any object should work).
    # @param type A string identifying the type of function to
    #        create. It may be one of:
    #            - 'power law' (power-law curve)
    #            - 'hyperbolic' (non-linear hyperbolic curve)
    #            - 'loglog' (log-log interpolated data)
    #            - 'linear' (linear interpolated data)
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

        if fntype == FUNCTION_TYPE.LOGLOG or fntype == FUNCTION_TYPE.LIN:
            self._func = factory3(fntype, parameters[0], parameters[1])
        else:
            self._func = factory2(fntype, parameters)

        if id != None:
            detfn.defs[id] = self

    ## Return the detfn object with the corresponding id.
    # @param id The identifier provided to the constructor of a detfn object.
    def lookup(id):
        return detfn.defs.get(id)

    ## Return the identifier provided to the constuctor.
    def id(self):
        return self._id

    ## Return the pyslat.DeterministicFn function wrapped by the object.
    def function(self):
        return self._func

    ## Evaluate the function at a specific x value.
    # @param x The value at which to evaluate the function.
    def ValueAt(self, x):
        return self._func.ValueAt(x)

    ## Return a string describing the detfn object.
    #  Used for debugging, warning, and error messages.
    def __str__(self):
        return("Deterministic Function '{}', of type '{}', " +
               "from parameters {}.".format(self._id, 
                                            self._type,
                                            self._parameters))

## Represents a probabilistic function.
#
# Wraps a pyslatcore::ProbabilisticFn.
class probfn:
    ## A dictionary of all probfn objects, indexed by name.
    defs = dict()

    ## @var _id 
    # A programmer-supplied string identifying the object.
    # This is used to access the detfn by id, and included in 
    # debugging, warning, and error messages.

    ## @var _type
    # A FUNCTION_TYPE value, indicating the type of function
    # represented by the object. Not needed outside the 
    # constructor, but saved to assist in debugging.

    ## @var _mu_func
    # A list of two items. The first is a LOGNORMAL_MU_TYPE value, 
    # indicating the attribute the second element--a deterministic
    # function--describes.

    ## @var _sigma_func
    # A list of two items. The first is a LOGNORMAL_SIGMA_TYPE value, 
    # indicating the attribute the second element--a deterministic
    # function--describes.

    ## Constructor
    # @param id A string identifing the object. This will be used in
    #        in debugging, warning, and error messages.
    # @param type A string identifying the type of function. Currently,
    #        only 'lognormal' is supported.
    #
    # @param _mu_func 
    # A list of two items. The first is a LOGNORMAL_MU_TYPE value, 
    # indicating the attribute the second element--a deterministic
    # function--describes.
    #
    # @param _sigma_func
    # A list of two items. The first is a LOGNORMAL_SIGMA_TYPE value, 
    # indicating the attribute the second element--a deterministic
    # function--describes.
    def __init__(self, id, type, mu_func, sigma_func):
        
        self._id = id
        self._type = type
        self._mu_func = mu_func
        self._sigma_func = sigma_func
        self._func = MakeLogNormalProbabilisticFn(mu_func[1].function(), 
                                                  mu_func[0],
                                                  sigma_func[1].function(),
                                                  sigma_func[0]) 
        if id != None:
            probfn.defs[id] = self
        
        if not(type == 'lognormal'):
            raise ValueError("Invalid type '" + type + "' for probfn '" + id + "'.")

    
    ## Return the probfn object with the corresponding id.
    # @param id The identifier provided to the constructor of a probfn object.
    def lookup(id):
        return probfn.defs.get(id)

    ## Return the id provided to the constructor.
    def id(self):
        return self._id

    ## Return the pyslatcore::ProbabilisticFn wrapped by this object.
    def function(self):
        return self._func

    ## Return mean(x).
    #  Return the mean of the probabilistic distribution, for a given 'x'.
    #  @param x The x value at which to evaluate the function.
    def Mean(self, x):
        return self._func.Mean(x)

    ## Return mean(ln(x)).
    #  Return the mean of the log of the probabilistic distribution, for a given 'x'.
    #  @param x The x value at which to evaluate the function.
    def MeanLn(self, x):
        return self._func.MeanLn(x)

    ## Return median(x).
    #  Return the median of the probabilistic distribution, for a given 'x'.
    #  @param x The x value at which to evaluate the function.
    def Median(self, x):
        return self._func.Median(x)

    ## Return standard deviation(ln(x)).
    #  Return the standard deviation of the log  of the probabilistic 
    #  distribution, for a given 'x'.
    #  @param x The x value at which to evaluate the function.
    def SD_ln(self, x):
        return self._func.SD_ln(x)

    ## Return standard deviation(x).
    #  Return the standard deviation the probabilistic distribution, for a 
    #  given 'x'.
    #  @param x The x value at which to evaluate the function.
    def SD(self, x):
        return self._func.SD(x)

    ## Return the inverse probability of exceedence.
    #  Given an input value 'x', and a probability 'p', return
    #  the corresponding output value 'y' such that f(x) <= y 
    #  with probability p. For example, if p == 0.10, then we'll
    #  return a value y such that f(x) <= y 10% of the time.
    #
    #  @param x The input point at which to evaluate the function.
    #  @param p The probability we're interested in.
    # 
    #  @return y, such that f(x) <= y with probability p.
    def X_at_exceedence(self, x, p):
        return self._func.X_at_exceedence(x, p)

    ## Return a string describing the probfn object.
    #  Used for debugging, warning, and error messages.
    def __str__(self):
        return(("Probabilistic function '{}' of type {}, using the deterministic function {} for {}, and " +
               "the deterministic function {} for {}.").format(
                   self._id, self._type,
                   self._mu_func[1].id(), self._mu_func[0],
                   self._sigma_func[1].id(), self._sigma_func[0]))

        
## Represents an intensity measure.
#
# Wraps a pyslatcore::IM.
class im:
    ## A dictionary of all im objects, indexed by name.
    defs = dict()

    ## @var _id
    # A programmer-supplied string identifying the object.
    # This is used to access the im by id, and included in 
    # debugging, warning, and error messages.

    ## @var _detfn
    # A deterministic function, representing the IM rate-of-exceedence curve.
    
    ## @var _func
    # The wrapped pyslatcore::IM object.

    ## @var _collapse
    # A lognormal distribution representing probability(collapse) | IM.
    
    ## @var _demolition
    # A lognormal distribution representing probability(demolition) | IM.

    ## Constructor
    # @param id A string identifying the object. This will be used in
    #        debugging, warning, and error messages.
    # @param detfn The deterministic function representing the exceedence 
    #        curve.
    def __init__(self, id, detfn):
        self._id = id
        self._detfn = detfn
        self._func = MakeIM(detfn.function())
        self._collapse = None
        self._demolition = None
        if id != None:
            im.defs[id] = self

            
    ## Return the im object with the corresponding id.
    # @param id The identifier provided to the constructor of a probfn object.
    def lookup(id):
        return im.defs.get(id)


    ## Return the id provided to the constructor.
    def id(self):
        return self._id

    ## Return the pyslatcore::IM wrapped by this object.
    def function(self):
        return self._func

    ## Return the rate-of-exceedence for a given IM value
    # @param x The IM value of interest.
    def getlambda(self, x):
        if type(x) == list:
            result = list(self._func.get_lambda(x))
        else:
            result = self._func.get_lambda(x)
        return result

    ## Return the probality of repair for a given IM value.
    # @param x The IM value of interest.
    def pRepair(self, x):
        if type(x) == list:
            result = list(self._func.pRepair(x))
        else:
            result = self._func.pRepair(x)
        return result
    
    ## Return the probality of collapse for a given IM value.
    # @param x The IM value of interest.
    def pCollapse(self, x):
        if type(x) == list:
            result = list(self._func.pCollapse(x))
        else:
            result = self._func.pCollapse(x)
        return result

    ## Set the probality of collapse | IM. 
    # @param collapse A deterministic function representing the 
    #        probability of collapse given IM.
    def SetCollapse(self, collapse):
        self._collapse = collapse
        self._func.SetCollapse(collapse)

    ## Return the rate of collapse
    # Returns the rate of collapse of the underlying im object, 
    # calculated by the library.
    def CollapseRate(self):
        return self._func.CollapseRate()

    ## Return the probality of demolition for a given IM value.
    # @param x The IM value of interest.
    def pDemolition(self, x):
        if type(x) == list:
            result = list(self._func.pDemolition(x))
        else:
            result = self._func.pDemolition(x)
        return result

    ## Set the probality of demolition | IM. 
    # @param collapse A deterministic function representing the 
    #        probability of demolition given IM.
    def SetDemolition(self, demolition):
        self._demolition = demolition
        self._func.SetDemolition(demolition)

    ## Return the rate of demolition
    # Returns the rate of demolition of the underlying im object, 
    # calculated by the library.
    def DemolitionRate(self):
        return self._func.DemolitionRate()

    ## Return a string describing the im object.
    #  Used for debugging, warning, and error messages.
    def __str__(self):
        if self._collapse:
            return("Intensity measure '{}', based on the deterministic function '{}', with {}".format(self._id, self._detfn, self._collapse))
        else:
            return("Intensity measure '{}', based on the deterministic function '{}'.".format(self._id, self._detfn.id()))

## Represents an Engineering Demand Parameter
#
# Wraps a pyslatcore::EDP.
class edp:
    ## A dictionary of all edp objects, indexed by name.
    defs = dict()

    ## @var _id
    # A programmer-supplied string identifying the object.
    # This is used to access the edp by id, and included in 
    # debugging, warning, and error messages.

    ## @var _im
    # The Intensity Measure driving the demand.

    ## @var _fn
    # The probabilitic function representing the EDP|IM relationship.

    ## @var _func
    # The wrapped pyslatcore::EDP object.

    ## Constructor
    #  @param id A string identifying the object. This will be used in 
    #         debugging, warning, and error messages.
    #  @param im The intensity measure driving the engineering demand.
    #  @param fn The probabilitic function describing the EDP|IM relationship.
    def __init__(self, id, im, fn):
        self._id = id
        self._im = im
        self._fn = fn
        self._func = MakeEDP(im.function(), fn.function(), id)
        if id != None:
            edp.defs[id] = self

    ## Return the edp object with the corresponding id.
    #  @param id The identifier provided to the constructor of a edp object.
    def lookup(id):
        if not isinstance(id, pyslatcore.EDP):
            return edp.defs.get(id)
        else:
            for e in edp.defs.values():
                if (e.function().AreSame(id)):
                    return e
            return None

    ## Return the id provided to the constructor.
    def id(self):
        return self._id

    ## Return the IM driving the EDP.
    def get_IM(self):
        return self._im

    ## Return the probabilitic function describing the EDP|IM relationship.
    def function(self):
        return self._func

    ## Return mean(EDP) for a given IM.
    #  @param x The IM value at which to calculate mean(x).
    def Mean(self, x):
        if type(x) == list:
            result = list(self._func.Mean(x))
        else:
            result = self._func.Mean(x)
        return result

    ## Return mean(ln(EDP)) for a given IM.
    #  @param x The IM value at which to calculate mean(ln(x)).
    def MeanLn(self, x):
        if type(x) == list:
            result = list(self._func.MeanLn(x))
        else:
            result = self._func.MeanLn(x)
        return result

    ## Return median(EDP) for a given IM.
    #  @param x The IM value at which to calculate median(x).
    def Median(self, x):
        if type(x) == list:
            result = list(self._func.Median(x))
        else:
            result = self._func.Median(x)
        return result

    ## Return standard deviation(ln(EDP)) for a given IM.
    #  @param x The IM value at which to calculate standard deviation(ln(x)).
    def SD_ln(self, x):
        if type(x) == list:
            result = list(self._func.SD_ln(x))
        else:
            result = self._func.SD_ln(x)
        return result

    ## Return standard deviation(EDP) for a given IM.
    #  @param x The IM value at which to calculate standard deviation(x).
    def SD(self, x):
        if type(x) == list:
            result = list(self._func.SD(x))
        else:
            result = self._func.SD(x)
        return result

    ## Return the probability exceeding a given value.
    #  @param x The EDP value we're interested in
    #  @todo Is this 'probability' or 'rate'? Make sure this is described
    #        correctly at all levels of code.
    def getlambda(self, x):
        if type(x) == list:
            result = list(self._func.get_lambda(x))
        else:
            result = self._func.get_lambda(x)
        return result

    ## Return the inverse probability of exceedence.
    #  Given an IM value 'im', and a probability 'p', return
    #  the corresponding output value 'y' such that f(im) <= y 
    #  with probability p. For example, if p == 0.10, then we'll
    #  return a value y such that f(im) <= y 10% of the time.
    #
    #  @param im The input point at which to evaluate the function.
    #  @param p The probability we're interested in.
    # 
    #  @return y, such that f(im) <= y with probability p.
    def X_at_exceedence(self, im, p):
        return self._fn.X_at_exceedence(im, p)

    ## Return a string describing the im object.
    #  Used for debugging, warning, and error messages.
    def __str__(self):
        return(("Engineering Demand Parameter '{}' using the intensity measure '{}', and " +
                "the probabilistic function {}.").format(self._id, self._im.id(), self._fn.id()))

## Represents a fragility function.
#
# Wraps a pyslatcore::FragilityFn.
class fragfn:
    ## A dictionary of all fragfn objects, indexed by name.
    defs = dict()

    ## @var _id
    # A programmer-supplied string identifying the object.
    # This is used to access the edp by id, and included in 
    # debugging, warning, and error messages.

    ## @var _func
    #  The wrapped pyslatcore::FragilityFn

    ## Constructor
    #  @param id A string identifying the object. This will be used in 
    #         debugging, warning, and error messages.
    def __init__(self, id):
        self._id = id
        self._func = None
        if id != None:
            fragfn.defs[id] = self

    ## Return the fragfn object with the corresponding id.
    #  @param id The identifier provided to the constructor of a fragfn object.
    def lookup(id):
        if not isinstance(id, pyslatcore.FragilityFn):
            return fragfn.defs.get(id)
        else:
            for f in fragfn.defs.values():
                if (f.function().AreSame(id)):
                    return f
            return None
        
    ## Return the id provided to the constructor.
    def id(self):
        return(self._id)

    ## Return the wrapped pyslatcore::FragilityFn.
    def function(self):
        return self._func

    ## Return a list representing the probability of the onset of each 
    #  damage state being exceeded for a given EDP value.
    def pExceeded(self, edp):
        return self._func.pExceeded(edp)

## Fragility function from a database    
#
# This functionality isn't implemented yet. At this point, it is a placeholder
# to support the corresponding functionality in the SLAT command language.
class fragfn_db(fragfn):
    ## Constructor
    #  @param id A string identifying the object. This will be used in 
    #         debugging, warning, and error messages.
    #  @param db_params Database paremeters
    def __init__(self, id, db_params):
        super().__init__(id)
        self._db_params = db_params

    def __str__(self):
        return("Fragility Function '{}', from database: {}.".format(self._id, self._db_params))

## User-defined fragility function
class fragfn_user(fragfn):
    ## Constructor
    #  @param id A string identifying the object. This will be used in 
    #         debugging, warning, and error messages.
    #  @param options A dictionary mapping 'mu' and 'sd' to a LOGNORMAL_MU_TYPE
    #         and a LOGNORMAL_SIGMA_TYPE. 
    #  @param scalars A list of [mu, sigma] pairs (one per damage state). These
    #         describe the onset of each damage state as a log normal 
    #         distribution, with mu and sigma interpreted based on 'options'.
    #   
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
## Represents a loss function.
#
# Wraps a pyslatcore::LossFn.
class lossfn:
    ## A dictionary of all edp objects, indexed by name.
    defs = dict()

    ## @var _id
    # A programmer-supplied string identifying the object.
    # This is used to access the lossfn by id, and included in 
    # debugging, warning, and error messages.

    ## @var _options
    #  A dictionary mapping 'mu' and 'sd' to a LOGNORMAL_MU_TYPE
    #  and a LOGNORMAL_SIGMA_TYPE. 

    ## @var _data
    #  A list of [mu, sigma] pairs (one per damage state). These
    #  describe the onset of each damage state as a log normal 
    #  distribution, with mu and sigma interpreted based on 'options'.

    ## @var _func
    #  The wrapped pyslatcore::LossFn

    ## Consructor
    #  @param id A string identifying the object. This will be used in 
    #         debugging, warning, and error messages.
    #  @param options A dictionary mapping 'mu' and 'sd' to a LOGNORMAL_MU_TYPE
    #         and a LOGNORMAL_SIGMA_TYPE. 
    #  @param data A list of [mu, sigma] pairs (one per damage state). These
    #         describe the onset of each damage state as a log normal 
    #         distribution, with mu and sigma interpreted based on 'options'.
    def __init__(self, id, options, data):
        self._id = id
        self._options = options
        self._data = data

        params = []
        for d in data:
            if d[0]:
                params.append(MakeLogNormalDist(d[0], options['mu'], d[1], options['sd']))
            else:
                params.append(DefaultLogNormalDist())
        self._func = MakeLossFn(params)
        if id != None:
            lossfn.defs[id] = self

    ## Return the lossfn object with the corresponding id.
    #  @param id The identifier provided to the constructor of a lossfn object.
    def lookup(id):
        return lossfn.defs.get(id)

    ## Return the id provided to the constructor.
    def id(self):
        return(self._id)

    ## Return the wrapped pyslatcore::LossFn
    def function(self):
        return self._func

    ## Return a string describing the im object.
    #  Used for debugging, warning, and error messages.
    def __str__(self):
        return("Loss Function '{}', from {} as {}.".format(self._id, self._data, self._options))

## Represents a component group
#
# Wraps a pyslatcore::CompGroup
class compgroup:
    ## A dictionary of all fragfn objects, indexed by name.
    defs = dict()

    ## @var _id
    # A programmer-supplied string identifying the object.
    # This is used to access the component group by id, and included in 
    # debugging, warning, and error messages.

    ## @var _edp
    # The engineering demand parameter affecting the components.
    
    ## @var _frag
    # The fragility function for the components.

    ## @var _cost
    # The cost function for the component group.

    ## @var _delay
    # The delay functtion for the component group.

    ## @var _count
    # The number of components in the group.

    ## @var _func
    # The wrapped pyslatcore::CompGroup object.
    
    ## Constructor
    #  @param id A string identifying the object. This will be used in 
    #         debugging, warning, and error messages.
    # @param edp The edp object driving the component group.
    # @param fram The fragility function.
    # @param cost The cost function (may be None).
    # @param delay The delay function (may be None).
    # @param count The number of components.
    def __init__(self, id, edp, frag, cost, delay, count):
        self._id = id
        self._edp = edp
        self._frag = frag
        self._cost = cost
        self._count = count
        self._delay = delay
        self._func = MakeCompGroup(edp.function(),
                                   frag.function(),
                                   cost and cost.function(),
                                   delay and delay.function(),
                                   count, id)
        if id != None:
            compgroup.defs[id] = self

    ## Return the compgroup object with the corresponding id.
    #  @param id The identifier provided to the constructor of a compgroup object.
    def lookup(id):
        if not isinstance(id, pyslatcore.CompGroup):
            return compgroup.defs.get(id)
        else:
            for c in compgroup.defs.values():
                if (c.function().AreSame(id)):
                    return c
            return None

    ## Return the im driving the EDP affecting the components.
    def get_IM(self):
        return self._edp.get_IM()

    ## Return the fragility function.
    def fragfn(self):
        return self._frag

    ## Return the number of damage states.
    def size(self):
        return self._frag.size()
        
    ## Return the underlying pyslatcore::CompGroup.
    def function(self):
        return self._func

    ## Return mean(cost|EDP).
    # @param edp The EDP value we're interested in.
    def E_Cost_EDP(self, edp):
        return self._func.E_Cost_EDP(edp)

    ## Return standard deviation(ln(EDP)).
    # @param edp The EDP value we're interested in.
    def SD_ln_Cost_EDP(self, edp):
        return self._func.SD_ln_cost_EDP(edp)

    ## Return mean(cost|IM).
    # @param im The IM value we're interested in.
    def E_Cost_IM(self, im):
        return self._func.E_Cost_IM(im)

    ## Return standard deviation(ln(IM)).
    # @param im The IM value we're interested in.
    def SD_ln_Cost_IM(self, im):
        return self._func.SD_ln_Cost_IM(im)

    ## Return mean(cost) for a given number of years, assuming a given
    #  discount rate.
    # @param t Number of years
    # @param l Discount rate.
    def E_cost(self, t, l):
        return self._func.E_cost(t, l)

    ## Return the mean(annual cost) 
    def E_annual_cost(self):
        return self._func.E_annual_cost()

    ## Return the expected rate of exceedence of a given cost.
    # @param loss The cost we're interested in.
    def lambda_cost(self, loss):
        return self._func.lambda_cost(loss)

    ## Return a list of the probability of exceeding the onset of each damage 
    # state for a given IM.
    # @param im The IM value we're interested in.
    def pDS_IM(self, im):
        return self._func.pDS_IM(im)

    ## Return mean(delay|IM).
    # @param im The IM value we're interested in.
    def E_Delay_IM(self, im):
        return self._func.E_Delay_IM(im)

    ## Return mean(delay|EDP).
    # @param edp The EDP value we're interested in.
    def E_Delay_EDP(self, edp):
        return self._func.E_Delay_EDP(edp)

    ## Return standard deviation(ln(delay|IM)).
    # @param im The IM value we're interested in.
    def SD_ln_Delay_IM(self, im):
        return self._func.SD_ln_Delay_IM(im)

    ## Return standard deviation(ln(delay|EDP)).
    # @param edp The EDP value we're interested in.
    def SD_ln_Delay_EDP(self, edp):
        return self._func.SD_ln_Delay_EDP(edp)
    
    ## Return a vector indicating the expected rate of occurance for each 
    # damage state.
    def Rate(self):
        return self._func.Rate()

    ## Return the id provided to the constructor.
    def id(self):
        return(self._id)
        
    ## Return a string describing the im object.
    #  Used for debugging, warning, and error messages.
    def __str__(self):
        return("Component Group '{}' using {}, {}, {}, and {} ({} component(s))".format(
            self._id, 
            self._edp.id(),
            self._frag.id(),
            self._cost and self._cost.id(),
            self._delay and self._delay.id(),
            self._count))

## Represents a structure
#
# Wrap a pyslatcore::Structure
class structure:
    ## A dictionary of all fragfn objects, indexed by name.
    defs = dict()
    
    ## @var _id
    # A programmer-supplied string identifying the object.
    # This is used to access the component group by id, and included in 
    # debugging, warning, and error messages.

    ## @var _structure
    # The underlying pyslatcore::Structure.

    ## @var _im
    # The intensity measure driving the structure. It is set
    # when the first component group is supplied; subsequent 
    # component groups must be tied to the same IM.

    ## Constructor
    #  @param id A string identifying the object. This will be used in 
    #         debugging, warning, and error messages.
    def __init__(self, id):
        super().__init__()
        self._structure = MakeStructure()
        self._im = None
        if id != None:
            structure.defs[id] = self

    ## Return the structure object with the corresponding id.
    #  @param id The identifier provided to the constructor of a structure object.
    def lookup(id):
        return structure.defs.get(id)

    ## Return a string describing the structure object.
    #  Used for debugging, warning, and error messages.
    def __str__(self):
        return "Structure"

    ## Return the im connected to the structure.
    def get_IM(self):
        return self._im

    ## Return the pdf|im for the structure:
    def pdf(self, im):
        return self._structure.pdf(im)

    ## Add a component group to the structure.
    #
    # If this is the first group to be added to the structure, the 
    # associated IM will be recorded. If a subsequent component group
    # has a different IM, an error will be raised.
    #
    # @param group The component group to be added.
    #
    # @todo Test that IM mis-matches are handled correctly.
    def AddCompGroup(self, group):
        self._structure.AddCompGroup(group.function())
        if self._im == None:
            self._im = group.get_IM()
        else:
            if not(self._im == group.get_IM()):
                raise ValueError("Component group tied to wrong IM " +
                                 "(expected " + self._im + "; got " + 
                                 group.get_IM() << ")");


    ## Set the rebuild cost of the structure
    # @param cost A log normal distribution represeting the rebuild cost of
    # the structure.
    def setRebuildCost(self, cost):
        self._structure.setRebuildCost(cost)

    ## Return the rebuild cost of the structure
    def getRebuildCost(self):
        return lognormaldist(self._structure.getRebuildCost())

    ## Set the rebuild cost of the structure
    # @param cost A log normal distribution represeting the demolitionrate cost
    # of the structure.
    def setDemolitionCost(self, cost):
        self._structure.setDemolitionCost(cost)

    ## Return the demolitionrate cost of the structure
    def getDemolitionCost(self):
        return lognormaldist(self._structure.getDemolitionCost())

    ## Return the cost of an event of a given intensity measure.
    #  The cost is expressed as a log normal distribution, and may include
    #  the possibility of collapse.
    #
    # @param im The IM value we're interested in.
    # @param consider_collapse A boolean indicating whether to consider
    #        collapse.
    # @return cost, as a lognormaldist.
    def Cost(self, im, consider_collapse):
        return lognormaldist(self._structure.Cost(im, consider_collapse))

    ## Return the total cost of an event of a given intensity measure.
    #  The cost is expressed as a log normal distribution, and includes
    #  repair, demolition, and collapse.
    #
    # @param im The IM value we're interested in.
    # @return total cost, as a lognormaldist.
    def TotalCost(self, im):
        return lognormaldist(self._structure.TotalCost(im))

    ## Return the repair, demolition, and collapse costs of an event of
    #  a given intensity measure.
    #
    # @param im The IM value we're interested in.
    # @return A list of three values, each a pyslat.lognormaldist, representing
    #         the cost of repair, demolition, and collapse
    def CostsByFate(self, im):
        result = self._structure.CostsByFate(im)
        return [lognormaldist(result[0]),
                lognormaldist(result[1]),
                lognormaldist(result[2])]

    ## Return the annual cost.
    #  @return The annual cost, as a pyslat.lognormaldist.
    def AnnualCost(self):
        return lognormaldist(self._structure.AnnualCost())

    ## Return mean(cost) for a given number of years, assuming a given
    #  discount rate.
    # @param t Number of years
    # @param l Discount rate.
    def E_cost(self, t, l):
        return self._structure.E_cost(t, l)
    
    ## Return the deaggregated cost.
    #  @param im The intensity measure value we're interested in.
    #  @return The cost, as a pair of pyslat.lognormaldist objects.
    #          The first represents the cost of non-collapse (repair and
    #          demolition); the second represents the cost due to collapse.
    def DeaggregatedCost(self, im):
        return self._structure.DeaggregatedCost(im)

    ## Return the components, grouped by EDP.
    #  @return A list, each item of which is list of component groups with 
    #          a common EDP.
    def ComponentsByEDP(self):
        return self._structure.ComponentsByEDP()

    ## Return the components, grouped by fragility function.
    #  @return A list, each item of which is list of component groups with 
    #          a common fragility function.
    def ComponentsByFragility(self):
        return self._structure.ComponentsByFragility()
    
## Objects for recording output.
#  This class doesn't wrap anything. It makes sense to move at least the
#  generate_output() method to C++, to run it in parallel, but I'm undecided
#  as to whether we should move more of the class there.
# @todo Further subclass to simplify code.
class recorder:
    ## A dictionary of all recorder objects, indexed by name.
    defs = dict()
    
    ## @var _type
    #  The type of recorder, as a string. This corresponds to 'recorder_type'
    #  in the SLAT interpreter.

    ## @var _function
    #  The underlying object being recorded. This will vary based on the
    #  recorder type.

    ## @var _options
    #   The options provided to the recorder, such as the destination file and 
    #   whether to append to the file.

    ## @var _at 
    # This is only required for some recorders. If present, it is a list of 
    # values for which output will be generated.

    ## Constructor
    #  @param id The identifier provided to the constructor of a structure object.
    #  @param type The type of recorder, as a string. This corresponds to
    #         'recorder_type' in the SLAT interpreter.
    #  @param function The underlying object being recorded.
    #  @param options  Additional options, such as the output file name.
    #  @param at An optional set of points for which to record output.
    def __init__(self, id, type, function, options, columns, at):
        super().__init__()

        print("recorder")
        print("id: {}".format(id))
        print("type: {}".format(type))
        print("function: {}".format(function))
        print("options: {}".format(options))
        print("at: {}".format(at))

        self._type = type
        self._function = function
        self._options = options
        self._at = at
        if at != None:
            self._at = list(at)

        if not type == 'dsrate' and not type == 'collrate' \
           and not type == 'structcost' \
           and at==None:
            raise ValueError('MUST PROVIDE ''AT'' CLAUSE')

        if (type =='dsedp' or type == 'dsim') and columns == None:
            columns = []
            for i in range(function.size()):
                columns.append("DS{}".format(i + 1))
            columns.append("E(DS)")
        elif (type == 'probfn' or type == 'edpim') and columns == None:
            columns = ['mean_ln_x', 'sd_ln_x']
        elif (type == 'costedp' or type =='costim' \
              or type == 'delayedp' or type == 'delayim' \
              or type == 'totalcost') \
             and columns == None:
            columns = ['mean_x', 'sd_ln_x']
        self._columns = columns
        if id != None:
            recorder.defs[id] = self
            
        print(self)

    ## Return the recorder object with the corresponding id.
    #  @param id The identifier provided to the constructor of a structure object.
    #  @return The recorder with the corresponding identifier.
    def lookup(id):
        return recorder.defs.get(id)

    ## Returns all recorders.
    # @return All recorders.
    def all():
        return recorder.defs.values()

    ## Return a string describing the recorder.
    #  Used for debugging, warning, and error messages.
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

    ## Generate the output for the recorder. 
    # The output is sent to stdout; the calling function ('run()') will 
    # redirect it as necessary.
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
                      'costedp': ['EDP', None],
                      'costim': ['IM', None],
                      'delayedp': ['EDP', None],
                      'delayim': ['IM', None],
                      'costrate': ['t', 'Rate'],
                      'totalcost': ['IM', None],
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


            if self._type == 'imrate':
                yvals = self._function.getlambda(self._at)
                for x, y in zip(self._at, yvals):
                    print("{:>15.6}{:>15.6}".format(x, y))
            elif self._type == 'collapse':
                yvals_demo = self._function.pDemolition(self._at)
                yvals_coll = self._function.pCollapse(self._at)
                for x, demo, coll in zip(self._at, yvals_demo, yvals_coll):
                    print("{:>15.6}{:>15.6}{:>15.6}".format(x, demo, coll))
            elif self._type == 'edpim':
                yvals = []
                for y in self._columns:
                    if y == 'mean_x':
                        yvals.append(self._function.Mean(self._at))
                    elif y == 'mean_ln_x':
                        yvals.append(self._function.MeanLn(self._at))
                    elif y == 'median_x':
                        yvals.append(self._function.Median(self._at))
                    elif y == 'sd_ln_x':
                        yvals.append(self._function.SD_ln(self._at))
                    elif y == 'sd_x':
                        yvals.append(self._function.SD(self._at))
                for x, y in zip(self._at, map(list, zip(*yvals))):
                    line = "{:>15.6}".format(x)
                    for yval in y:
                        line = "{}{:>15.6}".format(line, yval)
                    print(line)
#            elif type(self._function) == edp:
#                print(self._type)
#                print(self._columns)
#                raise ValueError("EDP: " + self._type)
            else:
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
                    elif self._type == 'costrate':
                        loss_rate = self._function.lambda_cost(x)
                        line = "{}{:>15.6}".format(line, loss_rate)
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
                                if self._type == 'costedp':
                                    yval = self._function.E_Cost_EDP(x)
                                elif self._type == 'costim':
                                    yval = self._function.E_Cost_IM(x)
                                elif self._type == 'delayim':
                                    yval = self._function.E_Delay_IM(x)
                                elif self._type == 'delayedp':
                                    yval = self._function.E_Delay_EDP(x)
                                elif self._type == 'totalcost':
                                    yval = self._function.TotalCost(x).mean()
                                else:
                                    yval = self._function.Mean(x)
                            elif y == 'mean_ln_x':
                                if self._type == 'totalcost':
                                    yval = self._function.TotalCost(x).mean_ln()
                                else:
                                    yval = self._function.MeanLn(x)
                            elif y == 'median_x':
                                if self._type == 'totalcost':
                                    yval = self._function.TotalCost(x).median()
                                else:
                                    yval = self._function.Median(x)
                            elif y == 'sd_ln_x':
                                if self._type == 'costedp':
                                    yval = self._function.SD_ln_Cost_EDP(x)
                                elif self._type == 'costim':
                                    yval = self._function.SD_ln_Cost_IM(x)
                                elif self._type == 'delayim':
                                    yval = self._function.SD_ln_Delay_IM(x)
                                elif self._type == 'delayedp':
                                    yval = self._function.SD_ln_Delay_EDP(x)
                                elif self._type == 'totalcost':
                                    yval = self._function.TotalCost(x).sd_ln()
                                else:
                                    yval = self._function.SD_ln(x)
                            elif y == 'sd_x':
                                if self._type == 'totalcost':
                                    yval = self._function.TotalCost(x).sd()
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
                              yval = self._function.E_Cost_EDP(x)
                        else:
                            yval = "*****"
                        line = "{}{:>15.6}".format(line, yval)
                    print(line)

    ## Run the recorder.
    #  This invokes generate_output(), redirecting the output to a file
    #  as required.
    def run(self):
        #print("RUN {}".format(self))
        destination = full_output_path(self._options.get('filename'))
        if destination != None:
            if self._options.get('append'):
                f = open(destination, "a")
            else:
                f = open(destination, "w")
            with redirect_stdout(f):
                self.generate_output()

        else:
            self.generate_output()

## Collapse Rate Recorder
#  This subclass represents the 'collrate' recorder.
#  It prints the IM identifier, rate of demolition, and rate of collapse, 
#  including column headers.
class CollRateRecorder(recorder):
    ## Constructor
    # Just recorder.__init__.
    def __init__(self, id, type, function, options, columns, at):
        super().__init__(id, type, function, options, columns, at)

    ## Print the IM identifier, rate of demolition, and rate of collapse, 
    #  including column headers.
    def generate_output(self):
        print("{:>15}{:>30}{:>30}".format("IM", "rate(Demolition)", "rate(Collapse)"))
        print("{:>15}{:>30}{:>30}".format(self._function.id(), self._function.DemolitionRate(),
                                          self._function.CollapseRate()))
## Structure Cost Recorder
#  This subclass represents the ''structcost' recorder.
#  It handles all the variations of 'structcost' supported by the SLAT language.
class StructcostRecorder(recorder):
    ## Constructor
    # Supplements recorder.__init__ with additional pre-processing.
    def __init__(self, id, type, function, options, columns, at):
        print("> StructcostRecorder(", id, ")")
        print("type: ", type)
        print("function: ", function)
        print("options: ", options)
        print("columns: ", columns)
        print("at: ", at)
        if  (not options['structcost-type'] == 'annual') \
            and at==None:
            raise ValueError('MUST PROVIDE ''AT'' CLAUSE')

        if columns == None or len(columns) == 0:
            columns = ['mean_x']
            
        super().__init__(id, type, function, options, columns, at)


    ## Print 'structcost'recorder data.
    # Output varies with 'structcost-type':
    #    - 'annual': One line (plus headings), displaying the requested
    #       parameters (mean(x), mean(ln(x)), etc.) of the annual cost
    #       distribution.
    #    - 'by-edp', 'by-frag', 'by-fate': displays the cost for each
    #       'at' intensity measure value, appropriately deaggregated.
    #       
    #  @todo Refactor code for 'by-*' cases.
    def generate_output(self):
        if self._options['structcost-type'] == 'annual':
            line1 = ""
            line2 = ""
            annual_loss = self._function.AnnualCost()
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
        elif self._options['structcost-type'] == 'by-edp':
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
            
                
        elif self._options['structcost-type'] == 'by-frag':
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
                

        elif self._options['structcost-type'] == 'by-fate':
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
                        dist = self._function.CostsByFate(x)[0]
                    elif f == 'demo':
                        dist = self._function.CostsByFate(x)[1]
                    elif f == 'coll':
                        dist = self._function.CostsByFate(x)[2]
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
        elif self._options['structcost-type'] == 'npv':
            discount_rate = self._options['discount-rate']
            print("{:>15}{:>15}".format("Year", "E[ALt]"))
            for year in self._at:
                line = "{:>15}".format(year)
                e = self._function.E_cost(year, discount_rate)
                line = "{}{:>15.6}".format(line, e)
                print(line)
        else:
            raise ValueError("NOT SUPPORTED")

## PDF Recorder
#  This subclass represents the 'pdf' recorder.
#  It prints the IM identifier and PDF or normalised PDF (depending on the
#  presence of the 'normalise' flag), including column headers.
class PdfRecorder(recorder):
    ## Constructor
    # Just recorder.__init__.
    def __init__(self, id, type, function, options, columns, at):
        super().__init__(id, type, function, options, columns, at)

    ## Print the IM identifier and pdf
    #  including column headers.
    def generate_output(self):
        print("{:>15.6}{:>15.6}".format(self._function.get_IM().id(), "pdf"))
        results = []
        max_pdf = 0
        for im in self._at:
            pdf = self._function.pdf(im)
            results.append(pdf)
            if (pdf > max_pdf):
                max_pdf = pdf

        if self._options.get('normalise', None):
            divisor = max_pdf
        else:
            divisor = 1.0

        for i in range(len(self._at)):
            print("{:>15.6}{:>15.6}".format(self._at[i], results[i]/divisor))
        
## Factory method for recorders.
# Constructs an instance of a recorder or appropriate subclass.
# Parameters the same as for recorder.__init__.
# @return A recorder.
def MakeRecorder(id, type, function, options, columns, at):
    if (type == 'collrate'):
        return CollRateRecorder(id, type, function, options, columns, at)
    elif (type == 'structcost'):
        return StructcostRecorder(id, type, function, options, columns, at)
    elif (type == 'pdf'):
        return PdfRecorder(id, type, function, options, columns, at)
    else:
        return recorder(id, type, function, options, columns, at)
    
## Import a probability function from a file.
# This implements the 'importprobfn' command in the SLAT language.
# @param id The identifier for the new probability function.
# @param filename The name of the file containing the input data. Currently, 
#        the file must be in the format supported by the original SLAT code.
# @return A pyslat.probfn constructed using the data in the file.
def ImportProbFn(id, filename):
    filename = full_input_path(filename)
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

## Import an intensity measure from a file.
# This implements the 'importimfn' command in the SLAT language.
# @param id The identifier for the new intensity measure.
# @param filename The name of the file containing the input data. Currently, 
#        the file must be in the format supported by the original SLAT code.
# @return A pyslat.im constructed using the data in the file.
def ImportIMFn(id, filename):
    filename = full_input_path(filename)
    data = np.loadtxt(filename, skiprows=2)
    x = []
    y = []
    for d in data:
        x.append(d[0])
        y.append(d[1])
    im_func = detfn(None, 'loglog', [x.copy(), y.copy()])
    return(im(id, im_func))
