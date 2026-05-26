#ifndef NUCLEO_H_INCLUDED
#define NUCLEO_H_INCLUDED

#include "system.h"


typedef enum {
    ATIVO,
    BLOQ_P,
    TERMINADO
} ESTADO_PROC;

typedef struct bcp_t {
    char nome[30];
    int estado;
    PTR_DESC contexto;

    struct bcp_t *fila_sem;
    struct bcp_t *prox_desc;
    void (*codigo)(void);
} BCP;

typedef BCP* PTR_DESC_PROC;

extern PTR_DESC_PROC prim;
extern PTR_DESC_PROC atual;

void inicia_fila_prontos(void);

int cria_processo(void (*tarefa)(void), char* nome);

void termina_processo(void);

void dispara_sistema(void);
void yield(void);
PTR_DESC_PROC proximo_ativo_depois(PTR_DESC_PROC a_partir);

#endif // NUCLEO_H_INCLUDED
