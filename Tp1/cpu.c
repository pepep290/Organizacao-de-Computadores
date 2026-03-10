#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ram.h"
#include "cpu.h"
void cpu_set_programa(CPU* cpu, Instrucao* programa) {
    cpu->programa = programa;
}

void cpu_iniciar(CPU* cpu, RAM* ram) {
    cpu->opcode = 0;
    cpu->PC = 0;

    while (cpu->opcode != -1) {
        Instrucao* inst = &cpu->programa[cpu->PC];
        cpu->opcode = inst->opcode;
        
        switch (cpu->opcode) {
            case -1: // Halt
            printf("programa terminou!!\n");
            ram_imprimir(ram);
            break;
            
            case 0: // Soma
            cpu->registrador1 = ram_get_dado(ram, inst->add1);
            cpu->registrador2 = ram_get_dado(ram, inst->add2);
            cpu->registrador1 += cpu->registrador2;
            ram_set_dado(ram, inst->add3, cpu->registrador1);
            printf("Inst sum -> RAM posicao %d com conteudo %d\n", inst->add3, cpu->registrador1);
            break;
            
            case 1: // Subtrai
            cpu->registrador1 = ram_get_dado(ram, inst->add1);
            cpu->registrador2 = ram_get_dado(ram, inst->add2);
            cpu->registrador1 -= cpu->registrador2;
            ram_set_dado(ram, inst->add3, cpu->registrador1);
            printf("Inst sub -> RAM posicao %d com conteudo %d\n", inst->add3, cpu->registrador1);
            break;
            
            case 2: // Copia registrador -> RAM
            if (inst->add1 == 1) {
                ram_set_dado(ram, inst->add2, cpu->registrador1);
                printf("Inst copy_reg_ram -> RAM posicao %d com conteudo %d\n", inst->add2, cpu->registrador1);
            } else if (inst->add1 == 2) {
                ram_set_dado(ram, inst->add2, cpu->registrador2);
                printf("Inst copy_reg_ram -> RAM posicao %d com conteudo %d\n", inst->add2, cpu->registrador2);
            }
            break;
            
            case 3: // Copia RAM -> registrador
            if (inst->add1 == 1) {
                cpu->registrador1 = ram_get_dado(ram, inst->add2);
                    printf("Inst copy_ram_reg -> Registrador1 com conteudo %d\n", cpu->registrador1);
                } else if (inst->add1 == 2) {
                    cpu->registrador2 = ram_get_dado(ram, inst->add2);
                    printf("Inst copy_ram_reg -> Registrador2 com conteudo %d\n", cpu->registrador2);
                }
                break;
                
                case 4: // Salva valor externo no registrador
                if (inst->add1 == 1) {
                    cpu->registrador1 = inst->add2;
                    printf("Inst copy_ext_reg -> Registrador1 com conteudo %d\n", cpu->registrador1);
                } else if (inst->add1 == 2) {
                    cpu->registrador2 = inst->add2;
                    printf("Inst copy_ext_reg -> Registrador2 com conteudo %d\n", cpu->registrador2);
                }
                break;
                
                case 5: // Obtém valor do registrador
                if (inst->add1 == 1) {
                    inst->add2 = cpu->registrador1;
                    printf("Inst obtain_reg -> Registrador1 com conteudo %d\n", cpu->registrador1);
                } else if (inst->add1 == 2) {
                    inst->add2 = cpu->registrador2;
                    printf("Inst obtain_reg -> Registrador2 com conteudo %d\n", cpu->registrador2);
                }
                break;
            }
            cpu->PC++;
        }
}
