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

#include <glib/gi18n.h>

char* ancora = 0;

static gboolean delete_event()
{
	gtk_main_quit();

	return TRUE;
}

static void click_nt()
{
	int P;
	static t_lista_t* T = NULL;
	GtkWidget* Dialogo;
	GtkWidget* Label;

	if (partita_in_corso == 0)
		return;
	if (T == NULL)
		T = giocatore[CurrentPlayer]->truppe;
	if (giocatore[CurrentPlayer]->truppe == NULL)
	{
		Dialogo = gtk_dialog_new_with_buttons("F.C.", NULL, GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
				_("_Ok"), GTK_RESPONSE_ACCEPT, NULL);
		gtk_window_set_icon(GTK_WINDOW(Dialogo), Immagine.logo);
		Label = gtk_label_new("Non ci sono unità!");
		gtk_widget_show(Label);
		gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))), Label, TRUE,
				TRUE, 0);
		gtk_dialog_run(GTK_DIALOG(Dialogo));
		gtk_widget_destroy(Dialogo);
	}
	else
	{
		P = T->pos;
		cx = P % LARGHEZZA + 1 - caselle_orizzontali / 2;
		cy = P / LARGHEZZA + 1 - caselle_verticali / 2;
		if (cx < 0)
			cx = 0;
		if (cy < 0)
			cy = 0;
		if (cx > LARGHEZZA - caselle_orizzontali)
			cx = (LARGHEZZA - caselle_orizzontali);
		if (cy > ALTEZZA - caselle_verticali)
			cy = (ALTEZZA - caselle_verticali);
		gtk_pulisci_mappa();
		gtk_aggiorna_coordinate(cx, cy);
		gtk_stampa_mappa(cx, cy, 'n');
		T = T->next;
	}
}

static void click_nc()
{
	int P;
	static t_lista_s* S = NULL;

	if (partita_in_corso == 0)
		return;
	if (S == NULL)
		S = giocatore[CurrentPlayer]->struttura[Cas];
	if (giocatore[CurrentPlayer]->struttura[Cas] == NULL)
	{
		fprintf(stderr, "c'è un bug!\n");
	}
	else
	{
		P = S->pos;
		cx = P % LARGHEZZA + 1 - caselle_orizzontali / 2;
		cy = P / LARGHEZZA + 1 - caselle_verticali / 2;
		if (cx < 0)
			cx = 0;
		if (cy < 0)
			cy = 0;
		if (cx > LARGHEZZA - caselle_orizzontali)
			cx = (LARGHEZZA - caselle_orizzontali);
		if (cy > ALTEZZA - caselle_verticali)
			cy = (ALTEZZA - caselle_verticali);
		gtk_pulisci_mappa();
		gtk_aggiorna_coordinate(cx, cy);
		gtk_stampa_mappa(cx, cy, 'n');
		S = S->next;
	}
}

static void click_turno()
{
	int W;

	if (partita_in_corso == 0)
		return;
	if (giocatore[CurrentPlayer] != NULL && infogiocatore[CurrentPlayer] != NULL)
	{
		if (infogiocatore[CurrentPlayer]->tipo == Locale)
		{
			fineturno();
			gtk_aggiorna_giocatore_c();
			gtk_aggiorna_contarisorse();
			gtk_aggiorna_tab();
			gtk_pulisci_mappa();
			gtk_aggiorna_coordinate(cx, cy);
			gtk_stampa_mappa(cx, cy, 'n');
			W = controllovincitore();
			if (W != 0)
			{
				gtk_proclama_vincitore(W);
			}
		}
		else
		{
			printf("debug: frocio chi legge! non provare a fregare l'IA...\n");
		}
	}

	return;
}

