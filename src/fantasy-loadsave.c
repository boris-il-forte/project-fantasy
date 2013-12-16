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
#include <dirent.h>
#include "fantasy-core.h"

#define DBG(s) fprintf(stderr, s); fprintf(stderr, "\n");
#define DBGI(s,i) fprintf(stderr, s, i+1); fprintf(stderr, "\n");

#define BUFSIZE 64

/*
 * Funzioni di utilità
 *
 */

void ckfread(void *ptr, size_t size, FILE *stream)
{
	if (fread(ptr, size, 1, stream) < 1)
	{
		perror("fread non riuscita");
		exit(1);
	}
}

void ckfwrite(void *ptr, size_t size, FILE *stream)
{
	if (fwrite(ptr, size, 1, stream) < 1)
	{
		perror("fwrite non riuscita");
		exit(1);
	}
}

void scriviStringa(char *stringa, FILE *stream)
{
	int sizeOfString = strlen(stringa) + 1;
	ckfwrite(&sizeOfString, sizeof(unsigned short), stream);

	ckfwrite(stringa, sizeof(char) * sizeOfString, stream);
}

void leggiStringa(char *buffer, FILE *stream)
{
	int stringLength = 0;
	fread(&stringLength, sizeof(unsigned short), 1, stream);

	ckfread(buffer, sizeof(char) * stringLength, stream);
}

/*
 * Funzioni configurazione
 *
 */

int listaskin(char *nomedir, char *sottodir[])
{
	int i;
	int j = 0;
	DIR *dir;
	struct dirent *dp;

	if ((dir = opendir(nomedir)) == NULL)
	{
		perror("Cannot open skin dir");
		exit(1);
	}

	for (i = 0; (dp = readdir(dir)) != NULL && i < 64; i++)
	{ // FIXME magic64
		if ((dp->d_type & DT_DIR) && strcmp(dp->d_name, ".") != 0
				&& strcmp(dp->d_name, "..") != 0)
		{
			sottodir[j] = malloc(sizeof(char) * (strlen(dp->d_name) + 1));
			snprintf(sottodir[j], strlen(dp->d_name) + 1, "%s", dp->d_name);
			j++;
		}
		//free(dp);
	}
	closedir(dir);
	if (i == 64)
		printf("listaskin: troncata a %d per evitare buffer overflow.\n", 64); // magic64

	return j;
}

int caricaconfig(char *nomefile)
{
	FILE *fp;
	char Buf[BUFSIZE];
	char *Buf2; // ritorno strtok. controlla memleaks

	fp = fopen(nomefile, "r");
	if (fp == NULL)
	{
		sprintf(infogioco.skin, "Il_RatMan");
		sprintf(infogioco.ext, "png");
		salvaconfig(nomefile);
		return 0;
	}
	while (fgets(Buf, BUFSIZE, fp) != NULL)
	{
		if (Buf[0] != '#')
		{
			Buf2 = strtok(Buf, " "); // accetta commenti inline dopo uno spazio.
			Buf2 = strtok(Buf, "#"); // .. o dopo un cancelletto anche attaccato.
			if (Buf[strlen(Buf) - 1] == '\n')
				Buf[strlen(Buf) - 1] = '\0';
			printf("Buf2 vale \"%s\"\n", Buf2);
			Buf2 = strtok(Buf, "=");
			if (strcmp(Buf2, "skin") == 0)
			{
				Buf2 = strtok(NULL, "="); // memleak sicuro.
				sprintf(infogioco.skin, "%s", Buf2);
			}
			else
			{
				fprintf(stderr, "Parametro sconosciuto: \"%s\"\n", Buf2);
				exit(1); // enforce strict config file
			}
		}
	}
	// secondo giro
	sprintf(Buf, "skin/");
	strcat(Buf, infogioco.skin);
	strcat(Buf, "/skin.ini");
	fp = fopen(Buf, "r");
	if (fp == NULL)
	{
		perror("not a valid skin");
		return 1;
	}
	while (fgets(Buf, BUFSIZE, fp) != NULL)
	{
		if (Buf[0] != ';')
		{
			Buf2 = strtok(Buf, " "); // accetta commenti inline dopo uno spazio.
			Buf2 = strtok(Buf, ";"); // .. o dopo un puntoevirgola anche attaccato.
			if (Buf[strlen(Buf) - 1] == '\n')
				Buf[strlen(Buf) - 1] = '\0';
			printf("Buf2 vale \"%s\"\n", Buf2);
			Buf2 = strtok(Buf, "=");
			if (strcmp(Buf2, "ext") == 0)
			{
				Buf2 = strtok(NULL, "="); // memleak sicuro.
				sprintf(infogioco.ext, "%s", Buf2);
			}
			else
			{
				fprintf(stderr, "Parametro sconosciuto: \"%s\"\n", Buf2);
				exit(1); // enforce strict ini file
			}
		}
	}

	fclose(fp);

	return 0;
}

