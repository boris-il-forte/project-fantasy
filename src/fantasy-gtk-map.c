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

#include "fantasy-core-mov.h"

#include <glib/gi18n.h>

//struct spin buton
typedef struct s_spin
{
	GtkAdjustment* A;
	int somma;
} t_spin;

static GtkWidget *Casella[LARGHEZZA * ALTEZZA];
static GtkWidget *Thumb[LARGHEZZA * ALTEZZA];

static int mossa;

static gboolean set_adjustmentvalue(GtkAdjustment* S, t_spin* C)
{
	int n = gtk_adjustment_get_value(GTK_ADJUSTMENT(S));
	int max = C->somma;

	GtkAdjustment* D = C->A;
	gtk_adjustment_set_value(GTK_ADJUSTMENT(D), max - n);
	return FALSE;
}

static void addestra_truppa(t_callback_s* Struct)
{
	int P = Struct->pos;
	int r;
	t_truppa T = Struct->tipo;
	GtkWidget *Dialogo;
	GtkWidget *Label;

	r = pagaunita(T);
	if (r == 0)
	{
		addestratruppa(P, T);
		gtk_aggiorna_contarisorse();
	}
	else
	{
		Dialogo = gtk_dialog_new_with_buttons("F.C.", NULL,
					GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL, _("_OK"),
					GTK_RESPONSE_ACCEPT, NULL);
		gtk_window_set_icon(GTK_WINDOW(Dialogo), Immagine.logo);
		switch (r)
		{
			case 1:
				Label = gtk_label_new("Oro insufficente!!");
				break;
			case 2:
				Label = gtk_label_new("Cibo insufficente!");
				break;
			case 3:
				Label = gtk_label_new("Smeraldi insufficenti!!");
				break;
			default:
				Label = gtk_label_new(
							"Errore! questo messaggio non deve comparire!");
				break;
		}
		gtk_widget_show(Label);
		gtk_box_pack_start(
					GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),
					Label, TRUE,
					TRUE, 0);
		gtk_dialog_run(GTK_DIALOG(Dialogo));
		gtk_widget_destroy(Dialogo);
	}
}

static void evacua_truppa(t_lista_t *T)
{
	int Occupata;

	Occupata = evacuatruppa(T);
	if (Occupata)
	{
		return;
	}
	else
	{
		gtk_aggiorna_tab();
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx, cy, 'n');
	}
}

static void click_bersaglio(char* pos)
{
	int Dst = (int) (pos - ancora);
	int Src = mossa;
	int pdst, psrc;

	if (Src == Dst)
	{
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx, cy, 'n');
	}
	else
	{
		pdst = infomappa.truppe[Dst]->numero;
		psrc = infomappa.truppe[Src]->numero;
		combatticampoaperto(Dst, Src);
		gtk_aggiorna_tab();
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx, cy, 'n');
		pdst -= (infomappa.truppe[Dst] != NULL) ?
					(infomappa.truppe[Dst]->numero) : (0);
		psrc -= (infomappa.truppe[Src] != NULL) ?
					(infomappa.truppe[Src]->numero) : (0);
		gtk_popup_combattimento(
					Casella[Dst % LARGHEZZA - cx
								+ caselle_orizzontali * (Dst / LARGHEZZA - cy)],
					pdst);
		gtk_popup_combattimento(
					Casella[Src % LARGHEZZA - cx
								+ caselle_orizzontali * (Src / LARGHEZZA - cy)],
					psrc);
	}
}

static void click_destinazione(char* pos)
{
	int Dst = (int) (pos - ancora);
	int Src = mossa;

	if (Src == Dst)
	{
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx, cy, 'n');
		return;
	}
	else
	{
		spostatruppa(Src, Dst);
		gtk_aggiorna_tab();
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx, cy, 'n');
	}
}

