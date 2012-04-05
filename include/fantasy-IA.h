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


//define utili
#define MOLTO 100
#define NORMALE 50
#define POCO 20

//funzione per assegnare le IA random
void fantasia_assegna_ia_random(int numIA, int numG);

//funzioni IA
void fantasia_create_player(int num, char type, *thread);
t_iadata fantasia_giocatore_inizializza(char modo);
void fantasia_giocatore_artificiale(t_iaparam *param);

//funzioni di appoggio