int salvaconfig(char *nomefile)
{
	FILE *fp;

	fp = fopen(nomefile, "w");
	if (fp == NULL)
	{
		perror("fopen write fallita");
		exit(1); // da sostituire con return appropriatamente gestita
	}
	fputs("skin=", fp); // controlla ritorno
	fputs(infogioco.skin, fp);
	fputs("\n", fp);

	fclose(fp);

	return 0;
}

/*
 * Funzioni per caricare
 *
 */

int headerOK(FILE *fp)
{
	char ver[4];
	ckfread(ver, 2 * sizeof(char), fp);
	ver[2] = '\0';
	ckfread(&ver[3], 1, fp);
	if (strcmp(ver, "FC") != 0)
	{
		fprintf(stderr, "MIME type sconosciuto. Intestazione errata: %c%c\n",
				ver[0], ver[1]);
		return 0;
	}

	if (ver[3] != 1)
	{
		fprintf(stderr,
				"Formato FantasyCore sconosciuto(supportato: v1). Versione: v%u ?\n",
				ver[3]);
		return 0;
	}

	return 1;
}

void caricaInfoStato(FILE* fp, int *inGioco)
{
	// Cx,Cy, CurrentPlayer
	ckfread(&cx, sizeof(cx), fp);
	ckfread(&cy, sizeof(cy), fp);
	ckfread(&CurrentPlayer, sizeof(CurrentPlayer), fp);
	//carica la maschera dei giocatori attivi
	ckfread(inGioco, MAXGIOCATORI * sizeof(int), fp);
}

void caricaMappa(FILE* fp)
{
	ckfread(infomappa.prato, sizeof(infomappa.prato), fp);
	ckfread(infomappa.castelli, sizeof(infomappa.castelli), fp);
	ckfread(infomappa.fattorie, sizeof(infomappa.fattorie), fp);
	ckfread(infomappa.stalle, sizeof(infomappa.stalle), fp);
	ckfread(infomappa.grotte, sizeof(infomappa.grotte), fp);
	ckfread(infomappa.nidi, sizeof(infomappa.nidi), fp);
	ckfread(&infomappa.numstalle, sizeof(infomappa.numstalle), fp);
	ckfread(&infomappa.numnidi, sizeof(infomappa.numnidi), fp);
	ckfread(&infomappa.numgrotte, sizeof(infomappa.numgrotte), fp);
	ckfread(&infomappa.numfattorie, sizeof(infomappa.numfattorie), fp);
}

void caricaInfoGiocatore(int *inGioco, FILE* fp)
{
	int i;

	for (i = 0; i < MAXGIOCATORI; i++)
	{
		if (inGioco[i])
		{
			infogiocatore[i] = (t_infoplayer*) malloc(sizeof(t_infoplayer*));
			ckfread(&infogiocatore[i]->atteggiamento,
					sizeof(infogiocatore[i]->atteggiamento), fp);
			leggiStringa(infogiocatore[i]->nome, fp);
			ckfread(&infogiocatore[i]->tipo, sizeof(infogiocatore[i]->tipo),
					fp);
		}
		else
		{
			infogiocatore[i] = NULL;
		}
	}

}

