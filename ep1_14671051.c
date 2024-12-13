#ifdef MATRIZ
#include "grafo_matrizadj.h"
#else
#include "grafo_listaadj.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct no {
    int vertice;
    struct no* prox;
} NO;

typedef struct fila {
    NO* inicio;
    NO* fim;
} Fila;

void iniciaFila(Fila* f)
{
    f->inicio = NULL;
    f->fim = NULL;
}

bool insereVerticeFila(Fila* f, int vertice)
{
    NO* novo;
    if (!(novo  = (NO*) malloc(sizeof(NO)))) {
        fprintf(stderr, "Ocorreu um erro na alocacao de memoria de um novo NO da fila\n");
        return false;
    }
    novo->vertice = vertice;
    novo->prox = NULL;

    if (f->inicio == NULL) f->inicio = novo;
    else f->fim->prox = novo;
    f->fim = novo;

    return true;
}

int removeVerticeFila(Fila* f)
{
    if (!f->inicio) return -1;

    NO* aux = f->inicio;
    int vertice = aux->vertice;
    f->inicio = f->inicio->prox;

    free(aux);
    if (!f->inicio) f->fim = NULL;

    return vertice;
}

int* iniciaVetor(int tamanho)
{
    int* vetor;

    if (!(vetor = (int*) malloc(sizeof(int) * tamanho))) return NULL;

    for (int i = 0; i < tamanho; i++) {
        vetor[i] = -1;
    }

    return vetor;
}

void imprimeCaminho(int vInicial, int v, int* verticeAntecessor, FILE* saida)
{
    if (vInicial == v) {
        fprintf(saida, "%i", vInicial);
        return;
    } else {
        imprimeCaminho(vInicial, verticeAntecessor[v], verticeAntecessor, saida);
        fprintf(saida, " %i", v);
    }
}

bool visitaLargura(int v, Grafo* g, char* cor, int* distancia, int* verticeAntecessor, FILE* saida)
{
    cor[v] = 'c';
    distancia[v] = 0;

    Fila f;
    iniciaFila(&f);

    if (!insereVerticeFila(&f, v)) return false;
    fprintf(saida, "%i", v);
    while (f.inicio) {
        int w = removeVerticeFila(&f);

        Apontador verticeAdj = primeiroListaAdj(w, g);
        while (verticeAdj != VERTICE_INVALIDO) {
            int verticeDest = verticeDestino(verticeAdj, g);
            if (cor[verticeDest] == 'b') {
                fprintf(saida, " %i", verticeDest);
                distancia[verticeDest] = distancia[w] + 1;
                verticeAntecessor[verticeDest] = w;
                cor[verticeDest] = 'c';
                insereVerticeFila(&f, verticeDest);
            }
            verticeAdj = proxListaAdj(w, g, verticeAdj);
        }

        cor[w] = 'p';
    }

    return true;
}

void caminhoBuscaLargura(Grafo* g, int* distancia, int* verticeAntecessor, FILE* saida)
{
    char* cor;
    if (!(cor = (char*) malloc(sizeof(char) * g->numeroVertices))) {
        fprintf(stderr, "Ocorreu um erro na alocacao de memoria do vetor cor\n");
        return;
    }
    memset(cor, 'b', g->numeroVertices);

    fprintf(saida, "BL:\n");
    for (int v = 0; v < g->numeroVertices; v++) {
        if (cor[v] == 'b') {
            if (v != 0) fprintf(saida, " ");
            if (!visitaLargura(v, g, cor, distancia, verticeAntecessor, saida)) return;
        }
    }
    fprintf(saida, "\n\n");

    free(cor);
    cor = NULL;
}

void caminhosNORaizBuscaLargura(Grafo* g, int* distancia, int* verticeAntecessor, FILE* saida)
{
    int vInicial = 0;
    fprintf(saida, "Caminhos BL:\n");
    for (int v = 0; v < g->numeroVertices; v++) {
        if (distancia[vInicial] == distancia[v]) vInicial = v;
        imprimeCaminho(vInicial, v, verticeAntecessor, saida);
        fprintf(saida, "\n");
    }
    fprintf(saida, "\n");
}

