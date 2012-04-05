#include <gtk/gtk.h>
#include "fantasy-core.h"
#include "fantasy-gtk.h"

//da spostare!!!
void fantasia_gtk_fineturno()
{
	int W;
	
	if(partita_in_corso==0) return;
	if(giocatore[CurrentPlayer]!=NULL)
	{
		fineturno();
		gtk_aggiorna_giocatore_c();
		gtk_aggiorna_contarisorse();
		gtk_aggiorna_tab_castelli();
		gtk_aggiorna_tab_strutture();
		gtk_aggiorna_tab_armate();
		gtk_pulisci_mappa();
		gtk_stampa_mappa(cx,cy,'n');
		W=controllovincitore();
		if(W!=0)
		{
			gtk_proclama_vincitore(W);
		}
	}
}
