#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "MMU.h" 

CPU* criar_cpu(int tam_l1, int tam_l2, int tam_l3){//aloca a struct cpu, e inicializa todas as variaveis com 0
    CPU* cpu = (CPU*)malloc(sizeof(CPU));
    if (cpu == NULL) {
        printf("Erro: Falha ao alocar a CPU.\n");
        return NULL;
    }
    cpu->cache1 = criar_cache(tam_l1);
    cpu->cache2 = criar_cache(tam_l2);
    cpu->cache3 = criar_cache(tam_l3);

    if (cpu->cache1 == NULL || cpu->cache2 == NULL || cpu->cache3 == NULL) {
        printf("Erro: Falha ao alocar as caches.\n");
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
    cpu->hitC1 = 0;
    cpu->missC1 = 0;
    cpu->hitC2 = 0;
    cpu->missC2 = 0;
    cpu->hitC3 = 0; 
    cpu->missC3 = 0;

    return cpu;
}

void destruir_cpu(CPU* cpu){//desaloca cpu
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

    printf("\n--- SIMULADOR INICIADO ---\n");

    while (cpu->opcode != -1){
        Instrucao inst = cpu->programa[cpu->PC];
        cpu->opcode = inst.opcode;

        if (cpu->opcode == -1){
            printf("\n--- FIM DO PROGRAMA (PC=%d) ---\n", cpu->PC);
            break;
        }

        printf("PC[%d]: Opcode %d\n", cpu->PC, cpu->opcode);

        cpu->registrador1 = buscarMMU(cpu, ram, inst.add1);
        cpu->registrador2 = buscarMMU(cpu, ram, inst.add2);
        cpu->registrador3 = buscarMMU(cpu, ram, inst.add3);

        switch (cpu->registrador1->cacheHit){
            case 1: cpu->hitC1++; break;
            case 2: cpu->missC1++; cpu->hitC2++; break;
            case 3: cpu->missC1++; cpu->missC2++; cpu->hitC3++; break;
            case 4: cpu->missC1++; cpu->missC2++; cpu->missC3++; break;
        }
        switch (cpu->registrador2->cacheHit){
            case 1: cpu->hitC1++; break;
            case 2: cpu->missC1++; cpu->hitC2++; break;
            case 3: cpu->missC1++; cpu->missC2++; cpu->hitC3++; break;
            case 4: cpu->missC1++; cpu->missC2++; cpu->missC3++; break;
        }
        switch (cpu->registrador3->cacheHit){
            case 1: cpu->hitC1++; break;
            case 2: cpu->missC1++; cpu->hitC2++; break;
            case 3: cpu->missC1++; cpu->missC2++; cpu->hitC3++; break;
            case 4: cpu->missC1++; cpu->missC2++; cpu->missC3++; break;
        }
        
        cpu->custo += cpu->registrador1->custo + cpu->registrador2->custo + cpu->registrador3->custo;

        switch (cpu->opcode){
            case 0:
                {
                    int val1 = cpu->registrador1->palavras[inst.add1.endPalavra];
                    int val2 = cpu->registrador2->palavras[inst.add2.endPalavra];
                    int resultado = val1 + val2;
                    
                    cpu->registrador3->palavras[inst.add3.endPalavra] = resultado;
                    cpu->registrador3->atualizado = true;
                    
                    printf("  SOMA: %d + %d = %d -> Bloco %d\n", val1, val2, resultado, inst.add3.endBloco);
                }
                break;

            case 1: 
                {
                    int val1 = cpu->registrador1->palavras[inst.add1.endPalavra];
                    int val2 = cpu->registrador2->palavras[inst.add2.endPalavra];
                    int resultado = val1 - val2;

                    cpu->registrador3->palavras[inst.add3.endPalavra] = resultado;
                    cpu->registrador3->atualizado = true;

                    printf("  SUB: %d - %d = %d -> Bloco %d\n", val1, val2, resultado, inst.add3.endPalavra);
                }
                break;
        }
        cpu->PC++;
    }
}