void buscaLargura(Grafo* g, FILE* saida)
{
    int* verticeAntecessor = iniciaVetor(g->numeroVertices);
    if (verticeAntecessor == NULL) {
        fprintf(stderr, "Ocorreu um erro na alocacao de memoria do vetor verticeAntecessor\n");
        return;
    }

    int* distancia = iniciaVetor(g->numeroVertices);
    if (distancia == NULL) {
        fprintf(stderr, "Ocorreu um erro na alocacao de memoria do vetor distancia\n");
        return;
    }
    caminhoBuscaLargura(g, distancia, verticeAntecessor, saida);
    caminhosNORaizBuscaLargura(g, distancia, verticeAntecessor, saida);

    free(distancia);
    distancia = NULL;
    free(verticeAntecessor);
    verticeAntecessor = NULL;
}

void visitaCaminho(int v, Grafo* g, int* tempo, int* tempoDescoberta, int* tempoTermino, char* cor, int* verticeAntecessor, FILE* saida)
{
    cor[v] = 'c';
    tempoDescoberta[v] = ++(*tempo);

    fprintf(saida, "%i", v);
    Apontador verticeAdj = primeiroListaAdj(v, g);
    while (verticeAdj != VERTICE_INVALIDO) {
        int verticeDest = verticeDestino(verticeAdj, g);
        if (cor[verticeDest] == 'b') {
            fprintf(saida, " ");
            verticeAntecessor[verticeDest] = v;
            visitaCaminho(verticeDest, g, tempo, tempoDescoberta, tempoTermino, cor, verticeAntecessor, saida);
        }
        verticeAdj = proxListaAdj(v, g, verticeAdj);
    }

    tempoTermino[v] = ++(*tempo);
    cor[v] = 'p';
}

void caminhoBuscaProfundidade(Grafo* g, int* tempoDescoberta, int* tempoTermino, int* verticeAntecessor, FILE* saida)
{
    char* cor;
    if (!(cor = (char*) malloc(sizeof(char) * g->numeroVertices))) {
        fprintf(stderr, "Ocorreu um erro na alocacao de memoria do vetor cor\n");
        return;
    }
    memset(cor, 'b', g->numeroVertices);

    int tempo = 0;
    fprintf(saida, "BP:\n");
    for (int v = 0; v < g->numeroVertices; v++) {
        if (cor[v] == 'b') {
            if (v != 0) fprintf(saida, " ");
            visitaCaminho(v, g, &tempo, tempoDescoberta, tempoTermino, cor, verticeAntecessor, saida);
        }
    }
    fprintf(saida, "\n\n");

    free(cor);
    cor = NULL;
}

void caminhosNORaizBuscaProfundidade(Grafo* g, int* tempoDescoberta, int* tempoTermino, int* verticeAntecessor, FILE* saida)
{
    int vInicial = 0;
    fprintf(saida, "Caminhos BP:\n");
    for (int v = 0; v < g->numeroVertices; v++) {
        if (tempoTermino[vInicial] < tempoDescoberta[v]) vInicial = v;
        imprimeCaminho(vInicial, v, verticeAntecessor, saida);
        fprintf(saida, "\n");
    }
    fprintf(saida, "\n");
}

void componentesConexos(Grafo* g, int* tempoDescoberta, int* tempoTermino, FILE* saida)
{
    int vInicial = 0;
    int componentes = 1;
    fprintf(saida, "Componentes Conectados:\n");
    fprintf(saida, "C%i :", componentes);
    for (int v = 0; v < g->numeroVertices; v++) {
        if (tempoTermino[vInicial] < tempoDescoberta[v]) {
            vInicial = v;
            fprintf(saida, "\nC%i :", ++componentes);
        }
        fprintf(saida, " %i", v);
    }
    fprintf(saida, "\n\n");
}

void buscaProfundidade(Grafo* g, FILE* saida)
{
    int* tempoDescoberta;
    if (!(tempoDescoberta  = (int*) calloc(sizeof(int), g->numeroVertices))) {
        fprintf(stderr, "Ocorreu um erro na alocacao de memoria do vetor tempoDescoberta\n");
        return;
    }
    int* tempoTermino = (int*) calloc(sizeof(int), g->numeroVertices);
    if (!(tempoDescoberta  = (int*) calloc(sizeof(int), g->numeroVertices))) {
        fprintf(stderr, "Ocorreu um erro na alocacao de memoria do vetor tempoTermino\n");
        return;
    }
    int* verticeAntecessor = iniciaVetor(g->numeroVertices);
    if (verticeAntecessor == NULL) {
        fprintf(stderr, "Ocorreu um erro na alocacao de memoria do vetor verticeAntecessor\n");
        return;
    }

    caminhoBuscaProfundidade(g, tempoDescoberta, tempoTermino, verticeAntecessor, saida);

    caminhosNORaizBuscaProfundidade(g, tempoDescoberta, tempoTermino, verticeAntecessor, saida);
    componentesConexos(g, tempoDescoberta, tempoTermino, saida);

    free(tempoDescoberta);
    tempoDescoberta = NULL;
    free(tempoTermino);
    tempoTermino = NULL;
    free(verticeAntecessor);
    verticeAntecessor = NULL;
}

