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

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fantasy-core.h"
#include "fantasy-gtk.h"
#include "fantasy-IA.h"

// callbacks
static void menuitem_response()
{
	printf("bottone schiacciato\n");
}

static gboolean set_adjustmentmax(GtkAdjustment* S, GtkAdjustment* D)
{
	int max = gtk_adjustment_get_value(GTK_ADJUSTMENT(S));
	int n = gtk_adjustment_get_value(GTK_ADJUSTMENT(D));

	gtk_adjustment_set_value(GTK_ADJUSTMENT(D), n < (max - 1) ? n : (max - 1));
	gtk_adjustment_set_upper(GTK_ADJUSTMENT(D), max - 1);
	return FALSE;
}

static void chiudi_da_menu()
{
	GtkWidget * Dialogo;
	GtkWidget * Domanda;

	Domanda = gtk_label_new("Vuoi veramente uscire?");
	Dialogo = gtk_dialog_new_with_buttons("Fantasy C", NULL,
			GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL, "Si",
			GTK_RESPONSE_YES, "No", GTK_RESPONSE_NO, NULL);
	gtk_box_pack_start(
	GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))), Domanda,
	TRUE, TRUE, 0);
	gtk_widget_show(Domanda);
	gtk_window_set_icon(GTK_WINDOW(Dialogo), Immagine.logo);
	if (gtk_dialog_run(GTK_DIALOG(Dialogo)) == GTK_RESPONSE_YES)
		gtk_main_quit();
	else
		gtk_widget_destroy(Dialogo);
}

static void salva_carica(int Data)
{
	GtkWidget *Fselect;
	GtkWidget *Dialogo;
	GtkWidget *Label;
	GtkFileFilter *filter = gtk_file_filter_new();
	GtkFileFilter *filter_all = gtk_file_filter_new();
	char* buf = NULL;
	const char* Home = g_get_home_dir();

	if (Data == 0)
	{
		Fselect = gtk_file_chooser_dialog_new("Carica", NULL,
				GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL,
				GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
	}
	else
	{
		if (partita_in_corso == 0)
			return;
		Fselect = gtk_file_chooser_dialog_new("Salva", NULL,
				GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL,
				GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT, NULL);
		gtk_file_chooser_set_do_overwrite_confirmation(
		GTK_FILE_CHOOSER(Fselect), TRUE);
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(Fselect), ".fc");
	}
	gtk_window_set_icon(GTK_WINDOW(Fselect), Immagine.logo);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(Fselect), Home);
	gtk_file_filter_add_pattern(filter, "*fc");
	gtk_file_filter_add_pattern(filter, "*FC");
	gtk_file_filter_set_name(filter, "File FC");
	gtk_file_filter_add_pattern(filter_all, "*");
	gtk_file_filter_set_name(filter_all, "Tutti i file");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(Fselect), filter);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(Fselect), filter_all);
	while (gtk_dialog_run(GTK_DIALOG(Fselect)) == GTK_RESPONSE_ACCEPT)
	{
		buf = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(Fselect));
		if (Data == 0)
		{
			if (carica(buf) != 0)
			{
				Dialogo = gtk_dialog_new();
				gtk_window_set_modal(GTK_WINDOW(Dialogo), TRUE);
				gtk_dialog_add_buttons(GTK_DIALOG(Dialogo), GTK_STOCK_OK, 1,
				NULL);
				gtk_window_set_icon(GTK_WINDOW(Dialogo), Immagine.logo);
				gtk_window_set_title(GTK_WINDOW(Dialogo), "Errore!");
				Label = gtk_label_new("File non valido!");
				gtk_box_pack_start(
				GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),
						Label, TRUE, TRUE, 0);
				gtk_widget_show(Label);
				gtk_dialog_run(GTK_DIALOG(Dialogo));
				gtk_widget_destroy(Dialogo);
			}
			else
			{
				gtk_azzera_tab();
				gtk_pulisci_mappa();
				gtk_aggiorna_coordinate(cx, cy);
				gtk_stampa_mappa(cx, cy, 'n');
				gtk_aggiorna_contarisorse();
				gtk_aggiorna_giocatore_c();
				gtk_aggiorna_tab();
				partita_in_corso = 1;
				break;
			}
		}
		else
		{
			salva(buf);
			break;
		}
	}
	gtk_widget_destroy(Fselect);
}

