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

// callbacks
static void menuitem_response()
{
	printf("bottone schiacciato\n");
}

static gboolean set_adjustmentmax(GtkAdjustment* S, GtkAdjustment* D)
{
	int max=gtk_adjustment_get_value(GTK_ADJUSTMENT(S));
	int n=gtk_adjustment_get_value(GTK_ADJUSTMENT(D));
	
	gtk_adjustment_set_value(GTK_ADJUSTMENT(D), n<(max-1) ? n : (max-1));
	gtk_adjustment_set_upper(GTK_ADJUSTMENT(D), max-1);
	return FALSE;
}

static gboolean set_adjustmentvalue(GtkAdjustment* S,t_spin* C)
{
	int n=gtk_adjustment_get_value(GTK_ADJUSTMENT(S));
	int max=C->somma;
	
	GtkAdjustment* D=C->A;
	gtk_adjustment_set_value(GTK_ADJUSTMENT(D), max-n);
	return FALSE;
}

static void chiudi_da_menu()
{
	GtkWidget * Dialogo;
	GtkWidget * Domanda;
	
	Domanda=gtk_label_new("Vuoi veramente uscire?");
	Dialogo=gtk_dialog_new_with_buttons("Fantasy C",NULL,GTK_DIALOG_DESTROY_WITH_PARENT, "Si",GTK_RESPONSE_YES, "No",GTK_RESPONSE_NO,NULL);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),Domanda, TRUE, TRUE, 0);
	gtk_widget_show(Domanda);
	gtk_window_set_icon(GTK_WINDOW(Dialogo),Immagine.logo);
	if(gtk_dialog_run(GTK_DIALOG(Dialogo))==GTK_RESPONSE_YES)
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
	char* buf=NULL;
	const char* Home=g_get_home_dir();
	
	if(Data==0)
	{
		Fselect=gtk_file_chooser_dialog_new("Carica", NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OPEN,GTK_RESPONSE_ACCEPT,NULL);
	} 
	else 
	{
		if(partita_in_corso==0) return;
		Fselect=gtk_file_chooser_dialog_new("Salva", NULL,GTK_FILE_CHOOSER_ACTION_SAVE,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_SAVE,GTK_RESPONSE_ACCEPT,NULL);
		gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(Fselect),TRUE);
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(Fselect),".fc");
	}
	gtk_window_set_icon(GTK_WINDOW(Fselect),Immagine.logo);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(Fselect),Home);
	gtk_file_filter_add_pattern(filter, "*fc");
	gtk_file_filter_add_pattern(filter, "*FC");
	gtk_file_filter_set_name(filter,"File FC");
	gtk_file_filter_add_pattern(filter_all, "*");
	gtk_file_filter_set_name(filter_all,"Tutti i file");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(Fselect),filter);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(Fselect),filter_all);
	while(gtk_dialog_run(GTK_DIALOG(Fselect))==GTK_RESPONSE_ACCEPT)
	{
		buf=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(Fselect));
		if(Data==0)
		{
			if(carica(buf)!=0)
			{
				Dialogo=gtk_dialog_new();
				gtk_dialog_add_buttons(GTK_DIALOG(Dialogo),GTK_STOCK_OK,1,NULL);
				gtk_window_set_icon(GTK_WINDOW(Dialogo),Immagine.logo);
				gtk_window_set_title(GTK_WINDOW(Dialogo),"Errore!");
				Label=gtk_label_new("File non valido!");
				gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),Label, TRUE, TRUE, 0);
				gtk_widget_show(Label);
				gtk_dialog_run(GTK_DIALOG(Dialogo));
				gtk_widget_destroy(Dialogo);
			}
			else
			{
				gtk_azzera_tab();
				gtk_pulisci_mappa();
				gtk_stampa_mappa(cx,cy, 'n');
				gtk_aggiorna_contarisorse();
				gtk_aggiorna_giocatore_c();
				gtk_aggiorna_tab_castelli();
				gtk_aggiorna_tab_strutture();
				gtk_aggiorna_tab_armate();
				partita_in_corso=1;
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
	
	fprintf(stderr,"debug nuova_partita\n");
	Giocatori=(GtkAdjustment *)gtk_adjustment_new(4, 2, MAXGIOCATORI, 1, 2, 0);
	IA=(GtkAdjustment *)gtk_adjustment_new(3, 0, MAXGIOCATORI-1, 1, 1, 0);
	g_signal_connect(Giocatori, "value_changed", G_CALLBACK(set_adjustmentmax), IA);
	Dialogo=gtk_dialog_new();
	gtk_dialog_add_buttons(GTK_DIALOG(Dialogo),"Inizia!",1,"Annulla",0,NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(Dialogo),1);
	gtk_window_set_icon(GTK_WINDOW(Dialogo),Immagine.logo);
	gtk_window_set_title(GTK_WINDOW(Dialogo),"Fantasy C");
	Opzioni=gtk_frame_new("Opzioni Nuova Partita");
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),Opzioni, TRUE, TRUE, 0);
	Vbox=gtk_vbox_new(TRUE,0);
	gtk_container_add(GTK_CONTAINER(Opzioni), Vbox);
	Hbox=gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(Vbox),Hbox, TRUE, TRUE, 0);
	Label=gtk_label_new("Giocatori");
	gtk_box_pack_start(GTK_BOX(Hbox),Label, TRUE, TRUE, 0);
	SpinGiocatori=gtk_spin_button_new(GTK_ADJUSTMENT(Giocatori), 0, 0);
	gtk_box_pack_start(GTK_BOX(Hbox),SpinGiocatori, TRUE, TRUE,0);
	Hbox=gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(Vbox),Hbox, TRUE, TRUE, 0);
	Label=gtk_label_new("Gestiti dall'IA");
	gtk_box_pack_start(GTK_BOX(Hbox),Label, TRUE, TRUE, 0);
	SpinIA=gtk_spin_button_new(GTK_ADJUSTMENT(IA), 0, 0);
	gtk_box_pack_start(GTK_BOX(Hbox),SpinIA, TRUE, TRUE, 0);
	gtk_widget_show_all(Opzioni);
	if(gtk_dialog_run(GTK_DIALOG(Dialogo))==1)
	{
		cx=0;
		cy=0;
		caricadati();
		inizializza();
		generamappa();
		visualizza_su_terminale();
		creagiocatori(gtk_adjustment_get_value(GTK_ADJUSTMENT(Giocatori)));
		CurrentPlayer=0;
		gtk_azzera_tab();
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx,cy, 'n');
		gtk_aggiorna_contarisorse();
		gtk_aggiorna_giocatore_c();
		Listacastelli[0]=gtk_riempi_tab_castelli(1, "Capitale");
		gtk_aggiorna_tab_armate();
		gtk_aggiorna_tab_strutture();
		partita_in_corso=1;
		gtk_widget_destroy(Dialogo);
	}
	else gtk_widget_destroy(Dialogo);
}

static void preferenze()
{
	int scelta;
	int subdirs_no,i,d=0;
	char *sottodir[64]; // magic
	char *skindefault="NintendOtaku";
	GtkWidget *Dialogo;
	GtkWidget *Opzioni;
	GtkWidget *Label;
	GtkWidget *Radio[64];
	GtkWidget *Vbox;

	Radio[0]=NULL;
	fprintf(stderr,"debug preferenze\n");
	subdirs_no=listaskin("skin",sottodir);
	Dialogo=gtk_dialog_new();
	gtk_dialog_add_buttons(GTK_DIALOG(Dialogo),"Salva",1,"Ripristina",2,"Annulla",0,NULL);
	gtk_window_set_icon(GTK_WINDOW(Dialogo),Immagine.logo);
	gtk_window_set_title(GTK_WINDOW(Dialogo),"Fantasy C Config");
	Opzioni=gtk_frame_new("Opzioni Skins");
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),Opzioni, TRUE, TRUE, 0);
	Vbox=gtk_vbox_new(TRUE,0);
	gtk_container_add(GTK_CONTAINER(Opzioni), Vbox);
	Label=gtk_label_new("Skin");
	gtk_box_pack_start(GTK_BOX(Vbox),Label, TRUE, TRUE, 0);
	for(i=0;i<subdirs_no;i++)
	{
		Radio[i]=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(Radio[0]),sottodir[i]); //una modifica
		gtk_box_pack_start(GTK_BOX(Vbox),Radio[i], TRUE, TRUE, 0); //due modifiche
		if(strcmp(sottodir[i],infogioco.skin)==0) gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(Radio[i]),TRUE);
		if(strcmp(sottodir[i],skindefault)==0) d=i;
	}
	gtk_widget_show_all(Opzioni);
	do
	{
		scelta=gtk_dialog_run(GTK_DIALOG(Dialogo));
		if(scelta==1) // salva
		{
			for(i=0;gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(Radio[i]))!=TRUE; i++);
			sprintf(infogioco.skin,"%s",sottodir[i]);
			salvaconfig("fantasy.config");
			gtk_carica_immagini();
			
			if(partita_in_corso!=0)
			{
				gtk_pulisci_mappa();
				gtk_stampa_mappa(cx,cy,'n');
			}
		}
		else if(scelta==2) // reset
		{
			if(remove("fantasy.config"))
			{
				perror("cannot remove config file");
				exit(1);
			}
			caricaconfig("fantasy.config");
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(Radio[d]),TRUE);
		}
	} while(scelta==2);
	gtk_widget_destroy(Dialogo);
}