static void annulla_mossa()
{
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx, cy, 'n');
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

	aggiorna_tr_callback(pos - ancora);
	/*crea menu*/
	menu = gtk_menu_new();
	/*etichetta addestra*/
	scelta = gtk_menu_item_new_with_label("Addestra:");
	/*crea pulsanti per addestrare unità*/
	lista = gtk_menu_new();
	//reclute
	oggetto = gtk_menu_item_new_with_label("Reclute");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(addestra_truppa),
				(gpointer )&tr_callback[Rec]);
	gtk_widget_show(oggetto);
	//fanteria
	oggetto = gtk_menu_item_new_with_label("Fanteria");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(addestra_truppa),
				(gpointer )&tr_callback[Fan]);
	gtk_widget_show(oggetto);
	//lanceri
	oggetto = gtk_menu_item_new_with_label("Lancieri");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(addestra_truppa),
				(gpointer )&tr_callback[Lan]);
	gtk_widget_show(oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
	gtk_widget_show(scelta);
	/*etichetta evacua*/
	scelta = gtk_menu_item_new_with_label("Evacua:");
	/*crea pulsanti per evacuare unità*/
	lista = gtk_menu_new();
	//inizializza la lista truppe
	S = puntastruttura(pos - ancora);
	T = S->in;
	//lista le unità nella struttura
	while (T != NULL)
	{
		identificatruppa(T->truppa, buf);
		oggetto = gtk_menu_item_new_with_label(buf);
		gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
		g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(evacua_truppa),
					(gpointer )T);
		gtk_widget_show(oggetto);
		T = T->next;
	}
	//attacca la lista alla scelta
	if (S->in != NULL)
	{
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
		gtk_widget_show(scelta);
	}
	//mostra il menu
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, 1,
				gtk_get_current_event_time());
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

	aggiorna_tr_callback(pos - ancora);
	/*crea menu*/
	menu = gtk_menu_new();
	/*etichetta addestra*/
	scelta = gtk_menu_item_new_with_label("Addestra:");
	/*crea pulsanti per addestrare unità*/
	lista = gtk_menu_new();
	//reclute
	oggetto = gtk_menu_item_new_with_label("Reclute");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(addestra_truppa),
				(gpointer )&tr_callback[Rec]);
	gtk_widget_show(oggetto);
	//Cavalleria
	oggetto = gtk_menu_item_new_with_label("Cavalleria");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(addestra_truppa),
				(gpointer )&tr_callback[Cav]);
	gtk_widget_show(oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
	gtk_widget_show(scelta);
	/*etichetta evacua*/
	scelta = gtk_menu_item_new_with_label("Evacua:");
	/*crea pulsanti per evacuare unità*/
	lista = gtk_menu_new();
	//inizializza la lista truppe
	S = puntastruttura(pos - ancora);
	T = S->in;
	//lista le unità nella struttura
	while (T != NULL)
	{
		identificatruppa(T->truppa, buf);
		oggetto = gtk_menu_item_new_with_label(buf);
		gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
		g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(evacua_truppa),
					(gpointer )T);
		gtk_widget_show(oggetto);
		T = T->next;
	}
	//attacca la lista alla scelta
	if (S->in != NULL)
	{
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
		gtk_widget_show(scelta);
	}
	//mostra il menu
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, 1,
				gtk_get_current_event_time());
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

	aggiorna_tr_callback(pos - ancora);
	/*crea menu*/
	menu = gtk_menu_new();
	/*etichetta addestra*/
	scelta = gtk_menu_item_new_with_label("Addestra:");
	/*crea pulsanti per addestrare unità*/
	lista = gtk_menu_new();
	//reclute
	oggetto = gtk_menu_item_new_with_label("Reclute");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(addestra_truppa),
				(gpointer )&tr_callback[Rec]);
	gtk_widget_show(oggetto);
	//fanteria
	oggetto = gtk_menu_item_new_with_label("Arcieri");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(addestra_truppa),
				(gpointer )&tr_callback[Arc]);
	gtk_widget_show(oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
	gtk_widget_show(scelta);
	/*etichetta evacua*/
	scelta = gtk_menu_item_new_with_label("Evacua:");
	/*crea pulsanti per evacuare unità*/
	lista = gtk_menu_new();
	//inizializza la lista truppe
	S = puntastruttura(pos - ancora);
	T = S->in;
	//lista le unità nella struttura
	while (T != NULL)
	{
		identificatruppa(T->truppa, buf);
		oggetto = gtk_menu_item_new_with_label(buf);
		gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
		g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(evacua_truppa),
					(gpointer )T);
		gtk_widget_show(oggetto);
		T = T->next;
	}
	//attacca la lista alla scelta
	if (S->in != NULL)
	{
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
		gtk_widget_show(scelta);
	}
	//mostra il menu
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, 1,
				gtk_get_current_event_time());
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

	aggiorna_tr_callback(pos - ancora);
	/*crea menu*/
	menu = gtk_menu_new();
	/*etichetta addestra*/
	scelta = gtk_menu_item_new_with_label("Addestra:");
	/*crea pulsanti per addestrare unità*/
	lista = gtk_menu_new();
	//Draghi
	oggetto = gtk_menu_item_new_with_label("Draghi");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(addestra_truppa),
				(gpointer )&tr_callback[Dra]);
	gtk_widget_show(oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
	gtk_widget_show(scelta);
	/*etichetta evacua*/
	scelta = gtk_menu_item_new_with_label("Evacua:");
	/*crea pulsanti per evacuare unità*/
	lista = gtk_menu_new();
	//inizializza la lista truppe
	S = puntastruttura(pos - ancora);
	T = S->in;
	//lista le unità nella struttura
	while (T != NULL)
	{
		identificatruppa(T->truppa, buf);
		oggetto = gtk_menu_item_new_with_label(buf);
		gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
		g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(evacua_truppa),
					(gpointer )T);
		gtk_widget_show(oggetto);
		T = T->next;
	}
	//attacca la lista alla scelta
	if (S->in != NULL)
	{
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
		gtk_widget_show(scelta);
	}
	//mostra il menu
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, 1,
				gtk_get_current_event_time());
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

	aggiorna_tr_callback(pos - ancora);
	/*crea menu*/
	menu = gtk_menu_new();
	/*etichetta addestra*/
	scelta = gtk_menu_item_new_with_label("Addestra:");
	/*crea pulsanti per addestrare uniFtà*/
	lista = gtk_menu_new();
	//Fenici
	oggetto = gtk_menu_item_new_with_label("Fenici");
	gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
	g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(addestra_truppa),
				(gpointer )&tr_callback[Fen]);
	gtk_widget_show(oggetto);
	//attacca la lista alla scelta
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
	gtk_widget_show(scelta);
	/*etichetta evacua*/
	scelta = gtk_menu_item_new_with_label("Evacua:");
	/*crea pulsanti per evacuare unità*/
	lista = gtk_menu_new();
	//inizializza la lista truppe
	S = puntastruttura(pos - ancora);
	T = S->in;
	//lista le unità nella struttura
	while (T != NULL)
	{
		identificatruppa(T->truppa, buf);
		oggetto = gtk_menu_item_new_with_label(buf);
		gtk_menu_shell_append(GTK_MENU_SHELL(lista), oggetto);
		g_signal_connect_swapped(oggetto, "activate", G_CALLBACK(evacua_truppa),
					(gpointer )T);
		gtk_widget_show(oggetto);
		T = T->next;
	}
	//attacca la lista alla scelta
	if (S->in != NULL)
	{
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(scelta), lista);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), scelta);
		gtk_widget_show(scelta);
	}
	//mostra il menu
	gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, 1,
				gtk_get_current_event_time());
}

