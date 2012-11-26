%{
	# include<stdio.h>
%}
%token TRUE
%token FALSE
%token ID
%token END_RULE
%token OP_OR
%token OP_AND

%left OP_OR
%left OP_AND

%%



calculus 	: /* Empty */
		| wellFormedFormula END_RULE {return 0;}
		;
		
wellFormedFormula 	: TRUE {$$ = 1; printf("ricevuto vero\n");}
			| FALSE {$$ = 0;printf("ricevuto falso\n");}
			| ID {$$ = 1;}
			| wellFormedFormula OP_OR wellFormedFormula {$$ = $1 || $2; printf("%d or %d fa %s\n", $1,$2,$$?"vero":"falso");}
			| wellFormedFormula OP_AND wellFormedFormula {$$ = $1 && $2; printf("%d and %d fa %s\n", $1,$2,$$?"vero":"falso");}
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
