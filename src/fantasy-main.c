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
#include <stdio.h>
#include "fantasy-core.h"
#include "fantasy-gtk.h"

// main
int main(int argc, char *argv[])
{
	GtkWidget *finestra;

	gtk_init(&argc, &argv);
	partita_in_corso = 0;
	caricaconfig("fantasy.config");

#ifdef DEBUG
	printf("infogioco.skin=%s\n",infogioco.skin);
	printf("infogioco.ext=%s\n",infogioco.ext);
#endif

	finestra = gtk_crea_finestra_principale();

	if (argc > 1 && gtk_carica_avvio(argv[argc - 1]))
		return 1;

	gtk_start_stream_tastiera(finestra);

	gtk_widget_show(finestra);

	gtk_main();

	return 0;
}