static void centra_mappa(char* pos)
{
	int P=(int)(pos-infomappa.mappa);
	
	cx=P%LARGHEZZA+1-caselle_orizzontali/2;
	cy=P/LARGHEZZA+1-caselle_verticali/2;
	if(cx<0) cx=0;
	if(cy<0) cy=0;
	if(cx>LARGHEZZA-caselle_orizzontali)cx=(LARGHEZZA-caselle_orizzontali);
	if(cy>ALTEZZA-caselle_verticali)cy=(ALTEZZA-caselle_verticali);
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx,cy,'n');
}

static void sposta_mappa(char* v)
{
	int V=(int)(v-infomappa.mappa);
	
	if(partita_in_corso==0) return;
	switch(V)
	{
		case 1:
			if(cx-1<0)
				return;
			else
			{
				cx-=1;
				gtk_pulisci_mappa();
				gtk_stampa_mappa(cx,cy,'p');
			}
			break;
		case 3:
			if(cy-1<0)
				return;
			else
			{
				cy-=1;
				gtk_pulisci_mappa();
				gtk_stampa_mappa(cx,cy,'p');
			}
			break;
		case 5:
			if(cy+1>ALTEZZA-caselle_verticali)
				return;
			else
			{
				cy+=1;
				gtk_pulisci_mappa();
				gtk_stampa_mappa(cx,cy,'p');
			}
			break;
		case 7:
			if(cx+1>LARGHEZZA-caselle_orizzontali)
				return;
			else
			{
				cx+=1;
				gtk_pulisci_mappa();
				gtk_stampa_mappa(cx,cy,'p');
			}
			break;
		default:
			break;
	}
}

static void addestra_truppa(t_callback_s* Struct)
{
	int P=Struct->pos;
	int r;
	t_truppa T=Struct->tipo;
	GtkWidget *Dialogo;
	GtkWidget *Label;
	
	r=pagaunita(T);
	if(r==0) 
	{
		addestratruppa(P,T);
		gtk_aggiorna_contarisorse();
	}
	else
	{
		Dialogo=gtk_dialog_new_with_buttons("F.C.",NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_OK,NULL);
		gtk_window_set_icon(GTK_WINDOW(Dialogo),Immagine.logo);
		switch(r)
		{
			case 1:
				Label=gtk_label_new("Oro insufficente!!");
				break;
			case 2:
				Label=gtk_label_new("Cibo insufficente!");
				break;
			case 3:
				Label=gtk_label_new("Smeraldi insufficenti!!");
				break;
			default:
				Label=gtk_label_new("Errore! questo messaggio non deve comparire!");
				break;
		}
		gtk_widget_show(Label);
		gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),Label, TRUE, TRUE, 0);
		gtk_dialog_run(GTK_DIALOG(Dialogo));
		gtk_widget_destroy(Dialogo);
	}
}

static void evacua_truppa(t_lista_t *T)
{
	int Occupata;
	
	Occupata=evacuatruppa(T);
	if(Occupata)
	{
		return;
	}
	else
	{
		gtk_aggiorna_tab_armate();
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx,cy,'n');
	}
}

static void click_bersaglio(char* pos)
{
	int Dst=(int)(pos-infomappa.mappa);
	int Src= Mossa;
	int pdst, psrc;
	
	if(Src==Dst)
	{
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx,cy,'n');
	}
	else
	{
		pdst=infomappa.truppe[Dst]->numero;
		psrc=infomappa.truppe[Src]->numero;
		combatticampoaperto(Dst,Src);
		gtk_aggiorna_tab_armate();
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx,cy,'n');
		pdst-=(infomappa.truppe[Dst]!=NULL)?(infomappa.truppe[Dst]->numero):(0);
		psrc-=(infomappa.truppe[Src]!=NULL)?(infomappa.truppe[Src]->numero):(0);
		gtk_popup_combattimento(Casella[Dst%LARGHEZZA-cx+caselle_orizzontali*(Dst/LARGHEZZA-cy)], pdst);
		gtk_popup_combattimento(Casella[Src%LARGHEZZA-cx+caselle_orizzontali*(Src/LARGHEZZA-cy)], psrc);
	}
}

static void click_destinazione(char* pos)
{
	int Dst=(int)(pos-infomappa.mappa);
	int Src= Mossa;
	
	if(Src==Dst)
	{
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx,cy,'n');
		return;
	}
	else
	{
		spostatruppa(Src,Dst);
		gtk_aggiorna_tab_armate();
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx,cy,'n');
	}
}

static void annulla_mossa()
{
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx,cy,'n');
	return;
}

static void click_castello(char* pos)
{
	GtkWidget *menu;
	GtkWidget *scelta;
	GtkWidget *lista;
	GtkWidget *oggetto;
	t_lista_s *S;
	t_lista_t *T;
	char buf[50];
	
	aggiorna_tr_callback(pos);
	/*crea menu*/
	menu=gtk_menu_new();
	/*etichetta addestra*/
	scelta=gtk_menu_item_new_with_label("Addestra:");
	/*crea pulsanti per addestrare unità*/
	lista=gtk_menu_new();
	//reclute
	oggetto=gtk_menu_item_new_with_label("Reclute");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(addestra_truppa),(gpointer)&tr_callback[Rec]);
	gtk_widget_show(oggetto);
	//fanteria
	oggetto=gtk_menu_item_new_with_label("Fanteria");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(addestra_truppa),(gpointer)&tr_callback[Fan]);
	gtk_widget_show(oggetto);
	//lanceri
	oggetto=gtk_menu_item_new_with_label("Lancieri");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(addestra_truppa),(gpointer)&tr_callback[Lan]);
	gtk_widget_show(oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
	gtk_widget_show(scelta);
	/*etichetta evacua*/
	scelta=gtk_menu_item_new_with_label("Evacua:");
	/*crea pulsanti per evacuare unità*/
	lista=gtk_menu_new();
	//inizializza la lista truppe
	S=puntastruttura(pos-infomappa.mappa);
	T=S->in;
	//lista le unità nella struttura
	while(T!=NULL)
	{
		identificatruppa(T->truppa, buf);
		oggetto=gtk_menu_item_new_with_label(buf);
		gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
		g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(evacua_truppa),(gpointer)T);
		gtk_widget_show(oggetto);
		T=T->next;
	}
	//attacca la lista alla scelta
	if(S->in!=NULL)
	{
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
		gtk_widget_show(scelta);
	}
	//mostra il menu
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,1, gtk_get_current_event_time());
}

static void click_scuderia(char* pos)
{
	GtkWidget *menu;
	GtkWidget *scelta;
	GtkWidget *lista;
	GtkWidget *oggetto;
	t_lista_s *S;
	t_lista_t *T;
	char buf[50];
	
	aggiorna_tr_callback(pos);
	/*crea menu*/
	menu=gtk_menu_new();
	/*etichetta addestra*/
	scelta=gtk_menu_item_new_with_label("Addestra:");
	/*crea pulsanti per addestrare unità*/
	lista=gtk_menu_new();
	//reclute
	oggetto=gtk_menu_item_new_with_label("Reclute");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(addestra_truppa),(gpointer)&tr_callback[Rec]);
	gtk_widget_show(oggetto);
	//Cavalleria
	oggetto=gtk_menu_item_new_with_label("Cavalleria");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(menuitem_response),(gpointer)NULL);
	gtk_widget_show(oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
	gtk_widget_show(scelta);
	/*etichetta evacua*/
	scelta=gtk_menu_item_new_with_label("Evacua:");
	/*crea pulsanti per evacuare unità*/
	lista=gtk_menu_new();
	//inizializza la lista truppe
	S=puntastruttura(pos-infomappa.mappa);
	T=S->in;
	//lista le unità nella struttura
	while(T!=NULL)
	{
		identificatruppa(T->truppa, buf);
		oggetto=gtk_menu_item_new_with_label(buf);
		gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
		g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(evacua_truppa),(gpointer)T);
		gtk_widget_show(oggetto);
		T=T->next;
	}
	//attacca la lista alla scelta
	if(S->in!=NULL)
	{
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
		gtk_widget_show(scelta);
	}
	//mostra il menu
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,1, gtk_get_current_event_time());
}

