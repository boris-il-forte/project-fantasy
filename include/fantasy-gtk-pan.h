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

#ifndef FANTASY_GTK_PAN_H_
#define FANTASY_GTK_PAN_H_

#include <gtk/gtk.h>

//funzioni frecce
GtkWidget *gtk_crea_4_frecce();
void gtk_aggiorna_coordinate(int x, int y);

// funzioni contarisorse
GtkWidget *gtk_crea_contarisorse();
void gtk_aggiorna_contarisorse();

// funzioni giocatore attuale
GtkWidget *gtk_crea_giocatore_c();
void gtk_aggiorna_giocatore_c();

// funzioni footer
GtkWidget *gtk_crea_footer();

#endif /* FANTASY_GTK_PAN_H_ */
