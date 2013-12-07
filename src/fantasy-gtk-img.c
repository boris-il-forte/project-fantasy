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
int caselle_orizzontali,caselle_verticali;

Immagini Immagine;

//calcola le dimensioni dei widget
void gtk_calcola_dimensioni()
{
	int w, h;
	int wof, hof;

	w = gdk_screen_get_width(gdk_screen_get_default());
	h = gdk_screen_get_height(gdk_screen_get_default());
	Dim_casella = 0.05 * MIN(w,h) + 3; //FIXME
	wof = 4 * Dim_casella;
	hof = 7 * Dim_casella;
	caselle_orizzontali = (w - wof) / Dim_casella; // 27 eee
	caselle_verticali = (h - hof) / Dim_casella; // 16 eee
	printf("\nCasella: %dpx\nOrizzontali: %d Verticali: %d\n", Dim_casella,
			caselle_orizzontali, caselle_verticali);
}

//carica le immagini del gioco
void gtk_carica_immagini()
{
	int i, j;
	char Buf[64];
	char Buf2[64];
	char BufErr[128];
	const char *err = "impossibile caricare l'immagine ";
	//carico il file di configurazione
	caricaconfig("fantasy.config");
	//carico immagini del gioco
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
	// carico immagini da skin
	sprintf(Buf, "skin/");
	strcat(Buf, infogioco.skin);
	strcat(Buf, "/mov.");
	strcat(Buf, infogioco.ext);
	if ((Immagine.movimento = gdk_pixbuf_new_from_file_at_size(Buf, Dim_casella,
			Dim_casella, NULL)) == NULL)
	{
		sprintf(BufErr, "%s", err);
		strcat(BufErr, Buf);
		perror(BufErr);
		Immagine.movimento = Immagine.err;
	}
	sprintf(Buf, "skin/");
	strcat(Buf, infogioco.skin);
	strcat(Buf, "/atk.");
	strcat(Buf, infogioco.ext);
	if ((Immagine.attacco = gdk_pixbuf_new_from_file_at_size(Buf, Dim_casella,
			Dim_casella, NULL)) == NULL)
	{
		sprintf(BufErr, "%s", err);
		strcat(BufErr, Buf);
		perror(BufErr);
		Immagine.attacco = Immagine.err;
	}

	//carica le immagini del prato
	for (i = 0; i < 5; i++)
	{
		sprintf(Buf2, "/p/p%d.", i);
		sprintf(Buf, "skin/");
		strcat(Buf, infogioco.skin);
		strcat(Buf, Buf2);
		strcat(Buf, infogioco.ext); // non esistono XPM ancora. colossale standardizzazione richiesta per le skin.
		if ((Immagine.p[i] = gdk_pixbuf_new_from_file_at_size(Buf, Dim_casella,
				Dim_casella, NULL)) == NULL)
		{
			sprintf(BufErr, "%s", err);
			strcat(BufErr, Buf);
			perror(BufErr);
			Immagine.p[i] = Immagine.err;
		}
	}

	//carica gli scudi colorati
	for (i = 0; i < MAXGIOCATORI; i++)
	{
		sprintf(Buf2, "/a/%d.", i);
		sprintf(Buf, "skin/");
		strcat(Buf, infogioco.skin);
		strcat(Buf, Buf2);
		strcat(Buf, infogioco.ext);
		if ((Immagine.a[i] = gdk_pixbuf_new_from_file_at_size(Buf,
				Dim_casella / 2, Dim_casella / 2, NULL)) == NULL)
		{
			sprintf(BufErr, "%s", err);
			strcat(BufErr, Buf);
			perror(BufErr);
			Immagine.a[i] = Immagine.err;
		}
	}
	//carica il castello
	for (i = 0; i <= MAXGIOCATORI; i++)
		for (j = 0; j < 9; j++)
		{
			sprintf(Buf2, "/c/%d/c%d.", i, j);
			sprintf(Buf, "skin/");
			strcat(Buf, infogioco.skin);
			strcat(Buf, Buf2);
			strcat(Buf, infogioco.ext);
			if ((Immagine.c[i][j] = gdk_pixbuf_new_from_file_at_size(Buf,
					Dim_casella, Dim_casella, NULL)) == NULL)
			{
				sprintf(BufErr, "%s", err);
				strcat(BufErr, Buf);
				perror(BufErr);
				Immagine.c[i][j] = Immagine.err;
			}
		}
	//carica la grotta
	for (i = 0; i <= MAXGIOCATORI; i++)
		for (j = 0; j < 4; j++)
		{
			sprintf(Buf2, "/g/%d/g%d.", i, j);
			sprintf(Buf, "skin/");
			strcat(Buf, infogioco.skin);
			strcat(Buf, Buf2);
			strcat(Buf, infogioco.ext);
			if ((Immagine.g[i][j] = gdk_pixbuf_new_from_file_at_size(Buf,
					Dim_casella, Dim_casella, NULL)) == NULL)
			{
				sprintf(BufErr, "%s", err);
				strcat(BufErr, Buf);
				perror(BufErr);
				Immagine.g[i][j] = Immagine.err;
			}
		}
	//carica la stalla
	for (i = 0; i <= MAXGIOCATORI; i++)
		for (j = 0; j < 4; j++)
		{
			sprintf(Buf2, "/s/%d/s%d.", i, j);
			sprintf(Buf, "skin/");
			strcat(Buf, infogioco.skin);
			strcat(Buf, Buf2);
			strcat(Buf, infogioco.ext);
			if ((Immagine.s[i][j] = gdk_pixbuf_new_from_file_at_size(Buf,
					Dim_casella, Dim_casella, NULL)) == NULL)
			{
				sprintf(BufErr, "%s", err);
				strcat(BufErr, Buf);
				perror(BufErr);
				Immagine.s[i][j] = Immagine.err;
			}
		}
	//carica la fattoria
	for (i = 0; i <= MAXGIOCATORI; i++)
		for (j = 0; j < 4; j++)
		{
			sprintf(Buf2, "/f/%d/f%d.", i, j);
			sprintf(Buf, "skin/");
			strcat(Buf, infogioco.skin);
			strcat(Buf, Buf2);
			strcat(Buf, infogioco.ext);
			if ((Immagine.f[i][j] = gdk_pixbuf_new_from_file_at_size(Buf,
					Dim_casella, Dim_casella, NULL)) == NULL)
			{
				sprintf(BufErr, "%s", err);
				strcat(BufErr, Buf);
				perror(BufErr);
				Immagine.f[i][j] = Immagine.err;
			}
		}
	//carica il nido
	for (i = 0; i < MAXGIOCATORI; i++)
		for (j = 0; j < 4; j++)
		{
			sprintf(Buf2, "/n/%d/n%d.", i, j);
			sprintf(Buf, "skin/");
			strcat(Buf, infogioco.skin);
			strcat(Buf, Buf2);
			strcat(Buf, infogioco.ext);
			if ((Immagine.n[i][j] = gdk_pixbuf_new_from_file_at_size(Buf,
					Dim_casella, Dim_casella, NULL)) == NULL)
			{
				sprintf(BufErr, "%s", err);
				strcat(BufErr, Buf);
				perror(BufErr);
				Immagine.n[i][j] = Immagine.err;
			}
		}
	//carica le truppe
	for (i = 0; i < MAXGIOCATORI; i++)
		for (j = 0; j <= Fen; j++)
		{
			sprintf(Buf2, "/t/t%d%d.", i, j);
			sprintf(Buf, "skin/");
			strcat(Buf, infogioco.skin);
			strcat(Buf, Buf2);
			strcat(Buf, infogioco.ext);
			if ((Immagine.t[i][j] = gdk_pixbuf_new_from_file_at_size(Buf,
					Dim_casella, Dim_casella, NULL)) == NULL)
			{
				sprintf(BufErr, "%s", err);
				strcat(BufErr, Buf);
				perror(BufErr);
				Immagine.t[i][j] = Immagine.err;
			}
		}
}




