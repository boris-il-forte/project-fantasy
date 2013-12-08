/*
 * Project Fantasy, gioco di strategia a turni
 *
 *
 * Copyright (C) 2011 Davide Tateo, gionnico
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "fantasy-core.h"
#include "fantasy-IA.h"

#define INIZIO_ORO 1000
#define INIZIO_CIBO 1000
#define INIZIO_SMERALDI 200
#define PROD_ORO_CAS 500
#define PROD_ORO_FAT 200
#define PROD_ORO_SCU 400
#define PROD_ORO_NID 100
#define PROD_ORO_GRO 1000
#define PROD_SMERALDI_NID 500
#define PROD_SMERALDI_GRO 100
#define PROD_CIBO_CAS 400
#define PROD_CIBO_FAT 1000
#define PROD_CIBO_SCU 100
#define FAME 10

//carica dati unità
void caricadati()
{
	//dati recluta
	Dtruppa[Rec].att = 15;
	Dtruppa[Rec].dif = 5;
	Dtruppa[Rec].git = 2;
	Dtruppa[Rec].vel = 10;
	Dtruppa[Rec].mor = 25;
	Dtruppa[Rec].oro = 150;
	Dtruppa[Rec].cibo = 50;
	Dtruppa[Rec].smeraldi = 0;
	//dati fante
	Dtruppa[Fan].att = 20;
	Dtruppa[Fan].dif = 15;
	Dtruppa[Fan].git = 1;
	Dtruppa[Fan].vel = 5;
	Dtruppa[Fan].mor = 50;
	Dtruppa[Fan].oro = 400;
	Dtruppa[Fan].cibo = 100;
	Dtruppa[Fan].smeraldi = 0;
	//dati lanciere
	Dtruppa[Lan].att = 10;
	Dtruppa[Lan].dif = 25;
	Dtruppa[Lan].git = 1;
	Dtruppa[Lan].vel = 5;
	Dtruppa[Lan].mor = 40;
	Dtruppa[Lan].oro = 300;
	Dtruppa[Lan].cibo = 50;
	Dtruppa[Lan].smeraldi = 0;
	//dati arciere
	Dtruppa[Arc].att = 15;
	Dtruppa[Arc].dif = 5;
	Dtruppa[Arc].git = 12;
	Dtruppa[Arc].vel = 10;
	Dtruppa[Arc].mor = 30;
	Dtruppa[Arc].oro = 250;
	Dtruppa[Arc].cibo = 50;
	Dtruppa[Arc].smeraldi = 0;
	//dati cavaliere
	Dtruppa[Cav].att = 90;
	Dtruppa[Cav].dif = 5;
	Dtruppa[Cav].git = 1;
	Dtruppa[Cav].vel = 20;
	Dtruppa[Cav].mor = 75;
	Dtruppa[Cav].oro = 1000;
	Dtruppa[Cav].cibo = 200;
	Dtruppa[Cav].smeraldi = 10;
	//dati drago
	Dtruppa[Dra].att = 2000;
	Dtruppa[Dra].dif = 2000;
	Dtruppa[Dra].git = 2;
	Dtruppa[Dra].vel = 20;
	Dtruppa[Dra].mor = 100;
	Dtruppa[Dra].oro = 3000;
	Dtruppa[Dra].cibo = 1500;
	Dtruppa[Dra].smeraldi = 500;
	//dati fenice
	Dtruppa[Fen].att = 2000;
	Dtruppa[Fen].dif = 800;
	Dtruppa[Fen].git = 2;
	Dtruppa[Fen].vel = 25;
	Dtruppa[Fen].mor = 80;
	Dtruppa[Fen].oro = 2500;
	Dtruppa[Fen].cibo = 1000;
	Dtruppa[Fen].smeraldi = 500;
}

//inizializza le strutture dati 
void inizializza()
{
	int i, j;
	if (partita_in_corso != 0)
		liberaheap();
	for (i = 0; i < NUMCASTELLI; i++)
		infomappa.castelli[i] = -1;
	for (i = 0; i < MAXSTALLE; i++)
		infomappa.stalle[i] = -1;
	for (i = 0; i < MAXGROTTE; i++)
		infomappa.grotte[i] = -1;
	for (i = 0; i < MAXNIDI; i++)
		infomappa.nidi[i] = -1;
	for (i = 0; i < MAXGIOCATORI; i++)
		giocatore[i] = NULL;
	for (i = 0; i < MAXGIOCATORI; i++)
		infogiocatore[i] = NULL;
	for (i = 0; i < ALTEZZA; i++)
		for (j = 0; j < LARGHEZZA; j++)
		{
			accedi(j,i,infomappa.mappa)=' ';
			accedi(j,i,infomappa.truppe)=NULL;
		}
	}

// controlla che i castelli generati siano sufficentemente distanti l'uno dall'altro
int conflitticastello(int x, int y, int cur)
{
	int i, j, k;
	int const pos = x + LARGHEZZA * y;
	if (x != 0 && y != 0 && x < LARGHEZZA - 1 && y < ALTEZZA - 1)
	{
		for (i = 0; i < NUMCASTELLI; i++)
			for (j = -10; j <= 10; j++)
				for (k = -10; k <= 10; k++)
					if (i != cur && pos + k - LARGHEZZA * j == infomappa.castelli[i])
						return 1;
		return 0;
	}
	else
		return 1;
}
// controlla che le strutture della mappa non si sovrappongano e siano sufficentemente distanti da castelli e altre strutture 
int conflittomappa(int x, int y)
{
	int i, j;
	if (x != 0 && y != 0 && x < LARGHEZZA - 1 && y < ALTEZZA - 1)
	{
		for (i = -3; i <= 4; i++)
			for (j = -3; j <= 4; j++)
				if (accedi(x+i,y+j,infomappa.mappa)!=' ')return 1;
			}
			else return 1;
	return 0;
}

//genera un prato vario
void pratoacaso()
{
	int n, i, j;
	srand((unsigned) time(NULL));
	for (i = 0; i < ALTEZZA; i++)
		for (j = 0; j < LARGHEZZA; j++)
		{
			n = rand() % 20;
			if (n < 12)
				n = 0;
			else if (n < 16)
				n = 1;
			else if (n < 18)
				n = 2;
			else if (n == 18)
				n = 3;
			else
				n = 4;
			accedi(j,i,infomappa.prato)=n;
		}
	}
//genera la mappa casuale
void generamappa()
{
	int i; //contatori 
	int x, y; //variabili posizionamento

	srand((unsigned) time(NULL)); //randomizza col tempo

	infomappa.numstalle = rand() % (MAXSTALLE / 2) + (MAXSTALLE / 2 + 1); //genera il numero delle stalle tra MAXSTALLE e MAXSTALLE/2;
	infomappa.numfattorie = rand() % (MAXFATTORIE / 2) + (MAXFATTORIE / 2 + 1); //genera il numero delle stalle tra MAXFATTORIE e MAXFATTORIE/2;
	infomappa.numgrotte = rand() % MAXGROTTE + 1; //genera il numero delle grotte
	infomappa.numnidi = rand() % MAXNIDI + 1; //genera il numero dei nidi

	/*genero i castelli*/
	for (i = 0; i < NUMCASTELLI; i++)
	{
		do
		{
			infomappa.castelli[i] = rand() % (ALTEZZA * LARGHEZZA);
			x = infomappa.castelli[i] % LARGHEZZA;
			y = infomappa.castelli[i] / LARGHEZZA;
		} while (conflitticastello(x, y, i));
		accedi(x-1,y-1,infomappa.mappa)='0'; //segna i castelli
		accedi(x,y-1,infomappa.mappa)='1';
		accedi(x+1,y-1,infomappa.mappa)='2';
		accedi(x-1,y,infomappa.mappa)='3';
		accedi(x,y,infomappa.mappa)='4';
		accedi(x+1,y,infomappa.mappa)='5';
		accedi(x-1,y+1,infomappa.mappa)='6';
		accedi(x,y+1,infomappa.mappa)='7';
		accedi(x+1,y+1,infomappa.mappa)='8';
	}
	/*genero le stalle*/
	for (i = 0; i < infomappa.numstalle; i++)
	{
		do
		{
			infomappa.stalle[i] = rand() % (ALTEZZA * LARGHEZZA);
			x = infomappa.stalle[i] % LARGHEZZA;
			y = infomappa.stalle[i] / LARGHEZZA;
		} while (conflittomappa(x, y));
		disegna(x, y, 'S'); //segna le stalle
	}
	/*genero le fattorie*/
	for (i = 0; i < infomappa.numfattorie; i++)
	{
		do
		{
			infomappa.fattorie[i] = rand() % (ALTEZZA * LARGHEZZA);
			x = infomappa.fattorie[i] % LARGHEZZA;
			y = infomappa.fattorie[i] / LARGHEZZA;
		} while (conflittomappa(x, y));
		disegna(x, y, 'C'); //segna le fattorie
	}
	/*genero le grotte*/
	for (i = 0; i < infomappa.numgrotte; i++)
	{
		do
		{
			infomappa.grotte[i] = rand() % (ALTEZZA * LARGHEZZA);
			x = infomappa.grotte[i] % LARGHEZZA;
			y = infomappa.grotte[i] / LARGHEZZA;
		} while (conflittomappa(x, y));
		disegna(x, y, 'G');
	}
	/*genero i nidi*/
	for (i = 0; i < infomappa.numnidi; i++)
	{
		do
		{
			infomappa.nidi[i] = rand() % (ALTEZZA * LARGHEZZA);
			x = infomappa.nidi[i] % LARGHEZZA;
			y = infomappa.nidi[i] / LARGHEZZA;
		} while (conflittomappa(x, y));
		disegna(x, y, 'N');
	}
	//crea prato vario
	pratoacaso();
}

