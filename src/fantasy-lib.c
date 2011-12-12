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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "fantasy-core.h"

#define INIZIO_ORO 1000
#define INIZIO_CIBO 1000
#define INIZIO_SMERALDI 200
#define PROD_ORO_CAS 500
#define PROD_ORO_FAT 200
#define PROD_ORO_SCU 400
#define PROD_ORO_NID 100
#define PROD_ORO_GRO 1000
#define PROD_SMERALDI_NID 500
#define PROD_SMERALDI_GRO 100
#define PROD_CIBO_CAS 400
#define PROD_CIBO_FAT 1000
#define PROD_CIBO_SCU 100
#define FAME 10

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
void inizializza()
{
	int i,j;
	if(partita_in_corso!=0) liberaheap ();
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

// assegna un valore di prato
int tipoprato (int Pos)
{
	return infomappa.prato[Pos];
}
//genera un prato vario
void pratoacaso ()
{
	int n,i,j;
	srand((unsigned)time(NULL));
	for (i=0; i<ALTEZZA; i++)
		for(j=0; j<LARGHEZZA; j++)
		{
			n=rand()%20;
			if (n<12) n=0;
			else if (n<16) n=1;
			else if (n<18) n=2;
			else if (n==18)n=3;
			else n=4;
			accedi(j,i,infomappa.prato)=n;
		}
}
//genera la mappa casuale
void generamappa ()
{
	int i; //contatori 
	int x,y; //variabili posizionamento

	srand((unsigned)time(NULL)); //randomizza col tempo

	infomappa.numstalle=rand()%(MAXSTALLE/2)+(MAXSTALLE/2+1); //genera il numero delle stalle tra MAXSTALLE e MAXSTALLE/2;
	infomappa.numfattorie=rand()%(MAXFATTORIE/2)+(MAXFATTORIE/2+1);//genera il numero delle stalle tra MAXFATTORIE e MAXFATTORIE/2;
	infomappa.numgrotte=rand()%MAXGROTTE+1; //genera il numero delle grotte 
	infomappa.numnidi=rand()%MAXNIDI+1; //genera il numero dei nidi	

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
		accedi(x-1,y-1,infomappa.mappa)='0'; //segna i castelli
		accedi(x,y-1,infomappa.mappa)='1';
		accedi(x+1,y-1,infomappa.mappa)='2';
		accedi(x-1,y,infomappa.mappa)='3';
		accedi(x,y,infomappa.mappa)='4';
		accedi(x+1,y,infomappa.mappa)='5';
		accedi(x-1,y+1,infomappa.mappa)='6';
		accedi(x,y+1,infomappa.mappa)='7';
		accedi(x+1,y+1,infomappa.mappa)='8';
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
	//crea prato vario
	pratoacaso ();
}

//genera la mappa caricata
void rigeneramappa ()
{
	int i; // contatori
	int x,y; //variabili posizionamento
	/*genero i castelli*/
	for (i=0; i<NUMCASTELLI; i++) {
		x=infomappa.castelli[i]%LARGHEZZA;
		y=infomappa.castelli[i]/LARGHEZZA;
		accedi(x-1,y-1,infomappa.mappa)='0'; //segna i castelli
		accedi(x,y-1,infomappa.mappa)='1';
		accedi(x+1,y-1,infomappa.mappa)='2';
		accedi(x-1,y,infomappa.mappa)='3';
		accedi(x,y,infomappa.mappa)='4';
		accedi(x+1,y,infomappa.mappa)='5';
		accedi(x-1,y+1,infomappa.mappa)='6';
		accedi(x,y+1,infomappa.mappa)='7';
		accedi(x+1,y+1,infomappa.mappa)='8';
	}
	/*genero le fattorie*/
	for (i=0; i<infomappa.numstalle; i++) {
		x=infomappa.fattorie[i]%LARGHEZZA;
		y=infomappa.fattorie[i]/LARGHEZZA;
		disegna(x,y,'C'); //segna le fattorie
	}
		/*genero le stalle*/
	for (i=0; i<infomappa.numstalle; i++) {
		x=infomappa.stalle[i]%LARGHEZZA;
		y=infomappa.stalle[i]/LARGHEZZA;
		disegna(x,y,'S'); //segna le stalle
	}
	/*genero le grotte*/
	for (i=0; i<infomappa.numgrotte; i++) {
		x=infomappa.grotte[i]%LARGHEZZA;
		y=infomappa.grotte[i]/LARGHEZZA;
		disegna(x,y,'G');
	}
		/*genero i nidi*/
	for (i=0; i<infomappa.numnidi; i++) {
		x=infomappa.nidi[i]%LARGHEZZA;
		y=infomappa.nidi[i]/LARGHEZZA;
		disegna(x,y,'N');
	}
	//visualizza su terminale
	visualizza_su_terminale();
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
		giocatore[i]->oro=INIZIO_ORO;
		giocatore[i]->cibo=INIZIO_CIBO;
		giocatore[i]->smeraldi=INIZIO_SMERALDI;
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

void identificatruppa(t_infotruppa* T, char* buf)
{
	switch(T->tipo)
	{
		case Rec:
			sprintf(buf,"-%d Reclute (%d)-",T->numero,T->morale);
			break;
		case Fan:
			sprintf(buf,"-%d Fanteria (%d)-",T->numero,T->morale);
			break;
		case Lan:
			sprintf(buf,"-%d Lancieri (%d)-",T->numero,T->morale);
			break;
		case Arc:
			sprintf(buf,"-%d Arcieri (%d)-",T->numero,T->morale);
			break;
		case Cav:
			sprintf(buf,"-%d Cavalieri (%d)-",T->numero,T->morale);
			break;
		case Dra:
			sprintf(buf,"-%d Draghi (%d)-",T->numero,T->morale);
			break;
		case Fen:
			sprintf(buf,"-%d Fenici (%d)-",T->numero,T->morale);
			break;
		default:
			sprintf(buf,"-%d ERRORE! (%d)-",T->numero,T->morale);
			break;
	}
	return;
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

//calcola il costo del percorso più breve utilizzando l'algoritmo di Dijkstra
int inizializza_dijkstra(int PosT, char ***G, int ***V, int *mx, char *vel)
{
	int Cs;
	int Sx,Sy;
	int i,j;
	int Q=0;
	int Mx,Vel;
	Sx=PosT%LARGHEZZA;
	Sy=PosT/LARGHEZZA;
	t_truppa Tipo;
	//inizializza strutture dati
	Tipo=infomappa.truppe[PosT]->tipo;
	Vel=Dtruppa[Tipo].vel;
	Mx=2*Vel+3;
	Cs=Vel+1;
	*V=malloc(sizeof(int*)*Mx);
	*G=malloc(sizeof(char*)*Mx);
	if(*V==NULL || *G==NULL)
	{
		perror("malloc fallita");
		exit(1);
	}
	for(i=0;i<Mx;i++)
	{
		(*V)[i]=malloc(sizeof(int)*Mx);
		(*G)[i]=malloc(sizeof(char)*Mx);
		if((*V)[i]==NULL || (*G)[i]==NULL) 
		{
			perror("malloc fallita");
			exit(1);
		}
	}
	printf("dbg Mossa=%d Mx=%d vel=%d V=%p Cs=%d \n",PosT,Mx,Vel,V, Cs); // Debug
	//prepara grafo e contatore Q
	for(i=0;i<Mx; i++)
	{
		(*G)[i][0]='#';
		(*G)[i][Mx-1]='#';
	}
	for(i=0;i<Mx; i++) 
	{
		(*G)[0][i]='#';
		(*G)[Mx-1][i]='#';
	}
	for(i=1; i<Mx-1;i++)
		for(j=1; j<Mx-1;j++)
		{
			if((Sx+i-Cs)<0 || (Sx+i-Cs)>=LARGHEZZA || (Sy+j-Cs)<0 || (Sy+j-Cs)>=ALTEZZA) (*G)[i][j]='#';
			else if((Cs-i)*(Cs-i)+(Cs-j)*(Cs-j)>Vel*Vel || infomappa.mappa[posiziona(Cs-i,Cs-j,Sx,Sy)]!=' ' || infomappa.truppe[posiziona(Cs-i,Cs-j,Sx,Sy)]!=NULL) (*G)[i][j]='#';
			else 
			{
				(*G)[i][j]='.';
				Q++;
			}
		}
	printf("comincia algoritmo \n"); //Debug
	//inizializzo V
	for(i=0;i<Mx;i++)
		for(j=0;j<Mx;j++)
			(*V)[i][j]=Vel*100+1;
	(*G)[Cs][Cs]='S'; //segna il source
	(*V)[Cs][Cs]=0;
	#ifdef DEBUG
	// stampo il grafo
	for(i=0;i<Mx;i++)
	{
		for(j=0;j<Mx;j++)
			printf("%c",(*G)[j][i]);
		printf("\n");
	}
	#endif
	//assegna il valore alle variabili passate da puntatore
	*vel=Vel;
	*mx=Mx;
	//ritorna il numero di caselle libere
	return Q;
}

void calcola_dijkstra(char **G, int Mx, int vel, int **V, int Q)
{
	const int Cs=vel+1;
	int i,j,k,l;
	int v=1;
	int vp=0;
	int X,Y;
	int w;
	int S[Mx*Mx];
	S[0]=1;
	S[v]=Cs*Mx+Cs;
	while(Q!=0 && v<=S[0]) //fino a che non hai considerato ogni casella libera...
	{
		//estrai minimi e impilali
		for(i=-1; i<=1; i++)
			for(j=-1; j<=1; j++)
			{
				if(i+j==1 || i+j==-1) //considera solo le caselle minime (distanza 1)
				{
					X=S[v]/Mx+i; //calcola la x della casella
					Y=S[v]%Mx+j; //calcola la y della casella
					if (G[X][Y]=='.') //se non è stato ancora contato...
					{
						Q--; //conta un elemento in meno dal totale
						G[X][Y]='+'; //segnalo come contato
						S[S[0]+1]=X*Mx+Y; //impilalo
						S[0]++; //aumenta il puntatore alla pila
						vp++;
					}
				}
			}
		//printf("primo passo ok \n");
		for(i=S[0]-vp; i<=S[0]; i++)
		{
			X=S[i]/Mx; //calcola la x della casella
			Y=S[i]%Mx; //calcola la y della casella
			//rilassa gli archi
			for(k=-1; k<=1; k++)
				for(l=-1; l<=1; l++)
				{
					w=((k+l==1 || k+l==-1)?100:141); 
					if (G[X+k][Y+l]!='#' && V[X+k][Y+l]>V[X][Y]+w) V[X+k][Y+l]=V[X][Y]+w;
				}
		}
		//printf("secondo passo ok \n");
		v++; //passa al prossimo vertice
		vp=0; //annulla il conteggio dei nuovi vertici
	}
}

// calcola se lo spostamento è lecito
int spostalecito (int PosT, int PosC , int **V)
{
	t_truppa Tipo= infomappa.truppe[PosT]->tipo;
	int Vel=(int)Dtruppa[Tipo].vel;
	int x= PosT%LARGHEZZA-PosC%LARGHEZZA;
	int y= PosT/LARGHEZZA-PosC/LARGHEZZA;
	int s;
	int c=Vel+1;
	if(infomappa.mappa[PosC]!=' ')
	{
		if(x*x+y*y<=Vel*Vel)
		{
			s=(Vel*100-10);
			if(V[c-x-1][c-y]<=s || V[c-x+1][c-y]<=s || V[c-x][c-y-1]<=s || V[c-x][c-y+1]<=s)
			{
				return 1;
			}
		}
	}
	if(x*x+y*y<=Vel*Vel)
	{
		if(V[c-x][c-y]<=(Vel*100))
		{
			return 1;
		}
	}
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
void eliminamorti (t_infotruppa* M)
{
	int G=M->giocatore;
	int Pos;
	t_lista_t* T=giocatore[G]->truppe;
	t_lista_t* Tp=NULL;
	fprintf(stderr,"debug: eliminamorti\n");
	while(T!=NULL && T->truppa->numero!=0 && T->truppa->morale!=0)
	{
		Tp=T;
		T=T->next;
	}
	if(Tp!=NULL)
	{
		Pos=T->pos;
		Tp->next=T->next;
		free(T);
		infomappa.truppe[Pos]=NULL;
	}
	else 
	{
		Pos=T->pos;
		giocatore[G]->truppe=T->next;
		free(T);
		infomappa.truppe[Pos]=NULL;
	}
}

//elimina i puntatori alle unita morte nelle strutture
void eliminamortistrutture (int Pos)
{
	t_lista_s* S;
	t_lista_t* T;
	t_lista_t* Tp=NULL;
	S=puntastruttura(Pos);
	T=S->in;
	while(T!=NULL && T->truppa->numero!=0 && T->truppa->morale!=0)
	{
		Tp=T;
		T=T->next;
	}
	if(Tp!=NULL)
	{
		Tp->next=T->next;
		free(T);
	}
	else 
	{
		S->in=T->next;
		free(T);
	}
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
			eliminamortistrutture(Pos);
			return 1;
		}
		combatti(Difensori[i],Attaccanti[i],'n');
		if(Attaccanti[i]->numero==0) eliminamorti(Attaccanti[i]);
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
	fprintf(stderr,"debug: assaltamura: Pos= %d\n", Pos);
	if (T==NULL) return 1;
	for(i=0; i<12; i++) Difensori[i]=NULL;
	for(i=0; i<12; i++) Attaccanti[i]=NULL;
	do
	{
		Attaccanti[j]=puntacasellaoccupata(Pos,j);
		j++;
	}while(Attaccanti[j-1]!=NULL && j<12);
	for (i=0; T!=NULL && i<j-1 && i<12; i++)
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
			eliminamortistrutture(Pos);
			return 1;
		}
		combatti(Difensori[i],Attaccanti[i],'n');
		if(Attaccanti[i]->numero==0) eliminamorti(Attaccanti[i]);
	}
	return 0;
}

//fa combattere gli attaccanti contro la seconda linea di un castello presidiato
int assaltabreccia(t_infotruppa* Attaccante,t_lista_t* Difensori)
{
	t_lista_t* Temp;
	t_lista_t* Secondalinea;
	fprintf(stderr,"debug: assaltabreccia\n");
	while (Difensori!=NULL)
	{
		Secondalinea=Difensori;
		while(Secondalinea!=NULL && Secondalinea->truppa->combattuto==1) Secondalinea=Secondalinea->next;
		if(Secondalinea==NULL && Difensori!=NULL)
		{
			Secondalinea=Difensori;
			while (Secondalinea!=NULL) 
			{
				Secondalinea->truppa->morale/=3;
				Secondalinea=Secondalinea->next;
			}
			Secondalinea=Difensori;
		}
		combatti(Attaccante,Secondalinea->truppa,'n');
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
				if(Temp->truppa->morale==0 || Temp->truppa->numero==0) 
				{
					if(Temp==Difensori) Difensori=Temp->next;
					eliminamortistrutture(Temp->pos);
				}
				Temp=Temp->next;
			}
		}
		else
		{
			combatti(Secondalinea->truppa,Attaccante,'n');
			if(Attaccante->numero==0 || Attaccante->morale==0) eliminamorti(Attaccante);
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
		if(Sp!=NULL)
		{
			while(Sp->next!=NULL) Sp=Sp->next;
			Sp->next=S;
			S->next=NULL;
		}
		else
		{
			giocatore[g1]->struttura[Tipo]=(t_lista_s*)malloc(sizeof(t_lista_s));
			giocatore[g1]->struttura[Tipo]->in=NULL;
			giocatore[g1]->struttura[Tipo]->pos=Pos;
			giocatore[g1]->struttura[Tipo]->next=NULL;
		}
	}
}

