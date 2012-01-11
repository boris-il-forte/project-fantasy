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
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include "fantasy-core.h"
#include "fantasy-gtk.h"
#define MAXCODAKEY 5

struct datatime_s
{
	GtkWidget *finestra;
	GtkWidget *Mappa;
	int wp;
	int hp;
};

//implementazione di una coda di pressione di chiavi
struct keypress_s
{
	int first;
	int last;
	unsigned int keyval[20];
};

int need_resize=0;

//callbacks

static gboolean delete_event()
{
	gtk_main_quit();

	return TRUE;
}

static gboolean rid()
{
	need_resize=1;
	fprintf(stderr,"rid: debug!\n");
	return FALSE;
}

static void input_tastiera(GtkWidget* Window, GdkEventKey* K, struct keypress_s *keypress)
{
	int F;
	int L;
	if(partita_in_corso==0) return;
	F=keypress->first;
	L=keypress->last;
	if(F==((L+1<MAXCODAKEY)?(L+1):(0))) return;
	if(K->type == GDK_KEY_PRESS && Window!=NULL)
	{
		keypress->keyval[L]=K->keyval;
		L++;
		keypress->last=(L<MAXCODAKEY)?(L):(0);
	}

	return;
}

static void click_nt()
{
	int P;
	static t_lista_t* T=NULL;
	GtkWidget* Dialogo;
	GtkWidget* Label;

	if(partita_in_corso==0) return;
	if(T==NULL)T=giocatore[CurrentPlayer]->truppe;
	if(giocatore[CurrentPlayer]->truppe==NULL)
	{
		Dialogo=gtk_dialog_new_with_buttons("F.C.",NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_OK,NULL);
		gtk_window_set_icon(GTK_WINDOW(Dialogo),Immagine.logo);
		Label=gtk_label_new("Non ci sono unità!");
		gtk_widget_show(Label);
		gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),Label, TRUE, TRUE, 0);
		gtk_dialog_run(GTK_DIALOG(Dialogo));
		gtk_widget_destroy(Dialogo);
	}
	else
	{
		P=T->pos;
		cx=P%LARGHEZZA+1-caselle_orizzontali/2;
		cy=P/LARGHEZZA+1-caselle_verticali/2;
		if(cx<0) cx=0;
		if(cy<0) cy=0;
		if(cx>LARGHEZZA-caselle_orizzontali) cx=(LARGHEZZA-caselle_orizzontali);
		if(cy>ALTEZZA-caselle_verticali) cy=(ALTEZZA-caselle_verticali);
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx,cy,'n');
		T=T->next;
	}
}

static void click_nc()
{
	int P;
	static t_lista_s* S=NULL;

	if(partita_in_corso==0) return;
	if(S==NULL)S=giocatore[CurrentPlayer]->struttura[Cas];
	if(giocatore[CurrentPlayer]->struttura[Cas]==NULL)
	{
		fprintf(stderr,"c'è un bug!\n");
	}
	else
	{
		P=S->pos;
		cx=P%LARGHEZZA+1-caselle_orizzontali/2;
		cy=P/LARGHEZZA+1-caselle_verticali/2;
		if(cx<0) cx=0;
		if(cy<0) cy=0;
		if(cx>LARGHEZZA-caselle_orizzontali) cx=(LARGHEZZA-caselle_orizzontali);
		if(cy>ALTEZZA-caselle_verticali) cy=(ALTEZZA-caselle_verticali);
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx,cy,'n');
		S=S->next;
	}
}

static void click_turno()
{
	if(partita_in_corso==0) return;
	if(giocatore[CurrentPlayer]!=NULL)
	{
		fineturno();
		fprintf(stderr,"debug: 1\n");
		gtk_aggiorna_giocatore_c();
		fprintf(stderr,"debug: 2\n");
		gtk_aggiorna_contarisorse();
		fprintf(stderr,"debug: 3\n");
		gtk_aggiorna_tab_castelli();
		fprintf(stderr,"debug: 4\n");
		gtk_aggiorna_tab_strutture();
		fprintf(stderr,"debug: 5\n");
		gtk_aggiorna_tab_armate();
		fprintf(stderr,"debug: 6\n");
		gtk_pulisci_mappa();
		fprintf(stderr,"debug: 7\n");
		gtk_stampa_mappa(cx,cy,'n');
		fprintf(stderr,"debug: 8\n");
	}

	return;
}

//thread routine