//genera la mappa caricata
void rigeneramappa()
{
	int i; // contatori
	int x, y; //variabili posizionamento
	/*genero i castelli*/
	for (i = 0; i < NUMCASTELLI; i++)
	{
		x = infomappa.castelli[i] % LARGHEZZA;
		y = infomappa.castelli[i] / LARGHEZZA;
		accedi(x-1,y-1,infomappa.mappa)='0'; //segna i castelli
		accedi(x,y-1,infomappa.mappa)='1';
		accedi(x+1,y-1,infomappa.mappa)='2';
		accedi(x-1,y,infomappa.mappa)='3';
		accedi(x,y,infomappa.mappa)='4';
		accedi(x+1,y,infomappa.mappa)='5';
		accedi(x-1,y+1,infomappa.mappa)='6';
		accedi(x,y+1,infomappa.mappa)='7';
		accedi(x+1,y+1,infomappa.mappa)='8';
	}
	/*genero le fattorie*/
	for (i = 0; i < infomappa.numstalle; i++)
	{
		x = infomappa.fattorie[i] % LARGHEZZA;
		y = infomappa.fattorie[i] / LARGHEZZA;
		disegna(x, y, 'C'); //segna le fattorie
	}
	/*genero le stalle*/
	for (i = 0; i < infomappa.numstalle; i++)
	{
		x = infomappa.stalle[i] % LARGHEZZA;
		y = infomappa.stalle[i] / LARGHEZZA;
		disegna(x, y, 'S'); //segna le stalle
	}
	/*genero le grotte*/
	for (i = 0; i < infomappa.numgrotte; i++)
	{
		x = infomappa.grotte[i] % LARGHEZZA;
		y = infomappa.grotte[i] / LARGHEZZA;
		disegna(x, y, 'G');
	}
	/*genero i nidi*/
	for (i = 0; i < infomappa.numnidi; i++)
	{
		x = infomappa.nidi[i] % LARGHEZZA;
		y = infomappa.nidi[i] / LARGHEZZA;
		disegna(x, y, 'N');
	}
	//visualizza su terminale
	visualizza_su_terminale();
}

//inizializza la struttura dati giocatori
void creagiocatori(int n, int ia)
{
	int i, j;
	char al;
	int num[MAXGIOCATORI];
	for (i = 0; i < n; i++)
		num[i] = NUMCASTELLI;
	for (i = 0; i < n; i++)
	{
		srand((unsigned) time(NULL));
		do
		{
			al = rand() % NUMCASTELLI;
			for (j = 0; j < i && al != num[j]; j++)
				;
		} while (j < i);
		num[i] = al;
	}

	for (i = 0; i < n; i++)
	{
		fprintf(stderr, "creo giocatore n:%d\n", i);
		giocatore[i] = (t_player*) malloc(sizeof(t_player));
		infogiocatore[i] = (t_infoplayer*) malloc(sizeof(t_infoplayer));
		giocatore[i]->oro = INIZIO_ORO;
		giocatore[i]->cibo = INIZIO_CIBO;
		giocatore[i]->smeraldi = INIZIO_SMERALDI;
		giocatore[i]->struttura[Cas] = (t_lista_s*) malloc(sizeof(t_lista_s));
		giocatore[i]->struttura[Cas]->pos = infomappa.castelli[num[i]];
		giocatore[i]->struttura[Cas]->in = NULL;
		giocatore[i]->struttura[Cas]->next = NULL;
		giocatore[i]->truppe = NULL;
		infogiocatore[i]->tipo = Locale;
		sprintf(infogiocatore[i]->nome, "Giocatore %d", i + 1);
		infogiocatore[i]->atteggiamento = 'u';
		for (j = 1; j < NUMSTRUTTURE; j++)
			giocatore[i]->struttura[j] = NULL;
	}
	while (i < MAXGIOCATORI)
	{
		giocatore[i] = NULL;
		i++;
	}

	fantasia_assegna_ia_random(ia, n);

}

