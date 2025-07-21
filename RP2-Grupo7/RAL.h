#ifndef RAL_H_INCLUDED
#define RAL_H_INCLUDED
#define CANT_BALDES  6
#define CANT_RANURAS 40
#define TAM_RELLENO 96
#define TAM_BALDE (sizeof(RAL) + TAM_RELLENO)
extern FILE *f;
/// TUPLA DE 100 BYTES.
/// RANURAS X BALDE: 40
/// DESPERDICIO: 96 bytes
/// TOTAL BALDES: 6
/// -1 EN DNI = VIRGEN, -2 LIBRE
 typedef struct
 RAL{
    Prestador arr[CANT_RANURAS];
 }RAL;

typedef struct{
    float maximo_alta;
    float medio_alta;
    float maximo_baja;
    float medio_baja;
    float cant_evo_succ;
    float max_evo_succ;
    float med_evo_succ;
    float cant_altas;
    float cant_bajas;
    float maximo_baldeA;
    float medio_baldeA;
    float maximo_baldeB;
    float medio_baldeB;
    float cant_accbal;
    float cant_accbalB;
    float cant_accbalE;
    float maximo_baldeE;
    float medio_baldeE;
    ///////////////////FALLOS ALTA
    float cant_alta_fail;
    float cant_acc_baldeA_fail;
    float maximo_alta_fail;
    float medio_alta_fail;
    float maximo_baldeA_fail;
    float medio_baldeA_fail;
    ///////////////////FALLOS BAJAS
    float cant_baja_fail;
    float cant_acc_baldeB_fail;
    float maximo_baja_fail;
    float medio_baja_fail;
    float maximo_baldeB_fail;
    float medio_baldeB_fail;
    //////////////////FALLOS EVOCAR
    float cant_evo_fail;
    float cant_acc_baldeE_fail;
    float maximo_evo_fail;
    float medio_evo_fail;
    float maximo_baldeE_fail;
    float medio_baldeE_fail;
}v_costo;

int hashing(long dni, int M){
    char x[8];
    int longi, i;
    int contador = 0;
    sprintf(x, "%ld",dni);
    longi = strlen(x);
    for(i=0;i<longi;i++){
        contador+=((int)x[i]*(i+1));
    }
    return (contador % M);
}

void crearArchivo();

void localizar_ral(RAL ral, int dni, int *pos, int *exito, float *costo, int *i_inicial,float *AB) {
    FILE *f = fopen("data.dat", "rb");
    if (!f) {
        perror("ERROR al abrir archivo");
        *exito = -1;
        return;
    }
    int libre = -1;
    int indice = hashing(dni,CANT_BALDES);           // Índice calculado por hashing
    int vueltas = 0;
    int contBaldes = 0;
    int contRanuras = 0;
    int baldeLibre=-1;
    RAL b;
    while (vueltas < CANT_BALDES) {
        *i_inicial = indice;
        contBaldes++;
        *AB = contBaldes;
        fseek(f, indice * TAM_BALDE, SEEK_SET);
        //long offset = indice * sizeof(RAL);
        if (fread(&b, sizeof(RAL), 1, f) != 1) {
            fclose(f);
            perror("ERROR al leer balde");
            *exito = -1;
            return;
        }
        for (int j = 0; j < CANT_RANURAS; j++) {
            contRanuras++;
            if (b.arr[j].dni == dni) {
                fclose(f);
                *pos = j;
                *costo = contRanuras;
                *exito = 1;
                return;
            }
            if (b.arr[j].dni == -2 && libre == -1) {
                libre = j;
                baldeLibre = indice;
            }
            if (b.arr[j].dni == -1) {
                if (libre != -1) {
                    *pos = libre;
                    *i_inicial = baldeLibre;
                } else {
                    *pos = j;
                    *i_inicial = indice;
                }
                *costo = contRanuras;
                *exito = 0;
                fclose(f);
                return;
            }
        }
        *costo = contRanuras;
        indice = (indice + 1) % CANT_BALDES;
        vueltas++;
    }
    fclose(f);
    *exito = -1; // archivo lleno, no se encontró y no hay lugar
}

int alta_ral(RAL *r, Prestador p,int *exito,float *costo,float *AB){
    int pos=0;
    int e=0;
    int i = 0;
    localizar_ral(*r,p.dni,&pos,&e,costo,&i,AB);
        if(e==1){
            e=0; //se encontro al prestador
            fclose(f);
            return e;
        }
        if (e==0){
            f = fopen("data.dat","r+b");
            long offset = i * TAM_BALDE + pos * sizeof(Prestador);
            fseek(f, offset, SEEK_SET);
            fwrite(&p, sizeof(Prestador), 1, f);
            *costo;
            (*AB)++;
            e=1;
            fclose(f);
            return e;
        }
    fclose(f);
}

