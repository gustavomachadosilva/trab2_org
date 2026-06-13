/*
===============================================================================
BFS (Breadth-First Search) para testes no gem5
===============================================================================

Formato do arquivo de entrada:
--------------------------------

Primeira linha:
    <vertices> <arestas>

Demais linhas:
    <origem> <destino>

Exemplo:
--------------------------------
6 7
0 1
0 2
1 3
1 4
2 4
3 5
4 5

===============================================================================
Compilação
===============================================================================

gcc -O2 bfs.c -o bfs

===============================================================================
Execução normal
===============================================================================

./bfs grafo.txt

o arquivo txt é passado como arg para a main

===============================================================================
Execução no gem5
===============================================================================

build/X86/gem5.opt configs/example/se.py \
-c bfs \
-o "grafo.txt"

===============================================================================
Objetivo arquitetural
===============================================================================

Este benchmark possui:
    - acessos irregulares à memória;
    - uso intenso de ponteiros;
    - muitos branches;
    - baixa localidade espacial;
    - comportamento memory-bound.

Adequado para análise de:
    - cache misses;
    - branch prediction;
    - stalls de memória;
    - IPC;
    - impacto da hierarquia de memória.

===============================================================================
ATENÇÃO
===============================================================================

    Com um tamanho de grafo relevante a leitura do arquivo não deve impactar
expressivamente nos resultados obtidos.
    Entretanto é possível descartar as leituras de estatísticas desta etapa 
através do script python do gem5, utilizando m5.stats.reset(), após a leitura 
do arquivo e alocação de memória, e m5.stats.dump().

===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>

#define MAX_VERTICES 100000
#define PROG_NAME "/.bfs"
#define INPUT_FILE "grafo.txt"

typedef struct Node {
    int vertex;
    struct Node* next;
} Node;

typedef struct Queue {
    int* items;
    int front;
    int rear;
} Queue;

Node* graph[MAX_VERTICES];
int visited[MAX_VERTICES];

/* Cria novo nó */
Node* createNode(int v) {

    Node* newNode = (Node*) malloc(sizeof(Node));

    if (newNode == NULL) {
        printf("Erro de alocacao\n");
        exit(1);
    }

    newNode->vertex = v;
    newNode->next = NULL;

    return newNode;
}

/* Adiciona aresta */
void addEdge(int src, int dest) {

    Node* newNode = createNode(dest);

    newNode->next = graph[src];
    graph[src] = newNode;
}

/* Inicializa fila */
void initQueue(Queue* q, int size) {

    q->items = (int*) malloc(size * sizeof(int));

    if (q->items == NULL) {
        printf("Erro de alocacao da fila\n");
        exit(1);
    }

    q->front = 0;
    q->rear = -1;
}

/* Verifica se fila está vazia */
int isEmpty(Queue* q) {
    return q->rear < q->front;
}

/* Enfileira */
void enqueue(Queue* q, int value) {

    q->rear++;
    q->items[q->rear] = value;
}

/* Desenfileira */
int dequeue(Queue* q) {

    int item = q->items[q->front];
    q->front++;

    return item;
}

/* BFS */
void bfs(int startVertex, int vertices) {

    Queue q;

    initQueue(&q, vertices);

    visited[startVertex] = 1;
    enqueue(&q, startVertex);

    while (!isEmpty(&q)) {

        int currentVertex = dequeue(&q);

        Node* temp = graph[currentVertex];

        while (temp != NULL) {

            int adjVertex = temp->vertex;

            if (!visited[adjVertex]) {

                visited[adjVertex] = 1;
                enqueue(&q, adjVertex);
            }

            temp = temp->next;
        }
    }

    free(q.items);
}

/* Libera memória */
void freeGraph(int vertices) {

    for (int i = 0; i < vertices; i++) {

        Node* temp = graph[i];

        while (temp != NULL) {

            Node* next = temp->next;

            free(temp);

            temp = next;
        }
    }
}

/* Programa principal */
int main(int argc, char *argv[]) {

    if (argc < 2) {

        printf("Uso: %s <arquivo_grafo>\n", PROG_NAME);
        return 1;
    }

    FILE* file = fopen(INPUT_FILE, "r");

    if (file == NULL) {

        printf("Erro ao abrir arquivo\n");
        return 1;
    }

    int vertices, edges;

    fscanf(file, "%d %d", &vertices, &edges);

    if (vertices > MAX_VERTICES) {

        printf("Numero maximo de vertices: %d\n", MAX_VERTICES);
        fclose(file);
        return 1;
    }

    /* Inicializa estruturas */
    for (int i = 0; i < vertices; i++) {

        graph[i] = NULL;
        visited[i] = 0;
    }

    /* Lê arestas do arquivo */
    for (int i = 0; i < edges; i++) {

        int src, dest;

        fscanf(file, "%d %d", &src, &dest);

        addEdge(src, dest);
    }

    fclose(file);

    //Se quiser anotar estatísticas só a partir daqui com m5.stats.reset()

    /* Executa BFS */
    bfs(0, vertices);

    /* Libera memória */
    freeGraph(vertices);

    return 0;
}