//addestra una unità
void addestratruppa(int P, t_truppa T)
{
	int uominiunita;
	t_infotruppa* U;
	t_lista_s* S;
	t_lista_t* N;
	t_lista_t* Np;
	if (T == Cav)
	{
		uominiunita = 50;
	}
	else
		uominiunita = 100;
	U = generatruppa(T, CurrentPlayer, uominiunita);
	S = puntastruttura(P);
	if (S == NULL)
	{
		fprintf(stderr, "c'è un bug!\n");
		return;
	}
	N = S->in;
	if (N != NULL)
	{
		do
		{
			Np = N;
			N = N->next;
		} while (N != NULL);
		N = (t_lista_t*) malloc(sizeof(t_lista_t));
		N->truppa = U;
		N->pos = P;
		Np->next = N;
		N->next = NULL;
	}
	else
	{
		S->in = (t_lista_t*) malloc(sizeof(t_lista_t));
		S->in->truppa = U;
		S->in->pos = P;
		S->in->next = NULL;
	}
}

//verifica se le risorse sono sufficenti per addestrare una unità e ne scala il costo
int pagaunita(t_truppa T)
{
	if (Dtruppa[T].oro > giocatore[CurrentPlayer]->oro)
		return 1;
	else if (Dtruppa[T].cibo > giocatore[CurrentPlayer]->cibo)
		return 2;
	else if (Dtruppa[T].smeraldi > giocatore[CurrentPlayer]->smeraldi)
		return 3;
	else
	{
		giocatore[CurrentPlayer]->oro -= Dtruppa[T].oro;
		giocatore[CurrentPlayer]->cibo -= Dtruppa[T].cibo;
		giocatore[CurrentPlayer]->smeraldi -= Dtruppa[T].smeraldi;
		return 0;
	}
}

//fa uscire le unità da una strttura
int evacuatruppa(t_lista_t *T)
{
	int L;
	int P = T->pos;
	t_lista_s *S;
	t_lista_t *Tp;
	t_infotruppa** Libera;
	S = puntastruttura(P);
	Tp = puntatruppaprecedente(T, S);
	Libera = puntacasellalibera(P);
	if (Libera != NULL)
	{
		L = (int) (Libera - infomappa.truppe);
		infomappa.truppe[L] = T->truppa;
		if (Tp != NULL)
		{
			Tp->next = T->next;
			free(T);
		}
		else
		{
			S->in = T->next;
			free(T);
		}
		if (giocatore[CurrentPlayer]->truppe == NULL)
		{
			giocatore[CurrentPlayer]->truppe = (t_lista_t*) malloc(sizeof(t_lista_t));
			giocatore[CurrentPlayer]->truppe->truppa = infomappa.truppe[L];
			giocatore[CurrentPlayer]->truppe->pos = L;
			giocatore[CurrentPlayer]->truppe->next = NULL;
		}
		else
		{
			Tp = giocatore[CurrentPlayer]->truppe;
			T = giocatore[CurrentPlayer]->truppe->next;
			while (T != NULL)
			{
				Tp = T;
				T = T->next;
			}
			Tp->next = (t_lista_t*) malloc(sizeof(t_lista_t));
			Tp->next->truppa = infomappa.truppe[L];
			Tp->next->pos = L;
			Tp->next->next = NULL;
		}
		return 0;
	}
	else
	{
		printf("caselle occupate!\n");
		return 1;
	}
}

//muove la truppa da Src a Dst
void spostatruppa(int Src, int Dst)
{
	t_lista_t *T = giocatore[CurrentPlayer]->truppe;
	infomappa.truppe[Src]->stanca = 1;
	infomappa.truppe[Dst] = infomappa.truppe[Src];
	infomappa.truppe[Src] = NULL;
	while (T != NULL)
	{
		if (T->pos == Src)
			T->pos = Dst;
		else
			T = T->next;
	}
}

//muove la truppa in Src nella struttura in Dst
void spostainstruttura(int Src, int Dst)
{
	int g = controllounita(Src);
	t_lista_t* T;
	t_lista_t* Tp = NULL;
	t_lista_t* Ts;
	t_lista_s* S;
	//cerca la struttura
	S = puntastruttura(Dst);
	//cerca l'unità nella lista unità
	T = giocatore[g]->truppe;
	while (T->pos != Src && T->next != NULL)
	{
		Tp = T;
		T = T->next;
	}
	if (Tp == NULL)
		giocatore[g]->truppe = T->next;
	else
		Tp->next = T->next;
	//cerca la coda della lista struttura e sposta dentro l'unità
	Ts = S->in;
	if (Ts == NULL)
		S->in = T;
	else
	{
		while (Ts->next != NULL)
			Ts = Ts->next;
		Ts->next = T;
	}
	T->pos = Dst;
	T->truppa->stanca = 1;
	T->next = NULL;
	infomappa.truppe[Src] = NULL;
}

//genera l'unità desiderata
t_infotruppa* generatruppa(t_truppa Tipo, char Giocatore, int Numero)
{
	t_infotruppa* T;
	T = (t_infotruppa*) malloc(sizeof(t_infotruppa));
	T->tipo = Tipo;
	T->giocatore = Giocatore;
	T->numero = Numero;
	T->morale = Dtruppa[Tipo].mor;
	T->stanca = 0;
	T->combattuto = 0;
	return T;
}

void identificatruppa(t_infotruppa* T, char* buf)
{
	switch (T->tipo)
	{
	case Rec:
		sprintf(buf, "-%d Reclute(%d)-", T->numero, T->morale);
		break;
	case Fan:
		sprintf(buf, "-%d Fanteria(%d)-", T->numero, T->morale);
		break;
	case Lan:
		sprintf(buf, "-%d Lancieri(%d)-", T->numero, T->morale);
		break;
	case Arc:
		sprintf(buf, "-%d Arcieri(%d)-", T->numero, T->morale);
		break;
	case Cav:
		sprintf(buf, "-%d Cavalieri(%d)-", T->numero, T->morale);
		break;
	case Dra:
		sprintf(buf, "-%d Draghi(%d)-", T->numero, T->morale);
		break;
	case Fen:
		sprintf(buf, "-%d Fenici(%d)-", T->numero, T->morale);
		break;
	default:
		sprintf(buf, "-%d ERRORE!(%d)-", T->numero, T->morale);
		break;
	}
	return;
}

