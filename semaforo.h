#ifndef SEMAFORO_H_INCLUDED
#define SEMAFORO_H_INCLUDED
#include "nucleo.h"

typedef struct {
    int s;
    PTR_DESC_PROC Q;

}semaforo;

void inicia_semaforo(semaforo *sem, int n);
void P(semaforo *sem);
void V(semaforo *sem);



#endif // SEMAFORO_H_INCLUDED
