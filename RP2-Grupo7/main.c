#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Prestador.h"
#include "RAL.h"
/*

                 ALTA
ACCESOS MAXIMOS A BALDE: 2.000
ACCESOS MEDIO A BALDE: 2.000
ALTAS REALIZADAS: 201.000
COSTO MAX ALTA: 33.000
COSTO MEDIO DE ALTA: 16.045

FRACASO
ACCESO M├üXIMO FRUSTRADO A DISCO: 1.000
ACCESO MEDIO FRUSTRADO A DISCO: 1.000
ALTAS FRUSTRADAS: 5.000
COSTO MAX ALTAS FRUSTRADA: 2.000
COSTO MEDIO DE ALTAS FRUSTRADA: 1.200

                 BAJA
ACCESOS MAXIMOS A BALDE: 2.000
ACCESOS MEDIO A BALDE: 2.000
BAJAS REALIZADAS: 120.000
COSTO MAX BAJA: 32.000
COSTO MEDIO DE BAJA: 11.467

FRACASO
ACCESO M├üXIMO FRUSTRADO A DISCO: 1.000
ACCESO MEDIO FRUSTRADO A DISCO: 1.000
BAJAS FRUSTRADAS: 5.000
COSTO MAX BAJA FRUSTRADA: 33.000
COSTO MEDIO DE BAJA FRUSTRADA: 21.000

                 EVOCAR
ACCESOS MAXIMOS A BALDE: 1.000
ACCESOS MEDIO A BALDE: 1.000
EVOCAR REALIZADOS: 297.000
COSTO MAX EVOCAR: 32.000
COSTO MEDIO DE EVOCAR: 9.017

FRACASO
ACCESO M├üXIMO FRUSTRADO A DISCO: 1.000
ACCESO MEDIO FRUSTRADO A DISCO: 1.000
EVOCAR FRUSTRADOS: 79.000
COSTO MAX EVO FRUSTRADA: 33.000
COSTO MEDIO DE EVO FRUSTRADA: 19.342

Análisis: He aquí los costos finales, Los accesos a baldes tanto en altas como Bajas exitosas son 2, ya que ingresamos una vez a disco y luego una vez más para modificar.
En cambio en el evocar sólo 1, ya que ingresamos una única vez y devolvemos su posición, no precisamos modificar ni insertar nada, sólo retornar el elemento.
    Cuando hablamos de consulta de baldes en el fracaso, tenemos que el costo es menor, ya que no tiene que saltar a otro balde en el archivo, buscamos y se da el fracaso en ese mismo balde
por eso en las 3 operaciones tenemos que es de máximo 1 y medio 1.
    Los costos máximos de consulta de ranuras en Alta/Baja/Evocar exitosas son muy parecidos entre sí, ya que en el Alta precisa de llegar hasta una celda virgen;
en Baja/Evocar necesitan llegar hasta el elemento.
    Sus medios exitosos varían más, ya que en Alta precisamos encontrar una virgen siempre y se hacen muchas inserciones, siempre solemos ir hasta el final del balde o su último insertado , eso nos genera unos costos
medios mucho más grandes que la baja y evocar; la Baja/Evocar nuevamente comparte costos muy parecidos, pero la baja es más costosa ya que necesitamos verificar que todos sus campos sean iguales,
el evocar sólo con encontrar el dni nos devolverá el elemento.
    Cuando vemos los fracasos, tenemos que son muy variados, para el Alta es muy bajo, ya que con encontrar el elemento tenemos el fracaso.
En la Baja/Evocar nuevamente vemos que son iguales en los máximos, ya que tienen en el peor caso, ir hasta el último elemento insertado y luego la virgen para fallar;
sus medios son parecidos, pero son bastantes más altos que sus medios exitosos, porque por lo general se falla en ranuras más profundas del archivo, lo que genera buscar más.

*/
FILE *f;

void init_costo(v_costo *x){
    (*x).maximo_alta=0;
    (*x).medio_alta=0;
    (*x).maximo_baja=0;
    (*x).medio_baja=0;
    (*x).max_evo_succ=0;
    (*x).med_evo_succ=0;
    (*x).cant_altas=0;
    (*x).cant_bajas=0;
    (*x).medio_baldeA=0;
    (*x).maximo_baldeA=0;
    (*x).medio_baldeB=0;
    (*x).maximo_baldeB=0;
    (*x).cant_accbal=0;
    (*x).cant_accbalB=0;
    (*x).maximo_baldeE=0;
    (*x).medio_baldeE=0;
    (*x).cant_accbalE=0;
    (*x).cant_evo_succ=0;
    (*x).maximo_alta_fail=0;
    (*x).medio_alta_fail=0;
    (*x).maximo_baldeA_fail=0;
    (*x).medio_baldeA_fail=0;
    (*x).maximo_baja_fail=0;
    (*x).medio_baja_fail=0;
    (*x).maximo_baldeB_fail=0;
    (*x).medio_baldeB_fail=0;
    (*x).maximo_evo_fail=0;
    (*x).medio_evo_fail=0;
    (*x).maximo_baldeE_fail=0;
    (*x).medio_baldeE_fail=0;
    (*x).cant_alta_fail=0;
    (*x).cant_baja_fail=0;
    (*x).cant_evo_fail=0;
    (*x).cant_acc_baldeA_fail=0;
    (*x).cant_acc_baldeB_fail=0;
    (*x).cant_acc_baldeE_fail=0;
}