static void click_fattoria(char* pos)
{
	GtkWidget *menu;
	GtkWidget *scelta;
	GtkWidget *lista;
	GtkWidget *oggetto;
	t_lista_s *S;
	t_lista_t *T;
	char buf[50];
	
	aggiorna_tr_callback(pos);
	/*crea menu*/
	menu=gtk_menu_new();
	/*etichetta addestra*/
	scelta=gtk_menu_item_new_with_label("Addestra:");
	/*crea pulsanti per addestrare unità*/
	lista=gtk_menu_new();
	//reclute
	oggetto=gtk_menu_item_new_with_label("Reclute");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(addestra_truppa),(gpointer)&tr_callback[Rec]);
	gtk_widget_show(oggetto);
	//fanteria
	oggetto=gtk_menu_item_new_with_label("Arceri");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(menuitem_response),(gpointer)NULL);
	gtk_widget_show(oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
	gtk_widget_show(scelta);
	/*etichetta evacua*/
	scelta=gtk_menu_item_new_with_label("Evacua:");
	/*crea pulsanti per evacuare unità*/
	lista=gtk_menu_new();
	//inizializza la lista truppe
	S=puntastruttura(pos-infomappa.mappa);
	T=S->in;
	//lista le unità nella struttura
	while(T!=NULL)
	{
		identificatruppa(T->truppa, buf);
		oggetto=gtk_menu_item_new_with_label(buf);
		gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
		g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(evacua_truppa),(gpointer)T);
		gtk_widget_show(oggetto);
		T=T->next;
	}
	//attacca la lista alla scelta
	if(S->in!=NULL)
	{
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
		gtk_widget_show(scelta);
	}
	//mostra il menu
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,1, gtk_get_current_event_time());
}

static void click_grotta(char* pos)
{
	GtkWidget *menu;
	GtkWidget *scelta;
	GtkWidget *lista;
	GtkWidget *oggetto;
	t_lista_s *S;
	t_lista_t *T;
	char buf[50];
	
	aggiorna_tr_callback(pos);
	/*crea menu*/
	menu=gtk_menu_new();
	/*etichetta addestra*/
	scelta=gtk_menu_item_new_with_label("Addestra:");
	/*crea pulsanti per addestrare unità*/
	lista=gtk_menu_new();
	//Draghi
	oggetto=gtk_menu_item_new_with_label("Draghi");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(menuitem_response),(gpointer)NULL);
	gtk_widget_show(oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
	gtk_widget_show(scelta);
	/*etichetta evacua*/
	scelta=gtk_menu_item_new_with_label("Evacua:");
	/*crea pulsanti per evacuare unità*/
	lista=gtk_menu_new();
	//inizializza la lista truppe
	S=puntastruttura(pos-infomappa.mappa);
	T=S->in;
	//lista le unità nella struttura
	while(T!=NULL)
	{
		identificatruppa(T->truppa, buf);
		oggetto=gtk_menu_item_new_with_label(buf);
		gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
		g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(evacua_truppa),(gpointer)T);
		gtk_widget_show(oggetto);
		T=T->next;
	}
	//attacca la lista alla scelta
	if(S->in!=NULL)
	{
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
		gtk_widget_show(scelta);
	}
	//mostra il menu
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,1, gtk_get_current_event_time());
}

static void click_nido(char* pos)
{
	GtkWidget *menu;
	GtkWidget *scelta;
	GtkWidget *lista;
	GtkWidget *oggetto;
	t_lista_s *S;
	t_lista_t *T;
	char buf[50];
	
	aggiorna_tr_callback(pos);
	/*crea menu*/
	menu=gtk_menu_new();
	/*etichetta addestra*/
	scelta=gtk_menu_item_new_with_label("Addestra:");
	/*crea pulsanti per addestrare unità*/
	lista=gtk_menu_new();
	//Fenici
	oggetto=gtk_menu_item_new_with_label("Fenici");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(menuitem_response),(gpointer)NULL);
	gtk_widget_show(oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
	gtk_widget_show(scelta);
	/*etichetta evacua*/
	scelta=gtk_menu_item_new_with_label("Evacua:");
	/*crea pulsanti per evacuare unità*/
	lista=gtk_menu_new();
	//inizializza la lista truppe
	S=puntastruttura(pos-infomappa.mappa);
	T=S->in;
	//lista le unità nella struttura
	while(T!=NULL)
	{
		identificatruppa(T->truppa, buf);
		oggetto=gtk_menu_item_new_with_label(buf);
		gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
		g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(evacua_truppa),(gpointer)T);
		gtk_widget_show(oggetto);
		T=T->next;
	}
	//attacca la lista alla scelta
	if(S->in!=NULL)
	{
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
		gtk_widget_show(scelta);
	}
	//mostra il menu
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,1, gtk_get_current_event_time());
}

static void click_unita(char* pos, GdkEventButton *Event)
{
	GtkWidget* Dialogo;
	GtkWidget* Label;
	t_infotruppa* T;
	int Pos=(int)(pos-infomappa.mappa);
	
	T=infomappa.truppe[Pos];
	if(Event-> button==3)
	{
		if(T->combattuto==0)
		{
			Mossa=Pos;
			gtk_pulisci_mappa();
			gtk_stampa_mappa(cx,cy,'c');
			return;
		}
		else
		{
			Dialogo=gtk_dialog_new_with_buttons("F.C.",NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_OK,NULL);
			gtk_window_set_icon(GTK_WINDOW(Dialogo),Immagine.logo);
			Label=gtk_label_new("unità in combattimento!");
			gtk_widget_show(Label);
			gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),Label, TRUE, TRUE, 0);
			gtk_dialog_run(GTK_DIALOG(Dialogo));
			gtk_widget_destroy(Dialogo);
			return;
		}
	}
	else
	{
		if(T->stanca==0 && T->combattuto==0)
		{
			Mossa=Pos;
			gtk_pulisci_mappa();
			gtk_stampa_mappa(cx,cy,'s');
			return;
		}
		else
		{
			Dialogo=gtk_dialog_new_with_buttons("F.C.",NULL,GTK_DIALOG_DESTROY_WITH_PARENT,GTK_STOCK_OK,NULL);
			gtk_window_set_icon(GTK_WINDOW(Dialogo),Immagine.logo);
			if(T->stanca==1)Label=gtk_label_new("unità stanca!");
			else Label=gtk_label_new("unità in combattimento!");
			gtk_widget_show(Label);
			gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),Label, TRUE, TRUE, 0);
			gtk_dialog_run(GTK_DIALOG(Dialogo));
			gtk_widget_destroy(Dialogo);
			return;
		}
	}
}

static void su_unita(GtkWidget *Truppa, GdkEvent *event, char* pos)
{
	int Pos=(int)(pos-infomappa.mappa);
	t_infotruppa* T;
	char buf[30];
	
	if(event!=NULL)
	{
		T=infomappa.truppe[Pos];
		identificatruppa(T, buf);
		gtk_widget_set_tooltip_text(Truppa,buf);
	}
}

static void click_assediocastello(char* pos)
{
	int Pos=(int)(pos-infomappa.mappa);
	
	assediocastello(Pos);
	gtk_aggiorna_tab_armate();
	gtk_aggiorna_tab_castelli();
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx,cy,'n');
}

static void click_assaltostruttura(char* pos)
{
	int Pos=(int)(pos-infomappa.mappa);
	
	assediostruttura(Pos);
	gtk_aggiorna_tab_strutture();
	gtk_aggiorna_tab_armate();
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx,cy,'n');
}

static void click_entrastruttura(char* pos)
{
	int Pos=(int)(pos-infomappa.mappa);
	
	spostainstruttura(Mossa, Pos);
	gtk_aggiorna_tab_armate();
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx,cy,'n');
}

