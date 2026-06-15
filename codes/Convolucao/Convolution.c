/*
===============================================================================
Convolução 2D para testes no gem5
===============================================================================

Formato do arquivo de entrada:
--------------------------------

Primeira linha:
    <linhas> <colunas>

Demais linhas:
    matriz de entrada

Exemplo:
--------------------------------
4 4
1 2 3 4
5 6 7 8
9 1 2 3
4 5 6 7

O kernel utilizado é fixo (3x3) - Hard Coded - Se quiser, alterar na main:

    0 -1  0
   -1  5 -1
    0 -1  0

===============================================================================
Compilação
===============================================================================

gcc -O2 convolucao.c -o convolucao

===============================================================================
Execução normal
===============================================================================

./convolucao matriz.txt

O arquivo txt é enviado como arg da main

===============================================================================
Execução no gem5
===============================================================================

build/X86/gem5.opt configs/example/se.py \
-c convolucao \
-o "matriz.txt"

===============================================================================
Objetivo arquitetural
===============================================================================

Este benchmark possui:
    - muitos acessos regulares à memória;
    - forte localidade espacial;
    - loops aninhados;
    - alta reutilização de cache;
    - padrão previsível de execução.

Adequado para análise de:
    - hierarquia de cache;
    - IPC;
    - throughput de memória;
    - prefetching;
    - comportamento compute-bound.

===============================================================================
ATENÇÃO
===============================================================================

    Com um tamanho de matriz relevante a leitura do arquivo não deve impactar
expressivamente nos resultados obtidos.
    Entretanto é possível descartar as leituras de estatísticas desta etapa 
através do script python do gem5, utilizando m5.stats.reset(), após a leitura 
do arquivo e alocação de memória, e m5.stats.dump().

===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 1024

/* Kernel 3x3 */
int kernel[3][3] = {
    { 0, -1,  0},
    {-1,  5, -1},
    { 0, -1,  0}
};

/* Convolução */
void convolution2D(int** input, int** output, int rows, int cols) {

    for (int i = 1; i < rows - 1; i++) {

        for (int j = 1; j < cols - 1; j++) {

            int sum = 0;

            for (int ki = -1; ki <= 1; ki++) {

                for (int kj = -1; kj <= 1; kj++) {

                    sum += input[i + ki][j + kj] *
                           kernel[ki + 1][kj + 1];
                }
            }

            output[i][j] = sum;
        }
    }
}

/* Programa principal */
int main(int argc, char *argv[]) {

    if (argc < 2) {

        printf("Uso: %s <arquivo_entrada>\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");

    if (file == NULL) {

        printf("Erro ao abrir arquivo\n");
        return 1;
    }

    printf("Abriu o arquivo %s", argv[1]);

    int rows, cols;

    fscanf(file, "%d %d", &rows, &cols);

    if (rows > MAX_SIZE || cols > MAX_SIZE) {

        printf("Tamanho maximo: %d x %d\n",
               MAX_SIZE, MAX_SIZE);

        fclose(file);
        return 1;
    }

    /* Aloca matriz de entrada */
    int** input = (int**) malloc(rows * sizeof(int*));
    int** output = (int**) malloc(rows * sizeof(int*));

    if (input == NULL || output == NULL) {

        printf("Erro de alocacao\n");
        fclose(file);
        return 1;
    }

    for (int i = 0; i < rows; i++) {

        input[i] = (int*) malloc(cols * sizeof(int));
        output[i] = (int*) malloc(cols * sizeof(int));

        if (input[i] == NULL || output[i] == NULL) {

            printf("Erro de alocacao\n");
            fclose(file);
            return 1;
        }
    }

    /* Lê matriz */
    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {

            fscanf(file, "%d", &input[i][j]);
        }
    }

    fclose(file);

    //Se quiser anotar estatísticas só a partir daqui com m5.stats.reset()

    /* Executa convolução */
    convolution2D(input, output, rows, cols);

    /* Libera memória */
    for (int i = 0; i < rows; i++) {

        free(input[i]);
        free(output[i]);
    }

    free(input);
    free(output);

    return 0;
}