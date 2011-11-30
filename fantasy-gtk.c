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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fantasy-core.h"
#include "fantasy-gtk.h"

// Callbacks
static void menuitem_response( )
{
	printf ("bottone schiacciato\n");
}

static gboolean set_adjustmentmax (GtkObject* S, GtkObject* D)
{
	int max=gtk_adjustment_get_value (GTK_ADJUSTMENT(S));
	int n=gtk_adjustment_get_value (GTK_ADJUSTMENT(D));
	gtk_adjustment_set_value (GTK_ADJUSTMENT(D), n<(max-1) ? n : (max-1));
	gtk_adjustment_set_upper (GTK_ADJUSTMENT(D), max-1);
	return FALSE;
}

static gboolean set_adjustmentvalue (GtkObject* S,t_spin* C)
{
	int n=gtk_adjustment_get_value(GTK_ADJUSTMENT(S));
	int max=C->somma;
	GtkObject* D=C->A;
	gtk_adjustment_set_value (GTK_ADJUSTMENT(D), max-n);
	return FALSE;
}

static void chiudi_da_menu()
{
	GtkWidget * Dialogo;
	GtkWidget * Domanda;
	Domanda=gtk_label_new("Vuoi veramente uscire?");
	Dialogo=gtk_dialog_new_with_buttons("Fantasy C",NULL,GTK_DIALOG_DESTROY_WITH_PARENT, "Si",GTK_RESPONSE_YES, "No",GTK_RESPONSE_NO,NULL);
	gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG(Dialogo))),Domanda, TRUE, TRUE, 0);
	gtk_widget_show(Domanda);
	gtk_window_set_icon (GTK_WINDOW (Dialogo),Immagine.logo);
	if(gtk_dialog_run(GTK_DIALOG(Dialogo))==GTK_RESPONSE_YES) 
		gtk_main_quit();
	else
		gtk_widget_destroy (Dialogo);
}

static void salva_carica(int Data)
{
	GtkWidget *Fselect;
	GtkFileFilter *filter = gtk_file_filter_new ();
	GtkFileFilter *filter_all = gtk_file_filter_new ();
	char* buf=NULL;
	const char* Home=g_get_home_dir();
	if (Data==0) 
	{
		Fselect=gtk_file_chooser_dialog_new ("Carica", NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN,GTK_RESPONSE_ACCEPT,NULL);
	} 
	else 
	{
		if(partita_in_corso==0) return;
		Fselect=gtk_file_chooser_dialog_new ("Salva", NULL,GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_SAVE,GTK_RESPONSE_ACCEPT,NULL);
		gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(Fselect),TRUE);
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER(Fselect),".fc");
	}
	gtk_window_set_icon (GTK_WINDOW (Fselect),Immagine.logo);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(Fselect),Home);
	gtk_file_filter_add_pattern(filter, "*fc");
	gtk_file_filter_add_pattern(filter, "*FC");
	gtk_file_filter_set_name(filter,"File FC");
	gtk_file_filter_add_pattern(filter_all, "*");
	gtk_file_filter_set_name(filter_all,"Tutti i file");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(Fselect),filter);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(Fselect),filter_all);
	if (gtk_dialog_run (GTK_DIALOG(Fselect))==GTK_RESPONSE_ACCEPT)
	{
		buf=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(Fselect));
		if(Data==0) 
		{
			caricadati();
			inizializza();
			carica(buf);
			CurrentPlayer=0;
			gtk_azzera_tab ();
			gtk_pulisci_mappa();
			gtk_stampa_mappa(cx,cy, 'n');
			gtk_aggiorna_contarisorse();
			gtk_aggiorna_tab_castelli();
			gtk_aggiorna_tab_strutture();
			gtk_aggiorna_tab_armate();
			partita_in_corso=1;
		}
		else 
		{
			salva(buf);
		}
		gtk_widget_destroy(Fselect);
	}
	else
		gtk_widget_destroy(Fselect);
}

static void nuova_partita ()
{
	GtkWidget* Dialogo;
	GtkWidget* Opzioni;
	GtkWidget *SpinIA, *SpinGiocatori;
	GtkWidget* Label;
	GtkWidget *Hbox, *Vbox;
	GtkObject *Giocatori, *IA;
	fprintf(stderr,"debug nuova_partita\n");
	Giocatori=gtk_adjustment_new(4, 2, MAXGIOCATORI, 1, 2, 0);
	IA=gtk_adjustment_new(3, 0, MAXGIOCATORI-1, 1, 1, 0);
	g_signal_connect (Giocatori, "value_changed", G_CALLBACK (set_adjustmentmax), IA);
	Dialogo=gtk_dialog_new();
	gtk_dialog_add_buttons (GTK_DIALOG(Dialogo),"Inizia!",1,"Annulla",0,NULL);
	gtk_window_set_icon (GTK_WINDOW (Dialogo),Immagine.logo);
	gtk_window_set_title (GTK_WINDOW(Dialogo),"Fantasy C");
	Opzioni=gtk_frame_new("Opzioni Nuova Partita");
	gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG(Dialogo))),Opzioni, TRUE, TRUE, 0);
	Vbox=gtk_vbox_new(TRUE,0);
	gtk_container_add (GTK_CONTAINER (Opzioni), Vbox);
	Hbox=gtk_hbox_new(TRUE,0);
	gtk_box_pack_start (GTK_BOX (Vbox),Hbox, TRUE, TRUE, 0);
	Label=gtk_label_new("Giocatori");
	gtk_box_pack_start (GTK_BOX (Hbox),Label, TRUE, TRUE, 0);
	SpinGiocatori=gtk_spin_button_new(GTK_ADJUSTMENT(Giocatori), 0, 0);
	gtk_box_pack_start (GTK_BOX (Hbox),SpinGiocatori, TRUE, TRUE,0);
	Hbox=gtk_hbox_new(TRUE,0);
	gtk_box_pack_start (GTK_BOX (Vbox),Hbox, TRUE, TRUE, 0);
	Label=gtk_label_new("Gestiti dall'IA");
	gtk_box_pack_start (GTK_BOX (Hbox),Label, TRUE, TRUE, 0);
	SpinIA=gtk_spin_button_new(GTK_ADJUSTMENT(IA), 0, 0);
	gtk_box_pack_start (GTK_BOX (Hbox),SpinIA, TRUE, TRUE, 0);
	gtk_widget_show_all (Opzioni);
	if(gtk_dialog_run(GTK_DIALOG(Dialogo))==1)
	{
		cx=0;
		cy=0;
		caricadati();
		inizializza();
		generamappa();
		visualizza_su_terminale ();
		creagiocatori (gtk_adjustment_get_value(GTK_ADJUSTMENT(Giocatori)));
		CurrentPlayer=0;
		gtk_azzera_tab ();
		gtk_pulisci_mappa ();
		gtk_stampa_mappa(cx,cy, 'n');
		gtk_aggiorna_contarisorse();
		Listacastelli[0]=gtk_riempi_tab_castelli (1, "Capitale");
		gtk_aggiorna_tab_armate();
		gtk_aggiorna_tab_strutture();
		partita_in_corso=1;
		gtk_widget_destroy (Dialogo);
	}
	else
		gtk_widget_destroy (Dialogo);
}

static void centra_mappa(char* pos)
{
	int P= (int) (pos-infomappa.mappa);
	cx=P%LARGHEZZA+1-L_SCHERMO/2;
	cy=P/LARGHEZZA+1-A_SCHERMO/2;
	if(cx<0) cx=0;
	if(cy<0) cy=0;
	if(cx>LARGHEZZA-L_SCHERMO) cx=(LARGHEZZA-L_SCHERMO);
	if(cy>ALTEZZA-A_SCHERMO) cy=(ALTEZZA-A_SCHERMO);
	gtk_pulisci_mappa ();
	gtk_stampa_mappa(cx,cy,'n');
}

