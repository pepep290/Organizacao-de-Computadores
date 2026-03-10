#ifndef CACHE_H
#define CACHE_H

typedef struct {
    BlocoMemoria* linhas;
    int tamanho; //numero de linhas na cache
} Cache;

Cache* criar_cache(int tamanho);
void destruir_cache(Cache* cache);

#endif