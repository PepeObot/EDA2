#ifndef LSO_FD_H_INCLUDED
#define LSO_FD_H_INCLUDED
#include "Prestador.h"
#include <string.h>
#include <locale.h>
#define MAX1 31

typedef struct nodoEventoLSO {
    PrestadorABB dato;
    struct nodoEventoLSO *sig;
} nodoEventoLSO;

typedef struct {
    char fecha[11];
    nodoEventoLSO *cabecera;
    int cant_elem;
} NodoLSOFD;

typedef struct {
    NodoLSOFD vec[MAX1];
    int cant;
} LSO_FD;

// Estado de recorrido global
int pos_actual;
nodoEventoLSO *evento_actual;
nodoEventoLSO *evento_anterior = NULL;

void init_LSOFD(LSO_FD *lso) {
    lso->cant = 0;
    for (int i = 0; i < MAX1; i++) {
        lso->vec[i].fecha[0] = '\0';
        lso->vec[i].cabecera = NULL;
        lso->vec[i].cant_elem = 0;
    }
}

void inicio_LSOFD(LSO_FD *lso, char fecha[11], float *costo) {
    int li = 0, ls = lso->cant - 1, t, cmp;
    if (lso->cant == 0) {
        pos_actual = 0;
        evento_actual = NULL;
        return;
    }
    t = (li + ls) / 2;
    while (li < ls) {
        cmp = strcmp(lso->vec[t].fecha, fecha);
        (*costo)++;
        if (cmp < 0)
            li = t + 1;
        else
            ls = t;
        t = (li + ls) / 2;
    }
    pos_actual = ls;
    (*costo)++;
    if (strcmp(lso->vec[pos_actual].fecha, fecha) == 0)
        evento_actual = lso->vec[pos_actual].cabecera;
    else {
        if (strcmp(lso->vec[pos_actual].fecha, fecha) < 0)
            pos_actual++;
        if (pos_actual < lso->cant)
            evento_actual = lso->vec[pos_actual].cabecera;
        else
            evento_actual = NULL;
    }
}

int hay_mas_LSOFD(LSO_FD *lso) {
    return (evento_actual != NULL);
}

PrestadorABB dar_otro_LSOFD(LSO_FD *lso, float *costo) {
    PrestadorABB aux = evento_actual->dato;
    evento_actual = evento_actual->sig;
    return aux;
}

int localizar_LSOFD(LSO_FD *lso, Prestador x, int *pos, int *exito, float *costo) {
    inicio_LSOFD(lso, x.fecha, costo);
    *pos = pos_actual;
    nodoEventoLSO *aux = evento_actual;
    evento_anterior = NULL;
    evento_actual = lso->vec[*pos].cabecera;
    while (hay_mas_LSOFD(lso)) {
        PrestadorABB actual = evento_actual->dato;
        if (actual.hora == x.hora &&
            strcmp(actual.lugar, x.lugar) == 0 &&
            strcmp(actual.evento, x.evento) == 0) {
            *exito = -1;
            return -1;
        }
        evento_anterior = evento_actual;
        evento_actual = evento_actual->sig;
    }
    if (*pos < lso->cant && strcmp(x.fecha, lso->vec[*pos].fecha) == 0) {
        *exito = 1;
    } else {
        *exito = 0;
    }
    return *exito;
}

int alta_LSOFD(LSO_FD *lso, Prestador x, int *exito, float *costo) {
    int pos;
    float cp=0;
    if (localizar_LSOFD(lso, x, &pos, exito, &cp) == -1) return -1;
    nodoEventoLSO *nuevo = malloc(sizeof(nodoEventoLSO));
    if (!nuevo) return -1;
    strcpy(nuevo->dato.evento,x.evento);
    strcpy(nuevo->dato.lugar,x.lugar);
    nuevo->dato.hora = x.hora;
    nuevo->sig = NULL;
    if (*exito == 1) {
        nuevo->sig = lso->vec[pos].cabecera;
        lso->vec[pos].cabecera = nuevo;
        lso->vec[pos].cant_elem++;
        *exito = 1;
        return 1;
    }
    for (int i = lso->cant; i > pos; i--) {
        lso->vec[i] = lso->vec[i - 1];
        (*costo)++;
    }
    strncpy(lso->vec[pos].fecha, x.fecha, 11);
    lso->vec[pos].cabecera = nuevo;
    lso->vec[pos].cant_elem = 1;
    lso->cant++;
    *exito = 1;
    return 1;
}

int baja_LSOFD(LSO_FD *lso, Prestador x, int *exito, float *costo) {
    int pos = 0;
    *costo = 0;
    float c=0;
    *exito = 0;
    localizar_LSOFD(lso, x, &pos, exito, &c);
    if (*exito != -1) {
        *exito = 0; // no se encontró el evento exacto
        return 0;
    }
    nodoEventoLSO *aEliminar = evento_actual;
    if (evento_anterior != NULL) {
        evento_anterior->sig = aEliminar->sig;
    } else {
        lso->vec[pos].cabecera = aEliminar->sig;
    }
    free(aEliminar);
    lso->vec[pos].cant_elem--;
    if (lso->vec[pos].cant_elem < 0) {
        lso->vec[pos].cant_elem = 0;
    }
    *exito = 1;
    if (lso->vec[pos].cant_elem == 0) {
        for (int i = pos; i < lso->cant - 1; i++) {
            lso->vec[i] = lso->vec[i + 1];
            (*costo) += 1.0;
        }
        lso->cant--;
    }
    return 1;
}


Prestador* evocar_LSOFD(LSO_FD* lso, Prestador buscado, int* exito, float* costo) {
    int pos;
    float c=0;
    if (localizar_LSOFD(lso, buscado, &pos, exito, &c) == -1) {
        *exito = 1;
        *costo =c;
        return &lso->vec[pos].cabecera->dato;
    }if(*exito==1){
        *costo =c;
        *exito=1;}
    else
        *exito =0;
        // ESTO ES POR LA COMPARACIÓN, ME FALTABA XD.
    *costo =c;
    return NULL;
}

void evocar_todo_LSOFD(LSO_FD* lso, Prestador x, int *exito) {
    float costo = 0;
    inicio_LSOFD(lso, x.fecha, &costo);
    if (evento_actual == NULL || strcmp(lso->vec[pos_actual].fecha, x.fecha) != 0) {
        *exito = 0;
        return;
    }
    *exito = 1;
    printf("Eventos para la fecha %s:\n", x.fecha);
    while (hay_mas_LSOFD(lso)) {
        PrestadorABB actual = dar_otro_LSOFD(lso, &costo);
        printf("  [%02dhs] %s en %s\n", actual.hora, actual.evento, actual.lugar);
    }
}

void mostrar_LSOFD(LSO_FD *lso) {
    printf("MOSTRANDO LSO-FD:\n");
    for (int i = 0; i < lso->cant; i++) {
        printf("Fecha: %s (%d eventos)\n", lso->vec[i].fecha, lso->vec[i].cant_elem);
        nodoEventoLSO *aux = lso->vec[i].cabecera;
        while (aux != NULL) {
            printf("  [%02dhs] %s en %s\n", aux->dato.hora, aux->dato.evento, aux->dato.lugar);
            aux = aux->sig;
        }
    }
}

#endif // LSO_FD_H_INCLUDED
