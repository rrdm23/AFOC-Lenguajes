#include <stdio.h>
#include "memoria.h"

/* Declaracion de Memoria */
struct Inst Memoria[256]; 


/* Registros completos */
int AX,BX,CX,DX;

/* Variables de los distintos parametros */
short* par1;
short* par2;


/* Instruccion Actual (para la memoria) */
int instActual = 0;

/* a. Flags */
char CF = 0;
char SF = 0;
char ZF = 0;
char IF = 0;

/* b. UC: No hace falta */

/* c. IR (struct)*/
struct Inst IR; 

/* d. PC */
short PC = 0; 

/* e. RT */
char AH, AL = 0;
char BH, BL = 0;
char CH, CL = 0;
char DH, DL = 0;

/* Junta Registros */
void XX (int XH, int XL, int X){ /*Toma cada registro y junta sus cantidades*/
    X = XH * 256 + XL;
}

/* Separa (reparte) Registros */
void repartirX(int XX, int L,int H){ /* Reparte los datos del registro el L Y el H, segun corresponda */
    L=XX / 256;
    H= XX - (L * 256);
}

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

/* g. E/S ALU */
short B1 = 0;
short B2 = 0;
short B3 = 0;
short B4 = 0;

/* f. ALU */
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

/* h. MAR */
short MAR = -1;

/* i. BD: No hace falta */

/* j. MBR */
struct Inst MBR;


void mov (char fuente, char destino){
    destino = fuente;
}

short out(char reg){
    return getReg(reg);
}

void in(char reg, short val){
    setReg(reg, val);
}

void cmp(char oper1, char oper2){
    if (oper1 - oper2 == 0)
        ZF = 1;
    else
        ZF = 0;
}

void jmp(short dato){
    MAR = dato;
}

void jz(short dato){
    if (ZF == 0)
        MAR = dato;
}


/* CICLO DE FETCH */

int parteFtch = 0; /* variable que sirve para saber en que parte del ciclo va, puede ser 1,2,3. El 0 es cuando no ha hecho nada del ciclo */

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
	    /*haga todo*/
	    ftch1();
	    ftch2();
	    ftch3();
	    continue;
	}
	else if(estado==2){
	    if(parteFtch==0){
		/*haga Fetch 1*/
		ftch1();
		parteFtch++;
		estado = 3;
		continue;
	    }
	    else if (parteFtch==1){
		/*haga Fetch 2*/
		ftch2();
		parteFtch++;
		estado = 3;
		continue;	    }
	    else if (parteFtch==2){
		/*haga Fetch 3*/
		ftch3();
		parteFtch++;
		estado = 3;
		continue;	    }
	    else if (parteFtch==3){
		/*haga Fetch 4*/			/*FALTA*/
		if(instActual==255){
		    return;
		}
		else{
		    instActual++;
		    parteFtch = 0;
		    estado = 3;
		    continue;
		}
	    }
	}
	else if(estado==3){
	    continue;
	}
        
    }
}


void ftch1(){/*devuelve la instruccion que esta*/
    MAR = PC;
    PC++;
}

void ftch2(){ /*envia un mensaje "la instruccion se decodifico"*/
    /* MENSAJE */
}

void ftch3(){/*obtiene los operandos*/
    /*  AX = 0              BX = 1          CX = 2          DX = 3
	AL = 4              BL = 5          CL = 6          DL = 7
	AH = 8              BH = 9          CH = 10         DH = 11
	[dir] = 12          [BL] = 13       [BH] = 14       inm = 15
    */

    switch (MBR.opD){
    	    case 0: XX(AH,AL,AX); par1 = &AX; break;
	    case 1: XX(BH,BL,BX); par1 = &BX; break;
    	    case 2: XX(CH,CL,CX); par1 = &CX; break;
    	    case 3: XX(DH,DL,DX); par1 = &DX; break;
    	    case 4: par1 = &AL; break;
    	    case 5: par1 = &BL; break;
    	    case 6: par1 = &CL; break;
    	    case 7: par1 = &DL; break;
    	    case 8: par1 = &AH; break;
    	    case 9: par1 = &BH; break;
    	    case 10: par1 = &CH; break;
    	    case 11: par1 = &DH; break;
    	    case 12: par1 = &(Memoria[PC].dato4); break;
    	    case 13: par1 = &(Memoria[BL].dato4); break;
    	    case 14: par1 = &(Memoria[BH].dato4); break;
    	    case 15: par1 = &(Memoria[PC].dato4); break;
    }

    switch (MBR.opF){
    	    case 0: XX(AH,AL,AX); par2 = &AX; break;
	    case 1: XX(BH,BL,BX); par2 = &BX; break;
    	    case 2: XX(CH,CL,CX); par2 = &CX; break;
    	    case 3: XX(DH,DL,DX); par2 = &DX; break;
    	    case 4: par2 = &AL; break;
    	    case 5: par2 = &BL; break;
    	    case 6: par2 = &CL; break;
    	    case 7: par2 = &DL; break;
    	    case 8: par2 = &AH; break;
    	    case 9: par2 = &BH; break;
    	    case 10: par2 = &CH; break;
    	    case 11: par2 = &DH; break;
    	    case 12: par2 = &(Memoria[PC].dato4); break;
    	    case 13: par2 = &(Memoria[BL].dato4); break;
    	    case 14: par2 = &(Memoria[BH].dato4); break;
    	    case 15: par2 = &(Memoria[PC].dato4); break;
    }
}

