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

//struct per tab
typedef struct s_listapertab
{
	GtkWidget* P;
	struct s_listapertab* next;
} t_listapertab;

//struct spin buton
typedef struct s_spin
{
	GtkAdjustment* A;
	int somma;
} t_spin;

//variabili globali
GtkWidget *Casella[LARGHEZZA*ALTEZZA];
GtkWidget *Thumb[LARGHEZZA*ALTEZZA];
GtkWidget *Notebook[4];
GtkWidget *Counter[NUMRISORSE];
GtkWidget *Listacastelli[NUMCASTELLI];
GtkWidget *Coordinate;
GtkWidget *CurrentI1;
GtkWidget *CurrentL;
GtkWidget *CurrentI2;
int Mossa;
int Dim_casella;
int caselle_orizzontali,caselle_verticali;
struct Immagini
{
	GdkPixbuf* logo;
	GdkPixbuf* decorazione;
	GdkPixbuf* freccia;
	GdkPixbuf* movimento;
	GdkPixbuf* attacco;
	GdkPixbuf* p[5]; // immagine del prato
	GdkPixbuf* c[MAXGIOCATORI+1][9]; // matrice che contiene i pezzi dei 12 castelli di proprietà + il castello vuoto
	GdkPixbuf* g[MAXGIOCATORI+1][4]; // matrice che contiene i pezzi delle 12 grotte di proprietà + la grotta vuota
	GdkPixbuf* s[MAXGIOCATORI+1][4]; // matrice che contiene i pezzi delle 12 scuderie di proprietà + la scuderia vuota
	GdkPixbuf* n[MAXGIOCATORI+1][4]; // matrice che contiene i pezzi dei 12 nidi di proprietà + il nido vuoto
	GdkPixbuf* f[MAXGIOCATORI+1][4]; // matrice che contiene i pezzi delle 12 fattorie di proprietà + la fattoria vuota
	GdkPixbuf* t[MAXGIOCATORI][Fen]; // matrice che contiene le truppe dei 12 giocatori
	GdkPixbuf* a[MAXGIOCATORI]; // array con gli scudi dei giocatori
	GdkPixbuf* err;
} Immagine;
t_listapertab* Listastrutture;
t_listapertab* Listatruppe;
/*header funzioni gtk*/

//funzioni generiche
void gtk_carica_immagini();
void gtk_calcola_dimensioni();
void gtk_inizializza_widget();
int gtk_carica_avvio(char *nomefile);

//funzioni menu
void gtk_crea_menu(GtkWidget *Vbox);

//funzioni mappa
void gtk_pulisci_mappa();
void gtk_stampa_mappa(int x, int y, char m);
GtkWidget *gtk_genera_mappa(GtkWidget *Hbox);

//funzioni tab
GtkWidget *gtk_crea_notebook_tab(GtkWidget *Notebook,char *buf);
void gtk_crea_notebook(GtkWidget *Frame);
GtkWidget *gtk_crea_elemento_tab(GtkWidget *tab,int x, int y,char *nome, char *buf);
GtkWidget * gtk_riempi_tab_castelli(int i, char* buf);
void gtk_aggiorna_tab_armate();
void gtk_aggiorna_tab_castelli();
void gtk_aggiorna_tab_strutture();
void gtk_pulisci_tab(GtkWidget *Target);
void gtk_azzera_tab();

//funzioni frecce
GtkWidget *gtk_crea_4_frecce();

//funzioni footer
GtkWidget *gtk_crea_footer();

//funzioni editor
GtkWidget *gtk_crea_pulsanti_editor_s();
GtkWidget *gtk_crea_pulsanti_editor_u();

// funzioni contarisorse
GtkWidget *gtk_crea_contarisorse();
void gtk_aggiorna_contarisorse();

// funzioni giocatore attuale
GtkWidget *gtk_crea_giocatore_c();
void gtk_aggiorna_giocatore_c();
