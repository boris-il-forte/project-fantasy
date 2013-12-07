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

#ifndef FANTASY_GTK_
#define FANTASY_GTK_

#include <gtk/gtk.h>

#include "fantasy-gtk-img.h"
#include "fantasy-gtk-map.h"
#include "fantasy-gtk-tab.h"

//variabili globali
GtkWidget *Counter[NUMRISORSE];
GtkWidget *Coordinate;
GtkWidget *CurrentI1;
GtkWidget *CurrentL;
GtkWidget *CurrentI2;


/*header funzioni gtk*/

//funzioni generiche
int gtk_carica_avvio(char *nomefile);
void gtk_proclama_vincitore(int g);

//funzioni menu
void gtk_crea_menu(GtkWidget *Vbox);

//funzioni frecce
GtkWidget *gtk_crea_4_frecce();
void gtk_aggiorna_coordinate(int x, int y);



//funzioni editor
GtkWidget *gtk_crea_pulsanti_editor_s();
GtkWidget *gtk_crea_pulsanti_editor_u();

// funzioni contarisorse
GtkWidget *gtk_crea_contarisorse();
void gtk_aggiorna_contarisorse();

// funzioni giocatore attuale
GtkWidget *gtk_crea_giocatore_c();
void gtk_aggiorna_giocatore_c();

// funzioni combattimento
void gtk_popup_combattimento(GtkWidget* Casella, int Perdite);

#endif