static void click_unisci(char* pos)
{
	int Pos=(int)(pos-infomappa.mappa);
	int max, somma, min;
	const int a=0,b=1;
	GtkWidget * Dialogo;
	GtkWidget * Vbox;
	GtkWidget * Label;
	GtkWidget * Spin1;
	GtkWidget * Spin2;
	GtkAdjustment *UA, *UB;
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
		default:
			max=100;
			break;
	}
	somma=TA->numero+TB->numero;
	min=(somma-max)>0?(somma-max):0;
	UA=(GtkAdjustment*)gtk_adjustment_new(TA->numero, min,max<somma?max:somma, 1, 0, 0);
	UB=(GtkAdjustment*)gtk_adjustment_new(TB->numero, min,max<somma?max:somma, 1, 0, 0);
	S_Callback[a].A=UA;
	S_Callback[a].somma=somma;
	S_Callback[b].A=UB;
	S_Callback[b].somma=somma;
	g_signal_connect(UA, "value_changed", G_CALLBACK(set_adjustmentvalue), &S_Callback[b]);
	g_signal_connect(UB, "value_changed", G_CALLBACK(set_adjustmentvalue), &S_Callback[a]);
	Dialogo=gtk_dialog_new_with_buttons("Fantasy C",NULL,GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK,GTK_RESPONSE_YES,GTK_STOCK_CANCEL,GTK_RESPONSE_NO,NULL);
	gtk_window_set_icon(GTK_WINDOW(Dialogo),Immagine.logo);
	Vbox=gtk_vbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),Vbox, TRUE, TRUE, 0);
	gtk_widget_show(Vbox);
	Label=gtk_label_new("Sposta effettivi");
	gtk_box_pack_start(GTK_BOX(Vbox),Label, TRUE, TRUE, 0);
	gtk_widget_show(Label);
	Spin1=gtk_spin_button_new(GTK_ADJUSTMENT(UA), 0, 0);
	gtk_box_pack_start(GTK_BOX(Vbox),Spin1, TRUE, TRUE, 0);
	gtk_widget_show(Spin1);
	Spin2=gtk_spin_button_new(GTK_ADJUSTMENT(UB), 0, 0);
	gtk_box_pack_start(GTK_BOX(Vbox),Spin2, TRUE, TRUE, 0);
	gtk_widget_show(Spin2);
	if(gtk_dialog_run(GTK_DIALOG(Dialogo))==GTK_RESPONSE_YES)
	{
		TB->stanca=1;
		TA->numero=gtk_adjustment_get_value(GTK_ADJUSTMENT(UA));
		TB->numero=gtk_adjustment_get_value(GTK_ADJUSTMENT(UB));
		if(TA->numero==0)
			eliminamorti(TA);
		if(TB->numero==0)
			eliminamorti(TB);
		gtk_widget_destroy(Dialogo);
	}
	else
	{
		TA->stanca=0;
		gtk_widget_destroy(Dialogo);
	}
	gtk_pulisci_mappa();
	gtk_aggiorna_tab_armate();
	gtk_stampa_mappa(cx,cy,'n');
}

// funzioni grafiche

//inizializza le strutture dati dei tab
void gtk_inizializza_widget()
{
	int i;
	
	for(i=0; i<NUMCASTELLI; i++)Listacastelli[i]=NULL;
	Listastrutture=NULL;
	Listatruppe=NULL;
}

//carica le immagini del gioco
void gtk_carica_immagini()
{
	int i,j;
	char Buf[64];
	char Buf2[64];
	
	//carico il file di configurazione
	caricaconfig("fantasy.config");
	//carico immagini del gioco
	sprintf(Buf,"img/fantasy-icon.xpm");
	if((Immagine.err=gdk_pixbuf_new_from_file_at_size(Buf,Dim_casella,Dim_casella,NULL)) == NULL)
	{
		perror("impossibile caricare l'immagine");
		exit(1);
	}
	if((Immagine.logo=gdk_pixbuf_new_from_file_at_size(Buf,Dim_casella,Dim_casella,NULL)) == NULL)
	{
		perror("impossibile caricare l'immagine");
		exit(1);
	}
	if((Immagine.decorazione=gdk_pixbuf_new_from_file_at_size(Buf,Dim_casella/2,Dim_casella/2,NULL)) == NULL)
	{
		perror("impossibile caricare l'immagine");
		exit(1);
	}
	sprintf(Buf,"img/freccia.xpm");
	if((Immagine.freccia=gdk_pixbuf_new_from_file_at_size(Buf,Dim_casella-7,Dim_casella-7,NULL)) == NULL)
	{
		perror("impossibile caricare l'immagine");
		exit(1);
	}
	// carico immagini da skin
	sprintf(Buf,"skin/");
	strcat(Buf,infogioco.skin);
	strcat(Buf,"/mov.");
	strcat(Buf,infogioco.ext);
	if((Immagine.movimento=gdk_pixbuf_new_from_file_at_size(Buf,Dim_casella,Dim_casella,NULL)) == NULL)
	{
		perror("impossibile caricare l'immagine");
		exit(1);
	}
	sprintf(Buf,"skin/");
	strcat(Buf,infogioco.skin);
	strcat(Buf,"/atk.");
	strcat(Buf,infogioco.ext);
	if((Immagine.attacco=gdk_pixbuf_new_from_file_at_size(Buf,Dim_casella,Dim_casella,NULL)) == NULL)
	{
		perror("impossibile caricare l'immagine");
		exit(1);
	}
	
	//carica le immagini del prato
	for(i=0; i<5; i++)
	{
		sprintf(Buf2,"/p/p%d.",i);
		sprintf(Buf,"skin/");
		strcat(Buf,infogioco.skin);
		strcat(Buf,Buf2);
		strcat(Buf,infogioco.ext); // non esistono XPM ancora. colossale standardizzazione richiesta per le skin.
		if((Immagine.p[i]=gdk_pixbuf_new_from_file_at_size(Buf,Dim_casella,Dim_casella,NULL)) == NULL)
		{
			perror("impossibile caricare l'immagine");
			exit(1);
		}
	}
	
	//carica gli scudi colorati
	for(i=0; i<MAXGIOCATORI; i++)
	{
		sprintf(Buf2,"/a/%d.",i);
		sprintf(Buf,"skin/");
		strcat(Buf,infogioco.skin);
		strcat(Buf,Buf2);
		strcat(Buf,infogioco.ext);
		if((Immagine.a[i]=gdk_pixbuf_new_from_file_at_size(Buf,Dim_casella/2,Dim_casella/2,NULL)) == NULL)
		{
			perror("impossibile caricare l'immagine");
			exit(1);
		}
	}
	//carica il castello
	for(i=0;i<=MAXGIOCATORI;i++)
		for(j=0; j<9; j++)
		{
			sprintf(Buf2,"/c/%d/c%d.",i,j);
			sprintf(Buf,"skin/");
			strcat(Buf,infogioco.skin);
			strcat(Buf,Buf2);
			strcat(Buf,infogioco.ext);
			if((Immagine.c[i][j]=gdk_pixbuf_new_from_file_at_size(Buf,Dim_casella,Dim_casella,NULL)) == NULL)
			{
				perror("impossibile caricare l'immagine");
				exit(1);
			}
		}
	//carica la grotta
	for(i=0;i<=MAXGIOCATORI;i++)
		for(j=0; j<4; j++)
		{
			sprintf(Buf2,"/g/%d/g%d.",i,j);
			sprintf(Buf,"skin/");
			strcat(Buf,infogioco.skin);
			strcat(Buf,Buf2);
			strcat(Buf,infogioco.ext);
			if((Immagine.g[i][j]=gdk_pixbuf_new_from_file_at_size(Buf,Dim_casella,Dim_casella,NULL)) == NULL)
			{
				perror("impossibile caricare l'immagine");
				exit(1);
			}
		}
	//carica la stalla
	for(i=0;i<=MAXGIOCATORI;i++)
		for(j=0; j<4; j++)
		{
			sprintf(Buf2,"/s/%d/s%d.",i,j);
			sprintf(Buf,"skin/");
			strcat(Buf,infogioco.skin);
			strcat(Buf,Buf2);
			strcat(Buf,infogioco.ext);
			if((Immagine.s[i][j]=gdk_pixbuf_new_from_file_at_size(Buf,Dim_casella,Dim_casella,NULL)) == NULL)
			{
				perror("impossibile caricare l'immagine");
				exit(1);
			}
		}
	//carica la fattoria
	for(i=0;i<=MAXGIOCATORI;i++)
		for(j=0; j<4; j++)
		{
			sprintf(Buf2,"/f/%d/f%d.",i,j);
			sprintf(Buf,"skin/");
			strcat(Buf,infogioco.skin);
			strcat(Buf,Buf2);
			strcat(Buf,infogioco.ext);
			if((Immagine.f[i][j]=gdk_pixbuf_new_from_file_at_size(Buf,Dim_casella,Dim_casella,NULL)) == NULL)
			{
				perror("impossibile caricare l'immagine");
				exit(1);
			}
		}
	//carica il nido
	for(i=0;i<MAXGIOCATORI;i++)
		for(j=0; j<4; j++)
		{
			sprintf(Buf2,"/n/%d/n%d.",i,j);
			sprintf(Buf,"skin/");
			strcat(Buf,infogioco.skin);
			strcat(Buf,Buf2);
			strcat(Buf,infogioco.ext);
			if((Immagine.n[i][j]=gdk_pixbuf_new_from_file_at_size(Buf,Dim_casella,Dim_casella,NULL)) == NULL)
			{
				perror("impossibile caricare l'immagine");
				exit(1);
			}
		}
	//carica le truppe
	for(i=0; i<MAXGIOCATORI; i++)
		for(j=0; j<=Lan; j++)
		{
			sprintf(Buf2,"/t/t%d%d.",i,j);
			sprintf(Buf,"skin/");
			strcat(Buf,infogioco.skin);
			strcat(Buf,Buf2);
			strcat(Buf,infogioco.ext);
			if((Immagine.t[i][j]=gdk_pixbuf_new_from_file_at_size(Buf,Dim_casella,Dim_casella,NULL)) == NULL)
			{
				perror("impossibile caricare l'immagine");
				exit(1);
			}
		}
}

