#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
    }
}

// Interpretar un programa (nodo raíz)
void interpret_program(ASTNode* node) {
    printf("🚀 Ejecutando programa...\n");
    
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
                    printf("\033[33m💡 ¿Quisiste decir: %s?\033[0m\n", sugerido);
                } else {
                    printf("Modo desconocido: %s\n", value);
                }
                return; // No continuar si el modo no es válido
            }
            strncpy(gpu_mode, value, sizeof(gpu_mode) - 1);
            gpu_mode[sizeof(gpu_mode) - 1] = '\0'; // Asegurar null-terminator
            printf("\033[36m✅ Modo GPU cambiado a: %s\033[0m\n", gpu_mode);
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
                        printf("\033[33m⚠️  Error: 'dynamic_boost' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
                printf("\033[36m✅ Dynamic Boost establecido a: %d\033[0m\n", val);
            } else {
                printf("\033[33m⚠️  Error: 'dynamic_boost' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
                        printf("\033[33m⚠️  Error: 'cpu_max_perf' debe ser un número (0-100), no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
                printf("\033[36m✅ CPU Max Performance establecido a: %d%%\033[0m\n", val);
            } else {
                printf("\033[33m⚠️  Error: 'cpu_max_perf' debe ser un número (0-100), no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
                        printf("\033[33m⚠️  Error: 'cpu_min_perf' debe ser un número (0-100), no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
                printf("\033[36m✅ CPU Min Performance establecido a: %d%%\033[0m\n", val);
            } else {
                printf("\033[33m⚠️  Error: 'cpu_min_perf' debe ser un número (0-100), no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
                        printf("\033[33m⚠️  Error: 'turbo_boost' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
                printf("\033[36m✅ Turbo Boost establecido a: %d\033[0m\n", val);
            } else {
                printf("\033[33m⚠️  Error: 'turbo_boost' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
                        printf("\033[33m⚠️  Error: 'persist_mode' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
                printf("\033[36m✅ Persistence Mode establecido a: %d\033[0m\n", val);
            } else {
                printf("\033[33m⚠️  Error: 'persist_mode' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
                        printf("\033[33m⚠️  Error: 'battery_conservation' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
                printf("\033[36m✅ Battery Conservation establecido a: %d\033[0m\n", val);
            } else {
                printf("\033[33m⚠️  Error: 'battery_conservation' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
                        printf("\033[33m⚠️  Error: 'fnlock' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
                printf("\033[36m✅ FnLock establecido a: %d\033[0m\n", val);
            } else {
                printf("\033[33m⚠️  Error: 'fnlock' debe ser un número (0 o 1), no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
    printf("🏷️  Identificador: %s\n", node->value);
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
            printf("\033[33m💡 ¿Quisiste decir: %s?\033[0m\n", sugerido);
            comando_a_ejecutar = sugerido; // Usar el comando sugerido
        } else {
            printf("Comando del sistema desconocido: %s\n", node->value);
            return;
        }
    }
    
    // Ejecutar el comando (original o sugerido)
    if (strcmp(comando_a_ejecutar, "status") == 0) {
        printf("\033[36m📊 Estado actual de la GPU:\n");
        
        // Ejecutar nvidia-smi para obtener información real
        char* gpu_info = execute_system_command("nvidia-smi --query-gpu=name,power.draw,fan.speed,temperature.gpu,clocks.current.graphics,power.limit --format=csv,noheader,nounits");
        
        if (gpu_info) {
            printf("   %s\033[0m\n", gpu_info);
            free(gpu_info);
        } else {
            printf("   Error: No se pudo obtener información de la GPU\033[0m\n");
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

