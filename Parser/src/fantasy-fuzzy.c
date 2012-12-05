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
 * Lista delle variabili crisp
 */

struct s_crispdata
{
	const char* label;
	int* var;
	struct s_crispdata* next;
};

struct s_crispdata* crispData = NULL;

/*
 * Lista costantate per le MF
 */

struct s_mflist
{
	const char* mfname;
	const t_mfunction function;
	int paramNumber;
};

const struct s_mflist listaMF[] =
{
	{"tol", f_tol, 2},
	{"tor", f_tor, 2},
	{"tri", f_tri, 3},
	{"tra", f_tra, 4},
	{"int", f_int, 2},
	{"sgt", f_sgt, 1}
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

//retta valore della retta passante per (x1,y1) e (x2,y2) nel punto x;
double retta(int x1, double y1, int x2, double y2, int x)
{
	double m =  (y2 - y1) / (x2 - x1);
	return m*(x - x1) + y1;
}

/*
 * Operatori Fuzzy
 */

//l'operatore is è la funzione di appartenenza della variabile linguistica (sinistro) all'insieme fuzzy (destro)
static double fuzzy_is(t_rule* sinistro, t_rule* destro)
{
	int* valore = (int*) sinistro->dati;
	t_fuzzyset* fuzzySet = (t_fuzzyset*) destro->dati;
	
	return fuzzySet->mfunction(fuzzySet->parameters, *valore);
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
	if(parameters->paramNumber != 2) return 0;
	int a = parameters->param[0];
	int b = parameters->param[1];
	
	if(value < a)
		return 1.0;
	else if(value > b)
		return 0;
	else
		return retta(a, 1.0, b, 0, value);
}

double f_tor(t_paramlist* parameters, int value)
{
	if(parameters->paramNumber != 2) return 0;
	int a = parameters->param[0];
	int b = parameters->param[1];
	
	if(value > b)
		return 1.0;
	else if(value < a)
		return 0;
	else
		return retta(a, 0, b, 1.0, value);
}

double f_tri(t_paramlist* parameters, int value)
{
	if(parameters->paramNumber != 3) return 0;
	int a = parameters->param[0];
	int b = parameters->param[1];
	int c =  parameters->param[2];
	
	if(value < c)
		return retta(a, 0, b, 1.0, value);
	else if(value > c)
		return retta(a, 1.0, b, 0, value);
	else 
		return 1.0;
}

double f_tra(t_paramlist* parameters, int value)
{
	if(parameters->paramNumber != 4) return 0;
	int a = parameters->param[0];
	int b = parameters->param[1];
	int c = parameters->param[2];
	int d = parameters->param[3];
	
	if(value < a || value > d) 
		return 0;
	else if(value > a && value < b) 
		return retta(a, 0, b, 1.0, value);
	else if(value > c && value < d) 
		return retta(c, 1.0, d, 0, value);
	else
		return  1.0;
}

double f_int(t_paramlist* parameters, int value)
{
	if(parameters->paramNumber != 2) return 0;
	int a = parameters->param[0];
	int b = parameters->param[1];
	
	if(value > a && value < b)
		return 1.0;
	else
		return 0;
}

double f_sgt(t_paramlist* parameters, int value)
{
	if(parameters->paramNumber != 2) return 0;
	int a = parameters->param[0];
	
	if(value = a)
		return 1.0;
	else
		return 0;
} 

/*
 * Funzioni esterne 
 */

////crea lista parametri
t_paramlist* creaParametri(t_paramlist* parametriPrecedenti, int parametro)
{
	t_paramlist* parametri = parametriPrecedenti;
	
	if(parametri->paramNumber == MAX_PARAM_NUMBER) return parametri;
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
	t_mfunction mfunction = NULL;
	
	if(label == NULL || mfname == NULL) return NULL;
	for(i = 0; i < MF_NUMBER; i++)
	{
		if(strcmp(listaMF[i].mfname, mfname) == 0) 
		{
			mfunction = listaMF[i].function;
			if(parameters->paramNumber != listaMF[i].paramNumber) return NULL;
		}
	}
	if(mfunction == NULL) return NULL;
	fuzzySet->label = strdup(label);
	fuzzySet->parameters = parameters;
	fuzzySet->mfunction = mfunction;
	fuzzySet->next = testa;
	
	return fuzzySet;
}

//Cerca il fuzzy set corrispondente nella lista
int matchFuzzySet(t_fuzzyset* fuzzysetRoot, t_rule* nodo)
{
	int found = 0;
	t_fuzzyset* fuzzySet = fuzzysetRoot;
	char* label = nodo->nome;
	
	while(fuzzySet != NULL && !found)
	{
		if(strcmp(fuzzySet->label, label) == 0)
		{
			nodo->dati = fuzzySet;
			found = 1;
		}
		
		fuzzySet = fuzzySet->next;
	}

	return found;
}


//Aggiunge una variabile alla lista
void addCrispData(const char* label, int* var)
{
	struct s_crispdata* data = crispData;
	struct s_crispdata* pData = NULL; 
	
	struct s_crispdata* newData = (struct s_crispdata*) malloc(sizeof(struct s_crispdata));
	
	newData->label = strdup(label);
	newData->var = var;
	newData->next = NULL;
	
	if(crispData == NULL)
	{
		crispData = newData;
	}
	
	while(data != NULL)
	{
		pData = data;
		data = data->next;
	}
	
	pData->next = newData;
}

//Associa le variabili alle Label
int matchVariables(t_rule* nodo)
{
	int found = 0;
	char* label = nodo->nome;
	struct s_crispdata* data = crispData; 
	
	while(data != NULL && !found)
	{
		if(strcmp(data->label, label) == 0)
		{
			nodo->dati = data->var;
			found = 1;
		}
		
		data = data->next;
	}
	
	return found;
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
			return fuzzy_and(sinistro, destro);
		case OR_N:
			return fuzzy_or(sinistro, destro);
		case NOT_N:
			return fuzzy_not(destro);
		case IS_N:
			return fuzzy_is(sinistro, destro);
		default:
			return 0;
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



