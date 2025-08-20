#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "../include/interpreter.h"
#include "utils.h"

// Variables globales para simular el estado de la GPU
static char gpu_mode[50] = "normal";

// Sistema de variables
#define MAX_VARIABLES 100
#define MAX_VAR_NAME 50

typedef struct {
    char name[MAX_VAR_NAME];
    char value[100];
    int is_number;
} Variable;

static Variable variables[MAX_VARIABLES];
static int num_variables = 0;

// Buscar una variable por nombre
Variable* find_variable(const char* name) {
    for (int i = 0; i < num_variables; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return &variables[i];
        }
    }
    return NULL;
}

// Agregar o actualizar una variable
void set_variable(const char* name, const char* value, int is_number) {
    Variable* var = find_variable(name);
    if (var) {
        // Validación estricta de tipo
        if (var->is_number != is_number) {
            printf("\033[31m⛔ Error crítico: Conflicto de tipos al asignar a la variable '%s'. ", name);
            if (var->is_number) {
                printf("La variable fue definida como número y se intenta asignar texto.\033[0m\n");
            } else {
                printf("La variable fue definida como texto y se intenta asignar un número.\033[0m\n");
            }
            exit(1);
        }
        // Advertencia de sobrescritura
        printf("\033[33m📝 Advertencia: La variable '%s' ya existía y será sobrescrita.\033[0m\n", name);
        char respuesta[10];
        while (1) {
            printf("¿Desea continuar? (y/n): ");
            if (fgets(respuesta, sizeof(respuesta), stdin) != NULL) {
                // Eliminar salto de línea
                size_t len = strlen(respuesta);
                if (len > 0 && respuesta[len-1] == '\n') respuesta[len-1] = '\0';
                if (strcmp(respuesta, "y") == 0 || strcmp(respuesta, "Y") == 0) {
                    break; // Continuar
                } else if (strcmp(respuesta, "n") == 0 || strcmp(respuesta, "N") == 0) {
                    printf("\033[31m⛔ Ejecución abortada por el usuario.\033[0m\n");
                    exit(1);
                } else {
                    printf("Por favor, responda 'y' para continuar o 'n' para abortar.\n");
                }
            }
        }
        // Actualizar variable existente
        strncpy(var->value, value, sizeof(var->value) - 1);
        var->value[sizeof(var->value) - 1] = '\0'; // Asegurar null-terminator
        var->is_number = is_number;
    } else if (num_variables < MAX_VARIABLES) {
        // Crear nueva variable
        strncpy(variables[num_variables].name, name, sizeof(variables[num_variables].name) - 1);
        variables[num_variables].name[sizeof(variables[num_variables].name) - 1] = '\0';
        strncpy(variables[num_variables].value, value, sizeof(variables[num_variables].value) - 1);
        variables[num_variables].value[sizeof(variables[num_variables].value) - 1] = '\0';
        variables[num_variables].is_number = is_number;
        num_variables++;
    }
}

// Obtener el valor de una variable
const char* get_variable_value(const char* name) {
    Variable* var = find_variable(name);
    return var ? var->value : NULL;
}

// Verificar si una variable es número
int is_variable_number(const char* name) {
    Variable* var = find_variable(name);
    return var ? var->is_number : 0;
}

