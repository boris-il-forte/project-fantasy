/*
 * Project Fantasy, gioco di strategia a turni
 *
 *
 * Copyright (C) 2011 gionnico, Davide Tateo
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
#include <stdio.h>
#include <gtk/gtk.h>
#include "../fantasy-core.h"

int main(int argc, char *argv[])
{
	GtkWidget *finestra;
	GtkWidget *pulsante;
	GtkWidget *Vbox;
	GtkWidget *Layout;
	GtkWidget *Hbox;
	GtkWidget *Frame;
	GtkWidget *Frecce;
	GtkWidget *Risorse;
	
	//	inizializza
	gtk_init(&argc, &argv);
	// 	crea finestra
	finestra=gtk_window_new (GTK_WINDOW_TOPLEVEL);
	//g_signal_connect (finestra, "delete-event", G_CALLBACK (delete_event), NULL);
	//g_signal_connect (finestra, "key-press-event", G_CALLBACK (sposta_datastiera),NULL);
	gtk_window_set_title (GTK_WINDOW (finestra), "Fantasy Core Editor");
	gtk_window_set_icon (GTK_WINDOW (finestra),Immagine.logo);
	// 	crea box principale del layout
	Layout=gtk_vbox_new(FALSE,10);
	gtk_container_add (GTK_CONTAINER (finestra), Layout);
	gtk_widget_show (Layout);
	// 	crea il box per dividere pulsanti di gioco dalla mappa
	Hbox=gtk_hbox_new(FALSE,10);
	gtk_box_pack_start(GTK_BOX(Layout), Hbox, FALSE, FALSE, 0);
	gtk_widget_show (Hbox);
	// 	visualizza finestra
	gtk_widget_show (finestra);
	gtk_main();
	return 0;
}