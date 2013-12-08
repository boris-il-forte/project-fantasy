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

#ifndef FANTASY_GTK_PARTITA_H_
#define FANTASY_GTK_PARTITA_H_

#include <gtk/gtk.h>

int gtk_nuova_partita();
int gtk_salva_partita();
int gtk_carica_partita();
int gtk_carica_avvio(char *nomefile);



#endif /* FANTASY_GTK_PARTITA_H_ */
