#ifndef ABBN_H_INCLUDED
#define ABBN_H_INCLUDED
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include "Prestador.h"

typedef struct NodoABB{
    char fecha[11]; // formato: AAAA-MM-DD
    Prestador arb;  // guardamos el Prestador aquí
    struct NodoABB *izq, *der;
} NodoABB;

typedef struct ABB{
    NodoABB *cur;
    NodoABB *raiz;
    NodoABB *ant;
    int Nodos;
} ABB;


typedef struct {
    float valorActual;
    float valorMaximo;
    int contadorEventos;
    float sumaAcumulada;
} EstadisticasABB;

extern EstadisticasABB statsConEvento;
extern EstadisticasABB statsSinEvento;

NodoABB *crearnodoABB(char fecha[], Prestador arb){
    NodoABB *nodo_nuevo = (NodoABB *)malloc(sizeof(NodoABB));
    if (nodo_nuevo == NULL)
        return NULL;
    strcpy(nodo_nuevo->fecha, fecha);
    nodo_nuevo->arb = arb;
    nodo_nuevo->izq = NULL;
    nodo_nuevo->der = NULL;
    return nodo_nuevo;
}

void inicioABB(ABB *arbol){
    arbol->cur = arbol->raiz;
    arbol->ant = NULL;
    statsConEvento.valorActual=0;
    statsSinEvento.valorActual=0;
}

void init_ABB(ABB *a) {
    a->raiz = NULL;
    a->ant = NULL;
    a->cur = NULL;
    a->Nodos = 0;
}

int hayMasABB(ABB *arbol, char fechaBuscada[]){
    while (arbol->cur != NULL){
        int cmp = strcmp(fechaBuscada, arbol->cur->fecha);
        statsConEvento.valorActual++; // EVOCACIÓN EXITOSA
        statsSinEvento.valorActual++; // EVOCACION FRACASO
        if (cmp == 0){
            return 1; // encontrado fecha
        }
        arbol->ant = arbol->cur;
        if (cmp < 0){
            arbol->cur = arbol->cur->izq;
        } else {
            arbol->cur = arbol->cur->der;
        }
    }
    return 0; // no encontrado
}

NodoABB *deme_otroABB(ABB *arbol){
    NodoABB *ev = arbol->cur;
    arbol->ant = arbol->cur;
    arbol->cur = arbol->cur->izq; // baja al hijo izq (lista enlazada en izq)
    return ev;
}

int LocalizarABB(ABB *arbol, char fecha[], Prestador ev){
    inicioABB(arbol);
    while (hayMasABB(arbol, fecha)){
        if (arbol->cur->arb.hora == ev.hora){
            return 1; // encontrado evento exacto
        }
        deme_otroABB(arbol);
    }
    return 0; // no encontrado
}

int AltaABB(ABB *arbol, char fecha[], Prestador ev, float *costo){
    NodoABB *nodo = crearnodoABB(fecha, ev);
    if (nodo == NULL)
        return 0; // malloc falló
    if (arbol->raiz == NULL){
        arbol->raiz = nodo;
        *costo += 0.5;
        arbol->Nodos = 1;
        return 1;
    }
    if (LocalizarABB(arbol, fecha, ev) == 1){
        free(nodo);
        return 0; // evento ya existe
    }
    if (strcmp(arbol->ant->fecha, fecha) >= 0){
        arbol->ant->izq = nodo;
    } else {
        arbol->ant->der = nodo;
    }
    arbol->Nodos++;
    *costo += 0.5;
    return 1;
}

int baja_ABB(ABB *arbol, char fecha[], Prestador ev, float *costo){
    int res = LocalizarABB(arbol, fecha, ev);
    if (res == 0) return 0;
    NodoABB *nodoAEliminar = arbol->cur;
    if (strcmp(nodoAEliminar->arb.evento, ev.evento) != 0 ||
        strcmp(nodoAEliminar->arb.lugar, ev.lugar) != 0 ||
        nodoAEliminar->arb.hora != ev.hora){
        return 0;
    }
    if (nodoAEliminar->izq == NULL && nodoAEliminar->der == NULL){
        if (nodoAEliminar == arbol->raiz){
            arbol->raiz = NULL;
        } else if (arbol->ant->der == nodoAEliminar){
            arbol->ant->der = NULL;
        } else {
            arbol->ant->izq = NULL;
        }
        free(nodoAEliminar);
        *costo += 0.5;
    }
    // Caso 2: solo hijo derecho
    else if (nodoAEliminar->izq == NULL){
        if (nodoAEliminar == arbol->raiz){
            arbol->raiz = nodoAEliminar->der;
        } else if (arbol->ant->der == nodoAEliminar){
            arbol->ant->der = nodoAEliminar->der;
        } else {
            arbol->ant->izq = nodoAEliminar->der;
        }
        free(nodoAEliminar);
        *costo += 0.5;
    }
    // Caso 3: solo hijo izquierdo
    else if (nodoAEliminar->der == NULL){
        if (nodoAEliminar == arbol->raiz){
            arbol->raiz = nodoAEliminar->izq;
        } else if (arbol->ant->der == nodoAEliminar){
            arbol->ant->der = nodoAEliminar->izq;
        } else {
            arbol->ant->izq = nodoAEliminar->izq;
        }
        free(nodoAEliminar);
        *costo += 0.5;
    }
    // Caso 4: dos hijos
    else {
        NodoABB *actual = nodoAEliminar->izq;
        NodoABB *padre = nodoAEliminar;
        while (actual->der != NULL){
            padre = actual;
            actual = actual->der;
        }
        strcpy(nodoAEliminar->fecha, actual->fecha);
        nodoAEliminar->arb = actual->arb;
        if (padre->der == actual){
            padre->der = actual->izq;
        } else {
            padre->izq = actual->izq;
        }
        free(actual);
        *costo += 1.5; // costo por reemplazo
    }
    arbol->Nodos--;
    return 1;
}

void evocacionABB(ABB *arbol, char fechaBuscar[]){
    Prestador evAux;
    int a = 0;
    inicioABB(arbol);
    while (hayMasABB(arbol, fechaBuscar)){
        evAux = deme_otroABB(arbol)->arb;
        a++;
        printf("-------------------------------------\n");
        // Aquí usá tu función para mostrar Prestador
        // Ejemplo:
        printf("Fecha: %s\nHora: %d\nLugar: %s\nEvento: %s\n", fechaBuscar, evAux.hora, evAux.lugar, evAux.evento);
        printf("######################################\n");
    }
    if (a == 0){
        printf("No hay eventos asociados a la fecha: %s\n", fechaBuscar);
    }
}

void muestra_abb(NodoABB *nodo){
    if (nodo != NULL){
        printf("Fecha %s:\n", nodo->fecha);
        // Mostrar datos de Prestador
        printf("Hora: %d\nLugar: %s\nEvento: %s\n", nodo->arb.hora, nodo->arb.lugar, nodo->arb.evento);

        if (nodo->izq != NULL){
            printf("  Hijo izquierdo: %s\n", nodo->izq->fecha);
        }
        if (nodo->der != NULL){
            printf("  Hijo derecho: %s\n", nodo->der->fecha);
        }
        printf("-------------------------------------------------\n");
        muestra_abb(nodo->izq);
        muestra_abb(nodo->der);
    }
}

#endif // ABBN_H_INCLUDED
