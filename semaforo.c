#include <stdio.h>
#include <stdlib.h>
#include "semaforo.h"
#include "nucleo.h"


void inicia_semaforo(semaforo *sem, int n){
    if(!sem) return;
    sem->s = n;
    sem->Q = NULL;

}


void V(semaforo *sem){
    if(!sem) return;

    PTR_DESC_PROC p;

    //Se nao havia ninguem na lista de bloqueados, aumenta o contador
    if(sem->Q == NULL){
        sem->s++;

    }else{ //Caso contrario, libera o valor que estava bloqueado
        p = sem->Q;
        sem->Q = p->fila_sem;
        p->fila_sem = NULL;

        p->estado = ATIVO;
    }



}
void P(semaforo *sem){
    if(!sem) return;
    if(!atual) {
        printf("Nenhum processo atual.\n");
        exit(1);
    }

    //Se existe espaco disponivel no contador do semaforo, usa-o
    if(sem->s > 0){
        sem->s--;
        return;
    }

    //Bloqueia o processo atual
    atual->estado = BLOQ_P;

    //Se nao havia espaco livre, vou adicionar esse processo no fim da fila de processos do semaforo que estao aguardando
    atual->fila_sem = NULL;
    PTR_DESC_PROC aux = NULL;
    if(!sem->Q){
        sem->Q = atual;
    }else{
        aux = sem->Q;
        while(aux->fila_sem != NULL) aux = aux->fila_sem;
        aux->fila_sem = atual;
    }

    //Procura qual o proximo processo ativo, para lidar com o caso de deadlock
    PTR_DESC_PROC prox = NULL;
    if(prim){
        //Percorre a lista circular, buscando pelo proximo valor ativo
        prox = proximo_ativo_depois(atual);
    }

    if(!prox){
        printf("Deadlock: Todos os processos estao bloqueados\n");
        exit(1);
    }

    yield();


}
