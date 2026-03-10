#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cpu.h"
#include "ram.h"
#include "programas.h"
//Concertar o log(colocar subtraçao)-manus.ia

//
// OPCODES:
// -1 -> halt
// 0 -> soma (add1=ram_pos_origem1, add2=ram_pos_origem2, add3=ram_pos_destino)
// 1 -> subtrai (add1=ram_pos_origem1, add2=ram_pos_origem2, add3=ram_pos_destino)
// 2 -> copia do registrador para RAM (add1=reg_num, add2=ram_pos_destino)
// 3 -> copia da RAM para o registrador (add1=reg_num, add2=ram_pos_origem)
// 4 -> salva conteudo externo no registrador (add1=reg_num, add2=valor_externo)
// 5 -> obtem conteudo externo do registrador (add1=reg_num, add2=destino_inst_add2)

void programaMulti(CPU *cpu, RAM *ram, int multiplicador, int multiplicando)
{

    Instrucao *inst = malloc(sizeof(Instrucao) * (multiplicando + 6));

    int ramMultiplicador, ramMultiplicando, ramRes; // Cria valores aleatórios para as posições da ram
    ramMultiplicador = rand() % ram->tamanho;
    ramMultiplicando = rand() % ram->tamanho;
    ramRes = rand() % ram->tamanho;

    while (ramMultiplicando == ramMultiplicador)
    {
        ramMultiplicando = rand() % ram->tamanho;
    }

    // Copia valores externos para os registrados, e depois copia eles para a ram]
    inst[0] = (Instrucao){4, 1, multiplicador, -1};
    inst[1] = (Instrucao){4, 2, multiplicando, -1};

    inst[2] = (Instrucao){2, 1, ramMultiplicador, -1};
    inst[3] = (Instrucao){2, 2, ramMultiplicando, -1};

    inst[4] = (Instrucao){4, 1, 0, -1};
    inst[5] = (Instrucao){2, 1, ramRes, -1};

    // Loop de somas para fazer a multiplicação
    for (int i = 0; i < multiplicando;)
    {
        inst[i + 6].opcode = 0;
        inst[i + 6].add1 = ramMultiplicador;
        inst[i + 6].add2 = ramRes;
        inst[i + 6].add3 = ramRes;
        i++;
    }

    inst[multiplicando + 6].opcode = -1;

    cpu_set_programa(cpu, inst);
    cpu_iniciar(cpu, ram);
    ram_imprimir_exato(ram, ramRes);

    free(inst);
}

void programaAleatorio(CPU *cpu, RAM *ram, int qdeIntrucoes)
{

    ram_aleatoria(ram);
    srand(time(NULL)); // Inicializa a aleatoriedade

    Instrucao *inst = malloc(sizeof(Instrucao) * qdeIntrucoes);
    if (inst == NULL)
    {
        // Alocação falhou — finaliza o programa silenciosamente
        exit(1);
    }

    for (int i = 0; i < qdeIntrucoes - 1; i++)
    {
        inst[i].opcode = rand() % 6;          // opcode aleatório entre 0 e 5
        inst[i].add1 = rand() % ram->tamanho; // endereço 1 aleatório
        inst[i].add2 = rand() % ram->tamanho; // endereço 2 aleatório
        inst[i].add3 = rand() % ram->tamanho; // endereço 3 aleatório
    }

    // Instrução final como HALT
    inst[qdeIntrucoes - 1].opcode = -1;

    cpu_set_programa(cpu, inst);
    cpu_iniciar(cpu, ram);

    free(inst); // Libera a memória depois de usar
}

void programaQuad(CPU *cpu, RAM *ram, int base)
{
    programaMulti(cpu, ram, base, base);
}

