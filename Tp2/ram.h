#ifndef ram_h
#define ram_h
#include "BlocoDeMemoria.h"

typedef struct {
    BlocoMemoria* blocos;
    int tamanho; //numero de blocos
}RAM;

RAM* criar_ram(int tamanho);
void ram_aleatoria(RAM* ram);
void esvazia_ram(RAM* ram);
void destruir_ram(RAM* ram);
BlocoMemoria* ram_get_bloco(RAM* ram, int endereco_bloco);
void ram_set_bloco(RAM* ram, int endereco_bloco, BlocoMemoria *valor);
void ram_imprimir(RAM* ram);

#endif