t_lista_t * caricaTruppa(FILE *fp, char from)
{
	t_lista_t *nuova;
	nuova = malloc(sizeof(t_lista_t));
	nuova->truppa = malloc(sizeof(t_infotruppa));

	ckfread(&nuova->truppa->tipo, sizeof(nuova->truppa->tipo), fp);
	ckfread(&nuova->truppa->giocatore, sizeof(nuova->truppa->giocatore), fp);
	ckfread(&nuova->truppa->numero, sizeof(nuova->truppa->numero), fp);
	ckfread(&nuova->truppa->morale, sizeof(nuova->truppa->morale), fp);
	ckfread(&nuova->truppa->stanca, sizeof(nuova->truppa->stanca), fp);
	ckfread(&nuova->truppa->combattuto, sizeof(nuova->truppa->combattuto), fp);
	ckfread(&nuova->pos, sizeof(nuova->pos), fp);
	if (from == 0)
	{ // 0 = esterno; 1 = struttura
		infomappa.truppe[nuova->pos] = nuova->truppa; // assegnazione "di comodo"
	}

	nuova->next = NULL;

	return nuova;
}

t_lista_s * caricaStruttura(FILE *fp)
{
	int num_truppestruttura;
	int i;

	t_lista_t *truppa;
	t_lista_s *nuova = malloc(sizeof(t_lista_s));

	//carica posizione della struttura
	ckfread(&nuova->pos, sizeof(nuova->pos), fp);

	//carica truppe nella struttura
	ckfread(&num_truppestruttura, sizeof(num_truppestruttura), fp);

	nuova->in = NULL;

	for (i = 0; i < num_truppestruttura; i++)
	{
		if (i == 0)
		{
			nuova->in = caricaTruppa(fp, 1);
			truppa = nuova->in;
		}
		else
		{
			truppa->next = caricaTruppa(fp, 1);
			truppa = truppa->next;
		}
	}

	nuova->next = NULL;

	return nuova;

}

t_player* caricaGiocatore(FILE* fp)
{
	int i, j;
	int num_strutture, num_truppe;
	t_lista_t* truppa;
	t_lista_s* struttura;
	t_player* giocatore = malloc(sizeof(t_player));

	//Carica le strutture del giocatore
	for (i = 0; i < NUMSTRUTTURE; i++)
		giocatore->struttura[i] = NULL;

	for (i = 0; i < NUMSTRUTTURE; i++)
	{
		ckfread(&num_strutture, sizeof(num_strutture), fp);
		for (j = 0; j < num_strutture; j++)
		{
			if (j == 0)
			{
				giocatore->struttura[i] = caricaStruttura(fp);
				struttura = giocatore->struttura[i];
			}
			else
			{
				struttura->next = caricaStruttura(fp);
				struttura = struttura->next;
			}
		}
	}

	//Carica le truppe del giocatore
	ckfread(&num_truppe, sizeof(num_truppe), fp);
	giocatore->truppe = NULL;
	for (i = 0; i < num_truppe; i++)
	{
		if (i == 0)
		{
			giocatore->truppe = caricaTruppa(fp, 0);
			truppa = giocatore->truppe;
		}
		else
		{
			truppa->next = caricaTruppa(fp, 0);
			truppa = truppa->next;
		}

	}

	//carica oro, cibo e smeraldi
	ckfread(&giocatore->oro, sizeof(giocatore->oro), fp);
	ckfread(&giocatore->cibo, sizeof(giocatore->cibo), fp);
	ckfread(&giocatore->smeraldi, sizeof(giocatore->smeraldi), fp);

	return giocatore;
}

void caricaGiocatori(int *inGioco, FILE* fp)
{
	int i;

	for (i = 0; i < MAXGIOCATORI; i++)
	{
		if (inGioco[i])
		{
			giocatore[i] = caricaGiocatore(fp);
		}
		else
		{
			giocatore[i] = NULL;
		}
	}
}