static void sposta_mappa(char* v)
{
	int V=(int)(v-infomappa.mappa);
	if(partita_in_corso==0) return;
	switch (V)
	{
		case 1:
			if(cx-1<0)
				return;
			else
			{
				cx-=1;
				gtk_pulisci_mappa ();
				gtk_stampa_mappa(cx,cy,'n');
			}
			break;
		case 3:
			if(cy-1<0)
				return;
			else
			{
				cy-=1;
				gtk_pulisci_mappa ();
				gtk_stampa_mappa(cx,cy,'n');
			}
			break;
		case 5:
			if(cy+1>ALTEZZA-A_SCHERMO)
				return;
			else
			{
				cy+=1;
				gtk_pulisci_mappa ();
				gtk_stampa_mappa(cx,cy,'n');
			}
			break;
		case 7:
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

static void addestra_truppa (t_callback_s* Struct)
{
	int P=Struct->pos;
	t_truppa T=Struct->tipo;
	t_infotruppa* U;
	t_lista_s* S;
	t_lista_t* N;
	t_lista_t* Np;
	U=generatruppa (T, 0, 100);
	S=puntastruttura(P);
	if(S==NULL)
	{
		fprintf(stderr,"c'è un bug!\n");
		return;
	}
	N=S->in;
	if(N!=NULL)
	{
		do
		{
			Np=N;
			N=N->next;
		} while (N!=NULL);
		N=(t_lista_t*)malloc(sizeof(t_lista_t));
		N->truppa=U;
		N->pos=P;
		Np->next=N;
		N->next=NULL;
	}
	else
	{
		S->in=(t_lista_t*)malloc(sizeof(t_lista_t));
		S->in->truppa=U;
		S->in->pos=P;
		S->in->next=NULL;
	}
}

static void evacua_truppa (t_callback_s* Struct)
{
	int L;
	int P=Struct->pos;
	t_truppa Tipo=Struct->tipo; 
	t_lista_s* S;
	t_lista_t* T;
	t_lista_t* Tp;
	t_infotruppa** Libera;
	S=puntastruttura(P);
	fprintf(stderr,"pos:  %d", P);
	T=S->in;
	Tp=T;
	while (T!=NULL)
	{
		if(T->truppa->tipo==Tipo)
		{
			Libera=puntacasellalibera(P);
			if(Libera!=NULL)
			{
				L=(int) (Libera-infomappa.truppe);
				infomappa.truppe[L]=T->truppa;
				if(Tp!=T)
				{
					Tp->next=T->next;
					free(T);
				}
				else
				{
					Tp=T->next;
					free(T);
					S->in=Tp;
				}
				if(giocatore[CurrentPlayer]->truppe==NULL)
				{
					giocatore[CurrentPlayer]->truppe=(t_lista_t*)malloc(sizeof(t_lista_t));
					giocatore[CurrentPlayer]->truppe->truppa=infomappa.truppe[L];
					giocatore[CurrentPlayer]->truppe->pos=L;
					giocatore[CurrentPlayer]->truppe->next=NULL;
				}
				else
				{
					Tp=giocatore[CurrentPlayer]->truppe;
					T=giocatore[CurrentPlayer]->truppe->next;
					while(T!=NULL)
					{
						Tp=T;
						T=T->next;
					}
					Tp->next=(t_lista_t*)malloc(sizeof(t_lista_t));
					Tp->next->truppa=infomappa.truppe[L];
					Tp->next->pos=L;
					Tp->next->next=NULL;
				}
				gtk_aggiorna_tab_armate ();
				gtk_pulisci_mappa ();
				gtk_stampa_mappa(cx,cy,'n');
			}
			else
			{
				printf("caselle occupate!\n");
				return;
			}
			return;
		}
		else
		{
			Tp=T;
			T=T->next;
		}
	}
	return;
}

static void muovi_unita (char* pos)
{
	int Pos= (int) (pos-infomappa.mappa);
	t_infotruppa* T;
	T=infomappa.truppe[Pos];
	GtkWidget* Dialogo;
	GtkWidget* Label;
	if(T->stanca==0 && T->combattuto==0)
	{
		T->stanca=1;
		Mossa=Pos;
		gtk_pulisci_mappa ();
		gtk_stampa_mappa(cx,cy,'s');
		return;
	}
	else
	{
		Dialogo=gtk_dialog_new_with_buttons("F.C.",NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_OK);
		gtk_window_set_icon (GTK_WINDOW (Dialogo),Immagine.logo);
		if(T->stanca==1)Label=gtk_label_new("unità stanca!");
		else Label=gtk_label_new("unità in combattimento!");
		gtk_widget_show(Label);
		gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG(Dialogo))),Label, TRUE, TRUE, 0);
		gtk_dialog_run(GTK_DIALOG(Dialogo));
		gtk_widget_destroy(Dialogo);
		return;
	}
}

static void combatti_unita (char* pos)
{
	int Pos= (int) (pos-infomappa.mappa);
	t_infotruppa* T;
	T=infomappa.truppe[Pos];
	GtkWidget* Dialogo;
	GtkWidget* Label;
	if(T->combattuto==0)
	{
		T->combattuto=1;
		Mossa=Pos;
		gtk_pulisci_mappa ();
		gtk_stampa_mappa(cx,cy,'c');
		return;
	}
	else
	{
		Dialogo=gtk_dialog_new_with_buttons("F.C.",NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_OK);
		gtk_window_set_icon (GTK_WINDOW (Dialogo),Immagine.logo);
		Label=gtk_label_new("unità in combattimento!");
		gtk_widget_show(Label);
		gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG(Dialogo))),Label, TRUE, TRUE, 0);
		gtk_dialog_run(GTK_DIALOG(Dialogo));
		gtk_widget_destroy(Dialogo);
		return;
	}
}

static void click_bersaglio (char* pos)
{
	int Dst= (int) (pos-infomappa.mappa);
	int Src= Mossa;
	t_infotruppa* A=infomappa.truppe[Src];
	t_infotruppa* D=infomappa.truppe[Dst];
	if (Src==Dst)
	{
		infomappa.truppe[Src]->combattuto=0;
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx,cy,'n');
	}
	else combatti(A, D, 'n');
}

static void click_destinazione (char* pos)
{
	int Dst= (int) (pos-infomappa.mappa);
	int Src= Mossa;
	t_lista_t *T=giocatore[CurrentPlayer]->truppe;
	if (Src==Dst)
	{
		infomappa.truppe[Src]->stanca=0;
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx,cy,'n');
		return;
	}
	infomappa.truppe[Dst]=infomappa.truppe[Src];
	infomappa.truppe[Src]=NULL;
	while (T!=NULL)
	{
		if(T->pos==Src)
			T->pos=Dst;
		else
			T=T->next;
	}
	gtk_aggiorna_tab_armate ();
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx,cy,'n');
}



