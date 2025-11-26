// funciones.h
#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <stddef.h>  // para size_t

// Prototipos de funciones genéricas
int pedir_texto(const char *msj, char *buff, size_t tam);
double pedir_double(const char *msj);
double pedir_double_entre(const char *msj, double vmin, double vmax);
int pedir_entero(const char *msj);
int pedir_entero_entre(const char *msj, int vmin, int vmax);
void reportar_error(const char *msj);
void mostrar_info(const char *msj);
void esperar_enter(void);

// Prototipo de habilitarANSI
void habilitarANSI(void);

#endif // FUNCIONES_H
