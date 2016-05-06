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

class mySlatListener(slatListener):
    def __init__(self):
        super().__init__()
        self._stack = []
        
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
            print("Stack: ", self._stack)

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
        else:
            type = "hyperbolic"
        value = self._stack.pop()

        print("    Create a ", type, " function named ", ctx.ID(), 
              ", using the parameters: ", value)


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
        self._stack.append("(evalate the variable {})".format(ctx.ID().getText()))


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
        self._stack.append("<START>")

    # Exit a parse tree produced by slatParser#parameter_array.
    def exitParameter_array(self, ctx:slatParser.Parameter_arrayContext):
        values = []
        value = self._stack.pop()
        while value != "<START>":
            values.append(value)
            value = self._stack.pop()
        values.reverse()
        self._stack.append(values)

    # Enter a parse tree produced by slatParser#parameter_dictionary.
    def enterParameter_dictionary(self, ctx:slatParser.Parameter_dictionaryContext):
        self._stack.append("<START>")

    # Exit a parse tree produced by slatParser#parameter_dictionary.
    def exitParameter_dictionary(self, ctx:slatParser.Parameter_dictionaryContext):
        values = dict()
        value = self._stack.pop()
        while value != "<START>":
            values[value[0]] = value[1]
            value = self._stack.pop()
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
        id = ctx.ID()
        refs = ctx.function_ref()
        mufn = (refs[0].ID() or refs[0].var_ref()).getText() 
        sigmafn = (refs[1].ID() or refs[1].var_ref()).getText()
        options = self._stack.pop()
        mu = options['mu']
        sd = options['sd']

        print(("    Create a probabilistic function '{}', using the "+
               "function '{}' for mu ({}), and the function " +
               "'{}' for sigma ({}).").format(id, mufn, mu, sigmafn, sd))

    # Enter a parse tree produced by slatParser#im_command.
    def enterIm_command(self, ctx:slatParser.Im_commandContext):
        pass

    # Exit a parse tree produced by slatParser#im_command.
    def exitIm_command(self, ctx:slatParser.Im_commandContext):
        im_id = ctx.ID(0).getText()
        fn_id = ctx.ID(1).getText()
        print(("    Create an impulse measurement '{}' from the deterministic function '{}'.").format(im_id, fn_id))

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
        else:
            params = ctx.fragfn_user_defined_params()
            options = self._stack.pop()
            scalars = self._stack.pop()

            print("    Define the fragility function named [" + id + 
                  "] using the parameters below to represent ",
                  options)
            print("    ......", scalars)
                

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
            elif ctx.mu_option().MEDIAN_X():
                mu = "median(x)"
            elif ctx.mu_option().MEAN_X():
                mu = "mean(x)"
            else:
                mu = "ERROR"
        else:
            mu = "default"

        if ctx.sd_option():
            if ctx.sd_option().SD_X():
                sd = "sd(x)"
            elif ctx.sd_option().SD_LN_X():
                sd = "sd(ln(x))"
            else:
                sd = "ERROR"
        else:
            sd = "default"
            
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
        command = self._stack.pop()
        print("    Create a LOSS FUNCTION '" + id + "' from " + command + ".")

    # Enter a parse tree produced by slatParser#simple_loss_command.
    def enterSimple_loss_command(self, ctx:slatParser.Simple_loss_commandContext):
        pass

    # Exit a parse tree produced by slatParser#simple_loss_command.
    def exitSimple_loss_command(self, ctx:slatParser.Simple_loss_commandContext):
        options = self._stack.pop()
        data = self._stack.pop()
        self._stack.append(("a simple loss command, data={}; options={}").format(data, options))
    
    # Enter a parse tree produced by slatParser#scalar2_sequence.
    def enterScalar2_sequence(self, ctx:slatParser.Scalar2_sequenceContext):
        self._stack.append("<START>")
        pass

    # Exit a parse tree produced by slatParser#scalar2_sequence.
    def exitScalar2_sequence(self, ctx:slatParser.Scalar2_sequenceContext):
        values = []
        value = self._stack.pop()
        while value != "<START>":
            values.append(value)
            value = self._stack.pop()
        values.reverse()
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

    # Enter a parse tree produced by slatParser#print_command.
    def enterPrint_command(self, ctx:slatParser.Print_commandContext):
        pass

    # Exit a parse tree produced by slatParser#print_command.
    def exitPrint_command(self, ctx:slatParser.Print_commandContext):
        if ctx.print_message():
            if ctx.print_message().STRING():
                object = "the message [" + ctx.print_message().STRING().getText().strip('\'"') + "]"
            else:
                object = "a blank line"
                
        elif ctx.print_function():
            fntype = ctx.print_function()
            if fntype.DETFN():
                object = 'deterministic function'
            elif fntype.PROBFN():
                object = 'probabilistic function'
            elif fntype.IM():
                object = 'intensity measure'
            elif fntype.EDP():
                object = 'engineering demand parameter'
            elif fntype.FRAGFN():
                object = 'fragility function'
            elif fntype.LOSSFN():
                object = 'loss function'
            elif fntype.COMPGROUP():
                object = 'component group'
            else:
                object = 'unknown'
            object = 'the ' + object + " known as " + ctx.print_function().ID().getText()

        if ctx.print_options():
            options = self._stack.pop()
            
            if options['filename']:
                destination = "the file [{}]".format(options['filename'])
                if options['append']:
                    destination = "appending to " + destination
                else:
                    destination = "overwriting " + destination + ", if it exists, "
            else:
                destination = "standard output"
        else:
            destination = "standard output"
                
        print("    Print ", object, " to ", destination)
                

    # Enter a parse tree produced by slatParser#print_message.
    def enterPrint_message(self, ctx:slatParser.Print_messageContext):
        pass

    # Exit a parse tree produced by slatParser#print_message.
    def exitPrint_message(self, ctx:slatParser.Print_messageContext):
        pass

        
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
            destination = "to standard output"

        if ctx.recorder_cols():
            cols = self._stack.pop()
            columns = " to the columns: "
            for col in cols:
                columns = "{} {}".format(columns, col)
        else:
            columns = " to the default columns"

        if ctx.recorder_at():
            at = self._stack.pop()
        else:
            at = None

        message = "    Record the "

        type = ctx.recorder_type()
        if type:
            message = message + self._stack.pop()
        else:
            message = message + "DS-rate relationship"
        
        id = ctx.ID().getText()
        message = message + " known as " + id
        if at:
            message =  "{}, at the values {}, ".format(message, at)
        else:
            message = message + " "

        message = message + destination + columns + "."

        print(message)

    # Enter a parse tree produced by slatParser#recorder_type.
    def enterRecorder_type(self, ctx:slatParser.Recorder_typeContext):
        pass

    # Exit a parse tree produced by slatParser#recorder_type.
    def exitRecorder_type(self, ctx:slatParser.Recorder_typeContext):
        type = "ERROR"
        if ctx.DETFN():
            value = "deterministic function"
        elif ctx.PROBFN():
            value = "probabilistic function"
        elif ctx.IMRATE():
            value = "IM-rate relationship"
        elif ctx.EDPIM():
            value = "EDP-IM relationship"
        elif ctx.EDPRATE():
            value = "EDP-rate relationship"
        elif ctx.DSEDP():
            value = "DS-EDP relationship"
        elif ctx.DSIM():
            value = "DS-IM relationship"
        elif ctx.LOSSDS():
            value = "LOSS-DS relationship"
        elif ctx.LOSSEDP():
            value = "LOSS-EDP relationship"
        elif ctx.LOSSIM():
            value = "LOSS-IM relationship"
        else:
            value = "ERROR"
        self._stack.append(value)

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
                self._stack.append({'from': start, 'to': end, 'by': incr})

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
        self._stack.append("<START>")

    # Exit a parse tree produced by slatParser#recorder_cols.
    def exitRecorder_cols(self, ctx:slatParser.Recorder_colsContext):
        values = []
        value = self._stack.pop()
        while value != "<START>":
            values.append(value)
            value = self._stack.pop()
        values.reverse()
        self._stack.append(values)

    # Enter a parse tree produced by slatParser#python_script.
    def enterPython_script(self, ctx:slatParser.Python_scriptContext):
        pass

    # Exit a parse tree produced by slatParser#python_script.
    def exitPython_script(self, ctx:slatParser.Python_scriptContext):
        self._stack.append("(evaluate the Python expression '{}')".format(ctx.python_expression().getText()))
        pass

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
        pass

    # Enter a parse tree produced by slatParser#set_command.
    def enterSet_command(self, ctx:slatParser.Set_commandContext):
        pass

    # Exit a parse tree produced by slatParser#set_command.
    def exitSet_command(self, ctx:slatParser.Set_commandContext):
        id = ctx.ID().getText()
        value = self._stack.pop()
        print(("    Set the variable '{}' to {}.").format(id, value ))

def main(argv):
    for file in glob.glob('test_cases/*.lines'):
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
                    listener = mySlatListener()
                    walker = ParseTreeWalker()
                    walker.walk(listener, tree)

if __name__ == '__main__':
    main(sys.argv)
