#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nucleo.h"

/* Instanciação das variáveis reais de rastreamento da fila circular */
PTR_DESC_PROC prim = NULL;
PTR_DESC_PROC atual = NULL;

/* Contexto isolado para a função main do Windows (suporte do módulo system.c) */
static descritor main_desc;
static PTR_DESC main_ctx = &main_desc;
static int main_ready = 0;

static void processo_trampolim(void *arg) {
    PTR_DESC_PROC proc = (PTR_DESC_PROC)arg;

    if (proc && proc->codigo) {
        // Executa a função lógica real do usuário
        proc->codigo();
    }

    // Se a função do usuário terminar, invoca o encerramento automático
    termina_processo();
}


void inicia_fila_prontos(){
    prim = NULL;
    atual = NULL;
    main_ready = 0;
    printf("[NUCLEO] Fila circular iniciada!\n\n");
}

PTR_DESC_PROC proximo_ativo_depois(PTR_DESC_PROC a_partir){
    if(prim == NULL) return NULL;
    if(a_partir == NULL) a_partir = prim;

    PTR_DESC_PROC aux = a_partir->prox_desc;

    while(aux != a_partir){
        if(aux->estado == ATIVO) return aux;
        aux = aux->prox_desc;
    }

    //Se rodou a lista toda, verifica se o inicial já era um ativo, devolvendo ele mesmo
    if(a_partir->estado == ATIVO) return a_partir;

    return NULL;
}

void yield(){
    if(atual == NULL) return;

    PTR_DESC_PROC prox = proximo_ativo_depois(atual);
    if(prox != NULL && atual != prox) {
        PTR_DESC_PROC antigo = atual;
        atual = prox;
        transfer(antigo->contexto, atual->contexto);
    }
}



int cria_processo(void (*tarefa)(void), char* nome){
    PTR_DESC_PROC novo = (PTR_DESC_PROC)malloc(sizeof(BCP));
    if (novo == NULL) {
        printf("ERRO! Falha de memoria ao alocar o BCP de: %s\n", nome);
        return -1;
    }

    /* Inicializa os campos internos do descritor */
    strncpy(novo->nome, nome, 29);
    novo->nome[29] = '\0';
    novo->estado = ATIVO;
    novo->codigo = tarefa;
    novo->fila_sem = NULL;
    novo->contexto = cria_desc();
    if (novo->contexto == NULL) {
        printf("ERRO! Falha ao criar o contexto fisico de: %s\n", nome);
        free(novo);
        return -1;
    }

    //inicializa a fiber
    newprocess(processo_trampolim, novo, novo->contexto);

    /* Encadeamento na Fila Circular */
    if (prim == NULL) {
        prim = novo;
        novo->prox_desc = prim; /* Aponta para si mesmo para fechar o círculo */
    } else {
        PTR_DESC_PROC aux = prim;
        /* Encontra o último elemento atual da fila circular */
        while (aux->prox_desc != prim) {
            aux = aux->prox_desc;
        }
        /* Insere o novo nó no fim e reestabelece o fechamento do círculo */
        aux->prox_desc = novo;
        novo->prox_desc = prim;
    }

    printf("[NUCLEO] Processo %s alocado com sucesso e enviado ao fim da fila!\n", nome);
    return 0;
}


void dispara_sistema(){
    if(prim == NULL) {
        printf("[NUCLEO] ERRO: Nenhum processo na fila para ser disparado.\n");
        return;
    }

    system_init_main(main_ctx);
    main_ready = 1;

    atual = prim->estado == ATIVO ? prim : proximo_ativo_depois(prim);

    if(atual != NULL){
        printf("[NUCLEO] Disparando o Sistema! Passando CPU para o processo: %s\n\n", atual->nome);
        transfer(main_ctx, atual->contexto);
    }else{
        printf("[NUCLEO] ERRO: Nnehum processo ativo encontrado para disparar o sistema.\n");
    }
}


void termina_processo() {

    if (atual == NULL) {
        return;
    }

    printf("[NUCLEO] Processo %s solicitou finalizacao!\n", atual->nome);

    //atualiza o estado do processo
    atual->estado = TERMINADO;

    if (atual->contexto != NULL) {
        free(atual->contexto);
        atual->contexto = NULL;
    }

    /* Procura o próximo processo ativo no sistema */
    PTR_DESC_PROC prox = proximo_ativo_depois(atual);

    if (prox != NULL) {
        PTR_DESC_PROC antigo = atual;
        atual = prox;
        transfer(antigo->contexto, atual->contexto);
    } else {
        /* Se não restarem processos ativos, devolve o controle com segurança para o main() */
        printf("[NUCLEO] Todos os processos finalizaram. Retornando ao controle do Main.\n");
        if (main_ready) {
            transfer(atual->contexto, main_ctx);
        } else {
            fprintf(stderr, "ERRO CRITICO: Contexto base do Main inacessivel.\n");
            exit(1);
        }
    }
}


