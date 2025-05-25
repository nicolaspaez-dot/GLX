#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"

int main() {
    FILE* archivo;
    char linea[256];

    archivo = fopen("gx_programs/ejemplo.gx", "r");
    if (archivo == NULL) {
        perror("No se pudo abrir el archivo");
        return 1;
    }

    while (fgets(linea, sizeof(linea), archivo)) {
        printf("Línea: %s", linea);

        int cantidad = 0;
        char** tokens = lexer_tokenize(linea, &cantidad);

        for (int i = 0; i < cantidad; i++) {
            printf("  Token[%d]: %s\n", i, tokens[i]);
        }

        liberar_tokens(tokens, cantidad);
    }

    fclose(archivo);
    return 0;
}
