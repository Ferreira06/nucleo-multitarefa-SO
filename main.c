#include <stdio.h>
#include <stdlib.h>
#include "nucleo.h"

//Codigo genérico gerado com IA para testar o nucleo criado

// Processo de Teste 1
void processo_a(void) {
    for(int i = 0; i < 5; i++) {
        printf("Processo A executando - Passo %d\n", i);
        yield(); // Pausa o Processo A e passa a CPU para o próximo
    }
}

// Processo de Teste 2
void processo_b(void) {
    for(int i = 0; i < 5; i++) {
        printf("\tProcesso B executando - Passo %d\n", i);
        yield(); // Pausa o Processo B e passa a CPU para o próximo
    }
}

int main(void) {
    printf("--- INICIANDO TESTE DO NUCLEO MULTITAREFAS ---\n\n");

    // 1. Prepara a infraestrutura
    inicia_fila_prontos();

    // 2. Cria os processos na memória
    cria_processo(processo_a, "Tarefa A");
    cria_processo(processo_b, "Tarefa B");

    // 3. Dá a partida no motor cooperativo
    dispara_sistema();

    // 4. O código só chega aqui quando o processo_a e processo_b terminarem
    printf("\n--- TESTE FINALIZADO COM SUCESSO ---\n");

    return 0;
}