//libera le strutture dati allocate
void liberaheap()
{
	int i, j;
	t_lista_s* S;
	t_lista_s* Sn;
	t_lista_t* T;
	t_lista_t* Tn;
	for (i = 0; i < MAXGIOCATORI; i++)
	{
		if (giocatore[i] != NULL)
		{
			for (j = 0; j < NUMSTRUTTURE; j++)
			{
				S = giocatore[i]->struttura[j];
				while (S != NULL)
				{
					T = S->in;
					while (T != NULL)
					{
						Tn = T->next;
						free(T);
						T = Tn;
					}
					Sn = S->next;
					free(S);
					S = Sn;
				}
			}
			T = giocatore[i]->truppe;
			while (T != NULL)
			{
				Tn = T->next;
				free(T);
				T = Tn;
			}
			free(giocatore[i]);
			free(infogiocatore[i]);
		}
	}
	for (i = 0; i < ALTEZZA * LARGHEZZA; i++)
		free(infomappa.truppe[i]);
}

//libera le strutture dati di uno specifico giocatore
void liberagiocatore(int G)
{
	t_lista_t *T;
	t_lista_t *Tn;
	T = giocatore[G]->truppe;
	//elimina le truppe del giocatore sconfitto
	while (T != NULL)
	{
		free(infomappa.truppe[T->pos]);
		infomappa.truppe[T->pos] = NULL;
		Tn = T->next;
		free(T);
		T = Tn;
	}
	//elimina il resto della struttura dati
	free(giocatore[G]);
	free(infogiocatore[G]);
	giocatore[G] = NULL;
	infogiocatore[G] = NULL;
}

//calcola il costo del percorso più breve utilizzando l'algoritmo di Dijkstra
int inizializza_dijkstra(int PosT, char ***G, int ***V, int *mx, char *vel)
{
	int Cs;
	int Sx, Sy;
	int i, j;
	int Q = 0;
	int Mx, Vel;
	Sx = PosT % LARGHEZZA;
	Sy = PosT / LARGHEZZA;
	t_truppa Tipo;
	//inizializza strutture dati
	Tipo = infomappa.truppe[PosT]->tipo;
	Vel = Dtruppa[Tipo].vel;
	Mx = 2 * Vel + 3;
	Cs = Vel + 1;
	*V = malloc(sizeof(int*) * Mx);
	*G = malloc(sizeof(char*) * Mx);
	if (*V == NULL || *G == NULL)
	{
		perror("malloc fallita");
		exit(1);
	}
	for (i = 0; i < Mx; i++)
	{
		(*V)[i] = malloc(sizeof(int) * Mx);
		(*G)[i] = malloc(sizeof(char) * Mx);
		if ((*V)[i] == NULL || (*G)[i] == NULL)
		{
			perror("malloc fallita");
			exit(1);
		}
	}
	printf("dbg Mossa=%d Mx=%d vel=%d V=%p Cs=%d \n", PosT, Mx, Vel, V, Cs); // Debug
	//prepara grafo e contatore Q
	for (i = 0; i < Mx; i++)
	{
		(*G)[i][0] = '#';
		(*G)[i][Mx - 1] = '#';
	}
	for (i = 0; i < Mx; i++)
	{
		(*G)[0][i] = '#';
		(*G)[Mx - 1][i] = '#';
	}
	for (i = 1; i < Mx - 1; i++)
		for (j = 1; j < Mx - 1; j++)
		{
			if ((Sx + i - Cs) < 0||(Sx+i-Cs)>=LARGHEZZA ||(Sy+j-Cs)<0 ||(Sy+j-Cs)>=ALTEZZA)
				(*G)[i][j] = '#';
			else if ((Cs - i) * (Cs - i) + (Cs - j) * (Cs - j)
					> Vel
							* Vel|| infomappa.mappa[posiziona(Cs-i,Cs-j,Sx,Sy)]!=' ' || infomappa.truppe[posiziona(Cs-i,Cs-j,Sx,Sy)]!=NULL)
				(*G)[i][j] = '#';
			else
			{
				(*G)[i][j] = '.';
				Q++;
			}
		}
	printf("comincia algoritmo \n"); //Debug
	//inizializzo V
	for (i = 0; i < Mx; i++)
		for (j = 0; j < Mx; j++)
			(*V)[i][j] = Vel * 100 + 1;
	(*G)[Cs][Cs] = 'S'; //segna il source
	(*V)[Cs][Cs] = 0;
#ifdef DEBUG
	// stampo il grafo
	for(i=0;i<Mx;i++)
	{
		for(j=0;j<Mx;j++)
		printf("%c",(*G)[j][i]);
		printf("\n");
	}
#endif
	//assegna il valore alle variabili passate da puntatore
	*vel = Vel;
	*mx = Mx;
	//ritorna il numero di caselle libere
	return Q;
}

void calcola_dijkstra(char **G, int Mx, int vel, int **V, int Q)
{
	const int Cs = vel + 1;
	int i, j, k, l;
	int v = 1;
	int vp = 0;
	int X, Y;
	int w;
	int S[Mx * Mx];
	S[0] = 1;
	S[v] = Cs * Mx + Cs;
	while (Q != 0 && v <= S[0]) //fino a che non hai considerato ogni casella libera...
	{
		//estrai minimi e impilali
		for (i = -1; i <= 1; i++)
			for (j = -1; j <= 1; j++)
			{
				if (i + j == 1 || i + j == -1) //considera solo le caselle minime(distanza 1)
				{
					X = S[v] / Mx + i; //calcola la x della casella
					Y = S[v] % Mx + j; //calcola la y della casella
					if (G[X][Y] == '.') //se non è stato ancora contato...
					{
						Q--; //conta un elemento in meno dal totale
						G[X][Y] = '+'; //segnalo come contato
						S[S[0] + 1] = X * Mx + Y; //impilalo
						S[0]++; //aumenta il puntatore alla pila
						vp++;
					}
				}
			}
		//printf("primo passo ok \n");
		for (i = S[0] - vp; i <= S[0]; i++)
		{
			X = S[i] / Mx; //calcola la x della casella
			Y = S[i] % Mx; //calcola la y della casella
			//rilassa gli archi
			for (k = -1; k <= 1; k++)
				for (l = -1; l <= 1; l++)
				{
					w = ((k + l == 1 || k + l == -1) ? 100 : 141);
					if (G[X + k][Y + l] != '#' && V[X + k][Y + l] > V[X][Y] + w)
					{
						if (w == 100 || !(G[X + k][Y] == '#' && G[X][Y + l] == '#'))
						{
							V[X + k][Y + l] = V[X][Y] + w;
						}
					}
				}
		}
		//printf("secondo passo ok \n");
		v++; //passa al prossimo vertice
		vp = 0; //annulla il conteggio dei nuovi vertici
	}
}

//calcola la posizione della struttura
int calcolaposizionestruttura(char struttura, int x, int y)
{
	switch (struttura)
	{
	case '0':
		return posiziona(-1, -1, x, y);
	case '1':
		return posiziona(0, -1, x, y);
	case '2':
		return posiziona(1, -1, x, y);
	case '3':
		return posiziona(-1, 0, x, y);
	case '4':
	case 'G':
	case 'C':
	case 'S':
	case 'N':
		return posiziona(0, 0, x, y);
	case '5':
	case 'H':
	case 'D':
	case 'T':
	case 'O':
		return posiziona(1, 0, x, y);
	case '6':
		return posiziona(-1, 1, x, y);
	case '7':
	case 'I':
	case 'E':
	case 'U':
	case 'P':
		return posiziona(0, 1, x, y);
	case '8':
	case 'J':
	case 'F':
	case 'V':
	case 'Q':
		return posiziona(1, 1, x, y);

	}

}

