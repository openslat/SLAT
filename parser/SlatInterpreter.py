#! /usr/bin/env python3

import sys
import io
import re
from antlr4 import *
from slatLexer import slatLexer
from slatParser import slatParser
from slatParserListener import slatParserListener
import glob
from distutils import text_file
import numbers
import pyslat
import math
import numpy as np
from contextlib import redirect_stdout

def preprocess_string(s):        
    return s.strip('\'"').replace('\\\"', '\"').replace('\\\'', '\'').replace('\\\\', '\\')

class recorder:
    def __init__(self, type, function, options, columns, at):
        super().__init__()

        self._type = type
        self._function = function
        self._options = options
        self._at = at

        if not type == 'dsrate' and not type == 'collrate' \
           and at==None:
            raise ValueError('MUST PROVIDE ''AT'' CLAUSE')

        if (type =='dsedp' or type == 'dsim') and columns == None:
            columns = []
            for i in range(function.size()):
                columns.append("DS{}".format(i + 1))
        elif (type == 'probfn' or type == 'edpim') and columns == None:
            columns = ['mean_ln_x', 'sd_ln_x']
        elif (type == 'structloss' or type == 'lossedp' or type =='lossim') \
             and columns == None:
            columns = ['mean_x', 'sd_ln_x']
        self._columns = columns

    def __str__(self):
        return "Recorder: {} {} {} {} {}".format(self._type, self._function, self._options, self._columns, self._at)

    def generate_output(self):
        #print(self)
        if self._type == 'dsrate':
            # TODO: How does this recorder work?
            print("DSRATE recorder not implemented")
        elif self._type == 'collrate':
            print("Rate of Collapse for IM {} is {}".format(self._function.id(), self._function.CollapseRate()))
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
                      'structloss': ['IM', None],
                      'annloss': ['t', ["E[ALt]"]],
                      'lossrate': ['t', 'Rate'],
                      'collapse': ['IM', 'p(Collapse)']}
        
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
                elif self._type == 'collapse':
                    p = self._function.pCollapse(x)
                    line = "{}{:>15.6}".format(line, p)
                elif not self._columns == None:
                    line = "{:>15.6}".format(x)
                    for y in self._columns:
                        if isinstance(y, numbers.Number):
                            if isinstance(self._function, pyslat.probfn):
                                yval = self._function.function().X_at_exceedence(x, y)
                            elif isinstance(self._function, pyslat.edp):
                                yval = self._function.X_at_exceedence(x, y)
                            else:
                                yval = "----"
                        elif y == 'mean_x':
                            if self._type == 'lossedp':
                                yval = self._function.E_Loss_EDP(x)
                            elif self._type == 'lossim':
                                yval = self._function.E_Loss_IM(x)
                            elif self._type == 'structloss':
                                yval = self._function.Loss(x, self._options['collapse']).mean()
                            else:
                                yval = self._function.Mean(x)
                        elif y == 'mean_ln_x':
                            if self._type == 'structloss':
                                yval = self._function.Loss(x, self._options['collapse']).mean_ln()
                            else:
                                yval = self._function.MeanLn(x)
                        elif y == 'median_x':
                            if self._type == 'structloss':
                                yval = self._function.Loss(x, self._options['collapse']).median()
                            else:
                                yval = self._function.Median(x)
                        elif y == 'sd_ln_x':
                            if self._type == 'lossedp':
                                yval = self._function.SD_ln_Loss_EDP(x)
                            elif self._type == 'lossim':
                                yval = self._function.SD_ln_Loss_IM(x)
                            elif self._type == 'structloss':
                                yval = self._function.Loss(x, self._options['collapse']).sd_ln()
                            else:
                                yval = self._function.SD_ln(x)
                        elif y == 'sd_x':
                            if self._type == 'structloss':
                                yval = self._function.Loss(x, self._options['collapse']).sd(x)
                            else:
                                yval = self._function.SD(x)
                        else:
                            yval = "+++++++++"
                        line = "{}{:>15.6}".format(line, yval)
                else:
                    if isinstance(self._function, pyslat.detfn):
                        yval = self._function.ValueAt(x)
                    elif (isinstance(self._function, pyslat.im) or
                          isinstance(self._function, pyslat.edp) or
                          isinstance(self._function, pyslat.IM) or
                          isinstance(self._function, pyslat.EDP)):
                          yval = self._function.getlambda(x)
                    elif isinstance(self._function, pyslat.compgroup):
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
    
        

