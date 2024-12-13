#include <stdbool.h>

#define VERTICE_INVALIDO NULL
#define AN -1

typedef double Peso;

typedef struct aresta {
    int vdest;
    Peso peso;
    struct aresta* prox;
} Aresta;

typedef Aresta* Apontador;

typedef struct {
    Apontador* listaAdj;
    int numeroVertices;
    int numeroArestas;
} Grafo;

bool iniciaGrafo(Grafo* g, int nv);

void imprimeGrafo(Grafo* g);

bool verificaVeracidadeVertice(int v, Grafo* g);

void insereAresta(int v1, int v2, Peso peso, Grafo* g);

bool existeAresta(int v1, int v2, Grafo* g);

Peso obtemPesoAresta(int v1, int v2, Grafo* g);

bool removeAresta(int v1, int v2, Peso* peso, Grafo* g);

bool listaAdjVazia(int v, Grafo* g);

Apontador proxListaAdj(int v, Grafo* g, Apontador atual);

Apontador primeiroListaAdj(int v, Grafo* g);

int verticeDestino(Apontador p, Grafo* g);

void liberaGrafo(Grafo* g);

int leGrafo(char* nomeArquivo, Grafo* g);
