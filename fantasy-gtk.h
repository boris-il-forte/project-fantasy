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

/*header funzioni gtk*/

//funzioni generiche
void gtk_carica_immagini ();
void gtk_calcola_dimensioni ();
void gtk_inizializza_widget ();

//funzioni menu
void gtk_crea_menu (GtkWidget *Vbox);

//funzioni mappa
void gtk_pulisci_mappa ();
void gtk_stampa_mappa(int x, int y, char m);
void gtk_genera_mappa (GtkWidget *Hbox);

//funzioni tab
GtkWidget *gtk_crea_notebook_tab (GtkWidget *Notebook,char *buf);
void gtk_crea_notebook (GtkWidget *Frame);
GtkWidget *gtk_crea_elemento_tab(GtkWidget *tab,int x, int y,char *buf);
GtkWidget * gtk_riempi_tab_castelli (int i, char* buf);
void gtk_aggiorna_tab_armate ();
void gtk_aggiorna_tab_castelli ();
void gtk_aggiorna_tab_strutture ();
void gtk_pulisci_tab (GtkWidget *Target);
void gtk_azzera_tab ();

//funzioni freccie
GtkWidget *gtk_crea_4_frecce();

// funzioni contarisorse
GtkWidget *gtk_crea_contarisorse();
void gtk_aggiorna_contarisorse();
