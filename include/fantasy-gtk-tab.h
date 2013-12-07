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

#ifndef FANTASY_GTK_TAB_H_
#define FANTASY_GTK_TAB_H_

#include <gtk/gtk.h>

//funzioni footer
GtkWidget *gtk_crea_footer();

//funzioni tab
GtkWidget *gtk_crea_notebook_tab(GtkWidget *Notebook,char *buf);
GtkWidget *gtk_crea_notebook(GtkWidget *Frame);
GtkWidget *gtk_crea_elemento_tab(GtkWidget *tab,int x, int y,char *nome, char *buf);
GtkWidget *gtk_riempi_tab_castelli(int i, char* buf);
void gtk_aggiorna_tab();
void gtk_pulisci_tab(GtkWidget *Target);
void gtk_azzera_tab();

void gtk_inizializza_tab();


#endif /* FANTASY_GTK_TAB_H_ */