void programaMDC(CPU *cpu, RAM *ram, int n1_initial, int n2_initial)
{
    // Aloca posições na RAM para n1, n2 e o resultado do MDC
    int pos_n1 = rand() % ram->tamanho;
    int pos_n2 = rand() % ram->tamanho;
    int pos_result = rand() % ram->tamanho;

    // Garante posições únicas na RAM
    while (pos_n2 == pos_n1)
        pos_n2 = rand() % ram->tamanho;
    while (pos_result == pos_n1 || pos_result == pos_n2)
        pos_result = rand() % ram->tamanho;

    // Aloca memória para as instruções.
    Instrucao *inst = malloc(sizeof(Instrucao) * 8000);
    int i = 0;

    // Inicializa n1 e n2 nas posições da RAM
    inst[i++] = (Instrucao){4, 1, n1_initial, -1}; // reg1 = n1_initial
    inst[i++] = (Instrucao){2, 1, pos_n1, -1};     // reg1 -> RAM[pos_n1]
    inst[i++] = (Instrucao){4, 1, n2_initial, -1}; // reg1 = n2_initial
    inst[i++] = (Instrucao){2, 1, pos_n2, -1};     // reg1 -> RAM[pos_n2]

    
    int atual_n1_val = n1_initial; 
    int atual_n2_val = n2_initial; 

    while (atual_n1_val != 0 && atual_n2_val != 0)
    {
        if (atual_n1_val > atual_n2_val)
        {
            // Gerar instrução para: RAM[pos_n1] = RAM[pos_n1] - RAM[pos_n2]
            inst[i++] = (Instrucao){1, pos_n1, pos_n2, pos_n1};
            atual_n1_val -= atual_n2_val; // Atualiza o valor simulado no C
        }
        else if (atual_n2_val > atual_n1_val)
        {
            // Gerar instrução para: RAM[pos_n2] = RAM[pos_n2] - RAM[pos_n1]
            inst[i++] = (Instrucao){1, pos_n2, pos_n1, pos_n2};
            atual_n2_val -= atual_n1_val; // Atualiza o valor
        }
        else
        { // atual_n1_val == atual_n2_val é o MDC
            break;
        }
        if (i >= 7990)
        {
            printf("Aviso: Array de instruções quase cheio para MDC. Considere alocação maior.\n");
            break;
        }
    }

    if (atual_n1_val != 0)
    {                                                  // Se n1_initial não é zero, ele é o MDC
        inst[i++] = (Instrucao){3, 1, pos_n1, -1};     // reg1 = RAM[pos_n1]
        inst[i++] = (Instrucao){2, 1, pos_result, -1}; // reg1 -> RAM[pos_result]
    }
    else
    {                                                  // Se n1_initial é zero, n2_initial deve ser o MDC
        inst[i++] = (Instrucao){3, 1, pos_n2, -1};     // reg1 = RAM[pos_n2]
        inst[i++] = (Instrucao){2, 1, pos_result, -1}; // reg1 -> RAM[pos_result]
    }

    inst[i++] = (Instrucao){-1, -1, -1, -1}; // Instrução HALT para finalizar o programa da CPU

    cpu_set_programa(cpu, inst);
    cpu_iniciar(cpu, ram);
    ram_imprimir_exato(ram, pos_result); // Imprime o MDC da RAM
    free(inst);
}

void programaMedia(CPU *cpu, RAM *ram, int n1, int n2)
{
    Instrucao *inst = malloc(sizeof(Instrucao) * 10);

    int i = 0;

    int pos_n1 = rand() % ram->tamanho;
    int pos_n2 = rand() % ram->tamanho;
    int pos_soma = rand() % ram->tamanho;
    inst[i++] = (Instrucao){0, 1, n1 + n2, -1};
    while (pos_n2 == pos_n1)
        pos_n2 = rand() % ram->tamanho;
    while (pos_soma == pos_n1 || pos_soma == pos_n2)
        pos_soma = rand() % ram->tamanho;

    // Inicializa n1 e n2 nas posições aleatórias
    inst[i++] = (Instrucao){4, 1, n1, -1};
    inst[i++] = (Instrucao){2, 1, pos_n1, -1};
    inst[i++] = (Instrucao){4, 1, n2, -1};
    inst[i++] = (Instrucao){2, 1, pos_n2, -1};
    // Soma n1 + n2 -> pos_soma
    inst[i++] = (Instrucao){3, 1, pos_n1, -1};
    inst[i++] = (Instrucao){3, 2, pos_n2, -1};
    inst[i++] = (Instrucao){0, 1, 2, pos_soma};
    // HALT
    // inst[i++] = (Instrucao){-1, -1, -1, -1};

    free(inst);
    programaDiv(cpu, ram, n1 + n2, 2);
}

