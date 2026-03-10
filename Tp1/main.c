#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "ram.h"
#include "programas.h"
// #include "cpu.c"
// #include "ram.c"
// #include "programas.c"

int main()
{

    RAM *ram = criar_ram(30);
    CPU *cpu = malloc(sizeof(CPU));

    int acao = 1;
    while (acao != 14)
    {
        acao = menu();
        switch (acao) // menu n pode ficar aq se nao da pau
        {

        case 0:
        {
            esvazia_ram(ram);
            printf("RAM foi esvaziada com sucesso...\n");
            break;
        }

        case 1:
        { // Programa Aleatorio-ok
            int qtdInst;
            printf("Digite a quantidade de instruções: ");
            scanf("%d", &qtdInst);
            programaAleatorio(cpu, ram, qtdInst);
            break;
        }
        case 2:
        { // Programa Multiplicacao-ok
            int multiplicador, multiplicando;
            printf("Digite o multiplicador: ");
            scanf("%d", &multiplicador);
            printf("Digite o multiplicando: ");
            scanf("%d", &multiplicando);
            programaMulti(cpu, ram, multiplicador, multiplicando);
            break;
        }

        case 3:
        { // Programa Potência-ok
            int base;
            printf("Digite a base: ");
            scanf("%d", &base);
            programaQuad(cpu, ram, base);
            break;
        }
        case 4:
        { // Programa MDC-ok
            int n1, n2;
            printf("Digite 2 números inteiros: ");
            scanf("%d%d", &n1, &n2);
            programaMDC(cpu, ram, n1, n2);
            break;
        }
        case 5:
        { // Programa Logarítmico-ok
            int numero, base;
            printf("Digite o número: ");
            scanf("%d", &numero);
            printf("Digite a base: ");
            scanf("%d", &base);
            programaLog(cpu, ram, numero, base);

            break;
        }
        case 6:
        { // Programa Divisão-ok
            int divisor, dividendo;
            printf("Digite o dividendo: ");
            scanf("%d", &dividendo);
            printf("Digite o divisor: ");
            scanf("%d", &divisor);
            programaDiv(cpu, ram, dividendo, divisor);

            break;
        }
        case 7:
        { // Programa Raiz-ok
            int numero;
            printf("Digite o número: ");
            scanf("%d", &numero);
            programaRaiz(cpu, ram, numero);

            break;
        }
        case 8:
        { // Programa Media-ok
            int n1, n2;
            printf("Digite os números: ");
            scanf("%d%d", &n1, &n2);
            programaMedia(cpu, ram, n1, n2);

            break;
        }
        case 9:
        { // pottencia-ok
            int base, expoente;
            printf("Digite base e expoente: ");
            scanf("%d%d", &base, &expoente);
            programaPot(cpu, ram, base, expoente);

            break;
        }
        case 10:
        { // produto escalar-ok
            int x1, y1, z1, x2, y2, z2;
            printf("Digite o x1,y1,z1: ");
            scanf("%d%d%d", &x1, &y1, &z1);
            printf("Digite o x2,y2,z2: ");
            scanf("%d%d%d", &x2, &y2, &z2);
            programaProdutoEscalar(cpu, ram, x1, y1, z1, x2, y2, z2);

            break;
        }
        case 11:
        { // fib-ok
            int n;
            printf("Digite o numero: ");
            scanf("%d", &n);
            programaFib(cpu, ram, n);

            break;
        }
        case 12:
        { // pitagoras-ok
            int a, b;
            printf("Digite A e B: ");
            scanf("%d%d", &a, &b);
            programaPitagoras(cpu, ram, a, b);

            break;
        }
        case 13:
        { // equaçao linear-ok
            int x, y, z;
            printf("Digite x, y, z: ");
            scanf("%d%d%d", &x, &y, &z);
            programaEquacaoLin(cpu, ram, x, y, z);

            break;
        }

        case 14:
        { // Sair

            printf("Saindo...\n");
            break;
        }
        default:
        {
            printf("Acão inválida.\n");
            break;
        }
        }
    }
    free(cpu);
    destruir_ram(ram);

    return 0;
}