//calcola le dimensioni dei widget
void gtk_calcola_dimensioni()
{
	int w,h;
	int wof, hof;
	
	w=gdk_screen_get_width(gdk_screen_get_default());
	h=gdk_screen_get_height(gdk_screen_get_default());
	Dim_casella=0.045*MIN(w,h)+3;
	wof=4*Dim_casella;
	hof=7*Dim_casella;
	caselle_orizzontali=(w-wof)/Dim_casella; // 27 eee
	caselle_verticali=(h-hof)/Dim_casella; // 16 eee
//	if(h>=960) Dim_casella=40;
//	else if(h>=720) Dim_casella=30;
//	else if(h>=480) Dim_casella=20;
//	else Dim_casella=10;
	printf("\nCasella: %dpx\nOrizzontali: %d Verticali: %d\n",Dim_casella,caselle_orizzontali,caselle_verticali);
}

//crea il menu principale
void gtk_crea_menu(GtkWidget *Vbox)
{
	GtkWidget *menu;
	GtkWidget *oggetto_menu;
	GtkWidget *barra_menu;
	GtkWidget *menu_file, *menu_modifica, *menu_help;
	
	//genera menu file
	menu=gtk_menu_new();
	// 	genera scelta 1
	oggetto_menu=gtk_menu_item_new_with_mnemonic("_Nuova Partita");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), oggetto_menu);
	g_signal_connect(oggetto_menu, "activate", G_CALLBACK(nuova_partita),(gpointer)NULL);
	gtk_widget_show(oggetto_menu);
	// 	genera scelta 2
	oggetto_menu=gtk_menu_item_new_with_mnemonic("_Carica");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), oggetto_menu);
	g_signal_connect_swapped(oggetto_menu, "activate", G_CALLBACK(salva_carica),(gpointer)0);
	gtk_widget_show(oggetto_menu);
	// 	genera scelta 3
	oggetto_menu=gtk_menu_item_new_with_mnemonic("_Salva");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), oggetto_menu);
	g_signal_connect_swapped(oggetto_menu, "activate", G_CALLBACK(salva_carica),(gpointer)1);
	gtk_widget_show(oggetto_menu);
	// 	genera scelta 4
	oggetto_menu=gtk_menu_item_new_with_mnemonic("_Esci");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), oggetto_menu);
	g_signal_connect(oggetto_menu, "activate", G_CALLBACK(chiudi_da_menu),(gpointer)NULL);
	gtk_widget_show(oggetto_menu);
	// 	genera etichetta file 
	menu_file=gtk_menu_item_new_with_mnemonic("_File");
	gtk_widget_show(menu_file);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_file), menu);
	//  genera menu modifica
	menu=gtk_menu_new();
	//  genera scelta 1
	oggetto_menu=gtk_menu_item_new_with_mnemonic("_Preferenze");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), oggetto_menu);
	g_signal_connect(oggetto_menu, "activate", G_CALLBACK(preferenze),(gpointer)NULL);
	gtk_widget_show(oggetto_menu);
	//  genera etichetta modifica
	menu_modifica=gtk_menu_item_new_with_mnemonic("_Modifica");
	gtk_widget_show(menu_modifica);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_modifica), menu);
	// 	genera menu help 
	menu=gtk_menu_new();
	// 	genera scelta 1
	oggetto_menu=gtk_menu_item_new_with_label("Manuale");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), oggetto_menu);
	g_signal_connect(oggetto_menu, "activate", G_CALLBACK(menuitem_response),(gpointer)NULL);
	gtk_widget_show(oggetto_menu);
	// 	genera scelta 2
	oggetto_menu=gtk_menu_item_new_with_label("Crediti");
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), oggetto_menu);
	g_signal_connect(oggetto_menu, "activate", G_CALLBACK(menuitem_response),(gpointer)NULL);
	gtk_widget_show(oggetto_menu);
	// 	genera etichetta help
	menu_help=gtk_menu_item_new_with_mnemonic("_Aiuto");
	gtk_widget_show(menu_help);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_help), menu);
	// 	genera barra dei menu 
	barra_menu=gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(Vbox), barra_menu, FALSE, FALSE, 0);
	gtk_widget_show(barra_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(barra_menu), menu_file);
	gtk_menu_shell_append(GTK_MENU_SHELL(barra_menu), menu_modifica);
	gtk_menu_shell_append(GTK_MENU_SHELL(barra_menu), menu_help);
	// 	fine menu 
}

//pulisce la mappa dalle immagini e dai segnali
void gtk_pulisci_mappa()
{
	int Pos;
	
	if(partita_in_corso==1)
		for(Pos=0; Pos<caselle_orizzontali*caselle_verticali; Pos++)
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
			g_signal_handlers_disconnect_matched(Casella[Pos],G_SIGNAL_MATCH_FUNC,0,0,0, su_unita, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],G_SIGNAL_MATCH_FUNC,0,0,0, annulla_mossa, 0);
			gtk_widget_destroy(Thumb[Pos]);
		}
}

//elimina le caselle dalla tabella
void gtk_pulisci_caselle()
{
	int Pos;
	
	if(partita_in_corso==1)
		for(Pos=0; Pos<caselle_orizzontali*caselle_verticali; Pos++)
		{
			gtk_widget_destroy(Casella[Pos]);
		}
}

