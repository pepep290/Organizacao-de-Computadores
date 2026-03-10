#ifndef MMU_H
#define MMU_H

#include "cpu.h"
#include "ram.h"
#include <stdbool.h>

BlocoMemoria* buscarMMU(CPU* cpu, RAM* ram, Endereco e);
int politicaLFU(Cache* cache, int conjunto);
void politica_WriteBack(CPU* cpu, RAM* ram, BlocoMemoria* bloco_substituido, int cache_origem);

#endif