static void click_unita(char* pos, GdkEventButton *Event)
{
	GtkWidget* Dialogo;
	GtkWidget* Label;
	t_infotruppa* T;
	int Pos = (int) (pos - ancora);

	T = infomappa.truppe[Pos];
	if (Event->button == 3)
	{
		if (T->combattuto == 0)
		{
			mossa = Pos;
			gtk_pulisci_mappa();
			gtk_stampa_mappa(cx, cy, 'c');
			return;
		}
		else
		{
			Dialogo = gtk_dialog_new_with_buttons("F.C.", NULL,
						GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
						_("_OK"), GTK_RESPONSE_ACCEPT, NULL);
			gtk_window_set_icon(GTK_WINDOW(Dialogo), Immagine.logo);
			Label = gtk_label_new("unità in combattimento!");
			gtk_widget_show(Label);
			gtk_box_pack_start(
						GTK_BOX(
									gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),
						Label,
						TRUE, TRUE, 0);
			gtk_dialog_run(GTK_DIALOG(Dialogo));
			gtk_widget_destroy(Dialogo);
			return;
		}
	}
	else
	{
		if (T->stanca == 0 && T->combattuto == 0)
		{
			mossa = Pos;
			gtk_pulisci_mappa();
			gtk_stampa_mappa(cx, cy, 's');
			return;
		}
		else
		{
			Dialogo = gtk_dialog_new_with_buttons("F.C.", NULL,
						GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
						_("_OK"), GTK_RESPONSE_ACCEPT, NULL);
			gtk_window_set_icon(GTK_WINDOW(Dialogo), Immagine.logo);
			if (T->stanca == 1)
				Label = gtk_label_new("unità stanca!");
			else
				Label = gtk_label_new("unità in combattimento!");
			gtk_widget_show(Label);
			gtk_box_pack_start(
						GTK_BOX(
									gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),
						Label,
						TRUE, TRUE, 0);
			gtk_dialog_run(GTK_DIALOG(Dialogo));
			gtk_widget_destroy(Dialogo);
			return;
		}
	}
}

static void su_unita(GtkWidget *Truppa, GdkEvent *event, char* pos)
{
	int Pos = (int) (pos - ancora);
	t_infotruppa* T;
	char buf[30];

	if (event != NULL)
	{
		T = infomappa.truppe[Pos];
		identificatruppa(T, buf);
		gtk_widget_set_tooltip_text(Truppa, buf);
	}
}