int baja_ral(RAL *r,Prestador p, int *exito,float *costo, int o, int PS, int H,float *AB){
    int pos=0;
    int e=0;
    int i = 0;
    RAL b;
    Prestador z;
    if(o!=2){
        localizar_ral(*r,p.dni,&pos,&e,costo,&i,AB);
        if(e!=1)
            return e=-1; //no se encontro al prestador
        else{
                f = fopen("data.dat","r+b");
                long offset = i * TAM_BALDE + pos * sizeof(Prestador);
                fseek(f, offset, SEEK_SET);
                if (fread(&z, sizeof(Prestador), 1, f) != 1) {
                    fclose(f);
                    perror("ERROR al leer balde");
                    e = -1;
                    return e;
                }
                if (z.dni == p.dni && o == 1 && z.cantidad == p.cantidad && z.monto==p.monto && strcmpi(z.cdventa,p.cdventa)==0 && strcmpi(z.NyA,p.NyA)==0 && strcmpi(z.tel,p.tel)==0){
                        Prestador l;
                        l.dni = -2;
                        l.monto= 0;
                        l.cantidad=0;
                        l.cdventa[0] = '\0';
                        l.NyA[0] = '\0';
                        l.tel[0] = '\0';
                        fseek(f,offset,SEEK_SET);
                        fwrite(&l,sizeof(Prestador),1,f);
                        fclose(f);
                        *costo = *costo;
                        (*AB)++;
                        return e=1;
                }
                else{
                    fclose(f);
                    return e=-1;
                    }
                fclose(f);
            }
    }else
    {
        f = fopen("data.dat","r+b");
        long offset = H * TAM_BALDE + PS * sizeof(Prestador);
        fseek(f, offset, SEEK_SET);
        if (fread(&z, sizeof(Prestador), 1, f) != 1) {
            fclose(f);
            perror("ERROR al leer balde");
            e = -1;
            return e;
        }
        Prestador l;
        l.dni = -2;
        l.monto= 0;
        l.cantidad=0;
        l.cdventa[0] = '\0';
        l.NyA[0] = '\0';
        l.tel[0] = '\0';
        fseek(f,offset,SEEK_SET);
        fwrite(&l,sizeof(Prestador),1,f);
        fclose(f);
        return e=1;
    }//no coinciden los datos para confirmacion
}

Prestador evocar_ral(RAL r,int dni,int *exito,float *costo,float *AB){
    int pos=0, e=0;
    int i = 0;
    Prestador z,aux;
    float c=0;
    localizar_ral(r,dni,&pos,&e,&c,&i,AB);
    f = fopen("data.dat", "rb");
    if (e==1){
        long offset = i * TAM_BALDE + pos * sizeof(Prestador);
        fseek(f, offset, SEEK_SET);
        if (fread(&z, sizeof(Prestador), 1, f) != 1){
                fclose(f);
                perror("ERROR al leer balde");
                *exito = -1;
                exit(1);
            }
        *costo=c;
        (*AB);
        *exito = e;
        fclose(f);
        aux = z;
        return aux;
    }else{
            *costo = c;
            *exito = e;
            fclose(f);
            return aux;
        }

}

void memPrevia(RAL *r) {
    float costo = 0.0f;
    int exito = 0;
    float AB=0;
    FILE *txt = fopen("Vendedores.txt", "r");
    if (!txt) {
        perror("Error al abrir Vendedores.txt");
        return;
    }
    Prestador x;
    char linea[128];
    int insertados = 0, yaExistian = 0;
    while (1) {
        if (!fgets(linea, sizeof(linea), txt)) break;
        x.dni = atoi(linea);
        if (!fgets(x.NyA, sizeof(x.NyA), txt)) break;
        x.NyA[strcspn(x.NyA, "\r\n")] = '\0';
        if (!fgets(x.tel, sizeof(x.tel), txt)) break;
        x.tel[strcspn(x.tel, "\r\n")] = '\0';
        if (!fgets(linea, sizeof(linea), txt)) break;
        x.monto = atof(linea);
        if (!fgets(linea, sizeof(linea), txt)) break;
        x.cantidad = atoi(linea);
        if (!fgets(x.cdventa, sizeof(x.cdventa), txt)) break;
        x.cdventa[strcspn(x.cdventa, "\r\n")] = '\0';
        int res = alta_ral(r, x, &exito, &costo,&AB);
        if (res) insertados++;
        else yaExistian++;
    }
    fclose(txt);
    system("cls");
    printf("Insertados: %d\n", insertados);
    printf("Ya existían: %d\n", yaExistian);
}

