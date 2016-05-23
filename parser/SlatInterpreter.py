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
import numpy as np
from contextlib import redirect_stdout

def frange(start, stop, step):
    i = start
    while i <= stop + step/2:
        yield i
        i += step

def preprocess_string(s):        
    return s.strip('\'"').replace('\\\"', '\"').replace('\\\'', '\'').replace('\\\\', '\\')

class detfn:
    def __init__(self, id, type, parameters):
        if type == 'power law':
            fntype = pyslat.FUNCTION_TYPE.PLC
        elif type == 'hyperbolic':
            fntype = pyslat.FUNCTION_TYPE.NLH
        elif type == 'loglog':
            fntype = pyslat.FUNCTION_TYPE.LOGLOG
        elif type == 'linear':
            fntype = pyslat.FUNCTION_TYPE.LIN
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
        self._func = pyslat.MakeCollapse(mu, sd)
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
        self._func = pyslat.MakeIM(detfn.function())

    def id(self):
        return self._id

    def function(self):
        return self._func

    def getlambda(self, x):
        return self._func.getlambda(x)

    def SetCollapse(self, collapse):
        self._collapse = collapse
        self._func.SetCollapse(collapse.func())

    def __str__(self):
        if self._collapse:
            return("Intensity measure '{}', based on the deterministic function '{}', with collapse function {}".format(self._id, self._detfn.id()), self._collapse)
        else:
            return("Intensity measure '{}', based on the deterministic function '{}'.".format(self._id, self._detfn.id()))

class edp:
    def __init__(self, id, im, fn):
        self._id = id
        self._im = im
        self._fn = fn
        self._func = pyslat.MakeEDP(im.function(), fn.function())
            
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
        self._func = pyslat.MakeFragilityFn(params)

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

    def E_loss(self, t, l):
        return self._func.E_loss(t, l)

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
                 
class recorder:
    def __init__(self, type, function, options, columns, at):
        super().__init__()

        self._type = type
        self._function = function
        self._options = options
        self._at = at

        if (type =='dsedp' or type == 'dsim') and columns == None:
            columns = []
            for i in range(function.size()):
                columns.append("DS{}".format(i + 1))
        elif (type == 'probfn' or type == 'edpim') and columns == None:
            columns = ['mean_ln_x', 'sd_ln_x']
        self._columns = columns

    def __str__(self):
        return "Recorder: {} {} {} {} {}".format(self._type, self._function, self._options, self._columns, self._at)

    def generate_output(self):
        #print(self)
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
                      'lossim': ['IM', None],
                      'annloss': ['t', ["E[ALt]"]],
                      'lossrate': ['t', 'Rate']}
        
            x_label = labels[self._type][0]
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
                elif self._type == 'annloss':
                    annual_loss = self._function.E_loss(int(x), self._options['lambda'])
                    line = "{}{:>15.6}".format(line, annual_loss)
                elif self._type == 'lossrate':
                    loss_rate = self._function.lambda_loss(x)
                    line = "{}{:>15.6}".format(line, loss_rate)
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
                          isinstance(self._function, pyslat.IM) or
                          isinstance(self._function, pyslat.EDP)):
                          yval = self._function.getlambda(x)
                    elif isinstance(self._function, pyslat.CompGroup):
                          yval = self._function.E_Loss_EDP(x)
                    else:
                        yval = "*****"
                    line = "{}{:>15.6}".format(line, yval)
                print(line)
                
            
    def run(self):
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
    
        

