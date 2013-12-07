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

#ifndef FANTASY_GTK_MAP_H_
#define FANTASY_GTK_MAP_H_

#include <gtk/gtk.h>
#include "fantasy-core.h"

//funzioni mappa
void gtk_pulisci_mappa();
void gtk_pulisci_caselle();
void gtk_stampa_mappa(int x, int y, char m);
void gtk_genera_mappa(GtkWidget *Mappa);

#endif /* FANTASY_GTK_MAP_H_ */
