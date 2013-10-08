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

#ifndef FANTASY_IA_
#define FANTASY_IA_

//define utili
#define MOLTO 100
#define NORMALE 50
#define POCO 20

typedef struct s_iadata
{
	//stile di gioco del giocatore
	int aggressivo;
	int esploratore;
	int difensore;
	
	//comportamento da tenere ora
	int priorA;
	int priorD;
	int priorE;
} t_iadata;

//funzioni IA
void fantasia_assegna_ia_random(int numIA, int numG);
void fantasia_crea_bot(int num, char type);
t_iadata fantasia_inizializza_bot(char modo);
void fantasia_usa_bot(int num);

#endif
