%option noyywrap
%option nodefault

%{
	#include "y.tab.h"
	#define UNKNOWN -1
%}
DIGIT [0-9]
CAPITAL [A-Z]
LETTER [a-z]CAPITAL
BLANK [ \n\r\t]
%%

{BLANK}
{CAPITAL}({LETTER}|{DIGIT})* { return ID; }
"&&" { return OP_AND; }
"||" { return OP_OR; }
"0" {return FALSE;}
"1" {return TRUE;}
";" {return END_RULE;}
. { yyerror ("Unknown char"); return UNKNOWN;}
