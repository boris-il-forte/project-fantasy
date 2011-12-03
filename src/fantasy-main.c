/*
 * Project Fantasy, gioco di strategia a turni
 *
 *
 * Copyright (C) 2011 Davide Tateo
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
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include "fantasy-core.h"
#include "fantasy-gtk.h"

//callbacks

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
				if(cy+1>ALTEZZA-A_SCHERMO)
					return;
				else
				{
					cy+=1;
					gtk_pulisci_mappa ();
					gtk_stampa_mappa(cx,cy,'n');
				}
				break;
			case GDK_KEY_Right:
				if(cx+1>LARGHEZZA-L_SCHERMO)
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

static void click_nt ()
{
	int P;
	static t_lista_t* T=NULL;
	GtkWidget* Dialogo;
	GtkWidget* Label;
	if(partita_in_corso==0) return;
	if(T==NULL) T=giocatore[CurrentPlayer]->truppe;
	if (giocatore[CurrentPlayer]->truppe==NULL)
	{
		Dialogo=gtk_dialog_new_with_buttons("F.C.",NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_OK);
		gtk_window_set_icon (GTK_WINDOW (Dialogo),Immagine.logo);
		Label=gtk_label_new("Non ci sono unità!");
		gtk_widget_show(Label);
		gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG(Dialogo))),Label, TRUE, TRUE, 0);
		gtk_dialog_run(GTK_DIALOG(Dialogo));
		gtk_widget_destroy(Dialogo);
	}
	else
	{
		P=T->pos;
		cx=P%LARGHEZZA+1-L_SCHERMO/2;
		cy=P/LARGHEZZA+1-A_SCHERMO/2;
		if(cx<0) cx=0;
		if(cy<0) cy=0;
		if(cx>LARGHEZZA-L_SCHERMO) cx=(LARGHEZZA-L_SCHERMO);
		if(cy>ALTEZZA-A_SCHERMO) cy=(ALTEZZA-A_SCHERMO);
		gtk_pulisci_mappa ();
		gtk_stampa_mappa(cx,cy,'n');
		T=T->next;
	}
}

static void click_nc ()
{
	int P;
	static t_lista_s* S=NULL;
	if(partita_in_corso==0) return;
	if(S==NULL) S=giocatore[CurrentPlayer]->struttura[Cas];
	if (giocatore[CurrentPlayer]->struttura[Cas]==NULL)
	{
		fprintf(stderr,"c'è un bug!\n");
	}
	else
	{
		P=S->pos;
		cx=P%LARGHEZZA+1-L_SCHERMO/2;
		cy=P/LARGHEZZA+1-A_SCHERMO/2;
		if(cx<0) cx=0;
		if(cy<0) cy=0;
		if(cx>LARGHEZZA-L_SCHERMO) cx=(LARGHEZZA-L_SCHERMO);
		if(cy>ALTEZZA-A_SCHERMO) cy=(ALTEZZA-A_SCHERMO);
		gtk_pulisci_mappa ();
		gtk_stampa_mappa(cx,cy,'n');
		S=S->next;
	}
}
static void click_turno ()
{
	if(partita_in_corso==0) return;
	if(giocatore[CurrentPlayer]!=NULL)
	{
		fineturno();
		gtk_aggiorna_contarisorse();
		gtk_aggiorna_tab_castelli();
		gtk_aggiorna_tab_strutture();
		gtk_aggiorna_tab_armate();
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx,cy,'n');
	}
	return;
}

// main
int main(int argc, char *argv[])
{
	/*variabili widget*/
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
	partita_in_corso=0;
	gtk_calcola_dimensioni ();
	gtk_carica_immagini();
	gtk_inizializza_widget();
