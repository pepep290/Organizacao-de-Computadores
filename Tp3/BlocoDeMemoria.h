#ifndef BLOCODEMEMORIA_H
#define BLOCODEMEMORIA_H
#define TAMANHO_BLOCO 4
#include <stdbool.h>


typedef struct{
    int palavras[TAMANHO_BLOCO];
    int endBloco;//endereço do bloco na RAM
    bool atualizado;//dirty bit - write back
    int contador_lfu;//contador de acessos - LFU
    int custo;
    int cacheHit; // 1=C1, 2=C2, 3=C3
}BlocoMemoria;

typedef struct{
    int endBloco;
    int endPalavra;
} Endereco;

#endif 