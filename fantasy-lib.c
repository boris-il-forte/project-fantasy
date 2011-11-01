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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "fantasy-core.h"

//carica dati unità
void caricadati ()
{
	//dati recluta
	Dtruppa[Rec].att=15;
	Dtruppa[Rec].dif=5;
	Dtruppa[Rec].git=2;
	Dtruppa[Rec].vel=10;
	Dtruppa[Rec].mor=25;
	//dati fante
	Dtruppa[Fan].att=40;
	Dtruppa[Fan].dif=15;
	Dtruppa[Fan].git=1;
	Dtruppa[Fan].vel=5;
	Dtruppa[Fan].mor=50;
	//dati lanciere
	Dtruppa[Lan].att=15;
	Dtruppa[Lan].dif=25;
	Dtruppa[Lan].git=1;
	Dtruppa[Lan].vel=5;
	Dtruppa[Lan].mor=40;
	//dati arciere
	Dtruppa[Arc].att=20;
	Dtruppa[Arc].dif=5;
	Dtruppa[Arc].git=20;
	Dtruppa[Arc].vel=10;
	Dtruppa[Arc].mor=30;
	//dati cavaliere
	Dtruppa[Cav].att=90;
	Dtruppa[Cav].dif=5;
	Dtruppa[Cav].git=1;
	Dtruppa[Cav].vel=20;
	Dtruppa[Cav].mor=75;
	//dati drago
	Dtruppa[Dra].att=2000;
	Dtruppa[Dra].dif=2000;
	Dtruppa[Dra].git=2;
	Dtruppa[Dra].vel=30;
	Dtruppa[Dra].mor=100;
	//dati fenice
	Dtruppa[Fen].att=2000;
	Dtruppa[Fen].dif=800;
	Dtruppa[Rec].git=2;
	Dtruppa[Fen].vel=35;
	Dtruppa[Fen].mor=80;
}

//inizializza le strutture dati 
void inizializza ()
{
	static int h=0;
	int i,j;
	if(h++!=0) liberaheap ();
	srand((unsigned)time(NULL));
	for (i=0; i<NUMCASTELLI; i++) infomappa.castelli[i]=-1;
	for (i=0; i<MAXSTALLE; i++) infomappa.stalle[i]=-1;
	for (i=0; i<MAXGROTTE; i++) infomappa.grotte[i]=-1;
	for (i=0; i<MAXNIDI; i++) infomappa.nidi[i]=-1;
	for (i=0; i<MAXGIOCATORI; i++) giocatore[i]=NULL;
	for (i=0; i<ALTEZZA; i++)
		for(j=0; j<LARGHEZZA; j++)
		{
			accedi(j,i,infomappa.mappa)=' ';
			accedi(j,i,infomappa.truppe)=NULL;
		}
	infomappa.numstalle=rand()%(MAXSTALLE/2)+(MAXSTALLE/2+1); //genera il numero delle stalle tra MAXSTALLE e MAXSTALLE/2;
	infomappa.numfattorie=rand()%(MAXFATTORIE/2)+(MAXFATTORIE/2+1);//genera il numero delle stalle tra MAXFATTORIE e MAXFATTORIE/2;
	infomappa.numgrotte=rand()%MAXGROTTE+1; //genera il numero delle grotte 
	infomappa.numnidi=rand()%MAXNIDI+1; //genera il numero dei nidi	
}

