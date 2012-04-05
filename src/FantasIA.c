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

#include <pthread.h>
#include "fantasy-core.h"
#include "fantasy-IA.h"

typedef struct s_iaparam
{
	int num;
	char mod;
} t_iaparam;

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


//assegna e crea le ia a giocatori random
void fantasia_assegna_ia_random(int numIA, int numG)
{
	return;
}

//crea un thead per il nuovo bot
void fantasia_create_player(int num, char type, *thread)
{
	t_iaparam *param;
	param=malloc
	param->num=num;
	param->mod=type;
	pthread_create(thread, NULL, fantasia_giocatore_artificiale, );
}

//inizializza il giocatore
t_iadata fantasia_giocatore_inizializza(char modo)
{
	t_iadata data;
	switch(modo)
	{
		case 'a':
			data.aggressivo=MOLTO;
			data.esploratore=NORMALE;
			data.difensore=POCO;
			break;
		case 'd':
			data.aggressivo=POCO;
			data.esploratore=NORMALE;
			data.difensore=MOLTO;
			break;
		case 'e':
			data.aggressivo=NORMALE;
			data.esploratore=MOLTO;
			data.difensore=NORMALE;
			break;
		default:
			data.aggressivo=POCO;
			data.esploratore=POCO;
			data.difensore=POCO;
			break;
	}
	data.priorA=data.aggressivo;
	data.priorD=data.difensore;
	data.priorE=data.esploratore;
	return data;
}


//gestisce un giocatore artificiale
void fantasia_giocatore_artificiale(t_iaparam *param)
{
	//dati IA
	s_iadata datiIa;
	int numeroGiocatore;
	
	//inizializza IA
	datiIa=fantasia_giocatore_inizializza(param.mod);
	
	//main loop IA
	while(controllosconfitto(numeroGiocatore)==0)
	{
		if(numeroGiocatore==CurrentPlayer)
		{
			fineturno();
		}
		sleep(1000);
	}
}


