static void nuova_partita()
{
	GtkWidget* Dialogo;
	GtkWidget* Opzioni;
	GtkWidget *SpinIA, *SpinGiocatori;
	GtkWidget* Label;
	GtkWidget *Hbox, *Vbox;
	GtkAdjustment *Giocatori, *IA;

	int numG;
	int numIA;

	fprintf(stderr, "debug nuova_partita\n");
	Giocatori = (GtkAdjustment *) gtk_adjustment_new(4, 2, MAXGIOCATORI, 1, 2,
			0);
	IA = (GtkAdjustment *) gtk_adjustment_new(3, 0, MAXGIOCATORI - 1, 1, 1, 0);
	g_signal_connect(Giocatori, "value_changed", G_CALLBACK(set_adjustmentmax),
			IA);
	Dialogo = gtk_dialog_new();
	gtk_window_set_modal(GTK_WINDOW(Dialogo), TRUE);
	gtk_dialog_add_buttons(GTK_DIALOG(Dialogo), "Inizia!", 1, "Annulla", 0,
	NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(Dialogo), 1);
	gtk_window_set_icon(GTK_WINDOW(Dialogo), Immagine.logo);
	gtk_window_set_title(GTK_WINDOW(Dialogo), "Fantasy C");
	Opzioni = gtk_frame_new("Opzioni Nuova Partita");
	gtk_box_pack_start(
	GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))), Opzioni,
	TRUE, TRUE, 0);
	Vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(Opzioni), Vbox);
	Hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start(GTK_BOX(Vbox), Hbox, TRUE, TRUE, 0);
	Label = gtk_label_new("Giocatori");
	gtk_box_pack_start(GTK_BOX(Hbox), Label, TRUE, TRUE, 0);
	SpinGiocatori = gtk_spin_button_new(GTK_ADJUSTMENT(Giocatori), 0, 0);
	gtk_box_pack_start(GTK_BOX(Hbox), SpinGiocatori, TRUE, TRUE, 0);
	Hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start(GTK_BOX(Vbox), Hbox, TRUE, TRUE, 0);
	Label = gtk_label_new("Gestiti dall'IA");
	gtk_box_pack_start(GTK_BOX(Hbox), Label, TRUE, TRUE, 0);
	SpinIA = gtk_spin_button_new(GTK_ADJUSTMENT(IA), 0, 0);
	gtk_box_pack_start(GTK_BOX(Hbox), SpinIA, TRUE, TRUE, 0);
	gtk_widget_show_all(Opzioni);
	if (gtk_dialog_run(GTK_DIALOG(Dialogo)) == 1)
	{
		cx = 0;
		cy = 0;
		caricadati();
		inizializza();
		generamappa();
		visualizza_su_terminale();
		numG = gtk_adjustment_get_value(GTK_ADJUSTMENT(Giocatori));
		numIA = gtk_adjustment_get_value(GTK_ADJUSTMENT(IA));
		creagiocatori(numG, numIA);
		CurrentPlayer = 0;
		gtk_azzera_tab();
		gtk_pulisci_mappa();
		gtk_aggiorna_coordinate(cx, cy);
		gtk_stampa_mappa(cx, cy, 'n');
		gtk_aggiorna_contarisorse();
		gtk_aggiorna_giocatore_c();
		gtk_aggiorna_tab();
		partita_in_corso = 1;
		gtk_widget_destroy(Dialogo);
	}
	else
		gtk_widget_destroy(Dialogo);
}

