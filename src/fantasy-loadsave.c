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
		sprintf(infogioco.skin, "NintendOtaku");
		sprintf(infogioco.ext, "xpm");
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
				ver[2]);
		return 0;
	}

	return 1;
}

void caricaInfoStato(FILE* fp, int inGioco[MAXGIOCATORI])
{
	//num giocatori
	ckfread(&NumGiocatori, sizeof(NumGiocatori), fp);
	// Cx,Cy, CurrentPlayer
	ckfread(&cx, sizeof(cx), fp);
	ckfread(&cy, sizeof(cy), fp);
	ckfread(&CurrentPlayer, sizeof(CurrentPlayer), fp);
	//carica la maschera dei giocatori attivi
	ckfread(inGioco, sizeof(inGioco), fp);
}

void caricaMappa(FILE* fp)
{
	ckfread(&infomappa.prato, sizeof(infomappa.prato), fp);
	ckfread(&infomappa.castelli, sizeof(infomappa.castelli), fp);
	ckfread(&infomappa.fattorie, sizeof(infomappa.fattorie), fp);
	ckfread(&infomappa.stalle, sizeof(infomappa.stalle), fp);
	ckfread(&infomappa.grotte, sizeof(infomappa.grotte), fp);
	ckfread(&infomappa.nidi, sizeof(infomappa.nidi), fp);
	ckfread(&infomappa.numstalle, sizeof(infomappa.numstalle), fp);
	ckfread(&infomappa.numnidi, sizeof(infomappa.numnidi), fp);
	ckfread(&infomappa.numgrotte, sizeof(infomappa.numgrotte), fp);
	ckfread(&infomappa.numfattorie, sizeof(infomappa.numfattorie), fp);
}

void caricaInfoGiocatore(int inGioco[MAXGIOCATORI], FILE* fp)
{
	int i;

	for (i = 0; i < NumGiocatori; i++)
	{
		if (inGioco[i])
		{
			infogiocatore[i] = (t_infoplayer*) malloc(sizeof(t_infoplayer*));
			ckfread(&infogiocatore[i]->atteggiamento,
					sizeof(infogiocatore[i]->atteggiamento), fp);
			ckfread(infogiocatore[i]->nome, sizeof(infogiocatore[i]->nome), fp);
			ckfread(&infogiocatore[i]->tipo, sizeof(infogiocatore[i]->tipo),
					fp);
		}
		else
		{
			infogiocatore[i] = NULL;
		}
	}

}

t_lista_t * caricaTruppa(t_lista_t *testa, FILE *fp, char from) //# stavolta faccio notare che è meglio in questa funzione fare solo l'inserimento e non cercare la coda! inutile cercare la coda in quanto nessun'altra parte del codice usa la suddetta funzione.
{
	t_lista_t *temp, *nuova;
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

	temp = testa;

	if (temp == NULL)
		return nuova;
	while (temp->next != NULL)
	{
		temp = temp->next;
	}
	temp->next = nuova;

	return testa;
}

t_lista_s * caricaTruppeInStruttura(t_lista_s *testa, FILE *fp)
{
	int num_truppestruttura;
	int l;

	t_lista_s *temp, *nuova;
	nuova = malloc(sizeof(t_lista_s));

	//carica posizione della struttura
	ckfread(&nuova->pos, sizeof(nuova->pos), fp);

	//carica truppe nella struttura
	ckfread(&num_truppestruttura, sizeof(num_truppestruttura), fp);
	nuova->in = NULL;
	for (l = 0; l < num_truppestruttura; l++)
	{
		nuova->in = caricaTruppa(nuova->in, fp, 1);
	}
	nuova->next = NULL;

	temp = testa;

	if (temp == NULL)
		return nuova;
	while (temp->next != NULL)
	{
		temp = temp->next;
	}
	temp->next = nuova;

	return testa;
}

t_player* caricaGiocatore(FILE* fp)
{
	int i, j;
	int num_strutture, num_truppe;
	t_player* giocatore = malloc(sizeof(t_player));

	//Carica le strutture del giocatore
	for (i = 0; i < NUMSTRUTTURE; i++)
		giocatore->struttura[i] = NULL;
	for (i = 0; i < NUMSTRUTTURE; i++)
	{
		ckfread(&num_strutture, sizeof(num_strutture), fp);
		for (j = 0; j < num_strutture; j++)
		{
			giocatore->struttura[i] = caricaTruppeInStruttura(
					giocatore->struttura[i], fp);
		}
	}

	//Carica le truppe del giocatore
	ckfread(&num_truppe, sizeof(num_truppe), fp);
	giocatore->truppe = NULL;
	for (i = 0; i < num_truppe; i++)
	{
		// *truppe(scorri)
		giocatore->truppe = caricaTruppa(giocatore->truppe, fp, 0);
	}

	//carica oro, cibo e smeraldi
	ckfread(&giocatore->oro, sizeof(giocatore->oro), fp);
	ckfread(&giocatore->cibo, sizeof(giocatore->cibo), fp);
	ckfread(&giocatore->smeraldi, sizeof(giocatore->smeraldi), fp);

	return giocatore;
}

