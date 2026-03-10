#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "MMU.h" 

#define CHANCE_INTERRUPCAO 5 

// inicializa cpu
CPU* criar_cpu(int tam_l1, int tam_l2, int tam_l3){
    CPU* cpu = (CPU*)malloc(sizeof(CPU));
    if (cpu == NULL) return NULL;
    
    cpu->cache1 = criar_cache(tam_l1);
    cpu->cache2 = criar_cache(tam_l2);
    cpu->cache3 = criar_cache(tam_l3);

    if (cpu->cache1 == NULL || cpu->cache2 == NULL || cpu->cache3 == NULL) {
        free(cpu);
        return NULL;
    }

    cpu->PC = 0;
    cpu->opcode = 0;
    cpu->programa = NULL;
    cpu->registrador1 = NULL;
    cpu->registrador2 = NULL;
    cpu->registrador3 = NULL;
    cpu->custo = 0;
    cpu->hitC1 = 0, cpu->missC1 = 0;
    cpu->hitC2 = 0, cpu->missC2 = 0;
    cpu->hitC3 = 0, cpu->missC3 = 0;
    cpu->hitRAM = 0, cpu->missRAM = 0;
    cpu->tratador_interrupcao = NULL;
    cpu->PC_salvo = 0;
    cpu->em_interrupcao = false;

    return cpu;
}

void destruir_cpu(CPU* cpu){
    if (cpu != NULL) {
        destruir_cache(cpu->cache1);
        destruir_cache(cpu->cache2);
        destruir_cache(cpu->cache3); 
        free(cpu);
    }
}

void cpu_set_programa(CPU* cpu, Instrucao* programa){
    cpu->programa = programa;
}

void cpu_iniciar(CPU* cpu, RAM* ram){
    cpu->opcode = 0;
    cpu->PC = 0;

    while (cpu->opcode != -1){
        
        Instrucao inst;
        if (cpu->em_interrupcao){ // verifica se o switch da interrupcao foi ativado
            inst = cpu->tratador_interrupcao[cpu->PC];// roda interrupcao
        }
        else{
            inst = cpu->programa[cpu->PC]; // programa principal
        }

        cpu->opcode = inst.opcode;

        if (cpu->opcode == -1){
            if (cpu->em_interrupcao) {
                cpu->em_interrupcao = false;
                cpu->PC = cpu->PC_salvo; 
                cpu->opcode = 0;
                continue; 
            }
            else{
                break; 
            }
        }

        cpu->registrador1 = buscarMMU(cpu, ram, inst.add1);
        cpu->registrador2 = buscarMMU(cpu, ram, inst.add2);
        cpu->registrador3 = buscarMMU(cpu, ram, inst.add3);

        // busca para registrador1
        BlocoMemoria* reg1_bloco = buscarMMU(cpu, ram, inst.add1);
        cpu->registrador1 = reg1_bloco;
        switch (reg1_bloco->cacheHit){
            case 1: cpu->hitC1++; break;
            case 2: cpu->missC1++; cpu->hitC2++; break;
            case 3: cpu->missC1++; cpu->missC2++; cpu->hitC3++; break;
            case 4: cpu->missC1++; cpu->missC2++; cpu->missC3++; cpu->hitRAM++; break;
            case 5: cpu->missC1++; cpu->missC2++; cpu->missC3++; cpu->missRAM++; break;
        }

        // busca para registrador2
        BlocoMemoria* reg2_bloco = buscarMMU(cpu, ram, inst.add2);
        cpu->registrador2 = reg2_bloco;
        switch (reg2_bloco->cacheHit){
            case 1: cpu->hitC1++; break;
            case 2: cpu->missC1++; cpu->hitC2++; break;
            case 3: cpu->missC1++; cpu->missC2++; cpu->hitC3++; break;
            case 4: cpu->missC1++; cpu->missC2++; cpu->missC3++; cpu->hitRAM++; break;
            case 5: cpu->missC1++; cpu->missC2++; cpu->missC3++; cpu->missRAM++; break;
        }

        // busca para registrador3
        BlocoMemoria* reg3_bloco = buscarMMU(cpu, ram, inst.add3);
        cpu->registrador3 = reg3_bloco;
        switch (reg3_bloco->cacheHit){
            case 1: cpu->hitC1++; break;
            case 2: cpu->missC1++; cpu->hitC2++; break;
            case 3: cpu->missC1++; cpu->missC2++; cpu->hitC3++; break;
            case 4: cpu->missC1++; cpu->missC2++; cpu->missC3++; cpu->hitRAM++; break;
            case 5: cpu->missC1++; cpu->missC2++; cpu->missC3++; cpu->missRAM++; break;
        }

        cpu->custo += cpu->registrador1->custo + cpu->registrador2->custo + cpu->registrador3->custo;

        switch (cpu->opcode){
            case 0:
                {
                    int val1 = cpu->registrador1->palavras[inst.add1.endPalavra];
                    int val2 = cpu->registrador2->palavras[inst.add2.endPalavra];
                    cpu->registrador3->palavras[inst.add3.endPalavra] = val1 + val2;
                    cpu->registrador3->atualizado = true;
                }
                break;
            case 1:
                {
                    int val1 = cpu->registrador1->palavras[inst.add1.endPalavra];
                    int val2 = cpu->registrador2->palavras[inst.add2.endPalavra];
                    cpu->registrador3->palavras[inst.add3.endPalavra] = val1 - val2;
                    cpu->registrador3->atualizado = true;
                }
                break;
            default:
                break;
        }

        if (!cpu->em_interrupcao && (rand() % 100) < CHANCE_INTERRUPCAO){ // verifica se o programa esta em interrupcao, e caso nao esteja, roda uma chance de 5% de entrar em uma
            cpu->em_interrupcao = true; // programa interrompido
            cpu->PC_salvo = cpu->PC + 1; // salva contador do programa
            cpu->PC = -1;
        }
        
        cpu->PC++;
    }
}