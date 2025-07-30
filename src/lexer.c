#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"

// Función auxiliar: agrega un token al array de tokens
void agregar_token(char*** tokens, int* cantidad, const char* valor) {
    *tokens = realloc(*tokens, (*cantidad + 1) * sizeof(char*));
    (*tokens)[*cantidad] = strdup(valor);
    (*cantidad)++; 
}

char** lexer_tokenize(const char* linea, int* cantidad) {
    char* copia = strdup(linea);
    char** tokens = NULL;
    *cantidad = 0;
    
    char* ptr = copia;
    while (*ptr) {
        // Saltar espacios y tabs
        while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n') ptr++;
        if (*ptr == '\0') break;

        // Ignorar comentarios (todo lo que viene después de #)
        if (*ptr == '#') {
            break; // Terminar el procesamiento de esta línea
        }

        // Ignorar guiones al inicio de línea (parte de la estructura)
        if (*ptr == '-') {
            ptr++;
            // Saltar espacios después del guión
            while (*ptr == ' ' || *ptr == '\t') ptr++;
            continue;
        }

        // Soporte robusto para strings entre comillas dobles con escapes
        if (*ptr == '"') {
            ptr++; // Saltar la comilla inicial
            char buffer[256];
            int buf_idx = 0;
            buffer[buf_idx++] = '"'; // Mantener la comilla inicial en el token
            int cerrado = 0;
            while (*ptr && buf_idx < 254) {
                if (*ptr == '\\') {
                    ptr++;
                    if (*ptr == 'n') buffer[buf_idx++] = '\n';
                    else if (*ptr == 't') buffer[buf_idx++] = '\t';
                    else if (*ptr == 'r') buffer[buf_idx++] = '\r';
                    else if (*ptr == '"') buffer[buf_idx++] = '"';
                    else if (*ptr == '\\') buffer[buf_idx++] = '\\';
                    else if (*ptr) buffer[buf_idx++] = *ptr;
                    if (*ptr) ptr++;
                } else if (*ptr == '"') {
                    buffer[buf_idx++] = '"'; // Mantener la comilla final en el token
                    ptr++; // Saltar la comilla final
                    cerrado = 1;
                    break;
                } else {
                    buffer[buf_idx++] = *ptr;
                    ptr++;
                }
            }
            buffer[buf_idx] = '\0';
            agregar_token(&tokens, cantidad, buffer);
            // Si no se cerró el string, avanzar hasta el final de la comilla para no romper el flujo
            if (!cerrado && *ptr == '"') ptr++;
            continue;
        }

        // Buscar delimitadores : o = en el token actual
        char* start = ptr;
        while (*ptr && *ptr != ' ' && *ptr != '\t' && *ptr != '\n' && *ptr != ':' && *ptr != '=') ptr++;
        size_t len = ptr - start;
        if (len > 0) {
            char temp[256];
            strncpy(temp, start, len);
            temp[len] = '\0';
            agregar_token(&tokens, cantidad, temp);
        }
        if (*ptr == ':' || *ptr == '=') {
            char temp[2] = {*ptr, '\0'};
            agregar_token(&tokens, cantidad, temp);
            ptr++;
        }
    }
    free(copia);
    return tokens;
}

void liberar_tokens(char** tokens, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

