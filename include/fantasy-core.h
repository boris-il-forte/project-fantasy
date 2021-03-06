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

#ifndef FANTASY_CORE_
#define FANTASY_CORE_

#define LARGHEZZA 150
#define ALTEZZA 40
#define NUMCASTELLI 12
#define MAXGIOCATORI 12
#define MAXSTALLE 20
#define MAXFATTORIE 20
#define MAXGROTTE 2
#define MAXNIDI 4
#define NUMTRUPPE 7
#define NUMSTRUTTURE 5
#define NUMRISORSE 3
#define accedi(x,y,z) z[(x)+LARGHEZZA*(y)] /*accede ad un array come se fosse una matrice*/
#define posiziona(x,y,z,k)(z)+LARGHEZZA*(k)-((x)+LARGHEZZA*(y))
#define posiziona_c(x,y) posiziona(0,0,x,y)
#define disegna(x,y,z) accedi(x,y,infomappa.mappa)=z; accedi(x+1,y,infomappa.mappa)=z+1; accedi(x+1,y+1,infomappa.mappa)=z+3; accedi(x,y+1,infomappa.mappa)=z+2 /*disegna la struttura di simbolo z*/
#define tipouguale(x,y,mossa) accedi(x,y,infomappa.truppe)->tipo==infomappa.truppe[mossa]->tipo

//definisco le strutture fondamentali
typedef enum e_truppa
{
	Rec, Fan, Lan, Arc, Cav, Dra, Fen
} t_truppa;
typedef enum e_struttura
{
	Cas, Fat, Scu, Gro, Nid
} t_struttura;
typedef enum e_tipoplayer
{
	Locale, IA, Remoto
} t_tipoplayer;

typedef struct s_infogioco
{
	char skin[32];
	char ext[5]; // estensione della skin(xpm, png, ...)
} t_infogioco;

typedef struct s_infotruppa
{
	t_truppa tipo;
	char giocatore;
	int numero;
	char morale;
	char stanca;
	char combattuto;
} t_infotruppa;

typedef struct s_dtruppa
{
	int att;
	int dif;
	char git;
	char vel;
	char mor;
	int oro;
	int cibo;
	int smeraldi;
} t_dtruppa;

typedef struct s_infomappa
{
	unsigned char mappa[ALTEZZA * LARGHEZZA];
	unsigned char prato[ALTEZZA * LARGHEZZA];
	unsigned char ambiente[ALTEZZA * LARGHEZZA];
	t_infotruppa* truppe[ALTEZZA * LARGHEZZA];
	int castelli[NUMCASTELLI];
	int fattorie[MAXFATTORIE];
	int stalle[MAXSTALLE];
	int grotte[MAXGROTTE];
	int nidi[MAXNIDI];
	unsigned char numstalle, numnidi, numgrotte, numfattorie;
} t_infomappa;

typedef struct s_callback_s
{
	t_truppa tipo;
	int pos;
} t_callback_s;

typedef struct s_lista_t
{
	t_infotruppa* truppa;
	int pos;
	struct s_lista_t* next;
} t_lista_t;

typedef struct s_lista_s
{
	int pos;
	t_lista_t* in;
	struct s_lista_s* next;
} t_lista_s;

typedef struct s_player
{
	t_lista_s* struttura[NUMSTRUTTURE];
	t_lista_t* truppe;
	int oro;
	int cibo;
	int smeraldi;
} t_player;

typedef struct s_infoplayer //TODO implemntare la struct seriamente!
{
	t_tipoplayer tipo;
	char atteggiamento;
	char nome[20]; //TODO eliminare magick number
} t_infoplayer;

//funzioni file
int caricaconfig(char *nomefile);
int salvaconfig(char *nomefile);
int carica(char *nomefile);
int salva(char *nomefile);
int listaskin(char *nomedir, char **sottodir);

//funzioni estrazioni info mappa
int calcolaposizionestruttura(char struttura, int x, int y);
t_struttura tipostruttura(char struttura);
int posizionistruttura(int posizione, int pos[9], t_struttura tipo);

//funzioni di combattimento
void combatti(t_infotruppa* Attaccante, t_infotruppa* Difensore, char m);
void combatticampoaperto(int Dst, int Src);
void assediocastello(int Pos);
void assediostruttura(int Pos);
int assaltamura(t_lista_s* Castello);
int assaltabreccia(t_infotruppa* Attaccante, t_lista_t* Difensori);
int assaltaedificio(t_lista_s* Edificio);

//funzioni di puntamento
t_infotruppa** puntacasellalibera(int Pos);
t_infotruppa* puntacasellaoccupata(int Pos, int C);
t_lista_s* puntastruttura(int Pos);
t_lista_t* puntatruppaprecedente(t_lista_t *T, t_lista_s *S);

//funzioni di controllo
int controlloedificio(int Pos, t_struttura s);
int controllounita(int Pos);
t_struttura controllotipostruttura(int Pos);
int controllosconfitto(int G);
int controllovincitore();

//funzioni di gestione dei dati di gioco
void creagiocatori(int n, int ia);
int pagaunita(t_truppa T);
void addestratruppa(int P, t_truppa T);
t_infotruppa* generatruppa(t_truppa Tipo, char Giocatore, int Numero);
int evacuatruppa(t_lista_t *T);
void spostatruppa(int Src, int Dst);
void spostainstruttura(int Src, int Dst);
void identificatruppa(t_infotruppa* T, char* buf);
void eliminamorti(t_infotruppa* M);
void eliminamortistrutture(int Pos);
void cambiaproprietario(int g1, int g2, int Pos, t_struttura Tipo);

// funzioni per il prato
void pratoacaso();
void campiCasuali();

//funzioni generiche
void caricadati();
void inizializza();
void generamappa();
void rigeneramappa();
void visualizza_su_terminale();
void liberaheap();
void liberagiocatore(int G);
void fineturno();
void aggiorna_tr_callback(int pos);

//variabili globali
int cx, cy; // posizione della mappa visibile
char partita_in_corso;
int CurrentPlayer;
t_infogioco infogioco;
t_infomappa infomappa;
t_dtruppa Dtruppa[NUMTRUPPE];
t_player *giocatore[MAXGIOCATORI];
t_infoplayer *infogiocatore[MAXGIOCATORI];
t_callback_s tr_callback[NUMTRUPPE];

#endif
