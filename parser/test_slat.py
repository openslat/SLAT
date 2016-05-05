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

# http://www.antlr.org/wiki/display/ANTLR3/Python+runtime
# http://www.antlr.org/download/antlr-3.1.3.jar
class mySlatListener(slatListener):
    def _scalar_value(self, ctx:slatParser.ScalarContext):
        if ctx.INTEGER():
            return float(ctx.INTEGER().getText())
        if ctx.FLOAT_VAL():
            return float(ctx.FLOAT_VAL().getText())
        if ctx.var_ref():
            return "The value of the variable '" + ctx.var_ref().ID().getText() + "'"
        if ctx.python_script():
            return "The value of the Python expression '" + ctx.python_script().python_expression().getText() + "'"
        return 0

    def _scalar2_value(self, ctx:slatParser.Scalar2Context):
        return [ self._scalar_value(ctx.scalar(0)),
                 self._scalar_value(ctx.scalar(1))]
    def _scalar3_value(self, ctx:slatParser.Scalar3Context):
        result = self._scalar2_value(ctx.scalar2())
        result.append(self._scalar_value(ctx.scalar()))
        return result

    def _parameter_value(self, ctx:slatParser.ParameterContext):
        if ctx.ID():
            return ctx.ID().getText()
        if ctx.STRING():
            return ctx.STRING().getText()
        if ctx.INTEGER():
            return int(ctx.INTEGER().getText())
        if ctx.FLOAT_VAL():
            return float(ctx.FLOAT_VAL().getText())
        return "parameter"
        
    def _parameters_value(self, ctx:slatParser.ParametersContext):
        if ctx.parameter():
            return self._parameter_value(ctx.parameter())
        if ctx.parameter_array():
            values = ctx.parameter_array().parameter()
            result = []
            for value in values:
                result.append(self._parameter_value(value))
            return result
        if ctx.parameter_dictionary():
            values = ctx.parameter_dictionary().dictionary_entry()
            result = dict()
            for value in values:
                key = (value.ID() or value.STRING).getText()
                value = self._parameters_value(value.parameters())
                result[key] = value
            return result
        return "UNKNOWN"
    
    # Enter a parse tree produced by slatParser#script.
    def enterScript(self, ctx:slatParser.ScriptContext):
        pass #print("> script")

    # Exit a parse tree produced by slatParser#script.
    def exitScript(self, ctx:slatParser.ScriptContext):
        pass #print("< script")

    # Enter a parse tree produced by slatParser#command.
    def enterCommand(self, ctx:slatParser.CommandContext):
        pass #print("> Command")
    
    # Exit a parse tree produced by slatParser#command.
    def exitCommand(self, ctx:slatParser.CommandContext):
        pass #print("< Command")

    # Enter a parse tree produced by slatParser#title_command.
    def enterTitle_command(self, ctx:slatParser.Title_commandContext):
        pass #print("TitleCommand: ", ctx.STRING())

    # Exit a parse tree produced by slatParser#title_command.
    def exitTitle_command(self, ctx:slatParser.Title_commandContext):
        print("Set the title to [" + ctx.STRING().getText().strip('\'"') + "].")

    # Enter a parse tree produced by slatParser#detfn_command.
    def enterDetfn_command(self, ctx:slatParser.Detfn_commandContext):
        pass
    
    # Exit a parse tree produced by slatParser#detfn_command.
    def exitDetfn_command(self, ctx:slatParser.Detfn_commandContext):
        if ctx.powerlaw_args():
            type = "power law"
            value = self._scalar2_value(ctx.powerlaw_args().scalar2())
        else:
            type = "hyperbolic"
            value = self._scalar3_value(ctx.hyperbolic_args().scalar3())

        print("Create a ", type, " function named ", ctx.ID(), 
              ", using the parameters: ", value)


    # Enter a parse tree produced by slatParser#hyperbolic_args.
    def enterHyperbolic_args(self, ctx:slatParser.Hyperbolic_argsContext):
        pass #print("> Hyperbolic_args")

    # Exit a parse tree produced by slatParser#hyperbolic_args.
    def exitHyperbolic_args(self, ctx:slatParser.Hyperbolic_argsContext):
        pass #print("< Hyperbolic_args")


    # Enter a parse tree produced by slatParser#powerlaw_args.
    def enterPowerlaw_args(self, ctx:slatParser.Powerlaw_argsContext):
        pass #print("> Powerlaw_args")

    # Exit a parse tree produced by slatParser#powerlaw_args.
    def exitPowerlaw_args(self, ctx:slatParser.Powerlaw_argsContext):
        pass #print("< Powerlaw_args")

    # Enter a parse tree produced by slatParser#scalar2.
    def enterScalar(self, ctx:slatParser.ScalarContext):
        pass #print("> scalar: ", self._scalar_value(ctx))

    # Exit a parse tree produced by slatParser#scalar.
    def exitScalar(self, ctx:slatParser.ScalarContext):
        pass #print("< scalar: ", self._scalar_value(ctx))

    # Enter a parse tree produced by slatParser#scalar2.
    def enterScalar2(self, ctx:slatParser.Scalar2Context):
        pass #print("> Scalar2: ", self._scalar2_value(ctx))

    # Exit a parse tree produced by slatParser#scalar2.
    def exitScalar2(self, ctx:slatParser.Scalar2Context):
        pass #print("< Scalar2: ", self._scalar2_value(ctx))

    # Enter a parse tree produced by slatParser#scalar3.
    def enterScalar3(self, ctx:slatParser.Scalar3Context):
        pass #print("> Scalar3: ", self._scalar3_value(ctx))

    # Exit a parse tree produced by slatParser#scalar3.
    def exitScalar3(self, ctx:slatParser.Scalar3Context):
        pass #print("< Scalar3: ", self._scalar3_value(ctx))

    # Enter a parse tree produced by slatParser#var_ref.
    def enterVar_ref(self, ctx:slatParser.Var_refContext):
        pass #print("> var_ref")

    # Exit a parse tree produced by slatParser#var_ref.
    def exitVar_ref(self, ctx:slatParser.Var_refContext):
        pass #print("< var_ref")


    # Enter a parse tree produced by slatParser#numerical_scalar.
    def enterNumerical_scalar(self, ctx:slatParser.Numerical_scalarContext):
        print("> numerical_scalar")

    # Exit a parse tree produced by slatParser#numerical_scalar.
    def exitNumerical_scalar(self, ctx:slatParser.Numerical_scalarContext):
        print("< numerical_scalar")

    # Enter a parse tree produced by slatParser#parameters.
    def enterParameters(self, ctx:slatParser.ParametersContext):
        pass #print("> Parameters")

    # Exit a parse tree produced by slatParser#parameters.
    def exitParameters(self, ctx:slatParser.ParametersContext):
        pass #print("< Parameters")


    # Enter a parse tree produced by slatParser#parameter.
    def enterParameter(self, ctx:slatParser.ParameterContext):
        pass #print("> Parameter")

    # Exit a parse tree produced by slatParser#parameter.
    def exitParameter(self, ctx:slatParser.ParameterContext):
        pass #print("< Parameter")

    # Enter a parse tree produced by slatParser#parameter_array.
    def enterParameter_array(self, ctx:slatParser.Parameter_arrayContext):
        pass #print("> Parameter_array")

    # Exit a parse tree produced by slatParser#parameter_array.
    def exitParameter_array(self, ctx:slatParser.Parameter_arrayContext):
        pass #print("< Parameter_array")

    # Enter a parse tree produced by slatParser#parameter_dictionary.
    def enterParameter_dictionary(self, ctx:slatParser.Parameter_dictionaryContext):
        pass #print("> Parameter_dictionary")

    # Exit a parse tree produced by slatParser#parameter_dictionary.
    def exitParameter_dictionary(self, ctx:slatParser.Parameter_dictionaryContext):
        pass #print("< Parameter_dictionary")

    # Enter a parse tree produced by slatParser#dictionary_entry.
    def enterDictionary_entry(self, ctx:slatParser.Dictionary_entryContext):
        pass #print("> Dictionary_entry")

    # Exit a parse tree produced by slatParser#dictionary_entry.
    def exitDictionary_entry(self, ctx:slatParser.Dictionary_entryContext):
        pass #print("< Dictionary_entry")

    # Enter a parse tree produced by slatParser#probfn_command.
    def enterProbfn_command(self, ctx:slatParser.Probfn_commandContext):
        print("> Probfn_command")

    # Exit a parse tree produced by slatParser#probfn_command.
    def exitProbfn_command(self, ctx:slatParser.Probfn_commandContext):
        print("< Probfn_command")

    # Enter a parse tree produced by slatParser#im_command.
    def enterIm_command(self, ctx:slatParser.Im_commandContext):
        print("> Im_command")

    # Exit a parse tree produced by slatParser#im_command.
    def exitIm_command(self, ctx:slatParser.Im_commandContext):
        print("< Im_command")

    # Enter a parse tree produced by slatParser#edp_command.
    def enterEdp_command(self, ctx:slatParser.Edp_commandContext):
        print("> Edp_command")

    # Exit a parse tree produced by slatParser#edp_command.
    def exitEdp_command(self, ctx:slatParser.Edp_commandContext):
        print("< Edp_command")

    # Enter a parse tree produced by slatParser#fragfn_command.
    def enterFragfn_command(self, ctx:slatParser.Fragfn_commandContext):
        pass #print("> Fragfn_command")
        
    # Exit a parse tree produced by slatParser#fragfn_command.
    def exitFragfn_command(self, ctx:slatParser.Fragfn_commandContext):
        id = ctx.ID().getText()
        db_params = ctx.fragfn_db_params()
        if db_params:
            dbkey = db_params.db_key().ID().getText().strip('\'"')

            if db_params.FILE_NAME():
                dbfile = "the database [" + db_params.FILE_NAME().getText().strip('\'"') \
                         + "]"
            else:
                dbfile = "the standard database"
                
            print("Define the fragility function named [" + id + 
                  "] using the key [" + dbkey + "] in", dbfile, ".")
        else:
            params = ctx.fragfn_user_defined_params()
            options = params.lognormal_options()
            scalars = params.scalar2()

            if options.mu_option():
                if options.mu_option().getTokens(slatParser.MEAN_LN_X):
                    mu = "mean of the log of X"
                elif options.mu_option().getTokens(slatParser.MEDIAN_X):
                    mu = "median of X"
                elif options.mu_option().getTokens(slatParser.MEAN_X):
                    mu = "mean of X"
            else:
                mu = "(default)"
            
            if options.sd_option():
                if options.sd_option().getTokens(slatParser.SD_LN_X):
                    sd = "standard deviation of the log of X"
                elif options.sd_option().getTokens(slatParser.SD_X):
                    sd = "standard deviation of X"
            else:
                sd = "(default)"
            
            print("Define the fragility function named [" + id + 
                  "] using the parameters below to represent ",
                  "mu of", mu, "and sd of", sd, ":")
                  
            for s in scalars:
                print("......", self._scalar2_value(s))
                

    # Enter a parse tree produced by slatParser#fragfn_db_params.
    def enterFragfn_db_params(self, ctx:slatParser.Fragfn_db_paramsContext):
        pass #print("> Fragfn_db_params")

    # Exit a parse tree produced by slatParser#fragfn_db_params.
    def exitFragfn_db_params(self, ctx:slatParser.Fragfn_db_paramsContext):
        pass #print("< Fragfn_db_params")

    # Enter a parse tree produced by slatParser#fragfn_user_defined_params.
    def enterFragfn_user_defined_params(self, ctx:slatParser.Fragfn_user_defined_paramsContext):
        pass #print("> Fragfn_user_defined_params")

    # Exit a parse tree produced by slatParser#fragfn_user_defined_params.
    def exitFragfn_user_defined_params(self, ctx:slatParser.Fragfn_user_defined_paramsContext):
        pass #print("< Fragfn_user_defined_params")

    # Enter a parse tree produced by slatParser#mu_option.
    def enterMu_option(self, ctx:slatParser.Mu_optionContext):
        pass #print("> Mu_option")

    # Exit a parse tree produced by slatParser#mu_option.
    def exitMu_option(self, ctx:slatParser.Mu_optionContext):
        pass #print("< Mu_option")

    # Enter a parse tree produced by slatParser#sd_option.
    def enterSd_option(self, ctx:slatParser.Sd_optionContext):
        pass #print("> Sd_option")

    # Exit a parse tree produced by slatParser#sd_option.
    def exitSd_option(self, ctx:slatParser.Sd_optionContext):
        pass #print("< Sd_option")

    # Enter a parse tree produced by slatParser#lognormal_options.
    def enterLognormal_options(self, ctx:slatParser.Lognormal_optionsContext):
        pass #print("> Lognormal_options")

    # Exit a parse tree produced by slatParser#lognormal_options.
    def exitLognormal_options(self, ctx:slatParser.Lognormal_optionsContext):
        pass #print("< Lognormal_options")

    # Enter a parse tree produced by slatParser#placement_type.
    def enterPlacement_type(self, ctx:slatParser.Placement_typeContext):
        print("> placement_type")

    # Exit a parse tree produced by slatParser#placement_type.
    def exitPlacement_type(self, ctx:slatParser.Placement_typeContext):
        print("< placement_type")

    # Enter a parse tree produced by slatParser#spread_type.
    def enterSpread_type(self, ctx:slatParser.Spread_typeContext):
        print("> Spread_type")

    # Exit a parse tree produced by slatParser#spread_type.
    def exitSpread_type(self, ctx:slatParser.Spread_typeContext):
        print("< Spread_type")

    # Enter a parse tree produced by slatParser#lognormal_dist.
    def enterLognormal_dist(self, ctx:slatParser.Lognormal_distContext):
        print("> Lognormal_dist")

    # Exit a parse tree produced by slatParser#lognormal_dist.
    def exitLognormal_dist(self, ctx:slatParser.Lognormal_distContext):
        print("< Lognormal_dist")

    # Enter a parse tree produced by slatParser#lognormal_parameter_array.
    def enterLognormal_parameter_array(self, ctx:slatParser.Lognormal_parameter_arrayContext):
        print("> Lognormal_parameter_array")

    # Exit a parse tree produced by slatParser#lognormal_parameter_array.
    def exitLognormal_parameter_array(self, ctx:slatParser.Lognormal_parameter_arrayContext):
        print("< Lognormal_parameter_array")

    # Enter a parse tree produced by slatParser#array_array.
    def enterArray_array(self, ctx:slatParser.Array_arrayContext):
        print("> Array_array")

    # Exit a parse tree produced by slatParser#array_array.
    def exitArray_array(self, ctx:slatParser.Array_arrayContext):
        print("< Array_array")

    # Enter a parse tree produced by slatParser#db_key.
    def enterDb_key(self, ctx:slatParser.Db_keyContext):
        pass #print("> Db_key")

    # Exit a parse tree produced by slatParser#db_key.
    def exitDb_key(self, ctx:slatParser.Db_keyContext):
        pass #print("< Db_key")

    # Enter a parse tree produced by slatParser#lossfn_command.
    def enterLossfn_command(self, ctx:slatParser.Lossfn_commandContext):
        pass #print("> Lossfn_command")
        
    # Exit a parse tree produced by slatParser#lossfn_command.
    def exitLossfn_command(self, ctx:slatParser.Lossfn_commandContext):
        id = ctx.ID().getText()
        simple_command = ctx.simple_loss_command()
        scalars = simple_command.scalar2()
        options = simple_command.lognormal_options()
        mu_option = options.mu_option()
        if mu_option:
            if mu_option.MEAN_LN_X():
                mu_option = "Mean(ln(x))"
            elif mu_option.MEDIAN_X():
                mu_option = "Median(x)"
            elif mu_option.MEAN_X():
                mu_option = "Mean(x)"
        else:
            mu_option = "(default)"

        sd_option = options.sd_option()
        if sd_option:
            if sd_option.SD_LN_X():
                sd_option = "standard deviation(ln(x))"
            elif sd_option.SD_X():
                sd_option = "standard deviation(x)"
        else:
            sd_option = "(default)"

        print("Create a loss function called '" + id + "' from the following data,",
              "with mu=" + mu_option + ", and sd=" + sd_option + ":")
        for s in scalars:
            print("    ", self._scalar2_value(s))

    # Enter a parse tree produced by slatParser#simple_loss_command.
    def enterSimple_loss_command(self, ctx:slatParser.Simple_loss_commandContext):
        pass #print("> Simple_loss_command")

    # Exit a parse tree produced by slatParser#simple_loss_command.
    def exitSimple_loss_command(self, ctx:slatParser.Simple_loss_commandContext):
        pass #print("< Simple_loss_command")

    # Enter a parse tree produced by slatParser#lossfn_heading.
    def enterLossfn_heading(self, ctx:slatParser.Lossfn_headingContext):
        print("> Lossfn_heading")
        
    # Exit a parse tree produced by slatParser#lossfn_heading.
    def exitLossfn_heading(self, ctx:slatParser.Lossfn_headingContext):
        print("< Lossfn_heading")

    # Enter a parse tree produced by slatParser#lossfn_headings.
    def enterLossfn_headings(self, ctx:slatParser.Lossfn_headingsContext):
        print("> Lossfn_headings")

    # Exit a parse tree produced by slatParser#lossfn_headings.
    def exitLossfn_headings(self, ctx:slatParser.Lossfn_headingsContext):
        print("< Lossfn_headings")

    # Enter a parse tree produced by slatParser#lossfn_anon_array.
    def enterLossfn_anon_array(self, ctx:slatParser.Lossfn_anon_arrayContext):
        print("> Lossfn_anon_array")

    # Exit a parse tree produced by slatParser#lossfn_anon_array.
    def exitLossfn_anon_array(self, ctx:slatParser.Lossfn_anon_arrayContext):
        print("< Lossfn_anon_array")

    # Enter a parse tree produced by slatParser#lossfn_dict.
    def enterLossfn_dict(self, ctx:slatParser.Lossfn_dictContext):
        print("> Lossfn_dict")

    # Exit a parse tree produced by slatParser#lossfn_dict.
    def exitLossfn_dict(self, ctx:slatParser.Lossfn_dictContext):
        print("< Lossfn_dict")

    # Enter a parse tree produced by slatParser#lossfn_named_array.
    def enterLossfn_named_array(self, ctx:slatParser.Lossfn_named_arrayContext):
        print("> Lossfn_named_array")

    # Exit a parse tree produced by slatParser#lossfn_named_array.
    def exitLossfn_named_array(self, ctx:slatParser.Lossfn_named_arrayContext):
        print("< Lossfn_named_array")

    # Enter a parse tree produced by slatParser#compgroup_command.
    def enterCompgroup_command(self, ctx:slatParser.Compgroup_commandContext):
        pass #print("> Compgroup_command")

    # Exit a parse tree produced by slatParser#compgroup_command.
    def exitCompgroup_command(self, ctx:slatParser.Compgroup_commandContext):
        compgroup_id = ctx.ID(0).getText()
        edp_id =  ctx.ID(1).getText()
        frag_id =  ctx.ID(2).getText()
        loss_id =  ctx.ID(3).getText()
        count = int(ctx.INTEGER().getText())
        print("Create a group of", count, "components, called '" + compgroup_id +
              "', using the EDP '" + edp_id + "',", "the Fragility Function '" +
              frag_id + "', and the Loss Function '" + loss_id + "'.")

    # Enter a parse tree produced by slatParser#print_command.
    def enterPrint_command(self, ctx:slatParser.Print_commandContext):
        pass
        #print("> print_command")

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
            options = ctx.print_options()
            destination = "standard output"
            if options.FILE_NAME:
                destination = "the file [" + options.FILE_NAME().getText().strip('\'"') + "]"
                
            if options.APPEND_OPTION():
                special = ", appending to the file if it exists."
            elif options.NEW_OPTION():
                special = ", overwriting the file it it exists. "
            else:
                special = "."
        else:
            destination = "standard output"
            special = "."
                
        print("Print ", object, " to ", destination, special)
                

    # Enter a parse tree produced by slatParser#print_message.
    def enterPrint_message(self, ctx:slatParser.Print_messageContext):
        pass #print("> Print_message: ", ctx.STRING())

    # Exit a parse tree produced by slatParser#print_message.
    def exitPrint_message(self, ctx:slatParser.Print_messageContext):
        pass #print("< Print_message: ", ctx.STRING())

        
    # Enter a parse tree produced by slatParser#print_function.
    def enterPrint_function(self, ctx:slatParser.Print_functionContext):
        pass #print("> print_function: ", ctx.PRINT(), ", ", ctx.print_function_type().getText(), ", ", ctx.ID())

    # Exit a parse tree produced by slatParser#print_function.
    def exitPrint_function(self, ctx:slatParser.Print_functionContext):
        pass #print("< print_function: ", ctx.PRINT(), ", ", ctx.print_function_type().getText(), ", ", ctx.ID())

    # Enter a parse tree produced by slatParser#print_options.
    def enterPrint_options(self, ctx:slatParser.Print_optionsContext):
        pass #print("> Print_options: ", ctx.FILE_NAME(), ", ", ctx.APPEND_OPTION(), ", ", ctx.NEW_OPTION())

    # Exit a parse tree produced by slatParser#print_options.
    def exitPrint_options(self, ctx:slatParser.Print_optionsContext):
        pass #print("< Print_options: ", ctx.FILE_NAME(), ", ", ctx.APPEND_OPTION(), ", ", ctx.NEW_OPTION())

    # Enter a parse tree produced by slatParser#integration_command.
    def enterIntegration_command(self, ctx:slatParser.Integration_commandContext):
        print("> Integration_comnand")

    # Exit a parse tree produced by slatParser#integration_command.
    def exitIntegration_command(self, ctx:slatParser.Integration_commandContext):
        print("< Integration_comnand")

    # Enter a parse tree produced by slatParser#integration_method.
    def enterIntegration_method(self, ctx:slatParser.Integration_methodContext):
        print("> Integration_method")

    # Exit a parse tree produced by slatParser#integration_method.
    def exitIntegration_method(self, ctx:slatParser.Integration_methodContext):
        print("< Integration_method")

    # Enter a parse tree produced by slatParser#recorder_command.
    def enterRecorder_command(self, ctx:slatParser.Recorder_commandContext):
        print("> Recorder_command")
        
    # Exit a parse tree produced by slatParser#recorder_command.
    def exitRecorder_command(self, ctx:slatParser.Recorder_commandContext):
        print("< Recorder_command")

    # Enter a parse tree produced by slatParser#recorder_type.
    def enterRecorder_type(self, ctx:slatParser.Recorder_typeContext):
        print("> Recorder_type")

    # Exit a parse tree produced by slatParser#recorder_type.
    def exitRecorder_type(self, ctx:slatParser.Recorder_typeContext):
        print("< Recorder_type")

    # Enter a parse tree produced by slatParser#recorder_at.
    def enterRecorder_at(self, ctx:slatParser.Recorder_atContext):
        print("> Recorder_at")

    # Exit a parse tree produced by slatParser#recorder_at.
    def exitRecorder_at(self, ctx:slatParser.Recorder_atContext):
        print("< Recorder_at")

    # Enter a parse tree produced by slatParser#float_array.
    def enterFloat_array(self, ctx:slatParser.Float_arrayContext):
        print("> Float_array")

    # Exit a parse tree produced by slatParser#float_array.
    def exitFloat_array(self, ctx:slatParser.Float_arrayContext):
        print("< Float_array")

    # Enter a parse tree produced by slatParser#col_spec.
    def enterCol_spec(self, ctx:slatParser.Col_specContext):
        print("> Col_spec")

    # Exit a parse tree produced by slatParser#col_spec.
    def exitCol_spec(self, ctx:slatParser.Col_specContext):
        print("< Col_spec")

    # Enter a parse tree produced by slatParser#recorder_cols.
    def enterRecorder_cols(self, ctx:slatParser.Recorder_colsContext):
        print("> Recorder_cols")

    # Exit a parse tree produced by slatParser#recorder_cols.
    def exitRecorder_cols(self, ctx:slatParser.Recorder_colsContext):
        print("< Recorder_cols")

    # Enter a parse tree produced by slatParser#python_script.
    def enterPython_script(self, ctx:slatParser.Python_scriptContext):
        pass #print("> Python_script:", ctx.getText())

    # Exit a parse tree produced by slatParser#python_script.
    def exitPython_script(self, ctx:slatParser.Python_scriptContext):
        print("Python_script:", ctx.getText())

    # Enter a parse tree produced by slatParser#non_paren_expression.
    def enterNon_paren_expression(self, ctx:slatParser.Non_paren_expressionContext):
        pass #print("> Non_paren_expressoin")

    # Exit a parse tree produced by slatParser#non_paren_expression.
    def exitNon_paren_expression(self, ctx:slatParser.Non_paren_expressionContext):
        pass #print("< Non_paren_expressoin")

    # Enter a parse tree produced by slatParser#balanced_paren_expression.
    def enterBalanced_paren_expression(self, ctx:slatParser.Balanced_paren_expressionContext):
        pass #print("> Balanced_paren_expression")

    # Exit a parse tree produced by slatParser#balanced_paren_expression.
    def exitBalanced_paren_expression(self, ctx:slatParser.Balanced_paren_expressionContext):
        pass #print("< Balanced_paren_expression")

    # Enter a parse tree produced by slatParser#analyze_command.
    def enterAnalyze_command(self, ctx:slatParser.Analyze_commandContext):
        print("> Analyze_command")

    # Exit a parse tree produced by slatParser#analyze_command.
    def exitAnalyze_command(self, ctx:slatParser.Analyze_commandContext):
        print("< Analyze_command")

    # Enter a parse tree produced by slatParser#set_command.
    def enterSet_command(self, ctx:slatParser.Set_commandContext):
        pass #print("> Set_command")

    # Exit a parse tree produced by slatParser#set_command.
    def exitSet_command(self, ctx:slatParser.Set_commandContext):
        id = ctx.ID().getText()
        if ctx.python_script():
            value = "the value to the Python script '" + ctx.python_script().python_expression().getText() + "'"
        elif ctx.var_ref():
            value = "the value of the variable '" + ctx.var_ref().ID().getText() + "'"
        elif ctx.parameters():
            value = "PARAMETERS: {}".format(self._parameters_value(ctx.parameters()))
        print("Set the variable '" + id + "' to", value + ".")

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
                    print(tree.toStringTree(recog=parser))
                    listener = mySlatListener()
                    walker = ParseTreeWalker()
                    walker.walk(listener, tree)

if __name__ == '__main__':
    main(sys.argv)
