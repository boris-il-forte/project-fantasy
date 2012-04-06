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
#include "fantasy-IA-gtk.h"

//mutex indispensabili
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//assegna e crea le ia a giocatori random
void fantasia_assegna_ia_random(int numIA, int numG)
{
	int i;
	int r;
	char occupato[numG];
	char rt;
	for (i=1; i<numG; i++) occupato[i]='F'; //segna le posizioni dell'array come libere
	occupato[0]='T'; //tranne la prima
	srand((unsigned)time(NULL));
	for(i=0; i<numIA; i++)
	{
		do
		{
			r=rand()%numG;
		}while(occupato[r]!='F');
		occupato[r]='T';
		switch(rand()%3)
		{
			case 0:
				rt='a';
				break;
			case 1:
				rt='d';
				break;
			case 3:
				rt='e';
				break;
			default:
				rt='n';
				break;
		}
		fantasia_create_player(r,rt);
	}
	return;
}

//crea un thead per il nuovo bot
void fantasia_create_player(int num, char type)
{
	pthread_t thread;
	t_iaparam *param;
	param=(t_iaparam*) malloc(sizeof(t_iaparam));
	param->num=num;
	param->mod=type;
	pthread_create(&thread, NULL, fantasia_giocatore_artificiale, param);
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
void *fantasia_giocatore_artificiale(void *P)
{
	//casting del puntatore in ingresso
	t_iaparam *param= (t_iaparam*) P;
	//dati IA
	t_iadata datiIa;
	int numeroGiocatore;
	
	//inizializza IA
	datiIa=fantasia_giocatore_inizializza(param->mod);
	numeroGiocatore=param->num;
	
	//main loop IA
	while(controllosconfitto(numeroGiocatore)==0)
	{
		if(numeroGiocatore==CurrentPlayer)
		{
			printf("sono il giocatore n: %d e sto per chiamare fineturno\n", param->num+1);
			pthread_mutex_lock(&mutex);
			//fantasia_gtk_fineturno(); //non funziona. chissà perchè!
			pthread_mutex_unlock(&mutex);
		}
		printf("sono il giocatore n: %d\n", param->num+1);
		sleep(1);
	}
	free(param);
	exit(0);
}