// controlla che i castelli generati siano sufficentemente distanti l'uno dall'altro
int conflitticastello(int x, int y,int cur)
{
	int i,j,k;
	int const pos=x+LARGHEZZA*y;
	if(x!=0 && y!=0 && x<LARGHEZZA-1 && y<ALTEZZA-1)
	{
		for (i=0; i<NUMCASTELLI; i++)
			for (j=-10; j<=10; j++)
				for (k=-10; k<=10; k++)
					if(i!=cur && pos+k-LARGHEZZA*j==infomappa.castelli[i]) return 1;
		return 0;
	}
	else return 1;
}
// controlla che le strutture della mappa non si sovrappongano e siano sufficentemente distanti da castelli e altre strutture 
int conflittomappa(int x, int y)
{
	int i,j;
	if(x!=0 && y!=0 && x<LARGHEZZA-1 && y<ALTEZZA-1)
	{
		for (i=-3; i<=4; i++)
			for (j=-3; j<=4; j++)
				if (accedi(x+i,y+j,infomappa.mappa)!=' ') return 1;
	}
	else return 1;
	return 0;
}
//genera la mappa casuale
void generamappa ()
{
	int i; //contatori 
	int x,y; //variabili posizionamento
	srand((unsigned)time(NULL)); //randomizza col tempo
	/*genero i castelli*/
	for (i=0; i<NUMCASTELLI; i++) 
	{
		do
		{
			infomappa.castelli[i]=rand()%(ALTEZZA*LARGHEZZA);
			x=infomappa.castelli[i]%LARGHEZZA;
			y=infomappa.castelli[i]/LARGHEZZA;
		}
		while (conflitticastello(x,y,i));
		accedi(x,y,infomappa.mappa)='4'; //segna i castelli
		accedi(x-1,y,infomappa.mappa)='3';
		accedi(x+1,y,infomappa.mappa)='5';
		accedi(x,y+1,infomappa.mappa)='7';
		accedi(x,y-1,infomappa.mappa)='1';
		accedi(x-1,y+1,infomappa.mappa)='6';
		accedi(x-1,y-1,infomappa.mappa)='0';
		accedi(x+1,y+1,infomappa.mappa)='8';
		accedi(x+1,y-1,infomappa.mappa)='2';
	}
	/*genero le stalle*/
	for (i=0; i<infomappa.numstalle; i++) 
	{
		do
		{
			infomappa.stalle[i]=rand()%(ALTEZZA*LARGHEZZA);
			x=infomappa.stalle[i]%LARGHEZZA;
			y=infomappa.stalle[i]/LARGHEZZA;
		}
		while (conflittomappa(x,y));
		disegna(x,y,'S'); //segna le stalle
	}
	/*genero le fattorie*/
	for (i=0; i<infomappa.numfattorie; i++) 
	{
		do
		{
			infomappa.fattorie[i]=rand()%(ALTEZZA*LARGHEZZA);
			x=infomappa.fattorie[i]%LARGHEZZA;
			y=infomappa.fattorie[i]/LARGHEZZA;
		}
		while (conflittomappa(x,y));
		disegna(x,y,'C'); //segna le fattorie
	}
	/*genero le grotte*/
	for (i=0; i<infomappa.numgrotte; i++) 
	{
		do
		{
			infomappa.grotte[i]=rand()%(ALTEZZA*LARGHEZZA);
			x=infomappa.grotte[i]%LARGHEZZA;
			y=infomappa.grotte[i]/LARGHEZZA;
		}
		while (conflittomappa(x,y));
		disegna(x,y,'G');
	}
	/*genero i nidi*/
	for (i=0; i<infomappa.numnidi; i++) 
	{
		do
		{
			infomappa.nidi[i]=rand()%(ALTEZZA*LARGHEZZA);
			x=infomappa.nidi[i]%LARGHEZZA;
			y=infomappa.nidi[i]/LARGHEZZA;
		}
		while (conflittomappa(x,y));
		disegna(x,y,'N');
	}
}

//inizializza la struttura dati giocatori
void creagiocatori (int n)
{
	int i,j;
	char al;
	int num[MAXGIOCATORI];
	for (i=0;i<n;i++) num[i]=NUMCASTELLI;
	for (i=0;i<n;i++)
	{
		srand((unsigned)time(NULL));
		do
		{
			al=rand()%NUMCASTELLI;
			for(j=0;j<i && al!=num[j]; j++);
		} while (j<i);
		num[i]=al;
	}
	for (i=0; i<n; i++)
	{
		fprintf(stderr,"creo giocatore n:%d\n",i);
		giocatore[i]=(t_player*)malloc(sizeof(t_player));
		giocatore[i]->oro=1000;
		giocatore[i]->cibo=1000;
		giocatore[i]->smeraldi=200;
		giocatore[i]->struttura[Cas]=(t_lista_s*)malloc(sizeof(t_lista_s));
		giocatore[i]->struttura[Cas]->pos=infomappa.castelli[num[i]];
		giocatore[i]->struttura[Cas]->in=NULL;
		giocatore[i]->struttura[Cas]->next=NULL;
		giocatore[i]->truppe=NULL;
		for (j=1;j<NUMSTRUTTURE;j++) giocatore[i]->struttura[j]=NULL;
	}
	while(i<MAXGIOCATORI)
	{
		giocatore[i]=NULL;
		i++;
	}
}
 
