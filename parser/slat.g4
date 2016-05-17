grammar slat;

WS  :   ( ' '
        | '\t'
        | '\r'
        | '\n'
	| COMMENT
        ) -> skip
    ;

LBRACE : '{';
RBRACE : '}';
LBRACKET : '[';
RBRACKET : ']';
LPAREN : '(';
RPAREN : ')';

COMMENT :   '#' ~[\r\n]*;
PRINT : 'print';
DETFN : 'detfn';
PROBFN : 'probfn';
IM : 'im';
EDP : 'edp';
FRAGFN: 'fragfn';
LOSSFN: 'lossfn';
COMPGROUP : 'compgroup';
IMRATE : 'imrate';
EDPIM : 'edpim';
EDPRATE : 'edprate';
DSEDP : 'dsedp';
DSIM : 'dsim';
LOSSDS : 'lossds';
LOSSEDP : 'lossedp';
LOSSIM : 'lossim';
DSRATE : 'dsrate';
ANNLOSS: 'annloss';
LOSSRATE: 'lossrate';	 
APPEND_OPTION : '--append';
NEW_OPTION : '--new';
DOLLARS : '$';
PYTHON_ESCAPE : '$('; 

MEAN_LN_X: 'mean_ln_x';
MEDIAN_X: 'median_x';
MEAN_X: 'mean_x';

SD_LN_X: 'sd_ln_x';
SD_X: 'sd_x';

MAQ: 'maq';

script : (WS | command)*;

command : (title_command | detfn_command | probfn_command | im_command | edp_command | fragfn_command | lossfn_command | compgroup_command | print_command | integration_command | recorder_command | analyze_command | set_command | importprobfn_command) ';' ;

title_command : 'title' STRING;

STRING : SINGLE_QUOTED_STRING | DOUBLE_QUOTED_STRING ;

fragment
SINGLE_QUOTED_STRING : SINGLE_QUOTE (SQ_UNESCAPED_CHAR| SQ_ESCAPED_CHAR)* SINGLE_QUOTE;

fragment
DOUBLE_QUOTED_STRING : DOUBLE_QUOTE (DQ_UNESCAPED_CHAR| DQ_ESCAPED_CHAR)* DOUBLE_QUOTE;

fragment
SINGLE_QUOTE : '\'';

