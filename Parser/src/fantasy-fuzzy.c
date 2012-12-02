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

#include "fantasy-fuzzy.h"
#include <stdio.h>
#include <string.h>


/*
 * lista costantate per le MF
 */
const struct s_mflist listaMF[] =
{
	{"tol", f_tol},
	{"tor", f_tor},
	{"tri", f_tri},
	{"tra", f_tra},
	{"int", f_int},
	{"sgt", f_sgt}
};


/*
 * Funzioni ausiliarie
 */

//max
static double max(double a, double b, double alfa)
{
	double max = (a > b) ? a : b;
	return (max > alfa) ? max : alfa;
}

/*
 * Operatori Fuzzy
 */

//l'operatore is è la funzione di appartenenza della variabile linguistica (sinistro) all'insieme fuzzy (destro)
static double fuzzy_is(t_rule* sinistro, t_rule* destro)
{
	return 1;
}

//l'operatore and fuzzy, osia una T-norm che dipende dal parametro alfa
static double fuzzy_and(t_rule* sinistro, t_rule* destro)
{
	double a = visitaAlbero(sinistro);
	double b = visitaAlbero(destro);
	return a*b/max(a, b, ALFA);
}

//l'operatore or fuzzy, osia una T-conorm (in questo caso il minimo)
static double fuzzy_or(t_rule* sinistro, t_rule* destro)
{
	double a = visitaAlbero(sinistro);
	double b = visitaAlbero(destro);\
	return (a > b) ? a : b;
}

//l'operatore not fuzzy: semplicemente 1-mf(destro)  
static double fuzzy_not(t_rule* destro)
{
	return (1- visitaAlbero(destro));
}

/*
 * Membership functions 
 */

double f_tol(t_paramlist* parameters, int value)
{
	return 0;
}

double f_tor(t_paramlist* parameters, int value)
{
	return 0;
}

double f_tri(t_paramlist* parameters, int value)
{
	return 0;
}

double f_tra(t_paramlist* parameters, int value)
{
	return 0;
}

double f_int(t_paramlist* parameters, int value)
{
	return 0;
}

double f_sgt(t_paramlist* parameters, int value)
{
	return 0;
} 

/*
 * Funzioni esterne 
 */

////crea lista parametri
t_paramlist* creaParametri(t_paramlist* parametriPrecedenti, int parametro)
{
	t_paramlist* parametri = parametriPrecedenti;
	
	if(parametri == NULL)
	{
		parametri = (t_paramlist*) malloc(sizeof(t_paramlist));
		parametri->paramNumber = 0;
	}
	parametri->param[parametri->paramNumber] = parametro;
	parametri->paramNumber++;
	
	return parametri;
}


//crea un fuzzy set
t_fuzzyset* creaFuzzySet(char* label, char* mfname, t_paramlist* parameters, t_fuzzyset* testa)
{
	int i;
	t_fuzzyset* fuzzySet = (t_fuzzyset*)malloc(sizeof(t_fuzzyset));
	t_mfunction mfunction;
	
	if(label == NULL || mfname == NULL) return 0;
	for(i = 0; i < MF_NUMBER; i++)
	{
		if(strcmp(listaMF[i].mfname, mfname) == 0) mfunction = listaMF[i].function;
	}
	fuzzySet->label = strdup(label);
	fuzzySet->parameters = parameters;
	fuzzySet->mfunction = mfunction;
	fuzzySet->next = testa;
	
	return fuzzySet;
}

//crea un nodo dell'albero della regola
t_rule* creaNodo(t_nodo tipo, t_rule* sinistro, t_rule* destro, const char* nome)
{
	t_rule* nodo;
	nodo = (t_rule*) malloc(sizeof(t_rule*));
	nodo->tipo = tipo;
	nodo->destro = destro;
	nodo->sinistro = sinistro;
	if(nome != NULL)
	{
		nodo->nome = strdup(nome);
	}
	return nodo;
}

//aggiunge un albero delle regole insieme al target della regola, alla lista di regole
t_rulebase* aggiungiAllaLista(t_rulebase* testa, t_rule* rule, t_rule* target)
{
	t_rulebase* nuovaTesta;
	
	nuovaTesta = (t_rulebase*) malloc(sizeof(t_rulebase));
	nuovaTesta->rule = rule;
	nuovaTesta->target = target;
	nuovaTesta->nextRule = testa;
	return nuovaTesta;
}

//visita l'albero delle regole, e calcola il suo risultato
double visitaAlbero(t_rule* radice)
{
	t_rule* destro;
	t_rule* sinistro;
	
	if(radice == NULL) return 0;
	destro = radice->destro;
	sinistro = radice->sinistro;
	switch(radice->tipo)
	{
		case AND_N:
			printf("AND ");
			return fuzzy_and(sinistro, destro);
		case OR_N:
			printf("OR ");
			return fuzzy_or(sinistro, destro);
		case NOT_N:
			printf("NOT ");
			return fuzzy_not(destro);
		case IS_N:
			printf("IS ");
			return fuzzy_is(sinistro, destro);
			break;
		case ID_N:
			printf("trovato ID: %s \n", radice->nome);
			return 1;
	}
}

//processa tutte le regole
void setFuzzyOutPuts(t_rulebase* testa)
{
	printf("Chiamo l'algoritmo \n");
	t_rulebase* testaCorrente = testa;
	int i=1;
	while(testaCorrente != NULL)
	{
		printf("Ciclo: i = %d \n", i);
		double fuzzyValue = visitaAlbero(testaCorrente->rule);
		//TODO: qui si dovrebbe assegnare il valore alla variabile di uscita
		printf("\nla regola %d vale %f \n", i, fuzzyValue);
		testaCorrente = testaCorrente->nextRule;
		i++;
	}
}



