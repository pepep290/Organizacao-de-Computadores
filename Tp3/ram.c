#include <stdio.h>
#include <stdlib.h>
#include "ram.h"
#include "BlocoDeMemoria.h"

RAM *criar_ram(int tamanho){
    RAM *ram = (RAM *)malloc(sizeof(RAM));
    if (ram == NULL) { return NULL; }
    
    ram->blocos = (BlocoMemoria *)malloc(tamanho * sizeof(BlocoMemoria));
    if (ram->blocos == NULL){ free(ram); return NULL; }
    
    ram->tamanho = tamanho;
    for (int i = 0; i < tamanho; i++){
        ram->blocos[i].endBloco = -1;
        ram->blocos[i].atualizado = false;
        ram->blocos[i].contador_lfu = 0;
        for (int j = 0; j < TAMANHO_BLOCO; j++){
            ram->blocos[i].palavras[j] = 0;
        }
    }
    return ram;
}

void ram_aleatoria(RAM *ram){
    for (int i = 0; i < ram->tamanho; i++){
        for (int j = 0; j < TAMANHO_BLOCO; j++){
            ram->blocos[i].palavras[j] = rand() % 100;
        }
    }
}

void esvazia_ram(RAM *ram){
    for (int i = 0; i < ram->tamanho; i++){
        for (int j = 0; j < TAMANHO_BLOCO; j++){
            ram->blocos[i].palavras[j] = 0;
        }
        ram->blocos[i].atualizado = 0;
        ram->blocos[i].contador_lfu = 0;
    }
}

BlocoMemoria *ram_get_bloco(RAM *ram, int endereco_bloco){
    int ram_index = endereco_bloco % ram->tamanho;
    return &ram->blocos[ram_index];
}

void ram_set_bloco(RAM *ram, int endereco_bloco, BlocoMemoria *valor){
    int ram_index = endereco_bloco % ram->tamanho;
    ram->blocos[ram_index] = *valor;
}

void destruir_ram(RAM *ram){
    if (ram != NULL){
        free(ram->blocos);
        free(ram);
    }
}