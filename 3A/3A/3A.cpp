// Tecnologico de Costa Rica - Proyecto 3A - Katerine Guzman Flores (2019390523) - Jose Pablo Aguero Mora (2021126372) - Prof: Victor Garro Abarca
/* Compresor y descompresior de archivos usando el Algoritmo de Huffman */

#include <stdio.h>
#include <stdlib.h>
#include "iostream"
#include "string"

using namespace std;

/* Tipo nodo para �rbol o Lista de �rboles*/
/* El prop�sito es dual, sirve como elemento de una lista enlazada */
/* Cuando se usa el puntero sig, y como elemento de un �rbol cuando */
/* se usan los punteros cero y uno */

//////// Structs correspondientes del compresor ////////
typedef struct _nodo
{
    unsigned char letra;    /* Letra a la que hace referencia el nodo */
    int frecuencia;         /* veces que aparece la letra en el texto o las letras */
                            /* de los nodos de las ramas cero y uno */
    struct _nodo* sig;      /* Puntero a siguiente nodo de una lista enlazada */
    struct _nodo* cero;     /* Puntero a la rama cero de un �rbol */
    struct _nodo* uno;      /* Puntero a la rama uno de un �rbol */
} tipoNodo;                /* Nombre de tipo */

/* Nodo para construir una lista para la tabla de codigos */
typedef struct _tabla
{
    unsigned char letra;    /* Letra a la que hace referencia el nodo */
    unsigned long int bits; /* Valor de la codificaci�n de la letra */
    unsigned char nbits;    /* N�mero de bits de la codificaci�n */
    struct _tabla* sig;     /* Siguiente elemento de la tabla */
} tipoTabla;               /* Nombre del tipo */
//////// Fin structs correspondientes del compresor ////////


//////// Structs correspondientes del descompresor ////////
typedef struct _nodo_D
{
    char letra;             /* Letra a la que hace referencia el nodo */
    unsigned long int bits; /* Valor de la codificaci�n de la letra */
    char nbits;             /* N�mero de bits de la codificaci�n */
    _nodo_D* cero;            /* Puntero a la rama cero de un �rbol */
    _nodo_D* uno;             /* Puntero a la rama uno de un �rbol */
} tipoNodo_D;                /* Nombre del tipo */
//////// Fin structs correspondientes del descompresor ////////

/* Variables globales */
tipoTabla* Tabla;

/* Funciones Prototipo */
//////// Para comprimir ////////
void Cuenta(tipoNodo** Lista, unsigned char c);
void Ordenar(tipoNodo** Lista);
void InsertarOrden(tipoNodo** Cabeza, tipoNodo* e);
void BorrarArbol(tipoNodo* n);
void CrearTabla(tipoNodo* n, int l, int v);
void InsertarTabla(unsigned char c, int l, int v);

//////// Para descomprimir //////// 
void BorrarArbol_D(tipoNodo_D* n); // Descomprimir 
tipoTabla* BuscaCaracter(tipoTabla* Tabla, unsigned char c);

