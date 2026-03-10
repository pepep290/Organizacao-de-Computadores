#include <stdio.h>
#include <stdlib.h>
#include "ram.h"

RAM* criar_ram(int tamanho) {
    RAM* ram = (RAM*)malloc(sizeof(RAM));
    ram->dados = (int*)calloc(tamanho, sizeof(int));
    ram->tamanho = tamanho;
    return ram;
}

RAM* ram_aleatoria(RAM* ram){

    for(int i = 0; i < ram->tamanho; i++){
        ram->dados[i] = rand() % 15;
    }
    return ram;
}

RAM* esvazia_ram(RAM* ram){

    for(int i = 0; i < ram->tamanho; i++){
        ram->dados[i] = 0;
    }
    return ram;
}


int ram_get_dado(RAM* ram, int endereco) {
    if (endereco < 0 || endereco >= ram->tamanho) {
        printf("Erro: Endereço de RAM inválido (%d)\n", endereco);
        return 0;
    }
    return ram->dados[endereco];
}

void ram_set_dado(RAM* ram, int endereco, int valor) {
    if (endereco < 0 || endereco >= ram->tamanho) {
        printf("Erro: Endereço de RAM inválido (%d)\n", endereco);
        return;
    }
    ram->dados[endereco] = valor;
}

void ram_imprimir(RAM* ram) {
    printf("Conteúdo da RAM:\n");
    for (int i = 0; i < ram->tamanho; i++) {
        printf("[%d] = %d\n", i, ram->dados[i]);
    }
}

void ram_imprimir_exato(RAM* ram, int add) {
    printf("Conteúdo da RAM[%d]: %d\n", add, ram->dados[add]);
}

void destruir_ram(RAM* ram) {
    free(ram->dados);
    free(ram);
}