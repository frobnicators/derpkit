grammar css3;
options {
	output=AST;
	language=C;
	k=4;
}

tokens {
	IMPORT;
	NESTED;
	NEST;
	RULE;
	ANY;
	ATTRIB;
	PARENTOF;
	PRECEDEDS;
	ATTRIBEQUAL;
	HASVALUE;
	BEGINSWITH;
	PREFIXEDWITH;
	SUFIXEDWITH;
	CONTAINS;
	PSEUDO;
	PROPERTY;
	FUNCTION;
	TAG;
	ID;
	CLASS;
}

/* Lexer */

// Whitespace -- ignored
WS	: ( ' ' | '\t' | '\r' | '\n' | '\f' )+ { $channel = HIDDEN; }
	;

IDENT
	:	('_' | 'a'..'z'| 'A'..'Z' | '\u0100'..'\ufffe' )
		('_' | '-' | 'a'..'z'| 'A'..'Z' | '\u0100'..'\ufffe' | '0'..'9')*
	|	'-' ('_' | 'a'..'z'| 'A'..'Z' | '\u0100'..'\ufffe' )
		('_' | '-' | 'a'..'z'| 'A'..'Z' | '\u0100'..'\ufffe' | '0'..'9')*
	;

FLOAT
	:	'-' ('0'..'9')* '.' ('0'..'9')+
	|	('0'..'9')* '.' ('0'..'9')+
	;

INT
	:	('0'..'9')+
	|	'-' ('0'..'9')+
	;

IMPORTANT : '!important';

// Single-line comments
SL_COMMENT
	:	'//'
		(~('\n'|'\r'))* ('\n'|'\r'('\n')?)
		{$channel=HIDDEN;}
	;

// multiple-line comments
COMMENT
	:	'/*' .* '*/' { $channel = HIDDEN; }
	;


stylesheet
	: importRule* (nested | ruleset)+ EOF!
	;

importRule
	: ('@import' | '@include')  STRING -> ^( IMPORT STRING )
	;

nested
 	: '@' nest '{' properties? nested* '}' -> ^( NESTED nest properties* nested* )
	;

nest
	: IDENT IDENT* pseudo* -> ^( NEST IDENT IDENT* pseudo* )
	;

ruleset
 	: selectors '{' properties? '}' -> ^( RULE selectors properties* )
	;

selectors
	: selector (',' selector)*
	;

selector
	: elem selectorOperation* attrib* pseudo? ->  elem selectorOperation* attrib* pseudo*
	;

selectorOperation
	: selectop? elem -> selectop* elem
	;

selectop
	: '>' -> PARENTOF
	| '+'  -> PRECEDEDS
	;

properties
	: declaration (';' declaration?)* ->  declaration+
	;

elem
	:     IDENT -> ^( TAG IDENT )
	| '#' IDENT -> ^( ID IDENT )
	| '.' IDENT -> ^( CLASS IDENT )
	| '*' -> ^( ANY )
	;

pseudo
	: (':'|'::') IDENT -> ^( PSEUDO IDENT )
	| (':'|'::') function -> ^( PSEUDO function )
	;

color
	:	'#'! IDENT
	;

attrib
	: '[' IDENT (attribRelate (STRING | IDENT))? ']' -> ^( ATTRIB IDENT (attribRelate STRING* IDENT*)? )
	;


/*DBL_QUOTE : '"'; //"
S_QUOTE : '\'';*/
STRING
	:	'"' (~('"'|'\n'|'\r'))* '"'
	|	'\'' (~('\''|'\n'|'\r'))* '\''
	;

attribRelate
	: '='  -> ATTRIBEQUAL
	| '~=' -> HASVALUE
	| '|=' -> BEGINSWITH
	| '^=' -> PREFIXEDWITH
	| '$=' -> SUFIXEDWITH
	| '*=' -> CONTAINS
	;

declaration
	: IDENT ':' args IMPORTANT? -> ^( PROPERTY IDENT args IMPORTANT?)
	;

args
	: expr (','? expr)* -> expr*
	;

num
	: INT
	| FLOAT;

expr
	: (num unit?)
	| IDENT
	| color
	| STRING
	| function
	;

unit
	: ('%'|'px'|'cm'|'mm'|'in'|'pt'|'pc'|'em'|'ex'|'deg'|'rad'|'grad'|'ms'|'s'|'hz'|'khz')
	;

function
	: IDENT '(' args? ')' -> IDENT '(' args* ')'
	;


