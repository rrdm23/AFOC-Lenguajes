#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "memoria.h"

/*------------------------PARTE LOGICA------------------------*/
/* Declaracion de Memoria */
struct Inst Memoria[256]; 

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
struct Inst IR; /*struct Inst* IR = NULL; */

/* d. PC */
short PC = 0; 

/* e. RT */
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
    /*MODIFICAR LA LABEL*/
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

/*---------------------------------------------------------------------------*/
/*Manejo de archivos*/

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

/*---------------------------------------------------------------------------*/
/*Revisar sintaxis de archivo ASM*/

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

/*---------------------------------------------------------------------------*/
/*Opcode instrucción*/
int opcodeInst(char* instr){
    char cwd[1024];
    char* path = getcwd(cwd, sizeof(cwd));

    char* pathArch = malloc (sizeof(cwd));
    strcpy(pathArch, path);
    strcat(pathArch, "/listainstr.txt");
    FILE* archInst = fopen (pathArch, "r");

    free(pathArch);
    path = NULL;
    memset(cwd, 0, 1024);

    int cont = 0;

    while (cont < contLineas("listainstr.txt")){
        if (!strcmp(instr, getInstrLinea("listainstr.txt", cont))){
            return cont;
        }
        else cont++;
    }

    fclose(archInst);
    return -1;
}

/*Parseo de instrucciones*/
struct Inst parseInstr(char* arch, int lineaInst){
    char cwd[1024];
    char* path = getcwd(cwd, sizeof(cwd));

    char* pathS = malloc (sizeof(cwd)+1);
    strcpy(pathS, path);
    strcat(pathS, "/");

    char* pathUs = malloc (sizeof(cwd)+25);
    strcpy(pathUs, pathS);
    strcat(pathUs, arch);
    FILE* archUs = fopen (pathUs, "r");
    
    free(pathS);
    free(pathUs);
    path = NULL;
    memset(cwd, 0, 1024);

    struct Inst err = {-1, -1, -1, -1, -1};

    if (!archUs)
        return err;

    int ptr = fgetc(archUs);

    int cont = 0;    

