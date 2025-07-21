#ifndef ABBFD_H_INCLUDED
#define ABBFD_H_INCLUDED


typedef struct nodoEvento {
    PrestadorABB dato;
    struct nodoEvento *sig;
} nodoEvento;


typedef struct nodo_FD {
    char fecha[11];
    nodoEvento *eventos; // lista de eventos con esa fecha
    int cant_el;
    struct nodo_FD *hi;
    struct nodo_FD *hd;
} nodo_FD;

typedef struct {
    nodo_FD *raiz;
    int Nodos_FD;
}ABB_FD;

nodo_FD* nodo_actual;
nodoEvento* evento_actualA;
nodoEvento* evento_anteriorA = NULL;

void init_ABBFD(ABB_FD *a) {
    a->raiz = NULL;
    a->Nodos_FD = 0;
}

void borrar_lvd(nodoEvento **l) {
    nodoEvento *actual = *l;
    while (actual != NULL) {
        nodoEvento *temp = actual;
        actual = actual->sig;
        free(temp);
    }
    *l = NULL;
}

void borrar_arbolFD(nodo_FD **x) {
    if (*x != NULL) {
        borrar_arbolFD(&(*x)->hi);
        borrar_arbolFD(&(*x)->hd);
        borrar_lvd(&(*x)->eventos);
        free(*x);
        *x = NULL;
    }
}

void inicio_ABBFD(ABB_FD* a, char fecha[11], nodo_FD** padre, float* costo) {
    nodo_FD* aux = a->raiz;
    *padre = NULL;
    *costo = 0;
    while (aux != NULL) {
        int cmp = strcmp(fecha, aux->fecha);
        if (cmp == 0) {
            *costo += 1.0;
            nodo_actual = aux;
            evento_actualA = nodo_actual->eventos;
            return;
        }
        *padre = aux;  // Guardamos el padre antes de avanzar
        if (cmp < 0) {
            aux = aux->hi;
        } else {
            aux = aux->hd;
        }
        *costo += 1.0;
    }
    nodo_actual = NULL;
    evento_actualA = NULL;
}


int hay_mas_ABBFD() {
    return evento_actualA != NULL;
}

PrestadorABB dar_otro_ABBFD(float* costo) {
    PrestadorABB aux = evento_actualA->dato;
    evento_actualA = evento_actualA->sig;
    return aux;
}

int LocalizarABBFD(ABB_FD* a, Prestador p, int* exito, float* costo, nodo_FD** padre) {
    inicio_ABBFD(a, p.fecha, padre, costo);
    if (nodo_actual == NULL) {
        *exito = 0; // fecha no encontrada
        return 0;
    }
    *exito = 0;
    evento_anteriorA = NULL;
    while (hay_mas_ABBFD()) {
        if (evento_actualA->dato.hora == p.hora &&
            strcmp(evento_actualA->dato.lugar, p.lugar) == 0 &&
            strcmp(evento_actualA->dato.evento, p.evento) == 0) {
            *exito = -1;
            return -1;
        }
        evento_anteriorA = evento_actualA;
        dar_otro_ABBFD(costo);
    }
    *exito = 1; // fecha encontrada, evento no
    return 1;
}



int AltaABBFD(ABB_FD *a, Prestador p, int *exito, float *costo) {
    nodo_FD* padre = NULL;
    float c=0;
    int r = LocalizarABBFD(a, p, exito, &c, &padre);
    if (*exito == -1) {
        *exito = 0;
        return -1;
    } // ya existe evento igual
    if (*exito == 1 && nodo_actual != NULL) {
        // Insertar evento en lista vinculada del nodo existente
        nodoEvento *nuevo = malloc(sizeof(nodoEvento));
        if (!nuevo) return *exito = -1;
        strcpy(nuevo->dato.evento,p.evento);
        strcpy(nuevo->dato.lugar,p.lugar);
        nuevo->dato.hora = p.hora;
        nuevo->sig = nodo_actual->eventos;
        nodo_actual->eventos = nuevo;
        nodo_actual->cant_el++;
        (*costo) += 1.0;
        *exito = 1;
        return 1;
    }
    // Crear nodo nuevo para la fecha que no existe
    nodo_FD *nuevoNodo = malloc(sizeof(nodo_FD));
    if (!nuevoNodo) return *exito = -1;
    strncpy(nuevoNodo->fecha, p.fecha, 11);
    nuevoNodo->hi = NULL;
    nuevoNodo->hd = NULL;
    nuevoNodo->cant_el++;
    nodoEvento *evento = malloc(sizeof(nodoEvento));
    if (!evento) {
        free(nuevoNodo);
        return *exito = -1;
    }
    strcpy(evento->dato.evento,p.evento);
    strcpy(evento->dato.lugar,p.lugar);
    evento->dato.hora = p.hora;
    evento->sig = NULL;
    nuevoNodo->eventos = evento;
    if (padre == NULL) {
        // Árbol vacío, insertamos en la raíz
        a->raiz = nuevoNodo;
    } else if (strcmp(p.fecha, padre->fecha) < 0) {
        padre->hi = nuevoNodo;
    } else {
        padre->hd = nuevoNodo;
    }
    a->Nodos_FD++;
    (*costo) += 1.5;
    *exito = 1;
    return 1;
}




