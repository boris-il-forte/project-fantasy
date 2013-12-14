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

#include "fantasy-gtk-img.h"

int Dim_casella;
int caselle_orizzontali, caselle_verticali;

Immagini Immagine;

//calcola le dimensioni dei widget
void gtk_calcola_dimensioni()
{
	int w, h;
	int wof, hof;

	w = gdk_screen_get_width(gdk_screen_get_default());
	h = gdk_screen_get_height(gdk_screen_get_default());
	Dim_casella = 0.05 * MIN(w, h) + 3; //FIXME
	wof = 4 * Dim_casella;
	hof = 7 * Dim_casella;
	caselle_orizzontali = (w - wof) / Dim_casella; // 27 eee
	caselle_verticali = (h - hof) / Dim_casella; // 16 eee
	printf("\nCasella: %dpx\nOrizzontali: %d Verticali: %d\n", Dim_casella,
			caselle_orizzontali, caselle_verticali);
}

void gtk_carica_immagine_skin(char name[64], GdkPixbuf **immagine,
		int resizeFactor)
{
	char BufErr[128];
	char Buf[128];
	const char *err = "impossibile caricare l'immagine ";
	int size = Dim_casella / resizeFactor;
	// carico immagini da skin
	sprintf(Buf, "skin/");
	strcat(Buf, infogioco.skin);
	strcat(Buf, "/");
	strcat(Buf, name);
	strcat(Buf, ".");
	strcat(Buf, infogioco.ext);
	if ((*immagine = gdk_pixbuf_new_from_file_at_size(Buf, size, size, NULL))
			== NULL)
	{
		sprintf(BufErr, "%s", err);
		strcat(BufErr, Buf);
		perror(BufErr);
		*immagine = Immagine.err;
	}

}

void gtk_carica_immagini_gioco()
{
	//carico immagini del gioco
	char Buf[64];
	sprintf(Buf, "img/fantasy-icon.png");
	if ((Immagine.err = gdk_pixbuf_new_from_file_at_size(Buf, Dim_casella,
			Dim_casella, NULL)) == NULL)
	{
		perror("impossibile caricare l'immagine fantasy-icon.png");
		exit(1);
	}
	if ((Immagine.logo = gdk_pixbuf_new_from_file_at_size(Buf, Dim_casella,
			Dim_casella, NULL)) == NULL)
	{
		perror("impossibile caricare l'immagine fantasy-icon.png");
		exit(1);
	}
	if ((Immagine.decorazione = gdk_pixbuf_new_from_file_at_size(Buf,
			Dim_casella / 2, Dim_casella / 2, NULL)) == NULL)
	{
		perror("impossibile caricare l'immagine fantasy-icon.png");
		exit(1);
	}
	sprintf(Buf, "img/freccia.png");
	if ((Immagine.freccia = gdk_pixbuf_new_from_file_at_size(Buf,
			Dim_casella - 7, Dim_casella - 7, NULL)) == NULL)
	{
		perror("impossibile caricare l'immagine freccia.png");
		exit(1);
	}
}

//carica le immagini del gioco
void gtk_carica_immagini()
{
	int i, j;

	char nomeImmagine[64];
	//carico il file di configurazione
	caricaconfig("fantasy.config");

	//carico immagini del gioco
	gtk_carica_immagini_gioco();
	// carico immagini da skin
	gtk_carica_immagine_skin("mov", &Immagine.movimento, 1);
	gtk_carica_immagine_skin("atk", &Immagine.attacco, 1);
	gtk_carica_immagine_skin("atk2", &Immagine.assalto, 1);
	gtk_carica_immagine_skin("ent", &Immagine.entrata, 1);

	//carica le immagini del prato
	for (i = 0; i < 5; i++)
	{
		sprintf(nomeImmagine, "p/p%d", i);
		gtk_carica_immagine_skin(nomeImmagine, &Immagine.p[i], 1);
	}

	for (i = 0; i < MAXGIOCATORI; i++)
	{
		//carica gli scudi colorati
		sprintf(nomeImmagine, "a/%d", i);
		gtk_carica_immagine_skin(nomeImmagine, &Immagine.a[i], 2);

		//carica le truppe
		for (j = 0; j <= Fen; j++)
		{
			sprintf(nomeImmagine, "t/t%d%d", i, j);
			gtk_carica_immagine_skin(nomeImmagine, &Immagine.t[i][j], 1);

		}
	}

	for (i = 0; i <= MAXGIOCATORI; i++)
	{
		//carica il castello
		for (j = 0; j < 9; j++)
		{
			sprintf(nomeImmagine, "c/%d/c%d", i, j, 1);
			gtk_carica_immagine_skin(nomeImmagine, &Immagine.c[i][j], 1);
		}

		//carica le altre strutture
		for (j = 0; j < 4; j++)
		{
			//carica la grotta
			sprintf(nomeImmagine, "g/%d/g%d", i, j);
			gtk_carica_immagine_skin(nomeImmagine, &Immagine.g[i][j], 1);
			//carica la stalla
			sprintf(nomeImmagine, "s/%d/s%d", i, j);
			gtk_carica_immagine_skin(nomeImmagine, &Immagine.s[i][j], 1);
			//carica la fattoria
			sprintf(nomeImmagine, "f/%d/f%d", i, j);
			gtk_carica_immagine_skin(nomeImmagine, &Immagine.f[i][j], 1);
			//carica il nido
			sprintf(nomeImmagine, "n/%d/n%d", i, j);
			gtk_carica_immagine_skin(nomeImmagine, &Immagine.n[i][j], 1);
		}
	}

}

