parser grammar slatParser;

options { tokenVocab=slatLexer; }

script : (WS | command)*;

command : (title_command | detfn_command | probfn_command | im_command | edp_command
	| fragfn_command | lossfn_command | compgroup_command | print_command 
	| integration_command | recorder_command | analyze_command | set_command 
	| importprobfn_command | collapse_command | demolition_command |structure_command
	| importimfn_command | rebuildcost_command | demolitioncost_command) SEMICOLON;

title_command : TITLE STRING;

detfn_command : DETFN ID (hyperbolic_args | powerlaw_args | loglog_args | linear_args);
hyperbolic_args : HYPERBOLIC scalar3;
powerlaw_args : POWERLAW scalar2;
loglog_args : LOGLOG LBRACKET scalar2 RBRACKET (COMMA LBRACKET scalar2 RBRACKET)*;
linear_args : LINEAR LBRACKET scalar2 RBRACKET (COMMA LBRACKET scalar2 RBRACKET)*;

scalar :  STRING | numerical_scalar;
scalar2 : scalar COMMA scalar;
scalar3 : scalar2 COMMA scalar;
var_ref : DOLLARS ID;

numerical_scalar : INTEGER | FLOAT_VAL | python_script | var_ref;

parameters : parameter | parameter_array | parameter_dictionary;

parameter : ID | STRING | INTEGER | FLOAT_VAL;

parameter_array : LBRACKET parameter (COMMA parameter)* RBRACKET;

parameter_dictionary : LPAREN dictionary_entry (COMMA dictionary_entry)* RPAREN;

dictionary_entry : (ID | STRING) COLON parameters;

probfn_command : PROBFN ID LOGNORMAL function_ref COMMA function_ref lognormal_options;
function_ref : ID | var_ref;

im_command : IM ID ID;
edp_command : EDP ID ID ID;

collapse_command: COLLAPSE ID lognormal_dist;
demolition_command: DEMOLITION ID lognormal_dist;

fragfn_command : FRAGFN ID 
	       (fragfn_db_params | fragfn_user_defined_params);
file_spec: FILE_NAME | ID;	       
fragfn_db_params : ((DB_FLAG file_spec)? STDFUNC_FLAG db_key) |
		   (STDFUNC_FLAG db_key DB_FLAG file_spec) ;
		 
fragfn_user_defined_params : scalar2_sequence lognormal_options;
mu_option : MU_FLAG (MEAN_LN_X | MEDIAN_X | MEAN_X);
sd_option : SD_FLAG (SD_LN_X | SD_X);
lognormal_options : (mu_option? sd_option?) | (sd_option mu_option);
	

placement_type : MEAN_LN_X | MEDIAN_X | MEAN_X;
spread_type : (SD_LN_X | SD_X);

lognormal_dist : scalar2 lognormal_options;

lognormal_parameter_array : LBRACKET lognormal_dist (COMMA lognormal_dist)* RBRACKET;
array_array : LBRACKET parameter_array (COMMA parameter_array)* RBRACKET;	       
db_key :  ID;

lossfn_command : LOSSFN ID simple_loss_command;
simple_loss_command : SIMPLE scalar2_sequence lognormal_options;
scalar2_sequence: LBRACKET scalar2 RBRACKET (COMMA LBRACKET scalar2 RBRACKET)*;

lossfn_heading : COST | DISP | UPPER_COST | LOWER_COST | LOWER_N | UPPER_N | MEAN_UNCERT | VAR_UNCERT;
lossfn_headings : LPAREN lossfn_heading (COMMA lossfn_heading)* RPAREN;
lossfn_anon_array : LBRACKET parameter_array (COMMA parameter_array)* RBRACKET;
lossfn_dict : LPAREN lossfn_heading COLON (COMMA lossfn_heading COLON parameter)* RPAREN;
lossfn_named_array : LBRACKET lossfn_dict (COMMA lossfn_dict)* RBRACKET;

compgroup_command : COMPGROUP ID ID ID ID INTEGER;

structure_command : STRUCTURE ID id_list;
id_list: ID (COMMA ID)*;

rebuildcost_command : REBUILDCOST ID lognormal_dist;
demolitioncost_command : DEMOLITIONCOST ID lognormal_dist;

print_command : (print_message | print_function | print_title) print_options?;

print_message : PRINT MESSAGE (STRING | python_script | var_ref)?;
print_title : PRINT TITLE;
print_function: PRINT (DETFN | PROBFN | IM | EDP | FRAGFN | LOSSFN | COMPGROUP) ID;

print_options : file_spec (APPEND_OPTION | NEW_OPTION)?;

integration_command : INTEGRATION integration_method numerical_scalar (INTEGER | var_ref | python_script);
integration_method : MAQ;

recorder_command : RECORDER recorder_id ((recorder_type ID recorder_at recorder_cols?) 
                  | ((DSRATE | COLLRATE) ID)
                  | ((COSTRATE | COLLAPSE | DEAGG | TOTALCOST) ID recorder_at)
		  | (ANNCOST ID recorder_at LAMBDA_FLAG lambda_value)
		  | (STRUCTCOST ID ((structcost_type recorder_at) | (ANNUAL_FLAG lognormal_options)) recorder_cols?))
		  print_options?;
lambda_value: numerical_scalar;
recorder_id : ID;

recorder_type : DETFN | PROBFN | IMRATE | EDPIM | EDPRATE | DSEDP
	      | DSIM | LOSSDS | COSTEDP | COSTIM | DELAYIM;
recorder_at : float_array | (FLOAT_VAL COLON FLOAT_VAL COLON FLOAT_VAL) 
	    | python_script | var_ref | counted_at;
counted_at: FLOAT_VAL COLON FLOAT_VAL (LINFLAG | LOGFLAG) INTEGER;

float_array : FLOAT_VAL (COMMA FLOAT_VAL)*;
structcost_type : BY_FATE_FLAG | BY_EDP_FLAG | BY_FRAG_FLAG;

col_spec : placement_type | spread_type | scalar;
recorder_cols : COLS_FLAG col_spec (COMMA col_spec)*;


python_script : PYTHON_ESCAPE python_expression PYTHON_END;
python_expression : (OTHER | PYMODELPAREN | NESTEDOTHER | NESTEDLPAREN | NESTEDRPAREN)*?;

analyze_command : ANALYZE;

set_command : SET ID (python_script | var_ref | parameters);

importprobfn_command : IMPORTPROBFN ID file_spec;

importimfn_command : IMPORTIMFN ID file_spec;
