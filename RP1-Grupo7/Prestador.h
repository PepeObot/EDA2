#ifndef PRESTADOR_H_INCLUDED
#define PRESTADOR_H_INCLUDED
//include setlocale
typedef struct{
    int hora;
    char fecha[11];
    char evento[120];
    char lugar[120];
}Prestador;

typedef struct{
    int hora;
    char evento[120];
    char lugar[120];
}PrestadorABB;

void init_prestador(Prestador lista[]){
    lista[0].hora=0;
    strcpy(lista[0].fecha,"");
    strcpy(lista[0].evento,"");
    strcpy(lista[0].lugar,"");
}

void borrar_prestador(Prestador lista[], int *cant){
    for(int i= 0;i<*cant;i++)
    {
        lista[i].hora=0;
        strcpy(lista[i].fecha,"");
        strcpy(lista[i].evento,"");
        strcpy(lista[i].lugar,"");
    }
    *cant =0;
}
#endif // PRESTADOR_H_INCLUDED
