#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

// Listas de palabras válidas para fuzzy match
const char* modos_validos[] = {"quiet", "balanced", "performance"};
const int num_modos = 3;
const char* parametros_validos[] = {"power_limit", "fan_speed", "clocks", "persist"};
const int num_parametros = 4;
const char* comandos_gpu_validos[] = {"status", "reset", "vars", "help", "mode", "power_limit", "fan_speed"};
const int num_comandos_gpu = 7;

// Función auxiliar para el mínimo de tres valores
static int min3(int a, int b, int c) {
    if (a < b && a < c) return a;
    if (b < c) return b;
    return c;
}

// Distancia de Levenshtein para fuzzy match
int levenshtein(const char* s1, const char* s2) {
    int len1 = strlen(s1), len2 = strlen(s2);
    int* prev = calloc(len2 + 1, sizeof(int));
    int* curr = calloc(len2 + 1, sizeof(int));
    for (int j = 0; j <= len2; j++) prev[j] = j;
    for (int i = 1; i <= len1; i++) {
        curr[0] = i;
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            curr[j] = min3(
                curr[j-1] + 1,
                prev[j] + 1,
                prev[j-1] + cost
            );
        }
        int* tmp = prev; prev = curr; curr = tmp;
    }
    int dist = prev[len2];
    free(prev); free(curr);
    return dist;
}

// Sugerir palabra similar si la distancia es baja
const char* sugerir_palabra(const char* palabra, const char** lista, int cantidad, int max_distancia) {
    int mejor_dist = max_distancia + 1;
    const char* sugerencia = NULL;
    for (int i = 0; i < cantidad; i++) {
        int dist = levenshtein(palabra, lista[i]);
        if (dist == 0) {
            // Si la distancia es 0, la palabra ya está en la lista, no sugerir nada
            return NULL;
        }
        if (dist < mejor_dist) {
            mejor_dist = dist;
            sugerencia = lista[i];
        }
    }
    return (mejor_dist <= max_distancia) ? sugerencia : NULL;
}