// 	crea finestra
	finestra=gtk_window_new (GTK_WINDOW_TOPLEVEL);
	g_signal_connect (finestra, "delete-event", G_CALLBACK (delete_event), NULL);
	g_signal_connect (finestra, "key-press-event", G_CALLBACK (sposta_datastiera),NULL);
	gtk_window_set_title (GTK_WINDOW (finestra), "Fantasy Core");
	gtk_window_set_icon (GTK_WINDOW (finestra),Immagine.logo);
// 	crea box principale del layout
	Layout=gtk_vbox_new(FALSE,10);
	gtk_container_add (GTK_CONTAINER (finestra), Layout);
	gtk_widget_show (Layout);
// 	crea il menu
	gtk_crea_menu (Layout);
// 	crea il box per dividere pulsanti di gioco dalla mappa
	Hbox=gtk_hbox_new(FALSE,10);
	gtk_box_pack_start(GTK_BOX(Layout), Hbox, FALSE, FALSE, 0);
	gtk_widget_show (Hbox);
// 	crea box per i pulsanti
	Frame=gtk_frame_new("Pulsanti");
	gtk_box_pack_start(GTK_BOX(Hbox), Frame, FALSE, FALSE, 0);
	Vbox=gtk_vbox_new(FALSE,10);
	gtk_container_add (GTK_CONTAINER (Frame), Vbox);
	gtk_widget_show (Vbox);
	gtk_widget_show (Frame);
// 	crea 4 pulsanti
	//crea pulsante seleziona truppa
	pulsante=gtk_button_new_with_label ("Prossima truppa");
	gtk_box_pack_start( GTK_BOX(Vbox), pulsante, FALSE, FALSE, 0);
	g_signal_connect_swapped(pulsante, "clicked", G_CALLBACK (click_nt),NULL);
	gtk_widget_show (pulsante);
	//crea pulsante seleziona castello
	pulsante=gtk_button_new_with_label ("Prossimo Castello");
	gtk_box_pack_start( GTK_BOX(Vbox), pulsante, FALSE, FALSE, 0);
	g_signal_connect_swapped(pulsante, "clicked", G_CALLBACK (click_nc),NULL);
	gtk_widget_show (pulsante);
	//crea pulsante ricerca minaccie
	pulsante=gtk_button_new_with_label ("Mostra Minacce");
	gtk_box_pack_start( GTK_BOX(Vbox), pulsante, FALSE, FALSE, 0);
	gtk_widget_show (pulsante);
	//crea pulsante attacca tutti
	pulsante=gtk_button_new_with_label ("Attaccate!");
	gtk_box_pack_start( GTK_BOX(Vbox), pulsante, FALSE, FALSE, 0);
	gtk_widget_show (pulsante);
// 	crea le frecce
	Frecce= gtk_crea_4_frecce();
	gtk_box_pack_start( GTK_BOX(Vbox), Frecce, FALSE, FALSE, 0);
	gtk_widget_show (Frecce);
// 	crea zona conteggio risorse
	Risorse=gtk_crea_contarisorse();
	gtk_box_pack_start( GTK_BOX(Vbox), Risorse, FALSE, FALSE, 0);
	gtk_widget_show (Risorse);
// 	crea pulsante fine turno
	pulsante=gtk_button_new_with_label ("Fine Turno");
	gtk_box_pack_start( GTK_BOX(Vbox), pulsante, FALSE, FALSE, 15);
	g_signal_connect_swapped(pulsante, "clicked", G_CALLBACK (click_turno),NULL);
	gtk_widget_show (pulsante);
// 	crea mappa
	gtk_genera_mappa (Hbox);
// 	crea zona riepilogo
	Frame=gtk_frame_new("Riepilogo Giocatore");
	gtk_box_pack_end(GTK_BOX(Layout), Frame, FALSE, FALSE, 0);
	gtk_widget_show (Frame);
	gtk_crea_notebook (Frame);
	gtk_widget_show(Notebook[0]);
// 	visualizza finestra
	gtk_widget_show (finestra);
	gtk_main();
	return 0;
}