static void click_castello(char* pos)
{
	GtkWidget *menu;
	GtkWidget *scelta;
	GtkWidget *lista;
	GtkWidget *oggetto;
	aggiorna_tr_callback(pos);
	/*crea menu*/
	menu=gtk_menu_new();
	/*etichetta addestra*/
	scelta=gtk_menu_item_new_with_label ("Addestra:");
	/*crea pulsanti per addestrare unità*/
	lista=gtk_menu_new();
	//reclute
	oggetto=gtk_menu_item_new_with_label ("Reclute");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (addestra_truppa), (gpointer) &tr_callback[Rec]);
	gtk_widget_show (oggetto);
	//fanteria
	oggetto=gtk_menu_item_new_with_label ("Fanteria");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (addestra_truppa), (gpointer) &tr_callback[Fan]);
	gtk_widget_show (oggetto);
	//lanceri
	oggetto=gtk_menu_item_new_with_label ("Lancieri");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (addestra_truppa), (gpointer) &tr_callback[Lan]);
	gtk_widget_show (oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (scelta), lista);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), scelta);
	gtk_widget_show (scelta);
	/*etichetta evacua*/
	scelta=gtk_menu_item_new_with_label ("Evacua:");
	/*crea pulsanti per addestrare unità*/
	lista=gtk_menu_new();
	//reclute
	oggetto=gtk_menu_item_new_with_label ("Reclute");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Rec]);
	gtk_widget_show (oggetto);
	//fanteria
	oggetto=gtk_menu_item_new_with_label ("Fanteria");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Fan]);
	gtk_widget_show (oggetto);
	//lanceri
	oggetto=gtk_menu_item_new_with_label ("Lancieri");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Lan]);
	gtk_widget_show (oggetto);
	//Arcieri
	oggetto=gtk_menu_item_new_with_label ("Arcieri");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Arc]);
	gtk_widget_show (oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (scelta), lista);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), scelta);
	gtk_widget_show (scelta);
	//mostra il menu
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,1, gtk_get_current_event_time());
}

static void click_scuderia(char* pos)
{
	GtkWidget *menu;
	GtkWidget *scelta;
	GtkWidget *lista;
	GtkWidget *oggetto;
	aggiorna_tr_callback(pos);
	/*crea menu*/
	menu=gtk_menu_new();
	/*etichetta addestra*/
	scelta=gtk_menu_item_new_with_label ("Addestra:");
	/*crea pulsanti per addestrare unità*/
	lista=gtk_menu_new();
	//reclute
	oggetto=gtk_menu_item_new_with_label ("Reclute");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (addestra_truppa), (gpointer) &tr_callback[Rec]);
	gtk_widget_show (oggetto);
	//Cavalleria
	oggetto=gtk_menu_item_new_with_label ("Cavalleria");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (menuitem_response), (gpointer) NULL);
	gtk_widget_show (oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (scelta), lista);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), scelta);
	gtk_widget_show (scelta);
	/*etichetta evacua*/
	scelta=gtk_menu_item_new_with_label ("Evacua:");
	/*crea pulsanti per addestrare unità*/
	lista=gtk_menu_new();
	//reclute
	oggetto=gtk_menu_item_new_with_label ("Reclute");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Rec]);
	gtk_widget_show (oggetto);
	//fanteria
	oggetto=gtk_menu_item_new_with_label ("Fanteria");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Fan]);
	gtk_widget_show (oggetto);
	//lanceri
	oggetto=gtk_menu_item_new_with_label ("Lancieri");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Lan]);
	gtk_widget_show (oggetto);
	//Cavalleria
	oggetto=gtk_menu_item_new_with_label ("Cavalleria");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Cav]);
	gtk_widget_show (oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (scelta), lista);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), scelta);
	gtk_widget_show (scelta);
	//mostra il menu
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,1, gtk_get_current_event_time());
}

static void click_fattoria(char* pos)
{
	GtkWidget *menu;
	GtkWidget *scelta;
	GtkWidget *lista;
	GtkWidget *oggetto;
	aggiorna_tr_callback(pos);
	/*crea menu*/
	menu=gtk_menu_new();
	/*etichetta addestra*/
	scelta=gtk_menu_item_new_with_label ("Addestra:");
	/*crea pulsanti per addestrare unità*/
	lista=gtk_menu_new();
	//reclute
	oggetto=gtk_menu_item_new_with_label ("Reclute");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (addestra_truppa), (gpointer) &tr_callback[Rec]);
	gtk_widget_show (oggetto);
	//fanteria
	oggetto=gtk_menu_item_new_with_label ("Arceri");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (menuitem_response), (gpointer) NULL);
	gtk_widget_show (oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (scelta), lista);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), scelta);
	gtk_widget_show (scelta);
	/*etichetta evacua*/
	scelta=gtk_menu_item_new_with_label ("Evacua:");
	/*crea pulsanti per evacuare unità*/
	lista=gtk_menu_new();
	//reclute
	oggetto=gtk_menu_item_new_with_label ("Reclute");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Rec]);
	gtk_widget_show (oggetto);
	//fanteria
	oggetto=gtk_menu_item_new_with_label ("Fanteria");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Fan]);
	gtk_widget_show (oggetto);
	//lanceri
	oggetto=gtk_menu_item_new_with_label ("Lancieri");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Lan]);
	gtk_widget_show (oggetto);
	//Arcieri
	oggetto=gtk_menu_item_new_with_label ("Arcieri");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Arc]);
	gtk_widget_show (oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (scelta), lista);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), scelta);
	gtk_widget_show (scelta);
	//mostra il menu
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,1, gtk_get_current_event_time());
}

static void click_grotta(char* pos)
{
	GtkWidget *menu;
	GtkWidget *scelta;
	GtkWidget *lista;
	GtkWidget *oggetto;
	aggiorna_tr_callback(pos);
	/*crea menu*/
	menu=gtk_menu_new();
	/*etichetta addestra*/
	scelta=gtk_menu_item_new_with_label ("Addestra:");
	/*crea pulsanti per addestrare unità*/
	lista=gtk_menu_new();
	//Draghi
	oggetto=gtk_menu_item_new_with_label ("Draghi");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (menuitem_response), (gpointer) NULL);
	gtk_widget_show (oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (scelta), lista);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), scelta);
	gtk_widget_show (scelta);
	/*etichetta evacua*/
	scelta=gtk_menu_item_new_with_label ("Evacua:");
	/*crea pulsanti per evacuare unità*/
	lista=gtk_menu_new();
	//reclute
	oggetto=gtk_menu_item_new_with_label ("Reclute");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Rec]);
	gtk_widget_show (oggetto);
	//fanteria
	oggetto=gtk_menu_item_new_with_label ("Fanteria");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Fan]);
	gtk_widget_show (oggetto);
	//lanceri
	oggetto=gtk_menu_item_new_with_label ("Lancieri");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Lan]);
	gtk_widget_show (oggetto);
	//Arcieri
	oggetto=gtk_menu_item_new_with_label ("Arcieri");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Arc]);
	gtk_widget_show (oggetto);
	//Draghi
	oggetto=gtk_menu_item_new_with_label ("Draghi");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Dra]);
	gtk_widget_show (oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (scelta), lista);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), scelta);
	gtk_widget_show (scelta);
	//mostra il menu
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,1, gtk_get_current_event_time());
}

static void click_nido(char* pos)
{
	GtkWidget *menu;
	GtkWidget *scelta;
	GtkWidget *lista;
	GtkWidget *oggetto;
	aggiorna_tr_callback(pos);
	/*crea menu*/
	menu=gtk_menu_new();
	/*etichetta addestra*/
	scelta=gtk_menu_item_new_with_label ("Addestra:");
	/*crea pulsanti per addestrare unità*/
	lista=gtk_menu_new();
	//Fenici
	oggetto=gtk_menu_item_new_with_label ("Fenici");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (menuitem_response), (gpointer) NULL);
	gtk_widget_show (oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (scelta), lista);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), scelta);
	gtk_widget_show (scelta);
	/*etichetta evacua*/
	scelta=gtk_menu_item_new_with_label ("Evacua:");
	/*crea pulsanti per evacuare unità*/
	lista=gtk_menu_new();
	//reclute
	oggetto=gtk_menu_item_new_with_label ("Reclute");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Rec]);
	gtk_widget_show (oggetto);
	//fanteria
	oggetto=gtk_menu_item_new_with_label ("Fanteria");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Fan]);
	gtk_widget_show (oggetto);
	//lanceri
	oggetto=gtk_menu_item_new_with_label ("Lancieri");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Lan]);
	gtk_widget_show (oggetto);
	//Arceri
	oggetto=gtk_menu_item_new_with_label ("Arcieri");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Arc]);
	gtk_widget_show (oggetto);
	//Fenici
	oggetto=gtk_menu_item_new_with_label ("Fenici");
	gtk_menu_shell_append (GTK_MENU_SHELL (lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK (evacua_truppa), (gpointer) &tr_callback[Fen]);
	gtk_widget_show (oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (scelta), lista);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), scelta);
	gtk_widget_show (scelta);
	//mostra il menu
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,1, gtk_get_current_event_time());
}

