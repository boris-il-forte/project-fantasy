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

#include "fantasy-gtk-tab.h"
#include "fantasy-gtk-img.h"
#include "fantasy-core.h"

//struct per tab
typedef struct s_listapertab
{
	GtkWidget* P;
	struct s_listapertab* next;
} t_listapertab;

GtkWidget *Notebook[4];
GtkWidget *Listacastelli[NUMCASTELLI];
t_listapertab* Listastrutture;
t_listapertab* Listatruppe;

static void centra_mappa(char* pos)
{
	int P = (int) (pos - infomappa.mappa);

	cx = P % LARGHEZZA + 1 - caselle_orizzontali / 2;
	cy = P / LARGHEZZA + 1 - caselle_verticali / 2;
	if (cx < 0)
		cx = 0;
	if (cy < 0)
		cy = 0;
	if (cx > LARGHEZZA - caselle_orizzontali)
		cx = (LARGHEZZA - caselle_orizzontali);
	if (cy > ALTEZZA - caselle_verticali)
		cy = (ALTEZZA - caselle_verticali);
	gtk_pulisci_mappa();
	gtk_aggiorna_coordinate(cx, cy);
	gtk_stampa_mappa(cx, cy, 'n');
}


//inizializza le strutture dati dei tab
void gtk_inizializza_tab()
{
	int i;

	for (i = 0; i < NUMCASTELLI; i++)
		Listacastelli[i] = NULL;
	Listastrutture = NULL;
	Listatruppe = NULL;
}

//crea un tab del notebook
GtkWidget *gtk_crea_notebook_tab(GtkWidget *Notebook, char *buf)
{
	GtkWidget *Etichetta;
	GtkWidget *Vbox;
	GtkWidget *Scroller;

	Etichetta = gtk_label_new(buf);
	Scroller = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(Scroller),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	Vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(Scroller), Vbox);
	gtk_widget_show(Vbox);
	gtk_widget_show(Etichetta);
	gtk_widget_show(Scroller);
	gtk_notebook_append_page(GTK_NOTEBOOK(Notebook), Scroller, Etichetta);
	return (GtkWidget *) Vbox;
}

//crea il notebook
GtkWidget* gtk_crea_notebook(GtkWidget *Frame)
{
	Notebook[0] = gtk_notebook_new();
	gtk_widget_set_size_request(Notebook[0], -1, 3 * Dim_casella - 10);
	gtk_container_add(GTK_CONTAINER(Frame), Notebook[0]);
	Notebook[1] = (GtkWidget *) gtk_crea_notebook_tab(Notebook[0], "Castelli");
	Notebook[2] = (GtkWidget *) gtk_crea_notebook_tab(Notebook[0], "Strutture");
	Notebook[3] = (GtkWidget *) gtk_crea_notebook_tab(Notebook[0], "Armate");

	return Notebook[0];
}

//crea un elemento di un tab di un notebook
GtkWidget *gtk_crea_elemento_tab(GtkWidget *tab, int x, int y, char *nome,
		char *buf)
{
	GtkWidget *Hbox;
	GtkWidget *Vbox;
	GtkWidget *Label;
	GtkWidget *Button;
	GtkWidget *Separator;
	char buffer[10];

	if (tab == NULL)
	{
		printf("errore!");
		return NULL;
	}
	Vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(GTK_BOX(tab), Vbox, FALSE, FALSE, 0);
	Separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_pack_start(GTK_BOX(Vbox), Separator, FALSE, FALSE, 3);
	gtk_widget_show(Separator);
	Hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_pack_start(GTK_BOX(Vbox), Hbox, FALSE, FALSE, 0);
	Label = gtk_label_new(nome);
	gtk_box_pack_start(GTK_BOX(Hbox), Label, FALSE, FALSE, 0);
	gtk_widget_show(Label);
	Label = gtk_label_new(buf);
	gtk_box_pack_start(GTK_BOX(Hbox), Label, FALSE, FALSE, 0);
	gtk_widget_show(Label);
	sprintf(buffer, "%d|%d", x, y);
	Button = gtk_button_new_with_label(buffer);
	g_signal_connect_swapped(Button, "button_press_event",
			G_CALLBACK(centra_mappa),
			(gpointer)&infomappa.mappa[x+y*LARGHEZZA]);
	gtk_box_pack_start(GTK_BOX(Hbox), Button, FALSE, FALSE, 0);
	gtk_widget_show(Button);
	gtk_widget_show(Hbox);
	gtk_widget_show(Vbox);
	return Vbox;
}

//riempe il tab castelli giocatore
GtkWidget * gtk_riempi_tab_castelli(int i, char* buf)
{
	int P;
	int x, y;
	GtkWidget *R;

	if (i != 0)
	{
		P = giocatore[CurrentPlayer]->struttura[Cas]->pos;
		x = P % LARGHEZZA + 1;
		y = P / LARGHEZZA + 1;
		R = gtk_crea_elemento_tab(Notebook[1], x, y, buf, "N°1");
		return R;
	}
	else
		return NULL;
}