//stampa la mappa
void gtk_stampa_mappa(int x, int y, char m)
{
	static char pre='n';
	int C;
	int R;
	int Pos=0;
	int G;
	int Mx=0;
	int i,j;
	int Q;
	int **V=NULL;
	char **Graph=NULL;
	char buf[10];
	char vel;
	GdkPixbuf *TmpB;
	
	sprintf(buf,"(%d|%d)",x+caselle_orizzontali/2-1,y+caselle_verticali/2-1);
	gtk_label_set_text(GTK_LABEL(Coordinate),buf);
	//gestisce memorizzazione del modo stampa
	if(m=='p')m=pre;
	else pre=m;
	//lancia il dijkstra se si sta muovendo una unità
	if(m=='s')
	{
		Q=inizializza_dijkstra(Mossa,&Graph,&V,&Mx,&vel);
		calcola_dijkstra(Graph,Mx,vel,V,Q);
		for(i=0;i<Mx;i++)
		{
			for(j=0;j<Mx;j++)
			{
				printf("%2d ",V[i][j]/100);
			}
			if(i/Mx==0)printf("(i=%d j=%d)\n",i,j);
		}

	}
	//stampa ogni casella
	for(R=y;R<y+caselle_verticali;R++)
		for(C=x;C<x+caselle_orizzontali;C++)
		{
			TmpB=gdk_pixbuf_copy(Immagine.p[tipoprato(posiziona(0,0,C,R))]);
			switch(accedi(C,R,infomappa.mappa))
			{
				/*stampa il castello*/
				case '0':
					G=controlloedificio(posiziona(-1,-1,C,R), Cas);
					gdk_pixbuf_composite(Immagine.c[G+1][0],TmpB,0,0,Dim_casella,Dim_casella,0,0,1,1,GDK_INTERP_BILINEAR,255);
					Thumb[Pos]=gtk_image_new_from_pixbuf(TmpB);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_castello),(gpointer)&infomappa.mappa[posiziona(-1,-1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(-1,-1,C,R),Cas)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assediocastello),(gpointer)&infomappa.mappa[posiziona(-1,-1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(-1,-1,C,R),Cas)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(-1,-1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '1':
					G=controlloedificio(posiziona(0,-1,C,R), Cas);
					gdk_pixbuf_composite(Immagine.c[G+1][1],TmpB,0,0,Dim_casella,Dim_casella,0,0,1,1,GDK_INTERP_BILINEAR,255);
					Thumb[Pos]=gtk_image_new_from_pixbuf(TmpB);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
 					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_castello),(gpointer)&infomappa.mappa[posiziona(0,-1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,-1,C,R),Cas)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assediocastello),(gpointer)&infomappa.mappa[posiziona(0,-1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(0,-1,C,R),Cas)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(0,-1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '2':
					G=controlloedificio(posiziona(1,-1,C,R), Cas);
					gdk_pixbuf_composite(Immagine.c[G+1][2],TmpB,0,0,Dim_casella,Dim_casella,0,0,1,1,GDK_INTERP_BILINEAR,255);
					Thumb[Pos]=gtk_image_new_from_pixbuf(TmpB);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_castello),(gpointer)&infomappa.mappa[posiziona(1,-1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,-1,C,R),Cas)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assediocastello),(gpointer)&infomappa.mappa[posiziona(1,-1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(1,-1,C,R),Cas)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(1,-1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '3':
					G=controlloedificio(posiziona(-1,0,C,R), Cas);
					gdk_pixbuf_composite(Immagine.c[G+1][3],TmpB,0,0,Dim_casella,Dim_casella,0,0,1,1,GDK_INTERP_BILINEAR,255);
					Thumb[Pos]=gtk_image_new_from_pixbuf(TmpB);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_castello),(gpointer)&infomappa.mappa[posiziona(-1,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(-1,0,C,R),Cas)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assediocastello),(gpointer)&infomappa.mappa[posiziona(-1,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(-1,0,C,R),Cas)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(-1,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '4':
					G=controlloedificio(posiziona(0,0,C,R), Cas);
					gdk_pixbuf_composite(Immagine.c[G+1][4],TmpB,0,0,Dim_casella,Dim_casella,0,0,1,1,GDK_INTERP_BILINEAR,255);
					Thumb[Pos]=gtk_image_new_from_pixbuf(TmpB);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_castello),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '5':
					G=controlloedificio(posiziona(1,0,C,R), Cas);
					gdk_pixbuf_composite(Immagine.c[G+1][5],TmpB,0,0,Dim_casella,Dim_casella,0,0,1,1,GDK_INTERP_BILINEAR,255);
					Thumb[Pos]=gtk_image_new_from_pixbuf(TmpB);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_castello),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Cas)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assediocastello),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Cas)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '6':
					G=controlloedificio(posiziona(-1,1,C,R), Cas);
					gdk_pixbuf_composite(Immagine.c[G+1][6],TmpB,0,0,Dim_casella,Dim_casella,0,0,1,1,GDK_INTERP_BILINEAR,255);
					Thumb[Pos]=gtk_image_new_from_pixbuf(TmpB);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_castello),(gpointer)&infomappa.mappa[posiziona(-1,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(-1,1,C,R),Cas)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assediocastello),(gpointer)&infomappa.mappa[posiziona(-1,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(-1,1,C,R),Cas)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(-1,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '7':
					G=controlloedificio(posiziona(0,1,C,R), Cas);
					gdk_pixbuf_composite(Immagine.c[G+1][7],TmpB,0,0,Dim_casella,Dim_casella,0,0,1,1,GDK_INTERP_BILINEAR,255);
					Thumb[Pos]=gtk_image_new_from_pixbuf(TmpB);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_castello),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Cas)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assediocastello),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Cas)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case '8':
					G=controlloedificio(posiziona(1,1,C,R), Cas);
					gdk_pixbuf_composite(Immagine.c[G+1][8],TmpB,0,0,Dim_casella,Dim_casella,0,0,1,1,GDK_INTERP_BILINEAR,255);
					Thumb[Pos]=gtk_image_new_from_pixbuf(TmpB);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_castello),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Cas)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assediocastello),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Cas)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				/*stampa la grotta*/
				case 'G':
					G=controlloedificio(posiziona(0,0,C,R),Gro);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.g[G+1][0]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_grotta),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Gro)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Gro)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'H':
					G=controlloedificio(posiziona(1,0,C,R),Gro);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.g[G+1][1]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_grotta),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Gro)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Gro)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'I':
					G=controlloedificio(posiziona(0,1,C,R),Gro);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.g[G+1][2]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_grotta),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Gro)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Gro)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'J':
					G=controlloedificio(posiziona(1,1,C,R),Gro);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.g[G+1][3]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_grotta),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Gro)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Gro)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				/*stampa la fattoria*/
				case 'C':
					G=controlloedificio(posiziona(0,0,C,R),Fat);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.f[G+1][0]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_fattoria),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Fat)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Fat)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'D':
					G=controlloedificio(posiziona(1,0,C,R),Fat);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.f[G+1][1]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_fattoria),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Fat)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Fat)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'E':
					G=controlloedificio(posiziona(0,1,C,R),Fat);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.f[G+1][2]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_fattoria),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Fat)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Fat)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'F':
					G=controlloedificio(posiziona(1,1,C,R),Fat);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.f[G+1][3]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_fattoria),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Fat)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Fat)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				/*stampa la scuderia*/
				case 'S':
					G=controlloedificio(posiziona(0,0,C,R),Scu);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.s[G+1][0]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_scuderia),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Scu)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Scu)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'T':
					G=controlloedificio(posiziona(1,0,C,R),Scu);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.s[G+1][1]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_scuderia),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Scu)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Scu)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'U':
					G=controlloedificio(posiziona(0,1,C,R),Scu);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.s[G+1][2]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_scuderia),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Scu)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Scu)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'V':
					G=controlloedificio(posiziona(1,1,C,R),Scu);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.s[G+1][3]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_scuderia),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Scu)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Scu)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;				
				/*stampa in nido*/
				case 'N':
					G=controlloedificio(posiziona(0,0,C,R),Nid);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.n[G+1][0]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_nido),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Nid)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(0,0,C,R),Nid)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'O':
					G=controlloedificio(posiziona(1,0,C,R),Nid);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.n[G+1][1]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_nido),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Nid)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(1,0,C,R),Nid)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(1,0,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'P':
					G=controlloedificio(posiziona(0,1,C,R),Nid);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.n[G+1][2]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_nido),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Nid)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(0,1,C,R),Nid)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(0,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case 'Q':
					G=controlloedificio(posiziona(1,1,C,R),Nid);
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.n[G+1][3]);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					if(m=='n' && G==CurrentPlayer)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_nido),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='c' && assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Nid)==1)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_assaltostruttura),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1 && controllodiverso(Mossa,posiziona(1,1,C,R),Nid)==0)
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_entrastruttura),(gpointer)&infomappa.mappa[posiziona(1,1,C,R)]);
					gtk_widget_show(Thumb[Pos]);
					break;
				case ' ':
					if(accedi(C,R,infomappa.truppe)!=NULL)
					{
						G=controllounita(posiziona(0,0,C,R));
						switch(accedi(C,R,infomappa.truppe)->tipo)
						{
							case Rec:
								gdk_pixbuf_composite(Immagine.t[G][Rec],TmpB,0,0,Dim_casella,Dim_casella,0,0,1,1,GDK_INTERP_BILINEAR,255);
								Thumb[Pos]=gtk_image_new_from_pixbuf(TmpB);
								gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
								gtk_widget_show(Thumb[Pos]);
								break;
							case Fan:
								gdk_pixbuf_composite(Immagine.t[G][Fan],TmpB,0,0,Dim_casella,Dim_casella,0,0,1,1,GDK_INTERP_BILINEAR,255);
								Thumb[Pos]=gtk_image_new_from_pixbuf(TmpB);
								gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
								gtk_widget_show(Thumb[Pos]);
								break;
							case Lan:
								gdk_pixbuf_composite(Immagine.t[G][Lan],TmpB,0,0,Dim_casella,Dim_casella,0,0,1,1,GDK_INTERP_BILINEAR,255);
								Thumb[Pos]=gtk_image_new_from_pixbuf(TmpB);
								gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
								gtk_widget_show(Thumb[Pos]);
								break;
							default:
								Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.err);
								gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
								gtk_widget_show(Thumb[Pos]);
								break;
						}
						if(m=='n')
						{
							if(G==CurrentPlayer)
								g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_unita),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
							g_signal_connect(Casella[Pos], "enter-notify-event", G_CALLBACK(su_unita),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
						}
						else if(m=='c')
						{	
							if(bersagliolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiversotruppe(Mossa,posiziona(0,0,C,R))==1)
								g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_bersaglio),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
							else
							g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(annulla_mossa),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
						}
						else if(m=='s')
						{
							if(assaltolecito(Mossa,posiziona(0,0,C,R))==1 && controllodiversotruppe(Mossa,posiziona(0,0,C,R))==0 && tipouguale(C,R))
								g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_unisci),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
							else
							g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(annulla_mossa),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
						}
					}
					else if(m=='s' && spostalecito(Mossa,posiziona(0,0,C,R),V)==1)
					{
						Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.movimento);
						gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
						g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(click_destinazione),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
						gtk_widget_show(Thumb[Pos]);
					}
					else if(m=='c' && bersagliolecito(Mossa,posiziona(0,0,C,R))==1)
					{
						Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.attacco);
						gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
						gtk_widget_show(Thumb[Pos]);
					}
					else
					{
						Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.p[tipoprato(posiziona(0,0,C,R))]);
						gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
						if(m!='n')
							g_signal_connect_swapped(Casella[Pos], "button_press_event", G_CALLBACK(annulla_mossa),(gpointer)&infomappa.mappa[posiziona(0,0,C,R)]);
						gtk_widget_show(Thumb[Pos]);
					}
					break;
				default:
					Thumb[Pos]=gtk_image_new_from_pixbuf(Immagine.err);
					gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
					gtk_widget_show(Thumb[Pos]);
					break;
			}
			g_object_unref(TmpB);
			Pos++;
		}
	if(m=='s') // grazie dijkstra
	{
		for(i=0;i<Mx;i++)free(V[i]);
		free(V);
	}
}