static void click_assediocastello(char* pos)
{
	int Pos = (int) (pos - ancora);
	int G = controlloedificio(Pos, Cas);
	int W;
	char buf[60];
	GtkWidget *Dialogo;
	GtkWidget *Label;

	assediocastello(Pos);
	gtk_aggiorna_tab();
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx, cy, 'n');
	if (controllosconfitto(G) == 1)
	{
		liberagiocatore(G);
		Dialogo = gtk_dialog_new_with_buttons("F.C.", NULL,
					GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL, _("_OK"),
					GTK_RESPONSE_ACCEPT, NULL);
		gtk_window_set_icon(GTK_WINDOW(Dialogo), Immagine.logo);
		sprintf(buf, " il giocatore %d \n è stato sconfitto! ", G + 1);
		Label = gtk_label_new(buf);
		gtk_widget_show(Label);
		gtk_box_pack_start(
					GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),
					Label, TRUE,
					TRUE, 0);
		gtk_dialog_run(GTK_DIALOG(Dialogo));
		gtk_widget_destroy(Dialogo);
		W = controllovincitore();
		if (W != 0)
		{
			gtk_proclama_vincitore(W);
		}
	}
}

static void click_assaltostruttura(char* pos)
{
	int Pos = (int) (pos - ancora);
	int G;
	int W;
	char buf[60];
	t_struttura t = controllotipostruttura(Pos);
	GtkWidget *Dialogo;
	GtkWidget *Label;

	G = controlloedificio(Pos, t);
	assediostruttura(Pos);
	gtk_aggiorna_tab();
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx, cy, 'n');
	if (controllosconfitto(G) == 1)
	{
		liberagiocatore(G);
		Dialogo = gtk_dialog_new_with_buttons("F.C.", NULL,
					GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL, _("_OK"),
					GTK_RESPONSE_ACCEPT, NULL);
		gtk_window_set_icon(GTK_WINDOW(Dialogo), Immagine.logo);
		sprintf(buf, " il giocatore %d \n è stato sconfitto! ", G + 1);
		Label = gtk_label_new(buf);
		gtk_widget_show(Label);
		gtk_box_pack_start(
					GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))),
					Label, TRUE,
					TRUE, 0);
		gtk_dialog_run(GTK_DIALOG(Dialogo));
		gtk_widget_destroy(Dialogo);
		W = controllovincitore();
		if (W != 0)
		{
			gtk_proclama_vincitore(W);
		}
	}
}

static void click_entrastruttura(char* pos)
{
	int Pos = (int) (pos - ancora);

	spostainstruttura(mossa, Pos);
	gtk_aggiorna_tab();
	gtk_pulisci_mappa();
	gtk_stampa_mappa(cx, cy, 'n');
}

static void click_unisci(char* pos)
{
	int Pos = (int) (pos - ancora);
	int max, somma, min;
	const int a = 0, b = 1;
	GtkWidget * Dialogo;
	GtkWidget * Vbox;
	GtkWidget * Label;
	GtkWidget * Spin1;
	GtkWidget * Spin2;
	GtkAdjustment *UA, *UB;
	t_infotruppa* TA = infomappa.truppe[mossa];
	t_infotruppa* TB = infomappa.truppe[Pos];
	t_spin S_Callback[2];
	t_truppa Tipo = TB->tipo;

	switch (Tipo)
	{
		case Rec:
		case Fan:
		case Lan:
		case Arc:
			max = 100 * 2;
			break;
		case Cav:
			max = 50 * 2;
			break;
		case Dra:
		case Fen:
			max = 30 * 2;
			break;
		default:
			max = 100;
			break;
	}
	somma = TA->numero + TB->numero;
	min = (somma - max) > 0 ? (somma - max) : 0;
	UA = (GtkAdjustment*) gtk_adjustment_new(TA->numero, min,
				max < somma ? max : somma, 1, 0, 0);
	UB = (GtkAdjustment*) gtk_adjustment_new(TB->numero, min,
				max < somma ? max : somma, 1, 0, 0);
	S_Callback[a].A = UA;
	S_Callback[a].somma = somma;
	S_Callback[b].A = UB;
	S_Callback[b].somma = somma;
	g_signal_connect(UA, "value_changed", G_CALLBACK(set_adjustmentvalue),
				&S_Callback[b]);
	g_signal_connect(UB, "value_changed", G_CALLBACK(set_adjustmentvalue),
				&S_Callback[a]);
	Dialogo = gtk_dialog_new_with_buttons("Fantasy C", NULL,
				GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL, _("_OK"),
				GTK_RESPONSE_YES, _("_Cancel"), GTK_RESPONSE_NO, NULL);
	gtk_window_set_icon(GTK_WINDOW(Dialogo), Immagine.logo);
	Vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(
				GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(Dialogo))), Vbox,
				TRUE, TRUE, 0);
	gtk_widget_show(Vbox);
	Label = gtk_label_new("Sposta effettivi");
	gtk_box_pack_start(GTK_BOX(Vbox), Label, TRUE, TRUE, 0);
	gtk_widget_show(Label);
	Spin1 = gtk_spin_button_new(GTK_ADJUSTMENT(UA), 0, 0);
	gtk_box_pack_start(GTK_BOX(Vbox), Spin1, TRUE, TRUE, 0);
	gtk_widget_show(Spin1);
	Spin2 = gtk_spin_button_new(GTK_ADJUSTMENT(UB), 0, 0);
	gtk_box_pack_start(GTK_BOX(Vbox), Spin2, TRUE, TRUE, 0);
	gtk_widget_show(Spin2);
	if (gtk_dialog_run(GTK_DIALOG(Dialogo)) == GTK_RESPONSE_YES)
	{
		TB->stanca = 1;
		TA->numero = gtk_adjustment_get_value(GTK_ADJUSTMENT(UA));
		TB->numero = gtk_adjustment_get_value(GTK_ADJUSTMENT(UB));
		if (TA->numero == 0)
			eliminamorti(TA);
		if (TB->numero == 0)
			eliminamorti(TB);
		gtk_widget_destroy(Dialogo);
	}
	else
	{
		TA->stanca = 0;
		gtk_widget_destroy(Dialogo);
	}
	gtk_pulisci_mappa();
	gtk_aggiorna_tab();
	gtk_stampa_mappa(cx, cy, 'n');
}

