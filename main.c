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

#define RESET "\033[0m"
#define BG_ZEBRA "\033[48;5;236m"
#define ROJO "\033[31m"
#define VERDE "\033[32m"
#define AMARILLO "\033[33m"
#define AZUL "\033[34m"
#define MAGENTA "\033[35m"
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
enum destinos
{
    DESTINO_BUENOS_AIRES = 1,
    DESTINO_CORDOBA,
    DESTINO_MENDOZA,
    DESTINO_SALTA,
    DESTINO_BARILOCHE
};
const char *DESTINOS[] = {
    "Sin destino",
    "Buenos Aires",
    "Cordoba",
    "Mendoza",
    "Salta",
    "Bariloche"};
typedef struct
{
    int dni;
    char nombre[MAX_NOMBRE_PASAJERO];
    int destino;
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
int seleccionar_destino();
void menu_seleccionar_destino();

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
            puts("Saliendo del sistema de venta de pasajes. Hasta luego...");
            esperar_enter();
            break;
        default:
            printf(ROJO "Opcion invalida. Por favor, intente de nuevo.\n" RESET);
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
        printf(ROJO "No se pudo abrir el archivo %s. Se iniciara con datos vacios.\n" RESET, nombre_archivo);
        return;
    }
    char linea[150];
    fgets(linea, sizeof(linea), archivo); // descartar cabecera
    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        Pasajero pasajero;
        if (sscanf(linea, "%d,%49[^,],%d,%d,%d",
                   &pasajero.dni,
                   pasajero.nombre,
                   &pasajero.destino,
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
                printf(ROJO "Se alcanzo la capacidad maxima de pasajeros al cargar desde el archivo.\n" RESET);
                break;
            }
        }
    }
    fclose(archivo);
    printf(VERDE "Datos de pasajeros cargados desde el archivo %s exitosamente.\n" RESET, nombre_archivo);
}
void guardar_pasajeros_en_archivo(Pasajero pasajeros[], int cantidad_pasajeros, const char *nombre_archivo)
{
    FILE *archivo = fopen(nombre_archivo, "w");
    if (archivo == NULL)
    {
        printf(ROJO "Error al abrir el archivo %s para escritura.\n" RESET, nombre_archivo);
        return;
    }
    fprintf(archivo, "DNI,Nombre,Destino,Asiento,Estado\n");
    for (int i = 0; i < cantidad_pasajeros; i++)
    {
        fprintf(archivo, "%d,%s,%d,%d,%d\n",
                pasajeros[i].dni,
                pasajeros[i].nombre,
                pasajeros[i].destino,
                pasajeros[i].asiento,
                pasajeros[i].activo);
    }
    fclose(archivo);
    printf(VERDE "Datos de pasajeros guardados en el archivo %s exitosamente.\n" RESET, nombre_archivo);
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
    printf(MAGENTA "\n*** Lista de Todos los Pasajeros ***\n" RESET);
    tabulacion();
    if (cantidad_pasajeros == 0)
    {
        printf(ROJO "No hay pasajeros registrados.\n" RESET);
        return;
    }
    for (int i = 0; i < cantidad_pasajeros; i++)
    {
        if (pasajeros[i].activo != -1) // solo los ocupados (activos o inactivos)
            listar(pasajeros, i, nro_orden++);
    }
    esperar_enter();
}
void listar_pasajeros_por_destino(Pasajero pasajeros[], int cantidad_pasajeros)
{
    if (cantidad_pasajeros == 0)
    {
        printf(ROJO "No hay pasajeros registrados.\n" RESET);
        return;
    }
    int destino_buscar;
    int nro_orden = 1;
    menu_seleccionar_destino();
    destino_buscar = seleccionar_destino();
    printf(MAGENTA "\n*** Lista de Pasajeros activos con destino %s ***\n" RESET, DESTINOS[destino_buscar]);
    tabulacion();
    bool hay_pasajeros = false;
    for (int i = 0; i < cantidad_pasajeros; i++)
    {
        if (pasajeros[i].activo == 1 && pasajeros[i].destino == destino_buscar)
        {
            listar(pasajeros, i, nro_orden++);
            hay_pasajeros = true;
        }
    }
    if (!hay_pasajeros)
    {
        printf(ROJO "No hay pasajeros activos registrados con destino %s.\n" RESET, DESTINOS[destino_buscar]);
    }
    else
    {
        esperar_enter();
    }
}
void listar_pasajeros_activos(Pasajero pasajeros[], int cantidad_pasajeros)
{
    printf(MAGENTA "\n*** Lista de Pasajeros Activos ***\n" RESET);
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
        printf(ROJO "No hay pasajeros activos registrados.\n" RESET);
    }
    else
    {
        esperar_enter();
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
    printf("%s%2d  | %-10d | %-30s | %-15s | %-8d | %s%s\n",
           (nro_orden % 2 == 0) ? BG_ZEBRA : RESET,
           nro_orden,
           pasajeros[indice].dni,
           pasajeros[indice].nombre,
           DESTINOS[pasajeros[indice].destino],
           pasajeros[indice].asiento,
           pasajeros[indice].activo ? "Activo    |" : "Inactivo  |",
           RESET);
}

