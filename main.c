/*
 * Sistema de Venta de Pasajes para un Vuelo
 *
 * Este programa implementa un sistema básico de gestión de
 * pasajeros para un vuelo con capacidad máxima de 20 asientos.
 *
 * Permite realizar operaciones de ABM (Alta, Baja y Modificación)
 * sobre una estructura de datos que almacena información de cada
 * pasajero: DNI, nombre, destino, número de asiento y estado
 * (activo/inactivo).
 *
 * Funcionalidades principales:
 * - Alta: Registrar un pasajero si hay asientos disponibles y libres.
 * - Baja lógica: Cancelar un pasaje marcando el pasajero como inactivo.
 * - Modificación: Cambiar destino o asiento validando disponibilidad.
 * - Listado de pasajeros activos: Solo los que viajan.
 * - Listado por destino: Filtrar pasajeros por ciudad destino.
 * - Listado general: Mostrar pasajeros activos e inactivos.
 *
 * Autor: Miguel Pereyra
 */

//------------------------------
// Directivas de pre procesador
//------------------------------
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "funciones.h"

#define MAX_PASAJEROS 100
#define MAX_ASIENTOS 20
#define MIN_dni 1000000
#define MAX_dni 99999999
#define MAX_NOMBRE_PASAJERO 50
#define MAX_DESTINO 30

enum opciones_menu_principal
{
    SALIR = 0,
    ALTA_PASAJERO = 1,
    BAJA_PASAJERO,
    MODIFICAR_PASAJERO,
    LISTAR_PASAJEROS_ACTIVOS,
    LISTAR_PASAJEROS_POR_DESTINO,
    LISTAR_TODOS_LOS_PASAJEROS
};

enum modificacion_campos
{
    VOLVER_MENU_PRINCIPAL = 0,
    MODIFICAR_DESTINO = 1,
    MODIFICAR_ASIENTO
};

typedef struct
{
    int dni;
    char nombre[MAX_NOMBRE_PASAJERO];
    char destino[MAX_DESTINO];
    int asiento;
    int activo; // 1 = pasajero confirmado, 0 = canceló el vuelo
} Pasajero;

//------------------------------
// prototipos de funciones
//------------------------------
void listar_todos_los_pasajeros(Pasajero pasajeros[], int cantidad_pasajeros);
void listar_pasajeros_por_destino(Pasajero pasajeros[], int cantidad_pasajeros);
void listar_pasajeros_activos(Pasajero pasajeros[], int cantidad_pasajeros);
void listar(Pasajero pasajeros[], int indice, int nro_orden);
void modificar_datos(Pasajero pasajeros[], int asientos[], int cantidad_pasajeros);
void baja_pasajero(Pasajero pasajeros[], int asientos[], int *cantidad_pasajeros);
void alta_pasajero(Pasajero pasajeros[], int asientos[], int *cantidad_pasajeros);
int seleccionar_asiento(int asientos[]);
void tabulacion();
void mostrar_asientos(int asientos[]);
bool hay_asiento_disponible(int asientos[]);
void menu_cambiar_datos();
void menu_principal();
void iniciar_arreglos(Pasajero pasajeros[], int asientos[]);
int buscar_dni_binario(Pasajero pasajeros[], int cantidad_pasajeros, int dni_buscado);
void insertar_pasajero_ordenado(Pasajero pasajeros[], int *cantidad_pasajeros, Pasajero nuevo_pasajero);
bool confirmar_baja();
void cargar_pasajeros_desde_archivo(Pasajero pasajeros[], int *cantidad_pasajeros, const char *nombre_archivo, int asientos[]);
void guardar_pasajeros_en_archivo(Pasajero pasajeros[], int cantidad_pasajeros, const char *nombre_archivo);

//------------------
// funcion principal
//------------------
int main()
{
    Pasajero pasajeros[MAX_PASAJEROS];
    int asientos[MAX_ASIENTOS];
    int cantidad_pasajeros = 0;
    iniciar_arreglos(pasajeros, asientos);
    // cargar datos de archivo
    cargar_pasajeros_desde_archivo(pasajeros, &cantidad_pasajeros, "pasajeros.csv", asientos);
    int opcion;
    do
    {
        menu_principal();
        opcion = pedir_entero("\nIngrese una opcion: ");
        switch (opcion)
        {
        case ALTA_PASAJERO:
            alta_pasajero(pasajeros, asientos, &cantidad_pasajeros);
            break;
        case BAJA_PASAJERO:
            baja_pasajero(pasajeros, asientos, &cantidad_pasajeros);
            break;
        case MODIFICAR_PASAJERO:
            modificar_datos(pasajeros, asientos, cantidad_pasajeros);
            break;
        case LISTAR_PASAJEROS_ACTIVOS:
            listar_pasajeros_activos(pasajeros, cantidad_pasajeros);
            break;
        case LISTAR_PASAJEROS_POR_DESTINO:
            listar_pasajeros_por_destino(pasajeros, cantidad_pasajeros);
            break;
        case LISTAR_TODOS_LOS_PASAJEROS:
            listar_todos_los_pasajeros(pasajeros, cantidad_pasajeros);
            break;
        case SALIR:
            // guardar datos en archivo
            guardar_pasajeros_en_archivo(pasajeros, cantidad_pasajeros, "pasajeros.csv");
            puts("Saliendo del sistema de venta de pasajes. ¡Hasta luego!");
            break;
        default:
            printf("Opcion invalida. Por favor, intente de nuevo.\n");
            break;
        }
    } while (opcion != SALIR);
    return 0;
}

