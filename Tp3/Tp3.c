#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include "BlocoDeMemoria.h"
#include "ram.h"
#include "cache.h"
#include "cpu.h"
#include "MMU.h"
#include "disco.h"

Instrucao* carregar_programa(const char* nome_arquivo, int* total_instrucoes) { // função auxiliar para interpretar .txt
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("ERRO: Nao foi possivel abrir o arquivo '%s'.\n", nome_arquivo);
        return NULL;
    }

    Instrucao *programa = (Instrucao *)malloc(10001 * sizeof(Instrucao));
    if (programa == NULL) {
        printf("ERRO: Falha ao alocar memoria para o programa.\n");
        fclose(arquivo);
        return NULL;
    }

    *total_instrucoes = 0;
    char linha[256];

    while (fgets(linha, sizeof(linha), arquivo)) {
        if (strlen(linha) < 10) continue;

        char *delimitadores = ":\n";
        char* token;
        
        token = strtok(linha, delimitadores); if (token == NULL) continue;
        programa[*total_instrucoes].opcode = atoi(token);

        token = strtok(NULL, delimitadores); if (token == NULL) continue;
        programa[*total_instrucoes].add1.endBloco = atoi(token);
        token = strtok(NULL, delimitadores); if (token == NULL) continue;
        programa[*total_instrucoes].add1.endPalavra = atoi(token);

        token = strtok(NULL, delimitadores); if (token == NULL) continue;
        programa[*total_instrucoes].add2.endBloco = atoi(token);
        token = strtok(NULL, delimitadores); if (token == NULL) continue;
        programa[*total_instrucoes].add2.endPalavra = atoi(token);

        token = strtok(NULL, delimitadores); if (token == NULL) continue;
        programa[*total_instrucoes].add3.endBloco = atoi(token);
        token = strtok(NULL, delimitadores); if (token == NULL) continue;
        programa[*total_instrucoes].add3.endPalavra = atoi(token);

        (*total_instrucoes)++;
    }

    fclose(arquivo);
    programa[*total_instrucoes].opcode = -1;
    printf("Arquivo '%s' com %d instrucoes carregado com sucesso.\n", nome_arquivo, *total_instrucoes);
    return programa;
}

int main() {
    srand(time(NULL));
    
    const char* arquivo_programa_principal = "10_90.txt";

    int total_instrucoes_main;
    Instrucao *programa_principal = carregar_programa(arquivo_programa_principal, &total_instrucoes_main);
    if (programa_principal == NULL) return 1;

    int total_instrucoes_ti;
    Instrucao *programa_interrupcao = carregar_programa("tratador_interrupcao.txt", &total_instrucoes_ti);
    if (programa_interrupcao == NULL) {
        free(programa_principal);
        return 1;
    }
    
    //=================================
    //====ESPECIFICAÇÕES DA MÁQUINA====
    //=================================
    const char *NOME_MAQUINA = "M1";
    int TAMANHO_C1 = 8;
    int TAMANHO_C2 = 16;
    int TAMANHO_C3 = 32;
    int TAMANHO_RAM = 1000;
    int TAMANHO_DISCO = 4000;

    printf("\n>>> Configurando Maquina %s (Caches: %d, %d, %d)\n", NOME_MAQUINA, TAMANHO_C1, TAMANHO_C2, TAMANHO_C3);

    //inicializa todo as memórias e a cpu
    RAM *ram = criar_ram(TAMANHO_RAM);
    criar_disco_aleatorio(TAMANHO_DISCO);
    CPU *cpu = criar_cpu(TAMANHO_C1, TAMANHO_C2, TAMANHO_C3);

    ram_aleatoria(ram);
    cpu_set_programa(cpu, programa_principal);
    cpu->tratador_interrupcao = programa_interrupcao;

    printf(">>> Iniciando Simulacao...\n");
    cpu_iniciar(cpu, ram);
    printf(">>> Simulacao Finalizada.\n");


    // calcula e imprime o relatorio final
    
    long long total_buscas_memoria = (long long)(cpu->hitC1 + cpu->missC1);
    if (total_buscas_memoria == 0) total_buscas_memoria = 1;

    float taxa_hit_c1 = (float)cpu->hitC1 / total_buscas_memoria * 100.0;
    float taxa_hit_c2 = (cpu->missC1 > 0) ? (float)cpu->hitC2 / cpu->missC1 * 100.0 : 0;
    float taxa_hit_c3 = (cpu->missC2 > 0) ? (float)cpu->hitC3 / cpu->missC2 * 100.0 : 0;
    float total_acessos_ram = (float)cpu->hitRAM + cpu->missRAM;
    float taxa_hit_ram = (total_acessos_ram > 0) ? (float)cpu->hitRAM / total_acessos_ram * 100.0 : 0;
    float taxa_acesso_disco = (float)cpu->missRAM / total_buscas_memoria * 100.0;

    
    printf("\n------------------------ RESULTADOS FINAIS (%s) ------------------------\n", NOME_MAQUINA);
    printf("| Nivel       | Tamanho | Hits    | Misses  | Taxa de Acerto/Acesso |\n");
    printf("|-------------|---------|---------|---------|-----------------------|\n");
    printf("| Cache L1    | %-7d | %-7d | %-7d | %20.2f%% |\n",
           cpu->cache1->tamanho, cpu->hitC1, cpu->missC1, taxa_hit_c1);
    printf("| Cache L2    | %-7d | %-7d | %-7d | %20.2f%% |\n",
           cpu->cache2->tamanho, cpu->hitC2, cpu->missC2, taxa_hit_c2);
    printf("| Cache L3    | %-7d | %-7d | %-7d | %20.2f%% |\n",
           cpu->cache3->tamanho, cpu->hitC3, cpu->missC3, taxa_hit_c3);
    printf("| RAM         | %-7d | %-7d | %-7d | %20.2f%% |\n",
           ram->tamanho, cpu->hitRAM, cpu->missRAM, taxa_hit_ram);
    printf("| Disco       | %-7d | %-7d | %-7s | %20.2f%% |\n",
           TAMANHO_DISCO, cpu->missRAM, "N/A", taxa_acesso_disco);
    printf("|-----------------------------------------------------------------------|\n");
    printf("| Custo Total de Execucao: %-40ld |\n", (long)cpu->custo);
    printf("-------------------------------------------------------------------------\n");
    

    // libera a memória 
    destruir_cpu(cpu);
    destruir_ram(ram);
    free(programa_principal);
    free(programa_interrupcao);

    return 0;
}