static void click_unita (char* pos)
{
	GtkWidget *menu;
	GtkWidget *oggetto;
	char buf[50];
	int Pos=(int) (pos-infomappa.mappa);
	t_infotruppa* T=infomappa.truppe[Pos];
	switch(T->tipo)
	{
		case Rec:
			sprintf(buf,"-%d Reclute (%d)-",T->numero,T->morale);
			break;
		case Fan:
			sprintf(buf,"-%d Fanteria (%d)-",T->numero,T->morale);
			break;
		case Lan:
			sprintf(buf,"-%d Lancieri (%d)-",T->numero,T->morale);
			break;
		case Arc:
			sprintf(buf,"-%d Arcieri (%d)-",T->numero,T->morale);
			break;
		case Cav:
			sprintf(buf,"-%d Cavalieri (%d)-",T->numero,T->morale);
			break;
		case Dra:
			sprintf(buf,"-%d Draghi (%d)-",T->numero,T->morale);
			break;
		case Fen:
			sprintf(buf,"-%d Fenici (%d)-",T->numero,T->morale);
			break;
		default:
			sprintf(buf,"-%d ERRORE! (%d)-",T->numero,T->morale);
			break;
	}
	/*crea menu*/
	menu=gtk_menu_new();
	/*info*/
	oggetto=gtk_menu_item_new_with_label (buf);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), oggetto);
	gtk_widget_show (oggetto);
	/*pulsanti*/
	oggetto=gtk_menu_item_new_with_label ("Muovi");
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (muovi_unita), (gpointer) pos);
	gtk_widget_show (oggetto);
	oggetto=gtk_menu_item_new_with_label ("Combatti");
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), oggetto);
	g_signal_connect_swapped (oggetto, "activate", G_CALLBACK (combatti_unita), (gpointer) pos);
	gtk_widget_show (oggetto);
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,1, gtk_get_current_event_time());
	printf("unità cliccata!\n");
}

static void click_nemico (char* pos)
{
	int Pos=(int) (pos-infomappa.mappa);
	char buf[50];
	t_infotruppa* T=infomappa.truppe[Pos];
	GtkWidget *menu;
	GtkWidget *oggetto;
	switch(T->tipo)
	{
		case Rec:
			sprintf(buf,"-%d Reclute (%d)-",T->numero,T->morale);
			break;
		case Fan:
			sprintf(buf,"-%d Fanteria (%d)-",T->numero,T->morale);
			break;
		case Lan:
			sprintf(buf,"-%d Lancieri (%d)-",T->numero,T->morale);
			break;
		case Arc:
			sprintf(buf,"-%d Arcieri (%d)-",T->numero,T->morale);
			break;
		case Cav:
			sprintf(buf,"-%d Cavalieri (%d)-",T->numero,T->morale);
			break;
		case Dra:
			sprintf(buf,"-%d Draghi (%d)-",T->numero,T->morale);
			break;
		case Fen:
			sprintf(buf,"-%d Fenici (%d)-",T->numero,T->morale);
			break;
		default:
			sprintf(buf,"-%d ERRORE! (%d)-",T->numero,T->morale);
			break;
	}
	/*crea menu*/
	menu=gtk_menu_new();
	/*info*/
	oggetto=gtk_menu_item_new_with_label (buf);
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), oggetto);
	gtk_widget_show (oggetto);
	/*pulsanti*/
	gtk_menu_popup (GTK_MENU (menu), NULL, NULL, NULL, NULL,1, gtk_get_current_event_time());
	printf("unità cliccata!\n");
}

static void click_assediocastello (char* pos)
{
	int Pos=(int) (pos-infomappa.mappa);
	int g;
	int C=0;
	t_lista_s* Castello;
	t_lista_t* Secondalinea;
	g=controlloedificio (Pos,Cas);
	if (g>0)
	{
		Castello=giocatore[g]->struttura[Cas];
		while(Castello->pos!=Pos) Castello=Castello->next;
		if(assaltamura(Castello)==1)
		{
			Secondalinea=Castello->in;
			while(Secondalinea!=NULL && Secondalinea->truppa->combattuto==1) Secondalinea=Secondalinea->next;
			if(Secondalinea==NULL && Castello->in!=NULL)
			{
				Secondalinea=Castello->in;
				while (Secondalinea!=NULL) 
				{
					Secondalinea->truppa->morale/=3;
					Secondalinea=Secondalinea->next;
				}
				Secondalinea=Castello->in;
			}
			if(assaltabreccia(puntacasellaoccupata (Pos,C++),Secondalinea)==1) cambiaproprietario (0, g,Pos,Cas);
		}
	}
	else
	{
		cambiaproprietario (0, g,Pos,Cas);
	}
	gtk_aggiorna_tab_armate ();
	gtk_aggiorna_tab_castelli ();
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx,cy,'n');
}

static void click_assaltostruttura (char* pos)
{
	int Pos=(int) (pos-infomappa.mappa);
	int g;
	int morale=0;
	t_struttura i;
	t_lista_s* Struttura;
	t_lista_t* Difensori;
	fprintf(stderr,"debug: click_assaltostruttura\n");
	for(i=1;i<NUMSTRUTTURE && g>0;i++) g=controlloedificio (Pos,i);
	if (g>0)
	{
		fprintf(stderr,"debug: di qualcuno\n");
		Struttura=giocatore[g]->struttura[i];
		while(Struttura->pos!=Pos) Struttura=Struttura->next;
		if(assaltaedificio(Struttura)==1) 
		{
			Difensori=Struttura->in;
			while (Difensori!=NULL)
			{
				Difensori->truppa->morale-=10;
				morale+=Difensori->truppa->morale;
				Difensori=Difensori->next;
			}
			Difensori=Struttura->in;
			if(morale==0) cambiaproprietario (0, g,Pos,i);
		}
		else
		{
			while (Difensori!=NULL)
			{
				Difensori->truppa->morale+=10;
				Difensori=Difensori->next;
			}
		}
	}
	else
	{
		fprintf(stderr,"debug: di nessuno\n");
		switch(infomappa.mappa[Pos])
		{
			case 'G':
				i=Gro;
				break;
			case 'S':
				i=Scu;
				break;
			case 'N':
				i=Nid;
				break;
			case 'C':
				i=Fat;
				break;
			default:
				fprintf(stderr,"c'è un bug!\n");
				i=Cas;
				break;
		}
		cambiaproprietario (0, g,Pos,i);
	}
	gtk_aggiorna_tab_strutture ();
	gtk_aggiorna_tab_armate ();
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx,cy,'n');
}

static void click_entrastruttura (char* pos)
{
	int Pos=(int) (pos-infomappa.mappa);
	int g=controllounita(Mossa);
	t_lista_t* T;
	t_lista_t* Tp=NULL;
	t_lista_t* Ts;
	t_lista_s* S;
	fprintf(stderr,"debug: click_entrastruttura\n");
	//cerca la struttura
	S=puntastruttura (Pos);
	//cerca l'unità nella lista unità
	T=giocatore[g]->truppe;
	while (T->pos!=Mossa && T->next!=NULL)
	{
		Tp=T;
		T=T->next;
	}
	if (Tp==NULL) giocatore[g]->truppe=T->next;
	else Tp->next=T->next;
	//cerca la coda della lista struttura e sposta dentro l'unità
	Ts=S->in;
	if(Ts==NULL) S->in=T;
	else
	{
		while(Ts->next!=NULL) Ts=Ts->next;
		Ts->next=T;
	}
	T->pos=Pos;
	T->next=NULL;
	infomappa.truppe[Mossa]=NULL;
	gtk_aggiorna_tab_armate ();
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx,cy,'n');
}