//------------------------
// Definicion de funciones
//------------------------
void cargar_pasajeros_desde_archivo(Pasajero pasajeros[], int *cantidad_pasajeros, const char *nombre_archivo, int asientos[])
{
    FILE *archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL)
    {
        printf("No se pudo abrir el archivo %s. Se iniciara con datos vacios.\n", nombre_archivo);
        return;
    }
    char linea[150];
    fgets(linea, sizeof(linea), archivo); // descartar cabecera
    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        Pasajero pasajero;
        if (sscanf(linea, "%d,%49[^,],%29[^,],%d,%d",
                   &pasajero.dni,
                   pasajero.nombre,
                   pasajero.destino,
                   &pasajero.asiento,
                   &pasajero.activo) == 5)
        {
            insertar_pasajero_ordenado(pasajeros, cantidad_pasajeros, pasajero);
            // Marca asiento si el pasajero está activo
            if (pasajero.activo == 1 &&
                pasajero.asiento >= 1 && pasajero.asiento <= MAX_ASIENTOS)
            {
                asientos[pasajero.asiento - 1] = 1;
            }
            if (*cantidad_pasajeros >= MAX_PASAJEROS)
            {
                printf("Se alcanzo la capacidad maxima de pasajeros al cargar desde el archivo.\n");
                break;
            }
        }
    }
    fclose(archivo);
    printf("Datos de pasajeros cargados desde el archivo %s exitosamente.\n", nombre_archivo);
}


void guardar_pasajeros_en_archivo(Pasajero pasajeros[], int cantidad_pasajeros, const char *nombre_archivo)
{
    FILE *archivo = fopen(nombre_archivo, "w");
    if (archivo == NULL)
    {
        printf("Error al abrir el archivo %s para escritura.\n", nombre_archivo);
        return;
    }
    fprintf(archivo, "DNI,Nombre,Destino,Asiento,Estado\n");
    for (int i = 0; i < cantidad_pasajeros; i++)
    {
        fprintf(archivo, "%d,%s,%s,%d,%d\n",
                pasajeros[i].dni,
                pasajeros[i].nombre,
                pasajeros[i].destino,
                pasajeros[i].asiento,
                pasajeros[i].activo);
    }
    fclose(archivo);
    printf("Datos de pasajeros guardados en el archivo %s exitosamente.\n", nombre_archivo);
}

void insertar_pasajero_ordenado(Pasajero pasajeros[], int *cantidad_pasajeros, Pasajero nuevo_pasajero)
{
    int i = *cantidad_pasajeros - 1;
    while (i >= 0 && pasajeros[i].dni > nuevo_pasajero.dni) // desplazamiento hacia la derecha
    {
        pasajeros[i + 1] = pasajeros[i];
        i--;
    }
    pasajeros[i + 1] = nuevo_pasajero; // inserta en la posicion correcta
    (*cantidad_pasajeros)++;           // incrementa el contador de pasajeros
}

int buscar_dni_binario(Pasajero pasajeros[], int cantidad_pasajeros, int dni_buscado)
{
    int inicio = 0;
    int fin = cantidad_pasajeros - 1;
    while (inicio <= fin)
    {
        int medio = (inicio + fin) / 2;

        if (pasajeros[medio].dni == dni_buscado)
        {
            return medio; // encontrado
        }
        else if (dni_buscado < pasajeros[medio].dni) // busca en la mitad izquierda
        {
            fin = medio - 1;
        }
        else // busca en la mitad derecha
        {
            inicio = medio + 1;
        }
    }
    return -1; // no encontrado
}