// Verificar si un string es un número
int is_string_number(const char* str) {
    if (!str || strlen(str) == 0) return 0;
    
    // Verificar si todos los caracteres son dígitos
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

// Función principal del interpreter
void interpret_ast(ASTNode* node) {
    if (!node) return;
    
    // Según el tipo de nodo, llamamos a la función específica
    switch (node->type) {
        case NODE_PROGRAM:
            interpret_program(node);
            break;
        case NODE_DECLARATION:
            interpret_declaration(node);
            break;
        case NODE_ASSIGNMENT:
            interpret_assignment(node);
            break;
        case NODE_IDENTIFIER:
            interpret_identifier(node);
            break;
        case NODE_NUMBER:
            interpret_number(node);
            break;
        case NODE_STRING:
            interpret_string(node);
            break;
        case NODE_GPU_COMMAND:
            interpret_gpu_command(node);
            break;
        case NODE_RUN_COMMAND:
            interpret_run_command(node);
            break;
    }
}

// Interpretar un programa (nodo raíz)
void interpret_program(ASTNode* node) {
    printf("Ejecutando programa...\n");
    
    // Ejecutar todos los hijos del programa
    for (int i = 0; i < node->num_children; i++) {
        interpret_ast(node->children[i]);
    }
}

// Interpretar una declaración (ej: "modo: quiet" o "power_limit: mi_potencia")
void interpret_declaration(ASTNode* node) {
    if (node->num_children > 0 && node->children[0]) {
        char* value = node->children[0]->value;
        NodeType value_type = node->children[0]->type;
        
        // Primero validar si el valor es válido para el tipo de declaración
        if (strcmp(node->value, "mode") == 0 || strcmp(node->value, "modo") == 0) {
            // Para modos, validar directamente si es un modo válido
            int es_valido = 0;
            for (int i = 0; i < num_modos; i++) {
                if (strcmp(value, modos_validos[i]) == 0) {
                    es_valido = 1;
                    break;
                }
            }
            if (!es_valido) {
                const char* sugerido = sugerir_palabra(value, modos_validos, num_modos, 2);
                if (sugerido) {
                    printf("\033[33mSugerencia: ¿Quisiste decir: %s?\033[0m\n", sugerido);
                } else {
                    printf("Modo desconocido: %s\n", value);
                }
                return; // No continuar si el modo no es válido
            }
            strncpy(gpu_mode, value, sizeof(gpu_mode) - 1);
            gpu_mode[sizeof(gpu_mode) - 1] = '\0'; // Asegurar null-terminator
            printf("\033[36mModo GPU cambiado a: %s\033[0m\n", gpu_mode);
        }
        else if (strcmp(node->value, "dynamic_boost") == 0) {
            // Para dynamic_boost, verificar si es número o variable numérica
            if (value_type == NODE_IDENTIFIER) {
                const char* var_value = get_variable_value(value);
                if (var_value) {
                    value = (char*)var_value;
                    if (is_variable_number(node->children[0]->value)) {
                        value_type = NODE_NUMBER;
                    } else {
                        printf("\033[33mError: 'dynamic_boost' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
                        return;
                    }
                } else {
                    printf("\033[31m⛔ Error crítico: La variable '%s' no está definida. Ejecución abortada.\033[0m\n", value);
                    exit(1);
                }
            }
            
            if (value_type == NODE_NUMBER) {
                int val = atoi(value);
                if (val < 0 || val > 1) {
                    printf("\033[31m⛔ Error crítico: 'dynamic_boost' fuera de rango (0-1). Valor recibido: %d. Ejecución abortada.\033[0m\n", val);
                    exit(1);
                }
                printf("\033[36mDynamic Boost establecido a: %d\033[0m\n", val);
            } else {
                printf("\033[33mError: 'dynamic_boost' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
            }
        }
        else if (strcmp(node->value, "cpu_max_perf") == 0) {
            // Para cpu_max_perf, verificar si es número o variable numérica
            if (value_type == NODE_IDENTIFIER) {
                const char* var_value = get_variable_value(value);
                if (var_value) {
                    value = (char*)var_value;
                    if (is_variable_number(node->children[0]->value)) {
                        value_type = NODE_NUMBER;
                    } else {
                        printf("\033[33mError: 'cpu_max_perf' debe ser un número (0-100), no '%s'. Revisa el valor asignado.\033[0m\n", value);
                        return;
                    }
                } else {
                    printf("\033[31m⛔ Error crítico: La variable '%s' no está definida. Ejecución abortada.\033[0m\n", value);
                    exit(1);
                }
            }
            
            if (value_type == NODE_NUMBER) {
                int val = atoi(value);
                if (val < 0 || val > 100) {
                    printf("\033[31m⛔ Error crítico: 'cpu_max_perf' fuera de rango (0-100). Valor recibido: %d. Ejecución abortada.\033[0m\n", val);
                    exit(1);
                }
                printf("\033[36mCPU Max Performance establecido a: %d%%\033[0m\n", val);
            } else {
                printf("\033[33mError: 'cpu_max_perf' debe ser un número (0-100), no '%s'. Revisa el valor asignado.\033[0m\n", value);
            }
        }
        else if (strcmp(node->value, "cpu_min_perf") == 0) {
            // Para cpu_min_perf, verificar si es número o variable numérica
            if (value_type == NODE_IDENTIFIER) {
                const char* var_value = get_variable_value(value);
                if (var_value) {
                    value = (char*)var_value;
                    if (is_variable_number(node->children[0]->value)) {
                        value_type = NODE_NUMBER;
                    } else {
                        printf("\033[33mError: 'cpu_min_perf' debe ser un número (0-100), no '%s'. Revisa el valor asignado.\033[0m\n", value);
                        return;
                    }
                } else {
                    printf("\033[31m⛔ Error crítico: La variable '%s' no está definida. Ejecución abortada.\033[0m\n", value);
                    exit(1);
                }
            }
            
            if (value_type == NODE_NUMBER) {
                int val = atoi(value);
                if (val < 0 || val > 100) {
                    printf("\033[31m⛔ Error crítico: 'cpu_min_perf' fuera de rango (0-100). Valor recibido: %d. Ejecución abortada.\033[0m\n", val);
                    exit(1);
                }
                printf("\033[36mCPU Min Performance establecido a: %d%%\033[0m\n", val);
            } else {
                printf("\033[33mError: 'cpu_min_perf' debe ser un número (0-100), no '%s'. Revisa el valor asignado.\033[0m\n", value);
            }
        }
        else if (strcmp(node->value, "turbo_boost") == 0) {
            // Para turbo_boost, verificar si es número o variable numérica
            if (value_type == NODE_IDENTIFIER) {
                const char* var_value = get_variable_value(value);
                if (var_value) {
                    value = (char*)var_value;
                    if (is_variable_number(node->children[0]->value)) {
                        value_type = NODE_NUMBER;
                    } else {
                        printf("\033[33mError: 'turbo_boost' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
                        return;
                    }
                } else {
                    printf("\033[31m⛔ Error crítico: La variable '%s' no está definida. Ejecución abortada.\033[0m\n", value);
                    exit(1);
                }
            }
            
            if (value_type == NODE_NUMBER) {
                int val = atoi(value);
                if (val < 0 || val > 1) {
                    printf("\033[31m⛔ Error crítico: 'turbo_boost' fuera de rango (0-1). Valor recibido: %d. Ejecución abortada.\033[0m\n", val);
                    exit(1);
                }
                printf("\033[36mTurbo Boost establecido a: %d\033[0m\n", val);
            } else {
                printf("\033[33mError: 'turbo_boost' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
            }
        }
        else if (strcmp(node->value, "persist_mode") == 0) {
            // Para persist_mode, verificar si es número o variable numérica
            if (value_type == NODE_IDENTIFIER) {
                const char* var_value = get_variable_value(value);
                if (var_value) {
                    value = (char*)var_value;
                    if (is_variable_number(node->children[0]->value)) {
                        value_type = NODE_NUMBER;
                    } else {
                        printf("\033[33mError: 'persist_mode' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
                    return;
                    }
                } else {
                    printf("\033[31m⛔ Error crítico: La variable '%s' no está definida. Ejecución abortada.\033[0m\n", value);
                    exit(1);
                }
            }
            
            if (value_type == NODE_NUMBER) {
                int val = atoi(value);
                if (val < 0 || val > 1) {
                    printf("\033[31m⛔ Error crítico: 'persist_mode' fuera de rango (0-1). Valor recibido: %d. Ejecución abortada.\033[0m\n", val);
                    exit(1);
                }
                printf("\033[36mPersistence Mode establecido a: %d\033[0m\n", val);
            } else {
                printf("\033[33mError: 'persist_mode' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
            }
        }
        else if (strcmp(node->value, "battery_conservation") == 0) {
            // Para battery_conservation, verificar si es número o variable numérica
            if (value_type == NODE_IDENTIFIER) {
                const char* var_value = get_variable_value(value);
                if (var_value) {
                    value = (char*)var_value;
                    if (is_variable_number(node->children[0]->value)) {
                        value_type = NODE_NUMBER;
                    } else {
                        printf("\033[33mError: 'battery_conservation' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
                    return;
                }
                } else {
                    printf("\033[31m⛔ Error crítico: La variable '%s' no está definida. Ejecución abortada.\033[0m\n", value);
                    exit(1);
                }
            }
            
            if (value_type == NODE_NUMBER) {
                int val = atoi(value);
                if (val < 0 || val > 1) {
                    printf("\033[31m⛔ Error crítico: 'battery_conservation' fuera de rango (0-1). Valor recibido: %d. Ejecución abortada.\033[0m\n", val);
                    exit(1);
                }
                printf("\033[36mBattery Conservation establecido a: %d\033[0m\n", val);
            } else {
                printf("\033[33mError: 'battery_conservation' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
            }
        }
        else if (strcmp(node->value, "fnlock") == 0) {
            // Para fnlock, verificar si es número o variable numérica
            if (value_type == NODE_IDENTIFIER) {
                const char* var_value = get_variable_value(value);
                if (var_value) {
                    value = (char*)var_value;
                    if (is_variable_number(node->children[0]->value)) {
                        value_type = NODE_NUMBER;
                    } else {
                        printf("\033[33mError: 'fnlock' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
                        return;
                    }
                } else {
                    printf("\033[31m⛔ Error crítico: La variable '%s' no está definida. Ejecución abortada.\033[0m\n", value);
                    exit(1);
                }
            }
            
            if (value_type == NODE_NUMBER) {
                int val = atoi(value);
                if (val < 0 || val > 1) {
                    printf("\033[31m⛔ Error crítico: 'fnlock' fuera de rango (0-1). Valor recibido: %d. Ejecución abortada.\033[0m\n", val);
                    exit(1);
                }
                printf("\033[36mFnLock establecido a: %d\033[0m\n", val);
            } else {
                printf("\033[33mError: 'fnlock' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
            }
        }
        else {
            // Parámetro desconocido, usar fuzzy match
            const char* sugerido = sugerir_palabra(node->value, parametros_validos, num_parametros, 2);
            if (sugerido) {
                printf("\033[33m💡 ¿Quisiste decir: %s?\033[0m\n", sugerido);
            } else {
                printf("\033[31m⛔ Error crítico: Parámetro desconocido: %s. Ejecución abortada.\033[0m\n", node->value);
                exit(1);
            }
        }
    }
}

// Interpretar una asignación (ej: "mi_potencia = 80")
void interpret_assignment(ASTNode* node) {
    if (node->num_children > 0 && node->children[0]) {
        char* value = node->children[0]->value;
        NodeType value_type = node->children[0]->type;
        
        // Si el valor es un identificador, buscar la variable
        if (value_type == NODE_IDENTIFIER) {
            const char* var_value = get_variable_value(value);
            if (var_value) {
                value = (char*)var_value;
                if (is_variable_number(node->children[0]->value)) {
                    value_type = NODE_NUMBER;
                } else {
                    value_type = NODE_STRING;
                }
            } else {
                printf("\033[31m⛔ Error crítico: La variable '%s' no está definida. Ejecución abortada.\033[0m\n", value);
                exit(1);
            }
        }
        // Guardar la variable
        set_variable(node->value, value, (value_type == NODE_NUMBER));
        printf("📝 Variable '%s' asignada a: %s\n", node->value, value);
    }
}

// Interpretar un identificador
void interpret_identifier(ASTNode* node) {
    printf("Identificador: %s\n", node->value);
}

// Interpretar un número
void interpret_number(ASTNode* node) {
    printf("🔢 Número: %s\n", node->value);
}

// Interpretar un string
void interpret_string(ASTNode* node) {
    printf("📄 String: %s\n", node->value);
}

// Interpretar un comando del sistema
void interpret_gpu_command(ASTNode* node) {
// Fuzzy match para comandos del sistema
    int es_valido = 0;
    const char* comando_a_ejecutar = node->value;
    
    for (int i = 0; i < num_comandos_gpu; i++) {
        if (strcmp(node->value, comandos_gpu_validos[i]) == 0) {
            es_valido = 1;
            break;
        }
    }
    
    if (!es_valido) {
        const char* sugerido = sugerir_palabra(node->value, comandos_gpu_validos, num_comandos_gpu, 2);
        if (sugerido) {
            printf("\033[33mSugerencia: ¿Quisiste decir: %s?\033[0m\n", sugerido);
            comando_a_ejecutar = sugerido; // Usar el comando sugerido
        } else {
            printf("Comando del sistema desconocido: %s\n", node->value);
            return;
        }
    }
    
    // Ejecutar el comando (original o sugerido)
    if (strcmp(comando_a_ejecutar, "status") == 0) {
        printf("\033[36mEstado actual del sistema:\033[0m\n");
        
        // Obtener información de GPU
        char* gpu_info = execute_system_command("nvidia-smi --query-gpu=name,power.draw,temperature.gpu,clocks.current.graphics --format=csv,noheader,nounits 2>/dev/null");
        if (gpu_info) {
            printf("   GPU: %s", gpu_info);
            free(gpu_info);
        } else {
            printf("   Advertencia: GPU: No se pudo obtener información\033[0m\n");
        }
        
        // Obtener información de CPU
        char* cpu_info = execute_system_command("cat /proc/cpuinfo | grep 'model name' | head -1 | cut -d':' -f2 | sed 's/^[ \t]*//'");
        if (cpu_info) {
            printf("   CPU: %s", cpu_info);
            free(cpu_info);
        }
        
        // Obtener información de memoria
        char* mem_info = execute_system_command("free -h | grep '^Mem:' | awk '{print \"Memoria: \" $2 \" total, \" $3 \" usado, \" $4 \" libre\"}'");
        if (mem_info) {
            printf("   %s", mem_info);
            free(mem_info);
        }
        
        // Obtener información de parámetros del sistema
        char* cpu_max = execute_system_command("cat /sys/devices/system/cpu/intel_pstate/max_perf_pct 2>/dev/null");
        char* cpu_min = execute_system_command("cat /sys/devices/system/cpu/intel_pstate/min_perf_pct 2>/dev/null");
        char* dynamic_boost = execute_system_command("cat /sys/devices/system/cpu/intel_pstate/hwp_dynamic_boost 2>/dev/null");
        char* turbo_boost = execute_system_command("cat /sys/devices/system/cpu/intel_pstate/no_turbo 2>/dev/null");
        char* battery_status = execute_system_command("cat /sys/class/power_supply/AC/online 2>/dev/null");
        
        if (cpu_max && cpu_min && dynamic_boost && turbo_boost && battery_status) {
            printf("   CPU Max Performance: %s%%\n", cpu_max);
            printf("   CPU Min Performance: %s%%\n", cpu_min);
            printf("   Dynamic Boost: %s\n", strcmp(dynamic_boost, "1") == 0 ? "ON" : "OFF");
            printf("   Turbo Boost: %s\n", strcmp(turbo_boost, "1") == 0 ? "OFF" : "ON");
            printf("   Estado de batería: %s\n", strcmp(battery_status, "1") == 0 ? "Enchufada" : "Con batería");
            
            free(cpu_max);
            free(cpu_min);
            free(dynamic_boost);
            free(turbo_boost);
            free(battery_status);
        } else {
            printf("   Advertencia: CPU/Sistema: No se pudo obtener información completa\033[0m\n");
            if (cpu_max) free(cpu_max);
            if (cpu_min) free(cpu_min);
            if (dynamic_boost) free(dynamic_boost);
            if (turbo_boost) free(turbo_boost);
            if (battery_status) free(battery_status);
        }
    }
    else if (strcmp(comando_a_ejecutar, "reset") == 0) {
        printf("\033[36m🔄 GPU reseteada a configuración por defecto\033[0m\n");
    }
    else if (strcmp(comando_a_ejecutar, "-") == 0) {
        // El guión "-" indica configuración, no hace nada por sí solo
    }
    else if (strcmp(comando_a_ejecutar, "#") == 0) {
        // Los comentarios no hacen nada
    }
    else if (strcmp(comando_a_ejecutar, "hola") == 0) {
        printf("\033[36m👋 ¡Hola! Bienvenido al controlador de GPU\033[0m\n");
    }
    else if (strcmp(comando_a_ejecutar, "mundo") == 0) {
        printf("\033[36m🌍 ¡Hola mundo desde GLX!\033[0m\n");
    }
    else if (strcmp(comando_a_ejecutar, "vars") == 0) {
        printf("\033[36m📋 Variables definidas:\n");
        if (num_variables == 0) {
            printf("   (ninguna variable definida)\033[0m\n");
        } else {
            for (int i = 0; i < num_variables; i++) {
                printf("   %s = %s (%s)\n", 
                       variables[i].name, 
                       variables[i].value,
                       variables[i].is_number ? "número" : "texto");
            }
            printf("\033[0m");
        }
    }
    else if (strcmp(comando_a_ejecutar, "help") == 0) {
        printf("\033[36m📚 Comandos disponibles:\n");
        printf("   status - Mostrar estado del sistema\n");
        printf("   reset - Resetear a valores por defecto\n");
        printf("   vars - Mostrar variables definidas\n");
        printf("   run mode: [quiet/balanced/performance] - Aplicar modo\n");
        printf("   dynamic_boost: [0/1] - Activar/desactivar Dynamic Boost\n");
        printf("   cpu_max_perf: [0-100] - Rendimiento máximo de CPU\n");
        printf("   cpu_min_perf: [0-100] - Rendimiento mínimo de CPU\n");
        printf("   turbo_boost: [0/1] - Activar/desactivar Turbo Boost\n");
        printf("   persist_mode: [0/1] - Activar/desactivar Persistence Mode\n");
        printf("   battery_conservation: [0/1] - Activar/desactivar conservación de batería\n");
        printf("   fnlock: [0/1] - Activar/desactivar FnLock\n");
        printf("   variable = valor - Definir una variable\033[0m\n");
    }
}

// Interpretar un comando de ejecución (ej: "run mode: quiet")
void interpret_run_command(ASTNode* node) {
    if (node->num_children > 0 && node->children[0]) {
        char* value = node->children[0]->value;
        NodeType value_type = node->children[0]->type;
        
        // Si el valor es un identificador, verificar si es una variable o un modo literal
        if (value_type == NODE_IDENTIFIER) {
            const char* var_value = get_variable_value(value);
            if (var_value) {
                // Es una variable definida
                value = (char*)var_value;
                if (is_variable_number(node->children[0]->value)) {
                    value_type = NODE_NUMBER;
                } else {
                    value_type = NODE_STRING;
                }
            }
            // Si no es una variable definida, tratar como valor literal del modo
        }

        // Validar si el valor es un modo válido
        int es_valido = 0;
        for (int i = 0; i < num_modos; i++) {
            if (strcmp(value, modos_validos[i]) == 0) {
                es_valido = 1;
                break;
            }
        }

        if (!es_valido) {
            const char* sugerido = sugerir_palabra(value, modos_validos, num_modos, 2);
            if (sugerido) {
                printf("\033[33m💡 ¿Quisiste decir: %s?\033[0m\n", sugerido);
                // Usar el modo sugerido para la validación
                // Validar el modo sugerido
                int sugerido_valido = 0;
                for (int i = 0; i < num_modos; i++) {
                    if (strcmp(sugerido, modos_validos[i]) == 0) {
                        sugerido_valido = 1;
                        break;
                    }
                }
                if (sugerido_valido) {
                    printf("\033[36mAplicando modo sugerido: %s\033[0m\n", sugerido);
                    value = (char*)sugerido;
                } else {
                    printf("Modo de ejecución desconocido: %s\n", value);
                    return;
                }
            } else {
                printf("Modo de ejecución desconocido: %s\n", value);
                return;
            }
        }

        // Cargar configuraciones desde modelo.txt
        printf("\033[36mCargando configuración para modo: %s\033[0m\n", value);
        
        int num_modes;
        // Buscar modelo.txt en ubicaciones del sistema
        char modelo_path[512];
        
        // Primero intentar en la ubicación del sistema
        strcpy(modelo_path, "/usr/local/share/glx/modelo.txt");
        
        // Verificar si existe
        FILE* test_file = fopen(modelo_path, "r");
        if (!test_file) {
            // Si no existe en /usr/local/share/glx/, intentar en la ubicación del ejecutable
            char exec_path[512];
            ssize_t len = readlink("/proc/self/exe", exec_path, sizeof(exec_path) - 1);
            if (len != -1) {
                exec_path[len] = '\0';
                char* last_slash = strrchr(exec_path, '/');
                if (last_slash) {
                    *last_slash = '\0';
                    snprintf(modelo_path, sizeof(modelo_path), "%s/../../modelo.txt", exec_path);
                } else {
                    strcpy(modelo_path, "modelo.txt");
                }
            } else {
                strcpy(modelo_path, "modelo.txt");
            }
        } else {
            fclose(test_file);
        }
        
        GPU_Mode* modes = load_gpu_modes(modelo_path, &num_modes);
        
        if (!modes) {
            printf("\033[31m❌ Error: No se pudo cargar modelo.txt\033[0m\n");
            return;
        }
        
        // Buscar el modo específico
        GPU_Mode* target_mode = NULL;
        for (int i = 0; i < num_modes; i++) {
            if (strcmp(modes[i].name, value) == 0) {
                target_mode = &modes[i];
                break;
            }
        }
        
        if (!target_mode) {
            printf("\033[31m❌ Error: Modo '%s' no encontrado en modelo.txt\033[0m\n", value);
            free(modes);
            return;
        }
        
        // Aplicar configuraciones
        printf("\033[36mAplicando configuraciones del sistema...\033[0m\n");
        
        // Dynamic Boost
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /sys/devices/system/cpu/intel_pstate/hwp_dynamic_boost", target_mode->dynamic_boost);
        char* result = execute_system_command(cmd);
        if (result) {
            printf("   Dynamic Boost: %d\033[0m\n", target_mode->dynamic_boost);
            free(result);
        } else {
            printf("   Advertencia: Dynamic Boost: Error al aplicar\033[0m\n");
        }
        
        // CPU Max Performance
        snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /sys/devices/system/cpu/intel_pstate/max_perf_pct", target_mode->cpu_max_perf);
        result = execute_system_command(cmd);
        if (result) {
            printf("   CPU Max Performance: %d%%\033[0m\n", target_mode->cpu_max_perf);
            free(result);
        } else {
            printf("   Advertencia: CPU Max Performance: Error al aplicar\033[0m\n");
        }
        
        // CPU Min Performance
        snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /sys/devices/system/cpu/intel_pstate/min_perf_pct", target_mode->cpu_min_perf);
        result = execute_system_command(cmd);
        if (result) {
            printf("   CPU Min Performance: %d%%\033[0m\n", target_mode->cpu_min_perf);
            free(result);
        } else {
            printf("   Advertencia: CPU Min Performance: Error al aplicar\033[0m\n");
        }
        
        // Turbo Boost
        snprintf(cmd, sizeof(cmd), "echo %d | sudo tee /sys/devices/system/cpu/intel_pstate/no_turbo", target_mode->turbo_boost);
        result = execute_system_command(cmd);
        if (result) {
            printf("   Turbo Boost: %s\033[0m\n", target_mode->turbo_boost ? "OFF" : "ON");
            free(result);
        } else {
            printf("   Advertencia: Turbo Boost: Error al aplicar\033[0m\n");
        }
        
        // Persistence Mode
        snprintf(cmd, sizeof(cmd), "sudo nvidia-smi -pm %d", target_mode->persist_mode);
        result = execute_system_command(cmd);
        if (result) {
            printf("   Persistence Mode: %s\033[0m\n", target_mode->persist_mode ? "ON" : "OFF");
            free(result);
        } else {
            printf("   Advertencia: Persistence Mode: Error al aplicar\033[0m\n");
        }
        
        // Battery Conservation
        snprintf(cmd, sizeof(cmd), "sudo legion_cli --donotexpecthwmon batteryconservation-%s", target_mode->battery_conservation ? "enable" : "disable");
        result = execute_system_command(cmd);
        if (result) {
            printf("   Battery Conservation: %s\033[0m\n", target_mode->battery_conservation ? "ON" : "OFF");
            free(result);
        } else {
            printf("   Advertencia: Battery Conservation: Error al aplicar\033[0m\n");
        }
        
        // FnLock
        snprintf(cmd, sizeof(cmd), "sudo legion_cli --donotexpecthwmon fnlock-%s", target_mode->fnlock ? "enable" : "disable");
        result = execute_system_command(cmd);
        if (result) {
            printf("   FnLock: %s\033[0m\n", target_mode->fnlock ? "ON" : "OFF");
            free(result);
        } else {
            printf("   Advertencia: FnLock: Error al aplicar\033[0m\n");
        }
        
        // RGB Control
        if (strlen(target_mode->rgb_color) > 0) {
            set_rgb_color(target_mode->rgb_color, target_mode->rgb_brightness);
        }
        
        printf("\033[36mModo '%s' aplicado exitosamente!\033[0m\n", value);
        
        free(modes);
    }
}

// Llama a esta función cuando detectes un identificador desconocido
// tipo: 0 = modo, 1 = parámetro, 2 = comando CLI
void manejar_identificador_desconocido(const char* palabra, int tipo) {
    const char* sugerido = NULL;
    if (tipo == 0) {
        sugerido = sugerir_palabra(palabra, modos_validos, num_modos, 2);
    } else if (tipo == 1) {
        sugerido = sugerir_palabra(palabra, parametros_validos, num_parametros, 2);
    } else if (tipo == 2) {
        sugerido = sugerir_palabra(palabra, comandos_gpu_validos, num_comandos_gpu, 2);
    }
    if (sugerido) {
        printf("\033[33m💡 ¿Quisiste decir: %s?\033[0m\n", sugerido); // Sugerencia en amarillo
    } else {
        printf("Identificador desconocido: %s\n", palabra);
    }
}

// Listar todas las variables definidas
void list_variables() {
    printf("\n📋 Variables definidas:\n");
    if (num_variables == 0) {
        printf("  (No hay variables definidas)\n");
        return;
    }
    for (int i = 0; i < num_variables; i++) {
        printf("  - %s = %s  [%s]\n", variables[i].name, variables[i].value, variables[i].is_number ? "número" : "string");
    }
}