void caricaGiocatori(int inGioco[MAXGIOCATORI], FILE* fp)
{
	int i;
	int num_strutture, num_truppe;
	for (i = 0; i < NumGiocatori; i++)
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
	while (i < MAXGIOCATORI)
	{
		giocatore[i] = NULL;
		i++;
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

	//num giocatori
	ckfwrite(&NumGiocatori, sizeof(NumGiocatori), fp);
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
	// infomappa
	ckfwrite(&infomappa.prato, sizeof(infomappa.prato), fp);
	ckfwrite(&infomappa.castelli, sizeof(infomappa.castelli), fp);
	ckfwrite(&infomappa.fattorie, sizeof(infomappa.fattorie), fp);
	ckfwrite(&infomappa.stalle, sizeof(infomappa.stalle), fp);
	ckfwrite(&infomappa.grotte, sizeof(infomappa.grotte), fp);
	ckfwrite(&infomappa.nidi, sizeof(infomappa.nidi), fp);
	ckfwrite(&infomappa.numstalle, sizeof(infomappa.numstalle), fp);
	ckfwrite(&infomappa.numnidi, sizeof(infomappa.numnidi), fp);
	ckfwrite(&infomappa.numgrotte, sizeof(infomappa.numgrotte), fp);
	ckfwrite(&infomappa.numfattorie, sizeof(infomappa.numfattorie), fp);
}

void salvaInfoGiocatore(FILE* fp)
{
	int i;

	for (i = 0; i < NumGiocatori; i++)
	{
		if (giocatore[i] != NULL)
		{
			ckfwrite(&infogiocatore[i]->atteggiamento,
					sizeof(infogiocatore[i]->atteggiamento), fp);
			ckfwrite(infogiocatore[i]->nome, sizeof(infogiocatore[i]->nome),
					fp);
			ckfwrite(&infogiocatore[i]->tipo, sizeof(infogiocatore[i]->tipo),
					fp);
		}
	}

}

void salvaTruppa(t_infotruppa* truppa, int pos, FILE* fp)
{
	// *truppe(scorri)
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

	//salva le truppe
	listaTruppe = giocatore->truppe;
	for (i = 0; i < num_truppe; i++)
	{
		salvaTruppa(listaTruppe->truppa, listaTruppe->pos, fp);
		listaTruppe = listaTruppe->next;
	}

	//Salva oro, cibo e smeraldi del giocatore
	ckfwrite(&giocatore->oro, sizeof(giocatore->oro), fp);
	ckfwrite(&giocatore->cibo, sizeof(giocatore->cibo), fp);
	ckfwrite(&giocatore->smeraldi, sizeof(giocatore->smeraldi), fp);
}

void salvaStruttura(t_lista_s *struttura, FILE *fp)
{

	int num_truppestruttura;

	int i;

	t_lista_t *listaTruppe;

	//Salva posizione struttura
	ckfwrite(&struttura->pos, sizeof(struttura->pos), fp);

	//salva numero truppe in struttura
	listaTruppe = struttura->in;
	for (i = 0; listaTruppe != NULL; i++)
	{ // *in(conta lista)
		listaTruppe = listaTruppe->next;
	}
	num_truppestruttura = i;
	ckfwrite(&num_truppestruttura, sizeof(num_truppestruttura), fp);

	//Salva le truppe dentro la struttura
	listaTruppe = struttura->in;
	for (i = 0; i < num_truppestruttura; i++)
	{
		salvaTruppa(listaTruppe->truppa, listaTruppe->pos, fp);
		listaTruppe = listaTruppe->next;

	}
}

void salvaGiocatore(t_player* giocatore, FILE *fp)
{

	int i, k;

	int num_strutture;

	t_lista_s* listaStruttura;

	for (i = 0; i < NUMSTRUTTURE; i++)
	{
		//conta le strutture
		listaStruttura = giocatore->struttura[i];
		for (k = 0; listaStruttura != NULL; k++)
		{
			listaStruttura = listaStruttura->next;
		}
		num_strutture = k;
		ckfwrite(&num_strutture, sizeof(num_strutture), fp);

		//Salva le strutture
		listaStruttura = giocatore->struttura[i];
		for (k = 0; k < num_strutture; k++)
		{
			salvaStruttura(giocatore->struttura[i], fp);
			listaStruttura = listaStruttura->next;
		}
	}

	//Salva le truppe all'esterno del giocatore
	salvaTruppe(giocatore, fp);
}

void salvaGiocatori(FILE* fp)
{
	int i;
	for (i = 0; i < NumGiocatori; i++)
	{
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

	//Salva l'header
	scriviHeader(fp);
	//Salva le info sullo stato generale del gioco
	salvaInfoStato(fp);
	//Salva le informazioni riguardanti la mappa
	salvaMappa(fp);
	//Salva le informazioni generali riguardanti i giocatori
	salvaInfoGiocatore(fp);
	//Salva la struttura dati dei giocatori
	salvaGiocatori(fp);
	//chiudi il file
	fclose(fp);

	return 0;
}
