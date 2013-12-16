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

#include "fantasy-gtk-key.h"
#include "fantasy-gtk-img.h"
#include "fantasy-gtk-map.h"
#include "fantasy-gtk-pan.h"
#include "fantasy-core.h"

#include <gdk/gdkkeysyms.h>

#define MAXCODAKEY 5

//implementazione di una coda di pressione di chiavi
struct keypress_s
{
	int first;
	int last;
	unsigned int keyval[20];
} keypress;

//callbacks

static void input_tastiera(GtkWidget* Window, GdkEventKey* K,
		struct keypress_s *keypress)
{
	int F;
	int L;
	if (partita_in_corso == 0)
		return;
	F = keypress->first;
	L = keypress->last;
	if (F == ((L + 1 < MAXCODAKEY) ? (L + 1) : (0)))
		return;
	if (K->type == GDK_KEY_PRESS && Window != NULL)
	{
		keypress->keyval[L] = K->keyval;
		L++;
		keypress->last = (L < MAXCODAKEY) ? (L) : (0);
	}

	return;
}

static gboolean leggistream_tastiera(struct keypress_s *keypress)
{
	int F = keypress->first;
	int L = keypress->last;
	if (F == L)
		return TRUE;
	switch (keypress->keyval[F])
	{
	case GDK_KEY_Left:
		if (cx - 1 >= 0)
		{
			cx -= 1;
			gtk_pulisci_mappa();
			gtk_aggiorna_coordinate(cx, cy);
			gtk_stampa_mappa(cx, cy, 'p');
		}
		break;
	case GDK_KEY_Up:
		if (cy - 1 >= 0)
		{
			cy -= 1;
			gtk_pulisci_mappa();
			gtk_aggiorna_coordinate(cx, cy);
			gtk_stampa_mappa(cx, cy, 'p');
		}
		break;
	case GDK_KEY_Down:
		if (cy + 1 <= ALTEZZA - caselle_verticali)
		{
			cy += 1;
			gtk_pulisci_mappa();
			gtk_aggiorna_coordinate(cx, cy);
			gtk_stampa_mappa(cx, cy, 'p');
		}
		break;
	case GDK_KEY_Right:
		if (cx + 1 <= LARGHEZZA - caselle_orizzontali)
		{
			cx += 1;
			gtk_pulisci_mappa();
			gtk_aggiorna_coordinate(cx, cy);
			gtk_stampa_mappa(cx, cy, 'p');
		}
		break;
	case GDK_KEY_Escape:
		gtk_pulisci_mappa();
		gtk_aggiorna_coordinate(cx, cy);
		gtk_stampa_mappa(cx, cy, 'n');
		break;
	default:
		break;
	}
	F++;
	keypress->first = (F < MAXCODAKEY) ? (F) : (0);
	return TRUE;
}

void gtk_start_stream_tastiera(GtkWidget* finestra)
{
	keypress.first = 0;
	keypress.last = 0;

	g_signal_connect(finestra, "key-press-event", G_CALLBACK(input_tastiera),
			(gpointer )&keypress);
	gdk_threads_add_timeout_full(G_PRIORITY_DEFAULT_IDLE, 50,
			(GSourceFunc) (leggistream_tastiera), (gpointer) &keypress, NULL);

}