void PO(v_costo *C) {
    // Inicialización de estructura RAL
    RAL *r = malloc(sizeof(RAL));
    int exito = 0;
    float costo = 0;
    float AB=0;
    crearArchivo();

    FILE *ops = fopen("Operaciones.txt", "r");
    if (!ops) {
        perror("Error al abrir Operaciones.txt");
        return;
    }
    char linea[128];
    while (fgets(linea, sizeof(linea), ops)) {
        int codigo = atoi(linea);
        costo = 0;
        exito = 0;
        AB=0;
        switch (codigo) {
            case 1: { // ALTA
                fflush(stdin);
                Prestador x;
                fgets(linea, sizeof(linea), ops);
                x.dni = atoi(linea);
                fgets(x.NyA, sizeof(x.NyA), ops);
                x.NyA[strcspn(x.NyA, "\r\n")] = '\0';
                fgets(x.tel, sizeof(x.tel), ops);
                x.tel[strcspn(x.tel, "\r\n")] = '\0';
                fgets(linea, sizeof(linea), ops);
                x.monto = atof(linea);
                fgets(linea, sizeof(linea), ops);
                x.cantidad = atoi(linea);
                fgets(x.cdventa, sizeof(x.cdventa), ops);
                x.cdventa[strcspn(x.cdventa, "\r\n")] = '\0';
                exito = alta_ral(r, x, &exito, &costo,&AB);
                if (exito==1) {
                    C->cant_altas++;
                    C->medio_alta += costo;
                    if (costo > C->maximo_alta)
                        C->maximo_alta = costo;
                    (*C).cant_accbal++;
                    (*C).medio_baldeA += AB;
                    if (AB > (*C).maximo_baldeA)
                        (*C).maximo_baldeA = AB;
                }else{
                    (*C).cant_alta_fail++;
                    C->medio_alta_fail+=costo;
                    if(costo > C->maximo_alta_fail)
                        C->maximo_alta_fail = costo;
                    (*C).cant_acc_baldeA_fail++;
                    (*C).medio_baldeA_fail += AB;
                    if (AB > (*C).maximo_baldeA_fail)
                        (*C).maximo_baldeA_fail=AB;
                }
                break;
            }
            case 2: { // BAJA
                fflush(stdin);
                Prestador x;
                fgets(linea, sizeof(linea), ops);
                x.dni = atoi(linea);
                fgets(x.NyA, sizeof(x.NyA), ops);
                x.NyA[strcspn(x.NyA, "\r\n")] = '\0';
                fgets(x.tel, sizeof(x.tel), ops);
                x.tel[strcspn(x.tel, "\r\n")] = '\0';
                fgets(linea, sizeof(linea), ops);
                x.monto = atof(linea);
                fgets(linea, sizeof(linea), ops);
                x.cantidad = atoi(linea);
                fgets(x.cdventa, sizeof(x.cdventa), ops);
                x.cdventa[strcspn(x.cdventa, "\r\n")] = '\0';
                exito = baja_ral(r, x, &exito, &costo, 1, 0, 0,&AB);
                if (exito==1) {
                    C->cant_bajas++;
                    C->medio_baja += costo;
                    if (costo > C->maximo_baja)
                        C->maximo_baja = costo;
                    (*C).cant_accbalB++;
                    (*C).medio_baldeB += AB;
                    if (AB > (*C).maximo_baldeB)
                        (*C).maximo_baldeB = AB;
                }else{
                    C->cant_baja_fail++;
                    C->medio_baja_fail+=costo;
                    if(costo > C->maximo_baja_fail)
                        C->maximo_baja_fail = costo;
                    (*C).cant_acc_baldeB_fail++;
                    (*C).medio_baldeB_fail  += AB;
                    if (AB > (*C).maximo_baldeB_fail)
                        (*C).maximo_baldeB_fail=AB;
                }
                break;
            }
            case 3: { // EVOCAR
                fflush(stdin);
                exito = 0;
                fgets(linea, sizeof(linea), ops);
                int dni = atoi(linea);
                evocar_ral(*r, dni, &exito, &costo,&AB);
                 if (exito==1) {
                    C->cant_evo_succ++;
                    C->med_evo_succ += costo;
                    if (costo > C->max_evo_succ)
                        C->max_evo_succ = costo;
                    C->cant_accbalE++;
                    C->medio_baldeE += AB;
                    if (AB > C->maximo_baldeE)
                        C->maximo_baldeE = AB;
                }else{
                    C->cant_evo_fail++;
                    C->medio_evo_fail+=costo;
                    if(costo > C->maximo_evo_fail)
                        C->maximo_evo_fail = costo;
                    C->cant_acc_baldeE_fail++;
                    C->medio_baldeE_fail += AB;
                    if (AB > C->maximo_baldeE_fail)
                        C->maximo_baldeE_fail=AB;
                }
                break;
            }
            default:
                printf("Código de operación no reconocido: %d\n", codigo);
        }
    }
    fclose(ops);
    // Cálculo de promedios al final
    if (C->cant_altas > 0)
        C->medio_alta /= C->cant_altas;
    if (C->cant_alta_fail > 0)
        C->medio_alta_fail /= C->cant_alta_fail;
    if (C->cant_bajas > 0)
        C->medio_baja /= C->cant_bajas;
    if (C->cant_baja_fail > 0)
        C->medio_baja_fail /= C->cant_baja_fail;
    if (C->cant_accbal>0)
        C->medio_baldeA /= C->cant_accbal;
    if (C->cant_acc_baldeA_fail  > 0)
        C->medio_baldeA_fail /= C->cant_acc_baldeA_fail;
    if (C->cant_accbalB>0)
        C->medio_baldeB /= C->cant_accbalB;
    if (C->cant_acc_baldeB_fail> 0)
        C->medio_baldeB_fail /= C->cant_acc_baldeB_fail;
    if (C->cant_evo_succ>0)
        C->med_evo_succ /= C->cant_evo_succ;
    if (C->cant_acc_baldeE_fail > 0)
        C->medio_baldeE_fail /= C->cant_acc_baldeE_fail;
    if (C->cant_evo_fail > 0)
        C->medio_evo_fail /= C->cant_evo_fail;
    if (C->cant_accbalE>0)
        C->medio_baldeE /= C->cant_accbalE;
    fflush(stdin);
    printf("Presione enter...\n");
    fgetchar();
}

