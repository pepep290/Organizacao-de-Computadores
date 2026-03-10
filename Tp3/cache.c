#include <stdio.h>
#include <stdlib.h>
#include "BlocoDeMemoria.h"
#include "cache.h"

Cache* criar_cache(int tamanho){//aloca estrutura principal de cache

    Cache* cache = (Cache*)malloc(sizeof(Cache));
    if (cache == NULL){
        printf("Erro: Falha ao alocar a estrutura da Cache.\n");
        return NULL;
    }

    //aloca as linhas de bloco
    cache->linhas = (BlocoMemoria*)malloc(tamanho * sizeof(BlocoMemoria));
    if (cache->linhas == NULL){
        printf("Erro: Falha ao alocar as linhas da Cache.\n");
        free(cache);
        return NULL;
    }

    //inicializa todas as linhas
    cache->tamanho = tamanho;
    for (int i = 0; i < tamanho; i++){
        cache->linhas[i].endBloco = -1; //-1 indica que a linha está vazia/inválida
        cache->linhas[i].atualizado = false;
        cache->linhas[i].contador_lfu = 0;
        cache->linhas[i].custo = 0;
        cache->linhas[i].cacheHit = 0;
        for (int j = 0; j < TAMANHO_BLOCO; j++) {
            cache->linhas[i].palavras[j] = 0;
        }
    }

    return cache;
}

void destruir_cache(Cache* cache) {
    if (cache != NULL) {
        free(cache->linhas);
        free(cache);
    }
}