    while (ptr != EOF){
        if (cont == lineaInst){
            int opF;
            int opD;
            int cuartoDato;
                   
            if (!numOper(opcodeInst(getInstrLinea(arch, lineaInst)))){
                opF = -1;
                opD = -1;
                cuartoDato = -1;
            }

            else if (numOper(opcodeInst(getInstrLinea(arch, lineaInst))) == 1){
                opF = -1; 
                ptr = fgetc(archUs);

                while (ptr != ' '){
                    ptr = fgetc(archUs);
                }       

                ptr = fgetc(archUs);

                while (ptr != '\n'){
                    if (ptr == 'A' || ptr == 'a'){
                        ptr = fgetc(archUs);
                        if (ptr == 'X' || ptr == 'x') opD = 0;
                        else if (ptr == 'L' || ptr == 'l') opD = 4;
                        else opD = 8;
                        break;
                    }
                    else if (ptr == 'B' || ptr == 'b'){
                        ptr = fgetc(archUs);
                        if (ptr == 'X' || ptr == 'x') opD = 1;
                        else if (ptr == 'L' || ptr == 'l') opD = 5;
                        else opD = 9;
                        break;
                    }
                    else if (ptr == 'C' || ptr == 'c'){
                        ptr = fgetc(archUs);
                        if (ptr == 'X' || ptr == 'x') opD = 2;
                        else if (ptr == 'L' || ptr == 'l') opD = 6;
                        else opD = 10;
                        break;
                    }
                    else if (ptr == 'D' || ptr == 'd'){
                        ptr = fgetc(archUs);
                        if (ptr == 'X' || ptr == 'x') opD = 3;
                        else if (ptr == 'L' || ptr == 'l') opD = 7;
                        else opD = 11;
                        break;
                    }
                    else if (ptr == '['){
                        ptr = fgetc(archUs);
                        if (ptr == 'B' || ptr == 'b'){
                            if (ptr == 'L' || ptr == 'l') opD = 13;
                            else opD = 14;
                        }
                        else{
                            opD = 12;
                            char dir[3];
                            int pos = 0;
                            while (ptr != ']'){
                                dir[pos] = ptr;
                                ptr = fgetc(archUs);
                                pos++;
                            }
                            
                            int num = atoi(dir);
                            cuartoDato = num;
                        }
                        break;
                    }
                    else{
                        opD = 15;
                        char inm[5];
                        int pos = 0;
                        while (ptr != ']'){
                            inm[pos] = ptr;
                            ptr = fgetc(archUs);
                            pos++;
                        }
                        int num = atoi(inm);
                        cuartoDato = num;                        
                        break;
                    }
                    ptr = fgetc(archUs);
                }
            }

            else{
                while (ptr != ' '){
                    ptr = fgetc(archUs);
                }
                
                while (ptr != ','){
                    if (ptr == 'A' || ptr == 'a'){
                        ptr = fgetc(archUs);
                        if (ptr == 'X' || ptr == 'x') opD = 0;
                        else if (ptr == 'L' || ptr == 'l') opD = 4;
                        else opD = 8;
                        break;
                    }
                    else if (ptr == 'B' || ptr == 'b'){
                        ptr = fgetc(archUs);
                        if (ptr == 'X' || ptr == 'x') opD = 1;
                        else if (ptr == 'L' || ptr == 'l') opD = 5;
                        else opD = 9;
                        break;
                    }
                    else if (ptr == 'C' || ptr == 'c'){
                        ptr = fgetc(archUs);
                        if (ptr == 'X' || ptr == 'x') opD = 2;
                        else if (ptr == 'L' || ptr == 'l') opD = 6;
                        else opD = 10;
                        break;
                    }
                    else if (ptr == 'D' || ptr == 'd'){
                        ptr = fgetc(archUs);
                        if (ptr == 'X' || ptr == 'x') opD = 3;
                        else if (ptr == 'L' || ptr == 'l') opD = 7;
                        else opD = 11;
                        break;
                    }
                    else if (ptr == '['){
                        if (opcodeInst(getInstrLinea(arch, lineaInst)) == 8 || opcodeInst(getInstrLinea(arch, lineaInst)) == 9)
                            return err;
                        ptr = fgetc(archUs);
                        if (ptr == 'B' || ptr == 'b'){
                            if (ptr == 'L' || ptr == 'l') opD = 13;
                            else opD = 14;
                        }
                        else{
                            opD = 12;
                            char dir[3];
                            int pos = 0;
                            while (ptr != ']'){
                                dir[pos] = ptr;
                                ptr = fgetc(archUs);
                                pos++;
                            }
                            
                            int num = atoi(dir);
                            cuartoDato = num;
                        }
                        break;
                    }
                    else{
                        opD = 15;
                        char inm[5];
                        int pos = 0;
                        while (ptr != ']'){
                            inm[pos] = ptr;
                            ptr = fgetc(archUs);
                            pos++;
                        }
                        int num = atoi(inm);
                        cuartoDato = num;                        
                        break;
                    }
                    ptr = fgetc(archUs);
                }
                
                ptr = fgetc(archUs);

                while (ptr != '\n'){
                    if (ptr == 'A' || ptr == 'a'){
                        ptr = fgetc(archUs);
                        if (ptr == 'X' || ptr == 'x'){
                            if (!opD) return err;
                            else opF = 0;
                        }
                        else if (ptr == 'L' || ptr == 'l'){
                            if(opD == 4) return err;
                            else opD = 4;
                        }
                        else opD = 8;
                        break;
                    }
                    else if (ptr == 'B' || ptr == 'b'){
                        ptr = fgetc(archUs);
                        if (ptr == 'X' || ptr == 'x') opD = 1;
                        else if (ptr == 'L' || ptr == 'l') opD = 5;
                        else opD = 9;
                        break;
                    }
                    else if (ptr == 'C' || ptr == 'c'){
                        ptr = fgetc(archUs);
                        if (ptr == 'X' || ptr == 'x') opD = 2;
                        else if (ptr == 'L' || ptr == 'l') opD = 6;
                        else opD = 10;
                        break;
                    }
                    else if (ptr == 'D' || ptr == 'd'){
                        ptr = fgetc(archUs);
                        if (ptr == 'X' || ptr == 'x') opD = 3;
                        else if (ptr == 'L' || ptr == 'l') opD = 7;
                        else opD = 11;
                        break;
                    }
                    else if (ptr == '['){
                        if (opcodeInst(getInstrLinea(arch, lineaInst)) == 8 || opcodeInst(getInstrLinea(arch, lineaInst)) == 9)
                            return err;
                        ptr = fgetc(archUs);
                        if (ptr == 'B' || ptr == 'b'){
                            if (ptr == 'L' || ptr == 'l') opD = 13;
                            else opD = 14;
                        }
                        else{
                            opD = 12;
                            char dir[3];
                            int pos = 0;
                            while (ptr != ']'){
                                dir[pos] = ptr;
                                ptr = fgetc(archUs);
                                pos++;
                            }
                            
                            int num = atoi(dir);
                            cuartoDato = num;
                        }
                        break;
                    }
                    else{
                        opD = 15;
                        char inm[5];
                        int pos = 0;
                        while (ptr != ']'){
                            inm[pos] = ptr;
                            ptr = fgetc(archUs);
                            pos++;
                        }
                        int num = atoi(inm);
                        cuartoDato = num;                        
                        break;
                    }
                    ptr = fgetc(archUs);
                }

            }
 
            struct Inst instrLinea = {0, opcodeInst(getInstrLinea(arch, lineaInst)), opF, opD, cuartoDato};
            return instrLinea;
        }
        else while (ptr != '\n') ptr = fgetc(archUs);
        cont++;
        ptr = fgetc(archUs);
    }

