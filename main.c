//
//  main.c
//  arbol_b_plus
//
//  Created by Oskar Beltran Magaña on 01/12/20.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define d 2

struct node{
    int nss;
    char nombre[26];
    char apellido[51];
    char descripcion[151];
    int edad;
    float peso;
    struct pagina * izquierda;
    struct pagina * derecha;
};

struct registro{
    int nss;
    char nombre[26];
    char apellido[51];
    char descripcion[151];
    int edad;
    float peso;
};

struct elemento_pagina{
    struct node * nodo;
    struct elemento_pagina * siguiente;
};

struct pagina{
    struct elemento_pagina * inicio;
};

typedef struct node nodo;
typedef struct registro registro;
typedef struct elemento_pagina epagina;
typedef struct pagina pagina;

int menu(pagina ** raiz);

//cargar archivo
void iniciar(pagina ** raiz);
int insertar_de_archivo(pagina ** raiz, registro * dato);
int insertar_epagina_de_archivo(epagina ** inicio, registro * dato);
nodo * aloja_de_archivo(registro * dato);

//alojamiento
nodo * aloja_nuevo_nodo(int id);
nodo * aloja_nodo(int id);
epagina * aloja_epagina(void);
pagina * aloja_pagina(void);

//insercion
int insertar(pagina ** raiz, int nss); //inserta nuevo nodo
int insertar_epagina(epagina ** inicio, int nss); //inserta elemento pagina en pagina
int m_pag(epagina * inicio); //estima la magnitud de la pagina
epagina ** obtener_centro(epagina ** inicio, int cont); //consigue el elemento centro en pagina desbordada
int balancear_arbol(epagina ** raiz); //identifica aquellos subarboles que se crean por accidente
int insertar_esta_pag(epagina ** inicio, epagina ** nuevo); //acomoda en la pagina que corresponde
epagina ** creacopia(epagina ** centro); //genera una copia del centro para subir a rama
int sube_ramas(pagina ** raiz); //cuando m < 2d en ramas, sube a raiz
int sube_hojas(pagina ** raiz); //lo mismo que anterior, pero en hojas

//busqueda
epagina ** buscar_pagina(pagina ** raiz, int nss);
epagina ** buscar_epagina(epagina ** inicio, int nss);

//eliminacion
int eliminar(pagina ** raiz, int nss);
int reacomodar(epagina ** inicio);
epagina ** obtener_ultimo(epagina ** inicio);

//impresion
void imprimir_arbol(pagina * raiz, int cont);
void imprimir_rama(epagina * inicio, int cont);

//guardado
void guardar(pagina ** raiz);
int guardar_arbol(pagina * raiz, FILE * archivo);
int guardar_epagina(epagina * inicio, FILE * archivo);

int main(int argc, const char * argv[]) {
    pagina * raiz = NULL;
    iniciar(&raiz);
    while(menu(&raiz) != 5)
        continue;
    return 0;
}

void iniciar(pagina ** raiz)
{
    FILE * archivo;
    registro paciente;
    archivo = fopen("arbol.rks", "rb");
    if(!archivo)
        return;
    fread(&paciente, sizeof(registro), 1, archivo);
    while(!(feof(archivo)))
    {
        insertar_de_archivo(raiz, &paciente);
        fread(&paciente, sizeof(registro), 1, archivo);
        
    }
    fclose(archivo);
}

int menu(pagina ** raiz)
{
    int opt;
    printf("=======================================\n");
    printf("    Bienvenide al sistema del IMSS\n");
    printf("=======================================\n");
    printf("Digite la opcion deseada\n");
    printf("1. Ingresar nuevo paciente\n");
    printf("2. Buscar paciente\n");
    printf("3. Eliminar paciente\n");
    printf("4. Mostrar listado de Px\n");
    printf("5. Salir\n");
    
    printf("Respuesta: ");
    scanf("%i", &opt);
    
    printf("=======================================\n");
    switch(opt)
    {
        case 1:
            printf("Ingresa el Numero de Seguro Social (NSS) del nuevo paciente\n");
            scanf("%i", &opt);
            insertar(raiz, opt);
            return 1;
        case 2:
            printf("Ingrese el NSS del paciente que desea encontrar\n");
            scanf("%i", &opt);
            epagina ** muestra = buscar_pagina(raiz, opt);
            if(!muestra)
                printf("Paciente no encontrado\n");
            else
            {
                printf("%i.\n", (*muestra)->nodo->nss);
                printf("%s, ", (*muestra)->nodo->apellido);
                printf("%s\n", (*muestra)->nodo->nombre);
            }
            return 2;
        case 3:
            printf("Introduzca el Px a eliminar de la base de datos\n");
            scanf("%i", &opt);
            eliminar(raiz, opt);
            return 3;
        case 4:
            imprimir_arbol((*raiz), 0);
            return 4;
        case 5:
            guardar(raiz);
            return 5;
        default:
            printf("Opcion invalida, intenta de nuevo.");
            return -1;
    }
    return 0;
}

