#!/usr/bin/env python3
import sys
import io
from antlr4 import *
from slatLexer import slatLexer
from slatParser import slatParser
from slatListener import slatListener

# http://www.antlr.org/wiki/display/ANTLR3/Python+runtime
# http://www.antlr.org/download/antlr-3.1.3.jar
class mySlatListener(slatListener):
    def _scalar_value(self, ctx:slatParser.ScalarContext):
        return (ctx.INTEGER() or ctx.FLOAT_VAL()).getText()

    def _scalar2_value(self, ctx:slatParser.Scalar2Context):
        return [ self._scalar_value(ctx.scalar(0)),
                 self._scalar_value(ctx.scalar(1))]
    def _scalar3_value(self, ctx:slatParser.Scalar3Context):
        result = self._scalar2_value(ctx.scalar2())
        result.append(self._scalar_value(ctx.scalar()))
        return result
    
    # Enter a parse tree produced by slatParser#script.
    def enterScript(self, ctx:slatParser.ScriptContext):
        print("> script")

    # Exit a parse tree produced by slatParser#script.
    def exitScript(self, ctx:slatParser.ScriptContext):
        print("< script")

    # Enter a parse tree produced by slatParser#command.
    def enterCommand(self, ctx:slatParser.CommandContext):
        print("> Command")

    # Exit a parse tree produced by slatParser#command.
    def exitCommand(self, ctx:slatParser.CommandContext):
        print("< Command")

    # Enter a parse tree produced by slatParser#title_command.
    def enterTitle_command(self, ctx:slatParser.Title_commandContext):
        print("TitleCommand: ", ctx.STRING())

    # Exit a parse tree produced by slatParser#title_command.
    def exitTitle_command(self, ctx:slatParser.Title_commandContext):
        print("< TitleCommand")

    # Enter a parse tree produced by slatParser#detfn_command.
    def enterDetfn_command(self, ctx:slatParser.Detfn_commandContext):
        if ctx.powerlaw_args():
            type = "power law"
            value = self._scalar2_value(ctx.powerlaw_args().scalar2())
        else:
            type = "hyperbolic"
            value = self._scalar3_value(ctx.hyperbolic_args().scalar3())

        print("> detfn_command: ", type, value)

    # Exit a parse tree produced by slatParser#detfn_command.
    def exitDetfn_command(self, ctx:slatParser.Detfn_commandContext):
        print("< detfn_command")

    # Enter a parse tree produced by slatParser#hyperbolic_args.
    def enterHyperbolic_args(self, ctx:slatParser.Hyperbolic_argsContext):
        print("> Hyperbolic_args")

    # Exit a parse tree produced by slatParser#hyperbolic_args.
    def exitHyperbolic_args(self, ctx:slatParser.Hyperbolic_argsContext):
        print("< Hyperbolic_args")


    # Enter a parse tree produced by slatParser#powerlaw_args.
    def enterPowerlaw_args(self, ctx:slatParser.Powerlaw_argsContext):
        print("> Powerlaw_args")

    # Exit a parse tree produced by slatParser#powerlaw_args.
    def exitPowerlaw_args(self, ctx:slatParser.Powerlaw_argsContext):
        print("< Powerlaw_args")

    # Enter a parse tree produced by slatParser#scalar2.
    def enterScalar(self, ctx:slatParser.ScalarContext):
        print("> scalar: ", self._scalar_value(ctx))

    # Exit a parse tree produced by slatParser#scalar.
    def exitScalar(self, ctx:slatParser.ScalarContext):
        print("< scalar: ", self._scalar_value(ctx))

    # Enter a parse tree produced by slatParser#scalar2.
    def enterScalar2(self, ctx:slatParser.Scalar2Context):
        print("> Scalar2: ", self._scalar2_value(ctx))

    # Exit a parse tree produced by slatParser#scalar2.
    def exitScalar2(self, ctx:slatParser.Scalar2Context):
        print("< Scalar2: ", self._scalar2_value(ctx))

    # Enter a parse tree produced by slatParser#scalar3.
    def enterScalar3(self, ctx:slatParser.Scalar3Context):
        print("> Scalar3: ", self._scalar3_value(ctx))

    # Exit a parse tree produced by slatParser#scalar3.
    def exitScalar3(self, ctx:slatParser.Scalar3Context):
        print("< Scalar3: ", self._scalar3_value(ctx))

    # Enter a parse tree produced by slatParser#var_ref.
    def enterVar_ref(self, ctx:slatParser.Var_refContext):
        print("> var_ref")

    # Exit a parse tree produced by slatParser#var_ref.
    def exitVar_ref(self, ctx:slatParser.Var_refContext):
        print("< var_ref")


    # Enter a parse tree produced by slatParser#numerical_scalar.
    def enterNumerical_scalar(self, ctx:slatParser.Numerical_scalarContext):
        print("> numerical_scalar")

    # Exit a parse tree produced by slatParser#numerical_scalar.
    def exitNumerical_scalar(self, ctx:slatParser.Numerical_scalarContext):
        print("< numerical_scalar")

    # Enter a parse tree produced by slatParser#parameters.
    def enterParameters(self, ctx:slatParser.ParametersContext):
        print("> Parameters")

    # Exit a parse tree produced by slatParser#parameters.
    def exitParameters(self, ctx:slatParser.ParametersContext):
        print("< Parameters")


    # Enter a parse tree produced by slatParser#parameter.
    def enterParameter(self, ctx:slatParser.ParameterContext):
        print("> Parameter")

    # Exit a parse tree produced by slatParser#parameter.
    def exitParameter(self, ctx:slatParser.ParameterContext):
        print("< Parameter")

    # Enter a parse tree produced by slatParser#parameter_array.
    def enterParameter_array(self, ctx:slatParser.Parameter_arrayContext):
        print("> Parameter_array")

    # Exit a parse tree produced by slatParser#parameter_array.
    def exitParameter_array(self, ctx:slatParser.Parameter_arrayContext):
        print("< Parameter_array")

    # Enter a parse tree produced by slatParser#parameter_dictionary.
    def enterParameter_dictionary(self, ctx:slatParser.Parameter_dictionaryContext):
        print("> Parameter_dictionary")

    # Exit a parse tree produced by slatParser#parameter_dictionary.
    def exitParameter_dictionary(self, ctx:slatParser.Parameter_dictionaryContext):
        print("< Parameter_dictionary")

    # Enter a parse tree produced by slatParser#dictionary_entry.
    def enterDictionary_entry(self, ctx:slatParser.Dictionary_entryContext):
        print("> Dictionary_entry")

    # Exit a parse tree produced by slatParser#dictionary_entry.
    def exitDictionary_entry(self, ctx:slatParser.Dictionary_entryContext):
        print("< Dictionary_entry")

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
        print("> Fragfn_command")

    # Exit a parse tree produced by slatParser#fragfn_command.
    def exitFragfn_command(self, ctx:slatParser.Fragfn_commandContext):
        print("< Fragfn_command")

    # Enter a parse tree produced by slatParser#fragfn_db_params.
    def enterFragfn_db_params(self, ctx:slatParser.Fragfn_db_paramsContext):
        print("> Fragfn_db_params")

    # Exit a parse tree produced by slatParser#fragfn_db_params.
    def exitFragfn_db_params(self, ctx:slatParser.Fragfn_db_paramsContext):
        print("< Fragfn_db_params")

    # Enter a parse tree produced by slatParser#fragfn_user_defined_params.
    def enterFragfn_user_defined_params(self, ctx:slatParser.Fragfn_user_defined_paramsContext):
        print("> Fragfn_user_defined_params")

    # Exit a parse tree produced by slatParser#fragfn_user_defined_params.
    def exitFragfn_user_defined_params(self, ctx:slatParser.Fragfn_user_defined_paramsContext):
        print("< Fragfn_user_defined_params")

    # Enter a parse tree produced by slatParser#mu_option.
    def enterMu_option(self, ctx:slatParser.Mu_optionContext):
        print("> Mu_option")

    # Exit a parse tree produced by slatParser#mu_option.
    def exitMu_option(self, ctx:slatParser.Mu_optionContext):
        print("< Mu_option")

    # Enter a parse tree produced by slatParser#sd_option.
    def enterSd_option(self, ctx:slatParser.Sd_optionContext):
        print("> Sd_option")

    # Exit a parse tree produced by slatParser#sd_option.
    def exitSd_option(self, ctx:slatParser.Sd_optionContext):
        print("< Sd_option")

    # Enter a parse tree produced by slatParser#lognormal_options.
    def enterLognormal_options(self, ctx:slatParser.Lognormal_optionsContext):
        print("> Lognormal_options")

    # Exit a parse tree produced by slatParser#lognormal_options.
    def exitLognormal_options(self, ctx:slatParser.Lognormal_optionsContext):
        print("< Lognormal_options")

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
        print("> Db_key")

    # Exit a parse tree produced by slatParser#db_key.
    def exitDb_key(self, ctx:slatParser.Db_keyContext):
        print("< Db_key")

    # Enter a parse tree produced by slatParser#lossfn_command.
    def enterLossfn_command(self, ctx:slatParser.Lossfn_commandContext):
        print("> Lossfn_command")

    # Exit a parse tree produced by slatParser#lossfn_command.
    def exitLossfn_command(self, ctx:slatParser.Lossfn_commandContext):
        print("< Lossfn_command")

    # Enter a parse tree produced by slatParser#simple_loss_command.
    def enterSimple_loss_command(self, ctx:slatParser.Simple_loss_commandContext):
        print("> Simple_loss_command")

    # Exit a parse tree produced by slatParser#simple_loss_command.
    def exitSimple_loss_command(self, ctx:slatParser.Simple_loss_commandContext):
        print("< Simple_loss_command")

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
        print("> Compgroup_command")

    # Exit a parse tree produced by slatParser#compgroup_command.
    def exitCompgroup_command(self, ctx:slatParser.Compgroup_commandContext):
        print("< Compgroup_command")

    # Enter a parse tree produced by slatParser#print_command.
    def enterPrint_command(self, ctx:slatParser.Print_commandContext):
        print("> Print_command")
        #print("> Print_command: ", ctx.STRING(), " #", ctx.getChildCount())
        if False:
            for c in ctx.getChildren():
                print("    ", c)
            for i in range(100):
                for t in ctx.getTokens(i):
                    print(i, "    ", t)

    # Exit a parse tree produced by slatParser#print_command.
    def exitPrint_command(self, ctx:slatParser.Print_commandContext):
        print("< Print_command: ")
        #print("< Print_command: ", ctx.STRING())

    # Enter a parse tree produced by slatParser#print_message.
    def enterPrint_message(self, ctx:slatParser.Print_messageContext):
        print("> Print_message: ", ctx.STRING())

    # Exit a parse tree produced by slatParser#print_message.
    def exitPrint_message(self, ctx:slatParser.Print_messageContext):
        print("< Print_message: ", ctx.STRING())

    # Enter a parse tree produced by slatParser#print_detfn.
    def enterPrint_detfn(self, ctx:slatParser.Print_detfnContext):
        print("> print_detfn: ", ctx.ID())

    # Exit a parse tree produced by slatParser#print_detfn.
    def exitPrint_detfn(self, ctx:slatParser.Print_detfnContext):
        print("< print_detfn: ", ctx.ID())


    # Enter a parse tree produced by slatParser#print_probfn.
    def enterPrint_probfn(self, ctx:slatParser.Print_probfnContext):
        print("> print_probfn: ", ctx.ID())

    # Exit a parse tree produced by slatParser#print_probfn.
    def exitPrint_probfn(self, ctx:slatParser.Print_probfnContext):
        print("< print_probfn: ", ctx.ID())


    # Enter a parse tree produced by slatParser#print_im.
    def enterPrint_im(self, ctx:slatParser.Print_imContext):
        print("> print_im: ", ctx.ID())

    # Exit a parse tree produced by slatParser#print_im.
    def exitPrint_im(self, ctx:slatParser.Print_imContext):
        print("< print_im: ", ctx.ID())

    # Enter a parse tree produced by slatParser#print_edp.
    def enterPrint_edp(self, ctx:slatParser.Print_edpContext):
        print("> print_edp: ", ctx.ID())

    # Exit a parse tree produced by slatParser#print_edp.
    def exitPrint_edp(self, ctx:slatParser.Print_edpContext):
        print("< print_edp: ", ctx.ID())


    # Enter a parse tree produced by slatParser#print_fragfn.
    def enterPrint_fragfn(self, ctx:slatParser.Print_fragfnContext):
        print("> print_fragfn: ", ctx.ID())

    # Exit a parse tree produced by slatParser#print_fragfn.
    def exitPrint_fragfn(self, ctx:slatParser.Print_fragfnContext):
        print("< print_fragfn: ", ctx.ID())


    # Enter a parse tree produced by slatParser#print_lossfn.
    def enterPrint_lossfn(self, ctx:slatParser.Print_lossfnContext):
        print("> print_lossfn: ", ctx.ID())

    # Exit a parse tree produced by slatParser#print_lossfn.
    def exitPrint_lossfn(self, ctx:slatParser.Print_lossfnContext):
        print("< print_lossfn: ", ctx.ID())


    # Enter a parse tree produced by slatParser#print_compgroup.
    def enterPrint_compgroup(self, ctx:slatParser.Print_compgroupContext):
        print("> print_compgroup: ", ctx.ID())

    # Exit a parse tree produced by slatParser#print_compgroup.
    def exitPrint_compgroup(self, ctx:slatParser.Print_compgroupContext):
        print("< print_compgroup: ", ctx.ID())


    # Enter a parse tree produced by slatParser#print_options.
    def enterPrint_options(self, ctx:slatParser.Print_optionsContext):
        print("> Print_options: ", ctx.FILE_NAME(), ", ", ctx.APPEND_OPTION(), ", ", ctx.NEW_OPTION())

    # Exit a parse tree produced by slatParser#print_options.
    def exitPrint_options(self, ctx:slatParser.Print_optionsContext):
        print("< Print_options: ", ctx.FILE_NAME(), ", ", ctx.APPEND_OPTION(), ", ", ctx.NEW_OPTION())

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
        print("> Python_script")

    # Exit a parse tree produced by slatParser#python_script.
    def exitPython_script(self, ctx:slatParser.Python_scriptContext):
        print("< Python_script")

    # Enter a parse tree produced by slatParser#non_paren_expression.
    def enterNon_paren_expression(self, ctx:slatParser.Non_paren_expressionContext):
        print("> Non_paren_expressoin")

    # Exit a parse tree produced by slatParser#non_paren_expression.
    def exitNon_paren_expression(self, ctx:slatParser.Non_paren_expressionContext):
        print("< Non_paren_expressoin")

    # Enter a parse tree produced by slatParser#balanced_paren_expression.
    def enterBalanced_paren_expression(self, ctx:slatParser.Balanced_paren_expressionContext):
        print("> Balanced_paren_expression")

    # Exit a parse tree produced by slatParser#balanced_paren_expression.
    def exitBalanced_paren_expression(self, ctx:slatParser.Balanced_paren_expressionContext):
        print("< Balanced_paren_expression")

    # Enter a parse tree produced by slatParser#analyze_command.
    def enterAnalyze_command(self, ctx:slatParser.Analyze_commandContext):
        print("> Analyze_command")

    # Exit a parse tree produced by slatParser#analyze_command.
    def exitAnalyze_command(self, ctx:slatParser.Analyze_commandContext):
        print("< Analyze_command")

    # Enter a parse tree produced by slatParser#set_command.
    def enterSet_command(self, ctx:slatParser.Set_commandContext):
        print("> Set_command")

    # Exit a parse tree produced by slatParser#set_command.
    def exitSet_command(self, ctx:slatParser.Set_commandContext):
        print("< Set_command")

