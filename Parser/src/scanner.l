%option noyywrap
%option nodefault

%{
	#include "fantasy-fuzzy.h"
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
FUZZYLABEL tol|tor|tri|tra|int|sgt

%%

{BLANK} { num_column++; }
{NEWLINE} { num_line++; num_column = 0; }
{CAPITAL}({LETTER}|{DIGIT})* { COUNT; yylval.str = strdup(yytext); return ID; }
{DIGIT}+ {COUNT; yylval.integer = atoi(yytext); return PARAMETER; }
{FUZZYLABEL} { COUNT; return F_LABEL; }
"and" { COUNT; return OP_AND; }
"or" { COUNT; return OP_OR; }
"~" { COUNT; return OP_NOT; }
"==>" { COUNT; return THEN; }
"is" { COUNT; return IS; }
"(" { COUNT; return OPEN_B; }
")" { COUNT; return CLOSE_B; }
";" { COUNT; return END_RULE; }
"->" { COUNT; return LIKE; }
"," { COUNT; return COMMA; }
. { fprintf(stderr,"Unknown char \"%c\" at line %d, column %d \n", *yytext, num_line, num_column+1); return UNKNOWN; }