gboolean ridimensiona_mappa(struct datatime_s *datatime)
{
	static int i=0;
	int w,h;
	if(i==0)
	{
		fprintf(stderr,"ridimensiona_mappa: debug! inizializza...\n");
		gtk_window_get_size(GTK_WINDOW(datatime->finestra),&datatime->wp ,&datatime->hp);
		i++;
		return TRUE;
	}
	if(need_resize)
	{
		fprintf(stderr,"ridimensiona_mappa: debug! INIZIATA!\n");
		gtk_window_get_size(GTK_WINDOW(datatime->finestra),&w,&h);
		fprintf(stderr,"ridimensiona_mappa: debug! w=%d, h=%d, wp=%d, hp=%d\n", w,h,datatime->wp,datatime->hp);
		if(w!=datatime->wp || h!=datatime->hp)
		{
			gtk_pulisci_caselle();
			caselle_orizzontali+=(w-datatime->wp)/Dim_casella;
			caselle_verticali+=(h-datatime->hp)/Dim_casella;
			gtk_table_resize(GTK_TABLE(datatime->Mappa),caselle_orizzontali,caselle_verticali);
			gtk_genera_mappa(datatime->Mappa);
			if(partita_in_corso!=0)
			{
				gtk_stampa_mappa(cx,cy,'n');
			}
			datatime->wp=w;
			datatime->hp=h;
		}
		fprintf(stderr,"ridimensiona_mappa: debug! FINITA!\n");
		need_resize=0;
	}
	
	return TRUE;
}

gboolean leggistream_tastiera (struct keypress_s *keypress)
{
	int F=keypress->first;
	int L=keypress->last;
	if(F==L) return TRUE;
	switch(keypress->keyval[F])
	{
		case GDK_KEY_Left:
			if(cx-1>=0)
			{
				cx-=1;
				gtk_pulisci_mappa();
				gtk_stampa_mappa(cx,cy,'p');
			}
			break;
		case GDK_KEY_Up:
			if(cy-1>=0)
			{
				cy-=1;
				gtk_pulisci_mappa();
				gtk_stampa_mappa(cx,cy,'p');
			}
			break;
		case GDK_KEY_Down:
			if(cy+1<=ALTEZZA-caselle_verticali)
			{
				cy+=1;
				gtk_pulisci_mappa();
				gtk_stampa_mappa(cx,cy,'p');
			}
			break;
		case GDK_KEY_Right:
			if(cx+1<=LARGHEZZA-caselle_orizzontali)
			{
				cx+=1;
				gtk_pulisci_mappa();
				gtk_stampa_mappa(cx,cy,'p');
			}
			break;
		case GDK_KEY_Escape:
			gtk_pulisci_mappa();
			gtk_stampa_mappa(cx,cy,'n');
			break;
		default:
			break;
	}
	F++;
	keypress->first=(F<MAXCODAKEY)?(F):(0);
	return TRUE;
}