    fclose(archUs);
    return err;
}

/*---------------------------------------------------------------------------*/
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


/*------------------------INTERFAZ GRAFICA------------------------*/
static void load_activated(GtkWidget *f)
{
    g_print("\n");
}

static void bryan2_activated(GtkWidget *f)
{
    g_print("\n");
}

static void quit_activated(GtkWidget *f)
{
    g_print("\n");
    gtk_main_quit();
}

static void another_activated(GtkWidget *widget, gpointer data)
{
    g_print("\n", (gchar*)data);
}

void UCF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void IRF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void PCF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void RTF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void FF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void ALUF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void B1F(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void B2F(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void B3F(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void B4F(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void MARF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void MBRF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void PLAYF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void PAUSEF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void STOPF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void STEPF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void RESETF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void LOADF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void MEMF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void ACERCAF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}
void AYUDAF(GtkWidget *widget,
                   GtkWidget *bryan_box){
    g_print("Falto agregar funcionalidad al boton");
}

int main( int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *box;
    GtkWidget *menubar;
    GtkWidget *filemenu;
    GtkWidget *file;
    GtkWidget *load;
    GtkWidget *fixed;
    GtkWidget *bryan2;
    GtkWidget *quit;

/* BOTONES IMPORTANTES */
/* UC */
    GtkWidget *UC;
/* IR */
    GtkWidget *IR;
/* PC */
    GtkWidget *PC;
/* RT */
    GtkWidget *RT;
/* Flags */
    GtkWidget *F;
/* ALU */
    GtkWidget *ALU;
/* B1 */
    GtkWidget *B1;
/* B2 */
    GtkWidget *B2;
/* B3 */
    GtkWidget *B3;
/* B4 */
    GtkWidget *B4;
/* MAR */
    GtkWidget *MAR;
/* MBR */
    GtkWidget *MBR;
/*LOAD*/
    GtkWidget *LOAD;
/*PLAY*/
    GtkWidget *PLAY;
/*PAUSE*/
    GtkWidget *PAUSE;
/*STEP*/
    GtkWidget *STEP;
/*RESET*/
    GtkWidget *RESET;
/*STOP*/
    GtkWidget *STOP;
/*MEM*/
    GtkWidget *MEM;
/*ACERCA DE Y AYUDA*/
    GtkWidget *ACERCA;
    GtkWidget *AYUDA;
/* DONDE SE VA MOSTRANDO TODO */
    GtkWidget *label;


/**/
   GtkWidget *bryan;
   GtkWidget *bryan_box;
    
/**/

    GtkWidget *anothermenu;
    GtkWidget *another;
    GtkWidget *anothermenuitem;

    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 800);
    gtk_window_set_title(GTK_WINDOW(window), "AFOC: Simulador Arquitectura de Von Neumann");
    
    fixed = gtk_fixed_new ();
    bryan_box = gtk_fixed_new ();
    gtk_container_add (GTK_CONTAINER (window), bryan_box);


    menubar = gtk_menu_bar_new();

    filemenu = gtk_menu_new();
    file = gtk_menu_item_new_with_label("File");
    bryan2 = gtk_menu_item_new_with_label("Esta es la Paleta");
    /*bryan = gtk_button_new_with_label ("Este es el Boton");*/

/**/
    UC = gtk_button_new_with_label ("Unidad de Control");
    IR = gtk_button_new_with_label ("Instruction Register");
    PC = gtk_button_new_with_label ("Program Counter");
    RT = gtk_button_new_with_label ("Registro de Trabajo");
    F = gtk_button_new_with_label ("Banderas");
    ALU = gtk_button_new_with_label ("Unidad Arimetica/Logica");
    B1 = gtk_button_new_with_label ("B1");
    B2 = gtk_button_new_with_label ("B2");
    B3 = gtk_button_new_with_label ("B3");
    B4 = gtk_button_new_with_label ("B4");
    MAR = gtk_button_new_with_label ("Memory Address Register");
    MBR = gtk_button_new_with_label ("Memery Buffer Register");
    PLAY = gtk_button_new_with_label ("PLAY");
    PAUSE = gtk_button_new_with_label ("PAUSE");
    RESET = gtk_button_new_with_label ("RESET");
    LOAD = gtk_button_new_with_label ("LOAD");
    STOP = gtk_button_new_with_label ("STOP");
    STEP = gtk_button_new_with_label ("STEP");
    MEM = gtk_button_new_with_label ("Memoria");
    label = gtk_label_new("*Aqui estara la linea en actual ejecucion*");
    ACERCA = gtk_button_new_with_label ("ACERCA DE");
    AYUDA = gtk_button_new_with_label ("AYUDA");    

/**/
    load = gtk_menu_item_new_with_label("Load");
    quit = gtk_menu_item_new_with_label("Quit");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);

    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), load);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), bryan2);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);

    //Connects GCallback function load_activated to "activate" signal for "load" menu item
    
    g_signal_connect(G_OBJECT(load), "activate", G_CALLBACK(load_activated), NULL);

    g_signal_connect(G_OBJECT(bryan2), "activate", G_CALLBACK(bryan2_activated), NULL);

    //Connects GCallback function quit_activated to "activate" signal for "quit" menu item
    g_signal_connect(G_OBJECT(quit), "activate", G_CALLBACK(quit_activated), NULL);

    anothermenu = gtk_menu_new();
    another = gtk_menu_item_new_with_label("Another");
    anothermenuitem = gtk_menu_item_new_with_label("Another Menu Item");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(another), anothermenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(anothermenu), anothermenuitem);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), another);

    //Connects GCallback function another_activated to "activate" signal for another
    g_signal_connect(G_OBJECT(anothermenuitem), "activate", G_CALLBACK(another_activated), "anothermenuitem");
    g_signal_connect(G_OBJECT(another), "activate", G_CALLBACK(another_activated), "another");
    /*g_signal_connect (G_OBJECT (bryan), "clicked", G_CALLBACK (imprime), (gpointer) bryan);*/