//crea le caselle della tabella per la mappa
void gtk_genera_mappa(GtkWidget *Mappa)
{
	int i, j;

	for (i = 0; i < caselle_orizzontali; i++)
		for (j = 0; j < caselle_verticali; j++)
		{
			GtkWidget *eventBox = gtk_event_box_new();
			gtk_container_set_border_width(GTK_CONTAINER(eventBox), 0);
			gtk_widget_set_events(eventBox,
						GDK_BUTTON_PRESS_MASK | GDK_ENTER_NOTIFY_MASK);
			gtk_widget_set_size_request(eventBox, Dim_casella, Dim_casella);
			gtk_grid_attach(GTK_GRID(Mappa), eventBox, i, j, 1, 1);
			gtk_widget_show(eventBox);
			gtk_widget_realize(eventBox);

			Casella[i + j * caselle_orizzontali] = eventBox;
		}
}

//restituisce l'ambiente da aggiungere
GdkPixbuf* gtk_immagine_ambiente(int x, int y)
{
	unsigned char ambiente = accedi(x, y, infomappa.ambiente);

	//se non c'è nulla, stampa niente
	if (ambiente == 0)
		return NULL;

	// altrimenti stampa i campi di grano
	if (ambiente <= 13) //FIXME magic
		return Immagine.campo[ambiente - 1];


	return NULL;

}

//aggiunge lo sfondo
void gtk_stampa_base_mappa(GdkPixbuf **buffer, int x, int y)
{
	*buffer = gdk_pixbuf_copy(Immagine.prato[accedi(x, y, infomappa.prato)]);
	GdkPixbuf* ambiente = gtk_immagine_ambiente(x, y);
	if (ambiente != NULL)
		gdk_pixbuf_composite(ambiente, *buffer, 0, 0, Dim_casella, Dim_casella,
					0, 0, 1, 1, GDK_INTERP_BILINEAR, 255);
}

