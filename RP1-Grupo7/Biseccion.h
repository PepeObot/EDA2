#ifndef BISECCION_H_INCLUDED
#define BISECCION_H_INCLUDED
#define MAX 774
#include "Prestador.h"
#include "LSO_FD.h"
#include <string.h>
#include <locale.h>


int hay_mas(Prestador lista[], char fechaBuscada[], int pos, int total, float *costo) {
    if (pos < total){
        (*costo)++;
        if (strcmp(lista[pos].fecha, fechaBuscada) == 0)
            return 1;
    }
    return 0;
}

Prestador dar_otro(Prestador lista[], int *pos) {
    Prestador ev = lista[*pos];
    (*pos)++;
    return ev;
}

int inicio(Prestador lista[], char fechaBuscada[], int total, float *costo) {
    int inf = 0;
    int sup = total - 1;
    int medio;
    if (total == 0) {
        return 0;
    }
    while (inf < sup) {
        medio = (inf + sup) / 2;
        *costo = *costo +1.0;
        if (strcmp(lista[medio].fecha, fechaBuscada) < 0) {
            inf = medio + 1;
        } else {
            sup = medio;
        }
    }
    *costo = *costo +1.0;
    if (strcmp(lista[inf].fecha, fechaBuscada) < 0) {
        return inf + 1;
    }
    return inf;
}

int localizar_bise(Prestador lista[], Prestador buscado, int total, int *pos, float *costo) {
    *pos = inicio(lista, buscado.fecha, total, costo);
    int entra=0;
    while (hay_mas(lista, buscado.fecha, *pos, total, costo)==1) {
        entra=1;
        Prestador temp = dar_otro(lista, pos);
        if (temp.hora == buscado.hora) {
            (*pos)--; // volver a la posición del duplicado
            return -1; // ya existe
        }
    }
    if (entra==0)
        return -2;
        else return entra;// no existe, se puede insertar
}


void alta_bise(Prestador lista[], Prestador x, int *exito, int *cant, float *costo) {
    int i = 0;
    int pos = 0;
    int e=0;
    if ((*cant + 1) > MAX) {
        *exito = -1;
    } else {
        e=localizar_bise(lista, x, *cant,&pos,costo);
        if(e!=-1){
            i = (*cant);
            *costo = 0;
            for (i; pos < i; i--) {
                *costo=*costo+1.0;
                lista[i] = lista[i - 1];
            }
            lista[i] = x;
            *exito = 1;
            (*cant)++;
            *exito =1;
        }
    }
}

void baja_bise(Prestador lista[], Prestador aux, int *exito, int *cant, int *pos, float *costo) {
    int op;
    int i = *cant;
    int p=0;
    int e = localizar_bise(lista, aux, *cant,&p,costo);
    if (e==-1) {
        *exito =0;
        if (0 == strcmpi(lista[p].lugar, aux.lugar) &&
            aux.hora == lista[p].hora &&
            0 == strcmpi(lista[p].evento, aux.evento)) {
            *costo = 0;
            for (i = p; i < *cant; i++) {
                lista[i] = lista[i + 1];
                *costo=*costo+1.0;
            }
            (*costo)--;
            (*cant)--;
            *exito = 1;
            return;
        }
    }
}

int evocar_prestador_bise(Prestador lista[],Prestador x, int *exito,int *cant,float *costo,int *ceb,int op){
    Prestador aux;
    int e=0;
    int ex=0; //flag
    float c=0;
    int pos = inicio(lista, x.fecha, *cant, &c);
    while(hay_mas(lista,x.fecha,pos,*cant,&c)){
        ex++;
        e = 1;
        aux = dar_otro(lista,&pos);
    }
    *costo = c;
    if(ex==0){
        return *exito =-1;
    }
    *exito = e;
    return *exito;
}

Prestador evocar_prestador_biseTODO(Prestador lista[], Prestador x, int *exito, int *cant){
    int total = *cant;
    float c=0;
    int pos = inicio(lista, x.fecha, total, &c);
    int encontrados = 0;

    while (hay_mas(lista, x.fecha, pos, total, &c)) {
        Prestador p = dar_otro(lista, &pos);
        printf("Evento: %s\nLugar: %s\nHora: %d\nFecha: %s\n",
               p.evento, p.lugar, p.hora, p.fecha);
        printf("-------------------------------\n");
        encontrados++;
    }

    if (encontrados == 0) {
        printf("No se encontraron eventos para la fecha: %s\n", x.fecha);
    }
}

void muestra_bise(Prestador lista[], int cant){
    setlocale(LC_ALL,"");
    int i = 0;
    int exito;
    while(i < cant){
        printf("%d\n",i);
        printf("Fecha: %s \n", lista[i].fecha);
        printf("Hora: %d \n", lista[i].hora);
        printf("Evento : %s \n", lista[i].evento);
        printf("Lugar: %s \n", lista[i].lugar);
        printf("******************************************\n");
        i++;
        if(i%5 == 0){
            fflush(stdin);
            printf("Presione ENTER para la página siguiente...\n");
            getchar();
        }
    }
}

#endif // BISECCION_H_INCLUDED