//aggiorna il tab castelli
void gtk_aggiorna_tab_castelli()
{
	int i;
	int x, y;
	char nome[20];
	char buf[20];
	t_lista_s* S = giocatore[CurrentPlayer]->struttura[Cas];

	for (i = 0; Listacastelli[i] != NULL; i++)
	{
		gtk_pulisci_tab(Listacastelli[i]);
		Listacastelli[i] = NULL;
	}
	for (i = 0; S != NULL && i < NUMCASTELLI; i++)
	{
		x = S->pos % LARGHEZZA + 1;
		y = S->pos / LARGHEZZA + 1;
		switch (i)
		{
			case 0:
				sprintf(nome, "Capitale");
				break;
			default:
				sprintf(nome, "Città %d", i);
				break;
		}
		sprintf(buf, "N°%d", i + 1);
		Listacastelli[i] = gtk_crea_elemento_tab(Notebook[1], x, y, nome, buf);
		S = S->next;
	}
}

//aggiorna il tab strutture
void gtk_aggiorna_tab_strutture()
{
	int i, j = 0;
	int x, y;
	char nome[20];
	char buf[20];
	t_lista_s* S[NUMSTRUTTURE - 1];
	t_listapertab* Tab;
	t_listapertab* Tabp;

	for (i = 1; i < NUMSTRUTTURE; i++)
		S[i - 1] = giocatore[CurrentPlayer]->struttura[i];
	Tab = Listastrutture;
	while (Tab != NULL)
	{
		gtk_pulisci_tab(Tab->P);
		Tabp = Tab;
		Tab = Tab->next;
		free(Tabp);
	}
	Listastrutture = NULL;
	for (i = 0; i < NUMSTRUTTURE - 1; i++)
		while (S[i] != NULL)
		{
			j++;
			x = S[i]->pos % LARGHEZZA + 1;
			y = S[i]->pos / LARGHEZZA + 1;
			switch (i + 1)
			{
				case Fat:
					sprintf(nome, "Fattoria");
					break;
				case Scu:
					sprintf(nome, "Scuderia");
					break;
				case Gro:
					sprintf(nome, "Grotta");
					break;
				case Nid:
					sprintf(nome, "Nido");
					break;
				default:
					sprintf(nome, "Errore");
					break;
			}
			Tabp = Tab;
			Tab = (t_listapertab*) malloc(sizeof(t_listapertab));
			if (Tabp == NULL)
				Listastrutture = Tab;
			else
				Tabp->next = Tab;
			sprintf(buf, "N°%d", j);
			Tab->P = gtk_crea_elemento_tab(Notebook[2], x, y, nome, buf);
			Tab->next = NULL;
			S[i] = S[i]->next;
		}
}

//aggiorna il tab armate
void gtk_aggiorna_tab_armate()
{
	int x, y;
	char nome[20];
	char buf[20];
	t_lista_t* T = giocatore[CurrentPlayer]->truppe;
	t_listapertab* Tab;
	t_listapertab* Tabp;

	Tab = Listatruppe;
	while (Tab != NULL)
	{
		gtk_pulisci_tab(Tab->P);
		Tabp = Tab;
		Tab = Tab->next;
		free(Tabp);
	}
	Listatruppe = NULL;
	while (T != NULL)
	{
		x = T->pos % LARGHEZZA + 1;
		y = T->pos / LARGHEZZA + 1;
		switch (T->truppa->tipo)
		{
			case Rec:
				sprintf(nome, "Reclute");
				break;
			case Fan:
				sprintf(nome, "Fanteria");
				break;
			case Lan:
				sprintf(nome, "Lancieri");
				break;
			case Arc:
				sprintf(nome, "Arcieri");
				break;
			case Cav:
				sprintf(nome, "Cavalleria");
				break;
			case Dra:
				sprintf(nome, "Drago");
				break;
			case Fen:
				sprintf(nome, "Fenice");
				break;
			default:
				sprintf(nome, "Errore");
				break;
		}
		Tabp = Tab;
		Tab = (t_listapertab*) malloc(sizeof(t_listapertab));
		if (Tabp == NULL)
			Listatruppe = Tab;
		else
			Tabp->next = Tab;
		sprintf(buf, "-%d(%d)-", T->truppa->numero, T->truppa->morale);
		Tab->P = gtk_crea_elemento_tab(Notebook[3], x, y, nome, buf);
		Tab->next = NULL;
		T = T->next;
	}
}

//aggiorna tutti i tab
void gtk_aggiorna_tab()
{
	gtk_aggiorna_tab_castelli();
	gtk_aggiorna_tab_strutture();
	gtk_aggiorna_tab_armate();
}

//azzera tab
void gtk_azzera_tab()
{
	int i;
	t_listapertab* Tab;
	t_listapertab* Tabp;

	for (i = 0; i < NUMCASTELLI; i++)
	{
		gtk_pulisci_tab(Listacastelli[i]);
		Listacastelli[i] = NULL;
	}
	Tab = Listatruppe;
	while (Tab != NULL)
	{
		gtk_pulisci_tab(Tab->P);
		Tabp = Tab;
		Tab = Tab->next;
		free(Tabp);
	}
	Tab = Listastrutture;
	while (Tab != NULL)
	{
		gtk_pulisci_tab(Tab->P);
		Tabp = Tab;
		Tab = Tab->next;
		free(Tabp);
	}
	Listatruppe = NULL;
	Listastrutture = NULL;
}

//pulisce i tab
void gtk_pulisci_tab(GtkWidget *Target)
{
	if (Target == NULL)
		return;
	else
		gtk_widget_destroy(Target);
}
