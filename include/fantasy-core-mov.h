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

#ifndef FANTASY_CORE_MOV_H_
#define FANTASY_CORE_MOV_H_

#include "fantasy-core.h"

//funzioni di controllo spostamento e attacco
void calcolaspostamento(int Mossa);
int bersagliolecito(int PosT, int PosC);
int assaltolecito(int PosT, int PosC);
int spostalecito(int PosT, int PosC);
int raggiungibile(int posizioneStruttura, t_struttura tipo, int posizioneTruppa);
int assaltabile(int posizioneStruttura, t_struttura tipo, int posizioneTruppa);
int calcolaresiduo(int x, int y);


#endif /* FANTASY_CORE_MOV_H_ */