void programaLog(CPU *cpu, RAM *ram, int numero, int base)
{
    if (base <= 1 || numero < 1)
    {
        printf("Base deve ser > 1 e número > 0!\n");
        return;
    }

    int pos_numero = rand() % ram->tamanho;
    int pos_base = rand() % ram->tamanho;
    int pos_resultado = rand() % ram->tamanho;

    // Inicializa RAM usando apenas opcodes
    Instrucao *inst = malloc(sizeof(Instrucao) * 10);
    int i = 0;
    inst[i++] = (Instrucao){4, 1, numero, -1};        // reg1 = numero
    inst[i++] = (Instrucao){2, 1, pos_numero, -1};    // reg1 -> RAM[pos_numero]
    inst[i++] = (Instrucao){4, 1, base, -1};          // reg1 = base
    inst[i++] = (Instrucao){2, 1, pos_base, -1};      // reg1 -> RAM[pos_base]
    inst[i++] = (Instrucao){4, 1, 0, -1};             // reg1 = 0
    inst[i++] = (Instrucao){2, 1, pos_resultado, -1}; // reg1 -> RAM[pos_resultado]

    int cont = 0;
    int temp = numero;
    while (temp >= base)
    {
        temp /= base;
        cont++;
    }

    // Salva o resultado do log em RAM[pos_resultado]
    inst[i++] = (Instrucao){4, 1, cont, -1};          // reg1 = count
    inst[i++] = (Instrucao){2, 1, pos_resultado, -1}; // reg1 -> RAM[pos_resultado]
    inst[i++] = (Instrucao){-1, -1, -1, -1};

    cpu_set_programa(cpu, inst);
    cpu_iniciar(cpu, ram);
    ram_imprimir_exato(ram, pos_resultado);
    free(inst);
}

void programaDiv(CPU *cpu, RAM *ram, int dividendo_val, int divisor_val)
{
    if (divisor_val <= 0)
    {
        printf("Erro: Divisor deve ser maior que zero!\n");
        return;
    }
    if (dividendo_val < 0)
    {
        printf("Erro: Dividendo deve ser não-negativo!\n");
        return;
    }

    // Aloca posições na RAM para uso interno da divisão
    int pos_dividendo_internal = rand() % ram->tamanho;
    int pos_divisor_internal = rand() % ram->tamanho;
    int pos_quociente_internal = rand() % ram->tamanho;
    int pos_um_internal = rand() % ram->tamanho;

    // Garante posições únicas para as variáveis internas
    while (pos_divisor_internal == pos_dividendo_internal)
        pos_divisor_internal = rand() % ram->tamanho;
    while (pos_quociente_internal == pos_dividendo_internal || pos_quociente_internal == pos_divisor_internal)
        pos_quociente_internal = rand() % ram->tamanho;
    while (pos_um_internal == pos_dividendo_internal || pos_um_internal == pos_divisor_internal || pos_um_internal == pos_quociente_internal)
        pos_um_internal = rand() % ram->tamanho;

    Instrucao *inst = malloc(sizeof(Instrucao) * (dividendo_val * 8 + 50));
    int i = 0;

    
    inst[i++] = (Instrucao){4, 1, dividendo_val, -1};          // reg1 = dividendo_val
    inst[i++] = (Instrucao){2, 1, pos_dividendo_internal, -1}; // reg1 -> RAM[pos_dividendo]
    inst[i++] = (Instrucao){4, 1, divisor_val, -1};            // reg1 = divisor_val
    inst[i++] = (Instrucao){2, 1, pos_divisor_internal, -1};   // reg1 -> RAM[pos_divisor]
    inst[i++] = (Instrucao){4, 1, 0, -1};                      // reg1 = 0
    inst[i++] = (Instrucao){2, 1, pos_quociente_internal, -1}; // reg1 -> RAM[pos_quociente]
    inst[i++] = (Instrucao){4, 1, 1, -1};                      // reg1 = 1
    inst[i++] = (Instrucao){2, 1, pos_um_internal, -1};        // reg1 -> RAM[pos_um]

    int atual_dividendo_val = dividendo_val; 
    while (atual_dividendo_val >= divisor_val)
    {
        // subtrai RAM[pos_divisor] de RAM[pos_dividendo]
        inst[i++] = (Instrucao){1, pos_dividendo_internal, pos_divisor_internal, pos_dividendo_internal};

        // soma RAM[pos_um_internal] a RAM[pos_quociente_internal]
        inst[i++] = (Instrucao){0, pos_quociente_internal, pos_um_internal, pos_quociente_internal};

        atual_dividendo_val -= divisor_val; // Atualiza a variável C para a próxima iteração
        if (i >= (dividendo_val * 8 + 40))
        {
            printf("Aviso: Array de instruções quase cheio para Divisão. Considere alocação maior.\n");
            break;
        }
    }
    inst[i++] = (Instrucao){-1, -1, -1, -1}; // Instrução HALT

    cpu_set_programa(cpu, inst);
    cpu_iniciar(cpu, ram);
    ram_imprimir_exato(ram, pos_quociente_internal);
    free(inst);
}

