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

#include "fantasy-gtk.h"

#include "fantasy-core.h"

//variabili globali
static GtkWidget *Counter[NUMRISORSE];
static GtkWidget *Coordinate;
static GtkWidget *CurrentI1;
static GtkWidget *CurrentL;
static GtkWidget *CurrentI2;

static void sposta_mappa(char* v)
{
	int V = (int) (v - ancora);

	if (partita_in_corso == 0)
		return;
	switch (V)
	{
		case 1:
			if (cx - 1 < 0)
				return;
			else
				cx -= 1;
			break;
		case 3:
			if (cy - 1 < 0)
				return;
			else
				cy -= 1;
			break;
		case 5:
			if (cy + 1 > ALTEZZA - caselle_verticali)
				return;
			else
				cy += 1;
			break;
		case 7:
			if (cx + 1 > LARGHEZZA - caselle_orizzontali)
				return;
			else
				cx += 1;
			break;
		default:
			return;
	}

	gtk_pulisci_mappa();
	gtk_aggiorna_coordinate(cx, cy);
	gtk_stampa_mappa(cx, cy, 'p');
}

// funzioni grafiche
void gtk_aggiorna_coordinate(int x, int y)
{
	char buf[10];
		sprintf(buf, "(%d|%d)", x + caselle_orizzontali / 2 - 1,
				y + caselle_verticali / 2 - 1);
		gtk_label_set_text(GTK_LABEL(Coordinate), buf);

}

//crea 4 frecce direzionali
GtkWidget *gtk_crea_4_frecce()
{
	GtkWidget *Pulsantiera;
	GtkWidget *Pulsante;
	GtkWidget *Freccia;
	int i;

	Pulsantiera = gtk_grid_new();
	for (i = 1; i < 9; i = i + 2)
	{
		Pulsante = gtk_button_new();
		gtk_widget_set_size_request(Pulsante, Dim_casella, Dim_casella);
		g_signal_connect_swapped(Pulsante, "button_press_event",
				G_CALLBACK(sposta_mappa), (gpointer) &ancora[i]);
		gtk_grid_attach(GTK_GRID(Pulsantiera), Pulsante, i / 3, i % 3, 1, 1);
		switch (i)
		{
			case 1:
				Immagine.freccia = gdk_pixbuf_rotate_simple(Immagine.freccia,
						GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE);
				break;
			case 3:
				Immagine.freccia = gdk_pixbuf_rotate_simple(Immagine.freccia,
						GDK_PIXBUF_ROTATE_CLOCKWISE);
				break;
			case 5:
				Immagine.freccia = gdk_pixbuf_rotate_simple(Immagine.freccia,
						GDK_PIXBUF_ROTATE_UPSIDEDOWN);
				break;
			case 7:
				Immagine.freccia = gdk_pixbuf_rotate_simple(Immagine.freccia,
						GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE);
				break;
			default:
				break;
		}
		Freccia = gtk_image_new_from_pixbuf(Immagine.freccia);
		gtk_container_add(GTK_CONTAINER(Pulsante), Freccia);
		gtk_widget_show(Freccia);
		gtk_widget_show(Pulsante);
	}
	Coordinate = gtk_label_new("(0|0)");
	gtk_widget_set_size_request(Coordinate, Dim_casella, Dim_casella);
	gtk_grid_attach(GTK_GRID(Pulsantiera), Coordinate, 1, 1, 1, 1);
	gtk_widget_show(Coordinate);
	return Pulsantiera;
}

// crea riepilogo risorse
GtkWidget *gtk_crea_contarisorse()
{
	int i;
	GtkWidget *Vbox;
	GtkWidget *Hbox[NUMRISORSE];
	GtkWidget *Icon[NUMRISORSE];

	Icon[0] = gtk_image_new_from_pixbuf(Immagine.oro);
	Icon[1] = gtk_image_new_from_pixbuf(Immagine.cibo);
	Icon[2] = gtk_image_new_from_pixbuf(Immagine.smeraldi);
	Vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	for (i = 0; i < NUMRISORSE; i++)
	{
		Hbox[i] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_box_pack_start(GTK_BOX(Vbox), Hbox[i], FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(Hbox[i]), Icon[i], TRUE, FALSE, 0);
		Counter[i] = gtk_entry_new();
		gtk_entry_set_width_chars (GTK_ENTRY(Counter[i]), 10);
		gtk_editable_set_editable(GTK_EDITABLE(Counter[i]), FALSE);
		gtk_entry_set_text(GTK_ENTRY(Counter[i]), "0");
		gtk_box_pack_start(GTK_BOX(Hbox[i]), Counter[i], TRUE, FALSE, 0);
		gtk_widget_show_all(Hbox[i]);
	}

	return Vbox;
}

