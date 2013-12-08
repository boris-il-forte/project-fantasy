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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include "fantasy-core.h"
#include "fantasy-gtk.h"

#define MAXCODAKEY 5

//implementazione di una coda di pressione di chiavi
struct keypress_s
{
	int first;
	int last;
	unsigned int keyval[20];
};

int need_resize = 0;

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

gboolean leggistream_tastiera(struct keypress_s *keypress)
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
			gtk_stampa_mappa(cx, cy, 'p');
		}
		break;
	case GDK_KEY_Up:
		if (cy - 1 >= 0)
		{
			cy -= 1;
			gtk_pulisci_mappa();
			gtk_stampa_mappa(cx, cy, 'p');
		}
		break;
	case GDK_KEY_Down:
		if (cy + 1 <= ALTEZZA - caselle_verticali)
		{
			cy += 1;
			gtk_pulisci_mappa();
			gtk_stampa_mappa(cx, cy, 'p');
		}
		break;
	case GDK_KEY_Right:
		if (cx + 1 <= LARGHEZZA - caselle_orizzontali)
		{
			cx += 1;
			gtk_pulisci_mappa();
			gtk_stampa_mappa(cx, cy, 'p');
		}
		break;
	case GDK_KEY_Escape:
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx, cy, 'n');
		break;
	default:
		break;
	}
	F++;
	keypress->first = (F < MAXCODAKEY) ? (F) : (0);
	return TRUE;
}

// main
int main(int argc, char *argv[])
{
	GtkWidget *finestra;
	struct keypress_s keypress;

	//inizializza
	gtk_init(&argc, &argv);
	partita_in_corso = 0;
	caricaconfig("fantasy.config");
#ifdef DEBUG
	printf("infogioco.skin=%s\n",infogioco.skin);
	printf("infogioco.ext=%s\n",infogioco.ext);
#endif
	keypress.first = 0;
	keypress.last = 0;

	finestra = gtk_crea_finestra_principale();
	//carica all'avvio
	if (argc > 1 && gtk_carica_avvio(argv[argc - 1]))
		return 1;

	// 	visualizza finestra
	gtk_widget_show(finestra);
	g_signal_connect(finestra, "key-press-event", G_CALLBACK(input_tastiera),
			(gpointer )&keypress);
	gdk_threads_add_timeout_full(G_PRIORITY_DEFAULT_IDLE, 50,
			(GSourceFunc) (leggistream_tastiera), (gpointer) &keypress, NULL);
	gtk_main();

	return 0;
}
