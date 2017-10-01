#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Inst{
	int orden;
        int cod;
        int opF;
	int opD;
	int dato4;
};

struct Inst* meteVal(struct Inst Memoria[256], int orden, int cod, int opF, int opD, int dato4){
	struct Inst tmp = {orden, cod, opF, opD, dato4};
	Memoria[orden] = tmp;
	return Memoria;
}

//struct Inst Memoria[256]; //esto va en el programa principal, para declarar la memoria
//struct Inst tmp = {0,1,2,4,15};
//Memoria[0]= tmp;
//meteVal(Memoria,1,2,3,4,5); //este es un ejemplo de como usar la funcion para agregar
//printf("%i\n",Memoria[0].cod);
//printf("%i\n",Memoria[1].opF);

