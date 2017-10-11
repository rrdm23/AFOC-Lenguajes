#include <stdio.h>
#include "memoria.h"

//a. Flags
char CF = 0;
char SF = 0;
char ZF = 0;
char IF = 0;

//b. UC: Nel :v

//c. IR (struct)
//instruccion* IR = NULL;

//d. PC
int PC = -1; 

//e. RT
char AH, AL = 0;
char BH, BL = 0;
char CH, CL = 0;
char DH, DL = 0;

short getReg(char reg){
    short RX;
    switch (reg) {
        case 'A':
            RX = AL;
            RX += (AH * 0x100);
            break;
        case 'B':
            RX = BL;
            RX += (BH * 0x100);
            break;
        case 'C':
            RX = CL;
            RX += (CH * 0x100);
            break;
        default:
            RX = DL;
            RX += (DH * 0x100);
    }
    return RX;
}

void setReg(char reg, short valor){
    switch (reg) {
        case 'A':
            AL = valor%0x100;
            AH = valor/0x100;
            break;
        case 'B':
            BL = valor%0x100;
            BH = valor/0x100;
            break;
        case 'C':
            CL = valor%0x100;
            CH = valor/0x100;
            break;
        default:
            DL = valor%0x100;
            DH = valor/0x100;
    }
}

//g. E/S ALU
short B1 = 0;
short B2 = 0;
short B3 = 0;
short B4 = 0;

//f. ALU
void add (){
    int aux = B1 + B2;
    if (aux > 32767){
        CF = 1;
    }
    B3 = aux % 32768;
    if (B3 == 0)
        ZF = 1;
    if (B3 < 0)
        SF = 1;
}

void sub(){
    if (B1 < B2)
        CF = 1;
    B3 = B1 - B2;
    if (B3 == 0)
        ZF = 1;
    if (B3 < 0)
        SF = 1;
}

void mul(){
    int tmp = B1 * B2;
    if (tmp == 0)
        ZF = 1;
    if (tmp < 0)
        SF = 1;
    B3 = tmp/0x100;
    B4 = tmp%0x100;
}

void div_mod(){
    if (B2 == 0){
        printf ("Error. Divisor cero.");
        return;
    }
    B3 = B1 / B2;
    if (B3 == 0)
        ZF = 1;
    if (B3 < 0)
        SF = 1;
    B4 = B1 % B2;
}

void and(){
    B3 = B1 & B2;
}

void or(){
    B3 = B1 | B2;
}

void not(){
    B3 = ~B1;
}

void xor(){
    B3 = B1 ^ B2;
}

void shl(){
    B3 = B1 << B2;
}

void shr(){
    B3 = B1 >> B2;
}

//h. MAR
short MAR = 0;

//i. BD: Nel :v

//j. MBR
short MBR = 0;




/* BHS */

/* Declaracion de Memoria */
struct Inst Memoria[256]; 

/* Registros */
void XX (int XH, int XL, int X){ /*Toma cada registro y junta sus cantidades*/
    X = XH * 256 + XL;
}

void repartirX(int XX, int L,int H){ /* Reparte los datos del registro el L Y el H, segun corresponda */
    L=XX / 256;
    H= XX - (L * 256);
}

/* Registros completos */
int AX,BX,CX,DX;

/* Punteros de los distintos direccionamientos de memoria de los parametros */
int par1;
int par2;


/* Instruccion Actual (para la memoria) */
int instActual = 0;

/* CICLO DE FETCH */

int parteFtch = 0; /* variable que sirve para saber en que parte del ciclo va, puede ser 1,2,3,4. el 0 es cuando no ha hecho nada del ciclo */

int estado = 0; /* estado es para la simulacion: 
			0=simulacion apagada
			1=corra todo (espere en ciclo inf)     
			2=Pasito a pasito:pase una y espere	    
			3=Detenida:no haga nada y espere    */

void cicloFetch(struct Inst Memoria[256]){ 
    while (1){
	if (estado==0){
	    break;
	}
	else if(estado==1){
	    //haga todo
	    continue;
	}
	else if(estado==2){
	    if(parteFtch==0){
		//haga Fetch 1
		parteFtch++;
		continue;
	    }
	    else if (parteFtch==1){
		//haga Fetch 2
		parteFtch++;
		continue;	    }
	    else if (parteFtch==2){
		//haga Fetch 3
		parteFtch++;
		continue;	    }
	    else if (parteFtch==3){
		//haga Fetch 4
		if(instActual==255){
		    return;
		}
		else{
		    instActual++;
		    parteFtch = 0;
		    continue;
		}
	    }
	}
	else if(estado==3){
	    continue;
	}
        
    }
}


void ftch1(){//devuelve la instruccion que esta
    PC++;
}

void ftch2(){ /*envia un mensaje "la instruccion se decodifico"*/
    /* MENSAJE */
}

void ftch3(){/*obtiene los operandos*/
    /*  AX = 0              BX = 1          CX = 2          DX = 3
	AL = 4              BL = 5          CL = 6          DL = 7
	AH = 8              BH = 9          CH = 10       DH = 11
	[dir] = 12          [BL] = 13      [BH] = 14    inm = 15
    */

    switch (Memoria[PC].opD){
    	    case 0: XX(AH,AL,AX); par1 = AX; break;
	    case 1: XX(BH,BL,BX); par1 = BX; break;
    	    case 2: XX(CH,CL,CX); par1 = CX; break;
    	    case 3: XX(DH,DL,DX); par1 = DX; break;
    	    case 4: par1 = AL; break;
    	    case 5: par1 = BL; break;
    	    case 6: par1 = CL; break;
    	    case 7: par1 = DL; break;
    	    case 8: par1 = AH; break;
    	    case 9: par1 = BH; break;
    	    case 10: par1 = CH; break;
    	    case 11: par1 = DH; break;
    	    case 12: par1 = Memoria[PC].dato4; break;
    	    case 13: par1 = &BL; break;
    	    case 14: par1 = &BH; break;
    	    case 15: par1 = Memoria[PC].dato4; break;
    }

    switch (Memoria[PC].opF){
    	    case 0: XX(AH,AL,AX); par2 = AX; break;
	    case 1: XX(BH,BL,BX); par2 = BX; break;
    	    case 2: XX(CH,CL,CX); par2 = CX; break;
    	    case 3: XX(DH,DL,DX); par2 = DX; break;
    	    case 4: par2 = AL; break;
    	    case 5: par2 = BL; break;
    	    case 6: par2 = CL; break;
    	    case 7: par2 = DL; break;
    	    case 8: par2 = AH; break;
    	    case 9: par2 = BH; break;
    	    case 10: par2 = CH; break;
    	    case 11: par2 = DH; break;
    	    case 12: par2 = Memoria[PC].dato4; break;
    	    case 13: par2 = &BL; break;
    	    case 14: par2 = &BH; break;
    	    case 15: par2 = Memoria[PC].dato4; break;
    }
}



/* BHS */




int main (){
    
    //short val1 = 0;
    //short val2 = 0;
    printf("Digite un valor para B1: ");
    scanf("%hd", &B1);
    printf("Digite un valor para B2: ");
    scanf("%hd", &B2);
    add();
    printf ("Resultado: %d\n", B3);

   return 0;
}
























