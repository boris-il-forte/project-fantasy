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

t_lista_t * inserisci_truppe_in_coda(t_lista_t *testa,FILE *fp, char from) //# stavolta faccio notare che è meglio in questa funzione fare solo l'inserimento e non cercare la coda! inutile cercare la coda in quanto nessun'altra parte del codice usa la suddetta funzione.
{
	t_lista_t *temp, *nuova;
	nuova=malloc(sizeof(t_lista_t));
	nuova->truppa=malloc(sizeof(t_infotruppa));
	
	fread(&nuova->truppa->tipo,sizeof(nuova->truppa->tipo),1,fp);
	fread(&nuova->truppa->giocatore,sizeof(nuova->truppa->giocatore),1,fp);
	fread(&nuova->truppa->numero,sizeof(nuova->truppa->numero),1,fp);
	fread(&nuova->truppa->morale,sizeof(nuova->truppa->morale),1,fp);
	fread(&nuova->truppa->stanca,sizeof(nuova->truppa->stanca),1,fp);
	fread(&nuova->truppa->combattuto,sizeof(nuova->truppa->combattuto),1,fp);
	fread(&nuova->pos,sizeof(nuova->pos),1,fp);
	if(from == 0) {// 0 = esterno; 1 = struttura
		infomappa.truppe[nuova->pos]=nuova->truppa; // assegnazione "di comodo"
	}
	nuova->next=NULL;

	temp=testa;

	if (temp == NULL)
		return nuova;
	while(temp->next != NULL) {
		temp=temp->next;
	}
	temp->next=nuova;

	return testa;
}

t_lista_s * inserisci_strutture_in_coda(t_lista_s *testa,FILE *fp)
{
	int num_truppestruttura;
	int l;

	t_lista_s *temp, *nuova;
	nuova=malloc(sizeof(t_lista_s));

	fread(&nuova->pos,sizeof(nuova->pos),1,fp);
	fread(&num_truppestruttura,sizeof(num_truppestruttura),1,fp); // intruso .. che ci vuoi fare? che nassa!
	nuova->in=NULL;
	for(l=0;l<num_truppestruttura;l++) { // *in (scorri)
		nuova->in=inserisci_truppe_in_coda(nuova->in,fp,1);
	} // END *in
	nuova->next=NULL;

	temp=testa;

	if (temp == NULL)
		return nuova;
	while(temp->next != NULL) {
		temp=temp->next;
	}
	temp->next=nuova;

	return testa;
}

int carica(char *nomefile)
{
	FILE *fp;
	char ver[4];
	int i,j,k;
	int num_giocatori,num_strutture,num_truppe;

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

	// Cx,Cy
	fread(&cx,sizeof(cx),1,fp);
	fread(&cy,sizeof(cy),1,fp);
	// infomappa
	// infomappa
	fread(&infomappa.castelli,sizeof(infomappa.castelli),NUMCASTELLI,fp);
	fread(&infomappa.fattorie,sizeof(infomappa.fattorie),MAXFATTORIE,fp);
	fread(&infomappa.stalle,sizeof(infomappa.stalle),MAXSTALLE,fp);
	fread(&infomappa.grotte,sizeof(infomappa.grotte),MAXGROTTE,fp);
	fread(&infomappa.nidi,sizeof(infomappa.nidi),MAXNIDI,fp);
	fread(&infomappa.numstalle,sizeof(infomappa.numstalle),1,fp);
	fread(&infomappa.numnidi,sizeof(infomappa.numnidi),1,fp);
	fread(&infomappa.numgrotte,sizeof(infomappa.numgrotte),1,fp);
	fread(&infomappa.numfattorie,sizeof(infomappa.numfattorie),1,fp);
	rigeneramappa();
	// END infomappa
	fread(&num_giocatori,sizeof(num_giocatori),1,fp);
	for(i=0;i<num_giocatori;i++) { // *giocatore
		giocatore[i]=malloc(sizeof(t_player));
		for(j=0;j<NUMSTRUTTURE; j++)
			giocatore[i]->struttura[j]=NULL;
		for(j=0;j<NUMSTRUTTURE; j++) { // **struttura
			fread(&num_strutture,sizeof(num_strutture),1,fp);
			for(k=0;k<num_strutture;k++) { // *struttura (scorri)
					giocatore[i]->struttura[j]=inserisci_strutture_in_coda(giocatore[i]->struttura[j],fp);
			} // END *struttura
		}
		fread(&num_truppe,sizeof(num_truppe),1,fp);
		giocatore[i]->truppe=NULL;
		for(j=0;j<num_truppe;j++) { // *truppe (scorri)
			giocatore[i]->truppe=inserisci_truppe_in_coda(giocatore[i]->truppe,fp,0);
		} // END *truppe
		fread(&giocatore[i]->oro,sizeof(giocatore[i]->oro),1,fp);
		fread(&giocatore[i]->cibo,sizeof(giocatore[i]->cibo),1,fp);
		fread(&giocatore[i]->smeraldi,sizeof(giocatore[i]->smeraldi),1,fp);
	} // END *giocatore
	while(i<MAXGIOCATORI) {
		giocatore[i]=NULL;
		i++;
	}
	fclose(fp);

	return 0;
}

