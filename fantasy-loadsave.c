/*
 * Project Fantasy, gioco di strategia a turni
 *
 *
 * Copyright (C) 2011 gionnico
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
#include <string.h>
#include "fantasy-core.h"

int carica(char *nomefile)
{
	FILE *fp;
	char ver[4];
	t_lista_t *Tp=NULL;

	fp=fopen(nomefile,"r");
	if(fp == NULL) {
		perror("fopen fallita");
		return 1;
	}
	// INIZIO controllo header standard: FC1 (1 come intero)
	fread(ver,1,2,fp);
	ver[2]='\0';
	fread(&ver[3],1,1,fp);
	if(strcmp(ver,"FC") != 0) {
		fprintf(stderr,"MIME type sconosciuto. Intestazione errata: %c%c\n",ver[0],ver[1]);
		return 1;
	}
	if(ver[3] != 1) {
		fprintf(stderr,"Formato FantasyCore sconosciuto (supportato: v1). Versione: v%u ?\n",ver[2]);
		return 1;
	}
	// FINE controllo header standard

	// infomappa
	fread((&infomappa)->castelli,sizeof(infomappa.castelli),NUMCASTELLI,fp);
	fread((&infomappa)->fattorie,sizeof(infomappa.fattorie),MAXFATTORIE,fp);
	fread((&infomappa)->stalle,sizeof(infomappa.stalle),MAXSTALLE,fp);
	fread((&infomappa)->grotte,sizeof(infomappa.grotte),MAXGROTTE,fp);
	fread((&infomappa)->nidi,sizeof(infomappa.nidi),MAXNIDI,fp);
	fread(&(&infomappa)->numstalle,sizeof(infomappa.numstalle),1,fp);
	fread(&(&infomappa)->numnidi,sizeof(infomappa.numnidi),1,fp);
	fread(&(&infomappa)->numgrotte,sizeof(infomappa.numgrotte),1,fp);
	fread(&(&infomappa)->numfattorie,sizeof(infomappa.numfattorie),1,fp);
	// END infomappa

	fclose(fp);

	return 0;
}

int salva(char *nomefile)
{
	FILE *fp;
	char ver[3];

	int i,j,k,l;
	int num_giocatori,num_strutture,num_truppestruttura,num_truppe;

	ver[0]='F';
	ver[1]='C';
	ver[2]=1;

	fp=fopen(nomefile,"w");
	if(fp == NULL) {
		perror("fopen fallita");
		return 1;
	}
	fwrite(ver,1,3,fp);

	// infomappa
	fwrite((&infomappa)->castelli,sizeof(infomappa.castelli),NUMCASTELLI,fp);
	fwrite((&infomappa)->fattorie,sizeof(infomappa.fattorie),MAXFATTORIE,fp);
	fwrite((&infomappa)->stalle,sizeof(infomappa.stalle),MAXSTALLE,fp);
	fwrite((&infomappa)->grotte,sizeof(infomappa.grotte),MAXGROTTE,fp);
	fwrite((&infomappa)->nidi,sizeof(infomappa.nidi),MAXNIDI,fp);
	fwrite(&(&infomappa)->numstalle,sizeof(infomappa.numstalle),1,fp);
	fwrite(&(&infomappa)->numnidi,sizeof(infomappa.numnidi),1,fp);
	fwrite(&(&infomappa)->numgrotte,sizeof(infomappa.numgrotte),1,fp);
	fwrite(&(&infomappa)->numfattorie,sizeof(infomappa.numfattorie),1,fp);
	// END infomappa
	for(i=0;i<MAXGIOCATORI && giocatore[i] != NULL;i++); // *giocatore (conta)
	num_giocatori=i-1;
	fwrite(&num_giocatori,sizeof(num_giocatori),1,fp);
	for(i=0;i<num_giocatori;i++) { // *giocatore (scorri)
		for(j=0;j<NUMSTRUTTURE; j++) { // **struttura
			for(k=0; giocatore[i]->struttura[j] != NULL; k++) { // *struttura (conta lista)
				giocatore[i]->struttura[j] = giocatore[i]->struttura[j]->next;
			}
			num_strutture=k-1;
			fwrite(&num_strutture,sizeof(num_strutture),1,fp);
			for(k=0;k<num_strutture;k++) { // *struttura (scorri)
				fwrite(&giocatore[i]->struttura[j]->pos,sizeof(giocatore[i]->struttura[j]->pos),1,fp);
				for(l=0; giocatore[i]->struttura[j]->in != NULL; l++) { // *in (conta lista)
					giocatore[i]->struttura[j]->in = giocatore[i]->struttura[j]->in->next;
				}
				num_truppestruttura=l-1;
				fwrite(&num_truppestruttura,sizeof(num_truppestruttura),1,fp);
				for(l=0;l<num_truppestruttura;l++) { // *in (scorri)
					fwrite(&giocatore[i]->struttura[j]->in->truppa->tipo,sizeof(giocatore[i]->struttura[j]->in->truppa->tipo),1,fp);
					fwrite(&giocatore[i]->struttura[j]->in->truppa->giocatore,sizeof(giocatore[i]->struttura[j]->in->truppa->giocatore),1,fp);
					fwrite(&giocatore[i]->struttura[j]->in->truppa->numero,sizeof(giocatore[i]->struttura[j]->in->truppa->numero),1,fp);
					fwrite(&giocatore[i]->struttura[j]->in->truppa->morale,sizeof(giocatore[i]->struttura[j]->in->truppa->morale),1,fp);
					fwrite(&giocatore[i]->struttura[j]->in->truppa->stanca,sizeof(giocatore[i]->struttura[j]->in->truppa->stanca),1,fp);
					fwrite(&giocatore[i]->struttura[j]->in->truppa->combattuto,sizeof(giocatore[i]->struttura[j]->in->truppa->combattuto),1,fp);
					fwrite(&giocatore[i]->struttura[j]->in->pos,sizeof(giocatore[i]->struttura[j]->in->pos),1,fp);
					giocatore[i]->struttura[j]->in=giocatore[i]->struttura[j]->in->next;
				} // END *in
				giocatore[i]->struttura[j] = giocatore[i]->struttura[j]->next;
			} // END *struttura
		} // END **struttura
		for(j=0; giocatore[i]->truppe != NULL; j++) { // *truppe (conta lista)
			giocatore[i]->truppe = giocatore[i]->truppe->next;
		}
		num_truppe=j-1;
		fwrite(&num_truppe,sizeof(num_truppe),1,fp);
		for(j=0;j<num_truppe;j++) { // *truppe (scorri)
			fwrite(&giocatore[i]->truppe->truppa->tipo,sizeof(giocatore[i]->truppe->truppa->tipo),1,fp);
			fwrite(&giocatore[i]->truppe->truppa->giocatore,sizeof(giocatore[i]->truppe->truppa->giocatore),1,fp);
			fwrite(&giocatore[i]->truppe->truppa->numero,sizeof(giocatore[i]->truppe->truppa->numero),1,fp);
			fwrite(&giocatore[i]->truppe->truppa->morale,sizeof(giocatore[i]->truppe->truppa->morale),1,fp);
			fwrite(&giocatore[i]->truppe->truppa->stanca,sizeof(giocatore[i]->truppe->truppa->stanca),1,fp);
			fwrite(&giocatore[i]->truppe->truppa->combattuto,sizeof(giocatore[i]->truppe->truppa->combattuto),1,fp);
			fwrite(&giocatore[i]->truppe->pos,sizeof(giocatore[i]->truppe->pos),1,fp);
			giocatore[i]->truppe=giocatore[i]->truppe->next;
		} // END *truppe
		fwrite(&giocatore[i]->oro,sizeof(giocatore[i]->oro),1,fp);
		fwrite(&giocatore[i]->cibo,sizeof(giocatore[i]->cibo),1,fp);
		fwrite(&giocatore[i]->smeraldi,sizeof(giocatore[i]->smeraldi),1,fp);
	} // END *giocatore
		
	fclose(fp);

	return 0;
}

t_lista_t* carica_lista_strutture()
{
	int i;
	t_lista_t* Head=NULL;
	t_lista_t* T=NULL;
	for(i=0; i<1;i++)	//aggiusta il ciclo con la condizione giusta
	{
		if (T!=NULL)
		{
			T->next=(t_lista_t*)malloc(sizeof (t_lista_t));
			T=T->next;
		}
		else 
		{
			T=(t_lista_t*)malloc(sizeof (t_lista_t));
			Head=T;
		}
		T->in=NULL;	//qua ci va una funzione analoga che attacchi le truppe se ci sono.
		T->pos=0;	//qua ci va la pos corretta
		T->next=NULL;
	}
	return Head;
}
