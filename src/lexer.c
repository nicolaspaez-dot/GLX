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
    char* token;
    char** tokens = NULL;
    *cantidad = 0;
    
    char* ptr = copia;
    while (*ptr) {
        // Saltar espacios y tabs
        while (*ptr == ' ' || *ptr == '\t' || *ptr == '\n') ptr++;
        if (*ptr == '\0') break;

        // Soporte para strings entre comillas dobles
        if (*ptr == '"') {
            ptr++; // Saltar la comilla inicial
            char* start = ptr;
            while (*ptr && *ptr != '"') ptr++;
            size_t len = ptr - start;
            char* str_token = (char*)malloc(len + 3); // +2 para comillas, +1 para null
            str_token[0] = '"';
            strncpy(str_token + 1, start, len);
            str_token[len + 1] = '"';
            str_token[len + 2] = '\0';
            agregar_token(&tokens, cantidad, str_token);
            free(str_token);
            if (*ptr == '"') ptr++; // Saltar la comilla final
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

//strtok: string y token "cosas por las que se divide"