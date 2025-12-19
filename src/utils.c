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
    "rgb_color",
    "rgb_brightness",
    "mode", 
    "modo"
};
const int num_parametros = 11;

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
            printf("Modo encontrado: %s\n", current_mode->name);
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
                if (strcmp(param_start, "dynamic_boost") == 0) {
                    current_mode->dynamic_boost = atoi(value);
                    printf("   Dynamic Boost: %d\n", current_mode->dynamic_boost);
                }
                else if (strcmp(param_start, "cpu_max_perf") == 0) {
                    current_mode->cpu_max_perf = atoi(value);
                    printf("   CPU Max Performance: %d%%\n", current_mode->cpu_max_perf);
                }
                else if (strcmp(param_start, "cpu_min_perf") == 0) {
                    current_mode->cpu_min_perf = atoi(value);
                    printf("   CPU Min Performance: %d%%\n", current_mode->cpu_min_perf);
                }
                else if (strcmp(param_start, "turbo_boost") == 0) {
                    current_mode->turbo_boost = atoi(value);
                    printf("   Turbo Boost: %s\n", current_mode->turbo_boost ? "OFF" : "ON");
                }
                else if (strcmp(param_start, "persist_mode") == 0) {
                    current_mode->persist_mode = atoi(value);
                    printf("   Persist Mode: %s\n", current_mode->persist_mode ? "ON" : "OFF");
                }
                else if (strcmp(param_start, "battery_conservation") == 0) {
                    current_mode->battery_conservation = atoi(value);
                    printf("   Battery Conservation: %s\n", current_mode->battery_conservation ? "ON" : "OFF");
                }
                else if (strcmp(param_start, "fnlock") == 0) {
                    current_mode->fnlock = atoi(value);
                    printf("   FnLock: %s\n", current_mode->fnlock ? "ON" : "OFF");
                }
                else if (strcmp(param_start, "rgb_color") == 0) {
                    strncpy(current_mode->rgb_color, value, sizeof(current_mode->rgb_color) - 1);
                    current_mode->rgb_color[sizeof(current_mode->rgb_color) - 1] = '\0';
                    printf("   RGB Color: %s\n", current_mode->rgb_color);
                }
                else if (strcmp(param_start, "rgb_brightness") == 0) {
                    current_mode->rgb_brightness = atoi(value);
                    printf("   RGB Brightness: %d%%\n", current_mode->rgb_brightness);
                }
            }
        }
    }
    
    fclose(file);
    printf("Cargados %d modos desde %s\n", *num_modes, filename);
    return modes;
}

// Función para controlar RGB del teclado
int set_rgb_color(const char* color, int brightness) {
    printf("\033[36m🎨 Configurando RGB: %s con brillo %d%%\033[0m\n", color, brightness);
    
    // El color del botón de encendido está vinculado al platform-profile
    // Cambiamos el perfil para cambiar el color automáticamente
    char cmd[256];
    const char* profile = NULL;
    
    if (strcmp(color, "blue") == 0) {
        printf("   🔵 Aplicando color azul (modo quiet)\n");
        profile = "low-power";
    }
    else if (strcmp(color, "white") == 0) {
        printf("   ⚪ Aplicando color blanco (modo balanced)\n");
        profile = "balanced";
    }
    else if (strcmp(color, "red") == 0) {
        printf("   🔴 Aplicando color rojo (modo performance)\n");
        profile = "performance";
    }
    else {
        printf("   ⚠️  Color no reconocido: %s\n", color);
        return 0;
    }
    
    // Cambiar platform-profile para cambiar el color del botón de encendido
    // Intentar primero la ruta moderna (kernel 6.x+)
    char path[256] = "/sys/firmware/acpi/platform_profile";
    
    // Si no existe, intentar la ruta legacy
    if (access(path, F_OK) == -1) {
        strcpy(path, "/sys/devices/pci0000:00/0000:00:1f.0/PNP0C09:00/platform-profile/platform-profile-0/profile");
    }

    snprintf(cmd, sizeof(cmd), "echo %s | sudo tee %s", profile, path);
    
    char* result = execute_system_command(cmd);
    if (result) {
        printf("   🎯 Platform-profile cambiado a '%s' - Color del botón de encendido: %s\n", profile, color);
        free(result);
    } else {
        printf("   ⚠️  Error al cambiar platform-profile\n");
        return 0;
    }
    
    // Ajustar brillo del backlight del teclado
    int max_brightness = 100; // Valor típico para backlight
    
    // Calcular brillo basado en el porcentaje
    int actual_brightness = (brightness * max_brightness) / 100;
    
    snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /sys/devices/pci0000:00/0000:00:1f.0/PNP0C09:00/VPC2004:00/leds/platform::kbd_backlight/brightness", actual_brightness);
    
    result = execute_system_command(cmd);
    if (result) {
        printf("   💡 Brillo del teclado ajustado a %d%%\n", brightness);
        free(result);
        return 1;
    } else {
        printf("   ⚠️  Error al ajustar brillo del teclado\n");
        return 0;
    }
}

// Función para obtener el color actual del botón de encendido
const char* get_current_power_button_color(void) {
    // Intentar leer de múltiples ubicaciones posibles
    const char* paths[] = {
        "/sys/firmware/acpi/platform_profile",
        "/sys/devices/pci0000:00/0000:00:1f.0/PNP0C09:00/platform-profile/platform-profile-0/profile"
    };
    
    char* result = NULL;
    for (int i = 0; i < 2; i++) {
        if (access(paths[i], R_OK) == 0) {
            char cmd[512];
            snprintf(cmd, sizeof(cmd), "cat %s 2>/dev/null", paths[i]);
            result = execute_system_command(cmd);
            if (result) break;
        }
    }
    
    if (!result) {
        return "desconocido";
    }
    
    // Eliminar salto de línea si existe
    size_t len = strlen(result);
    if (len > 0 && result[len-1] == '\n') {
        result[len-1] = '\0';
    }
    
    // Mapear perfil a color
    if (strcmp(result, "low-power") == 0 || strcmp(result, "quiet") == 0) {
        free(result);
        return "azul";
    } else if (strcmp(result, "balanced") == 0) {
        free(result);
        return "blanco";
    } else if (strcmp(result, "performance") == 0) {
        free(result);
        return "rojo";
    } else {
        free(result);
        return "desconocido";
    }
}