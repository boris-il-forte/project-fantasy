/*
 * Project Fantasy, gioco di strategia a turni
 *
 *
 * Copyright (C) 2012 Davide Tateo
 * Versione 1.0
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include<stdlib.h>
#define ALFA 0
#define MAX_PARAM_NUMBER 4

#define MF_NUMBER sizeof(listaMF)/sizeof(listaMF[0])

typedef enum e_nodo {AND_N, OR_N, NOT_N, IS_N, ID_N } t_nodo;

//struct per nodo regola
typedef struct s_rule
{
	t_nodo tipo;
	struct s_rule* destro;
	struct s_rule* sinistro;
	char* nome;
} t_rule;

//struct per lista regole
typedef struct s_rulebase
{
	t_rule* rule;
	t_rule* target;
	struct s_rulebase* nextRule;
} t_rulebase;

typedef struct s_paramlist
{
	int paramNumber;
	int param[MAX_PARAM_NUMBER];
} t_paramlist;

typedef double(*t_mfunction)(t_paramlist* parameters, int value);

typedef struct s_fuzzyset 
{
	char* label;
	t_paramlist* parameters;
	t_mfunction mfunction;
	struct s_fuzzyset* next;
} t_fuzzyset;


//Funzione per la gestione dei fuzzy set
t_paramlist* creaParametri(t_paramlist* parametriPrecedenti, int parametro);
t_fuzzyset* creaFuzzySet(char* label, char* mfname, t_paramlist* parameters, t_fuzzyset* testa);

//Funzioni per la gestione dei nodi
t_rule* creaNodo(t_nodo tipo,t_rule* sinistro, t_rule* destro, const char* nome);

t_rulebase* aggiungiAllaLista(t_rulebase* testa, t_rule* rule, t_rule* target);

double visitaAlbero(t_rule* radice);

//Funzioni per far partire il controllore fuzzy
void setFuzzyOutPuts(t_rulebase* testa);

/*
 * Membership Functions
 */

double f_tol(t_paramlist* parameters, int value);

double f_tor(t_paramlist* parameters, int value);

double f_tri(t_paramlist* parameters, int value);

double f_tra(t_paramlist* parameters, int value);

double f_int(t_paramlist* parameters, int value);

double f_sgt(t_paramlist* parameters, int value);

/*
 * Lista ausiliaria Membership Functions
 */

struct s_mflist
{
	const char* mfname;
	const t_mfunction function;
	int paramNumber;
};


extern const struct s_mflist listaMF[];