int carica(char *nomefile)
{
	FILE *fp;
	int inGioco[MAXGIOCATORI];

	//apri file
	fp = fopen(nomefile, "r");
	if (fp == NULL)
	{
		perror("fopen fallita");
		return 1;
	}

	//Controllo header standard: FC1(1 come intero)
	if (!headerOK(fp))
		return 1;
	//Reset e inizializzazioni
	caricadati();
	inizializza();
	//Carica le informazioni genersli dello stato del gioco
	caricaInfoStato(fp, inGioco);
	//Carica le informazioni della mappa
	caricaMappa(fp);
	//Carica le informazioni generali sui giocatori
	caricaInfoGiocatore(inGioco, fp);
	//Carica i giocatori
	caricaGiocatori(inGioco, fp);
	//chiudi il file
	fclose(fp);
	//ricrea la mappa
	rigeneramappa();

	return 0;
}

/*
 * Funzioni per salvare
 *
 */

void scriviHeader(FILE* fp)
{
	char ver[3];
	ver[0] = 'F';
	ver[1] = 'C';
	ver[2] = 1;
	ckfwrite(ver, sizeof(ver), fp);
}

void salvaInfoStato(FILE* fp)
{
	int vivo = 1;
	int morto = 0;
	int i;

	// Cx,Cy, CurrentPlayer
	ckfwrite(&cx, sizeof(cx), fp);
	ckfwrite(&cy, sizeof(cy), fp);
	ckfwrite(&CurrentPlayer, sizeof(CurrentPlayer), fp);

	for (i = 0; i < MAXGIOCATORI; i++)
	{
		if (giocatore[i] == NULL)
			ckfwrite(&morto, sizeof(morto), fp);
		else
			ckfwrite(&vivo, sizeof(morto), fp);
	}
}

void salvaMappa(FILE* fp)
{
	ckfwrite(infomappa.prato, sizeof(infomappa.prato), fp);
	ckfwrite(infomappa.castelli, sizeof(infomappa.castelli), fp);
	ckfwrite(infomappa.fattorie, sizeof(infomappa.fattorie), fp);
	ckfwrite(infomappa.stalle, sizeof(infomappa.stalle), fp);
	ckfwrite(infomappa.grotte, sizeof(infomappa.grotte), fp);
	ckfwrite(infomappa.nidi, sizeof(infomappa.nidi), fp);
	ckfwrite(&infomappa.numstalle, sizeof(infomappa.numstalle), fp);
	ckfwrite(&infomappa.numnidi, sizeof(infomappa.numnidi), fp);
	ckfwrite(&infomappa.numgrotte, sizeof(infomappa.numgrotte), fp);
	ckfwrite(&infomappa.numfattorie, sizeof(infomappa.numfattorie), fp);
}

void salvaInfoGiocatore(FILE* fp)
{
	int i;

	for (i = 0; i < MAXGIOCATORI; i++)
	{
		if (giocatore[i] != NULL)
		{
			ckfwrite(&infogiocatore[i]->atteggiamento,
					sizeof(infogiocatore[i]->atteggiamento), fp);
			scriviStringa(infogiocatore[i]->nome, fp);
			ckfwrite(&infogiocatore[i]->tipo, sizeof(infogiocatore[i]->tipo),
					fp);
		}
	}

}

void salvaTruppa(t_infotruppa* truppa, int pos, FILE* fp)
{
	ckfwrite(&truppa->tipo, sizeof(truppa->tipo), fp);
	ckfwrite(&truppa->giocatore, sizeof(truppa->giocatore), fp);
	ckfwrite(&truppa->numero, sizeof(truppa->numero), fp);
	ckfwrite(&truppa->morale, sizeof(truppa->morale), fp);
	ckfwrite(&truppa->stanca, sizeof(truppa->stanca), fp);
	ckfwrite(&truppa->combattuto, sizeof(truppa->combattuto), fp);
	ckfwrite(&pos, sizeof(pos), fp);
}

