#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>
#include <ctype.h>
#include "Biseccion.h"
#include "Prestador.h"
#include "LSO_FD.h"
#include "ABBFD.h"
//#include "LSOSS.h"
#include "ABBN.h"
#define MAX 774
#define MAXX 31
#define inf 999999999


/*

                            Cantidad De Elementos en LSOBB_SDF: 190
                            Cantidad De Elementos en LSOBB_FD: 30
                            Cantidad De Elementos en ABB: 190
                            Cantidad De Elementos en ABB_FD 30

                                     COMPARACION DE ESTRUCTURAS

                            COSTOS          LSOBB_SDF       ABB             LSO_FD          ABB_FD
                            ___________________________________________________________________________
                            MAX.ALTA        282.000         0.500           23.000          1.500
                            MED.ALTA        70.810          0.500           0.777           1.050
                            MAX.BAJA        287.000         1.500           0.000           0.500
                            MED.BAJA        131.027         0.645           0.000           0.495
                            MAX.EVOEX       25.000          21.000          6.000           8.000
                            MED.EVOEX       18.791          13.967          5.945           4.527
                            MAX.EVONOEX     0.000           0.000           0.000           0.000
                            MED.EVONOEX     0.000           0.000           0.000           0.000




    Acá tenemos los costos de las 4 estructuras. Los arboles, ambos tienen costos constantes muy bajos de inserción y elimnación, gracias a que son estructuras binarias. Las listas secuenciales,
    en especial la lista secuencial sin forzar tiene costos muy altos ya que requiere de muchos corrimientos, pero es mas mantenible que las binarias, la lista forzando dependecia es mas eficiente pero sigue
    mantienendo costos altos al lado del arbol que fuerza dependencia. Las evocaciones no exitosas no hay, ya que siempre evocamos elementos dentro de la estructura.
    En la LSO_FD Nunca damos de baja totalmente, ya que contamos en la baja los corrimientos de la lista, pero este no hace ningún corrimiento. Lo mismo para la media, si nunca damos de baja nos va a devolver 0.
    ABB sin forzar es el peor en las evocaciones, en conjunto con la lista sin forzar, ya que ambas recorren hacia el siguiente elemento uno por uno, el arbol hacia la izquierda, y la lista al siguiente.
    La LSO forzando es muy buena para buscar elementos y sea lo más rápido posible, el ABB forzando es el que tiene un poco de todo, bastante equilibrado, y lo peor de todo la lista, ya que en grandes
    cantidades de elementos sus bajas y altas son muy costosas.
*/

typedef struct{
    float maximo_alta;
    float medio_alta;
    float maximo_baja;
    float medio_baja;
    float max_evo_succ;
    float med_evo_succ;
    float max_evo_fail;
    float med_evo_fail;
    float cant_altas;
    float cant_bajas;
    float cant_evos_succ;
    float cant_evos_fail;
}v_costo;

void init_costo(v_costo *x){
    (*x).maximo_alta=0;
    (*x).medio_alta=0;
    (*x).maximo_baja=0;
    (*x).medio_baja=0;
    (*x).max_evo_succ=0;
    (*x).med_evo_succ=0;
    (*x).max_evo_fail=0;
    (*x).med_evo_fail=0;
    (*x).cant_altas=0;
    (*x).cant_bajas=0;
    (*x).cant_evos_fail=0;
    (*x).cant_evos_succ=0;
}


EstadisticasABB statsSinEvento = {0};
EstadisticasABB statsConEvento = {0};

