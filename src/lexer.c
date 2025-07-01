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
    
    // Primero separa solo por espacios y saltos de línea
    token = strtok(copia, " \t\n");
    while (token != NULL) {
        // Buscamos si hay dos puntos o igual en el token
        char* dos_puntos = strchr(token, ':');
        char* igual = strchr(token, '=');
        
        if (dos_puntos != NULL) {
            // Si el token tiene ":", se divide en partes
            *dos_puntos = '\0';  // Dividimos el string en el ":"
            
            // Agregamos la parte antes del ":"
            if (strlen(token) > 0) {
                agregar_token(&tokens, cantidad, token);
            }
            
            // Agregamos el ":" como token
            agregar_token(&tokens, cantidad, ":");
            
            // Agregamos la parte después del ":" si existe
            if (strlen(dos_puntos + 1) > 0) {
                agregar_token(&tokens, cantidad, dos_puntos + 1);
            }
        } else if (igual != NULL) {
            // Si el token tiene "=", se divide en partes
            *igual = '\0';  // Dividimos el string en el "="
            
            // Agregamos la parte antes del "="
            if (strlen(token) > 0) {
                agregar_token(&tokens, cantidad, token);
            }
            
            // Agregamos el "=" como token
            agregar_token(&tokens, cantidad, "=");
            
            // Agregamos la parte después del "=" si existe
            if (strlen(igual + 1) > 0) {
                agregar_token(&tokens, cantidad, igual + 1);
            }
        } else {
            // Si no hay ":" ni "=", agregamos el token completo
            agregar_token(&tokens, cantidad, token);
        }
        
        token = strtok(NULL, " \t\n");
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