void salvaTruppe(t_player* giocatore, FILE* fp)
{
	int i;
	int num_truppe;
	t_lista_t* listaTruppe;

	//salva numero di truppe
	listaTruppe = giocatore->truppe;
	for (i = 0; listaTruppe != NULL; i++)
	{
		listaTruppe = listaTruppe->next;
	}
	num_truppe = i;
	ckfwrite(&num_truppe, sizeof(num_truppe), fp);
	DBGI("in totale %d unità schierate", num_truppe - 1)

	//salva le truppe
	listaTruppe = giocatore->truppe;
	for (i = 0; i < num_truppe; i++)
	{
		DBGI("salvo unità %d", i)
		DBGI("nella posizione %d", listaTruppe->pos)
		salvaTruppa(listaTruppe->truppa, listaTruppe->pos, fp);
		listaTruppe = listaTruppe->next;
	}
}

void salvaStruttura(t_lista_s *struttura, FILE *fp)
{

	int num_truppestruttura;

	int i;

	t_lista_t *listaTruppe;

	//Salva posizione struttura
	ckfwrite(&struttura->pos, sizeof(struttura->pos), fp);
	DBGI("si trova alla posizione %d", struttura->pos);

	//salva numero truppe in struttura
	listaTruppe = struttura->in;
	for (i = 0; listaTruppe != NULL; i++)
	{ // *in(conta lista)
		listaTruppe = listaTruppe->next;
	}
	num_truppestruttura = i;
	ckfwrite(&num_truppestruttura, sizeof(num_truppestruttura), fp);

	DBGI("contiene %d", num_truppestruttura - 1)

	//Salva le truppe dentro la struttura
	listaTruppe = struttura->in;
	for (i = 0; i < num_truppestruttura; i++)
	{
		DBGI("salvo truppa interna %d", i)
		salvaTruppa(listaTruppe->truppa, listaTruppe->pos, fp);
		listaTruppe = listaTruppe->next;
	}
}

void salvaGiocatore(t_player* giocatore, FILE *fp)
{

	int i, j;

	int num_strutture;

	t_lista_s* listaStruttura;

	for (i = 0; i < NUMSTRUTTURE; i++)
	{
		DBGI("salvo le strutture %d", i)
		//conta le strutture
		listaStruttura = giocatore->struttura[i];
		for (j = 0; listaStruttura != NULL; j++)
		{
			listaStruttura = listaStruttura->next;
		}
		num_strutture = j;
		ckfwrite(&num_strutture, sizeof(num_strutture), fp);
		DBGI("sono %d", num_strutture - 1)

		//Salva le strutture
		listaStruttura = giocatore->struttura[i];
		for (j = 0; j < num_strutture; j++)
		{
			DBGI("salvo la struttura %d", j)
			salvaStruttura(listaStruttura, fp);
			listaStruttura = listaStruttura->next;
		}
	}

	DBG("salvo le truppe schierate");
	//Salva le truppe all'esterno del giocatore
	salvaTruppe(giocatore, fp);

	DBG("salvo risorse giocatore")
	//Salva oro, cibo e smeraldi del giocatore
	ckfwrite(&giocatore->oro, sizeof(giocatore->oro), fp);
	ckfwrite(&giocatore->cibo, sizeof(giocatore->cibo), fp);
	ckfwrite(&giocatore->smeraldi, sizeof(giocatore->smeraldi), fp);
}

void salvaGiocatori(FILE* fp)
{
	int i;
	for (i = 0; i < MAXGIOCATORI; i++)
	{
		DBGI("salvo giocatore %d", i);
		if (giocatore[i] != NULL)
			salvaGiocatore(giocatore[i], fp);
	}
}

int salva(char *nomefile)
{
	FILE *fp;

	//apri il file
	fp = fopen(nomefile, "w");
	if (fp == NULL)
	{
		perror("fopen fallita");
		return 1;
	}

	DBG("scrivo l'header")
	//Salva l'header
	scriviHeader(fp);
	DBG("salvo le info")
	//Salva le info sullo stato generale del gioco
	salvaInfoStato(fp);
	DBG("salvo mappa")
	//Salva le informazioni riguardanti la mappa
	salvaMappa(fp);
	DBG("salvo info giocatori")
	//Salva le informazioni generali riguardanti i giocatori
	salvaInfoGiocatore(fp);
	DBG("salvo giocatori")
	//Salva la struttura dati dei giocatori
	salvaGiocatori(fp);
	//chiudi il file
	fclose(fp);

	return 0;
}