static void click_unisci (char* pos)
{
	int Pos=(int)(pos-infomappa.mappa);
	int max, somma, min;
	const int a=0,b=1;
	GtkWidget * Dialogo;
	GtkWidget * Vbox;
	GtkWidget * Label;
	GtkWidget * Spin1;
	GtkWidget * Spin2;
	GtkObject *UA, *UB;
	t_infotruppa* TA=infomappa.truppe[Mossa];
	t_infotruppa* TB=infomappa.truppe[Pos];
	t_spin S_Callback[2];
	t_truppa Tipo=TB->tipo;
	switch(Tipo)
	{
		case Rec:
		case Fan:
		case Lan:
		case Arc:
			max=100*2;
			break;
		case Cav:
			max=50*2;
			break;
		case Dra:
		case Fen:
			max=30*2;
			break;
	}
	somma=TA->numero+TB->numero;
	min=(somma-max)>0?(somma-max):0;
	UA=gtk_adjustment_new(TA->numero, min,max<somma?max:somma, 1, 0, 0);
	UB=gtk_adjustment_new(TB->numero, min,max<somma?max:somma, 1, 0, 0);
	S_Callback[a].A=UA;
	S_Callback[a].somma=somma;
	S_Callback[b].A=UB;
	S_Callback[b].somma=somma;
	g_signal_connect (UA, "value_changed", G_CALLBACK (set_adjustmentvalue), &S_Callback[b]);
	g_signal_connect (UB, "value_changed", G_CALLBACK (set_adjustmentvalue), &S_Callback[a]);
	Dialogo=gtk_dialog_new_with_buttons("Fantasy C",NULL,GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK,GTK_RESPONSE_YES,GTK_STOCK_CANCEL,GTK_RESPONSE_NO,NULL);
	gtk_window_set_icon (GTK_WINDOW (Dialogo),Immagine.logo);
	Vbox=gtk_vbox_new(TRUE,0);
	gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG(Dialogo))),Vbox, TRUE, TRUE, 0);
	gtk_widget_show(Vbox);
	Label=gtk_label_new("Sposta effettivi");
	gtk_box_pack_start (GTK_BOX (Vbox),Label, TRUE, TRUE, 0);
	gtk_widget_show(Label);
	Spin1=gtk_spin_button_new(GTK_ADJUSTMENT(UA), 0, 0);
	gtk_box_pack_start (GTK_BOX (Vbox),Spin1, TRUE, TRUE, 0);
	gtk_widget_show(Spin1);
	Spin2=gtk_spin_button_new(GTK_ADJUSTMENT(UB), 0, 0);
	gtk_box_pack_start (GTK_BOX (Vbox),Spin2, TRUE, TRUE, 0);
	gtk_widget_show(Spin2);
	if(gtk_dialog_run(GTK_DIALOG(Dialogo))==GTK_RESPONSE_YES) 
	{
		TB->stanca=1;
		TA->numero=gtk_adjustment_get_value (GTK_ADJUSTMENT(UA));
		TB->numero=gtk_adjustment_get_value (GTK_ADJUSTMENT(UB));
		if(TA->numero==0)
			eliminamorti(TA);
		if(TB->numero==0)
			eliminamorti(TB);
		gtk_widget_destroy (Dialogo);
	}
	else
	{
		TA->stanca=0;
		gtk_widget_destroy (Dialogo);
	}
	gtk_pulisci_mappa();
	gtk_aggiorna_tab_armate();
	gtk_stampa_mappa(cx,cy,'n');
}

// funzioni grafiche
void gtk_inizializza_widget()
{
	int i;
	for (i=0; i<NUMCASTELLI; i++) Listacastelli[i]=NULL;
	Listastrutture=NULL;
	Listatruppe=NULL;
}

void gtk_carica_immagini ()
{
	int i,j;
	char Buf[100];
	sprintf(Buf,"immagini/Fantasy-icon.xpm");
	Immagine.err=gdk_pixbuf_new_from_file_at_size (Buf,30,30,NULL);
	Immagine.logo=gdk_pixbuf_new_from_file (Buf,NULL);
	sprintf(Buf,"immagini/Freccia.xpm");
	Immagine.freccia=gdk_pixbuf_new_from_file_at_size (Buf,30,30,NULL);
	sprintf(Buf,"immagini/p/p1.xpm");
	Immagine.p=gdk_pixbuf_new_from_file (Buf,NULL);
	sprintf(Buf,"immagini/mov.xpm");
	Immagine.movimento=gdk_pixbuf_new_from_file_at_size (Buf,Dim_casella,Dim_casella,NULL);
	sprintf(Buf,"immagini/atk.xpm");
	Immagine.attacco=gdk_pixbuf_new_from_file_at_size (Buf,Dim_casella,Dim_casella,NULL);
	
	//carica il castello
	for (i=0;i<1;i++)
		for (j=0; j<9; j++)
		{
			sprintf(Buf,"immagini/c/c%d.xpm",j);
			Immagine.c[i][j]=gdk_pixbuf_new_from_file_at_size (Buf,Dim_casella,Dim_casella,NULL);
		}
	//carica la grotta
	for (i=0;i<1;i++)
		for (j=0; j<4; j++)
		{
			sprintf(Buf,"immagini/g/g%d.xpm",j);
			Immagine.g[i][j]=gdk_pixbuf_new_from_file_at_size (Buf,Dim_casella,Dim_casella,NULL);
		}
	//carica la stalla
	for (i=0;i<1;i++)
		for (j=0; j<4; j++)
		{
			sprintf(Buf,"immagini/s/s%d.xpm",j);
			Immagine.s[i][j]=gdk_pixbuf_new_from_file_at_size (Buf,Dim_casella,Dim_casella,NULL);
		}
	//carica la fattoria
	for (i=0;i<1;i++)
		for (j=0; j<4; j++)
		{
			sprintf(Buf,"immagini/f/f%d.xpm",j);
			Immagine.f[i][j]=gdk_pixbuf_new_from_file_at_size (Buf,Dim_casella,Dim_casella,NULL);
		}
	//carica il nido
	for (i=0;i<1;i++)
		for (j=0; j<4; j++)
		{
			sprintf(Buf,"immagini/n/n%d.xpm",j);
			Immagine.n[i][j]=gdk_pixbuf_new_from_file_at_size (Buf,Dim_casella,Dim_casella,NULL);
		}
	//carica le truppe
	for (i=0; i<1; i++)
		for(j=0; j<=Lan; j++)
		{
			sprintf(Buf,"immagini/t/t%d%d.xpm",i,j);
			Immagine.t[i][j]=gdk_pixbuf_new_from_file_at_size (Buf,Dim_casella,Dim_casella,NULL);
		}
}

void gtk_calcola_dimensioni ()
{
	int w;
	w=gdk_screen_get_width (gdk_screen_get_default ());
	if (w>1500) Dim_casella=40;
	else if (w>1200) Dim_casella=28;
	else if (w>800) Dim_casella=15;
	else Dim_casella=10;
	printf("\nlarghezza: %d\naltezza: %d\n",w, Dim_casella);
		
}

