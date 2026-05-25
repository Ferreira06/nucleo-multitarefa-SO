#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nucleo.h"

BCP tabela_processos[MAX_PROCESSOS];

int fila_prontos[MAX_PROCESSOS];
int inicio_fila = 0;
int fim_fila = 0;
int qtd_prontos = 0;

BCP* processo_atual = NULL;

void inicia_fila_prontos(){
    for(int i=0;i<MAX_PROCESSOS;i++){
        tabela_processos[i].id = i;
        tabela_processos[i].estado = ESTADO_LIVRE;
        tabela_processos[i].contexto = NULL;
    }

    printf("[NUCLEO] Fila circular iniciada!\n\n");
}

int cria_processo(void (*tarefa)(void), char* nome){
    int id_livre = -1;

    //busca pelo proximo ESTADO_LIVRE
    for(int i=0;i<MAX_PROCESSOS;i++){
        if(tabela_processos[i].estado == ESTADO_LIVRE){
            id_livre = i;
            break;
        }
    }

    // caso esteja lotado, sem slots livres
    if(id_livre == -1){
        printf("ERRO! Limite de processos atingido. Nao foi possivel criar: %s\n", nome);
        return -1;
    }

    //atualiza o estado do processo
    tabela_processos[id_livre].estado = ESTADO_PRONTO;

    strcpy(tabela_processos[id_livre].nome, nome);

    tabela_processos[id_livre].contexto = (PTR_DESC)malloc(sizeof(descritor));
    if (tabela_processos[id_livre].contexto == NULL) {
        printf("ERRO! Falha de memoria ao criar o contexto de: %s\n", nome);
        return -1;
    }

    //inicializa a fiber
    newprocess((proc_fn)tarefa, NULL, tabela_processos[id_livre].contexto);

    fila_prontos[fim_fila] = id_livre;

    fim_fila = (fim_fila + 1) % MAX_PROCESSOS;
    qtd_prontos++;

    printf("[NUCLEO] Processo %s alocado com sucesso (ID: %d) e enviado ao fim da fila!\n", nome, id_livre);
    return id_livre;
}

void termina_processo() {

    if (processo_atual == NULL) {
        return;
    }

    printf("[NUCLEO] Processo %s solicitou finalizacao!\n", processo_atual->nome);

    //atualiza o estado do processo
    processo_atual->estado = ESTADO_TERMINADO;

    if (processo_atual->contexto != NULL) {
        free(processo_atual->contexto);
        processo_atual->contexto = NULL;
    }

    processo_atual->estado = ESTADO_LIVRE;

    extern void processo_trampolim();
    processo_trampolim();
}
