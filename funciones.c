#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "funciones.h"


#ifdef _WIN32
    #include <windows.h>
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif

    void habilitarANSI(void)
    {
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        if (h == INVALID_HANDLE_VALUE) return;
        DWORD modo = 0;
        if (!GetConsoleMode(h, &modo)) return;
        SetConsoleMode(h, modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#else
    // En Linux/Mac no hace falta hacer nada
    void habilitarANSI(void) { }
#endif

//Declaracion de funciones privadas
static void limpiarStdin(void);
static void aceptarComaOpunto(char *s);
static void saltarEspacios(const char **p);

//Implementacion

/* limpia stdin hasta fin de línea*/
static void limpiarStdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* no-op */ }
}

/* Lee una línea en buff (tamaño tam), sin '\n'. Devuelve 1 si ok, 0 si EOF/error. */
int pedir_texto(const char *msj, char *buff, size_t tam) {
    if (tam == 0) return 0;
    if (msj && *msj) printf("%s", msj);

    if (!fgets(buff, (int)tam, stdin)) {
        /* EOF o error */
        if (ferror(stdin)) clearerr(stdin);
        buff[0] = '\0';
        return 0;
    }

    size_t len = strlen(buff);
    if (len > 0 && buff[len - 1] == '\n') {
        buff[len - 1] = '\0';
    } else if (len == tam - 1) {
        /* línea más larga que el buffer: drenar resto */
        limpiarStdin();
    }
    return 1;
}

/* Reemplaza ',' por '.' para aceptar coma decimal */
static void aceptarComaOpunto(char *s) {
    for (; *s; ++s) if (*s == ',') *s = '.';
}

/* Salta espacios en *p */
static void saltarEspacios(const char **p) {
    while (**p && isspace((unsigned char)**p)) (*p)++;
}

double pedir_double(const char *msj) {
    char buff[128];
    for (;;) {
        if (!pedir_texto(msj, buff, sizeof(buff))) {
            puts("[X] Error de lectura. Intente de nuevo.");
            continue;
        }
        aceptarComaOpunto(buff);

        errno = 0;
        char *endp = NULL;
        double n = strtod(buff, &endp);

        /* Validaciones */
        if (endp == buff) {
            puts("[X] Entrada invalida: no se encontraron digitos.");
            continue;
        }
        const char *rest = endp;
        saltarEspacios(&rest);
        if (*rest != '\0') {
            puts("[X] Entrada invalida: sobran caracteres.");
            continue;
        }
        if (errno == ERANGE) {
            puts("[X] Numero fuera de rango.");
            continue;
        }
        return n;
    }
}

double pedir_double_entre(const char *msj, double vmin, double vmax) {
    for (;;) {
        double n = pedir_double(msj);
        if (n < vmin || n > vmax) {
            printf("Fuera de rango, el número debe estar entre %.2f y %.2f.\n", vmin, vmax);
            continue;
        }
        return n;
    }
}

int pedir_entero(const char *msj) {
    char buff[128];
    for (;;) {
        if (!pedir_texto(msj, buff, sizeof(buff))) {
            puts("[X] Error de lectura. Intente de nuevo.");
            continue;
        }
        errno = 0;
        char *endp = NULL;
        long val = strtol(buff, &endp, 10);

        if (endp == buff) {
            puts("[X] Entrada invalida: no se encontraron digitos.");
            continue;
        }
        const char *rest = endp;
        saltarEspacios(&rest);
        if (*rest != '\0') {
            puts("[X] Entrada invalida: sobran caracteres.");
            continue;
        }
        if (errno == ERANGE || val < INT_MIN || val > INT_MAX) {
            puts("[X] Entero fuera de rango.");
            continue;
        }
        return (int)val;
    }
}

int pedir_entero_entre(const char *msj, int vmin, int vmax) {
    for (;;) {
        int n = pedir_entero(msj);
        if (n < vmin || n > vmax) {
            printf("Fuera de rango, el numero debe estar entre %d y %d.\n", vmin, vmax);
            continue;
        }
        return n;
    }
}

void reportar_error(const char *msj) {
    printf("[X] Error: %s\n", msj ? msj : "(sin detalle)");
}

void mostrar_info(const char *msj) {
    printf("[!] Info: %s\n", msj ? msj : "(sin detalle)");
}

void esperar_enter(void) {
    puts("\nPresione [ENTER] para continuar...");
    /* Consumir hasta '\n' (por si quedó algo previo) */
    limpiarStdin();
    /* Esperar un Enter real */
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* no-op */ }
}