int visitaBP(int v, Grafo* g, char* cor, int* tempo, int* tempoDescoberta, int* verticesDeArticulacao, int verticeRaiz, FILE* saida)
{
    cor[v] = 'c';
    int menorTempoAdj = ++(*tempo);
    tempoDescoberta[v] = *tempo;

    Apontador verticeAdj = primeiroListaAdj(v, g);

    int filhos = 0;
    while (verticeAdj != VERTICE_INVALIDO) {
        int verticeDest = verticeDestino(verticeAdj, g);
        if (cor[verticeDest] == 'b') {
            filhos += 1;
            int tempoAdj = visitaBP(verticeDest, g, cor, tempo, tempoDescoberta, verticesDeArticulacao, verticeRaiz, saida);

            if (tempoAdj < menorTempoAdj) menorTempoAdj = tempoAdj;

            if (filhos >= 2) verticesDeArticulacao[v] = 1;
            if (v != verticeRaiz) {
                if (tempoDescoberta[v] <= tempoAdj) verticesDeArticulacao[v] = 1;
            }
        }
        else if (tempoDescoberta[verticeDest] < menorTempoAdj)  menorTempoAdj = tempoDescoberta[verticeDest];

        verticeAdj = proxListaAdj(v, g, verticeAdj);
    }

    cor[v] = 'p';

    return menorTempoAdj;
}

void encontraVerticesDeArticulacao(Grafo* g, FILE* saida)
{
    char* cor;
    if (!(cor = (char*) malloc(sizeof(char) * g->numeroVertices))) {
        fprintf(stderr, "Ocorreu um erro na alocacao de memoria do vetor cor\n");
        return;
    }
    memset(cor, 'b', g->numeroVertices);

    int* tempoDescoberta;
    if (!(tempoDescoberta  = (int*) calloc(sizeof(int), g->numeroVertices))) {
        fprintf(stderr, "Ocorreu um erro na alocacao de memoria do vetor tempoDescoberta\n");
        return;
    }

    int* verticesDeArticulacao;
    if (!(verticesDeArticulacao  = calloc(sizeof(int), g->numeroVertices))) {
        fprintf(stderr, "Ocorreu um erro na alocacao de memoria do vetor verticesDeArticulacao\n");
        return;
    }

    int tempo = 0;
    fprintf(saida, "Vertices de articulacao:\n");
    for (int i = 0; i < g->numeroVertices; i++) {
        if (cor[i] == 'b') {
            int verticeRaiz = i;
            visitaBP(i, g, cor, &tempo, tempoDescoberta, verticesDeArticulacao, verticeRaiz, saida);
        }
    }

    int primeiro = 0;
    for (int i = 0; i < g->numeroVertices; i++) {
        if (verticesDeArticulacao[i] == 1) {
            if (primeiro == 0) fprintf(saida, "%i", i);
            else fprintf(saida, " %i", i);
            primeiro = 1;
        }
    }

    free(tempoDescoberta);
    tempoDescoberta = NULL;
    free(verticesDeArticulacao);
    verticesDeArticulacao = NULL;
    free(cor);
    cor = NULL;
}

int main(int argc, char** argv)
{
    Grafo g;

    char nomeEntrada[250] = {};
    strcpy(nomeEntrada, argv[1]);

    char nomeSaida[250] = {};
    strcpy(nomeSaida, argv[2]);

    FILE* saida = fopen(nomeSaida, "w");
    if (saida == NULL) {
        fprintf(stderr, "Problema ao abrir o arquivo de saida\n");
        return 1;
    }

    if (leGrafo(nomeEntrada, &g) == 0) return 1;

    buscaLargura(&g, saida);
    buscaProfundidade(&g, saida);
    encontraVerticesDeArticulacao(&g, saida);

    liberaGrafo(&g);

    fclose(saida);

    return 0;
}