//crea le caselle della tabella per la mappa
void gtk_genera_mappa(GtkWidget *Mappa)
{
	int i,j;
	
	for(i=0;i<caselle_orizzontali;i++)
		for(j=0; j<caselle_verticali; j++)
		{
			Casella[i+j*caselle_orizzontali]=gtk_event_box_new();
			gtk_widget_set_events(Casella[i+j*caselle_orizzontali], GDK_BUTTON_PRESS_MASK |  GDK_ENTER_NOTIFY_MASK);
			gtk_widget_set_size_request(Casella[i+j*caselle_orizzontali], Dim_casella,Dim_casella);
			gtk_table_attach(GTK_TABLE(Mappa), Casella[i+j*caselle_orizzontali], i, i+1, j, j+1,!GTK_EXPAND,!GTK_EXPAND,0,0);
			gtk_widget_show(Casella[i+j*caselle_orizzontali]);
			gtk_widget_realize(Casella[i+j*caselle_orizzontali]);
			
		}
}

//crea un tab del notebook
GtkWidget *gtk_crea_notebook_tab(GtkWidget *Notebook,char *buf)
{
	GtkWidget *Etichetta;
	GtkWidget *Vbox;
	GtkWidget *Scroller;
	
	Etichetta=gtk_label_new(buf);
	Scroller=gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(Scroller),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	Vbox=gtk_vbox_new(FALSE,0);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(Scroller), Vbox);
	gtk_widget_show(Vbox);
	gtk_widget_show(Etichetta);
	gtk_widget_show(Scroller);
	gtk_notebook_append_page(GTK_NOTEBOOK(Notebook),Scroller, Etichetta);
	return(GtkWidget *)Vbox;
}

//crea il notebook
void gtk_crea_notebook(GtkWidget *Frame)
{
	Notebook[0]=gtk_notebook_new();
	gtk_widget_set_size_request(Notebook[0], -1,3*Dim_casella-10);
	gtk_container_add(GTK_CONTAINER(Frame), Notebook[0]);
	Notebook[1]=(GtkWidget *)gtk_crea_notebook_tab(Notebook[0],"Castelli");
	Notebook[2]=(GtkWidget *)gtk_crea_notebook_tab(Notebook[0],"Strutture");
	Notebook[3]=(GtkWidget *)gtk_crea_notebook_tab(Notebook[0],"Armate");
}

//crea un elemento di un tab di un notebook
GtkWidget *gtk_crea_elemento_tab(GtkWidget *tab,int x, int y,char *nome, char *buf)
{
	GtkWidget *Hbox;
	GtkWidget *Vbox;
	GtkWidget *Label;
	GtkWidget *Button;
	GtkWidget *Separator;
	char buffer[10];
	
	if(tab==NULL)
	{
		printf("errore!");
		return NULL;
	}
	Vbox=gtk_vbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(tab), Vbox,FALSE, FALSE, 0);
	Separator=gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(Vbox), Separator,FALSE, FALSE, 3);
	gtk_widget_show(Separator);
	Hbox=gtk_hbox_new(TRUE,0);
	gtk_box_pack_start(GTK_BOX(Vbox), Hbox,FALSE, FALSE, 0);
	Label=gtk_label_new(nome);
	gtk_box_pack_start(GTK_BOX(Hbox),Label, FALSE, FALSE, 0);
	gtk_widget_show(Label);
	Label=gtk_label_new(buf);
	gtk_box_pack_start(GTK_BOX(Hbox),Label, FALSE, FALSE, 0);
	gtk_widget_show(Label);
	sprintf(buffer,"%d|%d",x,y);
	Button=gtk_button_new_with_label(buffer);
	g_signal_connect_swapped(Button, "button_press_event", G_CALLBACK(centra_mappa),(gpointer)&infomappa.mappa[x+y*LARGHEZZA]);
	gtk_box_pack_start(GTK_BOX(Hbox),Button, FALSE, FALSE, 0);
	gtk_widget_show(Button);
	gtk_widget_show(Hbox);
	gtk_widget_show(Vbox);
	return Vbox;
}

//riempe il tab castelli giocatore
GtkWidget * gtk_riempi_tab_castelli(int i, char* buf)
{
	int P;
	int x,y;
	GtkWidget *R;
	
	if(i!=0)
	{
	P=giocatore[CurrentPlayer]->struttura[Cas]->pos;
	x=P%LARGHEZZA+1;
	y=P/LARGHEZZA+1;
	R=gtk_crea_elemento_tab(Notebook[1],x,y,buf,"N°1");
	return R;
	}
	else return NULL;
}

//aggiorna il tab castelli
void gtk_aggiorna_tab_castelli()
{
	int i;
	int x,y;
	char nome[20];
	char buf[20];
	t_lista_s* S=giocatore[CurrentPlayer]->struttura[Cas];
	
	for(i=0; Listacastelli[i]!=NULL;i++)gtk_pulisci_tab(Listacastelli[i]);
	for(i=0; S!=NULL && i<12; i++)
	{
		x=S->pos%LARGHEZZA+1;
		y=S->pos/LARGHEZZA+1;
		switch(i)
		{
			case 0:
				sprintf(nome,"Capitale");
				break;
			default:
				sprintf(nome,"Città %d",i);
				break;
		}
		sprintf(buf,"N°%d", i+1);
		Listacastelli[i]=gtk_crea_elemento_tab(Notebook[1],x, y, nome,buf);
		S=S->next;
	}
}

//aggiorna il tab strutture
void gtk_aggiorna_tab_strutture()
{
	int i,j=0;
	int x,y;
	char nome[20];
	char buf[20];
	t_lista_s* S[NUMSTRUTTURE-1];
	t_listapertab* Tab;
	t_listapertab* Tabp;
	
	fprintf(stderr, "debug: gtk_aggiorna_tab_strutture\n");
	for(i=1; i<NUMSTRUTTURE;i++)S[i-1]=giocatore[CurrentPlayer]->struttura[i];
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
			j++;
			x=S[i]->pos%LARGHEZZA+1;
			y=S[i]->pos/LARGHEZZA+1;
			switch(i+1)
			{
				case Fat:
					sprintf(nome,"Fattoria");
					break;
				case Scu:
					sprintf(nome,"Scuderia");
					break;
				case Gro:
					sprintf(nome,"Grotta");
					break;
				case Nid:
					sprintf(nome,"Nido");
					break;
				default:
					sprintf(nome,"Errore");
					break;
			}
			Tabp=Tab;
			Tab=(t_listapertab*)malloc(sizeof(t_listapertab));
			if(Tabp==NULL)Listastrutture=Tab;
			else Tabp->next=Tab;
			sprintf(buf,"N°%d",j);
			Tab->P=gtk_crea_elemento_tab(Notebook[2],x, y, nome,buf);
			Tab->next=NULL;
			S[i]=S[i]->next;
		}
}

//aggiorna il tab armate
void gtk_aggiorna_tab_armate()
{
	int x,y;
	char nome[20];
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
				sprintf(nome,"Reclute");
				break;
			case Fan:
				sprintf(nome,"Fanteria");
				break;
			case Lan:
				sprintf(nome,"Lancieri");
				break;
			case Arc:
				sprintf(nome,"Arcieri");
				break;
			case Cav:
				sprintf(nome,"Cavalleria");
				break;
			case Dra:
				sprintf(nome,"Drago");
				break;
			case Fen:
				sprintf(nome,"Fenice");
				break;
			default:
				sprintf(nome,"Errore");
				break;
		}
		Tabp=Tab;
		Tab=(t_listapertab*)malloc(sizeof(t_listapertab));
		if(Tabp==NULL)Listatruppe=Tab;
		else Tabp->next=Tab;
		sprintf(buf,"-%d(%d)-",T->truppa->numero,T->truppa->morale);
		Tab->P=gtk_crea_elemento_tab(Notebook[3],x, y, nome,buf);
		Tab->next=NULL;
		T=T->next;
	}
}

//pulisce i tab
void gtk_pulisci_tab(GtkWidget *Target)
{
	if(Target==NULL)return;
	else gtk_widget_destroy(Target);
}

//carica un file di salvataggio all'avvio
int gtk_carica_avvio(char *nomefile)
{
	if(carica(nomefile))return 1;
	gtk_azzera_tab();
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx,cy, 'n');
	gtk_aggiorna_contarisorse();
	gtk_aggiorna_giocatore_c();
	gtk_aggiorna_tab_castelli();
	gtk_aggiorna_tab_strutture();
	gtk_aggiorna_tab_armate();
	partita_in_corso=1;
	
	return 0;
}

