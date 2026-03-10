#ifndef programas_h
#define cpu_h


void programaMulti(CPU* cpu, RAM* ram, int multiplicador, int multiplicando);
void programaAleatorio(CPU* cpu, RAM* ram, int qdeIntrucoes);
void programaMDC(CPU* cpu, RAM* ram, int n1 ,int n2);
void programaMedia(CPU* cpu, RAM* ram, int n1 ,int n2);
void programaQuad(CPU* cpu, RAM* ram, int base);
void programaLog(CPU* cpu, RAM* ram, int numero, int base);
void programaDiv(CPU* cpu, RAM* ram, int divisor, int dividendo);
void programaRaiz(CPU* cpu, RAM* ram, int n);
void programaPot(CPU* cpu, RAM* ram, int base,int expoente);
void programaEquacaoLin(CPU* cpu, RAM* ram, int x, int y, int z);
void programaPitagoras(CPU* cpu, RAM* ram, int a, int b);
void programaFib(CPU* cpu, RAM* ram, int n);
void programaProdutoEscalar(CPU* cpu, RAM* ram, int ax, int ay, int az, int bx, int by, int bz);
int programaMultiAux(Instrucao* inst, int inst_PC, int m, int n);


int menu();
#endif