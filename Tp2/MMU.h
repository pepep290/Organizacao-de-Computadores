#ifndef MMU_H
#define MMU_H

#include "cpu.h"
#include "ram.h"

BlocoMemoria* buscarMMU(CPU* cpu, RAM* ram, Endereco e);

//confere os acessos dos blocos para a politica LFU
int politicaLFU(Cache* cache, int conjunto);

//realiza a politica de write back, enquanto verifica o "dirty bit"
void politica_WriteBack(CPU* cpu, RAM* ram, BlocoMemoria* bloco_sub, int cache_origem);

#endif