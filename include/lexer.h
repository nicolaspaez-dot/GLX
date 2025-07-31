#ifndef LEXER_H
#define LEXER_H

// Función para tokenizar una línea de texto
// Recibe: una línea de texto y un puntero donde guardar la cantidad de tokens
// Retorna: un array de strings (tokens)
char** lexer_tokenize(const char* linea, int* cantidad);

// Función para liberar la memoria de los tokens
// Recibe: el array de tokens y la cantidad de tokens
void liberar_tokens(char** tokens, int cantidad);

#endif // LEXER_H 