void crearArchivo() {
    f = fopen("data.dat", "wb");
    if (!f) {
        perror("Error al crear archivo");
        return;
    }
    RAL b;
    Prestador virgen;
    virgen.dni = -1;
    virgen.monto = 0;
    virgen.cantidad = 0;
    virgen.cdventa[0] = '\0';
    virgen.NyA[0] = '\0';
    virgen.tel[0] = '\0';

    for (int i = 0; i < CANT_RANURAS; i++)
        b.arr[i] = virgen;

    char relleno[TAM_RELLENO] = {0};

    for (int i = 0; i < CANT_BALDES; i++) {
        fwrite(&b, sizeof(RAL), 1, f);
        fwrite(relleno, sizeof(char), 96, f); // Esto asegura que TAM_BALDE sea exacto
    }

    fclose(f);
}

int main()
{
    v_costo C;
    init_costo(&C);
    int op=0,e=0,pos=0,h,b,o;
    float costo=0;
    float AB=0;
    Prestador y;
    RAL r;
    f = fopen("data.dat", "rb");
    if (f==NULL)
        crearArchivo();
    while(op!=8)
    {
        system("cls");
        printf("\n");
        printf("             MENU                \n");
        printf("\n");
        printf("1. Ingresar un nuevo vendedor\n");
        printf("2. Eliminar un vendedor\n");
        printf("3. Consultar un vendedor\n");
        printf("4. Mostrar balde\n");
        printf("5. Mostrar estructura\n");
        printf("6. Memorizacion previa\n");
        printf("7. Procesar Operaciones y ver costos\n");
        printf("8. Salir\n");
        printf("\n");
        printf("Ingrese una opcion: ");
        scanf("%d",&op);
        switch(op)
        {
            case 1:
                {
                    system("cls");
                    Prestador x;
                    printf("Ingrese DNI: \n");
                    scanf("%d",&x.dni);
                    getchar();
                    printf("Ingrese nombre y apellido: \n");
                    scanf("%[^\n]",x.NyA);
                    printf("Ingrese numero de telefono: \n");
                    scanf("%s",x.tel);
                    printf("Ingrese monto vendido: \n");
                    scanf("%f",&x.monto);
                    printf("Ingrese cantidad: \n");
                    scanf("%d",&x.cantidad);
                    printf("Ingrese canal de venta: \n");
                    scanf("%s",x.cdventa);
                    int result = alta_ral(&r,x,&e,&costo,&AB);
                    if (result !=1){
                        printf("Elemento repetido. Alta no exitosa\n");
                        fflush(stdin);
                        getchar();
                    }else printf("Alta exitosa");
                    break;
                }
            case 2:
                {
                    system("cls");
                    Prestador x;
                    printf("Ingrese DNI: \n");
                    scanf("%d",&x.dni);
                    h = hashing(x.dni,CANT_BALDES);
                    y = evocar_ral(r,x.dni,&e,&costo,&AB);
                    if(e)
                    {
                        system("cls");
                        printf("------------------------------\n");
                        printf(" DNI: %d\n", y.dni);
                        printf(" Nombre: %s\n", y.NyA);
                        printf(" Telefono: %s\n", y.tel);
                        printf(" Monto: %.2f\n", y.monto);
                        printf(" Cantidad: %d\n", y.cantidad);
                        printf(" Canal: %s\n", y.cdventa);
                        printf("------------------------------\n");
                        printf("Confirmar baja\n(0) NO\n(1) SI \n ");
                        scanf("%d",&b);
                        if(b==1)
                        {
                            o = 2;
                            baja_ral(&r,x,&e,&costo,o,pos,h,&AB);
                            if(e)
                            {
                                printf("Baja efectiva.\n");
                            }
                            else
                            {
                                printf("No se puedo realizar la baja.\n");
                            }
                        }
                        else
                        {
                            printf("No se realizó la baja");
                        }
                    }else
                         printf("No se encontró el vendedor.\n");
                         printf("Presione espacio para continuar\n");
                         fflush(stdin);
                         getchar();
                    break;
                }
            case 3:
                {
                    Prestador x;
                    printf("Ingrese DNI para consultar vendedor: \n");
                    scanf("%d",&x.dni);
                    y = evocar_ral(r,x.dni,&e,&costo,&AB);
                    if(e)
                    {
                        system("cls");
                        printf("  DNI: %d\n", y.dni);
                        printf("  Nombre: %s\n", y.NyA);
                        printf("  Telefono: %s\n", y.tel);
                        printf("  Monto: %.2f\n", y.monto);
                        printf("  Cantidad: %d\n", y.cantidad);
                        printf("  Canal: %s\n", y.cdventa);
                        getchar();
                    }
                    else printf("No se encontro el vendedor\n");
                        fflush(stdin);
                        printf("Presione enter...\n");getchar();
                    break;
                }
            case 4:
                {
                    printf("Ingrese el DNI del balde a buscar: \n");
                    scanf("%d",&b);
                    int h = 0;
                    localizar_ral(r,b,&pos,&e,&costo,&h,&AB);
                    if(e)
                        MostrarBalde(h);
                    else
                        printf("No se encontró\n");
                    fflush(stdin);
                    printf("Presione enter...\n");getchar();
                    break;
                }
            case 5:
                {
                    mostrarRAL();
                    fflush(stdin);
                    printf("Presione enter...\n");getchar();
                    break;
                }
            case 6:
                {
                    memPrevia(&r);
                    fflush(stdin);
                    printf("Presione enter...\n");getchar();
                    break;
                }
            case 7:
                {
                    PO(&C);
                    system("cls");
                    printf("\t\t ALTA\n");
                    printf("ACCESOS MAXIMOS A BALDE: %.3f\n",C.maximo_baldeA);
                    printf("ACCESOS MEDIO A BALDE: %.3f\n",C.medio_baldeA);
                    printf("ALTAS REALIZADAS: %.3f\n",C.cant_altas);
                    printf("COSTO MAX ALTA: %.3f\n",C.maximo_alta);
                    printf("COSTO MEDIO DE ALTA: %.3f\n",C.medio_alta);
                    //
                    printf("\nFRACASO\n");
                    printf("ACCESO MÁXIMO FRUSTRADO A DISCO: %.3f\n",C.maximo_baldeA_fail);
                    printf("ACCESO MEDIO FRUSTRADO A DISCO: %.3f\n",C.medio_baldeA_fail);
                    printf("ALTAS FRUSTRADAS: %.3f\n", C.cant_alta_fail);
                    printf("COSTO MAX ALTAS FRUSTRADA: %.3f\n",C.maximo_alta_fail);
                    printf("COSTO MEDIO DE ALTAS FRUSTRADA: %.3f\n",C.medio_alta_fail);
                    //
                    printf("\n\t\t BAJA\n");
                    printf("ACCESOS MAXIMOS A BALDE: %.3f\n",C.maximo_baldeB);
                    printf("ACCESOS MEDIO A BALDE: %.3f\n",C.medio_baldeB);
                    printf("BAJAS REALIZADAS: %.3f\n",C.cant_bajas);
                    printf("COSTO MAX BAJA: %.3f\n",C.maximo_baja);
                    printf("COSTO MEDIO DE BAJA: %.3f\n",C.medio_baja);
                    //
                    printf("\nFRACASO\n");
                    printf("ACCESO MÁXIMO FRUSTRADO A DISCO: %.3f\n",C.maximo_baldeB_fail);
                    printf("ACCESO MEDIO FRUSTRADO A DISCO: %.3f\n",C.medio_baldeB_fail);
                    printf("BAJAS FRUSTRADAS: %.3f\n", C.cant_baja_fail);
                    printf("COSTO MAX BAJA FRUSTRADA: %.3f\n",C.maximo_baja_fail);
                    printf("COSTO MEDIO DE BAJA FRUSTRADA: %.3f\n",C.medio_baja_fail);
                    //
                    printf("\n\t\t EVOCAR\n");
                    printf("ACCESOS MAXIMOS A BALDE: %.3f\n",C.maximo_baldeE);
                    printf("ACCESOS MEDIO A BALDE: %.3f\n",C.medio_baldeE);
                    printf("EVOCAR REALIZADOS: %.3f\n",C.cant_evo_succ);
                    printf("COSTO MAX EVOCAR: %.3f\n",C.max_evo_succ);
                    printf("COSTO MEDIO DE EVOCAR: %.3f\n",C.med_evo_succ);
                    //
                    printf("\nFRACASO\n");
                    printf("ACCESO MÁXIMO FRUSTRADO A DISCO: %.3f\n",C.maximo_baldeE_fail);
                    printf("ACCESO MEDIO FRUSTRADO A DISCO: %.3f\n",C.medio_baldeE_fail);
                    printf("EVOCAR FRUSTRADOS: %.3f\n", C.cant_evo_fail);
                    printf("COSTO MAX EVO FRUSTRADA: %.3f\n",C.maximo_evo_fail);
                    printf("COSTO MEDIO DE EVO FRUSTRADA: %.3f\n",C.medio_evo_fail);
                    fflush(stdin);
                    printf("Presione enter...\n");getchar();
                    break;
                }

        }
    }
    return 0;
}