//aggiunge le strutture
int gtk_stampa_strutture(GdkPixbuf *buffer, int x, int y)
{
	GdkPixbuf *struttura;
	int isStructure = 1;
	int G;

	switch (accedi(x, y, infomappa.mappa))
	{
		/*stampa il castello*/
		case '0':
			G = controlloedificio(posiziona(-1, -1, x, y), Cas);
			struttura = Immagine.castello[G + 1][0];

			break;
		case '1':
			G = controlloedificio(posiziona(0, -1, x, y), Cas);
			struttura = Immagine.castello[G + 1][1];
			break;
		case '2':
			G = controlloedificio(posiziona(1, -1, x, y), Cas);
			struttura = Immagine.castello[G + 1][2];

			break;
		case '3':
			G = controlloedificio(posiziona(-1, 0, x, y), Cas);
			struttura = Immagine.castello[G + 1][3];
			break;
		case '4':
			G = controlloedificio(posiziona(0, 0, x, y), Cas);
			struttura = Immagine.castello[G + 1][4];
			break;
		case '5':
			G = controlloedificio(posiziona(1, 0, x, y), Cas);
			struttura = Immagine.castello[G + 1][5];
			break;
		case '6':
			G = controlloedificio(posiziona(-1, 1, x, y), Cas);
			struttura = Immagine.castello[G + 1][6];
			break;
		case '7':
			G = controlloedificio(posiziona(0, 1, x, y), Cas);
			struttura = Immagine.castello[G + 1][7];
			break;
		case '8':
			G = controlloedificio(posiziona(1, 1, x, y), Cas);
			struttura = Immagine.castello[G + 1][8];
			break;
			/*stampa la grotta*/
		case 'G':
			G = controlloedificio(posiziona(0, 0, x, y), Gro);
			struttura = Immagine.grotta[G + 1][0];
			break;
		case 'H':
			G = controlloedificio(posiziona(1, 0, x, y), Gro);
			struttura = Immagine.grotta[G + 1][1];
			break;
		case 'I':
			G = controlloedificio(posiziona(0, 1, x, y), Gro);
			struttura = Immagine.grotta[G + 1][2];
			break;
		case 'J':
			G = controlloedificio(posiziona(1, 1, x, y), Gro);
			struttura = Immagine.grotta[G + 1][3];
			break;
			/*stampa la fattoria*/
		case 'C':
			G = controlloedificio(posiziona(0, 0, x, y), Fat);
			struttura = Immagine.fattoria[G + 1][0];
			break;
		case 'D':
			G = controlloedificio(posiziona(1, 0, x, y), Fat);
			struttura = Immagine.fattoria[G + 1][1];
			break;
		case 'E':
			G = controlloedificio(posiziona(0, 1, x, y), Fat);
			struttura = Immagine.fattoria[G + 1][2];
			break;
		case 'F':
			G = controlloedificio(posiziona(1, 1, x, y), Fat);
			struttura = Immagine.fattoria[G + 1][3];
			break;
			/*stampa la scuderia*/
		case 'S':
			G = controlloedificio(posiziona(0, 0, x, y), Scu);
			struttura = Immagine.scuderia[G + 1][0];
			break;
		case 'T':
			G = controlloedificio(posiziona(1, 0, x, y), Scu);
			struttura = Immagine.scuderia[G + 1][1];
			break;
		case 'U':
			G = controlloedificio(posiziona(0, 1, x, y), Scu);
			struttura = Immagine.scuderia[G + 1][2];
			break;
		case 'V':
			G = controlloedificio(posiziona(1, 1, x, y), Scu);
			struttura = Immagine.scuderia[G + 1][3];
			break;
			/*stampa in nido*/
		case 'N':
			G = controlloedificio(posiziona(0, 0, x, y), Nid);
			struttura = Immagine.nido[G + 1][0];
			break;
		case 'O':
			G = controlloedificio(posiziona(1, 0, x, y), Nid);
			struttura = Immagine.nido[G + 1][1];
			break;
		case 'P':
			G = controlloedificio(posiziona(0, 1, x, y), Nid);
			struttura = Immagine.nido[G + 1][2];
			break;
		case 'Q':
			G = controlloedificio(posiziona(1, 1, x, y), Nid);
			struttura = Immagine.nido[G + 1][3];
			break;
		default:
			G = -1;
			isStructure = 0;

	}

	if (isStructure)
	{
		gdk_pixbuf_composite(struttura, buffer, 0, 0, Dim_casella, Dim_casella,
					0, 0, 1, 1, GDK_INTERP_BILINEAR, 255);
	}

	return G;
}

//aggiunge le truppe
int gtk_stampa_truppe(GdkPixbuf *buffer, int x, int y)
{
	int G = -1;
	t_truppa tipo;

	if (accedi(x,y,infomappa.truppe)!= NULL)
	{
		G = controllounita(posiziona_c(x,y));
		tipo = accedi(x,y,infomappa.truppe)->tipo;
		gdk_pixbuf_composite(Immagine.truppa[G][tipo], buffer, 0, 0, Dim_casella, Dim_casella, 0, 0, 1, 1, GDK_INTERP_BILINEAR, 255);
	}

	return G;
}

//aggiunge la zona attacco e movimento in campo aperto
int gtk_stampa_area_campo(GdkPixbuf *buffer, char mode, int Mossa, int x, int y,
			int G)
{
	GdkPixbuf *area;
	int isArea;
	int posizione = posiziona_c(x, y);
	int alpha;

	switch (mode)
	{
		case 's':
			isArea = posizione != Mossa && spostalecito(Mossa, posizione);
			if (!isArea)
				return isArea;
			area = Immagine.movimento;
			alpha = 25 + calcolaresiduo(Mossa, posizione);
			break;
		case 'c':
			isArea = posizione != Mossa && bersagliolecito(Mossa, posizione)
						&& G != CurrentPlayer;
			if (!isArea)
				return isArea;
			area = Immagine.attacco;
			if (assaltolecito(Mossa, posizione))
				alpha = 130;
			else
				alpha = 80;
			break;
		default:
			return 0;
	}

	gdk_pixbuf_composite(area, buffer, 0, 0, Dim_casella, Dim_casella, 0, 0, 1,
				1, GDK_INTERP_BILINEAR, alpha);

	return isArea;
}

