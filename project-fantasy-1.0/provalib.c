#include<stdio.h>
#include "fantasy-core.h"
void veditest (t_infotruppa *A, t_infotruppa *D)
{
	switch (A->tipo)
	{
		case 0:
			printf("A: Rec");
			break;
		case 1:
			printf("A: Fan");
			break;
		case 2:
			printf("A: Lan");
			break;
		case 3:
			printf("A: Arc");
			break;
		case 4:
			
			printf("A: Cav");
			break;
		case 5:
			printf("A: Dra");
			break;
		case 6:
			printf("A: Fen");
			break;
	}
	printf(" n:%d m:%d\n", A->numero, A->morale );
	switch (D->tipo)
	{
		case 0:
			printf("D: Rec");
			break;
		case 1:
			printf("D: Fan");
			break;
		case 2:
			printf("D: Lan");
			break;
		case 3:
			printf("D: Arc");
			break;
		case 4:
			printf("D: Cav");
			break;
		case 5:
			printf("D: Dra");
			break;
		case 6:
			printf("D: Fen");
			break;
	}
	printf(" n:%d m:%d\n", D->numero, D->morale );
}

int main ()
{
	t_infotruppa* A,*D, *B;
	caricadati ();
	A=generatruppa (Dra, 1, 1);
	D=generatruppa (Fen, 2, 3);
	veditest (A,D);
	printf("\n");
	do
	{
	printf("combattimento!\n");
	combatti(A, D);
	veditest (A,D);
	B=A;
	A=D;
	D=B;
	getchar();
	} while (A->numero!=0 && D->numero!=0);
	return 0;
}