//estrai tipo struttura
t_struttura tipostruttura(char struttura)
{
	switch (struttura)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
		return Cas;
	case 'G':
	case 'H':
	case 'I':
	case 'J':
		return Gro;
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		return Fat;
	case 'S':
	case 'T':
	case 'U':
	case 'V':
		return Scu;
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
		return Nid;
	}
}

// calcola se lo spostamento è lecito
int spostalecito(int PosT, int PosC, int **V)
{
	t_truppa Tipo = infomappa.truppe[PosT]->tipo;
	int Vel = (int) Dtruppa[Tipo].vel;
	int x = PosT % LARGHEZZA - PosC % LARGHEZZA;
	int y = PosT / LARGHEZZA - PosC / LARGHEZZA;
	int s;
	int c = Vel + 1;
	if (x * x + y * y <= Vel * Vel)
	{
		if (Tipo < Dra)
		{
			if (infomappa.mappa[PosC] != ' ')
			{
				s = (Vel * 100 - 10);
				if (V[c - x - 1][c - y] <= s || V[c - x + 1][c - y] <= s || V[c - x][c - y - 1] <= s
						|| V[c - x][c - y + 1] <= s)
				{
					return 1;
				}
				else
					return 0;
			}
			else if (V[c - x][c - y] <= (Vel * 100))
			{
				return 1;
			}
			else
				return 0;
		}
		else
		{
			if (infomappa.mappa[PosC] == ' ')
			{
				return 1;
			}
			else
				return 0;
		}
	}
	else
		return 0;
}

//calcola area bersaglio
int bersagliolecito(int PosT, int PosC)
{
	t_truppa Tipo = infomappa.truppe[PosT]->tipo;
	char Git = Dtruppa[Tipo].git;
	int x = PosT % LARGHEZZA - PosC % LARGHEZZA;
	int y = PosT / LARGHEZZA - PosC / LARGHEZZA;
	if (x * x + y * y <= Git * Git)
		return 1;
	else
		return 0;
}

//calcola area assedio
int assaltolecito(int PosT, int PosC)
{
	int x = PosT % LARGHEZZA - PosC % LARGHEZZA;
	int y = PosT / LARGHEZZA - PosC / LARGHEZZA;
	if (x * x + y * y <= 1)
		return 1;
	else
		return 0;
}

//elimina i puntatori alle unita morte
void eliminamorti(t_infotruppa* M)
{
	int G = M->giocatore;
	int Pos;
	t_lista_t* T = giocatore[G]->truppe;
	t_lista_t* Tp = NULL;
	fprintf(stderr, "debug: eliminamorti\n");
	while (T != NULL && T->truppa->numero != 0 && T->truppa->morale != 0)
	{
		Tp = T;
		T = T->next;
	}
	if (Tp != NULL)
	{
		Pos = T->pos;
		Tp->next = T->next;
		free(T);
		infomappa.truppe[Pos] = NULL;
	}
	else
	{
		Pos = T->pos;
		giocatore[G]->truppe = T->next;
		free(T);
		infomappa.truppe[Pos] = NULL;
	}
}

//elimina i puntatori alle unita morte nelle strutture
void eliminamortistrutture(int Pos)
{
	t_lista_s* S;
	t_lista_t* T;
	t_lista_t* Tp = NULL;
	S = puntastruttura(Pos);
	T = S->in;
	while (T != NULL && T->truppa->numero != 0 && T->truppa->morale != 0)
	{
		Tp = T;
		T = T->next;
	}
	if (Tp != NULL)
	{
		Tp->next = T->next;
		free(T);
	}
	else
	{
		S->in = T->next;
		free(T);
	}
}

//fa combattere due unita'
void combatti(t_infotruppa* Attaccante, t_infotruppa* Difensore, char m)
{
	int Pa, Pd;
	int Bpa, Bma;
	int Bpd, Bmd;
	int na, nd;
	float E;
	float M;
	int D;
	fprintf(stderr, "debug: combatti\n");
//calcolo potenza attacco attaccante
	na = Attaccante->numero;
	Bpa = Dtruppa[Attaccante->tipo].att;
	Bma = Attaccante->morale;
	if (Attaccante->tipo == Dra || Attaccante->tipo == Fen)
		Pa = Bpa + Bma / 10;
	else
		Pa = Bpa * na + Bma * na / 10;
//calcolo potenza difesa difensore
	nd = Difensore->numero;
	Bpd = Dtruppa[Difensore->tipo].dif;
	Bmd = Difensore->morale;
	if (Difensore->tipo == Dra || Difensore->tipo == Fen)
		Pd = Bpd + Bmd / 10;
	else
		Pd = Bpd * nd + Bmd * nd / 10;
//calcolo dell'efficenza con la funzione versiera
	E = (float) (1600) / ((na - 100) * (na - 100) + 1600); //calcola versiera in x(a=20 x=X-100)moltiplicata per 1/40
	E *= Bpa; //calcola efficenza d'attacco truppa
	switch (m)
	{
	case 's':
		Pd += 150;
		break;
	case 'c':
		Pd += 300;
		break;
	case 'n':
	default:
		break;
	}
//calcola moltiplicatore
	M = (float) Pa / (float) Pd;
//calcola danni netti
	D = (int) (M * E);
	if (D > 5)
		nd -= D;
	else
		nd -= 5;
// gestisce le unità di pochi uomini
	if (nd < 5)
		nd = 0;
//gestisce il morale e assegna il nuovo valore alle unità
	if (nd < Difensore->numero / 2)
		Difensore->morale /= 2;
	Difensore->numero = nd;
	return;
}

//attacca un bersaglio in campo aperto
void combatticampoaperto(int Dst, int Src)
{
	t_infotruppa* A = infomappa.truppe[Src];
	t_infotruppa* D = infomappa.truppe[Dst];
	infomappa.truppe[Src]->combattuto = 1;
//attacco
	combatti(A, D, 'n');
	if (D->numero == 0 || D->morale == 0)
		eliminamorti(D);
//contrattacco
	if (infomappa.truppe[Dst] != NULL && bersagliolecito(Dst, Src) == 1)
	{
		combatti(D, A, 'n');
		if (A->numero == 0 || A->morale == 0)
			eliminamorti(A);
	}
}

