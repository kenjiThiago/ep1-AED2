#include <stdio.h>
#include "grafo_matrizadj.h"

bool iniciaGrafo(Grafo* g, int nv)
{
    if (nv > MAXIMO_VERTICES) {
        fprintf(stderr, "O numero de vertices (%i) e maior que o maximo (%i)\n", nv, MAXIMO_VERTICES);
        return false;
    }
    if (nv <= 0) {
        fprintf(stderr, "O numero de vertices (%i) nao e positivo\n", nv);
    }
    g->numeroVertices = nv;
    g->numeroArestas = 0;
    for (int i = 0; i < nv; i++) {
        for(int j = 0; j < nv; j++) {
            g->mat[i][j] = AN;
        }
    }
    return true;
}

void imprimeGrafo(Grafo* g)
{
    for (int i = 0; i < g->numeroVertices; i++) {
        for (int j = 0; j < g->numeroVertices; j++) {
            printf("%.1lf ", g->mat[i][j]);
        }
        printf("\n");
    }
}

bool verificaVeracidadeVertice(int v, Grafo* g)
{
    if (v >= g->numeroVertices) {
        fprintf(stderr, "O vertice (%i) e maior ou igual a o maximo (%i)\n", v, g->numeroVertices);
        return false;
    }
    if (v < 0) {
        fprintf(stderr, "O vertice (%i) e negativo\n", v);
        return false;
    }
    return true;
}

void insereAresta(int v1, int v2, Peso peso, Grafo* g)
{
    if(!(verificaVeracidadeVertice(v1, g) && verificaVeracidadeVertice(v2, g))) return;

    g->mat[v1][v2] = peso;
    g->mat[v2][v1] = peso;
    g->numeroArestas += 1;
}

bool existeAresta(int v1, int v2, Grafo* g)
{
    if (!(verificaVeracidadeVertice(v1, g) && verificaVeracidadeVertice(v2, g))) return false;

    if (g->mat[v1][v2] == AN) return false;

    return true;
}

Peso obtemPesoAresta(int v1, int v2, Grafo* g)
{
    if (!(verificaVeracidadeVertice(v1, g) && verificaVeracidadeVertice(v2, g))) return VERTICE_INVALIDO;

    return g->mat[v1][v2];
}

bool removeAresta(int v1, int v2, Peso* peso, Grafo* g)
{
    if (!(verificaVeracidadeVertice(v1, g) && verificaVeracidadeVertice(v2, g))) return false;

    if (g->mat[v1][v2] != AN) {
        *peso = g->mat[v1][v2];
        g->mat[v1][v2] = AN;
        g->mat[v2][v1] = AN;
        g->numeroArestas -= 1;
        return true;
    }

    return false;
}

bool listaAdjVazia(int v, Grafo* g)
{
    if (!verificaVeracidadeVertice(v, g)) return true;

    for (int i = 0; i < g->numeroVertices; i++) {
        if (g->mat[v][i] != AN) return false;
    }

    return true;
}

Apontador proxListaAdj(int v, Grafo* g, Apontador atual)
{
    if (!verificaVeracidadeVertice(v, g)) return VERTICE_INVALIDO;

    for (int i = atual + 1; i < g->numeroVertices; i++) {
        if (g->mat[v][i] != AN) return i;
    }

    return VERTICE_INVALIDO;
}

Apontador primeiroListaAdj(int v, Grafo* g)
{
    if (!verificaVeracidadeVertice(v, g)) return VERTICE_INVALIDO;

    for (int i = 0; i < g->numeroVertices; i++) {
        if (g->mat[v][i] != AN) return i;
    }

    return VERTICE_INVALIDO;
}

int verticeDestino(Apontador p, Grafo* g)
{
    return p;
}

void liberaGrafo(Grafo* g) {}

int leGrafo(char* nomeArquivo, Grafo* g)
{
    FILE* f = fopen(nomeArquivo, "r");
    if (f == NULL) {
        fprintf(stderr, "Problema ao abrir o arquivo de entrada\n");
        return 0;
    }

    int numeroVertices, numeroArestas;
    if (fscanf(f, "%i %i", &numeroVertices, & numeroArestas) != 2) {
        fprintf(stderr, "O arquivo nao esta correto\n");
        return 0;
    }

    iniciaGrafo(g, numeroVertices);

    for (int i = 0; i < numeroArestas; i++) {
        int vorigem, vdest;
        double peso;

        if (fscanf(f, "%i %i %lf", &vorigem, &vdest, &peso) != 3) {
            fprintf(stderr, "O arquivo nao esta correto\n");
            return 0;
        }

        insereAresta(vorigem, vdest, peso, g);
    }

    fclose(f);

    return 1;
}