GtkWidget *gtk_crea_finestra_principale()
{
	GtkWidget *finestra;
	GtkWidget *pulsante;
	GtkWidget *Vbox;
	GtkWidget *Layout;
	GtkWidget *Hboxmain, *Hbox;
	GtkWidget *Frame;
	GtkWidget *Frecce;
	GtkWidget *Risorse;
	GtkWidget *Tag;
	GtkWidget *Mappa;
	GtkWidget *Notebook;

	//inizializza dati
	gtk_calcola_dimensioni();
	gtk_carica_immagini();
	gtk_inizializza_tab();

	// 	crea finestra
	finestra = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(finestra, "delete-event", G_CALLBACK(delete_event), NULL);
	gtk_window_set_title(GTK_WINDOW(finestra), "Fantasy Core");
	gtk_window_set_icon(GTK_WINDOW(finestra), Immagine.logo);
	// 	crea box principale del layout
	Layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_container_add(GTK_CONTAINER(finestra), Layout);
	gtk_widget_show(Layout);
	// 	crea il menu
	gtk_crea_menu(Layout);
	// 	crea il box per dividere pulsanti di gioco dalla mappa
	Hboxmain = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_pack_start(GTK_BOX(Layout), Hboxmain, FALSE, FALSE, 0);
	gtk_widget_show(Hboxmain);
	// 	crea box per i pulsanti
	Frame = gtk_frame_new("Pulsanti");
	gtk_box_pack_start(GTK_BOX(Hboxmain), Frame, FALSE, FALSE, 0);
	gtk_widget_show(Frame);
	Vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_add(GTK_CONTAINER(Frame), Vbox);
	//gtk_widget_show(Vbox); // *ero qui
	// crea box orizzontale per 4 pulsanti fast switch
	Hbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(GTK_BOX(Vbox), Hbox, FALSE, FALSE, 5);
	gtk_widget_show(Hbox);
	// 	crea 4 pulsanti
	//crea pulsante seleziona castello
	pulsante = gtk_button_new_with_label("Prossimo Castello"); // Prossimo Castello
	gtk_box_pack_start(GTK_BOX(Hbox), pulsante, FALSE, FALSE, 2);
	gtk_widget_set_size_request(pulsante, 3 * Dim_casella, -1);
	g_signal_connect_swapped(pulsante, "clicked", G_CALLBACK(click_nc), NULL);
	gtk_widget_show(pulsante);
	//crea pulsante seleziona truppa
	pulsante = gtk_button_new_with_label("Prossima Truppa"); // Prossima Truppa
	gtk_box_pack_start(GTK_BOX(Hbox), pulsante, FALSE, FALSE, 2);
	gtk_widget_set_size_request(pulsante, 3 * Dim_casella, -1);
	g_signal_connect_swapped(pulsante, "clicked", G_CALLBACK(click_nt), NULL);
	gtk_widget_show(pulsante);
	//crea pulsante ricerca minaccie
	pulsante = gtk_button_new_with_label("Mostra Minacce"); // Mostra Minacce
	gtk_box_pack_start(GTK_BOX(Hbox), pulsante, FALSE, FALSE, 2);
	gtk_widget_set_size_request(pulsante, 3 * Dim_casella, -1);
	gtk_widget_show(pulsante);
	// 	crea le frecce
	Frecce = gtk_crea_4_frecce();
	gtk_box_pack_start(GTK_BOX(Vbox), Frecce, FALSE, FALSE, 0);
	gtk_widget_show(Frecce);
	//crea etichetta giocatore attuale
	Tag = gtk_crea_giocatore_c();
	gtk_box_pack_start(GTK_BOX(Vbox), Tag, FALSE, FALSE, 0);
	gtk_widget_show(Tag);
	// 	crea zona conteggio risorse
	Risorse = gtk_crea_contarisorse();
	gtk_box_pack_start(GTK_BOX(Vbox), Risorse, FALSE, FALSE, 0);
	gtk_widget_show(Risorse);
	// 	crea pulsante fine turno
	pulsante = gtk_button_new_with_label("Fine Turno");
	gtk_box_pack_start(GTK_BOX(Vbox), pulsante, FALSE, FALSE, 10);
	gtk_widget_set_size_request(pulsante, 3 * Dim_casella, -1);
	g_signal_connect_swapped(pulsante, "clicked", G_CALLBACK(click_turno), NULL);
	gtk_widget_show(pulsante);
	//crea footer
	Tag = gtk_crea_footer();
	gtk_box_pack_end(GTK_BOX(Vbox), Tag, FALSE, FALSE, 5);
	gtk_widget_show(Tag);
	// dividi spazio mappa in verticale
	gtk_widget_show(Vbox); // *ora qui
	Vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_box_pack_start(GTK_BOX(Hboxmain), Vbox, FALSE, FALSE, 5);
	gtk_widget_show(Vbox);
	//crea mappa
	Mappa = gtk_grid_new();
	gtk_box_pack_start(GTK_BOX(Vbox), Mappa, FALSE, FALSE, 0);
	gtk_genera_mappa(Mappa);
	gtk_widget_show(Mappa);
	//crea zona riepilogo
	Frame = gtk_frame_new("Riepilogo Giocatore");
	gtk_box_pack_end(GTK_BOX(Vbox), Frame, FALSE, FALSE, 0);
	gtk_widget_show(Frame);
	Notebook = gtk_crea_notebook(Frame);
	gtk_widget_show(Notebook);

	return finestra;

}