//gestisce tutto l'assedio a una struttura
void assediostruttura(int Pos)
{
	int g = -1;
	int morale = 0;
	t_struttura i;
	t_lista_s* Struttura;
	t_lista_t* Difensori;
	fprintf(stderr, "debug: click_assaltostruttura\n");
	for (i = 1; i < NUMSTRUTTURE && g < 0; i++)
		g = controlloedificio(Pos, i);
	i--;
	if (g >= 0)
	{
		fprintf(stderr, "debug: di qualcuno\n");
		Struttura = puntastruttura(Pos);
		Difensori = Struttura->in;
		if (assaltaedificio(Struttura) == 1)
		{
			while (Difensori != NULL)
			{
				Difensori->truppa->morale -= 10;
				if (Difensori->truppa->morale < 0)
					Difensori->truppa->morale = 0;
				morale += Difensori->truppa->morale;
				Difensori = Difensori->next;
			}
			Difensori = Struttura->in;
			if (morale == 0)
			{
				while (Struttura->in != NULL)
					eliminamortistrutture(Struttura->pos);
				cambiaproprietario(CurrentPlayer, g, Pos, i);
			}
		}
		else
		{
			while (Difensori != NULL)
			{
				Difensori->truppa->morale += 10;
				Difensori = Difensori->next;
			}
		}
	}
	else
	{
		fprintf(stderr, "debug: di nessuno, %d\n", g);
		switch (infomappa.mappa[Pos])
		{
		case 'G':
			i = Gro;
			break;
		case 'S':
			i = Scu;
			break;
		case 'N':
			i = Nid;
			break;
		case 'C':
			i = Fat;
			break;
		default:
			fprintf(stderr, "c'è un bug!\n");
			i = Cas;
			break;
		}
		cambiaproprietario(CurrentPlayer, g, Pos, i);
	}
}

//fa combattere gli attaccanti contro i difensori di una struttura
int assaltaedificio(t_lista_s* Edificio)
{
	fprintf(stderr, "debug: assaltamura\n");
	int i, j = 0;
	int Pos = Edificio->pos;
	t_infotruppa* Attaccanti[8];
	t_infotruppa* Difensori[8];
	t_lista_t* T;
	T = Edificio->in;
	if (T == NULL)
		return 1;
	for (i = 0; i < 8; i++)
		Difensori[i] = NULL;
	for (i = 0; i < 8; i++)
		Attaccanti[i] = NULL;
	do
	{
		Attaccanti[j] = puntacasellaoccupata(Pos, j);
		j++;
	} while (Attaccanti[j - 1] == NULL && j < 8);
	for (i = 0; T != NULL && i < j && i < 8; i++)
	{
		Difensori[i] = T->truppa;
		T = T->next;
	}
	for (i = 0; i < j && Difensori[i] != NULL; i++)
	{
		Attaccanti[i]->combattuto = 1;
		Difensori[i]->combattuto = 1;
		combatti(Attaccanti[i], Difensori[i], 's');
		if (Difensori[i]->numero == 0)
		{
			eliminamortistrutture(Pos);
			return 1;
		}
		combatti(Difensori[i], Attaccanti[i], 'n');
		if (Attaccanti[i]->numero == 0)
			eliminamorti(Attaccanti[i]);
	}
	return 0;
}

//gestisce tutto l'assedio al castello
void assediocastello(int Pos)
{
	int g;
	int C = 0;
	t_lista_s* Castello;
	g = controlloedificio(Pos, Cas);
	if (g >= 0)
	{
		Castello = giocatore[g]->struttura[Cas];
		while (Castello->pos != Pos)
			Castello = Castello->next;
		if (assaltamura(Castello) == 1)
			if (assaltabreccia(puntacasellaoccupata(Pos, C++), Castello->in) == 1)
				cambiaproprietario(CurrentPlayer, g, Pos, Cas);
	}
	else
	{
		cambiaproprietario(CurrentPlayer, g, Pos, Cas);
	}
}

//fa combattere gli attaccanti contro la prima linea di un castello presidiato
int assaltamura(t_lista_s* Castello)
{
	fprintf(stderr, "debug: assaltamura\n");
	int i, j = 0;
	int Pos = Castello->pos;
	t_infotruppa* Attaccanti[12];
	t_infotruppa* Difensori[12];
	t_lista_t* T;
	T = Castello->in;
	fprintf(stderr, "debug: assaltamura: Pos= %d\n", Pos);
	if (T == NULL)
		return 1;
	for (i = 0; i < 12; i++)
		Difensori[i] = NULL;
	for (i = 0; i < 12; i++)
		Attaccanti[i] = NULL;
	do
	{
		Attaccanti[j] = puntacasellaoccupata(Pos, j);
		j++;
	} while (Attaccanti[j - 1] != NULL && j < 12);
	for (i = 0; T != NULL && i < j - 1 && i < 12; i++)
	{
		Difensori[i] = T->truppa;
		T = T->next;
	}
	for (i = 0; i < j && Difensori[i] != NULL; i++)
	{
		Attaccanti[i]->combattuto = 1;
		Difensori[i]->combattuto = 1;
		combatti(Attaccanti[i], Difensori[i], 'm');
		if (Difensori[i]->numero == 0)
		{
			eliminamortistrutture(Pos);
			return 1;
		}
		combatti(Difensori[i], Attaccanti[i], 'n');
		if (Attaccanti[i]->numero == 0)
			eliminamorti(Attaccanti[i]);
	}
	return 0;
}

//fa combattere gli attaccanti contro la seconda linea di un castello presidiato
int assaltabreccia(t_infotruppa* Attaccante, t_lista_t* Difensori)
{
	t_lista_t* Temp;
	t_lista_t* Secondalinea;
	fprintf(stderr, "debug: assaltabreccia\n");
	while (Difensori != NULL)
	{
		Secondalinea = Difensori;
		while (Secondalinea != NULL && Secondalinea->truppa->combattuto == 1)
			Secondalinea = Secondalinea->next;
		if (Secondalinea == NULL && Difensori != NULL)
		{
			Secondalinea = Difensori;
			while (Secondalinea != NULL)
			{
				Secondalinea->truppa->morale /= 3;
				Secondalinea = Secondalinea->next;
			}
			Secondalinea = Difensori;
		}
		combatti(Attaccante, Secondalinea->truppa, 'n');
		if (Difensori->truppa->numero == 0 || Difensori->truppa->morale == 0)
		{
			//abbassa il morale dei difensori
			Temp = Difensori->next;
			while (Temp != NULL)
			{
				Temp->truppa->morale -= 10;
				if (Temp->truppa->morale < 0)
					Temp->truppa->morale = 0;
				Temp = Temp->next;
			}
			//fa morire le truppe in rotta o morte
			Temp = Difensori;
			while (Temp != NULL)
			{
				if (Temp->truppa->morale == 0 || Temp->truppa->numero == 0)
				{
					if (Temp == Difensori)
						Difensori = Temp->next;
					eliminamortistrutture(Temp->pos);
				}
				Temp = Temp->next;
			}
		}
		else
		{
			combatti(Secondalinea->truppa, Attaccante, 'n');
			if (Attaccante->numero == 0 || Attaccante->morale == 0)
				eliminamorti(Attaccante);
			return 0;
		}
	}
	return 1;
}

