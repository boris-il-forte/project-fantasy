%{
	#include<stdio.h>
	#include "fantasy-fuzzy.h"
	extern FILE *yyin;
	
	t_fuzzyset* fuzzysetRoot;
	t_rulebase* rulebaseRoot;
%}

%union {int integer; char* str; t_rule* nodo; t_rulebase* regola; t_fuzzyset* fset; t_paramlist* fshape;}

%token<str> ID
%token END_RULE
%token OP_OR
%token OP_AND
%token OP_NOT
%token OPEN_B
%token CLOSE_B
%token THEN
%token IS

%token LIKE
%token COMMA
%token<str> F_LABEL
%token<integer> PARAMETER

%type <nodo> wellFormedFormula
%type <nodo> fuzzyAssignment
%type <regola> ruleSet
%type <fset> fuzzySet
%type <fshape> shape
%type <fshape> parametersList

%left OP_AND
%left OP_OR
%right OP_NOT

%%

fuzzyFile		: fuzzySet ruleSet 
			;

fuzzySet		: /* Empty */ 
			{ 
				$$ = NULL; 
			}
			| ID LIKE F_LABEL shape END_RULE fuzzySet 
			{ 
				$$ = creaFuzzySet($1, $3, $4, $6); 
				if($$ = NULL) YYABORT; 
				fuzzysetRoot = $$; 
			}
			;
			
shape			: OPEN_B parametersList CLOSE_B 
			{
				$$ = $2;
			}
			;

parametersList		: PARAMETER 
			{ 
				$$ = creaParametri(NULL,$1); 
			}
			| PARAMETER COMMA parametersList 
			{ 
				$$ = creaParametri($3,$1); 
			}
			;

ruleSet 		: /* Empty */ 
			{
				$$ = NULL;
			}
			| wellFormedFormula THEN fuzzyAssignment END_RULE ruleSet 
			{
				$$ = aggiungiAllaLista($5, $1, $3); rulebaseRoot = $$;
			}
			;

wellFormedFormula	: fuzzyAssignment 
			{
				$$ = $1;
			}
			| OPEN_B wellFormedFormula CLOSE_B 
			{
				$$ = $2;
			}
			| OP_NOT wellFormedFormula 
			{
				$$ = creaNodo(NOT_N, NULL, $2, "");
			}
			| wellFormedFormula OP_OR wellFormedFormula 
			{
				$$ = creaNodo(OR_N, $1, $3, "");
			}
			| wellFormedFormula OP_AND wellFormedFormula 
			{
				$$ = creaNodo(AND_N, $1, $3, "");
			}
			;

fuzzyAssignment		: OPEN_B ID IS ID CLOSE_B 
			{
				t_rule* sinistro = creaNodo(ID_N, NULL, NULL, $2); 
				t_rule* destro = creaNodo(ID_N, NULL, NULL, $4); 
				$$ = creaNodo(IS_N, sinistro, destro, "");
				if(!matchVariables(sinistro)) YYABORT;
				if(!matchFuzzySet(fuzzysetRoot, destro))  YYABORT;
			}
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
	
	if(yyparse() == 0)
	{
		setFuzzyOutPuts(rulebaseRoot);
	}
	return 0;
}