void listar_todos_los_pasajeros(Pasajero pasajeros[], int cantidad_pasajeros)
{
    int nro_orden = 1;
    printf("\n*** Lista de Todos los Pasajeros ***\n");
    tabulacion();
    if (cantidad_pasajeros == 0)
    {
        printf("No hay pasajeros registrados.\n");
        return;
    }
    for (int i = 0; i < cantidad_pasajeros; i++)
    {
        if (pasajeros[i].activo != -1) // solo los ocupados (activos o inactivos)
            listar(pasajeros, i, nro_orden++);
    }
}
void listar_pasajeros_por_destino(Pasajero pasajeros[], int cantidad_pasajeros)
{
    char destino_buscar[MAX_DESTINO];
    int nro_orden = 1;
    pedir_texto("Ingrese el destino a buscar: ", destino_buscar, MAX_DESTINO);
    printf("\n*** Lista de Pasajeros con destino %s ***\n", destino_buscar);
    tabulacion();
    bool hay_pasajeros = false;
    for (int i = 0; i < cantidad_pasajeros; i++)
    {
        if (pasajeros[i].activo == 1 && strcmp(pasajeros[i].destino, destino_buscar) == 0)
        {
            listar(pasajeros, i, nro_orden++);
            hay_pasajeros = true;
        }
    }
    if (!hay_pasajeros)
    {
        printf("No hay pasajeros activos registrados con destino %s.\n", destino_buscar);
    }
}
void listar_pasajeros_activos(Pasajero pasajeros[], int cantidad_pasajeros)
{
    printf("\n*** Lista de Pasajeros Activos ***\n");
    tabulacion();
    bool hay_activos = false;
    int nro_orden = 1;
    for (int i = 0; i < cantidad_pasajeros; i++)
    {
        if (pasajeros[i].activo == 1)
        {
            listar(pasajeros, i, nro_orden++);
            hay_activos = true;
        }
    }
    if (!hay_activos)
    {
        printf("No hay pasajeros activos registrados.\n");
    }
}

void tabulacion()
{
    printf("--------------------------------------------------------------------------------------------\n");
    printf("%-2s | %-10s | %-30s | %-15s | %-8s | %s\n",
           "Nro", "DNI", "Nombre", "Destino", "Asiento", "Estado    |");
    printf("%-2s-+-%-10s-+-%-30s-+-%-15s-+-%-8s-+-%s\n",
           "---", "----------", "------------------------------", "---------------", "--------", "----------");
}

void listar(Pasajero pasajeros[], int indice, int nro_orden)
{
    printf("%2d  | %-10d | %-30s | %-15s | %-8d | %s\n",
           nro_orden,
           pasajeros[indice].dni,
           pasajeros[indice].nombre,
           pasajeros[indice].destino,
           pasajeros[indice].asiento,
           pasajeros[indice].activo == 1 ? "Activo    |" : "Inactivo  |");
}

void modificar_datos(Pasajero pasajeros[], int asientos[], int cantidad_pasajeros)
{
    int dni_modificar, indice_modificar;
    if (cantidad_pasajeros == 0)
    {
        printf("No hay pasajeros registrados para modificar.\n");
        return;
    }
    dni_modificar = pedir_entero_entre("Ingrese DNI del pasajero a modificar: ", MIN_dni, MAX_dni);
    indice_modificar = buscar_dni_binario(pasajeros, cantidad_pasajeros, dni_modificar);
    if (indice_modificar != -1 && pasajeros[indice_modificar].activo == 1)
    {
        int opcion_modificacion;
        menu_cambiar_datos();
        opcion_modificacion = pedir_entero("\nIngrese una opcion: ");
        switch (opcion_modificacion)
        {
        case MODIFICAR_DESTINO:
            pedir_texto("Ingrese nuevo destino: ", pasajeros[indice_modificar].destino, MAX_DESTINO);
            printf("Destino modificado exitosamente.\n");
            break;
        case MODIFICAR_ASIENTO:
            asientos[pasajeros[indice_modificar].asiento - 1] = 0; // Liberar asiento actual
            mostrar_asientos(asientos);
            pasajeros[indice_modificar].asiento = seleccionar_asiento(asientos);
            printf("Asiento modificado exitosamente.\n");
            break;
        case VOLVER_MENU_PRINCIPAL:
            printf("Volviendo al menu principal.\n");
            return;
        default:
            printf("Opcion invalida. Volviendo al menu principal.\n");
            return;
        }
        return;
    }
    printf("No se encontro un pasajero activo con DNI %d.\n", dni_modificar);
}

bool confirmar_baja()
{
    char buffer[3];
    pedir_texto("Confirma la baja del pasajero? (s/n): ", buffer, 3);
    char confirmacion = buffer[0];
    return tolower(confirmacion) == 's';
}