//cambia il proprietario a un edificio
void cambiaproprietario(int g1, int g2, int Pos, t_struttura Tipo)
{
	t_lista_s* S;
	t_lista_s* Sp;
	fprintf(stderr, "debug: cambiaproprietario\n");
	if (g2 < 0)
	{
		S = giocatore[g1]->struttura[Tipo];
		if (S != NULL)
		{
			while (S->next != NULL)
				S = S->next;
			S->next = (t_lista_s*) malloc(sizeof(t_lista_s));
			S->next->in = NULL;
			S->next->pos = Pos;
			S->next->next = NULL;
		}
		else
		{
			giocatore[g1]->struttura[Tipo] = (t_lista_s*) malloc(sizeof(t_lista_s));
			giocatore[g1]->struttura[Tipo]->in = NULL;
			giocatore[g1]->struttura[Tipo]->pos = Pos;
			giocatore[g1]->struttura[Tipo]->next = NULL;
		}
	}
	else
	{
		S = giocatore[g2]->struttura[Tipo];
		Sp = NULL;
		while (S->pos != Pos)
		{
			Sp = S;
			S = S->next;
		}
		if (Sp != NULL)
			Sp->next = S->next;
		else
			giocatore[g2]->struttura[Tipo] = NULL;
		Sp = giocatore[g1]->struttura[Tipo];
		if (Sp != NULL)
		{
			while (Sp->next != NULL)
				Sp = Sp->next;
			Sp->next = S;
			S->next = NULL;
		}
		else
		{
			giocatore[g1]->struttura[Tipo] = (t_lista_s*) malloc(sizeof(t_lista_s));
			giocatore[g1]->struttura[Tipo]->in = NULL;
			giocatore[g1]->struttura[Tipo]->pos = Pos;
			giocatore[g1]->struttura[Tipo]->next = NULL;
		}
	}
}
//controlla se il giocatore è stato sconfitto
int controllosconfitto(int G)
{
	int i;

	if (G == -1)
		return 0;

	for (i = 0; i < NUMSTRUTTURE; i++)
	{
		if (giocatore[G]->struttura[i] != NULL)
			return 0;
	}
	return 1;
}

//controlla se c'è un giocatore che ha vinto, e ritorna il suo numero
int controllovincitore()
{
	int i, j = 0;
	int w = 0;

	for (i = 0; i < MAXGIOCATORI && j < 2; i++)
	{
		if (giocatore[i] != NULL)
		{
			j++;
			w = i;
		}
	}
	if (j > 1)
		return 0;
	else
		return w + 1;
}

//controlla il tipo della struttura
t_struttura controllotipostruttura(int Pos)
{
	int i;
	t_struttura t;
	int* s[] =
	{ infomappa.castelli, infomappa.fattorie, infomappa.stalle, infomappa.grotte, infomappa.nidi };
	int n[] =
	{ NUMCASTELLI, infomappa.numfattorie, infomappa.numstalle, infomappa.numgrotte,
			infomappa.numnidi };
	for (t = Cas; t < NUMSTRUTTURE; t++)
		for (i = 0; i < n[t]; i++)
			if (s[t][i] == Pos)
				return t;
	return -1;
}

//controlla se la struttura e' del giocatore
int controlloedificio(int Pos, t_struttura s)
{
	int i;
	t_lista_s* L;
	for (i = 0; i < MAXGIOCATORI; i++)
	{
		if (giocatore[i] != NULL)
		{
			L = giocatore[i]->struttura[s];
			while (L != NULL)
			{
				if (L->pos == Pos)
					return i;
				else
					L = L->next;
			}
		}
	}
	return -1;
}

//controlla a quale giocatore appartiene l'unità
int controllounita(int Pos)
{
	int i;
	t_lista_t* L;
	for (i = 0; i < MAXGIOCATORI; i++)
	{
		if (giocatore[i] != NULL)
		{
			L = giocatore[i]->truppe;
			while (L != NULL)
			{
				if (L->pos == Pos)
					return i;
				else
					L = L->next;
			}
		}
	}
	return -1;
}

//controlla che unità e struttura non siano dello stesso giocatore
int controllodiverso(int PosT, int PosS, t_struttura tipo)
{
	if (controlloedificio(PosS, tipo) == controllounita(PosT))
		return 0;
	else
		return 1;
}

//controlla che le unita non siano dello stesso giocatore
int controllodiversotruppe(int PosA, int PosD)
{
	if (PosA == PosD)
		return 1;
	if (controllounita(PosA) == controllounita(PosD))
		return 0;
	else
		return 1;
}

//punta alla truppa precedente nella struttura scelta
t_lista_t* puntatruppaprecedente(t_lista_t *T, t_lista_s *S)
{
	t_lista_t *Tp;
	if (S->in == T)
		return NULL;
	else
	{
		Tp = S->in;
		while (Tp->next != T)
			Tp = Tp->next;
		return Tp;
	}
}
//punta alla lista struttura della struttura nella posizione desiderata
t_lista_s* puntastruttura(int Pos)
{
	int G;
	t_struttura i;
	t_lista_s* L;
	for (G = 0; G < MAXGIOCATORI; G++)
	{
		if (giocatore[G] != NULL)
		{
			for (i = Cas; i <= Nid; i++)
			{
				L = giocatore[G]->struttura[i];
				while (L != NULL)
				{
					if (L->pos == Pos)
						return L;
					else
						L = L->next;
				}
			}
		}
	}
	return NULL;
}

//punta alla prima casella libera disponibile attorno a una struttura
t_infotruppa** puntacasellalibera(int Pos)
{
	int x, y;
	int i, j;
	int hi, hj, bj, bi;
	x = Pos % LARGHEZZA;
	y = Pos / LARGHEZZA;
	if (infomappa.mappa[Pos - 1] != ' ')
	{
		bi = -2;
		bj = -2;
		//evita di andare fuori dal limite basso
		if (x + bi < 0)
			bi++;
		if (y + bj < 0)
			bj++;
		//evita di andare fuori dal limite alto
		for (hi = 1; x + hi < LARGHEZZA && hi < 2; hi++)
			;
		for (hj = 1; y + hj < ALTEZZA && hj < 2; hj++)
			;
		//iniziallizza i e j
		i = bi;
		j = bj;
		while ((accedi(x+i,y+j,infomappa.truppe)!=NULL || accedi(x+i,y+j,infomappa.mappa)!=' ')&& j<=hj)
		{
			i++;
			if(i>hi)
			{
				i=bi;
				j++;
			}
		}
		if (j > hj)
			return NULL;
		else
		{
			x += i;
			y += j;
			return &infomappa.truppe[x + y * LARGHEZZA];
		}
	}
	else
	{
		bi = -1;
		bj = -1;
		//evita di andare fuori dal limite basso
		if (x + bi < 0)
			bi++;
		if (y + bj < 0)
			bj++;
		//evita di andare fuori dal limite alto
		for (hi = 1; x + hi < LARGHEZZA && hi <= 2; hi++)
			;
		for (hj = 1; y + hj < ALTEZZA && hj <= 2; hj++)
			;
		//iniziallizza i e j
		i = bi;
		j = bj;
		while ((accedi(x+i,y+j,infomappa.truppe)!=NULL || accedi(x+i,y+j,infomappa.mappa)!=' ')&& j<=hj)
		{
			i++;
			if(i>hi)
			{
				i=bi;
				j++;
			}
		}
		if (j > hj)
			return NULL;
		else
		{
			x += i;
			y += j;
			return &infomappa.truppe[x + y * LARGHEZZA];
		}
	}
	return NULL;
}

