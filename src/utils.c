#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "utils.h"

// Listas de palabras válidas para fuzzy match
const char* modos_validos[] = {"quiet", "balanced", "performance"};
const int num_modos = 3;

// Parámetros válidos basados en modelo.txt (solo los que realmente funcionan)
const char* parametros_validos[] = {
    "dynamic_boost", 
    "cpu_max_perf", 
    "cpu_min_perf", 
    "turbo_boost", 
    "persist_mode", 
    "battery_conservation", 
    "fnlock", 
    "mode", 
    "modo"
};
const int num_parametros = 9;

// Comandos GPU válidos (solo los que realmente funcionan)
const char* comandos_gpu_validos[] = {
    "status", 
    "reset", 
    "vars", 
    "help", 
    "run", 
    "mode", 
    "modo"
};
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

// Función para ejecutar comandos del sistema y capturar su salida
char* execute_system_command(const char* command) {
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        return NULL;
    }
    
    char buffer[128];
    char* result = malloc(1);
    result[0] = '\0';
    
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        size_t old_len = strlen(result);
        size_t new_len = old_len + strlen(buffer);
        result = realloc(result, new_len + 1);
        strcpy(result + old_len, buffer);
    }
    
    pclose(pipe);
    return result;
}

// Función para cargar modos GPU desde archivo
GPU_Mode* load_gpu_modes(const char* filename, int* num_modes) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: No se pudo abrir el archivo %s\n", filename);
        return NULL;
    }
    
    GPU_Mode* modes = malloc(10 * sizeof(GPU_Mode)); // Máximo 10 modos
    *num_modes = 0;
    
    char line[256];
    GPU_Mode* current_mode = NULL;
    
    while (fgets(line, sizeof(line), file)) {
        // Eliminar salto de línea
        line[strcspn(line, "\n")] = 0;
        
        // Ignorar líneas vacías y comentarios
        if (strlen(line) == 0 || line[0] == '#') {
            continue;
        }
        
        // Buscar inicio de modo: "mode: nombre"
        if (strncmp(line, "mode:", 5) == 0) {
            if (*num_modes >= 10) break; // Máximo 10 modos
            
            current_mode = &modes[*num_modes];
            memset(current_mode, 0, sizeof(GPU_Mode)); // Inicializar a 0
            
            // Extraer nombre del modo
            char* mode_name = line + 5;
            while (*mode_name == ' ') mode_name++; // Saltar espacios
            
            strncpy(current_mode->name, mode_name, sizeof(current_mode->name) - 1);
            current_mode->name[sizeof(current_mode->name) - 1] = '\0';
            
            (*num_modes)++;
            printf("📋 Modo encontrado: %s\n", current_mode->name);
        }
        // Buscar configuraciones: "- parametro: valor"
        else if (current_mode && line[0] == '-' && strstr(line, ":")) {
            char* param_start = line + 1;
            while (*param_start == ' ') param_start++; // Saltar espacios después del guión
            
            char* colon = strchr(param_start, ':');
            if (colon) {
                *colon = '\0'; // Separar parámetro y valor
                char* value = colon + 1;
                while (*value == ' ') value++; // Saltar espacios después de los dos puntos
                
                // Parsear según el parámetro
                if (strcmp(param_start, "power_limit") == 0) {
                    current_mode->power_limit = atoi(value);
                    printf("   Power Limit: %dW\n", current_mode->power_limit);
                }
                else if (strcmp(param_start, "fan_speed") == 0) {
                    current_mode->fan_speed = atoi(value);
                    printf("   Fan Speed: %d%%\n", current_mode->fan_speed);
                }
                else if (strcmp(param_start, "clocks") == 0) {
                    strncpy(current_mode->clocks, value, sizeof(current_mode->clocks) - 1);
                    current_mode->clocks[sizeof(current_mode->clocks) - 1] = '\0';
                    printf("   Clocks: %s MHz\n", current_mode->clocks);
                }
                else if (strcmp(param_start, "persist mode") == 0) {
                    current_mode->persist_mode = (strcmp(value, "on") == 0) ? 1 : 0;
                    printf("   Persist Mode: %s\n", current_mode->persist_mode ? "ON" : "OFF");
                }
            }
        }
    }
    
    fclose(file);
    printf("✅ Cargados %d modos desde %s\n", *num_modes, filename);
    return modes;
}