//genera l'unità desiderata
t_infotruppa* generatruppa (t_truppa Tipo, char Giocatore, int Numero)
{
	t_infotruppa* T;
	T=(t_infotruppa*)malloc(sizeof(t_infotruppa));
	T->tipo=Tipo;
	T->giocatore=Giocatore;
	T->numero=Numero;
	T->morale=Dtruppa[Tipo].mor;
	T->stanca=0;
	T->combattuto=0;
	return T;
}

//libera le strutture dati allocate
void liberaheap ()
{
	int i,j;
	t_lista_s* S;
	t_lista_s* Sn;
	t_lista_t* T;
	t_lista_t* Tn;
	for (i=0; i<MAXGIOCATORI && giocatore[i]!=NULL; i++) 
	{
		for(j=0;j<NUMSTRUTTURE; j++)
		{
			S=giocatore[i]->struttura[j];
			while(S!=NULL)
			{
				T=S->in;
				while (T!=NULL)
				{
					Tn=T->next;
					free(T);
					T=Tn;
				}
				Sn=S->next;
				free(S);
				S=Sn;
			}
		}
		T=giocatore[i]->truppe;
		while(T!=NULL)
		{
			Tn=T->next;
			free(T);
			T=Tn;
		}
		free(giocatore[i]);
	}
	for (i=0; i<ALTEZZA*LARGHEZZA; i++) free(infomappa.truppe[i]);
}

// calcola se lo spostamento è lecito
int spostalecito (int PosT, int PosC )
{
	t_truppa Tipo= infomappa.truppe[PosT]->tipo;
	char Vel=Dtruppa[Tipo].vel;
	int x= PosT%LARGHEZZA-PosC%LARGHEZZA;
	int y= PosT/LARGHEZZA-PosC/LARGHEZZA;
	if(x*x+y*y<=Vel*Vel)
		return 1;
	else
		return 0;	
}

//calcola area bersaglio
int bersagliolecito (int PosT, int PosC )
{
	t_truppa Tipo= infomappa.truppe[PosT]->tipo;
	char Git=Dtruppa[Tipo].git;
	int x= PosT%LARGHEZZA-PosC%LARGHEZZA;
	int y= PosT/LARGHEZZA-PosC/LARGHEZZA;
	if(x*x+y*y<=Git*Git)
		return 1;
	else
		return 0;
}

//calcola area assedio
int assaltolecito (int PosT, int PosC )
{
	int x= PosT%LARGHEZZA-PosC%LARGHEZZA;
	int y= PosT/LARGHEZZA-PosC/LARGHEZZA;
	if(x*x+y*y<=1)
		return 1;
	else
		return 0;
}

//elimina i puntatori alle unita morte
void eliminamorti (t_infotruppa** M)
{
	int G=(*M)->giocatore;
	int Pos;
	t_lista_t* T=giocatore[G]->truppe;
	t_lista_t* Tp;
	while(T!=NULL && T->truppa->numero!=0)
	{
		Tp=T;
		T=T->next;
	}
	if(T!=NULL)
	{
		Pos=T->pos;
		Tp->next=T->next;
		free(T);
		free(*M);
		*M=NULL;
		infomappa.truppe[Pos]=NULL;
	}
	else fprintf(stderr,"o breccia con perdite o c'e' un bug!\n");
}