void programaRaiz(CPU *cpu, RAM *ram, int n)
{
    if (n < 0)
    {
        printf("Erro: Não existe raiz de número negativo!\n");
        return;
    }
    // Posições na RAM
    int pos_n = rand() % ram->tamanho;
    int pos_cont = rand() % ram->tamanho;
    int pos_sub = rand() % ram->tamanho; //(2*cont+1)
    int pos_dois = rand() % ram->tamanho;
    int pos_um = rand() % ram->tamanho;

    Instrucao *inst = malloc(sizeof(Instrucao) * (n * 12 + 30));
    int i = 0;
    // Inicialização
    inst[i++] = (Instrucao){4, 1, n, -1};
    inst[i++] = (Instrucao){2, 1, pos_n, -1};
    inst[i++] = (Instrucao){4, 1, 0, -1};
    inst[i++] = (Instrucao){2, 1, pos_cont, -1};
    inst[i++] = (Instrucao){4, 1, 2, -1};
    inst[i++] = (Instrucao){2, 1, pos_dois, -1};
    inst[i++] = (Instrucao){4, 1, 1, -1};
    inst[i++] = (Instrucao){2, 1, pos_um, -1};

    int valor = n;
    int cont = 0;
    while (valor >= (2 * cont + 1))//1+3=4 ... 1+3+5=9 ... 1+3+5+7=16 etc...
    {
        
        // RAM[pos_cont] + RAM[pos_cont] -> RAM[pos_sub] (simula 2*cont)
        inst[i++] = (Instrucao){0, pos_cont, pos_cont, pos_sub};
        // RAM[pos_sub] + RAM[pos_um] -> RAM[pos_sub] (simula +1)
        inst[i++] = (Instrucao){0, pos_sub, pos_um, pos_sub};

        // RAM[pos_n] - RAM[pos_sub] -> RAM[pos_n]
        inst[i++] = (Instrucao){1, pos_n, pos_sub, pos_n};

        // RAM[pos_cont] + RAM[pos_um] -> RAM[pos_cont]
        inst[i++] = (Instrucao){0, pos_cont, pos_um, pos_cont};

        valor -= (2 * cont + 1);
        cont++;
    }
    inst[i++] = (Instrucao){-1, -1, -1, -1};
    cpu_set_programa(cpu, inst);
    cpu_iniciar(cpu, ram);
    ram_imprimir_exato(ram, pos_cont);
    free(inst);
}

