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

#include "fantasy-core-mov.h"

#include <stdlib.h>
#include <stdio.h>

static char **Grafo = NULL;
static int **Valore = NULL;
static int Q = 0;
static int Mx, Vel;

// calcola se lo spostamento è lecito
int spostalecito(int PosT, int PosC)
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
				if (Valore[c - x - 1][c - y] <= s || Valore[c - x + 1][c - y] <= s
						|| Valore[c - x][c - y - 1] <= s || Valore[c - x][c - y + 1] <= s)
				{
					return 1;
				}
				else
					return 0;
			}
			else if (Valore[c - x][c - y] <= (Vel * 100))
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

//segna un edificio come raggiungibile
int raggiungibile(int posizioneStruttura, t_struttura tipo, int posizioneTruppa)
{
	int i;
	int posizioni[8];
	int lecito = 0;
	int n = posizionistruttura(posizioneStruttura, posizioni, tipo);
	for (i = 0; i < n; i++)
		lecito = lecito || spostalecito(posizioneTruppa, posizioni[i]);
	return lecito;
}

//segna un edificio come assaltabile
int assaltabile(int posizioneStruttura, t_struttura tipo, int posizioneTruppa)
{
	int i;
	int posizioni[8];
	int lecito = 0;
	int n = posizionistruttura(posizioneStruttura, posizioni, tipo);
	for (i = 0; i < n; i++)
		lecito = lecito || assaltolecito(posizioneTruppa, posizioni[i]);
	return lecito;
}

//calcola il costo del percorso più breve utilizzando l'algoritmo di Dijkstra
void inizializza_dijkstra(int PosT)
{
	int Cs;
	int Sx, Sy;
	int i, j;
	Q = 0;
	Sx = PosT % LARGHEZZA;
	Sy = PosT / LARGHEZZA;
	t_truppa Tipo;
	//inizializza strutture dati
	Tipo = infomappa.truppe[PosT]->tipo;
	Vel = Dtruppa[Tipo].vel;
	Mx = 2 * Vel + 3;
	Cs = Vel + 1;
	Valore = malloc(sizeof(int*) * Mx);
	Grafo = malloc(sizeof(char*) * Mx);
	if (Valore == NULL || Grafo == NULL)
	{
		perror("malloc fallita");
		exit(1);
	}
	for (i = 0; i < Mx; i++)
	{
		Valore[i] = malloc(sizeof(int) * Mx);
		Grafo[i] = malloc(sizeof(char) * Mx);
		if (Valore[i] == NULL || Grafo[i] == NULL)
		{
			perror("malloc fallita");
			exit(1);
		}
	}
	//prepara grafo e contatore Q
	for (i = 0; i < Mx; i++)
	{
		Grafo[i][0] = '#';
		Grafo[i][Mx - 1] = '#';
	}
	for (i = 0; i < Mx; i++)
	{
		Grafo[0][i] = '#';
		Grafo[Mx - 1][i] = '#';
	}
	for (i = 1; i < Mx - 1; i++)
		for (j = 1; j < Mx - 1; j++)
		{
			if ((Sx + i - Cs) < 0||(Sx+i-Cs)>=LARGHEZZA ||(Sy+j-Cs)<0 ||(Sy+j-Cs)>=ALTEZZA)
				Grafo[i][j] = '#';
			else if ((Cs - i) * (Cs - i) + (Cs - j) * (Cs - j)
					> Vel
							* Vel|| infomappa.mappa[posiziona(Cs-i,Cs-j,Sx,Sy)]!=' ' || infomappa.truppe[posiziona(Cs-i,Cs-j,Sx,Sy)]!=NULL)
				Grafo[i][j] = '#';
			else
			{
				Grafo[i][j] = '.';
				Q++;
			}
		}

	//inizializzo Valore
	for (i = 0; i < Mx; i++)
		for (j = 0; j < Mx; j++)
			Valore[i][j] = Vel * 100 + 1;
	Grafo[Cs][Cs] = 'S'; //segna il source
	Valore[Cs][Cs] = 0;
#ifdef DEBUG
	// stampo il grafo
	for(i=0;i<Mx;i++)
	{
		for(j=0;j<Mx;j++)
		printf("%c",Grafo[j][i]);
		printf("\n");
	}
#endif

}

void calcola_dijkstra()
{
	const int Cs = Vel + 1;
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
					if (Grafo[X][Y] == '.') //se non è stato ancora contato...
					{
						Q--; //conta un elemento in meno dal totale
						Grafo[X][Y] = '+'; //segnalo come contato
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
					if (Grafo[X + k][Y + l] != '#' && Valore[X + k][Y + l] > Valore[X][Y] + w)
					{
						if (w == 100 || !(Grafo[X + k][Y] == '#' && Grafo[X][Y + l] == '#'))
						{
							Valore[X + k][Y + l] = Valore[X][Y] + w;
						}
					}
				}
		}
		//printf("secondo passo ok \n");
		v++; //passa al prossimo vertice
		vp = 0; //annulla il conteggio dei nuovi vertici
	}
}

void pulisci_dijkstra()
{
	int i;
	for (i = 0; i < Mx; i++)
	{
		free(Valore[i]);
		free(Grafo[i]);
	}

	free(Valore);
	free(Grafo);
	Valore = NULL;
	Grafo = NULL;
}

void calcolaspostamento(int Mossa)
{
	if (Grafo != NULL && Valore != NULL)
	{
		pulisci_dijkstra();
	}

	inizializza_dijkstra(Mossa);
	calcola_dijkstra();
}