//azzera tab
void gtk_azzera_tab()
{
	int i;
	t_listapertab* Tab;
	t_listapertab* Tabp;
	
	for(i=0;i<NUMCASTELLI;i++)
	{
		gtk_pulisci_tab(Listacastelli[i]);
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
}

//crea 4 frecce direzionali
GtkWidget *gtk_crea_4_frecce()
{
	GtkWidget *Pulsantiera;
	GtkWidget *Pulsante;
	GtkWidget *Freccia;
	int i;
	
	Pulsantiera=gtk_table_new(3,3,FALSE);
	for(i=1; i<9;i=i+2)
	{
		Pulsante=gtk_button_new();
		gtk_widget_set_size_request(Pulsante, Dim_casella,Dim_casella);
		g_signal_connect_swapped(Pulsante, "button_press_event", G_CALLBACK(sposta_mappa),(gpointer)&infomappa.mappa[i]);
		gtk_table_attach_defaults(GTK_TABLE(Pulsantiera), Pulsante, i/3, i/3+1, i%3, i%3+1);
		switch(i)
		{
			case 1:
				Immagine.freccia=gdk_pixbuf_rotate_simple(Immagine.freccia,GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE);
				break;
			case 3:
				Immagine.freccia=gdk_pixbuf_rotate_simple(Immagine.freccia,GDK_PIXBUF_ROTATE_CLOCKWISE);
				break;
			case 5:
				Immagine.freccia=gdk_pixbuf_rotate_simple(Immagine.freccia,GDK_PIXBUF_ROTATE_UPSIDEDOWN);
				break;
			case 7:
				Immagine.freccia=gdk_pixbuf_rotate_simple(Immagine.freccia,GDK_PIXBUF_ROTATE_COUNTERCLOCKWISE);
				break;
			default:
				break;
		}
		Freccia=gtk_image_new_from_pixbuf(Immagine.freccia);
		gtk_container_add(GTK_CONTAINER(Pulsante), Freccia);
		gtk_widget_show(Freccia);
		gtk_widget_show(Pulsante);
	}
	Coordinate=gtk_label_new("(0|0)");
	gtk_widget_set_size_request(Coordinate, Dim_casella,Dim_casella);
	gtk_table_attach_defaults(GTK_TABLE(Pulsantiera), Coordinate, 1, 2, 1, 2);
	gtk_widget_show(Coordinate);
	return Pulsantiera;
}

//crea pulsanti editor per inserire strutture
GtkWidget *gtk_crea_pulsanti_editor_s()
{
	GtkWidget *Pulsantiera;
	GtkWidget *Pulsante;
	int i;
	
	Pulsantiera=gtk_table_new(3,2,TRUE);
	for(i=0; i<NUMSTRUTTURE;i++)
	{
		Pulsante=gtk_button_new();
		gtk_widget_set_size_request(Pulsante, Dim_casella,Dim_casella);
		g_signal_connect_swapped(Pulsante, "button_press_event", G_CALLBACK(menuitem_response),NULL);
		gtk_table_attach(GTK_TABLE(Pulsantiera), Pulsante, i%2, i%2+1, i/2, i/2+1,GTK_SHRINK,GTK_SHRINK,0,0);
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
	
	Pulsantiera=gtk_table_new(4,2,TRUE);
	for(i=0; i<NUMTRUPPE;i++)
	{
		Pulsante=gtk_button_new();
		gtk_widget_set_size_request(Pulsante, Dim_casella,Dim_casella);
		g_signal_connect_swapped(Pulsante, "button_press_event", G_CALLBACK(menuitem_response),NULL);
		gtk_table_attach(GTK_TABLE(Pulsantiera), Pulsante, i%2, i%2+1, i/2, i/2+1,GTK_SHRINK,GTK_SHRINK,0,0);
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
	
	Label[0]=gtk_label_new("Oro: ");
	Label[1]=gtk_label_new("Cibo: ");
	Label[2]=gtk_label_new("Smeraldi: ");
	Vbox=gtk_vbox_new(FALSE,5);
	for(i=0; i<NUMRISORSE; i++)
	{
		Hbox[i]=gtk_hbox_new(FALSE,0);
		gtk_box_pack_start(GTK_BOX(Vbox), Hbox[i], FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(Hbox[i]), Label[i], FALSE, FALSE, 0);
		gtk_widget_set_size_request(Label[i],2*Dim_casella,-1);
		Counter[i]=gtk_entry_new();
		gtk_editable_set_editable(GTK_EDITABLE(Counter[i]),FALSE);
		gtk_entry_set_text(GTK_ENTRY(Counter[i]),"0");
		gtk_entry_set_width_chars(GTK_ENTRY(Counter[i]),6);
		gtk_widget_set_size_request(Counter[i],Dim_casella,-1);
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
	char Buf[12];
	
	for(i=0; i<NUMRISORSE; i++)
	{
		n=(float)R[i];
		nk=0;
		while(n>999)
		{
			n=n/1000;
			nk++;
		}
		sprintf(Buf,"%.2f",n);
		for(j=0;nk>0 && j<6; j++, nk--) strcat(Buf,"k");
		gtk_entry_set_text(GTK_ENTRY(Counter[i]),Buf);
	}
}

//crea tag con il giocatore corrente
GtkWidget *gtk_crea_giocatore_c()
{
	GtkWidget *Table;
	
	Table=gtk_table_new(3, 1, FALSE);
	CurrentL=gtk_label_new("Fantasy Core");
	gtk_table_attach_defaults(GTK_TABLE(Table),CurrentL,1,2,0,1);
	gtk_widget_set_size_request(CurrentL,2*Dim_casella,-1);
	gtk_widget_show(CurrentL);
	CurrentI1=gtk_image_new_from_pixbuf(Immagine.decorazione);
	gtk_widget_set_size_request(CurrentI1,Dim_casella/2,Dim_casella/2);
	gtk_table_attach_defaults(GTK_TABLE(Table),CurrentI1,0,1,0,1);
	gtk_widget_show(CurrentI1);
	CurrentI2=gtk_image_new_from_pixbuf(Immagine.decorazione);
	gtk_widget_set_size_request(CurrentI2,Dim_casella/2,Dim_casella/2);
	gtk_table_attach_defaults(GTK_TABLE(Table),CurrentI2,2,3,0,1);
	gtk_widget_show_all(CurrentI2);
	return Table;
}

//aggiorna il tag con il giocatore corrente
void gtk_aggiorna_giocatore_c()
{
	char buf[20];
	
	gtk_image_set_from_pixbuf(GTK_IMAGE(CurrentI1),Immagine.a[CurrentPlayer]);
	gtk_image_set_from_pixbuf(GTK_IMAGE(CurrentI2),Immagine.a[CurrentPlayer]);
	sprintf(buf,"Giocatore %d",CurrentPlayer+1);
	gtk_label_set_text(GTK_LABEL(CurrentL),buf);
}

//crea un footer
GtkWidget *gtk_crea_footer()
{
	GtkWidget *Table;
	GtkWidget *Icon;
	GtkWidget *Label;
	
	Table=gtk_table_new(3, 1, FALSE);
	Label=gtk_label_new("Fantasy Core");
	gtk_widget_set_size_request(Label, 2*Dim_casella, -1);
	gtk_table_attach_defaults(GTK_TABLE(Table),Label,1,2,0,1);
	gtk_widget_show(Label);
	Icon=gtk_image_new_from_pixbuf(Immagine.decorazione);
	gtk_widget_set_size_request(Icon, Dim_casella/2, Dim_casella/2);
	gtk_table_attach_defaults(GTK_TABLE(Table),Icon,0,1,0,1);
	gtk_widget_show(Icon);
	Icon=gtk_image_new_from_pixbuf(Immagine.decorazione);
	gtk_widget_set_size_request(Icon, Dim_casella/2, Dim_casella/2);
	gtk_table_attach_defaults(GTK_TABLE(Table),Icon,2,3,0,1);
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
	int x,y;
	int wx,wy;
	
	Toplevel=gtk_widget_get_toplevel(Casella);
	gdk_window_get_root_origin(gtk_widget_get_window(Toplevel),&wx,&wy);
	gtk_widget_translate_coordinates(Casella,Toplevel, wx,wy, &x, &y);
	fprintf(stderr,"debug: x=%d y=%d wx=%d wy=%d\n",x,y,wx,wy);
	if(Perdite>0) sprintf(buf,"<span foreground=\"red\">-%d</span>",Perdite);
	else sprintf(buf,"<span foreground=\"green\">%d</span>",Perdite);
	Popup=gtk_window_new(GTK_WINDOW_POPUP);
	gtk_window_set_opacity(GTK_WINDOW(Popup),0.6);
	Label=gtk_label_new(NULL);
	gtk_label_set_markup(GTK_LABEL(Label),buf);
	gtk_container_add(GTK_CONTAINER(Popup),Label);
	gtk_window_move(GTK_WINDOW(Popup),x+Dim_casella/4,y+Dim_casella);
	gtk_widget_show_all(Popup);
	gdk_threads_add_timeout_full(G_PRIORITY_DEFAULT_IDLE,2000,(GSourceFunc) (autodestroy_popup),(gpointer) Popup,NULL);
	return;
}
