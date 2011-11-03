#include "../fantasy-loadsave.c"
#include <string.h>

int main(int argc, char *argv[])
{
	if(argc < 3) {
		printf("Usage: %s [l/s] [filename]\n",argv[0]);
		return 1;
	}
	if(strcmp(argv[1],"l") == 0) {
		printf("Carico il file: %s\n",argv[2]);
		if(carica(argv[2])) printf("Errore caricamento.\n");
		else printf("Caricato!\n");
	}
	else if(strcmp(argv[1],"s") == 0) {
		printf("Salvo il file: %s\n",argv[2]);
		if(salva(argv[2])) printf("Errore salvataggio.\n");
		else printf("Salvato!\n");
	} else {
		printf("Operazione sconosciuta. Scegli tra 'l 'e 's'.\n");
		return 1;
	}

	return 0;
}