int BajaABBFD(ABB_FD *a, Prestador p, int *exito, float *costo) {
    nodo_FD *padre = NULL;
    float c = 0;
    *exito = 0;
    int r = LocalizarABBFD(a, p, exito, &c, &padre);
    if (*exito != -1 || nodo_actual == NULL || evento_actualA == NULL) {
        return 0;
    }
    nodoEvento *aEliminar = evento_actualA;
    if (evento_anteriorA != NULL) {
        evento_anteriorA->sig = aEliminar->sig;
    } else {
        nodo_actual->eventos = aEliminar->sig;
    }
    free(aEliminar);
    nodo_actual->cant_el--;
    *costo += 0.5;
    *exito = 1;
    if (nodo_actual->eventos == NULL) {
        a->Nodos_FD--;
        // Caso 1: hoja
        if (nodo_actual->hi == NULL && nodo_actual->hd == NULL) {
            if (padre == NULL) a->raiz = NULL;
            else if (padre->hi == nodo_actual) padre->hi = NULL;
            else padre->hd = NULL;
            free(nodo_actual);
            *costo += 0.5;
        }
        // Caso 2: solo hijo izquierdo
        else if (nodo_actual->hd == NULL) {
            if (padre == NULL) a->raiz = nodo_actual->hi;
            else if (padre->hi == nodo_actual) padre->hi = nodo_actual->hi;
            else padre->hd = nodo_actual->hi;
            free(nodo_actual);
            *costo += 0.5;
        }
        // Caso 3: solo hijo derecho
        else if (nodo_actual->hi == NULL) {
            if (padre == NULL) a->raiz = nodo_actual->hd;
            else if (padre->hi == nodo_actual) padre->hi = nodo_actual->hd;
            else padre->hd = nodo_actual->hd;
            free(nodo_actual);
            *costo += 0.5;
        }
        // Caso 4: dos hijos
        else {
            nodo_FD *reemp = nodo_actual->hi;
            nodo_FD *padreReemp = nodo_actual;
            while (reemp->hd != NULL) {
                padreReemp = reemp;
                reemp = reemp->hd;
                *costo += 0.5;
            }
            // Copiar datos del reemplazo
            strcpy(nodo_actual->fecha, reemp->fecha);
            borrar_lvd(&nodo_actual->eventos);
            nodo_actual->eventos = reemp->eventos;
            nodo_actual->cant_el = reemp->cant_el;
            if (padreReemp == nodo_actual) padreReemp->hi = reemp->hi;
            else padreReemp->hd = reemp->hi;
            free(reemp);
            *costo += 1.5;
        }
    }

    return *exito;
}


Prestador EvocarPrestadorABBFD(ABB_FD *a, Prestador p, int *exito, float *costo) {
    *exito = 0;
    *costo = 0;
    nodo_FD* *padre=NULL;
    int loc = LocalizarABBFD(a, p, exito, costo,&padre);
    if (*exito == 0) {
        return; // EN ESTE CASO EL RETURN  NO DEVUELVE NADA PORQUE ES UNICAMENTE PARA CALCULO DE COSTOS.
    }
    *exito = 1;
    return;
}

void evocar_eventos_fecha_ABBFD(ABB_FD *a, char fecha[11], float *costo) {
    *costo = 0;
    nodo_FD* p=NULL;
    inicio_ABBFD(a, fecha,&p, costo);
    if (!hay_mas_ABBFD()) {
        printf("No hay eventos en la fecha %s.\n", fecha);
        return;
    }

    printf("Eventos en la fecha %s:\n", fecha);
    while (hay_mas_ABBFD()) {
        PrestadorABB p = dar_otro_ABBFD(costo);
        printf("  [%02dhs] %s en %s\n", p.hora, p.evento, p.lugar);
    }
}

void MostrarABBFD_rec(nodo_FD *r) {
    if (r == NULL) return;

    // Mostrar la fecha del nodo y todos sus eventos
    printf("Fecha: %s (%d eventos)\n", r->fecha, r->cant_el);
    nodoEvento *e = r->eventos;
    while (e != NULL) {
        printf("  [%02dhs] %s en %s\n", e->dato.hora, e->dato.evento, e->dato.lugar);
        e = e->sig;
    }

    // Mostrar hijos solo después de la LVD
    printf("  Hijo izquierdo: %s\n", (r->hi != NULL) ? r->hi->fecha : "NULL");
    printf("  Hijo derecho: %s\n", (r->hd != NULL) ? r->hd->fecha : "NULL");
    printf("----------------------------------\n");

    // Recorrer subárboles
    MostrarABBFD_rec(r->hi);
    MostrarABBFD_rec(r->hd);
}

void MostrarABBFD(ABB_FD *a) {
    printf("MOSTRANDO ARBOL-FD (fecha + eventos + hijos):\n");
    MostrarABBFD_rec(&a->raiz);
}





#endif // ABBFD_H_INCLUDED
