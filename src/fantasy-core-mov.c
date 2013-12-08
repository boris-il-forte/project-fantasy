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

//segna un edificio come raggiungibile
int raggiungibile(int posizioneStruttura, t_struttura tipo, int posizioneTruppa, int** V)
{
	int i;
	int posizioni[8];
	int lecito = 0;
	int n = posizionistruttura(posizioneStruttura, posizioni, tipo);
	for (i = 0; i < n; i++)
		lecito = lecito || spostalecito(posizioneTruppa, posizioni[i], V);
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





