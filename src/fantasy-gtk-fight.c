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


#include "fantasy-gtk-fight.h"
#include "fantasy-gtk-img.h"

//autodistrugge il popup
static gboolean autodestroy_popup(GtkWidget *Popup)
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
			gtk_nuova_partita();
			break;
		case 2:
			gtk_widget_destroy(Dialogo);
			gtk_carica_partita();
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


