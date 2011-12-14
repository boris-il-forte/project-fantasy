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

#include <stdio.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "fantasy-core.h"
#include "fantasy-gtk.h"

static gboolean delete_event()
{
	gtk_main_quit();
	return TRUE;
}

static void sposta_datastiera(GtkWidget* Window, GdkEventKey* K)
{
	if(partita_in_corso==0) return;
	if(K->type == GDK_KEY_PRESS && Window!=NULL)
	{
		fprintf(stderr,"debug: sposta_datastiera\n");
		switch (K->keyval)
		{
			case GDK_KEY_Left:
				if(cx-1<0)
					return;
				else
				{
					cx-=1;
					gtk_pulisci_mappa ();
					gtk_stampa_mappa(cx,cy,'n');
				}
				break;
			case GDK_KEY_Up:
				if(cy-1<0)
					return;
				else
				{
					cy-=1;
					gtk_pulisci_mappa ();
					gtk_stampa_mappa(cx,cy,'n');
				}
				break;
			case GDK_KEY_Down:
				if(cy+1>ALTEZZA-caselle_verticali)
					return;
				else
				{
					cy+=1;
					gtk_pulisci_mappa ();
					gtk_stampa_mappa(cx,cy,'n');
				}
				break;
			case GDK_KEY_Right:
				if(cx+1>LARGHEZZA-caselle_orizzontali)
					return;
				else
				{
					cx+=1;
					gtk_pulisci_mappa ();
					gtk_stampa_mappa(cx,cy,'n');
				}
				break;
			default:
				break;
		}
	}
	return;
}

int main(int argc, char *argv[])
{
	GtkWidget *finestra;
	GtkWidget *PulsantiS;
	GtkWidget *PulsantieraS;
	GtkWidget *Vbox;
	GtkWidget *Layout;
	GtkWidget *Hbox;
	GtkWidget *Frame;
	GtkWidget *Frecce;
	
	//	inizializza
	gtk_init(&argc, &argv);
	gtk_calcola_dimensioni ();
	gtk_carica_immagini();
	gtk_inizializza_widget();
	// 	crea finestra
	finestra=gtk_window_new (GTK_WINDOW_TOPLEVEL);
	g_signal_connect (finestra, "delete-event", G_CALLBACK (delete_event), NULL);
	g_signal_connect (finestra, "key-press-event", G_CALLBACK (sposta_datastiera),NULL);
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
	// 	crea box per i pulsanti
	Frame=gtk_frame_new(NULL);
	gtk_box_pack_start(GTK_BOX(Hbox), Frame, FALSE, FALSE, 0);
	Vbox=gtk_vbox_new(FALSE,10);
	gtk_container_add (GTK_CONTAINER (Frame), Vbox);
	gtk_widget_show (Vbox);
	gtk_widget_show (Frame);
	// 	crea box e i pulsanti struttura
	PulsantiS=gtk_frame_new("Pulsanti Strutture");
	gtk_box_pack_start(GTK_BOX(Vbox), PulsantiS, FALSE, FALSE, 0);
	gtk_widget_show (PulsantiS);
	PulsantieraS=gtk_crea_pulsanti_editor_s();
	gtk_container_add (GTK_CONTAINER(PulsantiS),PulsantieraS);
	gtk_widget_show (PulsantieraS);
	// 	crea box e i pulsanti unità
	PulsantiS=gtk_frame_new("Pulsanti Unità");
	gtk_box_pack_start(GTK_BOX(Vbox), PulsantiS, FALSE, FALSE, 0);
	gtk_widget_show (PulsantiS);
	PulsantieraS=gtk_crea_pulsanti_editor_u();
	gtk_container_add (GTK_CONTAINER(PulsantiS),PulsantieraS);
	gtk_widget_show (PulsantieraS);
	// 	crea le frecce
	Frecce= gtk_crea_4_frecce();
	gtk_box_pack_start( GTK_BOX(Vbox), Frecce, FALSE, FALSE, 0);
	gtk_widget_show (Frecce);
	// 	crea mappa
	gtk_genera_mappa (Hbox);
	// 	visualizza finestra
	gtk_widget_show (finestra);
	gtk_main();
	return 0;
}