void programaPot(CPU *cpu, RAM *ram, int base, int expoente)
{
    // Aloca posições na RAM para base, resultado e um valor temp
    int pos_base = rand() % ram->tamanho;
    int pos_resultado = rand() % ram->tamanho;
    int pos_temp_multiplicand = rand() % ram->tamanho; // Guarda o valor atual do resultado para a multiplicação
    int pos_zero = rand() % ram->tamanho;

    // Garante posições únicas na RAM
    while (pos_resultado == pos_base) pos_resultado = rand() % ram->tamanho;
    while (pos_temp_multiplicand == pos_base || pos_temp_multiplicand == pos_resultado) pos_temp_multiplicand = rand() % ram->tamanho;
    while (pos_zero == pos_base || pos_zero == pos_resultado || pos_zero == pos_temp_multiplicand) pos_zero = rand() % ram->tamanho;

    
    Instrucao *inst = malloc(sizeof(Instrucao) * 10000); 
    int i = 0;

    
    inst[i++] = (Instrucao){4, 1, base, -1};     // reg1 = base
    inst[i++] = (Instrucao){2, 1, pos_base, -1}; // reg1 -> RAM[pos_base]

    
    inst[i++] = (Instrucao){4, 1, 0, -1};     // reg1 = 0
    inst[i++] = (Instrucao){2, 1, pos_zero, -1}; // reg1 -> RAM[pos_zero]

    if (expoente == 0) {
        
        inst[i++] = (Instrucao){4, 1, 1, -1};     // reg1 = 1
        inst[i++] = (Instrucao){2, 1, pos_resultado, -1}; // reg1 -> RAM[pos_resultado]
    } else if (expoente == 1) {
        
        inst[i++] = (Instrucao){3, 1, pos_base, -1};     // reg1 = RAM[pos_base]
        inst[i++] = (Instrucao){2, 1, pos_resultado, -1}; // reg1 -> RAM[pos_resultado]
    } else {
        
        inst[i++] = (Instrucao){3, 1, pos_base, -1};     // reg1 = RAM[pos_base]
        inst[i++] = (Instrucao){2, 1, pos_resultado, -1}; // reg1 -> RAM[pos_resultado]

        
        for (int k = 0; k < expoente - 1; k++) {
            // Copia o valor atual de pos_resultado para pos_temp_multiplicand
            inst[i++] = (Instrucao){3, 1, pos_resultado, -1};     // reg1 = RAM[resultado]
            inst[i++] = (Instrucao){2, 1, pos_temp_multiplicand, -1}; // reg1 -> RAM[multiplicand]

            // Zera pos_resultado
            inst[i++] = (Instrucao){3, 1, pos_zero, -1};     // reg1 = RAM[pos_zero] (que é 0)
            inst[i++] = (Instrucao){2, 1, pos_resultado, -1}; // reg1 -> RAM[pos_resultado]

            
            for (int j = 0; j < base; j++) {
                inst[i++] = (Instrucao){0, pos_resultado, pos_temp_multiplicand, pos_resultado}; // RAM[pos_resultado] = RAM[pos_resultado] + RAM[pos_temp_multiplicand]
                if (i >= 9990) { 
                    printf("Aviso: Array de instruções quase cheio para Potência. Considere alocação maior.\n");
                    break;
                }
            }
            if (i >= 9990) break;
        }
    }

    inst[i++] = (Instrucao){-1, -1, -1, -1}; // Instrução HALT

    cpu_set_programa(cpu, inst);
    cpu_iniciar(cpu, ram);
    ram_imprimir_exato(ram, pos_resultado); // Imprime o resultado da potência da RAM
    free(inst);
}

int programaMultiAux(Instrucao* inst, int inst_pc, int m, int n) {
    int ramM = 2;
    int ramN = 1;
    int ramRes = 0;
    int i = 0;

    inst[inst_pc + i++] = (Instrucao){4, 1, m, -1};
    inst[inst_pc + i++] = (Instrucao){2, 1, ramM, -1};
    inst[inst_pc + i++] = (Instrucao){4, 2, n, -1};
    inst[inst_pc + i++] = (Instrucao){2, 2, ramN, -1};

    inst[inst_pc + i++] = (Instrucao){4, 1, 0, -1};
    inst[inst_pc + i++] = (Instrucao){2, 1, ramRes, -1};

    for (int j = 0; j < n; j++) {
        inst[inst_pc + i++] = (Instrucao){0, ramM, ramRes, ramRes};
    }

    return i;
} 