void gtk_crea_menu (GtkWidget *Vbox)
{
	GtkWidget *menu;
	GtkWidget *oggetto_menu;
	GtkWidget *barra_menu;
	GtkWidget *menu_file, *menu_help;
	//genera menu file
	menu=gtk_menu_new();
	// 	genera scelta 1
	oggetto_menu=gtk_menu_item_new_with_label ("Nuova Partita");
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), oggetto_menu);
	g_signal_connect (oggetto_menu, "activate", G_CALLBACK (nuova_partita), (gpointer) NULL);
	gtk_widget_show (oggetto_menu);
	// 	genera scelta 2
	oggetto_menu=gtk_menu_item_new_with_label ("Carica");
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), oggetto_menu);
	g_signal_connect_swapped (oggetto_menu, "activate", G_CALLBACK (salva_carica),(gpointer) 0);
	gtk_widget_show (oggetto_menu);
	// 	genera scelta 3
	oggetto_menu=gtk_menu_item_new_with_label ("Salva");
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), oggetto_menu);
	g_signal_connect_swapped (oggetto_menu, "activate", G_CALLBACK (salva_carica),(gpointer) 1);
	gtk_widget_show (oggetto_menu);
	// 	genera scelta 4
	oggetto_menu=gtk_menu_item_new_with_label ("Esci");
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), oggetto_menu);
	g_signal_connect (oggetto_menu, "activate", G_CALLBACK (chiudi_da_menu), (gpointer) NULL);
	gtk_widget_show (oggetto_menu);
	// 	genera etichetta file 
	menu_file=gtk_menu_item_new_with_label ("File");
	gtk_widget_show (menu_file);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_file), menu);
	// 	genera menu help 
	menu=gtk_menu_new();
	// 	genera scelta 1
	oggetto_menu=gtk_menu_item_new_with_label ("Manuale");
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), oggetto_menu);
	g_signal_connect (oggetto_menu, "activate", G_CALLBACK (menuitem_response), (gpointer) NULL);
	gtk_widget_show (oggetto_menu);
	// 	genera scelta 2
	oggetto_menu=gtk_menu_item_new_with_label ("Crediti");
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), oggetto_menu);
	g_signal_connect (oggetto_menu, "activate", G_CALLBACK (menuitem_response), (gpointer) NULL);
	gtk_widget_show (oggetto_menu);
	// 	genera etichetta help
	menu_help=gtk_menu_item_new_with_label ("Help");
	gtk_widget_show (menu_help);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_help), menu);
	// 	genera barra dei menu 
	barra_menu=gtk_menu_bar_new ();
	gtk_box_pack_start( GTK_BOX(Vbox), barra_menu, FALSE, FALSE, 0);
	gtk_widget_show (barra_menu);
	gtk_menu_shell_append (GTK_MENU_SHELL (barra_menu), menu_file);
	gtk_menu_shell_append (GTK_MENU_SHELL (barra_menu), menu_help);
	// 	fine menu 
}

void gtk_pulisci_mappa ()
{
	int Pos;
	if(partita_in_corso==1)
		for (Pos=0; Pos<L_SCHERMO*A_SCHERMO; Pos++) 
		{
			
			g_signal_handlers_disconnect_matched(Casella[Pos],G_SIGNAL_MATCH_FUNC,0,0,0, click_castello, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],G_SIGNAL_MATCH_FUNC,0,0,0, click_scuderia, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],G_SIGNAL_MATCH_FUNC,0,0,0, click_fattoria, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],G_SIGNAL_MATCH_FUNC,0,0,0, click_nido, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],G_SIGNAL_MATCH_FUNC,0,0,0, click_grotta, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],G_SIGNAL_MATCH_FUNC,0,0,0, click_unita, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],G_SIGNAL_MATCH_FUNC,0,0,0, click_destinazione, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],G_SIGNAL_MATCH_FUNC,0,0,0, click_bersaglio, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],G_SIGNAL_MATCH_FUNC,0,0,0, click_assediocastello, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],G_SIGNAL_MATCH_FUNC,0,0,0, click_assaltostruttura, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],G_SIGNAL_MATCH_FUNC,0,0,0, click_entrastruttura, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],G_SIGNAL_MATCH_FUNC,0,0,0, click_unisci, 0);
			gtk_widget_destroy(Thumb[Pos]);
		}
}

void gtk_stampa_mappa(int x, int y, char m)
{
	int C;
	int R;
	int Pos=0;
	int G;
	for(R=y;R<y+A_SCHERMO;R++)
		for(C=x;C<x+L_SCHERMO;C++)
		{
			switch (accedi(C,R,infomappa.mappa))
			{
				/*stampa il castello*/
				case '0':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.c[0][0]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(-1,-1,C,R), Cas)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_castello), (gpointer) &infomappa.mappa[posiziona(-1,-1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(-1,-1,C,R),Cas)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assediocastello), (gpointer) &infomappa.mappa[posiziona(-1,-1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(-1,-1,C,R),Cas)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(-1,-1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '1':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.c[0][1]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(0,-1,C,R),Cas)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_castello), (gpointer) &infomappa.mappa[posiziona(0,-1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,-1,C,R),Cas)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assediocastello), (gpointer) &infomappa.mappa[posiziona(0,-1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,-1,C,R),Cas)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(0,-1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '2':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.c[0][2]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(1,-1,C,R),Cas)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_castello), (gpointer) &infomappa.mappa[posiziona(1,-1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,-1,C,R),Cas)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assediocastello), (gpointer) &infomappa.mappa[posiziona(1,-1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,-1,C,R),Cas)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(1,-1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '3':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.c[0][3]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(-1,0,C,R),Cas)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_castello), (gpointer) &infomappa.mappa[posiziona(-1,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(-1,0,C,R),Cas)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assediocastello), (gpointer) &infomappa.mappa[posiziona(-1,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(-1,0,C,R),Cas)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(-1,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '4':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.c[0][4]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(0,0,C,R),Cas)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_castello), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '5':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.c[0][5]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(1,0,C,R),Cas)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_castello), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Cas)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assediocastello), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Cas)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '6':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.c[0][6]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(-1,1,C,R),Cas)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_castello), (gpointer) &infomappa.mappa[posiziona(-1,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(-1,1,C,R),Cas)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assediocastello), (gpointer) &infomappa.mappa[posiziona(-1,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(-1,1,C,R),Cas)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(-1,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '7':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.c[0][7]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(0,1,C,R),Cas)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_castello), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Cas)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assediocastello), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Cas)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '8':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.c[0][8]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(1,1,C,R),Cas)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_castello), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Cas)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assediocastello), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Cas)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				/*stampa la grotta*/
				case 'G':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.g[0][0]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(0,0,C,R),Gro)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_grotta), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Gro)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Gro)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'H':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.g[0][1]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(1,0,C,R),Gro)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_grotta), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Gro)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Gro)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'I':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.g[0][2]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(0,1,C,R),Gro)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_grotta), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Gro)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Gro)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'J':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.g[0][3]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(1,1,C,R),Gro)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_grotta), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Gro)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Gro)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				/*stampa la fattoria*/
				case 'C':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.f[0][0]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(0,0,C,R),Fat)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_fattoria), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Fat)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Fat)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'D':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.f[0][1]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(1,0,C,R),Fat)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_fattoria), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Fat)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Fat)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'E':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.f[0][2]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(0,1,C,R),Fat)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_fattoria), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Fat)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Fat)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'F':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.f[0][3]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(1,1,C,R),Fat)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_fattoria), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Fat)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Fat)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				/*stampa la scuderia*/
				case 'S':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.s[0][0]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(0,0,C,R),Scu)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_scuderia), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Scu)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Scu)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'T':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.s[0][1]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(1,0,C,R),Scu)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_scuderia), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Scu)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Scu)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'U':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.s[0][2]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(0,1,C,R),Scu)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_scuderia), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Scu)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Scu)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'V':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.s[0][3]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(1,1,C,R),Scu)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_scuderia), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Scu)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Scu)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;				
				/*stampa in nido*/
				case 'N':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.n[0][0]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(0,0,C,R),Nid)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_nido), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Nid)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Nid)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'O':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.n[0][1]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(1,0,C,R),Nid)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_nido), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Nid)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Nid)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(1,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'P':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.n[0][2]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(0,1,C,R),Nid)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_nido), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Nid)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Nid)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(0,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'Q':
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.n[0][3]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && controlloedificio (posiziona(1,1,C,R),Nid)==0) 
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_nido), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Nid)==1)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_assaltostruttura), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Nid)==0)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_entrastruttura), (gpointer) &infomappa.mappa[posiziona(1,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case ' ':
					if(accedi(C,R,infomappa.truppe)!=NULL) 
					{
						G=controllounita (posiziona(0,0,C,R));
						switch (accedi(C,R,infomappa.truppe)->tipo)
						{
							case Rec:
								Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.t[G][0]);
								gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
								gtk_widget_show(Thumb[Pos]);
								break;
							case Fan:
								Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.t[G][1]);
								gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
								gtk_widget_show(Thumb[Pos]);
								break;
							case Lan:
								Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.t[G][2]);
								gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
								gtk_widget_show(Thumb[Pos]);
								break;
							default:
								Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.err);
								gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
								gtk_widget_show(Thumb[Pos]);
								break;
						}
						if(m=='n')
						{
							if (G==0)
								g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_unita), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
							else
								g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_nemico), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
						}
						else if(m=='c' && bersagliolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiversotruppe(Mossa,posiziona(0,0,C,R))==1)
							g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_bersaglio), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
						else if(m=='s' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiversotruppe(Mossa,posiziona(0,0,C,R))==0 && tipouguale(C,R))
							g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_unisci), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					}
					else if (m=='s' && spostalecito(Mossa,posiziona(0,0,C,R))==1)
					{
						Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.movimento);
						gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_destinazione), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
						gtk_widget_show(Thumb[Pos]);
					}
					else if (m=='c' && bersagliolecito(Mossa,posiziona(0,0,C,R))==1)
					{
						Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.attacco);
						gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
						gtk_widget_show(Thumb[Pos]);
					}
					else
					{
						Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.p);
						gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
						gtk_widget_show(Thumb[Pos]);
					}
					if (m=='s' && posiziona(0,0,C,R)==Mossa)
						g_signal_connect_swapped (Casella[Pos], "button_press_event", G_CALLBACK (click_destinazione), (gpointer) &infomappa.mappa[posiziona(0,0,C,R)]);
					break;
				default:
					Thumb[Pos]=gtk_image_new_from_pixbuf (Immagine.err);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					gtk_widget_show(Thumb[Pos]);
					break;
			}
			Pos++;
		}
}

