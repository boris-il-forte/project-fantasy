%{
	# include<stdio.h>
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

%left OP_OR
%left OP_AND

%%



calculus 	: /* Empty */
		| calculus wellFormedFormula THEN fuzzyAssignment END_RULE  {return 0;}
		;
		
wellFormedFormula 	: TRUE {$$ = 1; printf("ricevuto vero\n");}
			| FALSE {$$ = 0;printf("ricevuto falso\n");}
			| OPEN_B wellFormedFormula CLOSE_B
			| fuzzyAssignment {$$ = 1;}
			| OP_NOT wellFormedFormula {$$ = !$2;}
			| wellFormedFormula OP_OR wellFormedFormula {$$ = $1 || $2; printf("%d or %d fa %s\n", $1,$2,$$?"vero":"falso");}
			| wellFormedFormula OP_AND wellFormedFormula {$$ = $1 && $2; printf("%d and %d fa %s\n", $1,$2,$$?"vero":"falso");}
			;

fuzzyAssignment	: OPEN_B ID IS ID CLOSE_B
		;
%%

int yyerror(char * msg) 
{
	printf("%s\n", msg);
	return 0;
}

int main(int argc, char* argv[]) 
{
	return yyparse();
}
