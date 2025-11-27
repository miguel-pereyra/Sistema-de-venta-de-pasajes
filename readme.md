##Sistema de Venta de Pasajes para un Vuelo

Versión 1.0.0 — Autor: Miguel Pereyra

Este proyecto implementa un sistema de gestión de pasajeros para un vuelo con capacidad máxima de 20 asientos.
Funciona por consola y permite realizar operaciones completas de ABM (Alta, Baja y Modificación), además de listados y persistencia en archivo CSV.

# Características principales

El sistema permite:

- Alta de pasajero

Validación de DNI

Selección de asiento libre

Inserción ordenada por DNI

Estado inicial: activo

- Baja lógica

Búsqueda por DNI mediante búsqueda binaria

Confirmación antes de dar de baja

Liberación automática del asiento

- Modificación

Cambio de destino

Cambio de asiento (liberando el anterior y validando disponibilidad)

- Listados

Pasajeros activos solamente

Pasajeros por destino

Todos los pasajeros (activos e inactivos)

- Persistencia

Carga inicial desde pasajeros.csv (si existe)

Guardado automático al salir del programa

CSV estructurado con campos:

DNI,Nombre,Destino,Asiento,Estado

# Estructura del proyecto
```text
Proyecto-Vuelo/
│── src/
│   ├── main.c
│   ├── funciones.c
│   └── funciones.h
│
│── data/
│   └── pasajeros.csv
│
│── makefile
│── .gitignore
│── README.md
```

El archivo pasajeros.csv se genera o actualiza automáticamente.

# Requisitos

Compilador C compatible con C99 o superior
(MinGW, GCC, Clang, etc.)

Windows o Linux
(Probado principalmente en Windows + MinGW)

# Compilación
- Compilar manualmente (sin makefile)
gcc src/main.c src/funciones.c -o vuelo

- Usando makefile

Dentro del directorio principal:
```
make
```

Generará:

vuelo.exe  (Windows)
./vuelo    (Linux)

# Ejecución
Windows:
vuelo.exe

Linux:
./vuelo


El sistema mostrará el menú principal:

[1] Alta de Pasajero
[2] Baja de Pasajero
[3] Modificación de Pasajero
[4] Listar Pasajeros Activos
[5] Listar Pasajeros por Destino
[6] Listar Todos los Pasajeros
[0] Salir

# Archivo CSV generado

Ejemplo de pasajeros.csv:

DNI,Nombre,Destino,Asiento,Estado
30567219,Juan Pablo Herrera,Rosario,15,1
33451290,Ana Maria Garcia,Cordoba,8,1
41329987,Carolina Sosa,Salta,12,0


Estado = 1 → activo

Estado = 0 → inactivo (baja lógica)

# Detalles técnicos importantes

Los pasajeros se almacenan ordenados por DNI ascendentes.

Las búsquedas por DNI usan búsqueda binaria → mayor eficiencia.

Los asientos se administran con un arreglo de 20 enteros (0 = libre, 1 = ocupado).

Los registros vacíos se marcan con activo = -1.