void modificar_datos(Pasajero pasajeros[], int asientos[], int cantidad_pasajeros)
{
    int dni_modificar, indice_modificar;
    if (cantidad_pasajeros == 0)
    {
        printf(ROJO "No hay pasajeros registrados para modificar.\n" RESET);
        return;
    }
    dni_modificar = pedir_entero_entre("Ingrese DNI del pasajero a modificar: ", MIN_dni, MAX_dni);
    indice_modificar = buscar_dni_binario(pasajeros, cantidad_pasajeros, dni_modificar);
    if (indice_modificar != -1 && pasajeros[indice_modificar].activo == 1)
    {
        int opcion_modificacion;
        tabulacion();
        listar(pasajeros, indice_modificar, 1);
        menu_cambiar_datos();
        opcion_modificacion = pedir_entero("\nIngrese una opcion: ");
        switch (opcion_modificacion)
        {
        case MODIFICAR_DESTINO:
            menu_seleccionar_destino();
            pasajeros[indice_modificar].destino = seleccionar_destino();
            printf(VERDE "Destino modificado exitosamente.\n" RESET);
            break;
        case MODIFICAR_ASIENTO:
            asientos[pasajeros[indice_modificar].asiento - 1] = 0; // Liberar asiento actual
            mostrar_asientos(asientos);
            pasajeros[indice_modificar].asiento = seleccionar_asiento(asientos);
            printf(VERDE "Asiento modificado exitosamente.\n" RESET);
            break;
        case VOLVER_MENU_PRINCIPAL:
            printf("Volviendo al menu principal.\n");
            return;
        default:
            printf(ROJO "Opcion invalida. Volviendo al menu principal.\n" RESET);
            return;
        }
        return;
    }
    printf(ROJO "No se encontro un pasajero activo con DNI %d.\n" RESET, dni_modificar);
}
bool confirmar_baja()
{
    char buffer[3];
    pedir_texto(AMARILLO "Confirma la baja del pasajero? (s/n): " RESET, buffer, 3);
    char confirmacion = buffer[0];
    return tolower(confirmacion) == 's';
}
void baja_pasajero(Pasajero pasajeros[], int asientos[], int *cantidad_pasajeros)
{
    int dni_baja, indice_baja, nro_orden = 1;
    if (*cantidad_pasajeros == 0)
    {
        printf(ROJO "No hay pasajeros registrados para dar de baja.\n" RESET);
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
            printf(VERDE "Pasajero dado de baja exitosamente.\n" RESET);
            return;
        }
        else
        {
            printf(ROJO "Baja cancelada por el usuario.\n" RESET);
            return;
        }
    }
    printf(ROJO "No se encontro un pasajero activo con DNI %d.\n" RESET, dni_baja);
}
void alta_pasajero(Pasajero pasajeros[], int asientos[], int *cantidad_pasajeros)
{
    Pasajero nuevo_pasajero;
    if (*cantidad_pasajeros >= MAX_PASAJEROS)
    {
        printf(ROJO "No se pueden registrar más pasajeros en el sistema.\n" RESET);
        return;
    }
    if (!hay_asiento_disponible(asientos))
    {
        printf(ROJO "No hay asientos disponibles para este vuelo.\n" RESET);
        return;
    }
    nuevo_pasajero.dni = pedir_entero_entre("Ingrese DNI del pasajero: ", MIN_dni, MAX_dni);
    if (buscar_dni_binario(pasajeros, *cantidad_pasajeros, nuevo_pasajero.dni) != -1)
    {
        printf(ROJO "Ya existe un pasajero registrado con DNI %d.\n" RESET, nuevo_pasajero.dni);
        return;
    }
    pedir_texto("Ingrese nombre del pasajero: ", nuevo_pasajero.nombre, MAX_NOMBRE_PASAJERO);
    menu_seleccionar_destino();
    nuevo_pasajero.destino = seleccionar_destino();
    mostrar_asientos(asientos);
    nuevo_pasajero.asiento = seleccionar_asiento(asientos);
    nuevo_pasajero.activo = 1;                                                 // Marcar como activo
    insertar_pasajero_ordenado(pasajeros, cantidad_pasajeros, nuevo_pasajero); // se encarga de incrementar cantidad_pasajeros
    printf(VERDE "Pasajero dado de alta exitosamente.\n" RESET);
}
int seleccionar_destino()
{
    int destino;
    for (;;)
    {
        destino = pedir_entero_entre("Seleccione un destino: ", DESTINO_BUENOS_AIRES, DESTINO_BARILOCHE);
        if (destino >= DESTINO_BUENOS_AIRES && destino <= DESTINO_BARILOCHE)
        {
            return destino;
        }
        else
        {
            printf(ROJO "Destino invalido. Por favor, intente de nuevo.\n" RESET);
        }
    }
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
            printf(ROJO "Asiento %d ya está ocupado. Por favor, elija otro.\n" RESET, asiento);
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
void menu_seleccionar_destino()
{
    printf(AZUL "\n***Menu de Destinos ***\n" RESET
                "\n[1] Buenos Aires"
                "\n[2] Cordoba"
                "\n[3] Mendoza"
                "\n[4] Salta"
                "\n[5] Bariloche\n");
}
void menu_cambiar_datos()
{
    printf(AZUL "\n*** Menu de Modificacion ***\n" RESET
                "\n[1] Modificar Destino"
                "\n[2] Modificar Asiento"
                "\n[0] Volver al Menu Principal");
}
void menu_principal()
{
    printf(AZUL "\n*** Menu Principal ***\n" RESET
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
        pasajeros[i].destino = 0;
    }
    for (int i = 0; i < MAX_ASIENTOS; i++)
    {
        asientos[i] = 0; // 0 = asiento libre
    }
}