//fa combattere due unita'
void combatti(t_infotruppa* Attaccante, t_infotruppa* Difensore, char m)
{
	int Pa,Pd;
	int Bp,Bm;
	int n;
	float p;
	fprintf(stderr,"debug: combatti\n");
	//calcolo potenza attacco attaccante
	n=Attaccante->numero;
	Bp=Dtruppa[Attaccante->tipo].att;
	Bm=Attaccante->morale;
	Pa=Bp*n+Bm*n/10;
	//calcolo potenza difesa difensore
	n=Difensore->numero;
	Bp=Dtruppa[Difensore->tipo].dif;
	Bm=Difensore->morale;
	Pd=Bp*n+Bm*n/10;
	switch (m)
	{
		case 's':
			Pd+=150;
		break;
		case 'c':
			Pd+=300;
		break;
		case 'n':
		default:
		break;
	}
	//calcola danni
	p=(float)Pd/(float)Pa;
	if (p<0.95) p*=n;
	else p=0.95*n;
	//evita creazione di truppe da combattimento
	if (p>=n) p=n;
	// gestisce le unità di pochi uomini
	if (Difensore->tipo<Dra)
	{
		if (p<5) n=0;
		else if (Difensore->tipo<Dra) n=(int)p;
	}
	else
	{
		if (p-0.5<(int)p) n=(int)p;
		else n=(int)p+1;
	}
	//gestisce il morale
	if (n<Difensore->numero/2) Difensore->morale/=2;
	Difensore->numero=n;
	return;
}
//fa combattere gli attaccanti contro i difensori di una struttura
int assaltaedificio(t_lista_s* Edificio)
{
	fprintf(stderr,"debug: assaltamura\n");
	int i,j=0;
	int Pos=Edificio->pos;
	t_infotruppa* Attaccanti[8];
	t_infotruppa* Difensori[8];
	t_lista_t* T;
	T=Edificio->in;
	if (T==NULL) return 1;
	for(i=0; i<8; i++) Difensori[i]=NULL;
	for(i=0; i<8; i++) Attaccanti[i]=NULL;
	do
	{
		Attaccanti[j]=puntacasellaoccupata(Pos,j);
		j++;
	}while(Attaccanti[j-1]==NULL && j<8);
	for (i=0; T!=NULL && i<j && i<8; i++)
	{
		Difensori[i]=T->truppa;
		T=T->next;
	}
	for(i=0;i<j && Difensori[i]!=NULL;i++)
	{
		Attaccanti[i]->combattuto=1;
		Difensori[i]->combattuto=1;
		combatti(Attaccanti[i],Difensori[i],'s');
		if(Difensori[i]->numero==0)
		{
			eliminamorti(&Difensori[i]);
			return 1;
		}
		combatti(Difensori[i],Attaccanti[i],'n');
		if(Attaccanti[i]->numero==0) eliminamorti(&Attaccanti[i]);
	}
	return 0;
}
//fa combattere gli attaccanti contro la prima linea di un castello presidiato
int assaltamura(t_lista_s* Castello)
{
	fprintf(stderr,"debug: assaltamura\n");
	int i,j=0;
	int Pos=Castello->pos;
	t_infotruppa* Attaccanti[12];
	t_infotruppa* Difensori[12];
	t_lista_t* T;
	T=Castello->in;
	if (T==NULL) return 1;
	for(i=0; i<12; i++) Difensori[i]=NULL;
	for(i=0; i<12; i++) Attaccanti[i]=NULL;
	do
	{
		Attaccanti[j]=puntacasellaoccupata(Pos,j);
		j++;
	}while(Attaccanti[j-1]==NULL && j<12);
	for (i=0; T!=NULL && i<j && i<12; i++)
	{
		Difensori[i]=T->truppa;
		T=T->next;
	}
	for(i=0;i<j && Difensori[i]!=NULL;i++)
	{
		Attaccanti[i]->combattuto=1;
		Difensori[i]->combattuto=1;
		combatti(Attaccanti[i],Difensori[i],'m');
		if(Difensori[i]->numero==0)
		{
			eliminamorti(&Difensori[i]);
			return 1;
		}
		combatti(Difensori[i],Attaccanti[i],'n');
		if(Attaccanti[i]->numero==0) eliminamorti(&Attaccanti[i]);
	}
	return 0;
}

//fa combattere gli attaccanti contro la seconda linea di un castello presidiato
int assaltabreccia(t_infotruppa* Attaccante,t_lista_t* Difensori)
{
	t_lista_t* Temp;
	fprintf(stderr,"debug: assaltabreccia\n");
	while (Difensori!=NULL)
	{
		combatti(Attaccante,Difensori->truppa,'n');
		if(Difensori->truppa->numero==0 || Difensori->truppa->morale==0)
		{
			//abbassa il morale dei difensori
			Temp=Difensori->next;
			while (Temp!=NULL)
			{
				Temp->truppa->morale-=10;
				if (Temp->truppa->morale<0) Temp->truppa->morale=0;
				Temp=Temp->next;
			}
			//fa morire le truppe in rotta o morte
			Temp=Difensori;
			while(Temp!=NULL)
			{
				if(Temp->truppa->morale==0 || Temp->truppa->numero==0) eliminamorti(&Temp->truppa);
				Temp=Temp->next;
			}
		}
		else
		{
			combatti(Difensori->truppa,Attaccante,'n');
			if(Attaccante->numero==0 || Attaccante->morale==0) eliminamorti(&Attaccante);
			return 0;
		}
	}
	return 1;
}

