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
	ATTRIB;
	PARENTOF;
	PRECEDEDS;
	ATTRIBEQUAL;
	HASVALUE;
	BEGINSWITH;
	PSEUDO;
	PROPERTY;
	FUNCTION;
	TAG;
	ID;
	CLASS;
}

DBL_QUOTE : '"'; //"
S_QUOTE : '\'';

stylesheet
	: importRule* (nested | ruleset)+
	;

importRule
	: ('@import' | '@include')  string -> ^( IMPORT string )
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
	;

pseudo
	: (':'|'::') IDENT -> ^( PSEUDO IDENT )
	| (':'|'::') function -> ^( PSEUDO function )
	;

attrib
	: '[' IDENT (attribRelate (string | IDENT))? ']' -> ^( ATTRIB IDENT (attribRelate string* IDENT*)? )
	;

attribRelate
	: '='  -> ATTRIBEQUAL
	| '~=' -> HASVALUE
	| '|=' -> BEGINSWITH
	;

declaration
	: IDENT ':' args -> ^( PROPERTY IDENT args )
	;

args
	: expr (','? expr)* -> expr*
	;

expr
	: (NUM unit?)
	| IDENT
	| COLOR
	| string
	| function
	;

unit
	: ('%'|'px'|'cm'|'mm'|'in'|'pt'|'pc'|'em'|'ex'|'deg'|'rad'|'grad'|'ms'|'s'|'hz'|'khz')
	;

function
	: IDENT '(' args? ')' -> IDENT '(' args* ')'
	;

string
	:	DBL_QUOTE! (~('"'|'\n'|'\r'))* DBL_QUOTE!
	|	S_QUOTE! (~('\''|'\n'|'\r'))* S_QUOTE!
	;


IDENT
	:	('_' | 'a'..'z'| 'A'..'Z' | '\u0100'..'\ufffe' )
		('_' | '-' | 'a'..'z'| 'A'..'Z' | '\u0100'..'\ufffe' | '0'..'9')*
	|	'-' ('_' | 'a'..'z'| 'A'..'Z' | '\u0100'..'\ufffe' )
		('_' | '-' | 'a'..'z'| 'A'..'Z' | '\u0100'..'\ufffe' | '0'..'9')*
	;

NUM
	:	'-' (('0'..'9')* '.')? ('0'..'9')+
	|	(('0'..'9')* '.')? ('0'..'9')+
	;

COLOR
	:	'#' ('0'..'9'|'a'..'f'|'A'..'F')+
	;

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

// Whitespace -- ignored
WS	: ( ' ' | '\t' | '\r' | '\n' | '\f' )+ { $channel = HIDDEN; }
	;

