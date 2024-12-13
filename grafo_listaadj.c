#include <stdio.h>
#include <stdlib.h>
#include "grafo_listaadj.h"

bool iniciaGrafo(Grafo* g, int nv)
{
    if (nv <= 0) {
        fprintf(stderr, "O numero de vertices (%i) nao e positivo\n", nv);
        return false;
    }

    g->numeroVertices = nv;
    if (!(g->listaAdj = (Apontador*) calloc(sizeof(Apontador), nv))) {
        fprintf(stderr, "Falha na alocacao de memoria na funcao iniciaGrafo\n");
        return false;
    }

    g->numeroArestas = 0;

    return true;
}

void imprimeGrafo(Grafo* g)
{
    Apontador p;
    for (int i = 0; i < g->numeroVertices; i++) {
        p = g->listaAdj[i];
        printf("%i: ", i);
        while (p) {
            printf("(%i, %.1lf)", p->vdest, p->peso);
            p = p->prox;
        }
        printf("\n");
    }
}

bool verificaVeracidadeVertice(int v, Grafo* g)
{
    if (v >= g-> numeroVertices) {
        fprintf(stderr, "O vertice (%i) e maior ou igual a o maximo de vertices (%i)\n", v, g->numeroVertices);
        return false;
    }
    if (v < 0) {
        fprintf(stderr, "O vertice (%i) e negativo\n", v);
        return false;
    }
    return true;
}

Apontador criaNovaAresta(int vOrigem, int vDest, Peso peso, Grafo* g)
{
    Apontador n = NULL;

    if (!(n = (Apontador) malloc(sizeof(Aresta)))) {
        fprintf(stderr, "Falha na alocacao de memoria em insereAresta\n");
        return VERTICE_INVALIDO;
    }

    n->peso = peso;
    n->vdest = vDest;
    n->prox = g->listaAdj[vOrigem];

    return n;
}

void insereAresta(int v1, int v2, Peso peso, Grafo* g)
{
    if (!(verificaVeracidadeVertice(v1, g) && verificaVeracidadeVertice(v2, g))) return;

    g->listaAdj[v1] = criaNovaAresta(v1, v2, peso, g);

    g-> listaAdj[v2] = criaNovaAresta(v2, v1, peso, g);

    g->numeroArestas += 1;
}

bool existeAresta(int v1, int v2, Grafo* g)
{
    if (!(verificaVeracidadeVertice(v1, g) && verificaVeracidadeVertice(v2, g))) return false;

    Apontador q1 = g->listaAdj[v1];

    while (q1) {
        if (q1->vdest == v2) return true;
        q1 = q1->prox;
    }
    return false;
}

Peso obtemPesoAresta(int v1, int v2, Grafo* g)
{
    if (!(verificaVeracidadeVertice(v1, g) && verificaVeracidadeVertice(v2, g))) return AN;

    Apontador q1 = g->listaAdj[v1];
    while (q1) {
        if (q1->vdest == v2) return q1->peso;
        q1 = q1->prox;
    }

    return AN;
}

void liberaAresta(int v, Grafo* g, Apontador q, Apontador ant)
{
    if (g->listaAdj[v] == q) g->listaAdj[v] = q->prox;
    else ant->prox = q->prox;


    q->prox = NULL;
    free(q);
    q = NULL;
}

Apontador buscaAresta(int vOrigem, int vDest, Grafo* g, Apontador* ant)
{
    Apontador q = g->listaAdj[vOrigem];
    *ant = NULL;

    while (q) {
        if (q->vdest == vDest) return q;
        *ant = q;
        q = q->prox;
    }

    return NULL;
}

bool removeAresta(int v1, int v2, Peso* peso, Grafo* g)
{
    if (!(verificaVeracidadeVertice(v1, g) && verificaVeracidadeVertice(v2, g))) return false;

    Apontador ant1;
    Apontador q1 = buscaAresta(v1, v2, g, &ant1);

    Apontador ant2;
    Apontador q2 = buscaAresta(v2, v1, g, &ant2);

    if (q1 != NULL && q2 != NULL) {
        *peso = q1->peso;
        liberaAresta(v1, g, q1, ant1);
        q1 = NULL;

        liberaAresta(v2, g, q2, ant2);
        q2 = NULL;

        g->numeroArestas -= 1;

        return true;
    }

    return false;
}

bool listaAdjVazia(int v, Grafo* g)
{
    if (!verificaVeracidadeVertice(v, g)) return false;

    return (g->listaAdj[v] == NULL);
}

Apontador proxListaAdj(int v, Grafo* g, Apontador atual)
{
    if (atual == NULL) {
        fprintf(stderr, "atual == NULL\n");
        return VERTICE_INVALIDO;
    }

    return (atual->prox);
}

Apontador primeiroListaAdj(int v, Grafo* g)
{
    if (!verificaVeracidadeVertice(v, g)) return VERTICE_INVALIDO;
    return g->listaAdj[v];
}

int verticeDestino(Apontador p, Grafo* g)
{
    return p->vdest;
}

void liberaGrafo(Grafo* g)
{
    Apontador p;
    for (int i = 0; i < g->numeroVertices; i++) {
        while (g->listaAdj[i]) {
            p = g->listaAdj[i];
            g->listaAdj[i] = p->prox;
            p->prox = NULL;
            free(p);
        }
    }

    g->numeroVertices = 0;
    g->numeroArestas = 0;

    free(g->listaAdj);
    g->listaAdj = NULL;
}

int leGrafo(char* nomeArquivo, Grafo* g)
{
    FILE* f = fopen(nomeArquivo, "r");

    int numeroVertices, numeroArestas;
    if (fscanf(f, "%i %i", &numeroVertices, &numeroArestas) != 2) {
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
