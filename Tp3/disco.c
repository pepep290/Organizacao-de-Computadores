#include <stdio.h>
#include <stdlib.h>
#include "disco.h"

#define NOME_ARQUIVO_DISCO "disco.bin"

// cria o disco.bin com 4000 de tamanho
void criar_disco_aleatorio(int tamanho_disco){
    FILE *arquivo_disco = fopen(NOME_ARQUIVO_DISCO, "wb");
    if (arquivo_disco == NULL) {
        printf("Erro: Nao foi possivel criar o arquivo de disco.\n");
        return;
    }

    for (int i = 0; i < tamanho_disco; i++) {
        BlocoMemoria bloco;
        bloco.endBloco = i;
        bloco.atualizado = false;
        bloco.contador_lfu = 0;
        for (int j = 0; j < TAMANHO_BLOCO; j++) {
            bloco.palavras[j] = rand() % 1000;
        }
        fwrite(&bloco, sizeof(BlocoMemoria), 1, arquivo_disco);
    }
    fclose(arquivo_disco);
    printf("Arquivo de disco '%s' com %d blocos criado.\n", NOME_ARQUIVO_DISCO, tamanho_disco);
}

BlocoMemoria ler_bloco_do_disco(int endBloco) {
    FILE *arquivo_disco = fopen(NOME_ARQUIVO_DISCO, "rb");
    BlocoMemoria bloco;

    if (arquivo_disco == NULL) {
        printf("Erro: Nao foi possivel abrir o arquivo de disco para leitura.\n");
        bloco.endBloco = -1; 
        return bloco;
    }

    fseek(arquivo_disco, endBloco * sizeof(BlocoMemoria), SEEK_SET);
    fread(&bloco, sizeof(BlocoMemoria), 1, arquivo_disco);
    
    fclose(arquivo_disco);
    return bloco;
}

void escrever_bloco_no_disco(int endBloco, BlocoMemoria* bloco) {
    FILE *arquivo_disco = fopen(NOME_ARQUIVO_DISCO, "r+b"); 
    if (arquivo_disco == NULL) {
        printf("Erro: Nao foi possivel abrir o arquivo de disco para escrita.\n");
        return;
    }

    fseek(arquivo_disco, endBloco * sizeof(BlocoMemoria), SEEK_SET);
    fwrite(bloco, sizeof(BlocoMemoria), 1, arquivo_disco);

    fclose(arquivo_disco);
}