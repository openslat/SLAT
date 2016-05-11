#! /usr/bin/env python3

import sys
import io
import re
from antlr4 import *
from slatLexer import slatLexer
from slatParser import slatParser
from slatListener import slatListener
import glob
from distutils import text_file
import numbers
import  pyslat
import math
import numpy
from contextlib import redirect_stdout

def frange(start, stop, step):
    i = start
    while i <= stop:
        yield i
        i += step

class detfn:
    def __init__(self, id, type, parameters):
        print("CREATE a detfn '{}' of type {} from {}.".format(id, type, parameters))
        if type == 'power law':
            fntype = pyslat.FUNCTION_TYPE.PLC
        elif type == 'hyperbolic':
            fntype = pyslat.FUNCTION_TYPE.NLH
        else:
            raise ValueError("Invalid detfn type: {}".format(type))
            
        self._id = id
        self._type = type
        self._parameters = parameters.copy()
        self._func = pyslat.factory(fntype, parameters)

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
        print(("CREATE a probfn '{}' of type {}, using the deterministic function {} for {}, and " +
               "the deterministic function {} for {}.").format(
                   id, type,
                   mu_func[1].id(), mu_func[0],
                   sigma_func[1].id(), sigma_func[0]))
        self._id = id
        self._type = type
        self._mu_func = mu_func
        self._sigma_func = sigma_func
        self._func = pyslat.MakeLogNormalProbabilisticFn({mu_func[0]: mu_func[1].function(),
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

    def __str__(self):
        return(("Probabilistic function '{}' of type {}, using the deterministic function {} for {}, and " +
               "the deterministic function {} for {}.").format(
                   self._id, self._type,
                   self._mu_func[1].id(), self._mu_func[0],
                   self._sigma_func[1].id(), self._sigma_func[0]))

class im:
    def __init__(self, id, detfn):
        print("CREATE im {} from {}.".format(id, detfn.id()))
        self._id = id
        self._detfn = detfn
        self._func = pyslat.MakeSimpleRelationship(detfn.function())

    def id(self):
        return self._id

    def function(self):
        return self._func

    def getlambda(self, x):
        return self._func.getlambda(x)

    def __str__(self):
        return("Intensity measure '{}', based on the deterministic function '{}'.".format(self._id, self._detfn.id()))

class edp:
    def __init__(self, id, im, fn):
        print(("CREATE an EDP '{}' using the deterministic function {} and " +
               "the probabilistic function {}.").format(id, im.id(), fn.id()))
        self._id = id
        self._im = im
        self._fn = fn
        self._func = pyslat.MakeCompoundRelationship(im.function(), fn.function())
            
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

    def P_exceedence(self, x, y):
        return self._func.P_exceedence(x, y)

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
        print("CREATE a fragfn from db params")

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
        self._func = pyslat.MakeFragilityFn(params)
        print("CREATE a fragfn from log normal distributions")

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
        self._func = pyslat.MakeLossFn(params)
        print("CREATE Loss Function '{}' from {} as {}.".format(id, data, options))

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
        print("CREATE {}".format(self))
        self._func = pyslat.MakeCompGroup(edp.function(),
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

    def id(self):
        return(self._id)
        
    def __str__(self):
        return("Component Group '{}' using {}, {}, and {} ({} components".format(
            self._id, 
            self._edp.id(),
            self._frag.id(),
            self._loss.id(),
            self._count))
                 
class recorder:
    def __init__(self, type, function, options, columns, at):
        super().__init__()

        self._type = type
        self._function = function
        self._options = options
        self._at = at

        if (type =='dsedp' or type == 'dsim') and columns == None:
            columns = []
            # TODO: Get actual number of damage states
            for i in range(function.size()):
                columns.append(i + 1)
        elif (type == 'probfn' or type == 'edpim') and columns == None:
            columns = ['mean_x', 'sd_ln_x']
        self._columns = columns

    def __str__(self):
        return "Recorder: {} {} {} {} {}".format(self._type, self._function, self._options, self._columns, self._at)

    def generate_output(self):
        print(self)
        if self._type == 'dsrate':
            # TODO: How does this recorder work?
            print("DSRATE recorder not implemented")
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
                      'lossim': ['IM', None]}
            x_label = labels[self._type][0]
            y_label = labels[self._type][1]

            if not self._columns == None:
                line = "{:>15}".format(x_label)
                for y in self._columns:
                    line = "{}{:>15}".format(line, y)
            else:
                line = "{:>15}{:>15}".format(x_label, y_label)
            print(line)
                
            for x in self._at:
                line = "{:>15.6}".format(x)
                if self._type == 'dsedp':
                    yvals = self._function.fragfn().pExceeded(x)
                    for y in yvals:
                        line = "{}{:>15.6}".format(line, y)
                elif not self._columns == None:
                    line = "{:>15.6}".format(x)
                    for y in self._columns:
                        if isinstance(y, numbers.Number):
                            if isinstance(self._function, probfn):
                                yval = self._function.function().X_at_exceedence(x, y)
                            elif (isinstance(self._function, pyslat.CompoundRateRelationship) or
                                  isinstance(self._function, edp)):
                                yval = self._function.P_exceedence(x, y)
                            else:
                                yval = "----"
                        elif y == 'mean_x':
                            if self._type == 'lossedp':
                                yval = self._function.E_Loss_EDP(x)
                            elif self._type == 'lossim':
                                yval = self._function.E_Loss_IM(x)
                            else:
                                yval = self._function.Mean(x)
                        elif y == 'mean_ln_x':
                            yval = self._function.MeanLn(x)
                        elif y == 'median_x':
                            yval = self._function.Median(x)
                        elif y == 'sd_ln_x':
                            if self._type == 'lossedp':
                                yval = self._function.SD_ln_Loss_EDP(x)
                            elif self._type == 'lossim':
                                yval = self._function.SD_ln_Loss_IM(x)
                            else:
                                yval = self._function.SD_ln(x)
                        elif y == 'sd_x':
                            yval = self._function.SD(x)
                        else:
                            yval = "+++++++++"
                        line = "{}{:>15.6}".format(line, yval)
                else:
                    if isinstance(self._function, detfn):
                        yval = self._function.ValueAt(x)
                    elif (isinstance(self._function, im) or
                          isinstance(self._function, edp) or
                          isinstance(self._function, pyslat.RateRelationship) or
                          isinstance(self._function, pyslat.CompoundRateRelationship)):
                          yval = self._function.getlambda(x)
                    elif isinstance(self._function, pyslat.CompGroup):
                          yval = self._function.E_Loss_EDP(x)
                    else:
                        yval = "*****"
                    line = "{}{:>15.6}".format(line, yval)
                print(line)
                
            
    def run(self):
        destination = self._options and self._options.get('filename')
        if destination != None:
            if self._options.get('append'):
                f = open(destination, "a")
            else:
                f = open(destination, "w")
            with redirect_stdout(f):
                self.generate_output()

        else:
            self.generate_output()
    
        

class mySlatListener(slatListener):
    def __init__(self):
        super().__init__()
        self._stack = []
        self._stack_stack = []
        self._variables = dict()
        self._detfns = dict()
        self._probfns = dict()
        self._ims = dict()
        self._edps = dict()
        self._fragfns = dict()
        self._lossfns = dict()
        self._compgroups = dict()
        self._recorders = []

    def _push_stack(self):
        self._stack_stack.append(self._stack)
        self._stack = []

    def _pop_stack(self):
        result = self._stack
        self._stack = self._stack_stack.pop()
        return result
        
    # Enter a parse tree produced by slatParser#script.
    def enterScript(self, ctx:slatParser.ScriptContext):
        pass

    # Exit a parse tree produced by slatParser#script.
    def exitScript(self, ctx:slatParser.ScriptContext):
        pass

    # Enter a parse tree produced by slatParser#command.
    def enterCommand(self, ctx:slatParser.CommandContext):
        print()
        print("COMMAND: ", ctx.getText())
        self._stack = []
    
    # Exit a parse tree produced by slatParser#command.
    def exitCommand(self, ctx:slatParser.CommandContext):
        if len(self._stack) > 0:
            print("Error--stack not empty: ", self._stack)
        if len(self._stack_stack) > 0:
            print("Error--stack stack not empty: ", self._stack_stack)

    # Enter a parse tree produced by slatParser#title_command.
    def enterTitle_command(self, ctx:slatParser.Title_commandContext):
        pass

    # Exit a parse tree produced by slatParser#title_command.
    def exitTitle_command(self, ctx:slatParser.Title_commandContext):
        print("    Set the title to [" + ctx.STRING().getText().strip('\'"') + "].")

    # Enter a parse tree produced by slatParser#detfn_command.
    def enterDetfn_command(self, ctx:slatParser.Detfn_commandContext):
        pass
    
    # Exit a parse tree produced by slatParser#detfn_command.
    def exitDetfn_command(self, ctx:slatParser.Detfn_commandContext):
        if ctx.powerlaw_args():
            type = "power law"
            fntype = pyslat.FUNCTION_TYPE.PLC
        else:
            type = "hyperbolic"
            fntype = pyslat.FUNCTION_TYPE.NLH
        value = self._stack.pop()

        print("    Create a ", type, " function named ", ctx.ID(), 
              ", using the parameters: ", value)
        id = ctx.ID().getText()
        self._detfns[id] = detfn(id, type, value)


    # Enter a parse tree produced by slatParser#hyperbolic_args.
    def enterHyperbolic_args(self, ctx:slatParser.Hyperbolic_argsContext):
        pass

    # Exit a parse tree produced by slatParser#hyperbolic_args.
    def exitHyperbolic_args(self, ctx:slatParser.Hyperbolic_argsContext):
        pass


    # Enter a parse tree produced by slatParser#powerlaw_args.
    def enterPowerlaw_args(self, ctx:slatParser.Powerlaw_argsContext):
        pass

    # Exit a parse tree produced by slatParser#powerlaw_args.
    def exitPowerlaw_args(self, ctx:slatParser.Powerlaw_argsContext):
        pass

    # Enter a parse tree produced by slatParser#scalar2.
    def enterScalar(self, ctx:slatParser.ScalarContext):
        pass

    # Exit a parse tree produced by slatParser#scalar.
    def exitScalar(self, ctx:slatParser.ScalarContext):
        if ctx.STRING():
            value = ctx.STRING().getText()
        else:
            value = self._stack.pop()
        self._stack.append(value)

    # Enter a parse tree produced by slatParser#scalar2.
    def enterScalar2(self, ctx:slatParser.Scalar2Context):
        pass

    # Exit a parse tree produced by slatParser#scalar2.
    def exitScalar2(self, ctx:slatParser.Scalar2Context):
        value2 = self._stack.pop()
        value1 = self._stack.pop()
        self._stack.append([value1, value2])

    # Enter a parse tree produced by slatParser#scalar3.
    def enterScalar3(self, ctx:slatParser.Scalar3Context):
        pass

    # Exit a parse tree produced by slatParser#scalar3.
    def exitScalar3(self, ctx:slatParser.Scalar3Context):
        value3 = self._stack.pop()
        scalar2 = self._stack.pop()
        scalar2.append(value3)
        self._stack.append(scalar2)
        
    # Enter a parse tree produced by slatParser#var_ref.
    def enterVar_ref(self, ctx:slatParser.Var_refContext):
        pass

    # Exit a parse tree produced by slatParser#var_ref.
    def exitVar_ref(self, ctx:slatParser.Var_refContext):
        variable = ctx.ID().getText()
        value = self._variables.get(variable)
        self._stack.append(value)


    # Enter a parse tree produced by slatParser#numerical_scalar.
    def enterNumerical_scalar(self, ctx:slatParser.Numerical_scalarContext):
        pass

    # Exit a parse tree produced by slatParser#numerical_scalar.
    def exitNumerical_scalar(self, ctx:slatParser.Numerical_scalarContext):
        if ctx.INTEGER():
            value = int(ctx.INTEGER().getText())
        elif ctx.FLOAT_VAL():
            value = float(ctx.FLOAT_VAL().getText())
        elif ctx.var_ref() or ctx.python_script():
            value = self._stack.pop()
        else:
            value = "ERROR"
        self._stack.append(value)
        pass

    # Enter a parse tree produced by slatParser#parameters.
    def enterParameters(self, ctx:slatParser.ParametersContext):
        pass

    # Exit a parse tree produced by slatParser#parameters.
    def exitParameters(self, ctx:slatParser.ParametersContext):
        pass

    # Enter a parse tree produced by slatParser#parameter.
    def enterParameter(self, ctx:slatParser.ParameterContext):
        pass

    # Exit a parse tree produced by slatParser#parameter.
    def exitParameter(self, ctx:slatParser.ParameterContext):
        if ctx.ID():
            value = ctx.ID().getText()
        if ctx.STRING():
            value =  ctx.STRING().getText()
        if ctx.INTEGER():
            value = int(ctx.INTEGER().getText())
        if ctx.FLOAT_VAL():
            value = float(ctx.FLOAT_VAL().getText())
        self._stack.append(value)

    # Enter a parse tree produced by slatParser#parameter_array.
    def enterParameter_array(self, ctx:slatParser.Parameter_arrayContext):
        self._push_stack()

    # Exit a parse tree produced by slatParser#parameter_array.
    def exitParameter_array(self, ctx:slatParser.Parameter_arrayContext):
        values = self._pop_stack()
        self._stack.append(values)

    # Enter a parse tree produced by slatParser#parameter_dictionary.
    def enterParameter_dictionary(self, ctx:slatParser.Parameter_dictionaryContext):
        self._push_stack()

    # Exit a parse tree produced by slatParser#parameter_dictionary.
    def exitParameter_dictionary(self, ctx:slatParser.Parameter_dictionaryContext):
        values = dict()
        for value in self._stack:
            values[value[0]] = value[1]
        self._pop_stack()
        self._stack.append(values)

    # Enter a parse tree produced by slatParser#dictionary_entry.
    def enterDictionary_entry(self, ctx:slatParser.Dictionary_entryContext):
        pass

    # Exit a parse tree produced by slatParser#dictionary_entry.
    def exitDictionary_entry(self, ctx:slatParser.Dictionary_entryContext):
        key = (ctx.ID() or ctx.STRING()).getText()
        value = self._stack.pop()
        self._stack.append([key, value])

    # Enter a parse tree produced by slatParser#probfn_command.
    def enterProbfn_command(self, ctx:slatParser.Probfn_commandContext):
        pass

    # Exit a parse tree produced by slatParser#probfn_command.
    def exitProbfn_command(self, ctx:slatParser.Probfn_commandContext):
        id = ctx.ID().getText()
        refs = ctx.function_ref()
        mufn = (refs[0].ID() or refs[0].var_ref()).getText() 
        sigmafn = (refs[1].ID() or refs[1].var_ref()).getText()
        options = self._stack.pop()
        mu = options['mu']
        sd = options['sd']

        print(("    Create a probabilistic function '{}', using the "+
               "function '{}' for mu ({}), and the function " +
               "'{}' for sigma ({}).").format(id, mufn, mu, sigmafn, sd))
        self._probfns[id] = probfn(id, 'lognormal', [mu, self._detfns.get(mufn)], [sd, self._detfns.get(sigmafn)])


    # Enter a parse tree produced by slatParser#im_command.
    def enterIm_command(self, ctx:slatParser.Im_commandContext):
        pass

    # Exit a parse tree produced by slatParser#im_command.
    def exitIm_command(self, ctx:slatParser.Im_commandContext):
        im_id = ctx.ID(0).getText()
        fn_id = ctx.ID(1).getText()
        print(("    Create an impulse measurement '{}' from the deterministic function '{}'.").format(im_id, fn_id))
        self._ims[im_id] = im(im_id, self._detfns.get(fn_id))
        #print(self._ims)

    # Enter a parse tree produced by slatParser#edp_command.
    def enterEdp_command(self, ctx:slatParser.Edp_commandContext):
        pass

    # Exit a parse tree produced by slatParser#edp_command.
    def exitEdp_command(self, ctx:slatParser.Edp_commandContext):
        edp_id = ctx.ID(0).getText()
        im_id = ctx.ID(1).getText()
        fn_id = ctx.ID(2).getText()
        print(("    Create an engineering demand parameter '{}' from the impulse response '{}'" +
               " and the deterministic function '{}'.").format(edp_id, im_id, fn_id))
        self._edps[edp_id] = edp(edp_id, self._ims.get(im_id), self._probfns.get(fn_id))
        #print(self._edps)

    # Enter a parse tree produced by slatParser#fragfn_command.
    def enterFragfn_command(self, ctx:slatParser.Fragfn_commandContext):
        pass
        
    # Exit a parse tree produced by slatParser#fragfn_command.
    def exitFragfn_command(self, ctx:slatParser.Fragfn_commandContext):
        id = ctx.ID().getText()
        db_params = ctx.fragfn_db_params()
        if db_params:
            db_params = self._stack.pop()
            print(("    Define the fragility function named [{}], using "+
                   "the key [{}] in {}.").format(id, 
                                                 db_params['key'],
                                                 db_params['database']))
            self._fragfns[id] = fragfn_db(id, db_params)
        else:
            params = ctx.fragfn_user_defined_params()
            options = self._stack.pop()
            scalars = self._stack.pop()

            print("    Define the fragility function named [" + id + 
                  "] using the parameters below to represent ",
                  options)
            print("    ......", scalars)
            self._fragfns[id] = fragfn_user(id, options, scalars)
            #print(self._fragfns)
                

    # Enter a parse tree produced by slatParser#fragfn_db_params.
    def enterFragfn_db_params(self, ctx:slatParser.Fragfn_db_paramsContext):
        pass

    # Exit a parse tree produced by slatParser#fragfn_db_params.
    def exitFragfn_db_params(self, ctx:slatParser.Fragfn_db_paramsContext):
        dbkey = ctx.db_key().ID().getText().strip('\'"')
        
        if ctx.FILE_NAME():
            dbfile = ctx.FILE_NAME().getText().strip('\'"') 
        else:
            dbfile = "(default database)"
        self._stack.append({'key': dbkey, 'database': dbfile})

    # Enter a parse tree produced by slatParser#fragfn_user_defined_params.
    def enterFragfn_user_defined_params(self, ctx:slatParser.Fragfn_user_defined_paramsContext):
        pass
    
    # Exit a parse tree produced by slatParser#fragfn_user_defined_params.
    def exitFragfn_user_defined_params(self, ctx:slatParser.Fragfn_user_defined_paramsContext):
        pass

    # Enter a parse tree produced by slatParser#mu_option.
    def enterMu_option(self, ctx:slatParser.Mu_optionContext):
        pass

    # Exit a parse tree produced by slatParser#mu_option.
    def exitMu_option(self, ctx:slatParser.Mu_optionContext):
        pass

    # Enter a parse tree produced by slatParser#sd_option.
    def enterSd_option(self, ctx:slatParser.Sd_optionContext):
        pass

    # Exit a parse tree produced by slatParser#sd_option.
    def exitSd_option(self, ctx:slatParser.Sd_optionContext):
        pass

    # Enter a parse tree produced by slatParser#lognormal_options.
    def enterLognormal_options(self, ctx:slatParser.Lognormal_optionsContext):
        pass

    # Exit a parse tree produced by slatParser#lognormal_options.
    def exitLognormal_options(self, ctx:slatParser.Lognormal_optionsContext):
        if ctx.mu_option():
            if ctx.mu_option().MEAN_LN_X():
                mu = "mean(ln(x))"
                mu = pyslat.LOGNORMAL_PARAM_TYPE.MEAN_LN_X
            elif ctx.mu_option().MEDIAN_X():
                mu = "median(x)"
                mu = pyslat.LOGNORMAL_PARAM_TYPE.MEDIAN_X
            elif ctx.mu_option().MEAN_X():
                mu = "mean(x)"
                mu = pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X
            else:
                mu = "ERROR"
        else:
            mu = pyslat.LOGNORMAL_PARAM_TYPE.MEAN_LN_X


        if ctx.sd_option():
            if ctx.sd_option().SD_X():
                sd = "sd(x)"
                sd = pyslat.LOGNORMAL_PARAM_TYPE.SD_X
            elif ctx.sd_option().SD_LN_X():
                sd = "sd(ln(x))"
                sd = pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X
            else:
                sd = "ERROR"
        else:
            sd = pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X
            
        self._stack.append({"mu": mu, "sd":sd})
        pass

    # Enter a parse tree produced by slatParser#placement_type.
    def enterPlacement_type(self, ctx:slatParser.Placement_typeContext):
        pass

    # Exit a parse tree produced by slatParser#placement_type.
    def exitPlacement_type(self, ctx:slatParser.Placement_typeContext):
        pass

    # Enter a parse tree produced by slatParser#spread_type.
    def enterSpread_type(self, ctx:slatParser.Spread_typeContext):
        pass

    # Exit a parse tree produced by slatParser#spread_type.
    def exitSpread_type(self, ctx:slatParser.Spread_typeContext):
        pass

    # Enter a parse tree produced by slatParser#lognormal_dist.
    def enterLognormal_dist(self, ctx:slatParser.Lognormal_distContext):
        pass

    # Exit a parse tree produced by slatParser#lognormal_dist.
    def exitLognormal_dist(self, ctx:slatParser.Lognormal_distContext):
        pass

    # Enter a parse tree produced by slatParser#lognormal_parameter_array.
    def enterLognormal_parameter_array(self, ctx:slatParser.Lognormal_parameter_arrayContext):
        pass

    # Exit a parse tree produced by slatParser#lognormal_parameter_array.
    def exitLognormal_parameter_array(self, ctx:slatParser.Lognormal_parameter_arrayContext):
        pass

    # Enter a parse tree produced by slatParser#array_array.
    def enterArray_array(self, ctx:slatParser.Array_arrayContext):
        pass

    # Exit a parse tree produced by slatParser#array_array.
    def exitArray_array(self, ctx:slatParser.Array_arrayContext):
        pass

    # Enter a parse tree produced by slatParser#db_key.
    def enterDb_key(self, ctx:slatParser.Db_keyContext):
        pass

    # Exit a parse tree produced by slatParser#db_key.
    def exitDb_key(self, ctx:slatParser.Db_keyContext):
        pass

    # Enter a parse tree produced by slatParser#lossfn_command.
    def enterLossfn_command(self, ctx:slatParser.Lossfn_commandContext):
        pass
        
    # Exit a parse tree produced by slatParser#lossfn_command.
    def exitLossfn_command(self, ctx:slatParser.Lossfn_commandContext):
        id = ctx.ID().getText()
        if ctx.simple_loss_command():
            options = self._stack.pop()
            data = self._stack.pop()
            self._lossfns[id] = lossfn(id, options, data)
        else:
            print("ERRROR")
        #print(self._lossfns)

    # Enter a parse tree produced by slatParser#simple_loss_command.
    def enterSimple_loss_command(self, ctx:slatParser.Simple_loss_commandContext):
        pass

    # Exit a parse tree produced by slatParser#simple_loss_command.
    def exitSimple_loss_command(self, ctx:slatParser.Simple_loss_commandContext):
        pass
    
    # Enter a parse tree produced by slatParser#scalar2_sequence.
    def enterScalar2_sequence(self, ctx:slatParser.Scalar2_sequenceContext):
        self._push_stack()

    # Exit a parse tree produced by slatParser#scalar2_sequence.
    def exitScalar2_sequence(self, ctx:slatParser.Scalar2_sequenceContext):
        values = self._pop_stack()
        self._stack.append(values)

    # Enter a parse tree produced by slatParser#lossfn_heading.
    def enterLossfn_heading(self, ctx:slatParser.Lossfn_headingContext):
        pass
        
    # Exit a parse tree produced by slatParser#lossfn_heading.
    def exitLossfn_heading(self, ctx:slatParser.Lossfn_headingContext):
        pass

    # Enter a parse tree produced by slatParser#lossfn_headings.
    def enterLossfn_headings(self, ctx:slatParser.Lossfn_headingsContext):
        pass

    # Exit a parse tree produced by slatParser#lossfn_headings.
    def exitLossfn_headings(self, ctx:slatParser.Lossfn_headingsContext):
        pass

    # Enter a parse tree produced by slatParser#lossfn_anon_array.
    def enterLossfn_anon_array(self, ctx:slatParser.Lossfn_anon_arrayContext):
        pass

    # Exit a parse tree produced by slatParser#lossfn_anon_array.
    def exitLossfn_anon_array(self, ctx:slatParser.Lossfn_anon_arrayContext):
        pass

    # Enter a parse tree produced by slatParser#lossfn_dict.
    def enterLossfn_dict(self, ctx:slatParser.Lossfn_dictContext):
        pass

    # Exit a parse tree produced by slatParser#lossfn_dict.
    def exitLossfn_dict(self, ctx:slatParser.Lossfn_dictContext):
        pass

    # Enter a parse tree produced by slatParser#lossfn_named_array.
    def enterLossfn_named_array(self, ctx:slatParser.Lossfn_named_arrayContext):
        pass

    # Exit a parse tree produced by slatParser#lossfn_named_array.
    def exitLossfn_named_array(self, ctx:slatParser.Lossfn_named_arrayContext):
        pass

    # Enter a parse tree produced by slatParser#compgroup_command.
    def enterCompgroup_command(self, ctx:slatParser.Compgroup_commandContext):
        pass

    # Exit a parse tree produced by slatParser#compgroup_command.
    def exitCompgroup_command(self, ctx:slatParser.Compgroup_commandContext):
        compgroup_id = ctx.ID(0).getText()
        edp_id =  ctx.ID(1).getText()
        frag_id =  ctx.ID(2).getText()
        loss_id =  ctx.ID(3).getText()
        count = int(ctx.INTEGER().getText())
        print("    Create a group of", count, "components, called '" + compgroup_id +
              "', using the EDP '" + edp_id + "',", "the Fragility Function '" +
              frag_id + "', and the Loss Function '" + loss_id + "'.")
        self._compgroups[compgroup_id] = compgroup(compgroup_id,
                                                   self._edps.get(edp_id),
                                                   self._fragfns.get(frag_id),
                                                   self._lossfns.get(loss_id),
                                                   count)

    # Enter a parse tree produced by slatParser#print_command.
    def enterPrint_command(self, ctx:slatParser.Print_commandContext):
        pass

    # Exit a parse tree produced by slatParser#print_command.
    def exitPrint_command(self, ctx:slatParser.Print_commandContext):
        if ctx.print_options():
            options = self._stack.pop()
        else:
            options = None

        if ctx.print_message():
            object = self._stack.pop()
        elif ctx.print_function():
            id = ctx.print_function().ID().getText()
            
            fntype = ctx.print_function()
            if fntype.DETFN():
                object = self._detfns.get(id)
            elif fntype.PROBFN():
                object = self._probfns.get(id)
            elif fntype.IM():
                object = self._ims.get(id)
            elif fntype.EDP():
                object = self._edps.get(id)
            elif fntype.FRAGFN():
                object = self._fragfns.get(id)
            elif fntype.LOSSFN():
                object = self._lossfns.get(id)
            elif fntype.COMPGROUP():
                object = self._compgroups.get(id)
            else:
                object = 'unknown'
        message = "{}".format(object)

        destination = options and options.get('filename')
        if destination != None:
            if options.get('append'):
                f = open(destination, "a")
            else:
                f = open(destination, "w")
            with redirect_stdout(f):
                print(message)
        else:
            print(message)

    # Enter a parse tree produced by slatParser#print_message.
    def enterPrint_message(self, ctx:slatParser.Print_messageContext):
        pass

    # Exit a parse tree produced by slatParser#print_message.
    def exitPrint_message(self, ctx:slatParser.Print_messageContext):
        if ctx.python_script():
            object = self._stack.pop()
        elif ctx.var_ref():
            object = self._stack.pop()
        elif ctx.STRING():
            object = ctx.STRING().getText().strip('\'"')
        else:
            object = ""
        self._stack.append(object)

        
    # Enter a parse tree produced by slatParser#print_function.
    def enterPrint_function(self, ctx:slatParser.Print_functionContext):
        pass

    # Exit a parse tree produced by slatParser#print_function.
    def exitPrint_function(self, ctx:slatParser.Print_functionContext):
        pass

    # Enter a parse tree produced by slatParser#print_options.
    def enterPrint_options(self, ctx:slatParser.Print_optionsContext):
        pass

    # Exit a parse tree produced by slatParser#print_options.
    def exitPrint_options(self, ctx:slatParser.Print_optionsContext):
        options = dict()
        if ctx.FILE_NAME():
            options['filename'] = ctx.FILE_NAME().getText()
        else:
            options['filename'] = None;
            
        if ctx.APPEND_OPTION():
            options['append'] = True
        else:
            options['append'] = False

        self._stack.append(options)

    # Enter a parse tree produced by slatParser#integration_command.
    def enterIntegration_command(self, ctx:slatParser.Integration_commandContext):
        pass

    # Exit a parse tree produced by slatParser#integration_command.
    def exitIntegration_command(self, ctx:slatParser.Integration_commandContext):
        method = ctx.integration_method()
        if method.MAQ():
            methodstring = "MAQ"
        else:
            methodstring = "<unknown>"
        precision = self._stack.pop()
        
        if ctx.INTEGER():
            iterations = int(ctx.INTEGER().getText())
        else:
            iterations = self._stack.pop()
        print(("    Integrate using the {} algorithm, with precision " +
               "of {} and max iterations of {}.").format(
                   methodstring, 
                   precision,
                   iterations))

    # Enter a parse tree produced by slatParser#integration_method.
    def enterIntegration_method(self, ctx:slatParser.Integration_methodContext):
        pass

    # Exit a parse tree produced by slatParser#integration_method.
    def exitIntegration_method(self, ctx:slatParser.Integration_methodContext):
        pass

    # Enter a parse tree produced by slatParser#recorder_command.
    def enterRecorder_command(self, ctx:slatParser.Recorder_commandContext):
        pass
        
    # Exit a parse tree produced by slatParser#recorder_command.
    def exitRecorder_command(self, ctx:slatParser.Recorder_commandContext):
        if ctx.print_options():
            options = self._stack.pop()
            
            if options['filename']:
                destination = "the file [{}]".format(options['filename'])
                if options['append']:
                    destination = "appending to " + destination
                else:
                    destination = "overwriting " + destination + ", if it exists,"
            else:
                destination = "to standard output"
        else:
            options = None
            destination = "to standard output"

        if ctx.recorder_cols():
            cols = self._stack.pop()
            columns = " to the columns: "
            for col in cols:
                columns = "{} {}".format(columns, col)
        else:
            cols = None
            columns = " to the default columns"

        if ctx.recorder_at():
            at = self._stack.pop()
        else:
            at = None

        message = "    Record the "

        type = ctx.recorder_type()
        if type:
            type = type.getText()
        else:
            type = "dsrate"

        id = ctx.ID().getText()

        if type == "dsrate" or type == 'dsedp' or type == 'dsim':
            function = self._compgroups.get(id)
        elif type == 'detfn':
            function = self._detfns.get(id)
        elif type == 'probfn':
            function = self._probfns.get(id)
        elif type == 'imrate':
            function = self._ims.get(id)
        elif type == 'edpim' or type == 'edprate':
            function = self._edps.get(id)
        elif type == 'lossds' or type == 'lossedp' or type == 'lossim':
            function = self._compgroups.get(id)
        else:
            print("Error!")
        message = "{} [{}, {}]".format(message, type, function)
            
        message = message + " known as " + id
        if at != None:
            message =  "{}, at the values {}, ".format(message, at)
        else:
            message = message + " "

        message = message + destination + columns + "."
        print(message)
        self._recorders.append(recorder(type, function, options, cols, at))

    # Enter a parse tree produced by slatParser#recorder_type.
    def enterRecorder_type(self, ctx:slatParser.Recorder_typeContext):
        pass

    # Exit a parse tree produced by slatParser#recorder_type.
    def exitRecorder_type(self, ctx:slatParser.Recorder_typeContext):
        pass
    
    # Enter a parse tree produced by slatParser#recorder_at.
    def enterRecorder_at(self, ctx:slatParser.Recorder_atContext):
        pass

    # Exit a parse tree produced by slatParser#recorder_at.
    def exitRecorder_at(self, ctx:slatParser.Recorder_atContext):
        if ctx.FLOAT_VAL():
            floats = ctx.FLOAT_VAL()
            if len(floats) != 3:
                print("NEED EXACTLY THREE VALUES")
                return
            else:
                start = float(floats[0].getText())
                incr = float(floats[1].getText())
                end = float(floats[2].getText())
                self._stack.append(frange(start, end, incr))

    # Enter a parse tree produced by slatParser#float_array.
    def enterFloat_array(self, ctx:slatParser.Float_arrayContext):
        pass

    # Exit a parse tree produced by slatParser#float_array.
    def exitFloat_array(self, ctx:slatParser.Float_arrayContext):
        vals = []
        floats = ctx.FLOAT_VAL()
        for f in floats:
            vals.append(float(f.getText()))
        self._stack.append(vals)

    # Enter a parse tree produced by slatParser#col_spec.
    def enterCol_spec(self, ctx:slatParser.Col_specContext):
        pass

    # Exit a parse tree produced by slatParser#col_spec.
    def exitCol_spec(self, ctx:slatParser.Col_specContext):
        if ctx.placement_type():
            self._stack.append(ctx.placement_type().getText())
        elif ctx.spread_type():
            self._stack.append(ctx.spread_type().getText())
        # else scalar is already on the stack

    # Enter a parse tree produced by slatParser#recorder_cols.
    def enterRecorder_cols(self, ctx:slatParser.Recorder_colsContext):
        self._push_stack()

    # Exit a parse tree produced by slatParser#recorder_cols.
    def exitRecorder_cols(self, ctx:slatParser.Recorder_colsContext):
        values = self._pop_stack()
        self._stack.append(values)

    # Enter a parse tree produced by slatParser#python_script.
    def enterPython_script(self, ctx:slatParser.Python_scriptContext):
        pass

    # Exit a parse tree produced by slatParser#python_script.
    def exitPython_script(self, ctx:slatParser.Python_scriptContext):
        expression =  ctx.python_expression().getText()
        value = eval(expression, {"__builtins__": {}}, {"math":math, "numpy": numpy})
        print("Evaluatate the Python expression '{}' --> {})".format(expression, value))
        self._stack.append(value)

    # Enter a parse tree produced by slatParser#non_paren_expression.
    def enterNon_paren_expression(self, ctx:slatParser.Non_paren_expressionContext):
        pass

    # Exit a parse tree produced by slatParser#non_paren_expression.
    def exitNon_paren_expression(self, ctx:slatParser.Non_paren_expressionContext):
        pass

    # Enter a parse tree produced by slatParser#balanced_paren_expression.
    def enterBalanced_paren_expression(self, ctx:slatParser.Balanced_paren_expressionContext):
        pass

    # Exit a parse tree produced by slatParser#balanced_paren_expression.
    def exitBalanced_paren_expression(self, ctx:slatParser.Balanced_paren_expressionContext):
        pass

    # Enter a parse tree produced by slatParser#analyze_command.
    def enterAnalyze_command(self, ctx:slatParser.Analyze_commandContext):
        pass

    # Exit a parse tree produced by slatParser#analyze_command.
    def exitAnalyze_command(self, ctx:slatParser.Analyze_commandContext):
        print("Perform analysis:")
        for rec in self._recorders:
            rec.run()

    # Enter a parse tree produced by slatParser#set_command.
    def enterSet_command(self, ctx:slatParser.Set_commandContext):
        pass

    # Exit a parse tree produced by slatParser#set_command.
    def exitSet_command(self, ctx:slatParser.Set_commandContext):
        id = ctx.ID().getText()
        value = self._stack.pop()
        self._variables[id] = value
        print(("    Set the variable '{}' to {}.").format(id, value ))

def main(argv):
    for file in glob.glob('test_cases/*.lines'):
        listener = mySlatListener()
        print("-----------")
        print("File:", file)
        for test_case in text_file.TextFile(file).readlines():
            if not re.search('^%%', test_case):
                print()
                print("INPUT:", test_case)

                if False and re.search('\$\(', test_case):
                    print("SKIP")
                else:
                    input = InputStream(test_case)
                    lexer = slatLexer(input)
                    stream = CommonTokenStream(lexer)
                    parser = slatParser(stream)
                    tree = parser.script()
                    #print(tree.toStringTree(recog=parser))
                    #listener = mySlatListener()
                    walker = ParseTreeWalker()
                    walker.walk(listener, tree)

if __name__ == '__main__':
    main(sys.argv)