/* MICROINSTRUCCIONES */

/* movReg (<-):permite mover datos del registro X al registro Y */
void movReg(short* R1, short* R2){ /* R1 <- R2 */
    int tmp = &R2;
    *R1 = tmp;
}

/* ALU: op [ejecuta la operación correspondiente de la ALU (add, sub, mul, div, and, or, xor, not, shl o shr)] */
void ALU(char* operacion){
    if (!strcmp(operacion, "add"))		{add();}
    else if (!strcmp(operacion, "sub"))		{sub();}
    else if (!strcmp(operacion, "mul"))		{mul();}
    else if (!strcmp(operacion, "div_mod"))	{div_mod();}
    else if (!strcmp(operacion, "and"))		{and();}
    else if (!strcmp(operacion, "or"))		{or();}
    else if (!strcmp(operacion, "xor"))		{xor();}
    else if (!strcmp(operacion, "not"))		{not();}
    else if (!strcmp(operacion, "shl"))		{shl();}
    else if (!strcmp(operacion, "shr"))		{shr();}   
}

/* MEM: op [ejecuta la operación de acceso a la memoria correspondiente (read o write)] */
void MEM(char* tipo){
    if (strcmp(tipo, "read")){READ();}
    else if (strcmp(tipo, "write")){WRITE();}
}

void READ(short address){
    /* CUANDO HAYA INTERFAZ, LOS DATOS NO SE RECIBIRAN EN PARAMETROS */
    MAR = address;
    MBR = Memoria[MAR];
}

void WRITE(struct Inst Buffer, short Address){
    /* CUANDO HAYA INTERFAZ, LOS DATOS NO SE RECIBIRAN EN PARAMETROS */
    MBR = Buffer;
    MAR = Address;
}

/* TEST: flag, N [bifurca a la instrucción N del microprograma si la bandera flag está encendida] */
void TEST(char flag, char N){
    /* CUANDO HAYA INTERFAZ, LOS DATOS NO SE RECIBIRAN EN PARAMETROS */
    if (flag=1){
	PC = N;}
    else{
	return;}
}

/* IN [abre una ventana y solicita un número que dejará almacenado en el MBR] */
void IN(){ /*IN(short num)*/
    /* CUANDO HAYA INTERFAZ, LOS DATOS NO SE RECIBIRAN EN PARAMETROS */\
    short cambiar = 0;
    scanf("%hd", &cambiar);										/*CAMBIAR A GRAFICA*/
    MBR.orden = -1;
    MBR.cod = -1;
    MBR.opF = -1;
    MBR.opD = -1;
    MBR.dato4 = cambiar;
}

/* OUT [En la ventana de salida despliega el contenido del MBR] */
void OUT(){
    /* VENTANA QUE DESPLIEGA EL CONTENIDO DEL MBR */
}


/* INSTRUCCIONES */

/* MOV */
void MOV(){
    movReg(*par1,*par2);
}

/* OUT	tiene 1 parametro */
void outU(){
    MBR.orden = -1;
    MBR.cod = -1;
    MBR.opF = -1;
    MBR.opD = -1;
    MBR.dato4 = *par1;
    OUT();
}

/* IN 	tiene 1 parametro*/
void inU(){
    IN();
    short y = MBR.dato4;
    short* x = &y;
    movReg(&par1,&x);
}

/* JMP */
void JMP(){
    PC = MBR.opD;
}

/* JZ */
void JZ(){
    if(ZF==1){
	JMP();
    }
}

short verificaS (short num1, short num2){
    while(num1>0 && num2>0){
	char u1 = num1 % 10;
	char u2 = num2 % 10;

	if (u1+u2>=10){return 1;}
	else{
	    num1 = num1 / 10;
	    num2 = num2 / 10;}
    }
    return 0;
}

short pot(char base, char exp){ /* trabajara solo pot positivas, no hace falta validar*/ 
    short result = 1;
    while(exp>0){
	result = result * base;
	exp--;
    }
    return result;
}

short Complemento(short num){
    char exp = 0;
    while (num!=0){
	exp++;
	num=num/10;
    }
    return (pot(10,exp));
}

short restaS(short num1, short num2){
    short compl = Complemento(num2);
    return (verificaS(num1,compl));
}

/* CMP */
void CMP(){/* destino = destino - fuente */ /* Afecta a CF, SF y ZF*/
    short destino = *par1;
    short fuente = *par2;
    B3 = destino - fuente;
    /* SIGN FLAG */
    if (B3 < 0){SF = 1;}

    /* ZERO FLAG */
    if (B3 = 0){ZF = 1;}
    
    /* CARRY FLAG */
    if (restaS(destino,fuente)==1){CF = 1;}
}




/* CLI */
void cls(){
    IF = 0;
}

/* STI */
void sti(){
    IF = 1;
}


int main (){
    
    /*short val1 = 0;*/
    /*short val2 = 0;*/
    printf("Digite un valor para B1: ");
    scanf("%hd", &B1);
    printf("Digite un valor para B2: ");
    scanf("%hd", &B2);
    add();
    printf ("Resultado: %d\n", B3);
    /*ALU("add");
    printf("%i\n",B3);*/

   return 0;
}


