class SlatInterpreter(slatListener):
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
        self._title = []

    def _push_stack(self):
        self._stack_stack.append(self._stack)
        self._stack = []

    def _pop_stack(self):
        result = self._stack
        self._stack = self._stack_stack.pop()
        return result
        
    # Enter a parse tree produced by slatParser#command.
    def enterCommand(self, ctx:slatParser.CommandContext):
        self._stack = []
    
    # Exit a parse tree produced by slatParser#command.
    def exitCommand(self, ctx:slatParser.CommandContext):
        if len(self._stack) > 0:
            raise  ValueError("Error--stack not empty: {}".format(self._stack))
        if len(self._stack_stack) > 0:
            raise ValueError("Error--stack stack not empty: {}".format(self._stack_stack))

    # Exit a parse tree produced by slatParser#title_command.
    def exitTitle_command(self, ctx:slatParser.Title_commandContext):
        self._title.append(preprocess_string(ctx.STRING().getText()))

    # Exit a parse tree produced by slatParser#detfn_command.
    def exitDetfn_command(self, ctx:slatParser.Detfn_commandContext):
        if ctx.powerlaw_args():
            type = "power law"
            fntype = pyslat.FUNCTION_TYPE.PLC
        elif ctx.hyperbolic_args():
            type = "hyperbolic"
            fntype = pyslat.FUNCTION_TYPE.NLH
        elif ctx.loglog_args():
            type = 'loglog'
            fntype = pyslat.FUNCTION_TYPE.LOGLOG
        elif ctx.linear_args():
            type = 'log'
            fntype = pyslat.FUNCTION_TYPE.LIN
        else:
            raise ValueError("Unhandled DETFN type.")
        value = self._stack.pop()

        id = ctx.ID().getText()
        self._detfns[id] = detfn(id, type, value)

    def enterLoglog_args(self, ctx:slatParser.Loglog_argsContext):
        self._push_stack()

    def exitLoglog_args(self, ctx:slatParser.Loglog_argsContext):
        values = self._pop_stack()
        # Put x and y values into separate lists:
        x_values = []
        y_values = []
        for pair in values:
            x_values.append(pair[0])
            y_values.append(pair[1])
        self._stack.append([x_values, y_values])

    # Exit a parse tree produced by slatParser#scalar.
    def exitScalar(self, ctx:slatParser.ScalarContext):
        if ctx.STRING():
            value = preprocess_string(ctx.STRING().getText())
        else:
            value = self._stack.pop()
        self._stack.append(value)

    # Exit a parse tree produced by slatParser#scalar2.
    def exitScalar2(self, ctx:slatParser.Scalar2Context):
        value2 = self._stack.pop()
        value1 = self._stack.pop()
        self._stack.append([value1, value2])

    # Exit a parse tree produced by slatParser#scalar3.
    def exitScalar3(self, ctx:slatParser.Scalar3Context):
        value3 = self._stack.pop()
        scalar2 = self._stack.pop()
        scalar2.append(value3)
        self._stack.append(scalar2)
        
    # Exit a parse tree produced by slatParser#var_ref.
    def exitVar_ref(self, ctx:slatParser.Var_refContext):
        variable = ctx.ID().getText()
        value = self._variables.get(variable)
        self._stack.append(value)

    # Exit a parse tree produced by slatParser#numerical_scalar.
    def exitNumerical_scalar(self, ctx:slatParser.Numerical_scalarContext):
        if ctx.INTEGER():
            value = int(ctx.INTEGER().getText())
        elif ctx.FLOAT_VAL():
            value = float(ctx.FLOAT_VAL().getText())
        elif ctx.var_ref() or ctx.python_script():
            value = self._stack.pop()
        else:
            raise ValueError("Unhandled type of numerical scalar")
        self._stack.append(value)

    # Exit a parse tree produced by slatParser#parameter.
    def exitParameter(self, ctx:slatParser.ParameterContext):
        if ctx.ID():
            value = ctx.ID().getText()
        elif ctx.STRING():
            value =  preprocess_string(ctx.STRING().getText())
        elif ctx.INTEGER():
            value = int(ctx.INTEGER().getText())
        elif ctx.FLOAT_VAL():
            value = float(ctx.FLOAT_VAL().getText())
        else:
            raise ValueError("Unhandled Parameter type")
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

    # Exit a parse tree produced by slatParser#dictionary_entry.
    def exitDictionary_entry(self, ctx:slatParser.Dictionary_entryContext):
        key = (ctx.ID() or preprocess_string(ctx.STRING()).getText())
        value = self._stack.pop()
        self._stack.append([key, value])

    # Exit a parse tree produced by slatParser#probfn_command.
    def exitProbfn_command(self, ctx:slatParser.Probfn_commandContext):
        id = ctx.ID().getText()
        refs = ctx.function_ref()
        mufn = (refs[0].ID() or refs[0].var_ref()).getText() 
        sigmafn = (refs[1].ID() or refs[1].var_ref()).getText()
        options = self._stack.pop()
        mu = options['mu']
        sd = options['sd']

        self._probfns[id] = probfn(id, 'lognormal', [mu, self._detfns.get(mufn)], [sd, self._detfns.get(sigmafn)])

    # Exit a parse tree produced by slatParser#im_command.
    def exitIm_command(self, ctx:slatParser.Im_commandContext):
        im_id = ctx.ID(0).getText()
        fn_id = ctx.ID(1).getText()
        self._ims[im_id] = im(im_id, self._detfns.get(fn_id))

    def exitCollapse_command(self, ctx:slatParser.Collapse_commandContext):
        id = ctx.ID().getText()
        options = self._stack.pop()
        params = self._stack.pop()
        mu = params[0]
        sd = params[1]

        if not options['mu'] == pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X:
            raise ValueError("Mu option for collapse not yet supported")

        if not options['sd'] == pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X:
            raise ValueError("Sd option for collapse not yet supported")

        self._ims[id].SetCollapse(collapse("anonymous", mu, sd))
        print(self._ims)

    # Exit a parse tree produced by slatParser#edp_command.
    def exitEdp_command(self, ctx:slatParser.Edp_commandContext):
        edp_id = ctx.ID(0).getText()
        im_id = ctx.ID(1).getText()
        fn_id = ctx.ID(2).getText()
        self._edps[edp_id] = edp(edp_id, self._ims.get(im_id), self._probfns.get(fn_id))

    # Exit a parse tree produced by slatParser#fragfn_command.
    def exitFragfn_command(self, ctx:slatParser.Fragfn_commandContext):
        id = ctx.ID().getText()
        db_params = ctx.fragfn_db_params()
        if db_params:
            db_params = self._stack.pop()
            self._fragfns[id] = fragfn_db(id, db_params)
        else:
            params = ctx.fragfn_user_defined_params()
            options = self._stack.pop()
            scalars = self._stack.pop()

            self._fragfns[id] = fragfn_user(id, options, scalars)

    # Exit a parse tree produced by slatParser#fragfn_db_params.
    def exitFragfn_db_params(self, ctx:slatParser.Fragfn_db_paramsContext):
        dbkey = ctx.db_key().ID().getText().strip('\'"')
        
        if ctx.FILE_NAME():
            dbfile = ctx.FILE_NAME().getText().strip('\'"') 
        else:
            dbfile = "(default database)"
        self._stack.append({'key': dbkey, 'database': dbfile})

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
                raise ValueError("Unhandled Lognormal option")
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
                raise ValueError("Unhandled Lognormal option")
        else:
            sd = pyslat.LOGNORMAL_PARAM_TYPE.SD_LN_X
            
        self._stack.append({"mu": mu, "sd":sd})

    # Exit a parse tree produced by slatParser#lossfn_command.
    def exitLossfn_command(self, ctx:slatParser.Lossfn_commandContext):
        id = ctx.ID().getText()
        if ctx.simple_loss_command():
            options = self._stack.pop()
            data = self._stack.pop()
            self._lossfns[id] = lossfn(id, options, data)
        else:
            raise ValueError("Unhanlded type of lossfn")

    # Enter a parse tree produced by slatParser#scalar2_sequence.
    def enterScalar2_sequence(self, ctx:slatParser.Scalar2_sequenceContext):
        self._push_stack()

    # Exit a parse tree produced by slatParser#scalar2_sequence.
    def exitScalar2_sequence(self, ctx:slatParser.Scalar2_sequenceContext):
        values = self._pop_stack()
        self._stack.append(values)

    # Exit a parse tree produced by slatParser#compgroup_command.
    def exitCompgroup_command(self, ctx:slatParser.Compgroup_commandContext):
        compgroup_id = ctx.ID(0).getText()
        edp_id =  ctx.ID(1).getText()
        frag_id =  ctx.ID(2).getText()
        loss_id =  ctx.ID(3).getText()
        count = int(ctx.INTEGER().getText())
        self._compgroups[compgroup_id] = compgroup(compgroup_id,
                                                   self._edps.get(edp_id),
                                                   self._fragfns.get(frag_id),
                                                   self._lossfns.get(loss_id),
                                                   count)

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
        elif ctx.print_title():
            object = ""
            for t in self._title:
                object = "{}\n{}".format(object, t)
        else:
            raise ValueError("Unhandled print variation")
                
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

    # Exit a parse tree produced by slatParser#print_message.
    def exitPrint_message(self, ctx:slatParser.Print_messageContext):
        if ctx.python_script():
            object = self._stack.pop()
        elif ctx.var_ref():
            object = self._stack.pop()
        elif ctx.STRING():
            object = preprocess_string(ctx.STRING().getText())
        else:
            object = ""
        self._stack.append(object)

        
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

    # Exit a parse tree produced by slatParser#integration_command.
    def exitIntegration_command(self, ctx:slatParser.Integration_commandContext):
        method = ctx.integration_method()
        if method.MAQ():
            methodstring = "MAQ"
        else:
            raise ValueError("Unhandled integration type")
        precision = self._stack.pop()
        
        if ctx.INTEGER():
            iterations = int(ctx.INTEGER().getText())
        else:
            iterations = self._stack.pop()
        # TODO: Implement choice of methods
        pyslat.IntegrationSettings(precision, iterations);

    # Exit a parse tree produced by slatParser#recorder_command.
    def exitRecorder_command(self, ctx:slatParser.Recorder_commandContext):
        if ctx.print_options():
            options = self._stack.pop()
        else:
            options = dict()

        if ctx.recorder_cols():
            cols = self._stack.pop()
        else:
            cols = None

        if ctx.lambda_value():
            options['lambda'] = self._stack.pop()
            
        if ctx.recorder_at():
            at = self._stack.pop()
        else:
            at = None

        type = ctx.recorder_type()
        if type:
            type = type.getText()
        elif ctx.DSRATE():
            type = "dsrate"
        elif ctx.ANNLOSS():
            type = "annloss"
        elif ctx.LOSSRATE():
            type = 'lossrate'
        else:
            raise ValueError("Unhandled recorder type")

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
        elif type == 'lossds' or type == 'lossedp' or type == 'lossim' \
             or type == 'annloss' or type == 'lossrate':
            function = self._compgroups.get(id)
        else:
            raise ValueError("Unhandled recorder type")

        self._recorders.append(recorder(type, function, options, cols, at))

    # Exit a parse tree produced by slatParser#recorder_at.
    def exitRecorder_at(self, ctx:slatParser.Recorder_atContext):
        if ctx.FLOAT_VAL():
            floats = ctx.FLOAT_VAL()
            if len(floats) != 3:
                raise ValueError("Recorder 'at' specification requires exactly three values.")
            else:
                start = float(floats[0].getText())
                incr = float(floats[1].getText())
                end = float(floats[2].getText())
                self._stack.append(frange(start, end, incr))

    # Exit a parse tree produced by slatParser#float_array.
    def exitFloat_array(self, ctx:slatParser.Float_arrayContext):
        vals = []
        floats = ctx.FLOAT_VAL()
        for f in floats:
            vals.append(float(f.getText()))
        self._stack.append(vals)

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

    # Exit a parse tree produced by slatParser#python_script.
    def exitPython_script(self, ctx:slatParser.Python_scriptContext):
        expression =  ctx.python_expression().getText()
        value = eval(expression, {"__builtins__": {}}, {"math":math, "numpy": np})
        #print("Evaluatate the Python expression '{}' --> {})".format(expression, value))
        self._stack.append(value)

    # Exit a parse tree produced by slatParser#analyze_command.
    def exitAnalyze_command(self, ctx:slatParser.Analyze_commandContext):
        print("Perform analysis:")
        for rec in self._recorders:
            rec.run()

    # Exit a parse tree produced by slatParser#set_command.
    def exitSet_command(self, ctx:slatParser.Set_commandContext):
        id = ctx.ID().getText()
        value = self._stack.pop()
        self._variables[id] = value
        #print(("    Set the variable '{}' to {}.").format(id, value ))

    def exitImportprobfn_command(self, ctx:slatParser.Importprobfn_commandContext):
        data = np.loadtxt(ctx.FILE_NAME().getText().strip('\'"'), skiprows=2)
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
        mu_func = detfn("anonymous", 'linear', [x.copy(), mu.copy()])
        sigma_func = detfn("anonymous", 'linear', [x.copy(), sigma.copy()])
        id = ctx.ID().getText()
        self._probfns[id] = probfn(id, 'lognormal', 
                                   [pyslat.LOGNORMAL_PARAM_TYPE.MEAN_X, mu_func],
                                   [pyslat.LOGNORMAL_PARAM_TYPE.SD_X,  sigma_func])

def main(argv):
    for file in argv[1:]:
        print("File:", file)
        text = open(file).read()
        print()
        print("INPUT:", text)

        input = InputStream(text)
        lexer = slatLexer(input)
        stream = CommonTokenStream(lexer)
        parser = slatParser(stream)
        tree = parser.script()
        #print(tree.toStringTree(recog=parser))
        listener = SlatInterpreter()
        walker = ParseTreeWalker()
        walker.walk(listener, tree)

if __name__ == '__main__':
    main(sys.argv)