int main(){
    setlocale(LC_ALL,"");
    int opcion,operacion,opcion_muestra;
    int cant_ss=0,cant_tris=0,cant_abb=0, cant_tris_FD=0, cant_tris_REP=0,cant_abb_fd=0;
    int cant_altas,cant_bajas,cant_evos,cant_rep=0,cant_rep_trs=0;
    int exito_alta=0,exito_baja=0,exito_evo=0,red=0;
    int cant_evo_b = 0;
    int cant_evo_b_fd=0;
    int cant_evo_abbfd;
    int op=0;
    int exxx=0;
    ABB a; int cant_evo_abb = 0;
    ABB_FD b;
    init_ABBFD(&b);
    LSO_FD lis[MAXX];
    init_ABB(&a);
    //RS rs;
    //init_RS(&rs);
    cant_altas=0;cant_bajas=0;cant_evos=0;
    v_costo c_lss;
    v_costo c_ltris;
    v_costo c_abb;
    v_costo c_lfd;
    v_costo c_abbFD;
    init_costo(&c_lfd);
    init_costo(&c_ltris);
    init_costo(&c_abb);
    init_costo(&c_abbFD);
    float costo;
    float cost;
    int pos=0;
    int i=0;
    cant_ss=0;
    cant_tris=0;
    cant_abb=0;
    Prestador aux;
    Prestador lista[MAX+1];
    Prestador lista_tris[MAX];
    init_prestador(lista);
    init_LSOFD(lis);
    do {
        printf("1. Comparar Estructuras\n");
        printf("2. Mostrar Estructuras\n");
        printf("3. Evocación de Estructuras\n");
        printf("4. Salir\n");
        printf("Elige una opcion: ");
        fflush(stdin);
        scanf("%d", &opcion);
        system("cls");

        switch (opcion) {
            case 1:{
                borrar_prestador(lista,&cant_tris);
                borrar_arbolFD(&b.raiz);
                init_ABBFD(&b);
                init_ABB(&a);
                init_LSOFD(lis);
                init_prestador(lista);
                FILE *fp;
                if((fp = fopen("Operaciones-Prestadores.txt","r"))==NULL){
                    printf("\tNo se pudo abrir el archivo\n");
                }
                else{
                    while(!(feof(fp))){
                        op=0;
                        fflush(stdin);
                        fscanf(fp,"%d",&operacion);
                        fgetc(fp);
                        fscanf(fp,"%80[^\n]",aux.fecha);
                        fgetc(fp);
                        switch(operacion){
                            case 1:{ /////////////////////////////////////////////////////////////// CAMBIAR
                                fflush(stdin);
                                fscanf(fp, "%120[^\n]", aux.evento);
                                fgetc(fp);
                                fscanf(fp, "%d", &aux.hora);
                                fgetc(fp);
                                fscanf(fp, "%120[^\n]", aux.lugar);
                                fgetc(fp);
                                costo=0;
                                exito_alta=0;
                                AltaABBFD(&b,aux,&exito_alta,&costo);
                                if(exito_alta==1){
                                    c_abbFD.cant_altas++;
                                    c_abbFD.medio_alta=c_abbFD.medio_alta+costo;
                                    if(costo>c_abbFD.maximo_alta)
                                        c_abbFD.maximo_alta=costo;
                                }
                                costo=0;
                                exito_alta=0;
                                exito_alta=AltaABB(&a,aux.fecha,aux,&costo);
                                if(exito_alta==1){
                                    c_abb.cant_altas++;
                                    c_abb.medio_alta=c_abb.medio_alta+costo;
                                    if(costo>c_abb.maximo_alta)
                                        c_abb.maximo_alta=costo;
                                }
                                exito_alta=0;
                                costo = 0;
                                alta_bise(lista_tris,aux,&exito_alta,&cant_tris,&costo);
                                if(exito_alta==1){
                                    c_ltris.cant_altas =  c_ltris.cant_altas +1;
                                    c_ltris.medio_alta=c_ltris.medio_alta+costo;
                                    if(costo>c_ltris.maximo_alta)
                                        c_ltris.maximo_alta=costo;
                                }
                                cant_altas++;
                                costo = 0;
                                alta_LSOFD(lis,aux,&exito_alta,&costo);
                                if(exito_alta==1){
                                    c_lfd.cant_altas =  c_lfd.cant_altas +1;
                                    c_lfd.medio_alta=c_lfd.medio_alta+costo;
                                    if(costo>c_lfd.maximo_alta)
                                        c_lfd.maximo_alta=costo;
                                }
                                break;
                            }
                            case 2:{/////////////////////////////////////////////////////////////// CAMBIAR
                                fflush(stdin);
                                fscanf(fp, "%120[^\n]", aux.evento);
                                fgetc(fp);
                                fscanf(fp, "%d", &aux.hora);
                                fgetc(fp);
                                fscanf(fp, "%120[^\n]", aux.lugar);
                                fgetc(fp);
                                costo=0;
                                exito_baja=0;
                                costo=0;
                                if(cant_tris>0)
                                    baja_bise(lista_tris,aux,&exito_baja,&cant_tris,&pos,&costo);
                                if(exito_baja==1){
                                    c_ltris.cant_bajas = c_ltris.cant_bajas +1;
                                    c_ltris.medio_baja=c_ltris.medio_baja+costo;
                                    if(costo>c_ltris.maximo_baja)
                                        c_ltris.maximo_baja=costo;
                                }
                                exito_baja=0;
                                costo=0;
                                if(a.raiz != NULL){
                                    exito_baja = baja_ABB(&a,aux.fecha,aux,&costo);
                                }
                                if(exito_baja==1){
                                    c_abb.cant_bajas = c_abb.cant_bajas +1;
                                    c_abb.medio_baja=c_abb.medio_baja+costo;
                                    if(costo>c_abb.maximo_baja)
                                        c_abb.maximo_baja=costo;
                                }
                                costo =0;
                                exito_baja=0;
                                baja_LSOFD(lis,aux,&exito_baja,&costo);
                                if(exito_baja==1){
                                    c_lfd.cant_bajas = c_lfd.cant_bajas +1;
                                    c_lfd.medio_baja=c_lfd.medio_baja+costo;
                                    if(costo>c_lfd.maximo_baja)
                                        c_lfd.maximo_baja=costo;
                                }
                                exito_baja = 0;
                                costo=0;
                                BajaABBFD(&b,aux,&exito_baja,&costo);
                                if(exito_baja==1){
                                    c_abbFD.cant_bajas = c_abbFD.cant_bajas +1;
                                    c_abbFD.medio_baja=c_abbFD.medio_baja+costo;
                                    if(costo>c_abbFD.maximo_baja)
                                        c_abbFD.maximo_baja=costo;
                                }
                                cant_bajas++;
                                break;
                            }//fin case 2
                            case 3:{
                                costo=0;
                                exito_evo=0;
                                evocar_prestador_bise(lista_tris,aux,&exito_evo,&cant_tris,&costo,&cant_evo_b,op);
                                if(exito_evo==1){
                                    c_ltris.cant_evos_succ = c_ltris.cant_evos_succ+1.0;
                                    c_ltris.med_evo_succ=c_ltris.med_evo_succ+costo;
                                    if(costo>c_ltris.max_evo_succ)
                                        c_ltris.max_evo_succ=costo;
                                }else{
                                    c_ltris.cant_evos_fail = c_ltris.cant_evos_fail+1;
                                    c_ltris.med_evo_fail=c_ltris.med_evo_fail+costo;
                                    if(costo>c_ltris.max_evo_fail)
                                        c_ltris.max_evo_fail=costo;
                                }
                                costo =0;
                                exito_evo=0;
                                evocar_prestador_ABB(&a,aux.fecha);
                                if(exito_evo==1){
                                    c_abb.cant_evos_succ = c_abb.cant_evos_succ+1;
                                    c_abb.med_evo_succ=c_abb.med_evo_succ+costo;
                                    if(costo>c_abb.max_evo_succ)
                                        c_abb.max_evo_succ=costo;
                                }else{
                                    c_abb.cant_evos_fail = c_abb.cant_evos_fail+1;
                                    c_abb.med_evo_fail=c_abb.med_evo_fail+costo;
                                    if(costo>c_abb.max_evo_fail)
                                        c_abb.max_evo_fail=costo;
                                }
                                costo=0;
                                evocar_LSOFD(lis,aux,&exito_evo,&costo);
                                if(exito_evo==1){
                                    c_lfd.cant_evos_succ = c_lfd.cant_evos_succ+1;
                                    c_lfd.med_evo_succ=c_lfd.med_evo_succ+costo;
                                    if(costo>c_lfd.max_evo_succ)
                                        c_lfd.max_evo_succ=costo;
                                }else{
                                    c_lfd.cant_evos_fail = c_lfd.cant_evos_fail+1;
                                    c_lfd.med_evo_fail=c_lfd.med_evo_fail+costo;
                                    if(costo>c_lfd.max_evo_fail)
                                        c_lfd.max_evo_fail=costo;
                                }
                                costo =0;
                                EvocarPrestadorABBFD(&b,aux,&exito_evo,&costo);
                                if(exito_evo==1){
                                    c_abbFD.cant_evos_succ = c_abbFD.cant_evos_succ+1;
                                    c_abbFD.med_evo_succ=c_abbFD.med_evo_succ+costo;
                                    if(costo>c_abbFD.max_evo_succ)
                                        c_abbFD.max_evo_succ=costo;
                                }else{
                                    c_abbFD.cant_evos_fail = c_abbFD.cant_evos_fail+1;
                                    c_abbFD.med_evo_fail=c_abbFD.med_evo_fail+costo;
                                    if(costo>c_abbFD.max_evo_fail)
                                        c_abbFD.max_evo_fail=costo;
                                }
                            cant_evos++;
                            break;
                            }//fin case 3 de operaciones

                        }//fin switch operaciones

                    }//fin while
                    fclose(fp);
                                printf("Cantidad De Elementos en LSOBB_SDF: %d\n",cant_tris);
                                printf("Cantidad De Elementos en LSOBB_FD: %d\n",lis->cant);
                                printf("Cantidad De Elementos en ABB: %d\n",a.Nodos);
                                printf("Cantidad De Elementos en ABB_FD %d\n\n",b.Nodos_FD);
                                printf("\t COMPARACION DE ESTRUCTURAS\n\n"
                   "COSTOS\t\tLSOBB_SDF\tABB\t\tLSO_FD\t\tABB_FD\n"
                   "___________________________________________________________________________\n");
            printf("MAX.ALTA\t%.3f\t\t%.3f\t\t%.3f\t\t%.3f\n",c_ltris.maximo_alta,c_abb.maximo_alta,c_lfd.maximo_alta,c_abbFD.maximo_alta);
            printf("MED.ALTA\t%.3f\t\t%.3f\t\t%.3f\t\t%.3f\n",c_ltris.medio_alta/c_ltris.cant_altas,c_abb.medio_alta/c_abb.cant_altas,c_lfd.medio_alta/c_lfd.cant_altas,c_abbFD.medio_alta/c_abbFD.cant_altas);
            printf("MAX.BAJA\t%.3f\t\t%.3f\t\t%.3f\t\t%.3f\n",c_ltris.maximo_baja,c_abb.maximo_baja,c_lfd.maximo_baja,c_abbFD.maximo_baja);
            printf("MED.BAJA\t%.3f\t\t%.3f\t\t%.3f\t\t%.3f\n",(c_ltris.cant_bajas!=0)?c_ltris.medio_baja/c_ltris.cant_bajas:0.0,(c_abb.cant_bajas!=0)?c_abb.medio_baja/c_abb.cant_bajas:0.0,(c_lfd.cant_bajas!=0)?c_lfd.medio_baja/c_lfd.cant_bajas:0.0,(c_abbFD.cant_bajas!=0)?c_abbFD.medio_baja/c_abbFD.cant_bajas:0.0);
            printf("MAX.EVOEX\t%.3f\t\t%.3f\t\t%.3f\t\t%.3f\n",c_ltris.max_evo_succ,statsConEvento.valorMaximo,c_lfd.max_evo_succ,c_abbFD.max_evo_succ);
            printf("MED.EVOEX\t%.3f\t\t%.3f\t\t%.3f\t\t%.3f\n",(c_ltris.cant_evos_succ!=0)?(c_ltris.med_evo_succ)/c_ltris.cant_evos_succ:0.0,(statsConEvento.contadorEventos!=0)?statsConEvento.sumaAcumulada/statsConEvento.contadorEventos:0.0,(c_lfd.cant_evos_succ!=0)?c_lfd.med_evo_succ/c_lfd.cant_evos_succ:0.0,(c_abbFD.cant_evos_succ!=0)?c_abbFD.med_evo_succ/c_abbFD.cant_evos_succ:0.0);
            printf("MAX.EVONOEX\t%.3f\t\t%.3f\t\t%.3f\t\t%.3f\n",c_ltris.max_evo_fail,statsSinEvento.valorMaximo,c_lfd.max_evo_fail,c_abbFD.max_evo_fail);
            printf("MED.EVONOEX\t%.3f\t\t%.3f\t\t%.3f\t\t%.3f\n",(c_ltris.cant_evos_fail!=0)?c_ltris.med_evo_fail/c_ltris.cant_evos_fail:0.0,(statsSinEvento.contadorEventos!=0)?statsSinEvento.sumaAcumulada/statsSinEvento.contadorEventos:0.0,(c_lfd.cant_evos_fail!=0)?c_lfd.med_evo_fail/c_lfd.cant_evos_fail:0.0,(c_abbFD.cant_evos_fail!=0)?c_abbFD.med_evo_fail/c_abbFD.cant_evos_fail:0.0);
                     }//fin else de lectura de archivo exitosa
                break;
            }//fin case 1 opciones menu
            case 2:{
                do{
                    printf("1. Mostrar LSOBB_SDF\n");
                    printf("2. Mostrar Arbol Binario de Busqueda\n");
                    printf("3. Mostrar LSOBB_DF\n");
                    printf("4. Mostrar ABB_DF\n");
                    printf("5. Volver al Menu\n");
                    printf("Elige una opcion: ");
                    fflush(stdin);
                    scanf("%d", &opcion_muestra);
                    switch(opcion_muestra){
                        case 1:{
                            if(cant_tris>0)
                                muestra_bise(lista_tris,cant_tris);
                            else
                                printf("\tLa lista esta vacia\n");
                            break;
                        }
                        case 2:{
                            if(a.Nodos>0)
                                muestra_abb(a.raiz);
                            else
                                printf("\tEl arbol esta vacio\n");
                            break;
                        }case 3:{
                            if(lis->cant>0)
                                mostrar_LSOFD(lis);
                            else
                                printf("\tLa LSO_FD esta vacio\n");
                            break;
                        }
                        case 4:{
                            MostrarABBFD(b.raiz);
                        }
                        case 5:{
                            printf("Saliendo...\n");
                            break;
                        }
                        default:
                            printf("Ingrese una opcion valida\n");
                            break;
                    }//fin switch opcion muestra

                }while (opcion_muestra!=5);


            break;
            }//fin case 2 menu opciones
            case 3:
            {
                do{
                    printf("1. Evocación en el ABB\n");
                    printf("2. Evocación en la LSOBB\n");
                    printf("3. Evocación en la LSOBB_FD\n");
                    printf("4. Evocación en la ABB_FD\n");
                    printf("5. Salir\n");
                    printf("Elige una opcion: ");
                    fflush(stdin);
                    scanf("%d", &opcion_muestra);
                    switch(opcion_muestra){
                        case 1:{
                            printf("Ingrese fecha: ");
                            init_prestador(&aux);
                            op = 4;
                            scanf("%s", aux.fecha);
                            evocacionABB(&a,aux.fecha);
                            if(exito_evo == 0){
                                printf("No se encuentra el evento dentro de la lista.\n");
                            }
                            break;
                        }
                        case 2:{
                            op = 4;
                            printf("Ingrese fecha: ");
                            init_prestador(&aux);
                            scanf("%s", aux.fecha);
                            evocar_prestador_biseTODO(lista_tris,aux,&exito_evo,&cant_tris);
                            cant_evo_b--;
                            if(exito_evo == 0){
                                printf("No se encuentra el evento dentro de la lista.\n");
                            }
                            break;
                        }
                        case 3:{
                            op = 4;
                            printf("Ingrese fecha: ");
                            init_prestador(&aux);
                            scanf("%s", aux.fecha);
                            evocar_todo_LSOFD(lis,aux,&exito_evo);
                            if(exito_evo == 0){
                                printf("No se encuentra el evento dentro de la lista.\n");
                            }
                            break;
                        }
                        case 4:{
                            op = 4;
                            printf("Ingrese fecha: ");
                            init_prestador(&aux);
                            scanf("%s", aux.fecha);
                            evocar_eventos_fecha_ABBFD(&b,aux.fecha,&costo);
                            if(exito_evo == 0){
                                printf("No se encuentra el evento dentro de la lista.\n");
                            }
                            break;
                        }
                        case 5:{
                            printf("Saliendo...\n");
                            break;
                        }
                        default:
                            printf("Ingrese una opcion valida\n");
                            break;
                    }//fin switch opcion muestra
                }while(opcion_muestra != 5);
            }
            case 5:
                printf("Saliendo...\n");
                break;
            default:
                printf("Opci�n no v�lida\n");
                break;
        }//fin switch menu principal
    } while (opcion != 4);

    return 0;
}// FIN MAIN

void evocar_prestador_ABB(ABB *arbol, char fechaBuscar[]){
    NodoABB *evAux;
    int a = 0;
    inicioABB(arbol);
    while (hayMasABB(arbol, fechaBuscar)){
        a++;
        evAux = deme_otroABB(arbol);
    }
    if (a == 0){
        statsSinEvento.sumaAcumulada += statsSinEvento.valorActual;
        if (statsSinEvento.valorActual > statsSinEvento.valorMaximo){
            statsSinEvento.valorMaximo = statsSinEvento.valorActual;
        }

        statsSinEvento.contadorEventos++;
    }
    else{
        if (statsConEvento.valorActual > statsConEvento.valorMaximo){
            statsConEvento.valorMaximo = statsConEvento.valorActual;
        }
        statsConEvento.contadorEventos++;
        statsConEvento.sumaAcumulada += statsConEvento.valorActual;
    }
}