//cambia il proprietario a un edificio
void cambiaproprietario (int g1, int g2, int Pos,t_struttura Tipo)
{
	t_lista_s* S;
	t_lista_s* Sp;
	fprintf(stderr,"debug: cambiaproprietario\n");
	if(g2<0)
	{
		S=giocatore[g1]->struttura[Tipo];
		if(S!=NULL)
		{
			while (S->next!=NULL) S=S->next;
			S->next=(t_lista_s*)malloc(sizeof(t_lista_s));
			S->next->in=NULL;
			S->next->pos=Pos;
			S->next->next=NULL;
		}
		else
		{
			giocatore[g1]->struttura[Tipo]=(t_lista_s*)malloc(sizeof(t_lista_s));
			giocatore[g1]->struttura[Tipo]->in=NULL;
			giocatore[g1]->struttura[Tipo]->pos=Pos;
			giocatore[g1]->struttura[Tipo]->next=NULL;
		}
	}
	else
	{
		S=giocatore[g2]->struttura[Tipo];
		Sp=NULL;
		while (S->pos!=Pos)
		{
			Sp=S;
			S=S->next;
		}
		if(Sp!=NULL) Sp->next=S->next;
		else giocatore[g2]->struttura[Tipo]=NULL;
		Sp=giocatore[g1]->struttura[Tipo];
		while(Sp->next!=NULL) Sp=Sp->next;
		Sp->next=S;
		S->next=NULL;
	}
}

//controlla se la struttura e' del giocatore
int controlloedificio (int Pos, t_struttura s)
{
	int i;
	t_lista_s* L;
	for (i=0; i<MAXGIOCATORI && giocatore[i]!=NULL; i++)
	{
		L=giocatore[i]->struttura[s];
		while(L!=NULL)
		{
			if(L->pos==Pos) return i;
			else L=L->next;
		}
	}
	return -1;
}

//controlla se la unita' e' del giocatore
int controllounita (int Pos)
{
	int i;
	t_lista_t* L;
	for(i=0; i<MAXGIOCATORI && giocatore[i]!=NULL; i++)
	{
		L=giocatore[i]->truppe;
		while(L!=NULL)
		{
			if(L->pos==Pos) return i;
			else L=L->next;
		}
	}
	return -1;
}

//controlla che unità e struttura non siano dello stesso giocatore
int controllodiverso(int PosT, int PosS, t_struttura tipo)
{
	if (controlloedificio (PosS, tipo)==controllounita (PosT)) return 0;
	else return 1;
}

//punta alla lista struttura della struttura nella posizione desiderata
t_lista_s* puntastruttura (int Pos)
{
	t_struttura i;
	t_lista_s* L;
	for(i=Cas; i<=Nid; i++)
	{
		L=giocatore[0]->struttura[i];
		while(L!=NULL)
		{
			if(L->pos==Pos) return L;
			else L=L->next;
		}
	}
	return NULL;
}

