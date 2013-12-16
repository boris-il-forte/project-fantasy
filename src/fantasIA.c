/*
 * Project Fantasy, gioco di strategia a turni
 *
 *
 * Copyright (C) 2011 Davide Tateo
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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "fantasy-core.h"
#include "fantasy-IA.h"


t_iadata ia[MAXGIOCATORI];

//assegna e crea le ia a giocatori random
void fantasia_assegna_ia_random(int numIA, int numG)
{
	int i;
	int r;
	char occupato[numG];
	char rt;
	for (i = 1; i < numG; i++)
		occupato[i] = 'F'; //segna le posizioni dell'array come libere
	occupato[0] = 'T'; //tranne la prima
	srand((unsigned) time(NULL));
	for (i = 0; i < numIA; i++)
	{
		do
		{
			r = rand() % numG;
		} while (occupato[r] != 'F');
		occupato[r] = 'T';
		switch (rand() % 3)
		{
			case 0:
				rt = 'a';
				break;
			case 1:
				rt = 'd';
				break;
			case 3:
				rt = 'e';
				break;
			default:
				rt = 'n';
				break;
		}
		fantasia_crea_bot(r, rt);
	}
	return;
}

//crea un nuovo bot
void fantasia_crea_bot(int num, char type)
{
	infogiocatore[num]->tipo = IA;
	infogiocatore[num]->atteggiamento = type;
	ia[num] = fantasia_inizializza_bot(type);
}

//inizializza il giocatore
t_iadata fantasia_inizializza_bot(char modo)
{
	t_iadata data;
	switch (modo)
	{
		case 'a':
			data.aggressivo = MOLTO;
			data.esploratore = NORMALE;
			data.difensore = POCO;
			break;
		case 'd':
			data.aggressivo = POCO;
			data.esploratore = NORMALE;
			data.difensore = MOLTO;
			break;
		case 'e':
			data.aggressivo = NORMALE;
			data.esploratore = MOLTO;
			data.difensore = NORMALE;
			break;
		default:
			data.aggressivo = POCO;
			data.esploratore = POCO;
			data.difensore = POCO;
			break;
	}
	data.priorA = data.aggressivo;
	data.priorD = data.difensore;
	data.priorE = data.esploratore;
	return data;
}

//gestisce un giocatore artificiale
void fantasia_usa_bot(int num)
{
	(void) num; //FIXME togliere...
	//fantasia_gtk_fineturno(); attiva fine tiurno...
}

