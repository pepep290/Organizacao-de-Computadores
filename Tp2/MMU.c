#include <stdio.h>
#include <limits.h>
#include "MMU.h"

#define VIAS 2
#define CUSTO_L1 10
#define CUSTO_L2 50
#define CUSTO_L3 100
#define CUSTO_RAM 1000

BlocoMemoria* buscarMMU(CPU* cpu, RAM* ram, Endereco e){//confere os acessos dos blocos para a politica LFU
    
    //procura na cache 1
    int num_conjuntos_l1 = cpu->cache1->tamanho / VIAS;
    int conjunto_l1 = e.endBloco % num_conjuntos_l1;
    for (int i = 0; i < VIAS; i++) {
        int indice = (conjunto_l1 * VIAS) + i;
        if (cpu->cache1->linhas[indice].endBloco == e.endBloco){//se achar, retorna hit
            printf("  > HIT L1 (Bloco %d)\n", e.endBloco);
            cpu->cache1->linhas[indice].cacheHit = 1;
            cpu->cache1->linhas[indice].custo = CUSTO_L1;
            cpu->cache1->linhas[indice].contador_lfu++;
            return &cpu->cache1->linhas[indice];
        }
    }
    printf("  > MISS L1 (Bloco %d)\n", e.endBloco);

    //procura na cache 2
    int num_conjuntos_l2 = cpu->cache2->tamanho / VIAS;
    int conjunto_l2 = e.endBloco % num_conjuntos_l2;
    for (int i = 0; i < VIAS; i++) {
        int indice = (conjunto_l2 * VIAS) + i;
        if (cpu->cache2->linhas[indice].endBloco == e.endBloco){//se achar, retorna hit
            printf("  > HIT L2 (Bloco %d)\n", e.endBloco);
            cpu->cache2->linhas[indice].contador_lfu++;
            
            int indice_sub_l1 = politicaLFU(cpu->cache1, conjunto_l1);
            politica_WriteBack(cpu, ram, &cpu->cache1->linhas[indice_sub_l1], 1);
            cpu->cache1->linhas[indice_sub_l1] = cpu->cache2->linhas[indice];
            
            cpu->cache1->linhas[indice_sub_l1].cacheHit = 2;
            cpu->cache1->linhas[indice_sub_l1].custo = CUSTO_L1 + CUSTO_L2;
            
            return &cpu->cache1->linhas[indice_sub_l1];
        }
    }
    printf("  > MISS L2 (Bloco %d)\n", e.endBloco);

    //procura na cache 3
    int num_conjuntos_l3 = cpu->cache3->tamanho / VIAS;
    int conjunto_l3 = e.endBloco % num_conjuntos_l3;
    for (int i = 0; i < VIAS; i++) {
        int indice = (conjunto_l3 * VIAS) + i;
        if (cpu->cache3->linhas[indice].endBloco == e.endBloco){//se achar, retorna hit
            printf("  > HIT L3 (Bloco %d)\n", e.endBloco);
            cpu->cache3->linhas[indice].contador_lfu++; // Incrementa LFU na L3

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
    printf("  > MISS L3 (Bloco %d)\n", e.endBloco);

    //busca na ram
    printf("  > MISS GERAL. Buscando na RAM (Bloco %d)\n", e.endBloco);
    BlocoMemoria bloco_da_ram = *ram_get_bloco(ram, e.endBloco);

    int indice_sub_l3 = politicaLFU(cpu->cache3, conjunto_l3);
    politica_WriteBack(cpu, ram, &cpu->cache3->linhas[indice_sub_l3], 3);
    cpu->cache3->linhas[indice_sub_l3] = bloco_da_ram;

    int indice_sub_l2 = politicaLFU(cpu->cache2, conjunto_l2);
    politica_WriteBack(cpu, ram, &cpu->cache2->linhas[indice_sub_l2], 2);
    cpu->cache2->linhas[indice_sub_l2] = cpu->cache3->linhas[indice_sub_l3];

    int indice_sub_l1 = politicaLFU(cpu->cache1, conjunto_l1);
    politica_WriteBack(cpu, ram, &cpu->cache1->linhas[indice_sub_l1], 1);
    cpu->cache1->linhas[indice_sub_l1] = cpu->cache2->linhas[indice_sub_l2];

    cpu->cache1->linhas[indice_sub_l1].cacheHit = 4;
    cpu->cache1->linhas[indice_sub_l1].custo = CUSTO_L1 + CUSTO_L2 + CUSTO_L3 + CUSTO_RAM;
    cpu->cache1->linhas[indice_sub_l1].contador_lfu = 1; 

    return &cpu->cache1->linhas[indice_sub_l1];
}

//confere os acessos dos blocos para a politica LFU
int politicaLFU(Cache* cache, int conjunto){
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

//realiza a politica de write back, enquanto verifica o "dirty bit"
void politica_WriteBack(CPU* cpu, RAM* ram, BlocoMemoria* bloco_sub, int cache_origem){

    if (bloco_sub->atualizado && bloco_sub->endBloco != -1){
        printf("  > WRITE-BACK: Bloco %d (sujo) da L%d sendo salvo.\n", bloco_sub->endBloco, cache_origem);
        
        bloco_sub->atualizado = false;
        
        Cache* cache_destino = (cache_origem == 1) ? cpu->cache2 : cpu->cache3;
        
        if (cache_origem == 3){
            ram_set_bloco(ram, bloco_sub->endBloco, bloco_sub);
        }
        else{
            int num_conjuntos_dest = cache_destino->tamanho / VIAS;
            int conjunto_dest = bloco_sub->endBloco % num_conjuntos_dest;
            int indice_sub_dest = politicaLFU(cache_destino, conjunto_dest);
            politica_WriteBack(cpu, ram, &cache_destino->linhas[indice_sub_dest], cache_origem + 1);
            cache_destino->linhas[indice_sub_dest] = *bloco_sub;
        }
    }
}