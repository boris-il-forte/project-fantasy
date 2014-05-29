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

#ifndef FANTASY_GTK_IMG_H_
#define FANTASY_GTK_IMG_H_

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "fantasy-core.h"


typedef struct immagini_s
{
	GdkPixbuf* logo;
	GdkPixbuf* decorazione;
	GdkPixbuf* freccia;
	GdkPixbuf* cibo;
	GdkPixbuf* oro;
	GdkPixbuf* smeraldi;
	GdkPixbuf* movimento;
	GdkPixbuf* entrata;
	GdkPixbuf* attacco;
	GdkPixbuf* assalto;
	GdkPixbuf* prato[5]; // immagine del prato
	GdkPixbuf* castello[MAXGIOCATORI+1][9]; // matrice che contiene i pezzi dei 12 castelli di proprietà + il castello vuoto
	GdkPixbuf* grotta[MAXGIOCATORI+1][4]; // matrice che contiene i pezzi delle 12 grotte di proprietà + la grotta vuota
	GdkPixbuf* scuderia[MAXGIOCATORI+1][4]; // matrice che contiene i pezzi delle 12 scuderie di proprietà + la scuderia vuota
	GdkPixbuf* nido[MAXGIOCATORI+1][4]; // matrice che contiene i pezzi dei 12 nidi di proprietà + il nido vuoto
	GdkPixbuf* fattoria[MAXGIOCATORI+1][4]; // matrice che contiene i pezzi delle 12 fattorie di proprietà + la fattoria vuota
	GdkPixbuf* truppa[MAXGIOCATORI][NUMTRUPPE]; // matrice che contiene le truppe dei 12 giocatori
	GdkPixbuf* scudo[MAXGIOCATORI]; // array con gli scudi dei giocatori
	GdkPixbuf* campo[MAXGIOCATORI]; // array con le immagini del campo di grano
	GdkPixbuf* err;
} Immagini;

void gtk_carica_immagini();
void gtk_calcola_dimensioni();

extern Immagini Immagine;
extern int Dim_casella;
extern int caselle_orizzontali,caselle_verticali;



#endif /* FANTASY_GTK_IMG_H_ */
