#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "memoria.h"

/* Declaracion de Memoria */
struct Inst Memoria[256]; 

/* Variables de los distintos parametros */
int par1;
int par2;

/* Instruccion Actual (para la memoria) */
int instActual = 0;

/* a. Flags */
char CF = 0;
char SF = 0;
char ZF = 0;
char IF = 0;

/* b. UC: No hace falta */

/* c. IR (struct)*/
struct Inst* IR = NULL; 

/* d. PC */
int PC = 0; 

/* e. RT */
char AH, AL = 0;
char BH, BL = 0;
char CH, CL = 0;
char DH, DL = 0;

/* Junta Registros
void XX (int XH, int XL, int X){ Toma cada registro y junta sus cantidades
    X = XH * 256 + XL;
}*/

/* Separa (reparte) Registros
void repartirX(int XX, int L,int H){ Reparte los datos del registro el L Y el H, segun corresponda
    L=XX / 256;
    H= XX - (L * 256);
}*/

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

void cls(){
    IF = 0;
}

void sti(){
    IF = 1;
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
	    //haga todo
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
		continue;
	    }
	    else if (parteFtch==1){
		/*haga Fetch 2*/
		ftch2();
		parteFtch++;
		continue;	    }
	    else if (parteFtch==2){
		/*haga Fetch 3*/
		ftch3();
		parteFtch++;
		continue;	    }
	    else if (parteFtch==3){
		/*haga Fetch 4*/			/*FALTA*/
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


void ftch1(){/*devuelve la instruccion que esta*/
    MAR++;
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
    	    case 0: par1 = getReg('A'); break;
	    case 1: par1 = getReg('B'); break;
    	    case 2: par1 = getReg('C'); break;
    	    case 3: par1 = getReg('D'); break;
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

    switch (MBR.opF){
    	    case 0: par2 = getReg('A'); break;
	    case 1: par2 = getReg('B'); break;
    	    case 2: par2 = getReg('C'); break;
    	    case 3: par2 = getReg('D'); break;
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

char* getInstrLinea(char* arch, int linea){
    char cwd[1024];
    char* path = getcwd(cwd, sizeof(cwd));

    char* pathS = malloc (sizeof(cwd)+1);
    strcpy(pathS, path);
    strcat(pathS, "/");

    char* pathUs = malloc (sizeof(cwd)+25);
    strcpy(pathUs, pathS);
    strcat(pathUs, arch);
    FILE* archInstr = fopen (pathUs, "r");
    
    free(pathS);
    free(pathUs);
    path = NULL;
    memset(cwd, 0, 1024);

    if (!archInstr)
        return NULL;

    int ptr = fgetc(archInstr);

    int cont = 0;

    while (ptr != EOF){
        if (cont == linea){
            char* tmpInst = malloc(sizeof(char)*6);
            int charInst = 0;
            while (ptr != ' ' && ptr != ':' && ptr != '\n'){
                tmpInst[charInst] = ptr;
                ptr = fgetc(archInstr);
                charInst++;
            }
            char* instr = malloc(sizeof(char)*charInst);
            memset(instr, 0, 8);
            while (charInst > 0){
                charInst--;
                *(instr+charInst) = *(tmpInst+charInst);
            }
            free(tmpInst);
            return instr;
        }

        else while (ptr != '\n'){
            ptr = fgetc(archInstr);
        }
        ptr = fgetc(archInstr);
        cont++;
    }

    fclose(archInstr);
    return NULL;
}

int contLineas(char* arch){
    char cwd[1024];
    char* path = getcwd(cwd, sizeof(cwd));

    char* pathS = malloc (sizeof(cwd)+1);
    strcpy(pathS, path);
    strcat(pathS, "/");

    char* pathUs = malloc (sizeof(cwd)+25);
    strcpy(pathUs, pathS);
    strcat(pathUs, arch);
    FILE* archC = fopen (pathUs, "r");
    
    free(pathS);
    free(pathUs);
    path = NULL;
    memset(cwd, 0, 1024);

    if (!archC)
        return NULL;

    int ptr = fgetc(archC);

    int cont = 0;

    while (ptr != EOF){
        while (ptr != '\n'){
            ptr = fgetc(archC);
        }
        ptr = fgetc(archC);
        cont++;
    }

    fclose(archC);
    return cont;
}

int numOper(int lineaInstr){
    char cwd[1024];
    char* path = getcwd(cwd, sizeof(cwd));

    char* pathArch = malloc (sizeof(cwd));
    strcpy(pathArch, path);
    strcat(pathArch, "/listainstr.txt");
    FILE* archInst = fopen (pathArch, "r");

    free(pathArch);
    path = NULL;
    memset(cwd, 0, 1024);

    if (!archInst)
        return NULL;

    int ptr = fgetc(archInst);

    int cont = 0;

    while (ptr != EOF){
        if (cont == lineaInstr){
            while (ptr != ':'){
                ptr = fgetc(archInst);
            }
            ptr = fgetc(archInst);
            int cantOp = ptr - 0x30;
            fclose(archInst);
            return (cantOp);
        }
        else while (ptr != '\n'){
            ptr = fgetc(archInst);
        }

        ptr = fgetc(archInst);
        cont++;
    }
    fclose(archInst);
    return -1;
}

int operInst(int linea, char* tipoOp){
    char cwd[1024];
    char* path = getcwd(cwd, sizeof(cwd));

    char* pathArch = malloc (sizeof(cwd));
    strcpy(pathArch, path);
    strcat(pathArch, "/listainstr.txt");
    FILE* archInst = fopen (pathArch, "r");

    free(pathArch);
    path = NULL;
    memset(cwd, 0, 1024);

    if (!archInst)
        return NULL;

    int ptr = fgetc(archInst);

    int cont = 0;

    char opActual[2];

    while (ptr != EOF){
        if (cont == linea){
            while (ptr != ':'){
                ptr = fgetc(archInst);
            }
            ptr = fgetc(archInst);
            while (ptr != ':'){
                ptr = fgetc(archInst);
            }
            ptr = fgetc(archInst);
            while (ptr != '\n'){
                opActual[0] = ptr;
                ptr = fgetc(archInst);
                opActual[1] = ptr;
                if (!strcmp(opActual, tipoOp))
                    return 1;
                else while (ptr != ','){
                    if (ptr == '\n') return 0;
                    else ptr = fgetc(archInst);
                }
                ptr = fgetc(archInst);
            }
        }
        else while (ptr != '\n'){
            ptr = fgetc(archInst);
        }

        ptr = fgetc(archInst);
        cont++;
    }
    fclose(archInst);
    return 0;
}

int loadASM(char* arch){
    char cwd[1024];
    char* path = getcwd(cwd, sizeof(cwd));

    char* pathArch = malloc (sizeof(cwd));
    strcpy(pathArch, path);
    strcat(pathArch, "/listainstr.txt");
    FILE* archInst = fopen (pathArch, "r");

    char* pathS = malloc (sizeof(cwd)+1);
    strcpy(pathS, path);
    strcat(pathS, "/");

    char* pathUs = malloc (sizeof(cwd)+25);
    strcpy(pathUs, pathS);
    strcat(pathUs, arch);
    FILE* archUs = fopen (pathUs, "r");
    
    free(pathS);
    free(pathUs);
    free(pathArch);
    path = NULL;
    memset(cwd, 0, 1024);

    if (!archUs)
        return -1;

    int ptr = fgetc(archUs);

    int cont = 0;

    while (ptr != EOF){
        char* instUs = getInstrLinea (arch, cont);
        while (ptr != '\n'){
            int numInst = 0;
            while (numInst < contLineas("listainstr.txt")){
                char* instDef = getInstrLinea ("listainstr.txt", numInst);
                if (!strcmp(instUs, instDef)){
                    printf("Instrucciones coinciden, %s con %s.\n", instUs, instDef);
                    while (ptr != ' ' && ptr != '\n'){
                        ptr = fgetc(archUs);
                    }
                    
                    printf("Sin operandos?\n");
                    if (ptr == '\n' && (numOper(numInst) != 0)) return -1;
                    else if (ptr == '\n') break;
                    ptr = fgetc(archUs);
                    
                    printf("Un operando?\n");
                    char* oper1;

                    while (ptr != ',' && ptr != '\n'){
                        if (ptr == 'A' || ptr == 'B' || ptr == 'C' || ptr == 'D'){
                            ptr = getc(archUs);
                            if (ptr == 'X'){
                                if (operInst(numInst, "AX")){
                                    ptr = getc(archUs);
                                    break;
                                }
                                else return -1;
                            }
                            else if (ptr == 'H' || ptr == 'L'){
                                if (operInst(numInst, "AL")){
                                    ptr = getc(archUs);
                                    break;
                                }
                                else return -1;
                            }
                            else return -1;
                        }
                        else if (ptr == 'a' || ptr == 'b' || ptr == 'c' || ptr == 'd'){
                            ptr = getc(archUs);
                            if (ptr == 'x'){
                                if (operInst(numInst, "AX")){
                                    ptr = getc(archUs);
                                    break;
                                }
                                else return -1;
                            }
                            else if (ptr == 'h' || ptr == 'l'){
                                if (operInst(numInst, "AX")){
                                    ptr = getc(archUs);
                                    break;
                                }
                                else return -1;
                            }
                            else return -1;
                        }
                        else if (ptr == '['){
                            ptr = fgetc(archUs);
                            if (operInst(numInst, "[]")){
                                if (ptr == 'B'){
                                    ptr = fgetc(archUs);
                                    if (ptr == 'L' || ptr == 'H'){
                                        ptr = fgetc(archUs);
                                        ptr = fgetc(archUs);
                                        break;
                                    }
                                    else return -1;
                                }
                                else if (ptr == 'b'){
                                    ptr = fgetc(archUs);
                                    if (ptr == 'l' || ptr == 'k'){
                                        ptr = fgetc(archUs);
                                        ptr = fgetc(archUs);
                                        break;
                                    }
                                    else return -1;
                                }
                                else if (ptr >= 0x30 && ptr <= 0x39){
                                    while (ptr != ']'){
                                        if (ptr >= 0x30 && ptr <= 0x39) continue;
                                        else return -1;
                                        ptr = fgetc(archUs);
                                    }
                                    ptr = fgetc(archUs);
                                }
                                else return -1;
                            }
                            else return -1;
                        }
                        else if (ptr >= 0x30 && ptr <= 0x39){
                            while (ptr != '\n'){
                                if (ptr >= 0x30 && ptr <= 0x39) continue;
                                else return -1;
                            }
                        }
                        ptr = getc(archUs);
                    }

                    if (ptr == '\n' && numOper(numInst) != 1) return -1;

                    else if(ptr == '\n') break;

                    else if (ptr == ',' && numOper(numInst) != 2) return -1;
                    
                    else if (ptr == ','){
                    printf("Dos operandos?\n");
                        while (ptr != '\n'){
                            if (ptr == 'A' || ptr == 'B' || ptr == 'C' || ptr == 'D'){
                                ptr = getc(archUs);
                                if (ptr == 'X'){
                                    if (operInst(numInst, "AX")){
                                        ptr = getc(archUs);
                                        break;
                                    }
                                    else return -1;
                                }
                                else if (ptr == 'H' || ptr == 'L'){
                                    if (operInst(numInst, "AL")){
                                        ptr = getc(archUs);
                                        break;
                                    }
                                    else return -1;
                                }
                                else return -1;
                            }
                            else if (ptr == 'a' || ptr == 'b' || ptr == 'c' || ptr == 'd'){
                                ptr = getc(archUs);
                                if (ptr == 'x'){
                                    if (operInst(numInst, "AX")){
                                        ptr = getc(archUs);
                                        break;
                                    }
                                    else return -1;
                                }
                                else if (ptr == 'h' || ptr == 'l'){
                                    if (operInst(numInst, "AX")){
                                        ptr = getc(archUs);
                                        break;
                                    }
                                    else return -1;
                                }
                                else return -1;
                            }
                            else if (ptr == '['){
                                ptr = fgetc(archUs);
                                if (operInst(numInst, "[]")){
                                    if (ptr == 'B'){
                                        ptr = fgetc(archUs);
                                        if (ptr == 'L' || ptr == 'H'){
                                            ptr = fgetc(archUs);
                                            ptr = fgetc(archUs);
                                            break;
                                        }
                                        else return -1;
                                    }
                                    else if (ptr == 'b'){
                                        ptr = fgetc(archUs);
                                        if (ptr == 'l' || ptr == 'k'){
                                            ptr = fgetc(archUs);
                                            ptr = fgetc(archUs);
                                            break;
                                        }
                                        else return -1;
                                    }
                                    else if (ptr >= 0x30 && ptr <= 0x39){
                                        while (ptr != ']'){
                                            if (ptr >= 0x30 && ptr <= 0x39) continue;
                                            else return -1;
                                            ptr = fgetc(archUs);
                                        }
                                        ptr = fgetc(archUs);
                                    }
                                    else return -1;
                                }
                                else return -1;
                            }
                            else if (ptr >= 0x30 && ptr <= 0x39){
                                while (ptr != '\n'){
                                    if (ptr >= 0x30 && ptr <= 0x39) continue;
                                    else return -1;
                                }
                            }
                            ptr = getc(archUs);
                        }
                        break;
                    }
                    else return -1;
                }
                numInst++;
                if (numInst == 12) return -1;
            }
            if (ptr == '\n') break;
            ptr = fgetc(archUs);
        }
        cont++;
        ptr = fgetc(archUs);
        instUs = NULL;
    }

    fclose(archInst);
    fclose(archUs);
    return 0;
}


/* MICROINSTRUCCIONES */

/* movReg (<-):permite mover datos del registro X al registro Y */
void movReg(int R1, int R2){ /* R1 <- R2 */
    R1 = R2;
}

/* ALU: op [ejecuta la operación correspondiente de la ALU (add, sub, mul, div, and, or, xor, not, shl o shr)] */
void ALU(char* operacion){
    if (operacion = "add")		{add();}
    else if (operacion == "sub")	{sub();}
    else if (operacion == "mul")	{mul();}
    else if (operacion == "div")	{div_mod();}
    else if (operacion == "and")	{and();}
    else if (operacion == "or")		{or();}
    else if (operacion == "xor")	{xor();}
    else if (operacion == "not")	{not();}
    else if (operacion == "shl")	{shl();}
    else if (operacion == "shr")	{shr();}   
}

/* MEM: op [ejecuta la operación de acceso a la memoria correspondiente (read o write)] */
void MEM(char* tipo){
    if (tipo == "read"){READ();}
    else if (tipo == "write"){WRITE();}
}

void READ(short address){
    MAR = address;
    MBR = Memoria[MAR];
}

void WRITE(struct Inst Buffer, short Address){
    MBR = Buffer;
    MAR = Address;
}

/* TEST: flag, N [bifurca a la instrucción N del microprograma si la bandera flag está encendida] */
void TEST(char flag, char N){
    if (flag=1){
	MAR = N;}
    else{
	return;}
}

/* IN [abre una ventana y solicita un número que dejará almacenado en el MBR] */
void IN(int num){
    MBR.orden = 0;
    MBR.cod = 0;
    MBR.opF = 0;
    MBR.opD = 0;
    MBR.dato4 = num;
}

/* OUT [En la ventana de salida despliega el contenido del MBR] */
void OUT(){
    /* VENTANA QUE DESPLIEGA EL CONTENIDO DEL MBR */
}

int main(){    
    /*printf("Digite un nombre de archivo: ");
    scanf("%s", arch);*/

    //char* x2 = getInstrLinea("arch.txt", 3);
    int x = loadASM("arch.txt");
    printf ("x: %d\n", x);
    //printf ("x: %s\n", x2);
   return 0;
}


