void MostrarBalde(int BALDE)
{
    RAL b;
    f = fopen("data.dat","rb");
    if(!f)
    {
        printf("No se pudo abrir el archivo\n");
        return;
    }
    fseek(f,BALDE * (sizeof(RAL)+96),SEEK_SET);
    if (fread(&b,sizeof(RAL),1,f)!=1)
    {
        printf("Error al leer el balde.\n");
        fclose(f);
        return;
    }
    printf("BALDE NUMERO %d\n",BALDE);
    for(int i = 0;i<40;i++)
    {
        if(b.arr[i].dni > -1)
        {
            printf("*\t\t*\n");
            printf("  Estado: OCUPADA\n");
            printf("  DNI: %d\n", b.arr[i].dni);
            printf("  Nombre: %s\n", b.arr[i].NyA);
            printf("  Telefono: %s\n", b.arr[i].tel);
            printf("  Monto: %.2f\n", b.arr[i].monto);
            printf("  Cantidad: %d\n", b.arr[i].cantidad);
            printf("  Canal: %s\n", b.arr[i].cdventa);
        }
    }
    fclose(f);
}

void mostrarRAL() {
    f = fopen("data.dat", "rb");
    if (!f) {
        perror("No se pudo abrir el archivo para mostrar");
        return;
    }
    RAL b;
    for (int i = 0; i < CANT_BALDES; i++) {
        if (fread(&b, sizeof(RAL), 1, f) != 1) {
            perror("Error al leer un balde");
            break;
        }
        printf("            \n--BALDE-- %d\n\n", i);
        for (int j = 0; j < CANT_RANURAS; j++) {
            printf("Ranura %2d:\n", j);
            if (b.arr[j].dni == -1) {
                printf("  Estado: VIRGEN\n");
            } else if (b.arr[j].dni == -2) {
                printf("  Estado: LIBRE\n");
            } else {
                printf("  Estado: OCUPADA\n");
                printf("  DNI: %d\n", b.arr[j].dni);
                printf("  Nombre: %s\n", b.arr[j].NyA);
                printf("  Telefono: %s\n", b.arr[j].tel);
                printf("  Monto: %.2f\n", b.arr[j].monto);
                printf("  Cantidad: %d\n", b.arr[j].cantidad);
                printf("  Canal: %s\n", b.arr[j].cdventa);
            }
        }
        fseek(f, TAM_RELLENO, SEEK_CUR);
    }
    fclose(f);
}



#endif // RAL_H_INCLUDED
