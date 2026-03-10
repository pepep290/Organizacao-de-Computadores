#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "BlocoDeMemoria.h"
#include "ram.h"
#include "cache.h"
#include "cpu.h"
#include "MMU.h"

int main(){
    srand(time(NULL));

    //recarrega arquivo de instrucoes
    printf("Carregando arquivo 'instructions.txt'...\n");

    FILE *arquivo_instrucoes = fopen("10_90.txt", "r");
    if (arquivo_instrucoes == NULL){
        printf("ERRO: Nao foi possivel abrir o arquivo 'instructions.txt'.\n");
        printf("Certifique-se que o arquivo existe e esta na mesma pasta.\n");
        return 1;
    }

    //aloca espaço para 10000 instruções + 1 para o HALT
    Instrucao *programa = (Instrucao *)malloc(10001 * sizeof(Instrucao));
    int total_instrucoes = 0;

    char linha[256];//copia todas as instrucoes, e depois roda o programa com essas
    while (fgets(linha, sizeof(linha), arquivo_instrucoes)){
        //usa strtok para quebrar a linha usando ":" como delimitador
        char *token = strtok(linha, ":");
        programa[total_instrucoes].opcode = atoi(token);

        token = strtok(NULL, ":");
        programa[total_instrucoes].add1.endBloco = atoi(token);
        token = strtok(NULL, ":");
        programa[total_instrucoes].add1.endPalavra = atoi(token);

        token = strtok(NULL, ":");
        programa[total_instrucoes].add2.endBloco = atoi(token);
        token = strtok(NULL, ":");
        programa[total_instrucoes].add2.endPalavra = atoi(token);

        token = strtok(NULL, ":");
        programa[total_instrucoes].add3.endBloco = atoi(token);
        token = strtok(NULL, "\n");
        programa[total_instrucoes].add3.endPalavra = atoi(token);

        total_instrucoes++;
    }

    fclose(arquivo_instrucoes);

    //adiciona o HALT (-1) no final
    programa[total_instrucoes].opcode = -1;
    printf("Programa com %d instrucoes carregado.\n", total_instrucoes);

    //============================================
    //======= ESPECIFICAÇÕES DA MÁQUINA ==========    
    //============================================
    // M1 = 8, 16, 32; 
    // M2 = 32, 64, 128;
    // M3 = 16, 64, 256;
    // M4 = 8, 32, 128;
    // M5 = 16, 32, 64;

    const char *NOME_MAQUINA = "M1";
    int TAMANHO_C1 = 8;
    int TAMANHO_C2 = 16;
    int TAMANHO_C3 = 32;
    int TAMANHO_RAM = 1000;

    printf("\n>>> Configurando Maquina %s (Caches: %d, %d, %d)\n",
           NOME_MAQUINA, TAMANHO_C1, TAMANHO_C2, TAMANHO_C3);

    RAM *ram = criar_ram(TAMANHO_RAM);//cria ram 
    CPU *cpu = criar_cpu(TAMANHO_C1, TAMANHO_C2, TAMANHO_C3);//cria os caches com tamanhos especificados pra cada maquina

    ram_aleatoria(ram);
    cpu_set_programa(cpu, programa);//inicia o vetor de instrucoes

    printf(">>> Iniciando Simulacao...\n");
    cpu_iniciar(cpu, ram);
    printf(">>> Simulacao Finalizada.\n");

    //calcula e exibe resultados
    long long total_buscas_memoria = (long long)cpu->PC * 3; // 3 acessos por instrução

    // Taxa de hit L1 = hits_L1 / total_buscas
    float taxa_hit_c1 = (float)cpu->hitC1 / total_buscas_memoria * 100.0;

    // Taxa de hit L2 = hits_L2 / (misses_L1)
    float taxa_hit_c2 = (cpu->missC1 > 0) ?
                        ((float)cpu->hitC2 / cpu->missC1 * 100.0) : 0;

    // Taxa de hit L3 = hits_L3 / (misses_L2)
    float taxa_hit_c3 = (cpu->missC2 > 0) ?
                        ((float)cpu->hitC3 / cpu->missC2 * 100.0) : 0;

    // Adicione também o cálculo da taxa de miss geral
    float taxa_miss_geral = (float)(cpu->missC3) / total_buscas_memoria * 100.0;

    printf("\n---------- RESULTADOS FINAIS (%s) ----------\n", NOME_MAQUINA);
    printf("| Config      | Tamanho | Hits    | Misses  | Taxa de Hit |\n");
    printf("|-------------|---------|---------|---------|-------------|\n");
    printf("| Cache L1    | %-7d | %-7d | %-7d | %10.2f%% |\n",
           cpu->cache1->tamanho, cpu->hitC1, cpu->missC1, taxa_hit_c1);
    printf("| Cache L2    | %-7d | %-7d | %-7d | %10.2f%% |\n",
           cpu->cache2->tamanho, cpu->hitC2, cpu->missC2, taxa_hit_c2);
    printf("| Cache L3    | %-7d | %-7d | %-7d | %10.2f%% |\n",
           cpu->cache3->tamanho, cpu->hitC3, cpu->missC3, taxa_hit_c3);
    printf("|-----------------------------------------------------------|\n");
    printf("| Taxa de Miss Geral: %10.2f%% |\n", taxa_miss_geral);
    printf("|-----------------------------------------------------------|\n");
    printf("| Custo Total de Execucao: %lld unidades de tempo\n", cpu->custo);
    printf("-------------------------------------------------------------\n");



    //PARTE FINAL - libera memória
    printf("\n\nLiberando memoria\n");
    destruir_cpu(cpu);
    destruir_ram(ram);
    free(programa);
    printf("Memoria liberada.\n");

    return 0;
}