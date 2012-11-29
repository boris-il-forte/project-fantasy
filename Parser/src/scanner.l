%option noyywrap
%option nodefault

%{
	#include "y.tab.h"
	#define UNKNOWN -1
%}
DIGIT [0-9]
CAPITAL [A-Z]
LETTER [a-z]|CAPITAL
BLANK [ \n\r\t]
%%

{BLANK}
{CAPITAL}({LETTER}|{DIGIT})* { return ID; }
"and" { return OP_AND; }
"or" { return OP_OR; }
"~" { return OP_NOT; }
"==>" { return THEN; }
"is" { return IS; }
"(" { return OPEN_B; }
")" { return CLOSE_B; }
"0" { return FALSE; }
"1" { return TRUE; }
";" { return END_RULE; }
. { yyerror ("Unknown char"); return UNKNOWN; }