//aggiorna i valori delle risorse
void gtk_aggiorna_contarisorse()
{
	int i, j;
	int nk;
	int R[] =
	{ giocatore[CurrentPlayer]->oro, giocatore[CurrentPlayer]->cibo,
			giocatore[CurrentPlayer]->smeraldi };
	float n;
	char Buf[12];

	for (i = 0; i < NUMRISORSE; i++)
	{
		n = (float) R[i];
		nk = 0;
		while (n > 99999)
		{
			n = n / 1000;
			nk++;
		}

		if(nk > 0)
		{
			sprintf(Buf, "%.2f", n);
			for (j = 0; nk > 0 && j < 6; j++, nk--)
				strcat(Buf, "k");
		}
		else
		{
			sprintf(Buf, "%d", R[i]);
		}
		gtk_entry_set_text(GTK_ENTRY(Counter[i]), Buf);
	}
}

//crea tag con il giocatore corrente
GtkWidget *gtk_crea_giocatore_c()
{
	GtkWidget *Table;

	Table = gtk_grid_new();
	CurrentL = gtk_label_new("Fantasy Core");
	gtk_grid_attach(GTK_GRID(Table), CurrentL, 1, 0, 1, 1);
	gtk_widget_set_size_request(CurrentL, 2 * Dim_casella, -1);
	gtk_widget_show(CurrentL);
	CurrentI1 = gtk_image_new_from_pixbuf(Immagine.decorazione);
	gtk_widget_set_size_request(CurrentI1, Dim_casella / 2, Dim_casella / 2);
	gtk_grid_attach(GTK_GRID(Table), CurrentI1, 0, 0, 1, 1);
	gtk_widget_show(CurrentI1);
	CurrentI2 = gtk_image_new_from_pixbuf(Immagine.decorazione);
	gtk_widget_set_size_request(CurrentI2, Dim_casella / 2, Dim_casella / 2);
	gtk_grid_attach(GTK_GRID(Table), CurrentI2, 2, 0, 1, 1);
	gtk_widget_show_all(CurrentI2);
	return Table;
}

//aggiorna il tag con il giocatore corrente
void gtk_aggiorna_giocatore_c()
{
	char buf[20];

	gtk_image_set_from_pixbuf(GTK_IMAGE(CurrentI1), Immagine.a[CurrentPlayer]);
	gtk_image_set_from_pixbuf(GTK_IMAGE(CurrentI2), Immagine.a[CurrentPlayer]);
	sprintf(buf, "%s", infogiocatore[CurrentPlayer]->nome);
	gtk_label_set_text(GTK_LABEL(CurrentL), buf);
}

//crea un footer
GtkWidget *gtk_crea_footer()
{
	GtkWidget *Table;
	GtkWidget *Icon;
	GtkWidget *Label;

	Table = gtk_grid_new();
	Label = gtk_label_new("Fantasy Core");
	gtk_widget_set_size_request(Label, 2 * Dim_casella, -1);
	gtk_grid_attach(GTK_GRID(Table), Label, 1, 0, 1, 1);
	gtk_widget_show(Label);
	Icon = gtk_image_new_from_pixbuf(Immagine.decorazione);
	gtk_widget_set_size_request(Icon, Dim_casella / 2, Dim_casella / 2);
	gtk_grid_attach(GTK_GRID(Table), Icon, 0, 0, 1, 1);
	gtk_widget_show(Icon);
	Icon = gtk_image_new_from_pixbuf(Immagine.decorazione);
	gtk_widget_set_size_request(Icon, Dim_casella / 2, Dim_casella / 2);
	gtk_grid_attach(GTK_GRID(Table), Icon, 2, 0, 1, 1);
	gtk_widget_show_all(Icon);
	return Table;
}

