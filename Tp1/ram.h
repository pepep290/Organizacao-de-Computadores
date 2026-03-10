#ifndef ram_h
#define ram_h

typedef struct {
    int* dados;
    int tamanho;
} RAM;

RAM* criar_ram(int tamanho);
RAM* ram_aleatoria(RAM* ram);
RAM* esvazia_ram(RAM* ram);
void destruir_ram(RAM* ram);
int ram_get_dado(RAM* ram, int endereco);
void ram_set_dado(RAM* ram, int endereco, int valor);
void ram_imprimir(RAM* ram);
void ram_imprimir_exato(RAM* ram, int add);

#endif