/**/
    g_signal_connect (G_OBJECT (UC), "clicked", G_CALLBACK (UCF), (gpointer) UC);
    g_signal_connect (G_OBJECT (IR), "clicked", G_CALLBACK (IRF), (gpointer) IR);
    g_signal_connect (G_OBJECT (PC), "clicked", G_CALLBACK (PCF), (gpointer) PC);
    g_signal_connect (G_OBJECT (RT), "clicked", G_CALLBACK (RTF), (gpointer) RT);
    g_signal_connect (G_OBJECT (F), "clicked", G_CALLBACK (FF), (gpointer) F);
    g_signal_connect (G_OBJECT (ALU), "clicked", G_CALLBACK (ALUF), (gpointer) ALU);
    g_signal_connect (G_OBJECT (B1), "clicked", G_CALLBACK (B1F), (gpointer) B1);
    g_signal_connect (G_OBJECT (B2), "clicked", G_CALLBACK (B2F), (gpointer) B2);
    g_signal_connect (G_OBJECT (B3), "clicked", G_CALLBACK (B3F), (gpointer) B3);
    g_signal_connect (G_OBJECT (B4), "clicked", G_CALLBACK (B4F), (gpointer) B4);
    g_signal_connect (G_OBJECT (MAR), "clicked", G_CALLBACK (MARF), (gpointer) MAR);
    g_signal_connect (G_OBJECT (MBR), "clicked", G_CALLBACK (MBRF), (gpointer) MBR);
    g_signal_connect (G_OBJECT (MEM), "clicked", G_CALLBACK (MEMF), (gpointer) MEM);
    g_signal_connect (G_OBJECT (PLAY), "clicked", G_CALLBACK (PLAYF), (gpointer) PLAY);
    g_signal_connect (G_OBJECT (PAUSE), "clicked", G_CALLBACK (PAUSEF), (gpointer) PAUSE);
    g_signal_connect (G_OBJECT (STOP), "clicked", G_CALLBACK (STOPF), (gpointer) STOP);
    g_signal_connect (G_OBJECT (RESET), "clicked", G_CALLBACK (RESETF), (gpointer) RESET);
    g_signal_connect (G_OBJECT (STEP), "clicked", G_CALLBACK (STEPF), (gpointer) STEP);
    g_signal_connect (G_OBJECT (LOAD), "clicked", G_CALLBACK (LOADF), (gpointer) LOAD);
    g_signal_connect (G_OBJECT (ACERCA), "clicked", G_CALLBACK (ACERCAF), (gpointer) ACERCA);
    g_signal_connect (G_OBJECT (AYUDA), "clicked", G_CALLBACK (AYUDAF), (gpointer) AYUDA);