void baja_pasajero(Pasajero pasajeros[], int asientos[], int *cantidad_pasajeros)
{
    int dni_baja, indice_baja, nro_orden = 1;
    if (*cantidad_pasajeros == 0)
    {
        printf("No hay pasajeros registrados para dar de baja.\n");
        return;
    }
    dni_baja = pedir_entero_entre("Ingrese DNI del pasajero a dar de baja: ", MIN_dni, MAX_dni);
    indice_baja = buscar_dni_binario(pasajeros, *cantidad_pasajeros, dni_baja);
    if (indice_baja != -1 && pasajeros[indice_baja].activo == 1)
    {
        tabulacion();
        listar(pasajeros, indice_baja, nro_orden);
        // preguntar confirmacion
        if (confirmar_baja())
        {
            pasajeros[indice_baja].activo = 0;                // Marcar como inactivo
            asientos[pasajeros[indice_baja].asiento - 1] = 0; // Liberar asiento
            printf("Pasajero dado de baja exitosamente.\n");
            return;
        }
        else
        {
            printf("Baja cancelada por el usuario.\n");
            return;
        }
    }
    printf("No se encontro un pasajero activo con DNI %d.\n", dni_baja);
}

void alta_pasajero(Pasajero pasajeros[], int asientos[], int *cantidad_pasajeros)
{
    Pasajero nuevo_pasajero;
    if (*cantidad_pasajeros >= MAX_PASAJEROS)
    {
        printf("No se pueden registrar más pasajeros en el sistema.\n");
        return;
    }
    if (!hay_asiento_disponible(asientos))
    {
        printf("No hay asientos disponibles para este vuelo.\n");
        return;
    }
    nuevo_pasajero.dni = pedir_entero_entre("Ingrese DNI del pasajero: ", MIN_dni, MAX_dni);
    if (buscar_dni_binario(pasajeros, *cantidad_pasajeros, nuevo_pasajero.dni) != -1)
    {
        printf("Ya existe un pasajero registrado con DNI %d.\n", nuevo_pasajero.dni);
        return;
    }
    pedir_texto("Ingrese nombre del pasajero: ", nuevo_pasajero.nombre, MAX_NOMBRE_PASAJERO);
    pedir_texto("Ingrese destino del pasajero: ", nuevo_pasajero.destino, MAX_DESTINO);
    mostrar_asientos(asientos);
    nuevo_pasajero.asiento = seleccionar_asiento(asientos);
    nuevo_pasajero.activo = 1; // Marcar como activo
    insertar_pasajero_ordenado(pasajeros, cantidad_pasajeros, nuevo_pasajero); // se encarga de incrementar cantidad_pasajeros
    printf("Pasajero dado de alta exitosamente.\n");
}

int seleccionar_asiento(int asientos[])
{
    int asiento;
    for (;;)
    {
        asiento = pedir_entero_entre("Seleccione un asiento disponible: ", 1, MAX_ASIENTOS);
        if (asientos[asiento - 1] == 0)
        {
            asientos[asiento - 1] = 1; // Marcar asiento como ocupado
            return asiento;
        }
        else
        {
            printf("Asiento %d ya está ocupado. Por favor, elija otro.\n", asiento);
        }
    }
}

bool hay_asiento_disponible(int asientos[])
{
    for (int i = 0; i < MAX_ASIENTOS; i++)
    {
        if (asientos[i] == 0)
            return true;
    }
    return false;
}

void mostrar_asientos(int asientos[])
{
    printf("Asientos disponibles:\n");
    for (int i = 0; i < MAX_ASIENTOS; i++)
    {
        if (asientos[i] == 0)
        {
            printf("Asiento %2d: Disponible\n", i + 1);
        }
        else
        {
            printf("Asiento %2d: Ocupado\n", i + 1);
        }
    }
}
void menu_cambiar_datos()
{
    printf("\n*** Menu de Modificacion ***\n"
           "\n[1] Modificar Destino"
           "\n[2] Modificar Asiento"
           "\n[0] Volver al Menu Principal");
}

void menu_principal()
{
    printf("\n*** Menu Principal ***\n"
           "\n[1] Alta de Pasajero"
           "\n[2] Baja de Pasajero"
           "\n[3] Modificacion de Pasajero"
           "\n[4] Listar Pasajeros Activos"
           "\n[5] Listar Pasajeros por Destino"
           "\n[6] Listar Todos los Pasajeros"
           "\n[0] Salir");
}

void iniciar_arreglos(Pasajero pasajeros[], int asientos[])
{
    for (int i = 0; i < MAX_PASAJEROS; i++)
    {
        pasajeros[i].activo = -1; // -1 = sin usar / vacio
        pasajeros[i].dni = 0;
        pasajeros[i].asiento = 0;
        pasajeros[i].nombre[0] = '\0';
        pasajeros[i].destino[0] = '\0';
    }
    for (int i = 0; i < MAX_ASIENTOS; i++)
    {
        asientos[i] = 0; // 0 = asiento libre
    }
}