void gtk_genera_mappa (GtkWidget *Hbox)
{
	GtkWidget *Mappa;
	int i,j;
	Mappa=gtk_table_new( L_SCHERMO, A_SCHERMO, FALSE);
	gtk_box_pack_start( GTK_BOX(Hbox), Mappa, FALSE, FALSE, 0);
	for (i=0;i<L_SCHERMO;i++)
		for (j=0; j<A_SCHERMO; j++)
		{
			Casella[i+j*L_SCHERMO]=gtk_event_box_new();
			gtk_widget_set_events (Casella[i+j*L_SCHERMO], GDK_BUTTON_PRESS_MASK);
			gtk_widget_set_usize(Casella[i+j*L_SCHERMO], Dim_casella,Dim_casella);
			gtk_table_attach_defaults (GTK_TABLE (Mappa), Casella[i+j*L_SCHERMO], i, i+1, j, j+1);
			gtk_widget_show (Casella[i+j*L_SCHERMO]);
			gtk_widget_realize (Casella[i+j*L_SCHERMO]);
			
		}
	gtk_widget_show (Mappa);
}

GtkWidget *gtk_crea_notebook_tab (GtkWidget *Notebook,char *buf)
{
	GtkWidget *Etichetta;
	GtkWidget *Vbox;
	GtkWidget *Scroller;
	Etichetta=gtk_label_new(buf);
	Scroller=gtk_scrolled_window_new( NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(Scroller),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	Vbox=gtk_vbox_new(FALSE,0);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (Scroller), Vbox);
	gtk_widget_show(Vbox);
	gtk_widget_show(Etichetta);
	gtk_widget_show(Scroller);
	gtk_notebook_append_page(GTK_NOTEBOOK (Notebook),Scroller, Etichetta);
	return (GtkWidget *)Vbox;
}

void gtk_crea_notebook (GtkWidget *Frame)
{
	Notebook[0]=gtk_notebook_new();
	gtk_container_add (GTK_CONTAINER (Frame), Notebook[0]);
	Notebook[1]=(GtkWidget *)gtk_crea_notebook_tab(Notebook[0],"Castelli");
	Notebook[2]=(GtkWidget *)gtk_crea_notebook_tab(Notebook[0],"Strutture");
	Notebook[3]=(GtkWidget *)gtk_crea_notebook_tab(Notebook[0],"Armate");
}

GtkWidget *gtk_crea_elemento_tab(GtkWidget *tab,int x, int y,char *buf)
{
	static int i=1;
	GtkWidget *Hbox;
	GtkWidget *Vbox;
	GtkWidget *Label;
	GtkWidget *Button;
	GtkWidget *Separator;
	char buffer[10];
	if (tab==NULL)
	{
		i=1;
		return NULL;
	}
	Vbox=gtk_vbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX (tab), Vbox,FALSE, FALSE, 0);
	if (i!=1)
	{
		Separator=gtk_hseparator_new ();
		gtk_box_pack_start(GTK_BOX (Vbox), Separator,FALSE, FALSE, 3);
		gtk_widget_show (Separator);
	}
	Hbox=gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX (Vbox), Hbox,FALSE, FALSE, 0);
	Label=gtk_label_new (buf);
	gtk_box_pack_start(GTK_BOX(Hbox),Label, FALSE, FALSE, 0);
	gtk_widget_show (Label);
	sprintf(buffer,"N° %d",i);
	Label=gtk_label_new (buffer);
	gtk_box_pack_start(GTK_BOX(Hbox),Label, FALSE, FALSE, 0);
	gtk_widget_show (Label);
	sprintf(buffer,"%d|%d",x,y);
	Button=gtk_button_new_with_label(buffer);
	g_signal_connect_swapped (Button, "clicked", G_CALLBACK (centra_mappa), (gpointer) &infomappa.mappa[x+y*LARGHEZZA]);
	gtk_box_pack_start(GTK_BOX(Hbox),Button, FALSE, FALSE, 0);
	gtk_widget_show (Button);
	gtk_widget_show (Hbox);
	gtk_widget_show (Vbox);
	i++;
	return Vbox;
}

//riempe il tab castelli giocatore
GtkWidget * gtk_riempi_tab_castelli (int i, char* buf)
{
	if(i!=0)
	{
	int P;
	int x,y;
	GtkWidget *R;
	P=giocatore[CurrentPlayer]->struttura[Cas]->pos;
	x=P%LARGHEZZA+1;
	y=P/LARGHEZZA+1;
	R=gtk_crea_elemento_tab(Notebook[1],x,y,buf);
	return R;
	}
	else return NULL;
}

//aggiorna il tab castelli
void gtk_aggiorna_tab_castelli ()
{
	int i;
	int x,y;
	char buf[20];
	t_lista_s* S=giocatore[CurrentPlayer]->struttura[Cas];
	for(i=0; Listacastelli[i]!=NULL;i++) gtk_pulisci_tab(Listacastelli[i]);
	for (i=0; S!=NULL && i<12; i++)
	{
		x=S->pos%LARGHEZZA+1;
		y=S->pos/LARGHEZZA+1;
		switch(i)
		{
			case 0:
				sprintf(buf,"Capitale");
				break;
			default:
				sprintf(buf,"Città %d",i);
				break;
		}
		Listacastelli[i]=gtk_crea_elemento_tab(Notebook[1],x, y, buf);
		S=S->next;
	}
}