static void preferenze()
{
	int scelta;
	int subdirs_no, i, d = 0;
	char *sottodir[64]; // FIXME magic
	char *skindefault = "Il_Ratman";
	GtkWidget *Dialogo;
	GtkWidget *Opzioni;
	GtkWidget *Label;
	GtkWidget *Radio[64];
	GtkWidget *Vbox;

	Radio[0] = NULL;
	fprintf(stderr, "debug preferenze\n");
	subdirs_no = listaskin("skin", sottodir);
	Dialogo = gtk_dialog_new();
	gtk_window_set_modal(GTK_WINDOW(Dialogo), TRUE);
	gtk_dialog_add_buttons(GTK_DIALOG(Dialogo), "Salva", 1, "Ripristina", 2,
			"Annulla", 0, NULL);
	gtk_window_set_icon(GTK_WINDOW(Dialogo), Immagine.logo);
	gtk_window_set_title(GTK_WINDOW(Dialogo), "Fantasy C Config");
	Opzioni = gtk_frame_new("Opzioni Skins");
	gtk_box_pack_start(
	GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))), Opzioni,
	TRUE, TRUE, 0);
	Vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(Opzioni), Vbox);
	Label = gtk_label_new("Skin");
	gtk_box_pack_start(GTK_BOX(Vbox), Label, TRUE, TRUE, 0);
	for (i = 0; i < subdirs_no; i++)
	{
		Radio[i] = gtk_radio_button_new_with_label_from_widget(
		GTK_RADIO_BUTTON(Radio[0]), sottodir[i]); //una modifica
		gtk_box_pack_start(GTK_BOX(Vbox), Radio[i], TRUE, TRUE, 0); //due modifiche
		if (strcmp(sottodir[i], infogioco.skin) == 0)
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Radio[i]), TRUE);
		if (strcmp(sottodir[i], skindefault) == 0)
			d = i;
	}
	gtk_widget_show_all(Opzioni);
	do
	{
		scelta = gtk_dialog_run(GTK_DIALOG(Dialogo));
		if (scelta == 1) // salva
		{
			for (i = 0;
					gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Radio[i]))
							!= TRUE; i++)
				;
			sprintf(infogioco.skin, "%s", sottodir[i]);
			salvaconfig("fantasy.config");
			gtk_carica_immagini();

			if (partita_in_corso != 0)
			{
				gtk_pulisci_mappa();
				gtk_stampa_mappa(cx, cy, 'n');
			}
		}
		else if (scelta == 2) // reset
		{
			if (remove("fantasy.config"))
			{
				perror("cannot remove config file");
				exit(1);
			}
			caricaconfig("fantasy.config");
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(Radio[d]), TRUE);
		}
	} while (scelta == 2);
	gtk_widget_destroy(Dialogo);
}