int insertar_de_archivo(pagina ** raiz, registro * dato)
{
    if(!(*raiz))
    {
        *raiz = aloja_pagina();
        (*raiz)->inicio = NULL;
        if(!(*raiz))
            return 0;
        if(insertar_epagina_de_archivo(&(*raiz)->inicio, dato) == 0)
       {
           return 0;
       }
        return 1;
    }
    
    insertar_epagina_de_archivo(&(*raiz)->inicio, dato);
    
    if(m_pag((*raiz)->inicio) > 2 * d)
    {
        if(!(*raiz)->inicio->nodo->izquierda || !(*raiz)->inicio->nodo->derecha)
            sube_hojas(raiz);
        else
            sube_ramas(raiz);
    }
    
    balancear_arbol(&(*raiz)->inicio);
    
    return 1;
}

int insertar_epagina_de_archivo(epagina ** inicio, registro * dato)
{
    if(!(*inicio))
    {
        if(!((*inicio) = aloja_epagina()))
            return 0;
        if(!((*inicio)->nodo = aloja_de_archivo(dato)))
            return 0;
        (*inicio)->siguiente = NULL;
        return 1;
    }
    if(dato->nss < (*inicio)->nodo->nss)
    {
        if((*inicio)->nodo->izquierda)
            return insertar_de_archivo(&(*inicio)->nodo->izquierda, dato);
        epagina * nuevo = NULL;
        if(!((nuevo) = aloja_epagina()))
            return 0;
        if(!(nuevo->nodo = aloja_de_archivo(dato)))
            return 0;
        nuevo->siguiente = (*inicio);
        (*inicio) = nuevo;
        return 1;
    }
    if(!(*inicio)->siguiente)
        if((*inicio)->nodo->derecha)
            return insertar_de_archivo(&(*inicio)->nodo->derecha, dato);
    return insertar_epagina_de_archivo(&(*inicio)->siguiente, dato);
}

nodo * aloja_de_archivo(registro * dato)
{
    nodo * tmp;
    if(!(tmp = (nodo *) malloc(sizeof(nodo))))
    {
        printf("Memoria insuficiente");
        return NULL;
    }
    tmp->nss = dato->nss;
    strcpy(tmp->nombre, dato->nombre);
    strcpy(tmp->apellido, dato->apellido);
    tmp->edad = dato->edad;
    tmp->peso = dato->peso;
    strcpy(tmp->descripcion, dato->descripcion);
        
    tmp->izquierda = NULL;
    tmp->derecha = NULL;
    
    return tmp;
}

nodo * aloja_nuevo_nodo(int id) //Se usa para insertar nodo en hoja
{
    nodo * tmp;
    if(!(tmp = (nodo *) malloc(sizeof(nodo))))
    {
        printf("Memoria insuficiente");
        return NULL;
    }
    tmp->nss = id;
    printf("Ingresa el nombre del Px: ");
    scanf("%s", tmp->nombre);
    printf("Ingresa los apellidos del Px: ");
    scanf("%s", tmp->apellido);
    printf("Ingresa la edad: ");
    scanf("%i", &(tmp->edad));
    printf("Ingresa el peso: ");
    scanf("%f", &(tmp->peso));
    printf("Escribe el Dx: ");
    scanf(" %150[^\n]", tmp->descripcion);
        
    tmp->izquierda = NULL;
    tmp->derecha = NULL;
    
    return tmp;
}

nodo * aloja_nodo(int id) //este es para insertar la copia del nodo que va a raiz/rama
{
    nodo * tmp = (nodo *) malloc(sizeof(nodo));
    if(!tmp)
        return NULL;
    tmp->nss = id;
    strcpy(tmp->nombre, "");
    strcpy(tmp->apellido, "");
    strcpy(tmp->descripcion, "");
    tmp->izquierda = NULL;
    tmp->derecha = NULL;
    return tmp;
}

epagina * aloja_epagina(void)
{
    return ((epagina *) malloc(sizeof(epagina)));
}

pagina * aloja_pagina(void)
{
    return ((pagina *) malloc(sizeof(pagina)));
}

int insertar(pagina ** raiz, int nss)
{
    if(!(*raiz))
    {
        *raiz = aloja_pagina();
        (*raiz)->inicio = NULL;
        if(!(*raiz))
            return 0;
        if(insertar_epagina(&(*raiz)->inicio, nss) == 0)
       {
           return 0;
       }
        return 1;
    }
    
    insertar_epagina(&(*raiz)->inicio, nss);
    
    if(m_pag((*raiz)->inicio) > 2 * d)
    {
        if(!(*raiz)->inicio->nodo->izquierda || !(*raiz)->inicio->nodo->derecha)
            sube_hojas(raiz);
        else
            sube_ramas(raiz);
    }
    
    balancear_arbol(&(*raiz)->inicio);
    
    return 1;
}

