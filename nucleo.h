#ifndef NUCLEO_H_INCLUDED
#define NUCLEO_H_INCLUDED

#include "system.h"

#define MAX_PROCESSOS 10

#define ESTADO_LIVRE 0
#define ESTADO_PRONTO 1
#define ESTADO_EXECUTANDO 2
#define ESTADO_TERMINADO 3

typedef struct{
    int id;
    char nome[30];
    int estado;
    PTR_DESC contexto;
}BCP;

void inicia_fila_prontos();

int cria_processo(void (*tarefa)(void), char* nome);

void termina_processo();

#endif // NUCLEO_H_INCLUDED