static void sposta_mappa(char* v)
{
	int V = (int) (v - infomappa.mappa);

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

//crea il menu principale
void gtk_crea_menu(GtkWidget *Vbox)
{
	GtkWidget *menu;
	GtkWidget *oggetto_menu;
	GtkWidget *barra_menu;
	GtkWidget *menu_file, *menu_modifica, *menu_help;

	//genera menu file
	menu = gtk_menu_new();
	// 	genera scelta 1
	oggetto_menu = gtk_menu_item_new_with_mnemonic("_Nuova Partita");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), oggetto_menu);
	g_signal_connect(oggetto_menu, "activate", G_CALLBACK(nuova_partita),
			(gpointer)NULL);
	gtk_widget_show(oggetto_menu);
	// 	genera scelta 2
	oggetto_menu = gtk_menu_item_new_with_mnemonic("_Carica");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), oggetto_menu);
	g_signal_connect_swapped(oggetto_menu, "activate", G_CALLBACK(salva_carica),
			(gpointer )0);
	gtk_widget_show(oggetto_menu);
	// 	genera scelta 3
	oggetto_menu = gtk_menu_item_new_with_mnemonic("_Salva");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), oggetto_menu);
	g_signal_connect_swapped(oggetto_menu, "activate", G_CALLBACK(salva_carica),
			(gpointer )1);
	gtk_widget_show(oggetto_menu);
	// 	genera scelta 4
	oggetto_menu = gtk_menu_item_new_with_mnemonic("_Esci");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), oggetto_menu);
	g_signal_connect(oggetto_menu, "activate", G_CALLBACK(chiudi_da_menu),
			(gpointer)NULL);
	gtk_widget_show(oggetto_menu);
	// 	genera etichetta file 
	menu_file = gtk_menu_item_new_with_mnemonic("_File");
	gtk_widget_show(menu_file);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_file), menu);
	//  genera menu modifica
	menu = gtk_menu_new();
	//  genera scelta 1
	oggetto_menu = gtk_menu_item_new_with_mnemonic("_Preferenze");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), oggetto_menu);
	g_signal_connect(oggetto_menu, "activate", G_CALLBACK(preferenze),
			(gpointer)NULL);
	gtk_widget_show(oggetto_menu);
	//  genera etichetta modifica
	menu_modifica = gtk_menu_item_new_with_mnemonic("_Modifica");
	gtk_widget_show(menu_modifica);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_modifica), menu);
	// 	genera menu help 
	menu = gtk_menu_new();
	// 	genera scelta 1
	oggetto_menu = gtk_menu_item_new_with_label("Manuale");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), oggetto_menu);
	g_signal_connect(oggetto_menu, "activate", G_CALLBACK(menuitem_response),
			(gpointer)NULL);
	gtk_widget_show(oggetto_menu);
	// 	genera scelta 2
	oggetto_menu = gtk_menu_item_new_with_label("Crediti");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), oggetto_menu);
	g_signal_connect(oggetto_menu, "activate", G_CALLBACK(menuitem_response),
			(gpointer)NULL);
	gtk_widget_show(oggetto_menu);
	// 	genera etichetta help
	menu_help = gtk_menu_item_new_with_mnemonic("_Aiuto");
	gtk_widget_show(menu_help);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_help), menu);
	// 	genera barra dei menu 
	barra_menu = gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(Vbox), barra_menu, FALSE, FALSE, 0);
	gtk_widget_show(barra_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(barra_menu), menu_file);
	gtk_menu_shell_append(GTK_MENU_SHELL(barra_menu), menu_modifica);
	gtk_menu_shell_append(GTK_MENU_SHELL(barra_menu), menu_help);
	// 	fine menu 
}



//carica un file di salvataggio all'avvio
int gtk_carica_avvio(char *nomefile)
{
	if (carica(nomefile))
		return 1;
	gtk_azzera_tab();
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx, cy, 'n');
	gtk_aggiorna_contarisorse();
	gtk_aggiorna_giocatore_c();
	gtk_aggiorna_tab();
	partita_in_corso = 1;

	return 0;
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
				G_CALLBACK(sposta_mappa), (gpointer )&infomappa.mappa[i]);
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

//crea pulsanti editor per inserire strutture
GtkWidget *gtk_crea_pulsanti_editor_s()
{
	GtkWidget *Pulsantiera;
	GtkWidget *Pulsante;
	int i;

	Pulsantiera = gtk_grid_new();
	for (i = 0; i < NUMSTRUTTURE; i++)
	{
		Pulsante = gtk_button_new();
		gtk_widget_set_size_request(Pulsante, Dim_casella, Dim_casella);
		g_signal_connect_swapped(Pulsante, "button_press_event",
				G_CALLBACK(menuitem_response), NULL);
		gtk_grid_attach(GTK_GRID(Pulsantiera), Pulsante, i % 2, i / 2, 1, 1);
		gtk_widget_show(Pulsante);
	}
	return Pulsantiera;
}