//controlla il tipo della struttura
t_struttura controllotipostruttura (int Pos)
{
	int i;
	t_struttura t;
	int* s[]={infomappa.castelli,infomappa.fattorie,infomappa.stalle, infomappa.grotte,infomappa.nidi};
	int n[]={NUMCASTELLI,infomappa.numfattorie,infomappa.numstalle, infomappa.numgrotte,infomappa.numnidi};
	for(t=Cas; t<NUMSTRUTTURE; t++)
		for(i=0; i<n[t]; i++) if(s[t][i]==Pos) return t;
	return -1;
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

//controlla a quale giocatore appartiene l'unità
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

//controlla che le unita non siano dello stesso giocatore
int controllodiversotruppe (int PosA, int PosD)
{
	if (PosA==PosD) return 1;
	if (controllounita(PosA)==controllounita(PosD)) return 0;
	else return 1;
}

//punta alla truppa precedente nella struttura scelta
t_lista_t* puntatruppaprecedente (t_lista_t *T, t_lista_s *S)
{
	t_lista_t *Tp;
	if(S->in==T) return NULL;
	else
	{
		Tp=S->in;
		while(Tp->next!=T) Tp=Tp->next;
		return Tp;
	}
}
//punta alla lista struttura della struttura nella posizione desiderata
t_lista_s* puntastruttura (int Pos)
{
	int G;
	t_struttura i;
	t_lista_s* L;
	for(G=0; G<MAXGIOCATORI && giocatore[G]!=NULL; G++)
		for(i=Cas; i<=Nid; i++)
		{
			L=giocatore[G]->struttura[i];
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
	int i=0,j,k=0;
	int hi,hj,bj,bi,m;
	x=Pos%LARGHEZZA;
	y=Pos/LARGHEZZA;
	fprintf(stderr,"debug: puntacasellaoccupata\n");
	if(infomappa.mappa[Pos-1]!=' ') //se è un castello
	{
		fprintf(stderr,"debug: puntacasellaoccupata prova lato\n");
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
	else // se è una struttura
	{
		bi=-1;
		bj=-1;
		m=-1;
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

//aggiorna tr_callback
void aggiorna_tr_callback(char* pos)
{
	t_truppa i;
	for (i=Rec; i<NUMTRUPPE; i++)
	{
		tr_callback[i].tipo=i;
		tr_callback[i].pos=pos-infomappa.mappa;
	}
}

//termina il turno
void fineturno()
{
	int i;
	int r[NUMSTRUTTURE];
	int c=0;
	int n=0;
	int f;
	t_lista_t* T=giocatore[CurrentPlayer]->truppe;
	t_lista_s* S;
	for(i=0; i<NUMSTRUTTURE; i++) r[i]=0;
	//conta le truppe, il loro numero e aggiorna loro stato
	while(T!=NULL)
	{
		T->truppa->stanca=0;
		T->truppa->combattuto=0;
		if (T->truppa->tipo<Cav) c+=T->truppa->numero/10;
		else if (T->truppa->tipo==Cav) c+=2*T->truppa->numero;
		else c+=30;
		T=T->next;
		n++;
	}
	//conta le strutture
	for(i=0;i<NUMSTRUTTURE; i++)
	{
		S=giocatore[CurrentPlayer]->struttura[i];
		while (S!=NULL)
		{
			r[i]++;
			S=S->next;
		}
	}
	//calcola le risorse per il prossimo turno
	giocatore[CurrentPlayer]->oro+=PROD_ORO_CAS*r[Cas]+PROD_ORO_FAT*r[Fat]+PROD_ORO_SCU*r[Scu]+PROD_ORO_NID*r[Nid]+PROD_ORO_GRO*r[Gro];
	giocatore[CurrentPlayer]->smeraldi+=PROD_SMERALDI_NID*r[Nid]+PROD_SMERALDI_GRO*r[Gro];
	giocatore[CurrentPlayer]->cibo+=PROD_CIBO_CAS*r[Cas]+PROD_CIBO_FAT*r[Fat]+PROD_CIBO_SCU*r[Scu]-FAME*c;
	//introduce il concetto di cibo da comprare e morte e scoraggiamento per fame
	if(giocatore[CurrentPlayer]->cibo<0) 
	{
		f=giocatore[CurrentPlayer]->cibo;
		giocatore[CurrentPlayer]->cibo=0;
		giocatore[CurrentPlayer]->oro+=f;
		if(giocatore[CurrentPlayer]->oro<0)
		{
			f=giocatore[CurrentPlayer]->oro;
			giocatore[CurrentPlayer]->oro=0;
			giocatore[CurrentPlayer]->smeraldi+=f;
			if(giocatore[CurrentPlayer]->smeraldi<0)
			{
				f=giocatore[CurrentPlayer]->smeraldi;
				giocatore[CurrentPlayer]->smeraldi=0;
				f=f/n;
				f=(f<=0)? -1 : f;
				T=giocatore[CurrentPlayer]->truppe;
				while(T!=NULL)
				{
					T->truppa->morale-=1;
					if (T->truppa->tipo<Dra) T->truppa->numero+=f;
					if(T->truppa->numero<=0) eliminamorti (T->truppa);
					T=T->next;
				}
			}
		}
	}
	//aggiorna il giocatore corrente
	do
	{
		CurrentPlayer++;
		if (CurrentPlayer==MAXGIOCATORI) CurrentPlayer=0;
	}while (giocatore[CurrentPlayer]==NULL);
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