/////// Funci�n principal para descomprimir archivos ///////
int Descomprimir(int argc, char* argv[]) // Descomprimir
{
    tipoNodo_D* Arbol;        /* Arbol de codificaci�n */
    long int Longitud;      /* Longitud de fichero */
    int nElementos;         /* Elementos de �rbol */
    unsigned long int bits; /* Almacen de bits para decodificaci�n */
    FILE* fe, * fs;          /* Ficheros de entrada y salida */

    tipoNodo_D* p, * q;        /* Auxiliares */
    unsigned char a;
    int i, j;

    if (argc < 4)
    {
        printf("Usar:\n%s <fichero_entrada> <fichero_salida>\n", argv[0]);
        return 1;
    }

    /* Crear un arbol con la informaci�n de la tabla */
    Arbol = (tipoNodo_D*)malloc(sizeof(tipoNodo_D)); /* un nodo nuevo */
    Arbol->letra = 0;
    Arbol->uno = Arbol->cero = NULL;
    fopen_s(&fe, argv[2], "rb");
    fread(&Longitud, sizeof(long int), 1, fe); /* Lee el n�mero de caracteres */
    fread(&nElementos, sizeof(int), 1, fe); /* Lee el n�mero de elementos */
    for (i = 0; i < nElementos; i++) /* Leer todos los elementos */
    {
        p = (tipoNodo_D*)malloc(sizeof(tipoNodo_D)); /* un nodo nuevo */
        fread(&p->letra, sizeof(char), 1, fe); /* Lee el car�cter */
        fread(&p->bits, sizeof(unsigned long int), 1, fe); /* Lee el c�digo */
        fread(&p->nbits, sizeof(char), 1, fe); /* Lee la longitud */
        p->cero = p->uno = NULL;
        /* Insertar el nodo en su lugar */
        j = 1 << (p->nbits - 1);
        q = Arbol;
        while (j > 1)
        {
            if (p->bits & j) /* es un uno*/
                if (q->uno) q = q->uno;   /* Si el nodo existe, nos movemos a �l */
                else                     /* Si no existe, lo creamos */
                {
                    q->uno = (tipoNodo_D*)malloc(sizeof(tipoNodo_D)); /* un nodo nuevo */
                    q = q->uno;
                    q->letra = 0;
                    q->uno = q->cero = NULL;
                }
            else /* es un cero */
                if (q->cero) q = q->cero; /* Si el nodo existe, nos movemos a �l */
                else                     /* Si no existe, lo creamos */
                {
                    q->cero = (tipoNodo_D*)malloc(sizeof(tipoNodo_D)); /* un nodo nuevo */
                    q = q->cero;
                    q->letra = 0;
                    q->uno = q->cero = NULL;
                }
            j >>= 1;  /* Siguiente bit */
        }
        /* Ultimo Bit */
        if (p->bits & 1) /* es un uno*/
            q->uno = p;
        else            /* es un cero */
            q->cero = p;
    }
    /* Leer datos comprimidos y extraer al fichero de salida */
    bits = 0;
    fopen_s(&fs, argv[3], "w");
    /* Lee los primeros cuatro bytes en la dobel palabra bits */
    fread(&a, sizeof(char), 1, fe);
    bits |= a;
    bits <<= 8;
    fread(&a, sizeof(char), 1, fe);
    bits |= a;
    bits <<= 8;
    fread(&a, sizeof(char), 1, fe);
    bits |= a;
    bits <<= 8;
    fread(&a, sizeof(char), 1, fe);
    bits |= a;
    j = 0; /* Cada 8 bits leemos otro byte */
    q = Arbol;
    /* Bucle */
    do {
        if (bits & 0x80000000) q = q->uno; else q = q->cero; /* Rama adecuada */
        bits <<= 1;           /* Siguiente bit */
        j++;
        if (8 == j)            /* Cada 8 bits */
        {
            i = fread(&a, sizeof(char), 1, fe); /* Leemos un byte desde el fichero */
            bits |= a;                    /* Y lo insertamos en bits */
            j = 0;                        /* No quedan huecos */
        }
        if (!q->uno && !q->cero)          /* Si el nodo es una letra */
        {
            putc(q->letra, fs);           /* La escribimos en el fich de salida */
            Longitud--;                   /* Actualizamos longitud que queda */
            q = Arbol;                      /* Volvemos a la raiz del �rbol */
        }
    } while (Longitud);                  /* Hasta que acabe el fichero */
    /* Procesar la cola */

    fclose(fs);                         /* Cerramos ficheros */
    fclose(fe);

    BorrarArbol_D(Arbol);                 /* Borramos el �rbol */
    return 0;
}

