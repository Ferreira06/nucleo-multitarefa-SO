#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nucleo.h"
#include "semaforo.h"

#define MAX_PROCESSOS 10


int buffer[MAX_PROCESSOS];

int in = 0;
int out = 0;

semaforo mutex;
semaforo cheio;
semaforo vazio;

void depositar(int item){
    buffer[in] = item;
    printf("[PRODUTOR] - Escrevendo a mensagem %d, no indice: %d\n", buffer[in], in);
    in = (in+1) % MAX_PROCESSOS;
}

void retirar(){
    printf("\t[CONSUMIDOR] - Lendo a mensagem %d, no indice: %d\n", buffer[out], out);
    buffer[out] = 0;
    out = (out+1) % MAX_PROCESSOS;
}

void Produtor(){
    int limite = 1; //Limite opcional, apenas para demonstração da funcionalidade

    while(limite < 20){
        P(&vazio);
        P(&mutex);
        depositar(limite);
        V(&mutex);
        V(&cheio);

        limite++;
    }
        yield();
}

void Consumidor(){
    int limite = 1; //Limite opcional, apenas para demonstração da funcionalidade

    while(limite < 20){
        P(&cheio);
        P(&mutex);
        retirar();
        V(&mutex);
        V(&vazio);

        limite++;
    }
        yield();
}


int main(void) {
    printf("--- INICIANDO TESTE DO NUCLEO MULTITAREFAS ---\n\n");

    inicia_fila_prontos();
    inicia_semaforo(&vazio, MAX_PROCESSOS);
    inicia_semaforo(&cheio, 0);
    inicia_semaforo(&mutex, 1);

    cria_processo(Produtor, "Produtor");
    cria_processo(Consumidor, "Consumidor");


    dispara_sistema();

    // 4. O código só chega aqui quando o processo_a e processo_b terminarem
    printf("\n--- TESTE FINALIZADO COM SUCESSO ---\n");

    return 0;
}
