/*
===============================================================================
MergeSort Recursivo para testes no gem5
===============================================================================

Formato do arquivo de entrada:
--------------------------------

Primeira linha:
    <quantidade_de_elementos>

Demais linhas:
    <valor>

Exemplo:
--------------------------------
5
10
4
8
1
7

===============================================================================
Compilação
===============================================================================

gcc -O2 mergesort.c -o mergesort

===============================================================================
Execução normal
===============================================================================

./mergesort array.txt

O arquivo txt é passado como arg da main na execução

===============================================================================
Execução no gem5
===============================================================================

build/X86/gem5.opt configs/example/se.py \
-c mergesort \
-o "array.txt"

===============================================================================
Objetivo arquitetural
===============================================================================

Este benchmark possui:
    - recursão intensa;
    - muitas chamadas de função;
    - branches frequentes;
    - uso de memória auxiliar;
    - comportamento misto entre CPU-bound e memory-bound.

Adequado para análise de:
    - branch prediction;
    - uso de stack;
    - cache;
    - IPC;
    - impacto de chamadas recursivas.

===============================================================================
ATENÇÃO
===============================================================================

    Com um tamanho de array relevante a leitura do arquivo não deve impactar
expressivamente nos resultados obtidos.
    Entretanto é possível descartar as leituras de estatísticas desta etapa 
através do script python do gem5, utilizando m5.stats.reset(), após a leitura 
do arquivo e alocação de memória, e m5.stats.dump().

===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100000
#define PROG_NAME "/.mergesort"
#define INPUT_FILE "array.txt"

/* Função de merge */
void merge(int arr[], int left, int mid, int right) {

    int n1 = mid - left + 1;
    int n2 = right - mid;

    int* L = (int*) malloc(n1 * sizeof(int));
    int* R = (int*) malloc(n2 * sizeof(int));

    if (L == NULL || R == NULL) {
        printf("Erro de alocacao\n");
        exit(1);
    }

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];

    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0;
    int j = 0;
    int k = left;

    while (i < n1 && j < n2) {

        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }

        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

/* MergeSort recursivo */
void mergeSort(int arr[], int left, int right) {

    if (left < right) {

        int mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}

/* Programa principal */
int main(int argc, char *argv[]) {

    if (argc < 2) {

        printf("Uso: %s <arquivo_entrada>\n", PROG_NAME);
        return 1;
    }

    FILE* file = fopen(INPUT_FILE, "r");

    if (file == NULL) {

        printf("Erro ao abrir arquivo\n");
        return 1;
    }

    int n;

    fscanf(file, "%d", &n);

    if (n > MAX_SIZE) {

        printf("Numero maximo de elementos: %d\n", MAX_SIZE);
        fclose(file);
        return 1;
    }

    int* arr = (int*) malloc(n * sizeof(int));

    if (arr == NULL) {

        printf("Erro de alocacao\n");
        fclose(file);
        return 1;
    }

    for (int i = 0; i < n; i++) {
        fscanf(file, "%d", &arr[i]);
    }

    fclose(file);
    
    //Se quiser anotar estatísticas só a partir daqui com m5.stats.reset()

    /* Executa MergeSort */
    mergeSort(arr, 0, n - 1);

    /* Libera Memória */
    free(arr);

    return 0;
}