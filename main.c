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
#define separador "-------------------------\n"
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
void listar(Pasajero pasajeros[], int indice);
void modificar_datos(Pasajero pasajeros[], int asientos[], int cantidad_pasajeros);
void baja_pasajero(Pasajero pasajeros[], int asientos[], int *cantidad_pasajeros);
void alta_pasajero(Pasajero pasajeros[], int asientos[], int *cantidad_pasajeros);
int seleccionar_asiento(int asientos[]);
void mostrar_asientos(int asientos[]);
void menu_cambiar_datos();
void menu_principal();
void iniciar_arreglos(Pasajero pasajeros[], int asientos[]);

//------------------
// funcion principal
//------------------
int main()
{
    Pasajero pasajeros[MAX_PASAJEROS];
    int asientos[MAX_ASIENTOS];
    int cantidad_pasajeros = 0;
    iniciar_arreglos(pasajeros, asientos);
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
void listar_todos_los_pasajeros(Pasajero pasajeros[], int cantidad_pasajeros)
{
    // activos e inactivos
    printf("\n*** Lista de Todos los Pasajeros ***\n", separador);

    if (cantidad_pasajeros == 0)
    {
        printf("No hay pasajeros registrados.\n");
        return;
    }
    for (int i = 0; i < cantidad_pasajeros; i++)
    {
        if (pasajeros[i].activo != -1) // solo los ocupados (activos o inactivos)
            listar(pasajeros, i);
    }
}
void listar_pasajeros_por_destino(Pasajero pasajeros[], int cantidad_pasajeros)
{
    char destino_buscar[MAX_DESTINO];
    pedir_texto("Ingrese el destino a buscar: ", destino_buscar, MAX_DESTINO);
    printf("\n*** Lista de Pasajeros con destino %s ***\n", separador, destino_buscar);
    bool hay_pasajeros = false;
    for (int i = 0; i < cantidad_pasajeros; i++)
    {
        if (pasajeros[i].activo == 1 && strcmp(pasajeros[i].destino, destino_buscar) == 0)
        {
            listar(pasajeros, i);
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
    printf("\n*** Lista de Pasajeros Activos ***\n", separador);
    bool hay_activos = false;
    for (int i = 0; i < cantidad_pasajeros; i++)
    {
        if (pasajeros[i].activo == 1)
        {
            listar(pasajeros, i);
            hay_activos = true;
        }
    }
    if (!hay_activos)
    {
        printf("No hay pasajeros activos registrados.\n");
    }
}
void listar(Pasajero pasajeros[], int indice)
{
    printf("DNI: %d\n", pasajeros[indice].dni);
    printf("Nombre: %s\n", pasajeros[indice].nombre);
    printf("Destino: %s\n", pasajeros[indice].destino);
    printf("Asiento: %d\n", pasajeros[indice].asiento);
    printf("Estado: %s\n", pasajeros[indice].activo == 1 ? "Activo" : "Inactivo");
    printf(separador);
}

void modificar_datos(Pasajero pasajeros[], int asientos[], int cantidad_pasajeros)
{
    int dni_modificar;
    if (cantidad_pasajeros == 0)
    {
        printf("No hay pasajeros registrados para modificar.\n");
        return;
    }
    dni_modificar = pedir_entero_entre("Ingrese DNI del pasajero a modificar: ", MIN_dni, MAX_dni);
    for (int i = 0; i < cantidad_pasajeros; i++)
    {
        if (pasajeros[i].dni == dni_modificar && pasajeros[i].activo == 1)
        {
            int opcion_modificacion;
            menu_cambiar_datos();
            opcion_modificacion = pedir_entero("\nIngrese una opcion: ");
            switch (opcion_modificacion)
            {
            case MODIFICAR_DESTINO:
                pedir_texto("Ingrese nuevo destino: ", pasajeros[i].destino, MAX_DESTINO);
                printf("Destino modificado exitosamente.\n");
                break;
            case MODIFICAR_ASIENTO:
                mostrar_asientos(asientos);
                asientos[pasajeros[i].asiento - 1] = 0; // Liberar asiento actual
                pasajeros[i].asiento = seleccionar_asiento(asientos);
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
    }
    printf("No se encontro un pasajero activo con DNI %d.\n", dni_modificar);
}

void baja_pasajero(Pasajero pasajeros[], int asientos[], int *cantidad_pasajeros)
{
    int dni_baja;
    if (*cantidad_pasajeros == 0)
    {
        printf("No hay pasajeros registrados para dar de baja.\n");
        return;
    }
    dni_baja = pedir_entero_entre("Ingrese DNI del pasajero a dar de baja: ", MIN_dni, MAX_dni);
    for (int i = 0; i < *cantidad_pasajeros; i++)
    {
        if (pasajeros[i].dni == dni_baja && pasajeros[i].activo == 1)
        {
            pasajeros[i].activo = 0;                // Marcar como inactivo
            asientos[pasajeros[i].asiento - 1] = 0; // Liberar asiento
            printf("Pasajero con DNI %d dado de baja exitosamente.\n", dni_baja);
            return;
        }
    }
    printf("No se encontro un pasajero activo con DNI %d.\n", dni_baja);
}

void alta_pasajero(Pasajero pasajeros[], int asientos[], int *cantidad_pasajeros)
{
    Pasajero nuevo_pasajero;
    if (*cantidad_pasajeros >= MAX_PASAJEROS || !hay_asiento_disponible(asientos))
    {
        printf("No hay asientos disponibles para dar de alta un nuevo pasajero.\n");
        return;
    }
    nuevo_pasajero.dni = pedir_entero_entre("Ingrese DNI del pasajero: ", MIN_dni, MAX_dni);
    pedir_texto("Ingrese nombre del pasajero: ", nuevo_pasajero.nombre, MAX_NOMBRE_PASAJERO);
    pedir_texto("Ingrese destino del pasajero: ", nuevo_pasajero.destino, MAX_DESTINO);
    mostrar_asientos(asientos);
    nuevo_pasajero.asiento = seleccionar_asiento(asientos);
    nuevo_pasajero.activo = 1; // Marcar como activo
    pasajeros[*cantidad_pasajeros] = nuevo_pasajero;
    (*cantidad_pasajeros)++;
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
