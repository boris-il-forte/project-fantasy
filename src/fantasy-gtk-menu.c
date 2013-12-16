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

static void menuitem_response()
{
	printf("bottone schiacciato\n");
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
				gtk_aggiorna_coordinate(cx, cy);
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

static void salva_carica(int Data)
{
	if (Data == 0)
	{
		gtk_carica_partita();
	}
	else
	{
		gtk_salva_partita();
	}

}

static void nuova_partita()
{
	gtk_nuova_partita();
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