class SlatInterpreter(slatParserListener):
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
        self._structures = dict()
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
        print(ctx.getText())
    
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
        self._detfns[id] = pyslat.detfn(id, type, value)

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

        self._probfns[id] = pyslat.probfn(id, 'lognormal', [mu, self._detfns.get(mufn)], [sd, self._detfns.get(sigmafn)])

    # Exit a parse tree produced by slatParser#im_command.
    def exitIm_command(self, ctx:slatParser.Im_commandContext):
        im_id = ctx.ID(0).getText()
        fn_id = ctx.ID(1).getText()
        self._ims[im_id] = pyslat.im(im_id, self._detfns.get(fn_id))

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

        self._ims[id].SetCollapse(pyslat.collapse("anonymous", mu, sd))

    # Exit a parse tree produced by slatParser#edp_command.
    def exitEdp_command(self, ctx:slatParser.Edp_commandContext):
        edp_id = ctx.ID(0).getText()
        im_id = ctx.ID(1).getText()
        fn_id = ctx.ID(2).getText()
        self._edps[edp_id] = pyslat.edp(edp_id, self._ims.get(im_id), self._probfns.get(fn_id))

    # Exit a parse tree produced by slatParser#fragfn_command.
    def exitFragfn_command(self, ctx:slatParser.Fragfn_commandContext):
        id = ctx.ID().getText()
        db_params = ctx.fragfn_db_params()
        if db_params:
            db_params = self._stack.pop()
            self._fragfns[id] = pyslat.fragfn_db(id, db_params)
        else:
            params = ctx.fragfn_user_defined_params()
            options = self._stack.pop()
            scalars = self._stack.pop()

            self._fragfns[id] = pyslat.fragfn_user(id, options, scalars)

    # Exit a parse tree produced by slatParser#fragfn_db_params.
    def exitFragfn_db_params(self, ctx:slatParser.Fragfn_db_paramsContext):
        dbkey = ctx.db_key().ID().getText().strip('\'"')
        
        if ctx.file_spec():
            dbfile = (ctx.file_spec().FILE_NAME() or ctx.file_spec().ID()).getText().strip('\'"') 
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
            self._lossfns[id] = pyslat.lossfn(id, options, data)
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
        self._compgroups[compgroup_id] = pyslat.compgroup(compgroup_id,
                                                   self._edps.get(edp_id),
                                                   self._fragfns.get(frag_id),
                                                   self._lossfns.get(loss_id),
                                                   count)

    def exitStructure_command(self, ctx:slatParser.Structure_commandContext):
        id = ctx.ID().getText();
        s = pyslat.structure()
        groups = []
        for g in ctx.id_list().ID():
            groups.append(g.getText())
            s.AddCompGroup(self._compgroups[g.getText()])
        print("Create structure {} from {}.".format(id, groups));
        self._structures[id] = s

    def exitRebuildcost_command(self, ctx:slatParser.Rebuildcost_commandContext):
        id = ctx.ID().getText()
        options = self._stack.pop()
        params = self._stack.pop()
        mu = params[0]
        sd = params[1]
        
        print("Rebuild cost for {}: {}, {}.".format(id, mu, sd))
        self._structures[id].setRebuildCost(pyslat.MakeLogNormalDist({options['mu']: mu, options['sd']: sd}))
        print(self._structures[id].getRebuildCost())
        print(self._structures[id].Loss(0.01, 0))
        print(self._structures[id].Loss(0.01, 1))
        print(self._structures[id].Loss(0.1106, 0))
        print(self._structures[id].Loss(0.1106, 1))
        print(self._structures[id].Loss(1.003, 0))
        print(self._structures[id].Loss(1.003, 1))

    
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
        if ctx.file_spec():
            options['filename'] = (ctx.file_spec().FILE_NAME() or ctx.file_spec().ID()).getText()
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
            print("AT: " )
            at = self._stack.pop()
            print(at)
        else:
            print("NO AT")
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
        elif ctx.COLLAPSE():
            type = 'collapse'
        elif ctx.COLLRATE():
            type = 'collrate'
        elif ctx.STRUCTLOSS():
            type = 'structloss'
        else:
            raise ValueError("Unhandled recorder type: {}")

        if type == 'structloss':
            if ctx.collapse_type() and \
               ctx.collapse_type().COLLAPSE_FLAG():
                options['collapse'] = True
            else:
                options['collapse'] = False
                
        id = ctx.ID().getText()

        if type == "dsrate" or type == 'dsedp' or type == 'dsim':
            function = self._compgroups.get(id)
        elif type == 'detfn':
            function = self._detfns.get(id)
        elif type == 'probfn':
            function = self._probfns.get(id)
        elif type == 'imrate' or type == 'collapse' or type == 'collrate':
            function = self._ims.get(id)
        elif type == 'edpim' or type == 'edprate':
            function = self._edps.get(id)
        elif type == 'lossds' or type == 'lossedp' or type == 'lossim' \
             or type == 'annloss' or type == 'lossrate':
            function = self._compgroups.get(id)
        elif type == 'structloss':
            function = self._structures.get(id)
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
                self._stack.append(pyslat.frange(start, end, incr))

    def exitCounted_at(self, ctx:slatParser.Counted_atContext):
        floats = ctx.FLOAT_VAL()
        start = float(floats[0].getText())
        end = float(floats[1].getText())
        count = int(ctx.INTEGER().getText())

        if ctx.LINFLAG():
            self._stack.append(pyslat.linrange(start, end, count))
        elif ctx.LOGFLAG():
            self._stack.append(pyslat.logrange(start, end, count))
        else:
            raise ValueError("Must specify --log or --linear")
        
        
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
        value = eval(expression, {"__builtins__": {}}, {"math":math, "numpy": np, "list":list, "map": map, "pyslat": pyslat})
        print("Evaluatate the Python expression '{}' --> {})".format(expression, value))
        self._stack.append(value)
        print(self._stack)

    # Exit a parse tree produced by slatParser#analyze_command.
    def exitAnalyze_command(self, ctx:slatParser.Analyze_commandContext):
        print("Perform analysis:")
        for rec in self._recorders:
            print(rec)
            rec.run()

    # Exit a parse tree produced by slatParser#set_command.
    def exitSet_command(self, ctx:slatParser.Set_commandContext):
        id = ctx.ID().getText()
        value = self._stack.pop()
        self._variables[id] = value
        print(("    Set the variable '{}' to {}.").format(id, value ))

    def exitImportprobfn_command(self, ctx:slatParser.Importprobfn_commandContext):
        data = np.loadtxt((ctx.file_spec().FILE_NAME() or ctx.file_spec().ID()).getText().strip('\'"'), skiprows=2)
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
        mu_func = pyslat.detfn("anonymous", 'linear', [x.copy(), mu.copy()])
        sigma_func = pyslat.detfn("anonymous", 'linear', [x.copy(), sigma.copy()])
        print("IMPORT")
        print(x)
        print(mu)
        print(sigma)
        id = ctx.ID().getText()
        self._probfns[id] = pyslat.probfn(id, 'lognormal', 
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
