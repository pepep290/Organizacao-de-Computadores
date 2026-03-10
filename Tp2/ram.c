#include <stdio.h>
#include <stdlib.h>
#include "ram.h"
#include "BlocoDeMemoria.h"

RAM *criar_ram(int tamanho){//FEITA
    RAM *ram = (RAM *)malloc(sizeof(RAM));

    if (ram == NULL)
        printf("\nErro ao alocar RAM\n");
    ram->blocos = (BlocoMemoria *)malloc(tamanho * sizeof(BlocoMemoria));

    if (ram->blocos == NULL){
        printf("\nErro ao alocar Blocos na RAM\n");
        free(ram);
    }
    ram->tamanho = tamanho; // atribui quantidade de blocos
    for (int i = 0; i < tamanho; i++){ // inicializa todos os blocos
        ram->blocos[i].endBloco = i;
        ram->blocos[i].atualizado = false;
        ram->blocos[i].contador_lfu = 0;
        for (int j = 0; j < TAMANHO_BLOCO; j++){
            ram->blocos[i].palavras[j] = 0;
        }
    }
    return ram;
}

void ram_aleatoria(RAM *ram){ //FEITA
    for (int i = 0; i < ram->tamanho; i++){
        // preenche cada palavra de cada bloco com um valor aleatório.
        for (int j = 0; j < TAMANHO_BLOCO; j++){
            ram->blocos[i].palavras[j] = rand() % 100; // valor de 0-100
        }
    }
}

void esvazia_ram(RAM *ram){//FEITA
    for (int i = 0; i < ram->tamanho; i++){
        for (int j = 0; j < TAMANHO_BLOCO; j++){
            ram->blocos[i].palavras[j] = 0;
        }
        ram->blocos[i].atualizado = 0;
        ram->blocos[i].contador_lfu = 0;
    }
}

BlocoMemoria *ram_get_bloco(RAM *ram, int endereco_bloco){//FEITA
    if (endereco_bloco < 0 || endereco_bloco >= ram->tamanho){
        printf("Erro: Endereço de RAM inválido (%d)\n", endereco_bloco);
        return NULL;
    }
    return &ram->blocos[endereco_bloco];
}

void ram_set_bloco(RAM *ram, int endereco_bloco, BlocoMemoria *valor){//FEITA
    if (endereco_bloco < 0 || endereco_bloco >= ram->tamanho){
        printf("Erro: Endereço de RAM inválido (%d)\n", endereco_bloco);
        return;
    }
    ram->blocos[endereco_bloco] = *valor;
}

void ram_imprimir(RAM *ram){//FEITA
    printf("Conteúdo da RAM:\n");
    for (int i = 0; i < ram->tamanho; i++){
        printf("Bloco[%04d]: { ", ram->blocos[i].endBloco);
        for (int j = 0; j < TAMANHO_BLOCO; j++){
            printf("%d ", ram->blocos[i].palavras[j]);
        }
        printf("}\n");
    }
}

void destruir_ram(RAM *ram){//FEITA

    if (ram != NULL){
        free(ram->blocos);
        free(ram);
    }

    else{
        printf("\nNão foi possível liberar a RAM\n\n");
        return;
    }
}