int salva(char *nomefile)
{
	FILE *fp;
	char ver[3];
	t_lista_t *Tptr,*Sptrbis;
	t_lista_s *Sptr;

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

	// Cx,Cy
	fwrite(&cx,sizeof(cx),1,fp);
	fwrite(&cy,sizeof(cy),1,fp);
	// infomappa
	fwrite(&infomappa.castelli,sizeof(infomappa.castelli),NUMCASTELLI,fp); //# errore! deve caricare solo infomappa.numfattorie e non MAXFATTORIE. prima bisogna salvare i valori infomappa.numfattorie (etc..) e poi caricare le varie posizioni.
	fwrite(&infomappa.fattorie,sizeof(infomappa.fattorie),MAXFATTORIE,fp);
	fwrite(&infomappa.stalle,sizeof(infomappa.stalle),MAXSTALLE,fp);
	fwrite(&infomappa.grotte,sizeof(infomappa.grotte),MAXGROTTE,fp);
	fwrite(&infomappa.nidi,sizeof(infomappa.nidi),MAXNIDI,fp);
	fwrite(&infomappa.numstalle,sizeof(infomappa.numstalle),1,fp);
	fwrite(&infomappa.numnidi,sizeof(infomappa.numnidi),1,fp);
	fwrite(&infomappa.numgrotte,sizeof(infomappa.numgrotte),1,fp);
	fwrite(&infomappa.numfattorie,sizeof(infomappa.numfattorie),1,fp);
	// END infomappa
	for(i=0;i<MAXGIOCATORI && giocatore[i] != NULL;i++); // *giocatore (conta)
	num_giocatori=i;
	fwrite(&num_giocatori,sizeof(num_giocatori),1,fp);
	for(i=0;i<num_giocatori;i++) { // *giocatore (scorri)
		for(j=0;j<NUMSTRUTTURE; j++) { // **struttura
			Sptr=giocatore[i]->struttura[j];
			for(k=0; Sptr != NULL; k++) { // *struttura (conta lista)
				Sptr = Sptr->next;
			}
			num_strutture=k;
			fwrite(&num_strutture,sizeof(num_strutture),1,fp);
			Sptr=giocatore[i]->struttura[j];
			for(k=0;k<num_strutture;k++) { // *struttura (scorri)
				fwrite(&Sptr->pos,sizeof(Sptr->pos),1,fp);
				Sptrbis=Sptr->in;
				for(l=0; Sptrbis != NULL; l++) { // *in (conta lista)
					Sptrbis = Sptrbis->next;
				}
				num_truppestruttura=l;
				fwrite(&num_truppestruttura,sizeof(num_truppestruttura),1,fp);
				Sptrbis=Sptr->in;
				for(l=0;l<num_truppestruttura;l++) { // *in (scorri)
					fwrite(&Sptrbis->truppa->tipo,sizeof(Sptrbis->truppa->tipo),1,fp);
					fwrite(&Sptrbis->truppa->giocatore,sizeof(Sptrbis->truppa->giocatore),1,fp);
					fwrite(&Sptrbis->truppa->numero,sizeof(Sptrbis->truppa->numero),1,fp);
					fwrite(&Sptrbis->truppa->morale,sizeof(Sptrbis->truppa->morale),1,fp);
					fwrite(&Sptrbis->truppa->stanca,sizeof(Sptrbis->truppa->stanca),1,fp);
					fwrite(&Sptrbis->truppa->combattuto,sizeof(Sptrbis->truppa->combattuto),1,fp);
					fwrite(&Sptrbis->pos,sizeof(Sptrbis->pos),1,fp);
					Sptrbis=Sptrbis->next; 
				} // END *in
				Sptr = Sptr->next;
			} // END *struttura
		} // END **struttura
		Tptr=giocatore[i]->truppe;
		for(j=0; Tptr != NULL; j++) { // *truppe (conta lista)
			Tptr = Tptr->next;
		}
		num_truppe=j;
		fwrite(&num_truppe,sizeof(num_truppe),1,fp);
		Tptr=giocatore[i]->truppe;
		for(j=0;j<num_truppe;j++) { // *truppe (scorri)
			fwrite(&Tptr->truppa->tipo,sizeof(Tptr->truppa->tipo),1,fp);
			fwrite(&Tptr->truppa->giocatore,sizeof(Tptr->truppa->giocatore),1,fp);
			fwrite(&Tptr->truppa->numero,sizeof(Tptr->truppa->numero),1,fp);
			fwrite(&Tptr->truppa->morale,sizeof(Tptr->truppa->morale),1,fp);
			fwrite(&Tptr->truppa->stanca,sizeof(Tptr->truppa->stanca),1,fp);
			fwrite(&Tptr->truppa->combattuto,sizeof(Tptr->truppa->combattuto),1,fp);
			fwrite(&Tptr->pos,sizeof(Tptr->pos),1,fp);
			Tptr=Tptr->next;
		} // END *truppe
		fwrite(&giocatore[i]->oro,sizeof(giocatore[i]->oro),1,fp);
		fwrite(&giocatore[i]->cibo,sizeof(giocatore[i]->cibo),1,fp);
		fwrite(&giocatore[i]->smeraldi,sizeof(giocatore[i]->smeraldi),1,fp);
	} // END *giocatore
		
	fclose(fp);

	return 0;
}
