%option noyywrap
%option nodefault

%{
	#include "y.tab.h"
	#define UNKNOWN -1
	#define COUNT num_column += yyleng
	
	int num_line = 1, num_column = 0;
%}

DIGIT [0-9]
CAPITAL [A-Z]
LETTER [a-z]|{CAPITAL}
BLANK [ \t]
NEWLINE \n\r|\n

%%

{BLANK} { num_column++; }
{NEWLINE} { num_line++; num_column = 0; }
{CAPITAL}({LETTER}|{DIGIT})* { COUNT; return ID; }
"and" { COUNT; return OP_AND; }
"or" { COUNT; return OP_OR; }
"~" { COUNT; return OP_NOT; }
"==>" { COUNT; return THEN; }
"is" { COUNT; return IS; }
"(" { COUNT; return OPEN_B; }
")" { COUNT; return CLOSE_B; }
"0" { COUNT; return FALSE; }
"1" { COUNT; return TRUE; }
";" { COUNT; return END_RULE; }
. { fprintf(stderr,"Unknown char \"%c\" at line %d, column %d \n", *yytext, num_line, num_column+1); return UNKNOWN; }