/////// Funci�n principal para comprimir archivos ///////
int Comprimir(int argc, char* argv[]) // Compresion
{
    tipoNodo* Lista;       /* Lista de letras y frecuencias */
    tipoNodo* Arbol;       /* Arbol de letras y frecuencias */

    FILE* fe, * fs;         /* Ficheros de entrada y salida */
    unsigned char c;       /* variables auxiliares */
    tipoNodo* p;
    tipoTabla* t;
    int nElementos;        /* N�mero de elementos en tabla */
    long int Longitud = 0; /* Longitud del fichero original */

    unsigned long int dWORD; /* Soble palabra usada durante la codificaci�n */
    int nBits;               /* N�mero de bits usados */

    if (argc < 4)
    {
        printf("Usar:\n%s <fichero_entrada> <fichero_salida>\n", argv[0]);
        return 1;
    }

    Lista = NULL;
    /* Fase 1: contar frecuencias */
    fopen_s(&fe, argv[2], "r");
    do {
        c = fgetc(fe);
        if (feof(fe)) {
            break;
        }
        Longitud++;        /* Actualiza la cuenta de la longitud del fichero */
        Cuenta(&Lista, c); /* Actualiza la lista de frecuencias */
    } while (1);
    fclose(fe);

    /* Ordenar la lista de menor a mayor */
    Ordenar(&Lista);

    /* Crear el arbol */
    Arbol = Lista;
    while (Arbol && Arbol->sig) /* Mientras existan al menos dos elementos en la lista */
    {
        p = (tipoNodo*)malloc(sizeof(tipoNodo)); /* Un nuevo �rbol */
        p->letra = 0;                             /* No corresponde a ninguna letra */
        p->uno = Arbol;                           /* Rama uno */
        Arbol = Arbol->sig;                       /* Siguiente nodo en */
        p->cero = Arbol;                          /* Rama cero */
        Arbol = Arbol->sig;                       /* Siguiente nodo */
        p->frecuencia = p->uno->frecuencia +
            p->cero->frecuencia;      /* Suma de frecuencias */
        InsertarOrden(&Arbol, p);                 /* Inserta en nuevo nodo */
    }                                            /* orden de frecuencia */

    /* Construir la tabla de c�digos binarios */
    Tabla = NULL;
    CrearTabla(Arbol, 0, 0);

    /* Crear fichero comprimido */
    fopen_s(&fs, argv[3], "wb");
    /* Escribir la longitud del fichero */
    fwrite(&Longitud, sizeof(long int), 1, fs);
    /* Cuenta el n�mero de elementos de tabla */
    nElementos = 0;
    t = Tabla;
    while (t)
    {
        nElementos++;
        t = t->sig;
    }
    /* Escribir el n�mero de elemenos de tabla */
    fwrite(&nElementos, sizeof(int), 1, fs);
    /* Escribir tabla en fichero */
    t = Tabla;
    while (t)
    {
        fwrite(&t->letra, sizeof(char), 1, fs);
        fwrite(&t->bits, sizeof(unsigned long int), 1, fs);
        fwrite(&t->nbits, sizeof(char), 1, fs);
        t = t->sig;
    }

    /* Codificaci�n del fichero de entrada */
    fopen_s(&fe, argv[2], "r");
    dWORD = 0; /* Valor inicial. */
    nBits = 0; /* Ning�n bit */
    do {
        c = fgetc(fe);
        if (feof(fe)) {
            break;
        }
        /* Busca c en tabla: */
        t = BuscaCaracter(Tabla, c);
        /* Si nBits + t->nbits > 32, sacar un byte */
        while (nBits + t->nbits > 32)
        {
            c = dWORD >> (nBits - 8);           /* Extrae los 8 bits de mayor peso */
            fwrite(&c, sizeof(char), 1, fs);  /* Y lo escribe en el fichero */
            nBits -= 8;                       /* Ya tenemos hueco para 8 bits m�s */
        }
        dWORD <<= t->nbits; /* Hacemos sitio para el nuevo caracter */
        dWORD |= t->bits;   /* Insertamos el nuevo caracter */
        nBits += t->nbits;  /* Actualizamos la cuenta de bits */
    } while (1);
    /* Extraer los cuatro bytes que quedan en dWORD*/
    while (nBits > 0)
    {
        if (nBits >= 8) c = dWORD >> (nBits - 8);
        else c = dWORD << (8 - nBits);
        fwrite(&c, sizeof(char), 1, fs);
        nBits -= 8;
    }

    fclose(fe);  /* Cierra los ficheros */
    fclose(fs);

    /* Borrar Arbol */
    BorrarArbol(Arbol);

    /* Borrar Tabla */
    while (Tabla)
    {
        t = Tabla;
        Tabla = t->sig;
        free(t);
    }

    return 0;
}

/* Actualiza la cuenta de frecuencia del car�cter c */
/* El puntero a Lista se pasa por referencia, ya que debe poder cambiar */
/* Ya sea por que la lista est� vac�a, o porque el nuevo elemento sea el */
/* primero */

//////////// Funciones auxiliares para comprimir //////////////
void Cuenta(tipoNodo** Lista, unsigned char c)
{
    tipoNodo* p, * a, * q;

    if (!*Lista)  /* Si la lista est� vac�a, el nuevo nodo ser� Lista */
    {
        *Lista = (tipoNodo*)malloc(sizeof(tipoNodo));   /* Un nodo nuevo */
        (*Lista)->letra = c;                             /* Para c */
        (*Lista)->frecuencia = 1;                        /* en su 1� aparici�n */
        (*Lista)->sig = (*Lista)->cero = (*Lista)->uno = NULL;
    }
    else
    {
        /* Buscar el caracter en la lista (ordenada por letra) */
        p = *Lista;
        a = NULL;
        while (p && p->letra < c)
        {
            a = p;      /* Guardamos el elemento actual para insertar */
            p = p->sig; /* Avanzamos al siguiente */
        }
        /* Dos casos: */
        /* 1) La letra es c se encontr� */
        if (p && p->letra == c) p->frecuencia++; /* Actualizar frecuencia */
        else
            /* 2) La letra c no se encontr�*/
        {
            /* Insertar un elemento nuevo */
            q = (tipoNodo*)malloc(sizeof(tipoNodo));
            q->letra = c;
            q->frecuencia = 1;
            q->cero = q->uno = NULL;
            q->sig = p;        /* Insertar entre los nodos p */
            if (a) a->sig = q;  /* y a */
            else *Lista = q;    /* Si a es NULL el nuevo es el primero */
        }
    }
}

