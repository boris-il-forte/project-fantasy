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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fantasy-core.h"

int carica(char *nomefile)
{
	FILE *fp;
	char ver[4];

	fp=fopen(nomefile,"r");
	if(fp == NULL) {
		perror("fopen fallita");
		return 1;
	}
	// INIZIO controllo header standard: FC1 (1 come intero)
	fread(ver,1,2,fp);
	ver[2]='\0';
	fread(&ver[3],1,1,fp);
	if(strcmp(ver,"FC") != 0) {
		fprintf(stderr,"MIME type sconosciuto. Intestazione errata: %c%c\n",ver[0],ver[1]);
		return 1;
	}
	if(ver[3] != 1) {
		fprintf(stderr,"Formato FantasyCore sconosciuto (supportato: v1). Versione: v%u ?\n",ver[2]);
		return 1;
	}
	// FINE controllo header standard
	fread((&infomappa)->castelli,sizeof(infomappa.castelli),NUMCASTELLI,fp);
	fread((&infomappa)->fattorie,sizeof(infomappa.fattorie),MAXFATTORIE,fp);
	fread((&infomappa)->stalle,sizeof(infomappa.stalle),MAXSTALLE,fp);
	fread((&infomappa)->grotte,sizeof(infomappa.grotte),MAXGROTTE,fp);
	fread((&infomappa)->nidi,sizeof(infomappa.nidi),MAXNIDI,fp);
	fread(&(&infomappa)->numstalle,sizeof(infomappa.numstalle),1,fp);
	fread(&(&infomappa)->numnidi,sizeof(infomappa.numnidi),1,fp);
	fread(&(&infomappa)->numgrotte,sizeof(infomappa.numgrotte),1,fp);
	fread(&(&infomappa)->numfattorie,sizeof(infomappa.numfattorie),1,fp);
	fclose(fp);

	return 0;
}

int salva(char *nomefile)
{
	FILE *fp;
	char ver[3];

	ver[0]='F';
	ver[1]='C';
	ver[2]=1;

	fp=fopen(nomefile,"w");
	if(fp == NULL) {
		perror("fopen fallita");
		return 1;
	}
	fwrite(ver,1,3,fp);
	fwrite((&infomappa)->castelli,sizeof(infomappa.castelli),NUMCASTELLI,fp);
	fwrite((&infomappa)->fattorie,sizeof(infomappa.fattorie),MAXFATTORIE,fp);
	fwrite((&infomappa)->stalle,sizeof(infomappa.stalle),MAXSTALLE,fp);
	fwrite((&infomappa)->grotte,sizeof(infomappa.grotte),MAXGROTTE,fp);
	fwrite((&infomappa)->nidi,sizeof(infomappa.nidi),MAXNIDI,fp);
	fwrite(&(&infomappa)->numstalle,sizeof(infomappa.numstalle),1,fp);
	fwrite(&(&infomappa)->numnidi,sizeof(infomappa.numnidi),1,fp);
	fwrite(&(&infomappa)->numgrotte,sizeof(infomappa.numgrotte),1,fp);
	fwrite(&(&infomappa)->numfattorie,sizeof(infomappa.numfattorie),1,fp);
	fclose(fp);

	return 0;
}
