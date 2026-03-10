#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include "BlocoDeMemoria.h"
#include "cache.h"
#include "ram.h"

typedef struct {
    int opcode;
    Endereco add1; 
    Endereco add2;
    Endereco add3;
} Instrucao;

typedef struct {
    BlocoMemoria *registrador1;
    BlocoMemoria *registrador2;
    BlocoMemoria *registrador3;
    int PC;
    Instrucao *programa;
    int opcode;
    long long custo;
    int missC1, hitC1; 
    int missC2, hitC2; 
    int missC3, hitC3; 
    int missRAM, hitRAM; // novas variaveis para o miss em RAM
    Cache *cache1;
    Cache *cache2;
    Cache *cache3;

    // variaveis para interrupcao
    Instrucao* tratador_interrupcao; // ponteiro para o .txt 
    int PC_salvo; // quando há interrupção, salva onde o programa parou                   
    bool em_interrupcao; // só se o programa está em interrupção
} CPU;

CPU* criar_cpu(int tam_l1, int tam_l2, int tam_l3);
void destruir_cpu(CPU* cpu);
void cpu_iniciar(CPU* cpu, RAM* ram);
void cpu_set_programa(CPU* cpu, Instrucao* programa);

#endif