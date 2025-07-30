#ifndef UTILS_H
#define UTILS_H


// Listas de palabras válidas para fuzzy match
extern const char* modos_validos[];
extern const int num_modos;
extern const char* parametros_validos[];
extern const int num_parametros;
extern const char* comandos_gpu_validos[];
extern const int num_comandos_gpu;

int levenshtein(const char* s1, const char* s2);
const char* sugerir_palabra(const char* palabra, const char** lista, int cantidad, int max_distancia);

#endif // UTILS_H