def main(argv):
    test_cases = [ 'title \'This is a title\';',
                   'title \'This is a title with a \\\' quote inside.\';',
                   'title \'This is a title with a \\\\ backslash.\';',
                   'title \'This is a title with a \\\' quote and a \\\\ backslash.\';',
                   'detfn detfn1 hyperbolic 1.0, 2, -.5;',
                   'detfn detfn1 powerlaw 0.5, 1.2;',
                   'fragfn FRAGID [0.0062, 0.4], [0.0230, 0.4], [0.0440, 0.4], [0.0564, 0.4] --mu mean_ln_x;',
                   'print message;',
                   'print message "This is a message string.";',
                   "print message 'This is a message string.' MESSAGE.TXT --new;",
                   "print  message 'This is a message string.' MESSAGE.TXT --append;",
                   'print detfn IM_FUNC INPUT_DEFS.TXT --new;',
                   'print probfn EDP_FN INPUT_DEFS.TXT --append;',
                   'print im IM1 INPUT_DEFS.TXT --append;',
                   'print edp EDP1 INPUT_DEFS.TXT --append;',
                   'print fragfn FRAG1 INPUT_DEFS.TXT --append;',
                   'print lossfn LOSS1 INPUT_DEFS.TXT --append;',
                   'print compgroup PG1 INPUT_DEFS.TXT --append;'
    ]
    for test_case in test_cases:
        print()
        print("'",test_case, "' --> ", sep='', end='')
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