/* Ordena Lista de menor a mayor por frecuencias */
/* De nuevo pasamos el puntero a lista por referencia */
void Ordenar(tipoNodo** Lista)
{
    tipoNodo* Lista2, * a;

    if (!*Lista) return; /* Lista vacia */
    Lista2 = *Lista;
    *Lista = NULL;
    while (Lista2)
    {
        a = Lista2;              /* Toma los elementos de Lista2 */
        Lista2 = a->sig;
        InsertarOrden(Lista, a); /* Y los inserta por orden en Lista */
    }
}

/* Inserta el elemento e en la Lista ordenado por frecuencia de menor a mayor */
/* El puntero a Cabeza se pasa por referencia */
void InsertarOrden(tipoNodo** Cabeza, tipoNodo* e)
{
    tipoNodo* p, * a;

    if (!*Cabeza) /* Si Cabeza es NULL, es el primer elemento */
    {
        *Cabeza = e;
        (*Cabeza)->sig = NULL;
    }
    else
    {
        /* Buscar el caracter en la lista (ordenada por frecuencia) */
        p = *Cabeza;
        a = NULL;
        while (p && p->frecuencia < e->frecuencia)
        {
            a = p;      /* Guardamos el elemento actual para insertar */
            p = p->sig; /* Avanzamos al siguiente */
        }
        /* Insertar el elemento */
        e->sig = p;
        if (a) a->sig = e;   /* Insertar entre a y p */
        else *Cabeza = e;    /* el nuevo es el primero */
    }
}

/* Funci�n recursiva para crear Tabla */
/* Recorre el �rbol cuya raiz es n y le asigna el c�digo v de l bits */
void CrearTabla(tipoNodo* n, int l, int v)
{
    if (n->uno)  CrearTabla(n->uno, l + 1, (v << 1) | 1);
    if (n->cero) CrearTabla(n->cero, l + 1, v << 1);
    if (!n->uno && !n->cero) InsertarTabla(n->letra, l, v);
}

/* Insertar un elemento en la tabla */
void InsertarTabla(unsigned char c, int l, int v)
{
    tipoTabla* t, * p, * a;

    t = (tipoTabla*)malloc(sizeof(tipoTabla)); /* Crea un elemento de tabla */
    t->letra = c;                               /* Y lo inicializa */
    t->bits = v;
    t->nbits = l;

    if (!Tabla)         /* Si tabla es NULL, entonces el elemento t es el 1� */
    {
        Tabla = t;
        Tabla->sig = NULL;
    }
    else
    {
        /* Buscar el caracter en la lista (ordenada por frecuencia) */
        p = Tabla;
        a = NULL;
        while (p && p->letra < t->letra)
        {
            a = p;      /* Guardamos el elemento actual para insertar */
            p = p->sig; /* Avanzamos al siguiente */
        }
        /* Insertar el elemento */
        t->sig = p;
        if (a) a->sig = t;  /* Insertar entre a y p */
        else Tabla = t;    /* el nuevo es el primero */
    }
}

/* Buscar un caracter en la tabla, devielve un puntero al elemento de la tabla */
tipoTabla* BuscaCaracter(tipoTabla* Tabla, unsigned char c)
{
    tipoTabla* t;

    t = Tabla;
    while (t && t->letra != c) t = t->sig;
    return t;
}

/* Funci�n recursiva para borrar un arbol */
void BorrarArbol(tipoNodo* n)
{
    if (n->cero) BorrarArbol(n->cero);
    if (n->uno)  BorrarArbol(n->uno);
    free(n);
}

/* Funci�n recursiva para borrar un arbol / Descomprimir */
void BorrarArbol_D(tipoNodo_D* n)
{
    if (n->cero) BorrarArbol_D(n->cero);
    if (n->uno)  BorrarArbol_D(n->uno);
    free(n);
}

/////////// Funci�n main que decide si se comprime o descomprime el archivo seleccionado /////////////
void main(int argc, char* argv[]) {
    char opcion = argv[1][0];

    if (opcion == 'e') {
        Comprimir(argc, argv);
        printf("Proceso de compresion finalizado");
    }
    else {
        Descomprimir(argc, argv);
        printf("Proceso de descompresion finalizado");
    }

}