void programaProdutoEscalar(CPU* cpu, RAM* ram, int ax, int ay, int az, int bx, int by, int bz) {
    Instrucao* inst = malloc(sizeof(Instrucao) * 300);
    int inst_PC = 0;

    int pos_ax = 10, pos_ay = 11, pos_az = 12;
    int pos_bx = 13, pos_by = 14, pos_bz = 15;
    int pos_res1 = 16, pos_res2 = 17, pos_res3 = 18;
    int pos_restotal = 19;

    inst[inst_PC++] = (Instrucao){4, 1, ax, -1}; inst[inst_PC++] = (Instrucao){2, 1, pos_ax, -1};
    inst[inst_PC++] = (Instrucao){4, 1, ay, -1}; inst[inst_PC++] = (Instrucao){2, 1, pos_ay, -1};
    inst[inst_PC++] = (Instrucao){4, 1, az, -1}; inst[inst_PC++] = (Instrucao){2, 1, pos_az, -1};
    inst[inst_PC++] = (Instrucao){4, 2, bx, -1}; inst[inst_PC++] = (Instrucao){2, 2, pos_bx, -1};
    inst[inst_PC++] = (Instrucao){4, 2, by, -1}; inst[inst_PC++] = (Instrucao){2, 2, pos_by, -1};
    inst[inst_PC++] = (Instrucao){4, 2, bz, -1}; inst[inst_PC++] = (Instrucao){2, 2, pos_bz, -1};
    
    int instrucoes_aux;

    instrucoes_aux = programaMultiAux(inst, inst_PC, ax, bx);
    inst_PC += instrucoes_aux;
    inst[inst_PC++] = (Instrucao){3, 1, 0, -1};
    inst[inst_PC++] = (Instrucao){2, 1, pos_res1, -1};

    instrucoes_aux = programaMultiAux(inst, inst_PC, ay, by);
    inst_PC += instrucoes_aux;
    inst[inst_PC++] = (Instrucao){3, 1, 0, -1};
    inst[inst_PC++] = (Instrucao){2, 1, pos_res2, -1};

    instrucoes_aux = programaMultiAux(inst, inst_PC, az, bz);
    inst_PC += instrucoes_aux;
    inst[inst_PC++] = (Instrucao){3, 1, 0, -1};
    inst[inst_PC++] = (Instrucao){2, 1, pos_res3, -1};

    inst[inst_PC++] = (Instrucao){0, pos_res1, pos_res2, pos_restotal};
    inst[inst_PC++] = (Instrucao){0, pos_restotal, pos_res3, pos_restotal};

    inst[inst_PC++] = (Instrucao){-1, 0, 0, 0};

    cpu_set_programa(cpu, inst);
    cpu_iniciar(cpu, ram);
    
    ram_imprimir_exato(ram, pos_restotal);

    free(inst);
}

void programaFib(CPU* cpu, RAM* ram, int n) {
    if (n < 0) {
        printf("Erro: n deve ser um número não negativo para Fibonacci.\n");
        return;
    }
    
    
    // Calcular o número máximo de instruções necessárias
    int max_instrucoes;
    if (n == 0 || n == 1) {
        max_instrucoes = 3; 
    } else { // n >= 2
        max_instrucoes = 2 + (n - 1) * 6 + 1; 
    }
    
    Instrucao* inst = (Instrucao*)malloc(sizeof(Instrucao) * max_instrucoes);
    if (inst == NULL) {
        printf("Erro: Falha ao alocar memória para o programa.\n");
        return;
    }
    
    int pc = 0; // Contador de programa para preencher o array de instruções
    
    if (n == 0) {
        // F(0) = 0
        inst[pc++] = (Instrucao){4, 1, 0, 0}; 
        inst[pc++] = (Instrucao){2, 1, 2, 0}; 
    } else if (n == 1) {
        // F(1) = 1
        inst[pc++] = (Instrucao){4, 1, 1, 0}; 
        inst[pc++] = (Instrucao){2, 1, 2, 0}; 
    } else { // n >= 2

        inst[pc++] = (Instrucao){4, 1, 0, 0};
        inst[pc++] = (Instrucao){2, 1, 0, 0}; 
        

        inst[pc++] = (Instrucao){4, 1, 1, 0}; 
        inst[pc++] = (Instrucao){2, 1, 1, 0}; 

        for (int i = 2; i <= n; i++) {

            inst[pc++] = (Instrucao){0, 0, 1, 2}; 
            

            inst[pc++] = (Instrucao){3, 1, 1, 0}; 
            inst[pc++] = (Instrucao){2, 1, 0, 0}; 
            

            inst[pc++] = (Instrucao){3, 1, 2, 0};
            inst[pc++] = (Instrucao){2, 1, 1, 0};
        }
    }
    
    // Halt
    inst[pc++] = (Instrucao){-1, 0, 0, 0};
    
    cpu_set_programa(cpu, inst);
    cpu_iniciar(cpu, ram);
    
    printf("\nResultado final F(%d): ", n);
    ram_imprimir_exato(ram, 2); 
    
    free(inst);
}