//crea pulsanti editor per inserire unità
GtkWidget *gtk_crea_pulsanti_editor_u()
{
	GtkWidget *Pulsantiera;
	GtkWidget *Pulsante;
	int i;

	Pulsantiera = gtk_grid_new();
	for (i = 0; i < NUMTRUPPE; i++)
	{
		Pulsante = gtk_button_new();
		gtk_widget_set_size_request(Pulsante, Dim_casella, Dim_casella);
		g_signal_connect_swapped(Pulsante, "button_press_event",
				G_CALLBACK(menuitem_response), NULL);
		gtk_grid_attach(GTK_GRID(Pulsantiera), Pulsante, i % 2, i / 2, 1, 1);
		gtk_widget_show(Pulsante);
	}
	return Pulsantiera;
}
// crea riepilogo risorse
GtkWidget *gtk_crea_contarisorse()
{
	int i;
	GtkWidget *Vbox;
	GtkWidget *Hbox[NUMRISORSE];
	GtkWidget *Label[NUMRISORSE];

	Label[0] = gtk_label_new("Oro: ");
	Label[1] = gtk_label_new("Cibo: ");
	Label[2] = gtk_label_new("Smeraldi: ");
	Vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	for (i = 0; i < NUMRISORSE; i++)
	{
		Hbox[i] = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_box_pack_start(GTK_BOX(Vbox), Hbox[i], FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(Hbox[i]), Label[i], FALSE, FALSE, 0);
		gtk_widget_set_size_request(Label[i], 2 * Dim_casella, -1);
		Counter[i] = gtk_entry_new();
		gtk_editable_set_editable(GTK_EDITABLE(Counter[i]), FALSE);
		gtk_entry_set_text(GTK_ENTRY(Counter[i]), "0");
		gtk_entry_set_width_chars(GTK_ENTRY(Counter[i]), 6);
		gtk_widget_set_size_request(Counter[i], Dim_casella, -1);
		gtk_box_pack_start(GTK_BOX(Hbox[i]), Counter[i], FALSE, FALSE, 0);
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
		while (n > 999)
		{
			n = n / 1000;
			nk++;
		}
		sprintf(Buf, "%.2f", n);
		for (j = 0; nk > 0 && j < 6; j++, nk--)
			strcat(Buf, "k");
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

//autodistrugge il popup
gboolean autodestroy_popup(GtkWidget *Popup)
{
	gtk_widget_destroy(Popup);
	return FALSE;
}

//fa comparire il popup delle perdite
void gtk_popup_combattimento(GtkWidget* Casella, int Perdite)
{
	GtkWidget *Popup;
	GtkWidget *Toplevel;
	GtkWidget *Label;
	char buf[50];
	int x, y;
	int wx, wy;

	Toplevel = gtk_widget_get_toplevel(Casella);
	gdk_window_get_root_origin(gtk_widget_get_window(Toplevel), &wx, &wy);
	gtk_widget_translate_coordinates(Casella, Toplevel, wx, wy, &x, &y);
	if (Perdite > 0)
		sprintf(buf, "<span foreground=\"red\">-%d</span>", Perdite);
	else
		sprintf(buf, "<span foreground=\"green\">%d</span>", Perdite);
	Popup = gtk_window_new(GTK_WINDOW_POPUP);
#if GTK_CHECK_VERSION(3,8,0)
	gtk_widget_set_opacity(Popup, 0.6);
#endif
	Label = gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(Label), buf);
	gtk_container_add(GTK_CONTAINER(Popup), Label);
	gtk_window_move(GTK_WINDOW(Popup), x + Dim_casella / 4, y + Dim_casella);
	gtk_widget_show_all(Popup);
	gdk_threads_add_timeout_full(G_PRIORITY_DEFAULT_IDLE, 2000,
			(GSourceFunc) (autodestroy_popup), (gpointer) Popup, NULL);
	return;
}

void gtk_proclama_vincitore(int g)
{
	char buf[100];
	GtkWidget *Dialogo;
	GtkWidget *Label;

	Dialogo = gtk_dialog_new_with_buttons("Fantasy C", NULL,
			GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL, "Nuova partita",
			1, "Carica", 2, "Esci", 3, NULL);
	gtk_window_set_icon(GTK_WINDOW(Dialogo), Immagine.logo);
	sprintf(buf, " il giocatore %d ha vinto la partita! ", g);
	Label = gtk_label_new(buf);
	gtk_widget_show(Label);
	gtk_box_pack_start(
	GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))), Label,
	TRUE, TRUE, 0);
	switch (gtk_dialog_run(GTK_DIALOG(Dialogo)))
	{
		case 1:
			gtk_widget_destroy(Dialogo);
			nuova_partita();
			break;
		case 2:
			gtk_widget_destroy(Dialogo);
			salva_carica(0);
			break;
		case 3:
			gtk_widget_destroy(Dialogo);
			gtk_main_quit();
			break;
		default:
			break;
	}
	return;
}
