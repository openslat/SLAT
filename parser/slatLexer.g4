lexer grammar slatLexer;

//mode default;

WS  :   ( ' '
        | '\t'
        | '\r'
        | '\n'
	| COMMENT
        ) -> skip
    ;
COMMENT :   '#' ~[\r\n]*;

PYTHON_ESCAPE : '$(' -> pushMode(PYMODE);

LBRACE : '{';
RBRACE : '}';
LBRACKET : '[';
RBRACKET : ']';
LPAREN : '(';
RPAREN : ')';

TITLE: 'title';
PRINT : 'print';
DETFN : 'detfn';
PROBFN : 'probfn';
COLLAPSE : 'collapse';
DEMOLITION: 'demolition';
COLLRATE : 'collrate';
IM : 'im';
EDP : 'edp';
FRAGFN: 'fragfn';
LOSSFN: 'lossfn';
COMPGROUP : 'compgroup';
STRUCTURE : 'structure';
REBUILDCOST: 'rebuildcost';
DEMOLITIONCOST: 'demolitioncost';
IMRATE : 'imrate';
EDPIM : 'edpim';
EDPRATE : 'edprate';
DSEDP : 'dsedp';
DSIM : 'dsim';
LOSSDS : 'lossds';
LOSSEDP : 'lossedp';
LOSSIM : 'lossim';
STRUCTLOSS: 'structloss';
DSRATE : 'dsrate';
ANNLOSS: 'annloss';
LOSSRATE: 'lossrate';
TOTALLOSS: 'totalloss';
DEAGG: 'deagg';
APPEND_OPTION : '--append';
NEW_OPTION : '--new';
DOLLARS : '$';

MEAN_LN_X: 'mean_ln_x';
MEDIAN_X: 'median_x';
MEAN_X: 'mean_x';

SD_LN_X: 'sd_ln_x';
SD_X: 'sd_x';

MAQ: 'maq';

BY_FATE_FLAG: '--by-fate';
BY_EDP_FLAG: '--by-edp';
BY_FRAG_FLAG: '--by-frag';
ANNUAL_FLAG: '--annual';

STRING : SINGLE_QUOTED_STRING | DOUBLE_QUOTED_STRING ;

HYPERBOLIC: 'hyperbolic';
POWERLAW: 'powerlaw';
LOGLOG: ' loglog';
LINEAR: 'linear';
LINFLAG: '--linear';
LOGFLAG: '--log';
COMMA: ',';
SEMICOLON: ';';
COLON: ':';
LOGNORMAL: 'lognormal';
DB_FLAG: '--db';
STDFUNC_FLAG: '--stdfunc';
MU_FLAG: '--mu';
SD_FLAG: '--sd';
SIMPLE: 'simple';
COST: 'cost';
DISP: 'disp';
UPPER_COST: 'upper_cost';
LOWER_COST: 'lower_cost';
LOWER_N: 'lower_n';
UPPER_N: 'upper_n';
MEAN_UNCERT: 'mean_uncert';
VAR_UNCERT: 'var_uncert';
MESSAGE: 'message';
INTEGRATION: 'integration';
RECORDER: 'recorder';
LAMBDA_FLAG: '--lambda';
COLS_FLAG: '--cols';
ANALYZE: 'analyze';
SET: 'set';
IMPORTPROBFN: 'importprobfn';
IMPORTIMFN: 'importimfn';
PLUS: '+';
MINUS: '-';
STAR: '*';

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
Nondigit : [a-zA-Z_.];

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

FILE_NAME : [/a-zA-Z_.0-9]+;

mode PYMODE;
PYTHON_END: ')' -> popMode ;
PYMODELPAREN: '(' -> pushMode(NESTEDPYMODE);
OTHER: .;
mode NESTEDPYMODE;
NESTEDLPAREN: '(' -> pushMode(NESTEDPYMODE);
NESTEDRPAREN: ')' -> popMode;
NESTEDOTHER: .;
