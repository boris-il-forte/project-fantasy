/*
 * Project Fantasy, gioco di strategia a turni
 *       
 *
 * Copyright (C) 2011 gionnico
 * Versione 1.0
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdlib.h>
#include <stdio.h>
#include "fantasy-core.h"

int carica(char *nomefile)
{
	FILE *fp;

	fp=fopen(nomefile,"r");
	if(fp == NULL) {
		perror("fopen fallita");
		return 1;
	}
	if(fread(&infomappa,sizeof(infomappa),1,fp) < 1) {
		perror("fread fallita");
		return 1;
	}
	if(fread(giocatore,sizeof(*giocatore),maxgiocatori,fp) < maxgiocatori) {
		perror("fread fallita");
		return 1;
	}
	fclose(fp);

	return 0;
}

int salva(char *nomefile)
{
	FILE *fp;

	fp=fopen(nomefile,"w");
	if(fp == NULL) {
		perror("fopen fallita");
		return 1;
	}
	if(fwrite(&infomappa,sizeof(infomappa),1,fp) < 1) {
		perror("fwrite fallita");
		return 1;
	}
	if(fwrite(giocatore,sizeof(*giocatore),maxgiocatori,fp) < maxgiocatori) {
		perror("fwrite fallita");
		return 1;
	}
	fclose(fp);

	return 0;
}
