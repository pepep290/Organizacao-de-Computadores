#ifndef cpu_h
#define cpu_h

#include <stdio.h>
#include <stdlib.h>
#include <time.h>   
#include "ram.h"

typedef struct {
    int opcode;
    int add1;
    int add2;
    int add3;
} Instrucao;

typedef struct {
    int registrador1;
    int registrador2;
    int PC;
    Instrucao* programa;
    int opcode;
} CPU;


void cpu_iniciar(CPU* cpu, RAM* ram);
void cpu_set_programa(CPU* cpu, Instrucao* programa);
#endif