//punta alla prossima casella occupata attorno a una struttura
t_infotruppa* puntacasellaoccupata(int Pos, int C)
{
	int x, y;
	int i = 0, j, k = 0;
	int hi, hj, bj, bi, m;
	x = Pos % LARGHEZZA;
	y = Pos / LARGHEZZA;
	fprintf(stderr, "debug: puntacasellaoccupata\n");
	if (infomappa.mappa[Pos - 1] != ' ') //se è un castello
	{
		fprintf(stderr, "debug: puntacasellaoccupata prova lato\n");
		bi = -2;
		bj = -2;
		m = -2;
		//evita di andare fuori dal limite basso
		if (x + bi < 0)
			bi++;
		if (y + bj < 0)
			bj++;
		//evita di andare fuori dal limite alto
		for (hi = 1; x + hi < LARGHEZZA && hi < 2; hi++)
			;
		for (hj = 1; y + hj < ALTEZZA && hj < 2; hj++)
			;
	}
	else // se è una struttura
	{
		bi = -1;
		bj = -1;
		m = -1;
		//evita di andare fuori dal limite basso
		if (x + bi < 0)
			bi++;
		if (y + bj < 0)
			bj++;
		//evita di andare fuori dal limite alto
		for (hi = 1; x + hi < LARGHEZZA && hi < 2; hi++)
			;
		for (hj = 1; y + hj < ALTEZZA && hj < 2; hj++)
			;
	}
	for (j = bj; k <= C && j <= hj; j++)
		for (i = bi; k <= C && i <= hi; i++)
		{
			if (!((i == m || i == 2) && (j == m || j == 2)))
			{
				if ((accedi(x+i,y+j,infomappa.truppe)!=NULL))k++;
			}
		}
	i--;
	j--;
	if (k == C)
	{
		C = 0;
		return NULL;
	}
	else
	{
		C++;
		x += i;
		y += j;
		return infomappa.truppe[x + y * LARGHEZZA];
	}
}

//aggiorna tr_callback
void aggiorna_tr_callback(char* pos)
{
	t_truppa i;
	for (i = Rec; i < NUMTRUPPE; i++)
	{
		tr_callback[i].tipo = i;
		tr_callback[i].pos = pos - infomappa.mappa;
	}
}

//termina il turno
void fineturno()
{
	int i;
	int r[NUMSTRUTTURE];
	int c = 0;
	int n = 0;
	int f;
	t_lista_t* T = giocatore[CurrentPlayer]->truppe;
	t_lista_s* S;
	for (i = 0; i < NUMSTRUTTURE; i++)
		r[i] = 0;
//conta le truppe, il loro numero e aggiorna loro stato
	while (T != NULL)
	{
		T->truppa->stanca = 0;
		T->truppa->combattuto = 0;
		if (T->truppa->tipo < Cav)
			c += T->truppa->numero / 10;
		else if (T->truppa->tipo == Cav)
			c += 2 * T->truppa->numero;
		else
			c += 30;
		T = T->next;
		n++;
	}
//conta le strutture
	for (i = 0; i < NUMSTRUTTURE; i++)
	{
		S = giocatore[CurrentPlayer]->struttura[i];
		while (S != NULL)
		{
			r[i]++;
			S = S->next;
		}
	}
//calcola le risorse per il prossimo turno
	giocatore[CurrentPlayer]->oro += PROD_ORO_CAS * r[Cas] + PROD_ORO_FAT * r[Fat]
			+ PROD_ORO_SCU * r[Scu] + PROD_ORO_NID * r[Nid] + PROD_ORO_GRO * r[Gro];
	giocatore[CurrentPlayer]->smeraldi += PROD_SMERALDI_NID * r[Nid] + PROD_SMERALDI_GRO * r[Gro];
	giocatore[CurrentPlayer]->cibo += PROD_CIBO_CAS * r[Cas] + PROD_CIBO_FAT * r[Fat]
			+ PROD_CIBO_SCU * r[Scu] - FAME * c;
//introduce il concetto di cibo da comprare e morte e scoraggiamento per fame
	if (giocatore[CurrentPlayer]->cibo < 0)
	{
		f = giocatore[CurrentPlayer]->cibo;
		giocatore[CurrentPlayer]->cibo = 0;
		giocatore[CurrentPlayer]->oro += f;
		if (giocatore[CurrentPlayer]->oro < 0)
		{
			f = giocatore[CurrentPlayer]->oro;
			giocatore[CurrentPlayer]->oro = 0;
			giocatore[CurrentPlayer]->smeraldi += f;
			if (giocatore[CurrentPlayer]->smeraldi < 0)
			{
				f = giocatore[CurrentPlayer]->smeraldi;
				giocatore[CurrentPlayer]->smeraldi = 0;
				f = f / n;
				f = (f <= 0) ? -1 : f;
				T = giocatore[CurrentPlayer]->truppe;
				while (T != NULL)
				{
					T->truppa->morale -= 1;
					if (T->truppa->tipo < Dra)
						T->truppa->numero += f;
					if (T->truppa->numero <= 0)
						eliminamorti(T->truppa);
					T = T->next;
				}
			}
		}
	}
//aggiorna il giocatore corrente
	do
	{
		do
		{
			CurrentPlayer++;
			CurrentPlayer %= MAXGIOCATORI;
		} while (giocatore[CurrentPlayer] == NULL);
		if (infogiocatore[CurrentPlayer]->tipo == IA)
			fantasia_usa_bot(CurrentPlayer);
	} while (infogiocatore[CurrentPlayer]->tipo == IA);
	return;
}

// stampa la mappa
void visualizza_su_terminale()
{
	int i, j;
	for (i = 0; i < ALTEZZA; i++)
	{
		for (j = 0; j < LARGHEZZA; j++)
		{
			printf("%c", accedi(j, i, infomappa.mappa));
		}
		printf("\n");
	}
}