int insertar_epagina(epagina ** inicio, int nss)
{
    if(!(*inicio))
    {
        if(!((*inicio) = aloja_epagina()))
            return 0;
        if(!((*inicio)->nodo = aloja_nuevo_nodo(nss)))
            return 0;
        (*inicio)->siguiente = NULL;
        return 1;
    }
    if(nss < (*inicio)->nodo->nss)
    {
        if((*inicio)->nodo->izquierda)
            return insertar(&(*inicio)->nodo->izquierda, nss);
        epagina * nuevo = NULL;
        if(!((nuevo) = aloja_epagina()))
            return 0;
        if(!(nuevo->nodo = aloja_nuevo_nodo(nss)))
            return 0;
        nuevo->siguiente = (*inicio);
        (*inicio) = nuevo;
        return 1;
    }
    if(!(*inicio)->siguiente)
        if((*inicio)->nodo->derecha)
            return insertar(&(*inicio)->nodo->derecha, nss);
    return insertar_epagina(&(*inicio)->siguiente, nss);
}

int m_pag(epagina * inicio) //mide la magnitud de las paginas
{
    if(!inicio)
        return 0;
    return 1 + m_pag(inicio->siguiente);
}

epagina ** obtener_centro(epagina ** inicio, int cont)
{
    if(cont == d)
        return inicio;
    else
        return obtener_centro(&(*inicio)->siguiente, cont+1);
}

int balancear_arbol(epagina ** raiz)
{
    if(!(*raiz))
        return 0;
    if((*raiz)->nodo->izquierda)
    {
        if(m_pag((*raiz)->nodo->izquierda->inicio) < d)
        {
            pagina ** tmp = &(*raiz)->nodo->izquierda;
            insertar_esta_pag(raiz, &(*raiz)->nodo->izquierda->inicio);
            (*tmp) = (*raiz)->nodo->derecha;
            return 1;
        }
    }
    
    if((*raiz)->nodo->derecha)
    {
        if(m_pag((*raiz)->nodo->derecha->inicio) < d)
        {
            pagina ** tmp = &(*raiz)->nodo->derecha;
            insertar_esta_pag(raiz, &(*raiz)->nodo->derecha->inicio);
           (*tmp) = (*raiz)->siguiente->nodo->izquierda;
            return 1;
        }
    }
    
    return balancear_arbol(&(*raiz)->siguiente);
}

int insertar_esta_pag(epagina ** inicio, epagina ** nuevo) //acomoda en la pagina al inicio de subarbol solitario
{
    if(!(*inicio))
    {
        (*inicio) = (*nuevo);
        (*nuevo)->siguiente = NULL;
        return 1;
    }
    
    if((*inicio)->nodo->nss > (*nuevo)->nodo->nss)
    {
        (*nuevo)->siguiente = (*inicio);
        (*inicio) = (*nuevo);
        return 1;
    }
    
    return insertar_esta_pag(&(*inicio)->siguiente, nuevo);
}

epagina ** creacopia(epagina ** centro) //crea copia vacia del centro
{
    epagina * tmp = aloja_epagina();
    if(!(tmp))
        return NULL;
    (tmp)->nodo = aloja_nodo((*centro)->nodo->nss);
    if(!(tmp)->nodo)
        return NULL;
    epagina ** copia = NULL;
    copia = &tmp;
    return copia;
}

int sube_ramas(pagina ** raiz)
{
    epagina ** centro = obtener_centro(&(*raiz)->inicio, 0);
    if(!(*centro))
        return 0;
    pagina * izq = aloja_pagina();
    pagina * der = aloja_pagina();
    if(!der || !izq)
        return 0;
    (*centro)->nodo->izquierda = izq;
    (*centro)->nodo->derecha = der;
    izq->inicio = (*raiz)->inicio;
    der->inicio = (*centro);
    (*raiz)->inicio = (*centro);
    (*centro)->siguiente = NULL;
    (*centro) = NULL;
    return 1;
}

int sube_hojas(pagina ** raiz)
{
    epagina ** centro = obtener_centro(&(*raiz)->inicio, 0);
    if(!(*centro))
        return 0;
    pagina * izq = aloja_pagina();
    pagina * der = aloja_pagina();
    if(!der || !izq)
        return 0;
    epagina ** copiac = creacopia(centro);
    if(!(*copiac))
        return 0;
    (*copiac)->nodo->izquierda = izq;
    (*copiac)->nodo->derecha = der;
    izq->inicio = (*raiz)->inicio;
    der->inicio = (*centro);
    (*centro) = NULL;
    (*raiz)->inicio = (*copiac);
    (*copiac)->siguiente = NULL;
    (*copiac) = NULL;
    return 1;
}

