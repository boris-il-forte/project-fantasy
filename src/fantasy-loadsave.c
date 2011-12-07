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

void ckfread(void *ptr, size_t size, size_t nitems, FILE *stream)
{
	if(fread(ptr,size,nitems,stream) < nitems) { perror("fread non riuscita"); exit(1); }
}

void ckfwrite(void *ptr, size_t size, size_t nitems, FILE *stream)
{
	if(fwrite(ptr,size,nitems,stream) < nitems) { perror("fwrite non riuscita"); exit(1); }
}

int caricaconfig(char* buf1)
{
	FILE *fp;
	char b[4];
	int i=0;

	fp=fopen("fantasy.config","r");
	if(fp == NULL) 
	{
		perror("fopen fallita");
		return 1;
	}
	while(fscanf(fp,"%c",&b[i]) && i<3)
	{
		if(b[0]=='#') while(b[0] != '\n') fscanf(fp,"%c",&b[0]);
		else if(b[i]=='\n') break;
		else i++;
	};
	b[i]='\0';
	sprintf(buf1,"%s",b);
	return 0;
}

t_lista_t * inserisci_truppe_in_coda(t_lista_t *testa,FILE *fp, char from) //# stavolta faccio notare che è meglio in questa funzione fare solo l'inserimento e non cercare la coda! inutile cercare la coda in quanto nessun'altra parte del codice usa la suddetta funzione.
{
	t_lista_t *temp, *nuova;
	nuova=malloc(sizeof(t_lista_t));
	nuova->truppa=malloc(sizeof(t_infotruppa));
	
	ckfread(&nuova->truppa->tipo,sizeof(nuova->truppa->tipo),1,fp);
	ckfread(&nuova->truppa->giocatore,sizeof(nuova->truppa->giocatore),1,fp);
	ckfread(&nuova->truppa->numero,sizeof(nuova->truppa->numero),1,fp);
	ckfread(&nuova->truppa->morale,sizeof(nuova->truppa->morale),1,fp);
	ckfread(&nuova->truppa->stanca,sizeof(nuova->truppa->stanca),1,fp);
	ckfread(&nuova->truppa->combattuto,sizeof(nuova->truppa->combattuto),1,fp);
	ckfread(&nuova->pos,sizeof(nuova->pos),1,fp);
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

	ckfread(&nuova->pos,sizeof(nuova->pos),1,fp);
	ckfread(&num_truppestruttura,sizeof(num_truppestruttura),1,fp); // intruso .. che ci vuoi fare? che nassa!
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
	ckfread(ver,1,2,fp);
	ver[2]='\0';
	ckfread(&ver[3],1,1,fp);
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
	ckfread(&cx,sizeof(cx),1,fp);
	ckfread(&cy,sizeof(cy),1,fp);
	// infomappa
	ckfread(&infomappa.castelli,sizeof(infomappa.castelli),1,fp);
	ckfread(&infomappa.fattorie,sizeof(infomappa.fattorie),1,fp);
	ckfread(&infomappa.stalle,sizeof(infomappa.stalle),1,fp);
	ckfread(&infomappa.grotte,sizeof(infomappa.grotte),1,fp);
	ckfread(&infomappa.nidi,sizeof(infomappa.nidi),1,fp);
	ckfread(&infomappa.numstalle,sizeof(infomappa.numstalle),1,fp);
	ckfread(&infomappa.numnidi,sizeof(infomappa.numnidi),1,fp);
	ckfread(&infomappa.numgrotte,sizeof(infomappa.numgrotte),1,fp);
	ckfread(&infomappa.numfattorie,sizeof(infomappa.numfattorie),1,fp);
	// END infomappa
	ckfread(&num_giocatori,sizeof(num_giocatori),1,fp);
	for(i=0;i<num_giocatori;i++) { // *giocatore
		giocatore[i]=malloc(sizeof(t_player));
		for(j=0;j<NUMSTRUTTURE; j++)
			giocatore[i]->struttura[j]=NULL;
		for(j=0;j<NUMSTRUTTURE; j++) { // **struttura
			ckfread(&num_strutture,sizeof(num_strutture),1,fp);
			for(k=0;k<num_strutture;k++) { // *struttura (scorri)
					giocatore[i]->struttura[j]=inserisci_strutture_in_coda(giocatore[i]->struttura[j],fp);
			} // END *struttura
		}
		ckfread(&num_truppe,sizeof(num_truppe),1,fp);
		giocatore[i]->truppe=NULL;
		for(j=0;j<num_truppe;j++) { // *truppe (scorri)
			giocatore[i]->truppe=inserisci_truppe_in_coda(giocatore[i]->truppe,fp,0);
		} // END *truppe
		ckfread(&giocatore[i]->oro,sizeof(giocatore[i]->oro),1,fp);
		ckfread(&giocatore[i]->cibo,sizeof(giocatore[i]->cibo),1,fp);
		ckfread(&giocatore[i]->smeraldi,sizeof(giocatore[i]->smeraldi),1,fp);
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
	ckfwrite(ver,1,3,fp);

	// Cx,Cy
	ckfwrite(&cx,sizeof(cx),1,fp);
	ckfwrite(&cy,sizeof(cy),1,fp);
	// infomappa
	ckfwrite(&infomappa.castelli,sizeof(infomappa.castelli),1,fp);
	ckfwrite(&infomappa.fattorie,sizeof(infomappa.fattorie),1,fp);
	ckfwrite(&infomappa.stalle,sizeof(infomappa.stalle),1,fp);
	ckfwrite(&infomappa.grotte,sizeof(infomappa.grotte),1,fp);
	ckfwrite(&infomappa.nidi,sizeof(infomappa.nidi),1,fp);
	ckfwrite(&infomappa.numstalle,sizeof(infomappa.numstalle),1,fp);
	ckfwrite(&infomappa.numnidi,sizeof(infomappa.numnidi),1,fp);
	ckfwrite(&infomappa.numgrotte,sizeof(infomappa.numgrotte),1,fp);
	ckfwrite(&infomappa.numfattorie,sizeof(infomappa.numfattorie),1,fp);
	// END infomappa
	for(i=0;i<MAXGIOCATORI && giocatore[i] != NULL;i++); // *giocatore (conta)
	num_giocatori=i;
	ckfwrite(&num_giocatori,sizeof(num_giocatori),1,fp);
	for(i=0;i<num_giocatori;i++) { // *giocatore (scorri)
		for(j=0;j<NUMSTRUTTURE; j++) { // **struttura
			Sptr=giocatore[i]->struttura[j];
			for(k=0; Sptr != NULL; k++) { // *struttura (conta lista)
				Sptr = Sptr->next;
			}
			num_strutture=k;
			ckfwrite(&num_strutture,sizeof(num_strutture),1,fp);
			Sptr=giocatore[i]->struttura[j];
			for(k=0;k<num_strutture;k++) { // *struttura (scorri)
				ckfwrite(&Sptr->pos,sizeof(Sptr->pos),1,fp);
				Sptrbis=Sptr->in;
				for(l=0; Sptrbis != NULL; l++) { // *in (conta lista)
					Sptrbis = Sptrbis->next;
				}
				num_truppestruttura=l;
				ckfwrite(&num_truppestruttura,sizeof(num_truppestruttura),1,fp);
				Sptrbis=Sptr->in;
				for(l=0;l<num_truppestruttura;l++) { // *in (scorri)
					ckfwrite(&Sptrbis->truppa->tipo,sizeof(Sptrbis->truppa->tipo),1,fp);
					ckfwrite(&Sptrbis->truppa->giocatore,sizeof(Sptrbis->truppa->giocatore),1,fp);
					ckfwrite(&Sptrbis->truppa->numero,sizeof(Sptrbis->truppa->numero),1,fp);
					ckfwrite(&Sptrbis->truppa->morale,sizeof(Sptrbis->truppa->morale),1,fp);
					ckfwrite(&Sptrbis->truppa->stanca,sizeof(Sptrbis->truppa->stanca),1,fp);
					ckfwrite(&Sptrbis->truppa->combattuto,sizeof(Sptrbis->truppa->combattuto),1,fp);
					ckfwrite(&Sptrbis->pos,sizeof(Sptrbis->pos),1,fp);
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
		ckfwrite(&num_truppe,sizeof(num_truppe),1,fp);
		Tptr=giocatore[i]->truppe;
		for(j=0;j<num_truppe;j++) { // *truppe (scorri)
			ckfwrite(&Tptr->truppa->tipo,sizeof(Tptr->truppa->tipo),1,fp);
			ckfwrite(&Tptr->truppa->giocatore,sizeof(Tptr->truppa->giocatore),1,fp);
			ckfwrite(&Tptr->truppa->numero,sizeof(Tptr->truppa->numero),1,fp);
			ckfwrite(&Tptr->truppa->morale,sizeof(Tptr->truppa->morale),1,fp);
			ckfwrite(&Tptr->truppa->stanca,sizeof(Tptr->truppa->stanca),1,fp);
			ckfwrite(&Tptr->truppa->combattuto,sizeof(Tptr->truppa->combattuto),1,fp);
			ckfwrite(&Tptr->pos,sizeof(Tptr->pos),1,fp);
			Tptr=Tptr->next;
		} // END *truppe
		ckfwrite(&giocatore[i]->oro,sizeof(giocatore[i]->oro),1,fp);
		ckfwrite(&giocatore[i]->cibo,sizeof(giocatore[i]->cibo),1,fp);
		ckfwrite(&giocatore[i]->smeraldi,sizeof(giocatore[i]->smeraldi),1,fp);
	} // END *giocatore
		
	fclose(fp);

	return 0;
}