/**/
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(box), menubar, FALSE, FALSE, 3);
    gtk_container_add(GTK_CONTAINER(window), box);

    /*gtk_fixed_put (GTK_FIXED (bryan_box), bryan, 150, 150);*/

/**/
    gtk_fixed_put (GTK_FIXED (bryan_box), UC, 50, 50);
    gtk_fixed_put (GTK_FIXED (bryan_box), IR, 50, 150);
    gtk_fixed_put (GTK_FIXED (bryan_box), PC, 50, 250);
    gtk_fixed_put (GTK_FIXED (bryan_box), RT, 500, 50);
    gtk_fixed_put (GTK_FIXED (bryan_box), F, 500, 150);
    gtk_fixed_put (GTK_FIXED (bryan_box), ALU, 500, 250);
    gtk_fixed_put (GTK_FIXED (bryan_box), B1, 500, 275);
    gtk_fixed_put (GTK_FIXED (bryan_box), B2, 530, 275);
    gtk_fixed_put (GTK_FIXED (bryan_box), B3, 560, 275);
    gtk_fixed_put (GTK_FIXED (bryan_box), B4, 590, 275);
    gtk_fixed_put (GTK_FIXED (bryan_box), MAR, 500, 375);
    gtk_fixed_put (GTK_FIXED (bryan_box), MBR, 500, 400);
    gtk_fixed_put (GTK_FIXED (bryan_box), MEM, 950, 50);
    gtk_fixed_put (GTK_FIXED (bryan_box), PLAY, 950, 150);
    gtk_fixed_put (GTK_FIXED (bryan_box), PAUSE, 950, 175);
    gtk_fixed_put (GTK_FIXED (bryan_box), RESET, 950, 200);
    gtk_fixed_put (GTK_FIXED (bryan_box), STEP, 950, 225);
    gtk_fixed_put (GTK_FIXED (bryan_box), STOP, 950, 250);
    gtk_fixed_put (GTK_FIXED (bryan_box), label, 350, 500);
    gtk_fixed_put (GTK_FIXED (bryan_box), ACERCA, 950, 350);
    gtk_fixed_put (GTK_FIXED (bryan_box), AYUDA, 950, 375);

/**/

    //Connects GCallback function gtk_main_quit to "destroy" signal for "window"
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);


    gtk_widget_show_all(window);
    gtk_main();

    struct Inst x = parseInstr("arch.txt", 3);
    printf("Fuente: %d\nDestino: %d\nCuarto dato: %d\n", x.opF, x.opD, x.dato4);
    printf("si pasa");

    return 0;
}