//aggiorna il tab strutture
void gtk_aggiorna_tab_strutture ()
{
	int i;
	int x,y;
	char buf[20];
	t_lista_s* S[NUMSTRUTTURE-1];
	t_listapertab* Tab;
	t_listapertab* Tabp;
	fprintf(stderr, "debug: gtk_aggiorna_tab_strutture\n");
	for(i=1; i<NUMSTRUTTURE;i++) S[i-1]=giocatore[CurrentPlayer]->struttura[i];
	Tab=Listastrutture;
	while(Tab!=NULL)
	{
		gtk_pulisci_tab(Tab->P);
		Tabp=Tab;
		Tab=Tab->next;
		free(Tabp);
	}
	Listastrutture=NULL;
	for(i=0;i<NUMSTRUTTURE-1; i++)
		while(S[i]!=NULL)
		{
			x=S[i]->pos%LARGHEZZA+1;
			y=S[i]->pos/LARGHEZZA+1;
			switch(i+1)
			{
				case Fat:
					sprintf(buf,"Fattoria");
					break;
				case Scu:
					sprintf(buf,"Scuderia");
					break;
				case Gro:
					sprintf(buf,"Grotta");
					break;
				case Nid:
					sprintf(buf,"Nido");
					break;
				default:
					sprintf(buf,"Errore");
					break;
			}
			Tabp=Tab;
			Tab=(t_listapertab*)malloc(sizeof(t_listapertab));
			if(Tabp==NULL) Listastrutture=Tab;
			else Tabp->next=Tab;
			Tab->P=gtk_crea_elemento_tab(Notebook[2],x, y, buf);
			Tab->next=NULL;
			S[i]=S[i]->next;
		}
}

//aggiorna il tab armate
void gtk_aggiorna_tab_armate ()
{
	int x,y;
	char buf[20];
	t_lista_t* T=giocatore[CurrentPlayer]->truppe;
	t_listapertab* Tab;
	t_listapertab* Tabp;
	Tab=Listatruppe;
	while(Tab!=NULL)
	{
		gtk_pulisci_tab(Tab->P);
		Tabp=Tab;
		Tab=Tab->next;
		free(Tabp);
	}
	Listatruppe=NULL;
	while(T!=NULL)
	{
		x=T->pos%LARGHEZZA+1;
		y=T->pos/LARGHEZZA+1;
		switch(T->truppa->tipo)
		{
			case Rec:
				sprintf(buf,"Reclute");
				break;
			case Fan:
				sprintf(buf,"Fanteria");
				break;
			case Lan:
				sprintf(buf,"Lancieri");
				break;
			case Arc:
				sprintf(buf,"Arcieri");
				break;
			case Cav:
				sprintf(buf,"Cavalleria");
				break;
			case Dra:
				sprintf(buf,"Drago");
				break;
			case Fen:
				sprintf(buf,"Fenice");
				break;
			default:
				sprintf(buf,"Errore");
				break;
		}
		Tabp=Tab;
		Tab=(t_listapertab*)malloc(sizeof(t_listapertab));
		if(Tabp==NULL) Listatruppe=Tab;
		else Tabp->next=Tab;
		Tab->P=gtk_crea_elemento_tab(Notebook[3],x, y, buf);
		Tab->next=NULL;
		T=T->next;
	}
}

//pulisce i tab
void gtk_pulisci_tab (GtkWidget *Target)
{
	if (Target==NULL) return;
	else gtk_widget_destroy (Target);
}

//azzera tab
void gtk_azzera_tab ()
{
	int i;
	t_listapertab* Tab;
	t_listapertab* Tabp;
	for(i=0;i<NUMCASTELLI;i++)
	{
		gtk_pulisci_tab (Listacastelli[i]);
		Listacastelli[i]=NULL;
	}
	Tab=Listatruppe;
	while(Tab!=NULL)
	{
		gtk_pulisci_tab(Tab->P);
		Tabp=Tab;
		Tab=Tab->next;
		free(Tabp);
	}
	Tab=Listastrutture;
	while(Tab!=NULL)
	{
		gtk_pulisci_tab(Tab->P);
		Tabp=Tab;
		Tab=Tab->next;
		free(Tabp);
	}
	Listatruppe=NULL;
	Listastrutture=NULL;
	gtk_crea_elemento_tab(NULL,0,0,NULL);
}

//crea 4 frecce direzionali
GtkWidget *gtk_crea_4_frecce()
{
	GtkWidget *Pulsantiera;
	GtkWidget *Pulsante;
	GtkWidget *Freccia;
	int i;
	Pulsantiera=gtk_table_new(3,3,FALSE);
	
	for (i=1; i<9;i=i+2)
	{
		Pulsante=gtk_button_new();
		gtk_widget_set_usize(Pulsante, DIM_FRECCIA,DIM_FRECCIA);
		g_signal_connect_swapped (Pulsante, "clicked", G_CALLBACK (sposta_mappa), (gpointer) &infomappa.mappa[i]);
		gtk_table_attach_defaults (GTK_TABLE (Pulsantiera), Pulsante, i/3, i/3+1, i%3, i%3+1);
		switch (i)
		{
			case 1:
				Immagine.freccia=gdk_pixbuf_rotate_simple (Immagine.freccia,GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE);
				break;
			case 3:
				Immagine.freccia=gdk_pixbuf_rotate_simple (Immagine.freccia,GDK_PIXBUF_ROTATE_CLOCKWISE);
				break;
			case 5:
				Immagine.freccia=gdk_pixbuf_rotate_simple (Immagine.freccia,GDK_PIXBUF_ROTATE_UPSIDEDOWN);
				break;
			case 7:
				Immagine.freccia=gdk_pixbuf_rotate_simple (Immagine.freccia,GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE);
				break;
			default:
				break;
		}
		Freccia=gtk_image_new_from_pixbuf (Immagine.freccia);
		gtk_container_add (GTK_CONTAINER (Pulsante), Freccia);
		gtk_widget_show (Freccia);
		gtk_widget_show (Pulsante);
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
	Label[0]=gtk_label_new("Oro:");
	Label[1]=gtk_label_new("Cibo:");
	Label[2]=gtk_label_new("Smeraldi:");
	Vbox=gtk_vbox_new(FALSE,10);
	for(i=0; i<NUMRISORSE; i++)
	{
		Hbox[i]=gtk_hbox_new(TRUE,10);
		gtk_box_pack_start(GTK_BOX(Vbox), Hbox[i], FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(Hbox[i]), Label[i], FALSE, FALSE, 0);
		Counter[i]=gtk_entry_new();
		gtk_editable_set_editable(GTK_EDITABLE(Counter[i]),FALSE);
		gtk_entry_set_text(GTK_ENTRY(Counter[i]),"0");
		gtk_entry_set_width_chars(GTK_ENTRY(Counter[i]),6);
		gtk_box_pack_start(GTK_BOX(Hbox[i]), Counter[i], FALSE, FALSE, 0);
		gtk_widget_show_all(Hbox[i]);
	}
	
	return Vbox;
}

//aggiorna i valori delle risorse
void gtk_aggiorna_contarisorse() 
{
	int i,j;
	int nk;
	int R[]={giocatore[CurrentPlayer]->oro,giocatore[CurrentPlayer]->cibo,giocatore[CurrentPlayer]->smeraldi};
	float n;
	char Buf[6];
	for(i=0; i<NUMRISORSE; i++)
	{
		n=(float)R[i];
		nk=0;
		while (n>999)
		{
			n=n/1000;
			nk++;
		}
		sprintf(Buf,"%.2f",n);
		for (j=0;nk>0 && j<6; j++, nk--) strcat(Buf,"k");
		gtk_entry_set_text(GTK_ENTRY(Counter[i]),Buf);
	}
}
