#ifndef LEXER_H
#define LEXER_H

char** lexer_tokenize(const char* linea, int* cantidad_tokens);
void liberar_tokens(char** tokens, int cantidad);

#endif