void programaPitagoras(CPU* cpu, RAM* ram, int a, int b) {

   Instrucao* inst = (Instrucao*)malloc(sizeof(Instrucao) * (a + b + 19));
    
    int pc = 0;
    
    printf("Calculando a^2 + b^2 para a=%d, b=%d...\n", a, b);
    
    pc += programaMultiAux(inst, pc, a, a);
    

    inst[pc++] = (Instrucao){3, 1, 0, -1};
    inst[pc++] = (Instrucao){2, 1, 12, -1}; 
    

    pc += programaMultiAux(inst, pc, b, b);
    
    inst[pc++] = (Instrucao){3, 1, 0, -1};  
    inst[pc++] = (Instrucao){2, 1, 13, -1}; 
    
    inst[pc++] = (Instrucao){0, 12, 13, 14};

    //Halt
    inst[pc++] = (Instrucao){-1, 0, 0, 0};
    
    cpu_set_programa(cpu, inst);
    cpu_iniciar(cpu, ram);
    printf("O resultado é a raiz de: ");
    ram_imprimir_exato(ram, 14); 
    
    free(inst);
}

void programaEquacaoLin(CPU* cpu, RAM* ram, int x, int y, int z) {
    

    Instrucao* inst = (Instrucao*)malloc(sizeof(Instrucao) * 400);

    int pc = 0;
    
    printf("Calculando %d*%d + %d...\n", x, y, z);

    inst[pc++] = (Instrucao){4, 1, z, -1};
    inst[pc++] = (Instrucao){2, 1, 10, -1}; // 'z' salvo em RAM[10]

  
    // deixa o resultado em RAM[0].
    pc += programaMultiAux(inst, pc, x, y);


    // e salva em um local seguro (RAM[11]).
    inst[pc++] = (Instrucao){3, 1, 0, -1};  // Carrega RAM[0] no registrador
    inst[pc++] = (Instrucao){2, 1, 11, -1}; // Salva o resultado em RAM[11]

    //O resultado final será salvo em RAM[12].
    inst[pc++] = (Instrucao){0, 11, 10, 12};

    //Halt
    inst[pc++] = (Instrucao){-1, 0, 0, 0};


    cpu_set_programa(cpu, inst);
    cpu_iniciar(cpu, ram);
    printf("\nResultado final %d*%d + %d: ", x, y, z);
    ram_imprimir_exato(ram, 12); // Imprime o resultado final da RAM[12]

    free(inst);
}


int menu()
{
    int acao;
    printf("0 - Esvaziar RAM\n"
           "1 - Programa Aleatorio\n"
           "2 - Programa Multiplicacao\n"
           "3 - Programa Quadrado\n"
           "4 - Programa MDC\n"
           "5 - Programa Logaritimo\n"
           "6 - Programa Divisão\n"
           "7 - Programa Raiz\n"
           "8 - Programa Media\n"
           "9 - Programa Potencia\n"
           "10 - Programa Produto Escalar\n" 
           "11 - Programa Fib\n"
           "12 - Programa Pitagoras\n"
           "13 - Programa Eq Linear\n"
           "14 - Sair\n\n"
           "====================================");    
    printf("Digite a acao desejada: ");
    scanf("%d", &acao);

    return acao;
}