//aggiunge la zona attacco e movimento sulle strutture
void gtk_stampa_area_strutture(GdkPixbuf *buffer, char mode, int G,
			t_struttura tipo, int posizione)
{
	GdkPixbuf *area;
	int isArea;

	switch (mode)
	{
		case 's':
			isArea = raggiungibile(posizione, tipo, mossa);
			if (!isArea || G != CurrentPlayer)
				return;
			area = Immagine.entrata;
			break;
		case 'c':
			isArea = assaltabile(posizione, tipo, mossa);
			if (!isArea || G == CurrentPlayer)
				return;
			area = Immagine.assalto;
			break;
		default:
			return;
	}

	gdk_pixbuf_composite(area, buffer, 0, 0, Dim_casella, Dim_casella, 0, 0, 1,
				1, GDK_INTERP_BILINEAR, 80);

}

void gtk_aggiungi_segnali_strutture_n(int G, t_struttura tipo, int posizione,
			int casella)
{
	if (G != CurrentPlayer)
		return;

	switch (tipo)
	{
		case Cas:
			g_signal_connect_swapped(Casella[casella], "button_press_event",
						G_CALLBACK(click_castello),
						(gpointer ) &ancora[posizione]);
			break;
		case Fat:
			g_signal_connect_swapped(Casella[casella], "button_press_event",
						G_CALLBACK(click_fattoria),
						(gpointer ) &ancora[posizione]);
			break;
		case Scu:
			g_signal_connect_swapped(Casella[casella], "button_press_event",
						G_CALLBACK(click_scuderia),
						(gpointer ) &ancora[posizione]);
			break;
		case Nid:
			g_signal_connect_swapped(Casella[casella], "button_press_event",
						G_CALLBACK(click_nido), (gpointer ) &ancora[posizione]);
			break;
		case Gro:
			g_signal_connect_swapped(Casella[casella], "button_press_event",
						G_CALLBACK(click_grotta),
						(gpointer ) &ancora[posizione]);
			break;
		default:
			break;
	}
}

void gtk_aggiungi_segnali_strutture_s(int G, t_struttura tipo, int posizione,
			int numeroCasella)
{
	int lecito = raggiungibile(posizione, tipo, mossa);
	if (lecito && G == CurrentPlayer)
	{
		g_signal_connect_swapped(Casella[numeroCasella], "button_press_event",
					G_CALLBACK(click_entrastruttura),
					(gpointer ) &ancora[posizione]);
	}
}

void gtk_aggiungi_segnali_strutture_c(int G, t_struttura tipo, int posizione,
			int casella)
{
	int lecito = assaltabile(posizione, tipo, mossa);
	if (lecito && G != CurrentPlayer)
	{
		if (tipo == Cas)
		{
			g_signal_connect_swapped(Casella[casella], "button_press_event",
						G_CALLBACK(click_assediocastello),
						(gpointer ) &ancora[posizione]);
		}
		else
		{
			g_signal_connect_swapped(Casella[casella], "button_press_event",
						G_CALLBACK(click_assaltostruttura),
						(gpointer ) &ancora[posizione]);
		}
	}
}

void gtk_aggiungi_segnali_strutture(char mode, int G, t_struttura tipo,
			int posizioneStruttura, int casella)
{

	switch (mode)
	{
		case 'n':
			gtk_aggiungi_segnali_strutture_n(G, tipo, posizioneStruttura,
						casella);
			break;
		case 's':
			gtk_aggiungi_segnali_strutture_s(G, tipo, posizioneStruttura,
						casella);
			break;
		case 'c':
			gtk_aggiungi_segnali_strutture_c(G, tipo, posizioneStruttura,
						casella);
			break;
		default:
			break;

	}
}

void gtk_aggiungi_segnali_truppe(char mode, int G, int x, int y, int posizione,
			int isArea)
{

	switch (mode)
	{
		case 'n':
			if (G == CurrentPlayer)
			{
				g_signal_connect_swapped(Casella[posizione],
							"button_press_event", G_CALLBACK(click_unita),
							(gpointer) &ancora[posiziona_c(x,y)]);
			}
			if (G != -1)
			{
				g_signal_connect(Casella[posizione], "enter-notify-event",
							G_CALLBACK(su_unita),
							(gpointer) &ancora[posiziona_c(x,y)]);
			}
			break;
		case 'c':
			if (G != -1 && bersagliolecito(mossa, posiziona(0, 0, x, y))
						&& G != CurrentPlayer)
			{
				g_signal_connect_swapped(Casella[posizione],
							"button_press_event", G_CALLBACK(click_bersaglio),
							(gpointer) &ancora[posiziona_c(x,y)]);
			}
			else
			{
				g_signal_connect_swapped(Casella[posizione],
							"button_press_event", G_CALLBACK(annulla_mossa),
							(gpointer) &ancora[posiziona_c(x,y)]);
			}
			break;
		case 's':
			if (assaltolecito(mossa, posiziona_c(x, y)) && G == CurrentPlayer
						&& tipouguale(x, y, mossa) && posiziona_c(x,y) != mossa)
			{
				g_signal_connect_swapped(Casella[posizione],
							"button_press_event", G_CALLBACK(click_unisci),
							(gpointer) &ancora[posiziona_c(x,y)]);
			}
			else if (isArea && posiziona_c(x,y) != mossa)
			{
				g_signal_connect_swapped(Casella[posizione],
							"button_press_event",
							G_CALLBACK(click_destinazione),
							(gpointer) &ancora[posiziona_c(x,y)]);
			}
			else
			{
				g_signal_connect_swapped(Casella[posizione],
							"button_press_event", G_CALLBACK(annulla_mossa),
							(gpointer) &ancora[posiziona_c(x,y)]);
			}
			break;
		default:
			break;

	}

}

