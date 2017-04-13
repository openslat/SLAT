#! /usr/bin/env python3

## @file
# SLAT command language interpreter.

import sys
import io
import re
from antlr4 import *
from slatLexer import slatLexer
from slatParser import slatParser
from slatParserListener import slatParserListener
import glob
from distutils import text_file
import pyslat
import math
import numpy as np
from contextlib import redirect_stdout

## Pre-process input string
#
#  Escape quotes.
def preprocess_string(s):        
    return s.strip('\'"').replace('\\\"', '\"').replace('\\\'', '\'').replace('\\\\', '\\')

## Interpreter class
#
# Based on the slatParserListener class, created by antlr.
# @todo Explain basic operation of the parser. 
# @todo Separately, document how to modify the SLAT language
class SlatInterpreter(slatParserListener):
    ## @var _stack
    #  Stack to deal with nested lists
    #  @todo Improve comments about stack.

    ## @var _variables
    #  A dictionary of variables defined by the user.
    
    ## @var _title
    # A list of strings provided by the 'title' command.

    ## Constructor
    #  Call parent constructor, and clear local variables.
    def __init__(self):
        super().__init__()
        self._stack = []
        self._stack_stack = []
        self._variables = dict()
        self._title = []

    ## Push a value onto the local stack.
    def _push_stack(self):
        self._stack_stack.append(self._stack)
        self._stack = []

    ## Pop a value from the local stack.
    #  @return The value that *was* on top of the stack.
    def _pop_stack(self):
        result = self._stack
        self._stack = self._stack_stack.pop()
        return result

    ## Enter a parse tree produced by slatParser#command.
    def enterCommand(self, ctx:slatParser.CommandContext):
        self._stack = []
        #print(ctx.getText())
    
    ## Exit a parse tree produced by slatParser#command.
    def exitCommand(self, ctx:slatParser.CommandContext):
        if len(self._stack) > 0:
            raise  ValueError("Error--stack not empty: {}".format(self._stack))
        if len(self._stack_stack) > 0:
            raise ValueError("Error--stack stack not empty: {}".format(self._stack_stack))

    ## Exit a parse tree produced by slatParser#title_command.
    def exitTitle_command(self, ctx:slatParser.Title_commandContext):
        self._title.append(preprocess_string(ctx.STRING().getText()))

    ## Exit a parse tree produced by slatParser#detfn_command.
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
            type = 'lin'
            fntype = pyslat.FUNCTION_TYPE.LIN
        else:
            raise ValueError("Unhandled DETFN type.")
        value = self._stack.pop()

        id = ctx.ID().getText()
        print(type)
        if type == 'loglog' or type == 'lin':
            pyslat.detfn(id, type, value[0], value[1])
        else:
            pyslat.detfn(id, type, value)

    ## Enter a parse tree produced by slatParser#loglog
    def enterLoglog_args(self, ctx:slatParser.Loglog_argsContext):
        self._push_stack()

    ## Exit a parse tree produced by slatParser#loglog
    def exitLoglog_args(self, ctx:slatParser.Loglog_argsContext):
        values = self._pop_stack()
        # Put x and y values into separate lists:
        x_values = []
        y_values = []
        for pair in values:
            x_values.append(pair[0])
            y_values.append(pair[1])
        self._stack.append([x_values, y_values])

    ## Exit a parse tree produced by slatParser#scalar.
    def exitScalar(self, ctx:slatParser.ScalarContext):
        if ctx.STRING():
            value = preprocess_string(ctx.STRING().getText())
        else:
            value = self._stack.pop()
        self._stack.append(value)

    ## Exit a parse tree produced by slatParser#scalar2.
    def exitScalar2(self, ctx:slatParser.Scalar2Context):
        value2 = self._stack.pop()
        value1 = self._stack.pop()
        self._stack.append([value1, value2])

    ## Exit a parse tree produced by slatParser#scalar3.
    def exitScalar3(self, ctx:slatParser.Scalar3Context):
        value3 = self._stack.pop()
        scalar2 = self._stack.pop()
        scalar2.append(value3)
        self._stack.append(scalar2)
        
    ## Exit a parse tree produced by slatParser#var_ref.
    def exitVar_ref(self, ctx:slatParser.Var_refContext):
        variable = ctx.ID().getText()
        value = self._variables.get(variable)
        self._stack.append(value)

    ## Exit a parse tree produced by slatParser#numerical_scalar.
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

    ## Exit a parse tree produced by slatParser#parameter.
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

    ## Enter a parse tree produced by slatParser#parameter_array.
    def enterParameter_array(self, ctx:slatParser.Parameter_arrayContext):
        self._push_stack()

    ## Exit a parse tree produced by slatParser#parameter_array.
    def exitParameter_array(self, ctx:slatParser.Parameter_arrayContext):
        values = self._pop_stack()
        self._stack.append(values)

    ## Enter a parse tree produced by slatParser#parameter_dictionary.
    def enterParameter_dictionary(self, ctx:slatParser.Parameter_dictionaryContext):
        self._push_stack()

    ## Exit a parse tree produced by slatParser#parameter_dictionary.
    def exitParameter_dictionary(self, ctx:slatParser.Parameter_dictionaryContext):
        values = dict()
        for value in self._stack:
            values[value[0]] = value[1]
        self._pop_stack()
        self._stack.append(values)

    ## Exit a parse tree produced by slatParser#dictionary_entry.
    def exitDictionary_entry(self, ctx:slatParser.Dictionary_entryContext):
        key = (ctx.ID() or preprocess_string(ctx.STRING()).getText())
        value = self._stack.pop()
        self._stack.append([key, value])

    ## Exit a parse tree produced by slatParser#probfn_command.
    def exitProbfn_command(self, ctx:slatParser.Probfn_commandContext):
        id = ctx.ID().getText()
        refs = ctx.function_ref()
        mufn = (refs[0].ID() or refs[0].var_ref()).getText() 
        sigmafn = (refs[1].ID() or refs[1].var_ref()).getText()
        options = self._stack.pop()
        mu = options['mu']
        sd = options['sd']

        pyslat.probfn(id, 'lognormal', [mu, pyslat.detfn.lookup(mufn)], [sd, pyslat.detfn.lookup(sigmafn)])

    ## Exit a parse tree produced by slatParser#im_command.
    def exitIm_command(self, ctx:slatParser.Im_commandContext):
        im_id = ctx.ID(0).getText()
        fn_id = ctx.ID(1).getText()
        pyslat.im(im_id, pyslat.detfn.lookup(fn_id))

    def exitCollapse_command(self, ctx:slatParser.Collapse_commandContext):
        id = ctx.ID().getText()
        options = self._stack.pop()
        params = self._stack.pop()
        mu = params[0]
        sd = params[1]

        dist = pyslat.MakeLogNormalDist(mu, options['mu'], sd, options['sd'])
        pyslat.im.lookup(id).SetCollapse(dist)

    def exitDemolition_command(self, ctx:slatParser.Demolition_commandContext):
        id = ctx.ID().getText()
        options = self._stack.pop()
        params = self._stack.pop()
        mu = params[0]
        sd = params[1]

        dist = pyslat.MakeLogNormalDist(mu, options['mu'], sd, options['sd'])
        pyslat.im.lookup(id).SetDemolition(dist)

    ## Exit a parse tree produced by slatParser#edp_command.
    def exitEdp_command(self, ctx:slatParser.Edp_commandContext):
        edp_id = ctx.ID(0).getText()
        im_id = ctx.ID(1).getText()
        fn_id = ctx.ID(2).getText()
        pyslat.edp(edp_id, pyslat.im.lookup(im_id), pyslat.probfn.lookup(fn_id))

    ## Exit a parse tree produced by slatParser#fragfn_command.
    def exitFragfn_command(self, ctx:slatParser.Fragfn_commandContext):
        id = ctx.ID().getText()
        db_params = ctx.fragfn_db_params()
        if db_params:
            db_params = self._stack.pop()
            pyslat.fragfn_db(id, db_params)
        else:
            params = ctx.fragfn_user_defined_params()
            options = self._stack.pop()
            scalars = self._stack.pop()

            pyslat.fragfn_user(id, options, scalars)

    ## Exit a parse tree produced by slatParser#fragfn_db_params.
    def exitFragfn_db_params(self, ctx:slatParser.Fragfn_db_paramsContext):
        dbkey = ctx.db_key().ID().getText().strip('\'"')
        
        if ctx.file_spec():
            dbfile = (ctx.file_spec().FILE_NAME() or ctx.file_spec().ID()).getText().strip('\'"') 
        else:
            dbfile = "(default database)"
        self._stack.append({'key': dbkey, 'database': dbfile})

    ## Exit a parse tree produced by slatParser#lognormal_options.
    def exitLognormal_options(self, ctx:slatParser.Lognormal_optionsContext):
        if ctx.mu_option():
            if ctx.mu_option().MEAN_LN_X():
                mu = "mean(ln(x))"
                mu = pyslat.LOGNORMAL_MU_TYPE.MEAN_LN_X
            elif ctx.mu_option().MEDIAN_X():
                mu = "median(x)"
                mu = pyslat.LOGNORMAL_MU_TYPE.MEDIAN_X
            elif ctx.mu_option().MEAN_X():
                mu = "mean(x)"
                mu = pyslat.LOGNORMAL_MU_TYPE.MEAN_X
            else:
                raise ValueError("Unhandled Lognormal option")
        else:
            mu = None


        if ctx.sd_option():
            if ctx.sd_option().SD_X():
                sd = "sd(x)"
                sd = pyslat.LOGNORMAL_SIGMA_TYPE.SD_X
            elif ctx.sd_option().SD_LN_X():
                sd = "sd(ln(x))"
                sd = pyslat.LOGNORMAL_SIGMA_TYPE.SD_LN_X
            else:
                raise ValueError("Unhandled Lognormal option")
        else:
            sd = None
            
        self._stack.append({"mu": mu, "sd":sd})

    ## Exit a parse tree produced by slatParser#lossfn_command.
    def exitLossfn_command(self, ctx:slatParser.Lossfn_commandContext):
        id = ctx.ID().getText()
        if ctx.simple_loss_command():
            options = self._stack.pop()
            data = self._stack.pop()
            pyslat.lossfn(id, options, data)
        else:
            raise ValueError("Unhanlded type of lossfn")

    ## Enter a parse tree produced by slatParser#scalar2_sequence.
    def enterScalar2_sequence(self, ctx:slatParser.Scalar2_sequenceContext):
        self._push_stack()

    ## Exit a parse tree produced by slatParser#scalar2_sequence.
    def exitScalar2_sequence(self, ctx:slatParser.Scalar2_sequenceContext):
        values = self._pop_stack()
        self._stack.append(values)

    ## Exit a parse tree produced by slatParser#compgroup_command.
    def exitCompgroup_command(self, ctx:slatParser.Compgroup_commandContext):
        print(" > exitCompgroup_command()")
        compgroup_id = ctx.group_id().getText()
        edp_id =  ctx.edp_id().getText()
        frag_id =  ctx.frag_id().getText()
        cost_id =  ctx.cost_id().getText()
        delay_id = ctx.delay_id() and ctx.delay_id().getText()
        count = int(ctx.INTEGER().getText())
        pyslat.compgroup(compgroup_id,
                         pyslat.edp.lookup(edp_id),
                         pyslat.fragfn.lookup(frag_id),
                         pyslat.lossfn.lookup(cost_id),
                         delay_id and pyslat.lossfn.lookup(delay_id),
                         count)

    ## Exit a parse tree prodcued by slatParser#structure_command.
    def exitStructure_command(self, ctx:slatParser.Structure_commandContext):
        id = ctx.ID().getText();
        s = pyslat.structure(id)
        groups = []
        for g in ctx.id_list().ID():
            groups.append(g.getText())
            s.AddCompGroup(pyslat.compgroup.lookup(g.getText()))
        print("Create structure {} from {}.".format(id, groups));
        
    ## Exit a parse tree prodcued by slatParser#rebuild_cost_command.
    def exitRebuildcost_command(self, ctx:slatParser.Rebuildcost_commandContext):
        id = ctx.ID().getText()
        options = self._stack.pop()
        params = self._stack.pop()
        mu = params[0]
        sd = params[1]
        pyslat.structure.lookup(id).setRebuildCost(pyslat.MakeLogNormalDist(mu, options['mu'], sd, options['sd']))

    def exitDemolitioncost_command(self, ctx:slatParser.Demolitioncost_commandContext):
        id = ctx.ID().getText()
        options = self._stack.pop()
        params = self._stack.pop()
        mu = params[0]
        sd = params[1]
        pyslat.structure.lookup(id).setDemolitionCost(pyslat.MakeLogNormalDist(mu, options['mu'], sd, options['sd']))
        
    
    ## Exit a parse tree produced by slatParser#print_command.
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
                object = pyslat.detfn.lookup(id)
            elif fntype.PROBFN():
                object = pyslat.probfn.lookup(id)
            elif fntype.IM():
                object = pyslat.im.lookup(id)
            elif fntype.EDP():
                object = pyslat.edp.lookup(id)
            elif fntype.FRAGFN():
                object = pyslat.fragfn.lookup(id)
            elif fntype.LOSSFN():
                object = pyslat.lossfn.lookup(id)
            elif fntype.COMPGROUP():
                object = pyslat.compgroup.lookup(id)
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

    ## Exit a parse tree produced by slatParser#print_message.
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

        
    ## Exit a parse tree produced by slatParser#print_options.
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

    ## Exit a parse tree produced by slatParser#integration_command.
    def exitIntegration_command(self, ctx:slatParser.Integration_commandContext):
        if (ctx.integration_search_option()):
            if (ctx.integration_search_option().BINARY()):
                search_method = pyslat.INTEGRATION_METHOD.BINARY_SUBDIVISION
            elif ctx.integration_search_option().REVBIN():
                search_method = pyslat.INTEGRATION_METHOD.REVERSE_BINARY_SUBDIVISION
            elif ctx.integration_search_option().LOWREV():
                search_method = pyslat.INTEGRATION_METHOD.LOW_FIRST_REVERSE_BINARY_SUBDIVISION
            elif ctx.integration_search_option().SCATTERED():
                search_method = pyslat.INTEGRATION_METHOD.SCATTERED
            elif ctx.integration_search_option().DIRECTED():
                search_method = pyslat.INTEGRATION_METHOD.DIRECTED
            else:
                raise ValueError("Unrecognised search algorithm: " +
                                 ctx.integration_search_option().getText())
            pyslat.Set_Integration_Method(search_method)

            if ctx.integration_search_option().INTEGER():
                pyslat.Set_Integration_Search_Limit(ctx.integration_search_option().INTEGER().getText())
            else:
                pyslat.Set_Integration_Search_Limit(self._stack.pop())

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
        

    ## Exit a parse tree produced by slatParser#recorder_command.
    def exitRecorder_command(self, ctx:slatParser.Recorder_commandContext):
        recorder_id = ctx.recorder_id().getText()

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
        elif ctx.ANNCOST():
            type = "anncost"
        elif ctx.COSTRATE():
            type = 'costrate'
        elif ctx.TOTALCOST():
            type = 'totalcost'
        elif ctx.COLLAPSE():
            type = 'collapse'
        elif ctx.COLLRATE():
            type = 'collrate'
        elif ctx.STRUCTCOST():
            type = 'structcost'
        elif ctx.DEAGG():
            type = 'deagg'
        else:
            print("---------------")
            print(type)
            print("---------------")
            print(ctx.DSRATE())
            print("---------------")
            print(ctx.recorder_type())
            print("---------------")
            raise ValueError("Unhandled recorder type: {}".format(type))

        if type == 'structcost':
            if ctx.structcost_type():
                if ctx.structcost_type().BY_FATE_FLAG():
                    options['structcost-type'] = 'by-fate'
                elif ctx.structcost_type().BY_EDP_FLAG():
                    options['structcost-type'] = 'by-edp'
                if ctx.structcost_type().BY_FRAG_FLAG():
                    options['structcost-type'] = 'by-frag'
            if ctx.NPV_FLAG():
                options['structcost-type'] = 'npv'
                options['discount-rate'] = float(ctx.FLOAT_VAL().getText())
                first_year = int(ctx.INTEGER()[0].getText())
                increment = int(ctx.INTEGER()[1].getText())
                last_year = int(ctx.INTEGER()[2].getText())
                at = list(range(first_year, last_year, increment))
                
            elif ctx.ANNUAL_FLAG():
                options['structcost-type'] = 'annual'
                cols = []
                for key, value in self._stack.pop().items():
                    if key == 'mu':
                        if value == pyslat.LOGNORMAL_MU_TYPE.MEAN_LN_X:
                            c = "mean_ln_x"
                        elif value == pyslat.LOGNORMAL_MU_TYPE.MEAN_X:
                            c = "mean_x"
                        elif value == pyslat.LOGNORMAL_MU_TYPE.MEDIAN_X:
                            c = "median_x"
                        cols.append(c)
                    elif key == 'sd':
                        if value == pyslat.LOGNORMAL_SIGMA_TYPE.SD_LN_X:
                            c = "sd_ln_x"
                        elif value == pyslat.LOGNORMAL_SIGMA_TYPE.SD_X:
                            c = "sd_x"
                        cols.append(c)
                cols.sort()
                
        id = ctx.ID().getText()

        if type == "dsrate" or type == 'dsedp' or type == 'dsim':
            function = pyslat.compgroup.lookup(id)
        elif type == 'detfn':
            function = pyslat.detfn.lookup(id)
        elif type == 'probfn':
            function = pyslat.probfn.lookup(id)
        elif type == 'imrate' or type == 'collapse' or type == 'collrate':
            function = pyslat.im.lookup(id)
        elif type == 'edpim' or type == 'edprate':
            function = pyslat.edp.lookup(id)
        elif type == 'lossds' or type == 'costedp' or type == 'costim' \
             or type == 'delayedp' or type == 'delayim' \
             or type == 'anncost' \
             or type == 'costrate':
            function = pyslat.compgroup.lookup(id)
        elif type == 'structcost' or type == 'deagg' or type == 'totalcost':
            function = pyslat.structure.lookup(id)
        else:
            raise ValueError("Unhandled recorder type")

        pyslat.MakeRecorder(recorder_id, type, function, options, cols, at)

    ## Exit a parse tree produced by slatParser#recorder_at.
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
        
        
    ## Exit a parse tree produced by slatParser#float_array.
    def exitFloat_array(self, ctx:slatParser.Float_arrayContext):
        vals = []
        floats = ctx.FLOAT_VAL()
        for f in floats:
            vals.append(float(f.getText()))
        self._stack.append(vals)

    ## Exit a parse tree produced by slatParser#col_spec.
    def exitCol_spec(self, ctx:slatParser.Col_specContext):
        if ctx.placement_type():
            self._stack.append(ctx.placement_type().getText())
        elif ctx.spread_type():
            self._stack.append(ctx.spread_type().getText())
        # else scalar is already on the stack

    ## Enter a parse tree produced by slatParser#recorder_cols.
    def enterRecorder_cols(self, ctx:slatParser.Recorder_colsContext):
        self._push_stack()

    ## Exit a parse tree produced by slatParser#recorder_cols.
    def exitRecorder_cols(self, ctx:slatParser.Recorder_colsContext):
        values = self._pop_stack()
        self._stack.append(values)

    ## Exit a parse tree produced by slatParser#python_script.
    def exitPython_script(self, ctx:slatParser.Python_scriptContext):
        expression =  ctx.python_expression().getText()
        value = eval(expression, {"__builtins__": {}}, {"math":math, "numpy": np, "list":list, "map": map, "pyslat": pyslat})
        #print("Evaluatate the Python expression '{}' --> {})".format(expression, value))
        self._stack.append(value)
        #print(self._stack)

    ## Exit a parse tree produced by slatParser#analyze_command.
    def exitAnalyze_command(self, ctx:slatParser.Analyze_commandContext):
        print("Perform analysis:")
        for rec in pyslat.recorder.all():
            print(rec)
            rec.run()

    ## Exit a parse tree produced by slatParser#set_command.
    def exitSet_command(self, ctx:slatParser.Set_commandContext):
        id = ctx.ID().getText()
        value = self._stack.pop()
        self._variables[id] = value
        #print(("    Set the variable '{}' to {}.").format(id, value ))

    def exitImportprobfn_command(self, ctx:slatParser.Importprobfn_commandContext):
        id = ctx.ID().getText()
        filename = (ctx.file_spec().FILE_NAME() or ctx.file_spec().ID()).getText().strip('\'"')
        pyslat.ImportProbFn(id, filename)

    def exitImportimfn_command(self, ctx:slatParser.Importprobfn_commandContext):
        id = ctx.ID().getText()
        filename = (ctx.file_spec().FILE_NAME() or ctx.file_spec().ID()).getText().strip('\'"')
        pyslat.ImportIMFn(id, filename)

    def exitLog_command(self, ctx:slatParser.Log_commandContext):
        if (ctx.STDERR_FLAG()):
            pyslat.LogToStdErr(True)
        if (ctx.NOSTDERR_FLAG()):
            pyslat.LogToStdErr(False)
        if (ctx.file_spec()):
            pyslat.SetLogFile(ctx.file_spec().getText())

    def exitIntstats_command(self, ctx:slatParser.Intstats_commandContext):
        if ctx.PRINT():
            print(pyslat.Format_Statistics())
        if ctx.RESET():
            pyslat.Reset_Statistics()

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
        parser._errHandler = BailErrorStrategy()
        tree = parser.script()
        #print(tree.toStringTree(recog=parser))
        listener = SlatInterpreter()
        walker = ParseTreeWalker()
        walker.walk(listener, tree)

if __name__ == '__main__':
    main(sys.argv)