//punta alla prima casella libera disponibile attorno a una struttura
t_infotruppa** puntacasellalibera (int Pos)
{
	int x, y;
	int i,j;
	int hi,hj,bj,bi;
	x=Pos%LARGHEZZA;
	y=Pos/LARGHEZZA;
	if(infomappa.mappa[Pos-1]!=' ')
	{
		bi=-2;
		bj=-2;
		//evita di andare fuori dal limite basso
		if (x+bi<0) bi++;
		if (y+bj<0) bj++;
		//evita di andare fuori dal limite alto
		for (hi=1;x+hi<LARGHEZZA && hi<2;hi++);
		for (hj=1;y+hj<ALTEZZA && hj<2;hj++);
		//iniziallizza i e j
		i=bi;
		j=bj;
		while((accedi(x+i,y+j,infomappa.truppe)!=NULL || accedi(x+i,y+j,infomappa.mappa)!=' ') && j<=hj)
		{
			i++;
			if (i>hi)
			{
				i=bi;
				j++;
			}
		}
		if(j>hj) return NULL;
		else 
		{
			x+=i;
			y+=j;
			return &infomappa.truppe[x+y*LARGHEZZA];
		}
	}
	else
	{
		bi=-1;
		bj=-1;
		//evita di andare fuori dal limite basso
		if (x+bi<0) bi++;
		if (y+bj<0) bj++;
		//evita di andare fuori dal limite alto
		for (hi=1;x+hi<LARGHEZZA && hi<=2;hi++);
		for (hj=1;y+hj<ALTEZZA && hj<=2;hj++);
		//iniziallizza i e j
		i=bi;
		j=bj;
		while((accedi(x+i,y+j,infomappa.truppe)!=NULL || accedi(x+i,y+j,infomappa.mappa)!=' ') && j<=hj)
		{
			i++;
			if (i>hi)
			{
				i=bi;
				j++;
			}
		}
		if(j>hj) return NULL;
		else 
		{
			x+=i;
			y+=j;
			return &infomappa.truppe[x+y*LARGHEZZA];
		}
	}
	return NULL;
}

//punta alla prossima casella occupata attorno a una struttura
t_infotruppa* puntacasellaoccupata (int Pos, int C)
{
	int x, y;
	int i,j,k=0;
	int hi,hj,bj,bi,m;
	x=Pos%LARGHEZZA;
	y=Pos/LARGHEZZA;
	fprintf(stderr,"debug: puntacasellaoccupata\n");
	if(infomappa.mappa[Pos-1]!=' ')
	{
		bi=-2;
		bj=-2;
		m=-2;
		//evita di andare fuori dal limite basso
		if (x+bi<0) bi++;
		if (y+bj<0) bj++;
		//evita di andare fuori dal limite alto
		for (hi=1;x+hi<LARGHEZZA && hi<2;hi++);
		for (hj=1;y+hj<ALTEZZA && hj<2;hj++);
	}
	else
	{
		bi=-1;
		bj=-1;
		m=-2;
		//evita di andare fuori dal limite basso
		if (x+bi<0) bi++;
		if (y+bj<0) bj++;
		//evita di andare fuori dal limite alto
		for (hi=1;x+hi<LARGHEZZA && hi<2;hi++);
		for (hj=1;y+hj<ALTEZZA && hj<2;hj++);
	}
	for(j=bj;k<=C && j<=hj;j++)
		for(i=bi;k<=C && i<=hi;i++)
		{
			if(!((i==m|| i==2) && (j==m || j==2)))
			{
				if ((accedi(x+i,y+j,infomappa.truppe)!=NULL)) k++;
			}
		}
	i--;
	j--;
	if(k==C)
	{
		C=0;
		return NULL;
	}
	else
	{
		C++;
		x+=i;
		y+=j;
		return infomappa.truppe[x+y*LARGHEZZA];
	}
}

//termina il turno
void fineturno()
{
	int i;
	int r[NUMSTRUTTURE];
	int c=0;
	t_lista_t* T=giocatore[0]->truppe;
	t_lista_s* S;
	for(i=0; i<NUMSTRUTTURE; i++) r[i]=0;
	while(T!=NULL)
	{
		T->truppa->stanca=0;
		T->truppa->combattuto=0;
		T=T->next;
		c++;
	}
	for(i=0;i<NUMSTRUTTURE; i++)
	{
		S=giocatore[0]->struttura[i];
		while (S!=NULL)
		{
			r[i]++;
			S=S->next;
		}
	}
	giocatore[0]->oro+=500*r[Cas]+200*r[Fat]+400*r[Scu]+100*r[Nid]+1000*r[Gro];
	giocatore[0]->smeraldi+=500*r[Nid]+100*r[Gro];
	giocatore[0]->cibo+=400*r[Cas]+1000*r[Fat]+100*r[Scu]-100*c;
	return;
}

// stampa la mappa
void visualizza_su_terminale () 
{
	int i,j;
	for (i=0; i<ALTEZZA; i++)
	{
		for(j=0; j<LARGHEZZA; j++)
		{
			printf("%c",accedi(j,i,infomappa.mappa)); 
		}
		printf("\n");
	}
}