epagina ** buscar_pagina(pagina ** raiz, int nss)
{
    if(!(*raiz))
        return NULL;
    return buscar_epagina(&(*raiz)->inicio, nss);
}

epagina ** buscar_epagina(epagina ** inicio, int nss)
{
    if(!(*inicio))
        return NULL;
    if((*inicio)->nodo->nss == nss)
        if(!(*inicio)->nodo->izquierda)
            return inicio;
    if((*inicio)->nodo->nss > nss)
        return buscar_pagina(&(*inicio)->nodo->izquierda, nss);
    if(!(*inicio)->siguiente)
        return buscar_pagina(&(*inicio)->nodo->derecha, nss);
    return buscar_epagina(&(*inicio)->siguiente, nss);
}

int eliminar(pagina ** raiz, int nss)
{
    epagina ** tmp = buscar_pagina(raiz, nss);
    if(!tmp)
    {
        printf("No se encuentra el Px con el NSS %i", nss);
        return 0;
    }
    epagina * adios = (*tmp);
    (*tmp) = (*tmp)->siguiente;
    if(!(adios))
        return 0;
    free(adios->nodo);
    free(adios);
    reacomodar(&(*raiz)->inicio);
    return 1;
}

int reacomodar(epagina ** inicio)
{
    if(!(*inicio))
        return 0;
    if((*inicio)->nodo->izquierda)
        reacomodar(&(*inicio)->nodo->izquierda->inicio);
    if((*inicio)->nodo->derecha)
        reacomodar(&(*inicio)->nodo->derecha->inicio);
    if(!(*inicio)->nodo->izquierda || !(*inicio)->nodo->derecha)
        return 1;
    if(m_pag((*inicio)->nodo->izquierda->inicio) < d || m_pag((*inicio)->nodo->derecha->inicio) < d) {
        epagina ** ultimo_izq = obtener_ultimo(&(*inicio)->nodo->izquierda->inicio);
        if(!(*ultimo_izq))
            return 0;
        (*ultimo_izq)->siguiente = (*inicio)->nodo->derecha->inicio;
        (*inicio)->nodo->derecha->inicio = (*inicio)->nodo->izquierda->inicio;
        (*inicio) = (*inicio)->siguiente;
    }
    return reacomodar(&(*inicio)->siguiente);
}

epagina ** obtener_ultimo(epagina ** inicio)
{
    if(!(*inicio))
        return NULL;
    if(!(*inicio)->siguiente)
        return inicio;
    return obtener_ultimo(&(*inicio)->siguiente);
}

void imprimir_arbol(pagina * raiz, int cont)
{
    if(!raiz)
        return;
    printf("%*s", cont, "Hoja: ");
    imprimir_rama(raiz->inicio, cont);
}

void imprimir_rama(epagina * inicio, int cont)
{
    if(!inicio)
        return;
    printf("├%i. %s, %s\n", inicio->nodo->nss, inicio->nodo->apellido, inicio->nodo->nombre);
    printf("%*c", cont, 32);
    imprimir_arbol(inicio->nodo->izquierda, cont + 1);
    if(!inicio->siguiente)
        imprimir_arbol(inicio->nodo->derecha, cont + 1);
    imprimir_rama(inicio->siguiente, cont);
}

void guardar(pagina ** raiz)
{
    FILE * archivo = fopen("arbol.rks", "wb");
    guardar_arbol(*raiz, archivo);
    fclose(archivo);
}

int guardar_arbol(pagina * raiz, FILE * archivo)
{
    if(!raiz)
        return 0;
    return guardar_epagina(raiz->inicio, archivo);
}

int guardar_epagina(epagina * inicio, FILE * archivo)
{
    if(!inicio)
        return 0;
    if(!inicio->siguiente)
        guardar_arbol(inicio->nodo->derecha, archivo);
    if(!inicio->nodo->izquierda && !inicio->nodo->derecha)
    {
        registro aux;
        aux.nss = inicio->nodo->nss;
        aux.edad = inicio->nodo->edad;
        aux.peso = inicio->nodo->peso;
        strcpy(aux.nombre, inicio->nodo->nombre);
        strcpy(aux.apellido, inicio->nodo->apellido);
        strcpy(aux.descripcion, inicio->nodo->descripcion);
        
        fwrite(&aux, sizeof(registro), 1, archivo);
        return guardar_epagina(inicio->siguiente, archivo);
    }
    guardar_arbol(inicio->nodo->izquierda, archivo);
    return guardar_epagina(inicio->siguiente, archivo);
}
