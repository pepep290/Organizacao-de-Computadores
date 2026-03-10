#ifndef DISCO_H
#define DISCO_H

#include "BlocoDeMemoria.h"

void criar_disco_aleatorio(int tamanho_disco);

BlocoMemoria ler_bloco_do_disco(int endBloco);

void escrever_bloco_no_disco(int endBloco, BlocoMemoria* bloco);

#endif