fragment
SQ_UNESCAPED_CHAR : ~[\\\'];

fragment
SQ_ESCAPED_CHAR : '\\\'' | '\\\\';

fragment
DOUBLE_QUOTE : '"';

fragment
DQ_UNESCAPED_CHAR : ~[\\"];

fragment
DQ_ESCAPED_CHAR : '\\"' | '\\\\';


fragment
Nondigit : [a-zA-Z_];

fragment
Digit : [0-9];

INTEGER : [1-9][0-9]*;
    
ID :   Nondigit (Nondigit | Digit)*;
FLOAT_VAL: SIGN? FRACTION EXPONENT?;

fragment
SIGN: '-' | '+';

fragment
FRACTION : (DIGIT_SEQUENCE ('.' DIGIT_SEQUENCE?)?) | ('.' DIGIT_SEQUENCE);

fragment
DIGIT_SEQUENCE : Digit+;

fragment
EXPONENT : ('e' | 'E') SIGN? DIGIT_SEQUENCE;

detfn_command : DETFN ID (hyperbolic_args | powerlaw_args | loglog_args | linear_args);
hyperbolic_args : 'hyperbolic' scalar3;
powerlaw_args : 'powerlaw' scalar2;
loglog_args : 'loglog' LBRACKET scalar2 RBRACKET (',' LBRACKET scalar2 RBRACKET)*;
linear_args : 'linear' LBRACKET scalar2 RBRACKET (',' LBRACKET scalar2 RBRACKET)*;

scalar :  STRING | numerical_scalar;
scalar2 : scalar ',' scalar;
scalar3 : scalar2 ',' scalar;
var_ref : DOLLARS ID;

numerical_scalar : INTEGER | FLOAT_VAL | python_script | var_ref;

parameters : parameter | parameter_array | parameter_dictionary;

parameter : ID | STRING | INTEGER | FLOAT_VAL;

parameter_array : LBRACKET parameter (',' parameter)* RBRACKET;

parameter_dictionary : LPAREN dictionary_entry (',' dictionary_entry)* RPAREN;

dictionary_entry : (ID | STRING) ':' parameters;

probfn_command : PROBFN ID 'lognormal' function_ref ',' function_ref lognormal_options;
function_ref : ID | var_ref;

im_command : IM ID ID;
edp_command : EDP ID ID ID;


fragfn_command : FRAGFN ID 
	       (fragfn_db_params | fragfn_user_defined_params);
fragfn_db_params : (('--db' FILE_NAME)? '--stdfunc' db_key) |
		   ('--stdfunc' db_key '--db' FILE_NAME) ;
		 
fragfn_user_defined_params : scalar2_sequence lognormal_options;
mu_option : '--mu' (MEAN_LN_X | MEDIAN_X | MEAN_X);
sd_option : '--sd' (SD_LN_X | SD_X);
lognormal_options : (mu_option? sd_option?) | (sd_option mu_option);
	

placement_type : MEAN_LN_X | MEDIAN_X | MEAN_X;
spread_type : (SD_LN_X | SD_X);

lognormal_dist : scalar2 lognormal_options;

lognormal_parameter_array : LBRACKET lognormal_dist (',' lognormal_dist)* RBRACKET;
array_array : LBRACKET parameter_array (',' parameter_array)* RBRACKET;	       
FILE_NAME : [/a-zA-Z_.0-9]+;
db_key :  ID;

lossfn_command : LOSSFN ID simple_loss_command;
simple_loss_command : 'simple' scalar2_sequence lognormal_options;
scalar2_sequence: LBRACKET scalar2 RBRACKET (',' LBRACKET scalar2 RBRACKET)*;

lossfn_heading : 'cost' | 'disp' | 'upper_cost' | 'lower_cost' | 'lower_n' | 'upper_n' | 'mean_uncert' | 'var_uncert';
lossfn_headings : LPAREN lossfn_heading (',' lossfn_heading)* RPAREN;
lossfn_anon_array : LBRACKET parameter_array (',' parameter_array)* RBRACKET;
lossfn_dict : LPAREN lossfn_heading ':' parameter (',' lossfn_heading ':' parameter)* RPAREN;
lossfn_named_array : LBRACKET lossfn_dict (',' lossfn_dict)* RBRACKET;

compgroup_command : COMPGROUP ID ID ID ID INTEGER;

print_command : (print_message | print_function | print_title) print_options?;

print_message : PRINT 'message' (STRING | python_script | var_ref)?;
print_title : PRINT 'title';
print_function: PRINT (DETFN | PROBFN | IM | EDP | FRAGFN | LOSSFN | COMPGROUP) ID;

print_options : FILE_NAME (APPEND_OPTION | NEW_OPTION)?;

integration_command : 'integration' integration_method numerical_scalar (INTEGER | var_ref | python_script);
integration_method : MAQ;

recorder_command : 'recorder' ((recorder_type ID recorder_at recorder_cols?) 
                  | (DSRATE ID)
                  | (LOSSRATE ID recorder_at)
		  | (ANNLOSS ID recorder_at '--lambda' lambda_value))
		  print_options?;
lambda_value: numerical_scalar;

recorder_type : DETFN | PROBFN | IMRATE | EDPIM | EDPRATE | DSEDP
	      | DSIM | LOSSDS | LOSSEDP | LOSSIM;
recorder_at : (float_array | (FLOAT_VAL ':' FLOAT_VAL ':' FLOAT_VAL) | python_script | var_ref);
float_array : FLOAT_VAL (',' FLOAT_VAL)*;

col_spec : placement_type | spread_type | scalar;
recorder_cols : '--cols' col_spec (',' col_spec)*;


python_script : PYTHON_ESCAPE python_expression RPAREN;
python_expression : (balanced_paren_expression | non_paren_expression)*;
balanced_paren_expression : LPAREN non_paren_expression* RPAREN;
non_paren_expression:  .+?;

analyze_command : 'analyze';

set_command : 'set' ID (python_script | var_ref | parameters);

importprobfn_command : 'importprobfn' ID FILE_NAME;
OTHER: .+?;
