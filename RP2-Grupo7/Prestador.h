#ifndef PRESTADOR_H_INCLUDED
#define PRESTADOR_H_INCLUDED
#include <string.h>
//include setlocale
typedef struct{
    long dni;
    char NyA[51];
    char tel[16];
    float monto;
    int cantidad;
    char cdventa[21];
}Prestador;


void init_prestador(Prestador lista[]){
    lista[0].dni=0;
    strcpy(lista[0].NyA,"");
    strcpy(lista[0].tel,"");
    strcpy(lista[0].cdventa,"");
}
#endif // PRESTADOR_H_INCLUDED
