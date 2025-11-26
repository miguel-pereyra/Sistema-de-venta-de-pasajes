# Compilador
CC = gcc

# Flags de compilación
CFLAGS = -Wall -Wextra -std=c11

# Archivos fuente
SRC = main.c funciones.c

# Archivos objeto (automático)
OBJ = $(SRC:.c=.o)

# Nombre del ejecutable
TARGET = vuelo

# Regla por defecto
all: $(TARGET)

# Cómo generar el ejecutable a partir de los .o
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

# Regla general para compilar cualquier .c → .o
%.o: %.c funciones.h
	$(CC) $(CFLAGS) -c $< -o $@

# Quitar archivos generados
clean:
	del *.o 2>nul || true
	del $(TARGET).exe 2>nul || true
