#include <stdio.h>
#include <limits.h>
#include "MMU.h"
#include "disco.h" 

#define VIAS 2
#define CUSTO_L1 10
#define CUSTO_L2 50
#define CUSTO_L3 100
#define CUSTO_RAM 1000
#define CUSTO_DISCO 10000 

void politica_WriteBack(CPU* cpu, RAM* ram, BlocoMemoria* bloco_substituido, int cache_origem){//politica de escrita
    if (bloco_substituido->atualizado && bloco_substituido->endBloco != -1){
        bloco_substituido->atualizado = false;
        
        if (cache_origem == 3){
            ram_set_bloco(ram, bloco_substituido->endBloco, bloco_substituido);
        }
        else{
            Cache* cache_destino = (cache_origem == 1) ? cpu->cache2 : cpu->cache3;
            int num_conjuntos_dest = cache_destino->tamanho / VIAS;
            int conjunto_dest = bloco_substituido->endBloco % num_conjuntos_dest;
            int indice_sub_dest = politicaLFU(cache_destino, conjunto_dest);

            politica_WriteBack(cpu, ram, &cache_destino->linhas[indice_sub_dest], cache_origem + 1);
            
            cache_destino->linhas[indice_sub_dest] = *bloco_substituido;
        }
    }
}

BlocoMemoria* buscarMMU(CPU* cpu, RAM* ram, Endereco e){
    //procura no cache 1
    int conjunto_l1 = e.endBloco % (cpu->cache1->tamanho / VIAS);
    for (int i = 0; i < VIAS; i++) {
        int indice = (conjunto_l1 * VIAS) + i;
        if (cpu->cache1->linhas[indice].endBloco == e.endBloco){
            cpu->cache1->linhas[indice].cacheHit = 1;
            cpu->cache1->linhas[indice].custo = CUSTO_L1;
            cpu->cache1->linhas[indice].contador_lfu++;
            return &cpu->cache1->linhas[indice];
        }
    }

    //procura no cache 2 e promove bloco se achar
    int conjunto_l2 = e.endBloco % (cpu->cache2->tamanho / VIAS);
    for (int i = 0; i < VIAS; i++) {
        int indice = (conjunto_l2 * VIAS) + i;
        if (cpu->cache2->linhas[indice].endBloco == e.endBloco){
            cpu->cache2->linhas[indice].contador_lfu++;
            int indice_sub_l1 = politicaLFU(cpu->cache1, conjunto_l1);
            politica_WriteBack(cpu, ram, &cpu->cache1->linhas[indice_sub_l1], 1);
            cpu->cache1->linhas[indice_sub_l1] = cpu->cache2->linhas[indice];
            cpu->cache1->linhas[indice_sub_l1].cacheHit = 2;
            cpu->cache1->linhas[indice_sub_l1].custo = CUSTO_L1 + CUSTO_L2;
            return &cpu->cache1->linhas[indice_sub_l1];
        }
    }

    //procura no cache 3 e promove bloco se achar
    int conjunto_l3 = e.endBloco % (cpu->cache3->tamanho / VIAS);
    for (int i = 0; i < VIAS; i++) {
        int indice = (conjunto_l3 * VIAS) + i;
        if (cpu->cache3->linhas[indice].endBloco == e.endBloco){
            cpu->cache3->linhas[indice].contador_lfu++;
            int indice_sub_l2 = politicaLFU(cpu->cache2, conjunto_l2);
            politica_WriteBack(cpu, ram, &cpu->cache2->linhas[indice_sub_l2], 2);
            cpu->cache2->linhas[indice_sub_l2] = cpu->cache3->linhas[indice];
            int indice_sub_l1 = politicaLFU(cpu->cache1, conjunto_l1);
            politica_WriteBack(cpu, ram, &cpu->cache1->linhas[indice_sub_l1], 1);
            cpu->cache1->linhas[indice_sub_l1] = cpu->cache2->linhas[indice_sub_l2];
            cpu->cache1->linhas[indice_sub_l1].cacheHit = 3;
            cpu->cache1->linhas[indice_sub_l1].custo = CUSTO_L1 + CUSTO_L2 + CUSTO_L3;
            return &cpu->cache1->linhas[indice_sub_l1];
        }
    }

    int ram_index = e.endBloco % ram->tamanho;
    BlocoMemoria* bloco_na_ram = &ram->blocos[ram_index];
    BlocoMemoria bloco_encontrado;

    if (bloco_na_ram->endBloco == e.endBloco) {
        cpu->hitRAM++;
        bloco_encontrado = *bloco_na_ram;
        bloco_encontrado.cacheHit = 4;
        bloco_encontrado.custo = CUSTO_L1 + CUSTO_L2 + CUSTO_L3 + CUSTO_RAM;
    } 
    else{
        cpu->missRAM++;
        if (bloco_na_ram->atualizado && bloco_na_ram->endBloco != -1) {
            escrever_bloco_no_disco(bloco_na_ram->endBloco, bloco_na_ram);
        }
        *bloco_na_ram = ler_bloco_do_disco(e.endBloco);
        bloco_encontrado = *bloco_na_ram;
        bloco_encontrado.cacheHit = 5;
        bloco_encontrado.custo = CUSTO_L1 + CUSTO_L2 + CUSTO_L3 + CUSTO_RAM + CUSTO_DISCO;
    }

    int indice_sub_l3 = politicaLFU(cpu->cache3, conjunto_l3);
    politica_WriteBack(cpu, ram, &cpu->cache3->linhas[indice_sub_l3], 3);
    cpu->cache3->linhas[indice_sub_l3] = bloco_encontrado;
    cpu->cache3->linhas[indice_sub_l3].contador_lfu = 1;

    int indice_sub_l2 = politicaLFU(cpu->cache2, conjunto_l2);
    politica_WriteBack(cpu, ram, &cpu->cache2->linhas[indice_sub_l2], 2);
    cpu->cache2->linhas[indice_sub_l2] = cpu->cache3->linhas[indice_sub_l3];
    cpu->cache2->linhas[indice_sub_l2].contador_lfu = 1;

    int indice_sub_l1 = politicaLFU(cpu->cache1, conjunto_l1);
    politica_WriteBack(cpu, ram, &cpu->cache1->linhas[indice_sub_l1], 1);
    cpu->cache1->linhas[indice_sub_l1] = cpu->cache2->linhas[indice_sub_l2];
    cpu->cache1->linhas[indice_sub_l1].contador_lfu = 1;

    cpu->cache1->linhas[indice_sub_l1].cacheHit = bloco_encontrado.cacheHit;
    cpu->cache1->linhas[indice_sub_l1].custo = bloco_encontrado.custo;

    return &cpu->cache1->linhas[indice_sub_l1];
}

int politicaLFU(Cache* cache, int conjunto){//politica de substituicao
    int indice_base = conjunto * VIAS;
    int indice_final = indice_base + VIAS;
    
    for (int i = indice_base; i < indice_final; i++){
        if (cache->linhas[i].endBloco == -1){
            return i;
        }
    }

    int indice_substituir = indice_base;
    int menor_frequencia = INT_MAX;

    for (int i = indice_base; i < indice_final; i++){
        if (cache->linhas[i].contador_lfu < menor_frequencia){
            menor_frequencia = cache->linhas[i].contador_lfu;
            indice_substituir = i;
        }
    }
    return indice_substituir;
}