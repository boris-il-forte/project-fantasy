%{
	# include<stdio.h>
	extern FILE *yyin;
%}
%token TRUE
%token FALSE
%token ID
%token END_RULE
%token OP_OR
%token OP_AND
%token OP_NOT
%token OPEN_B
%token CLOSE_B
%token THEN
%token IS

%left OP_AND
%left OP_OR
%right OP_NOT

%%

ruleSet 		: /* Empty */
			| ruleSet wellFormedFormula THEN fuzzyAssignment END_RULE
			;
		
wellFormedFormula 	: TRUE {$$ = 1; printf("ricevuto vero\n");}
			| FALSE {$$ = 0;printf("ricevuto falso\n");}
			| OPEN_B wellFormedFormula CLOSE_B {$$ = $2;}
			| fuzzyAssignment {$$ = 1; printf("letto assegnamento fuzzy: %d\n", $$);}
			| OP_NOT wellFormedFormula {$$ = !$2; printf("!%d fa %s\n", $2,$$?"vero":"falso");}
			| wellFormedFormula OP_OR wellFormedFormula {$$ = $1 || $3; printf("%d or %d fa %s\n", $1,$3,$$?"vero":"falso");}
			| wellFormedFormula OP_AND wellFormedFormula {$$ = $1 && $3; printf("%d and %d fa %s\n", $1,$3,$$?"vero":"falso");}
			;

fuzzyAssignment		: OPEN_B ID IS ID CLOSE_B
			;

%%

int yyerror(char * msg) 
{
	printf("%s\n", msg);
	return 0;
}

int main(int argc, char* argv[]) 
{
	if (argc > 1)
	{
		// open a file handle to a particular file:
		FILE *myfile = fopen(argv[1], "r");
		// make sure it's valid:
		if (!myfile) 
		{
			perror("Fatal error");
			return -1;
		}
		// set lex to read from it instead of defaulting to STDIN:
		yyin = myfile;
	}
	
	return yyparse();
	
}