//stampa la mappa
void gtk_stampa_mappa(int x, int y, char m)
{
	static char pre = 'n';
	int C, R;

	int posizioneAttuale;
	int G;
	char elem;
	int isArea;
	int Pos = 0;

	GdkPixbuf *buffer;
	t_struttura tipoS;

//gestisce memorizzazione del modo stampa
	if (m == 's')
	{
		calcolaspostamento(mossa);
	}
	if (m == 'p')
		m = pre;
	else
		pre = m;

//stampa ogni casella
	for (R = y; R < y + caselle_verticali; R++)
		for (C = x; C < x + caselle_orizzontali; C++)
		{
			elem = accedi(C, R, infomappa.mappa);
			gtk_stampa_base_mappa(&buffer, C, R);

			if (elem == ' ')
			{
				G = gtk_stampa_truppe(buffer, C, R);
				isArea = gtk_stampa_area_campo(buffer, m, mossa, C, R, G);
				gtk_aggiungi_segnali_truppe(m, G, C, R, Pos, isArea);
			}
			else
			{
				G = gtk_stampa_strutture(buffer, C, R);
				tipoS = tipostruttura(elem);
				posizioneAttuale = calcolaposizionestruttura(elem, C, R);
				gtk_stampa_area_strutture(buffer, m, G, tipoS,
							posizioneAttuale);
				gtk_aggiungi_segnali_strutture(m, G, tipoS, posizioneAttuale,
							Pos);
			}

			Thumb[Pos] = gtk_image_new_from_pixbuf(buffer);
			gtk_container_add(GTK_CONTAINER(Casella[Pos]), Thumb[Pos]);
			gtk_widget_show(Thumb[Pos]);

			g_object_unref(buffer);

			Pos++;
		}
}

//pulisce la mappa dalle immagini e dai segnali
void gtk_pulisci_mappa()
{
	int Pos;

	if (partita_in_corso == 1)
		for (Pos = 0; Pos < caselle_orizzontali * caselle_verticali; Pos++)
		{

			g_signal_handlers_disconnect_matched(Casella[Pos],
						G_SIGNAL_MATCH_FUNC, 0, 0, 0, click_castello, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],
						G_SIGNAL_MATCH_FUNC, 0, 0, 0, click_scuderia, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],
						G_SIGNAL_MATCH_FUNC, 0, 0, 0, click_fattoria, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],
						G_SIGNAL_MATCH_FUNC, 0, 0, 0, click_nido, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],
						G_SIGNAL_MATCH_FUNC, 0, 0, 0, click_grotta, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],
						G_SIGNAL_MATCH_FUNC, 0, 0, 0, click_unita, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],
						G_SIGNAL_MATCH_FUNC, 0, 0, 0, click_destinazione, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],
						G_SIGNAL_MATCH_FUNC, 0, 0, 0, click_bersaglio, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],
						G_SIGNAL_MATCH_FUNC, 0, 0, 0, click_assediocastello, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],
						G_SIGNAL_MATCH_FUNC, 0, 0, 0, click_assaltostruttura,
						0);
			g_signal_handlers_disconnect_matched(Casella[Pos],
						G_SIGNAL_MATCH_FUNC, 0, 0, 0, click_entrastruttura, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],
						G_SIGNAL_MATCH_FUNC, 0, 0, 0, click_unisci, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],
						G_SIGNAL_MATCH_FUNC, 0, 0, 0, su_unita, 0);
			g_signal_handlers_disconnect_matched(Casella[Pos],
						G_SIGNAL_MATCH_FUNC, 0, 0, 0, annulla_mossa, 0);
			gtk_widget_destroy(Thumb[Pos]);
		}
}

//elimina le caselle dalla tabella
void gtk_pulisci_caselle()
{
	int Pos;

	if (partita_in_corso == 1)
		for (Pos = 0; Pos < caselle_orizzontali * caselle_verticali; Pos++)
		{
			gtk_widget_destroy(Casella[Pos]);
		}
}
