##Descripción del Release
Este Release 1.1.0 introduce mejoras visuales y estructurales significativas respecto
a la versión 1.0.0.

● Se reemplaza el campo destino (antes texto libre) por un enum numérico 1–5.
● Se agrega interfaz ANSI: colores, fondo gris alternado (zebra rows), títulos destacados.
● Se mejora la lectura de tablas mediante filas alternadas BG.
● Ahora todos los destinos están validados y normalizados.
● Listados más prolijos y mayor claridad visual.
● Se mantiene la búsqueda binaria por DNI y el ABM completo.
● Persistencia mejorada con formato CSV normalizado.

Esta es la versión final del proyecto presentada por Miguel Pereyra.

## Compilación
Windows (MinGW / CodeBlocks)
gcc main.c funciones.c -o vuelo.exe

Linux
gcc main.c funciones.c -o vuelo

## Ejecución
Windows
vuelo.exe

Linux
./vuelo


El programa mostrará el menú principal:

[1] Alta de Pasajero
[2] Baja de Pasajero
[3] Modificación de Pasajero
[4] Listar Pasajeros Activos
[5] Listar Pasajeros por Destino
[6] Listar Todos los Pasajeros
[0] Salir

## Nuevo formato de CSV (Versión 1.1.0)

Tu archivo pasajeros.csv ahora tiene el siguiente formato:

DNI,Nombre,Destino,Asiento,Estado
30567219,Juan Pablo Herrera,2,15,1
33451290,Ana Maria Garcia,5,8,1
41329987,Carolina Sosa,4,12,0


Donde:

Destino = número entre 1 y 5 (enum)

Estado = 1 activo, 0 inactivo

Asiento = 1..20

## Mejoras visuales
- Colores ANSI

#define ROJO  "\033[31m"
#define VERDE "\033[32m"
#define AMARILLO "\033[33m"
#define AZUL "\033[34m"
#define MAGENTA "\033[35m"
#define RESET "\033[0m"

- Filas Zebradas
#define BG_ZEBRA  "\033[48;5;236m"

const char* bg = (nro_orden % 2 == 0) ? BG_ZEBRA : RESET;

- Destinos normalizados
enum destinos {
    DESTINO_BUENOS_AIRES = 1,
    DESTINO_CORDOBA,
    DESTINO_MENDOZA,
    DESTINO_SALTA,
    DESTINO_BARILOCHE
};

const char* DESTINOS[] = {
    "Buenos Aires",
    "Cordoba",
    "Mendoza",
    "Salta",
    "Bariloche"
};

## Changelog – v1.1.0
Added

Destino normalizado con enum (1..5).

Listados con alternancia visual (zebra rows).

Interfaz ANSI para mayor claridad.

Menú de selección de destino.

CSV actualizado al nuevo formato.

Improved

Listado más legible y ordenado.

Mensajes de error y éxito con colores.

Código más limpio y modular.

Fixed

Problemas con manipulación de destino como string.

Compatibilidad de lectura con búsqueda binaria.

Breaking Changes

El destino ya no es texto, sino número.

CSV de la versión 1.0.0 no es compatible.