// main
int main(int argc, char *argv[])
{
	/*variabili widget*/
	GtkWidget *finestra;
	GtkWidget *pulsante;
	GtkWidget *Vbox;
	GtkWidget *Layout;
	GtkWidget *Hboxmain,*Hbox;
	GtkWidget *Frame;
	GtkWidget *Frecce;
	GtkWidget *Risorse;
	GtkWidget *Tag;
	GtkWidget *Mappa;

	struct datatime_s datatime;
	struct keypress_s keypress;


//	inizializza
	gtk_init(&argc, &argv);
	partita_in_corso=0;
	caricaconfig("fantasy.config");
	#ifdef DEBUG
	printf("infogioco.skin=%s\n",infogioco.skin);
	printf("infogioco.ext=%s\n",infogioco.ext);
	#endif
	gtk_calcola_dimensioni();
	gtk_carica_immagini();
	gtk_inizializza_widget();
// 	crea finestra
	finestra=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(finestra, "delete-event", G_CALLBACK(delete_event), NULL);
	gtk_window_set_title(GTK_WINDOW(finestra), "Fantasy Core");
	gtk_window_set_icon(GTK_WINDOW(finestra),Immagine.logo);
// 	crea box principale del layout
	Layout=gtk_vbox_new(FALSE,5);
	gtk_container_add(GTK_CONTAINER(finestra), Layout);
	gtk_widget_show(Layout);
// 	crea il menu
	gtk_crea_menu(Layout);
// 	crea il box per dividere pulsanti di gioco dalla mappa
	Hboxmain=gtk_hbox_new(FALSE,10);
	gtk_box_pack_start(GTK_BOX(Layout), Hboxmain, FALSE, FALSE, 0);
	gtk_widget_show(Hboxmain);
// 	crea box per i pulsanti
	Frame=gtk_frame_new("Pulsanti");
	gtk_box_pack_start(GTK_BOX(Hboxmain), Frame, FALSE, FALSE, 0);
	gtk_widget_show(Frame);
	Vbox=gtk_vbox_new(FALSE,10);
	gtk_container_add(GTK_CONTAINER(Frame), Vbox);
	//gtk_widget_show(Vbox); // *ero qui
	// crea box orizzontale per 4 pulsanti fast switch
	Hbox=gtk_vbox_new(FALSE,0);
	gtk_box_pack_start(GTK_BOX(Vbox), Hbox, FALSE, FALSE, 5);
	gtk_widget_show(Hbox);
// 	crea 4 pulsanti
	//crea pulsante seleziona truppa
	pulsante=gtk_button_new_with_label("Prossima Truppa"); // Prossima Truppa
	gtk_box_pack_start(GTK_BOX(Hbox), pulsante, FALSE, FALSE, 2);
	gtk_widget_set_size_request(pulsante,3*Dim_casella,-1);
	g_signal_connect_swapped(pulsante, "clicked", G_CALLBACK(click_nt),NULL);
	gtk_widget_show(pulsante);
	//crea pulsante seleziona castello
	pulsante=gtk_button_new_with_label("prossimo castello"); // Prossimo Castello
	gtk_box_pack_start(GTK_BOX(Hbox), pulsante, FALSE, FALSE, 2);
	gtk_widget_set_size_request(pulsante,3*Dim_casella,-1);
	g_signal_connect_swapped(pulsante, "clicked", G_CALLBACK(click_nc),NULL);
	gtk_widget_show(pulsante);
	//crea pulsante ricerca minaccie
	pulsante=gtk_button_new_with_label("Mostra Minacce"); // Mostra Minacce
	gtk_box_pack_start(GTK_BOX(Hbox), pulsante, FALSE, FALSE, 2);
	gtk_widget_set_size_request(pulsante,3*Dim_casella,-1);
	gtk_widget_show(pulsante);
// 	crea le frecce
	Frecce= gtk_crea_4_frecce();
	gtk_box_pack_start(GTK_BOX(Vbox), Frecce, FALSE, FALSE, 0);
	gtk_widget_show(Frecce);
	//crea etichetta giocatore attuale
	Tag=gtk_crea_giocatore_c();
	gtk_box_pack_start(GTK_BOX(Vbox),Tag , FALSE, FALSE, 0);
	gtk_widget_show(Tag);
// 	crea zona conteggio risorse
	Risorse=gtk_crea_contarisorse();
	gtk_box_pack_start(GTK_BOX(Vbox), Risorse, FALSE, FALSE, 0);
	gtk_widget_show(Risorse);
// 	crea pulsante fine turno
	pulsante=gtk_button_new_with_label("Fine Turno");
	gtk_box_pack_start(GTK_BOX(Vbox), pulsante, FALSE, FALSE, 10);
	gtk_widget_set_size_request(pulsante,3*Dim_casella,-1);
	g_signal_connect_swapped(pulsante, "clicked", G_CALLBACK(click_turno),NULL);
	gtk_widget_show(pulsante);
	//crea footer
	Tag=gtk_crea_footer();
	gtk_box_pack_end(GTK_BOX(Vbox), Tag, FALSE, FALSE, 5);
	gtk_widget_show(Tag);
// dividi spazio mappa in verticale
	gtk_widget_show(Vbox); // *ora qui
	Vbox=gtk_vbox_new(FALSE,10);
	gtk_box_pack_start(GTK_BOX(Hboxmain), Vbox, FALSE, FALSE, 5);
	gtk_widget_show(Vbox);
	//crea mappa
	Mappa=gtk_table_new(caselle_orizzontali, caselle_verticali, FALSE);
	gtk_box_pack_start(GTK_BOX(Vbox), Mappa, FALSE, FALSE, 0);
	gtk_genera_mappa(Mappa);
	gtk_widget_show(Mappa);
	//crea zona riepilogo
	Frame=gtk_frame_new("Riepilogo Giocatore");
	gtk_box_pack_end(GTK_BOX(Vbox), Frame, FALSE, FALSE, 0);
	gtk_widget_show(Frame);
	gtk_crea_notebook(Frame);
	gtk_widget_show(Notebook[0]);
// carica_all_avvio
	if(argc > 1)
		if(gtk_carica_avvio(argv[argc-1])) return 1;
// 	visualizza finestra e crea le due routine del main loop
	gtk_widget_show(finestra);
	datatime.finestra=finestra;
	datatime.Mappa=Mappa;
	keypress.first=0;
	keypress.last=0;
	g_signal_connect(finestra,"key-press-event", G_CALLBACK(input_tastiera),(gpointer)&keypress);
	g_signal_connect(finestra,"configure-event", G_CALLBACK(rid),NULL);
	gdk_threads_add_timeout_full(G_PRIORITY_DEFAULT_IDLE,10,(GSourceFunc) (ridimensiona_mappa),(gpointer) &datatime,NULL);
	gdk_threads_add_timeout_full(G_PRIORITY_DEFAULT_IDLE,50,(GSourceFunc) (leggistream_tastiera),(gpointer) &keypress,NULL);
	gtk_main();

	return 0;
}
