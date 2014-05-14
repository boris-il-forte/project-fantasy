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

#include <glib/gi18n.h>

static gboolean set_adjustmentmax(GtkAdjustment* S, GtkAdjustment* D)
{
	int max = gtk_adjustment_get_value(GTK_ADJUSTMENT(S));
	int n = gtk_adjustment_get_value(GTK_ADJUSTMENT(D));

	gtk_adjustment_set_value(GTK_ADJUSTMENT(D), n < (max - 1) ? n : (max - 1));
	gtk_adjustment_set_upper(GTK_ADJUSTMENT(D), max - 1);
	return FALSE;
}

void gtk_personalizza_selezionatore(GtkWidget* Fselect)
{
	const char* Home = g_get_home_dir();
	GtkFileFilter* filter = gtk_file_filter_new();
	GtkFileFilter* filter_all = gtk_file_filter_new();
	gtk_window_set_icon(GTK_WINDOW(Fselect), Immagine.logo);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(Fselect), Home);
	gtk_file_filter_add_pattern(filter, "*fc");
	gtk_file_filter_add_pattern(filter, "*FC");
	gtk_file_filter_set_name(filter, "File FC");
	gtk_file_filter_add_pattern(filter_all, "*");
	gtk_file_filter_set_name(filter_all, "Tutti i file");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(Fselect), filter);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(Fselect), filter_all);
}

int gtk_nuova_partita()
{
	GtkWidget* Dialogo;
	GtkWidget* Opzioni;
	GtkWidget *SpinIA, *SpinGiocatori;
	GtkWidget* Label;
	GtkWidget *Hbox, *Vbox;
	GtkAdjustment *Giocatori, *IA;

	int numG;
	int numIA;

	int ok = 0;

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
		ok = 1;
	}

	gtk_widget_destroy(Dialogo);

	return ok;

}

int gtk_salva_partita()
{
	GtkWidget *Fselect;
	char* buf = NULL;

	if (partita_in_corso == 0)
		return 0;
	Fselect = gtk_file_chooser_dialog_new("Salva", NULL,
			GTK_FILE_CHOOSER_ACTION_SAVE, _("_Cancel"), GTK_RESPONSE_CANCEL,
			_("_Save"), GTK_RESPONSE_ACCEPT, NULL);
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(Fselect),
	TRUE);
	gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(Fselect), ".fc");

	gtk_personalizza_selezionatore(Fselect);

	if (gtk_dialog_run(GTK_DIALOG(Fselect)) == GTK_RESPONSE_ACCEPT)
	{
		buf = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(Fselect));

		salva(buf);
	}

	gtk_widget_destroy(Fselect);

	return 1;

}

int gtk_carica_partita()
{
	GtkWidget *Fselect;
	GtkWidget *Dialogo;
	GtkWidget *Label;

	char* buf = NULL;
	int ok = 0;

	Fselect = gtk_file_chooser_dialog_new("Carica", NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN, _("_Cancel"), GTK_RESPONSE_CANCEL,
			_("_Open") , GTK_RESPONSE_ACCEPT, NULL);

	gtk_personalizza_selezionatore(Fselect);

	while (gtk_dialog_run(GTK_DIALOG(Fselect)) == GTK_RESPONSE_ACCEPT)
	{
		buf = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(Fselect));
		if (carica(buf) != 0)
		{
			Dialogo = gtk_dialog_new();
			gtk_window_set_modal(GTK_WINDOW(Dialogo), TRUE);
			gtk_dialog_add_buttons(GTK_DIALOG(Dialogo), _("_Ok"), 1,
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
			ok = 1;
			break;
		}

	}

	gtk_widget_destroy(Fselect);

	return ok;
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
