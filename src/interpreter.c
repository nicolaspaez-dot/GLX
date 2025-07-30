#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/interpreter.h"
#include "utils.h"

// Variables globales para simular el estado de la GPU
static int gpu_power_limit = 100;
static int gpu_fan_speed = 50;
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
        if (strcmp(node->value, "mode") == 0) {
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
                    printf("¿Quisiste decir: %s?\n", sugerido);
                } else {
                    printf("Modo desconocido: %s\n", value);
                }
                return; // No continuar si el modo no es válido
            }
            strncpy(gpu_mode, value, sizeof(gpu_mode) - 1);
            gpu_mode[sizeof(gpu_mode) - 1] = '\0'; // Asegurar null-terminator
            printf("\033[36m✅ Modo GPU cambiado a: %s\033[0m\n", gpu_mode);
        }
        else if (strcmp(node->value, "power_limit") == 0 || strcmp(node->value, "power_limi") == 0) {
            // Fuzzy match para power_limit
            if (strcmp(node->value, "power_limi") == 0) {
                printf("¿Quisiste decir: power_limit?\n");
            }
            
            // Para power_limit, verificar si es número o variable numérica
            if (value_type == NODE_IDENTIFIER) {
                const char* var_value = get_variable_value(value);
                if (var_value) {
                    value = (char*)var_value;
                    if (is_variable_number(node->children[0]->value)) {
                        value_type = NODE_NUMBER;
                    } else {
                        printf("\033[33m⚠️  Error: 'power_limit' debe ser un número, no '%s'. Revisa el valor asignado.\033[0m\n", value);
                        return;
                    }
                } else {
                    printf("\033[31m⛔ Error crítico: La variable '%s' no está definida. Ejecución abortada.\033[0m\n", value);
                    exit(1);
                }
            }
            
            if (value_type == NODE_NUMBER) {
                int val = atoi(value);
                if (val < 0 || val > 150) {
                    printf("\033[31m⛔ Error crítico: 'power_limit' fuera de rango (0-150). Valor recibido: %d. Ejecución abortada.\033[0m\n", val);
                    exit(1);
                }
                gpu_power_limit = val;
                printf("\033[36m✅ Límite de potencia GPU establecido a: %d%%\033[0m\n", gpu_power_limit);
            } else {
                printf("\033[33m⚠️  Error: 'power_limit' debe ser un número, no '%s'. Revisa el valor asignado.\033[0m\n", value);
            }
        }
        else if (strcmp(node->value, "fan_speed") == 0 || strcmp(node->value, "fan_spped") == 0) {
            // Fuzzy match para fan_speed
            if (strcmp(node->value, "fan_spped") == 0) {
                printf("¿Quisiste decir: fan_speed?\n");
            }
            
            // Para fan_speed, verificar si es número o variable numérica
            if (value_type == NODE_IDENTIFIER) {
                const char* var_value = get_variable_value(value);
                if (var_value) {
                    value = (char*)var_value;
                    if (is_variable_number(node->children[0]->value)) {
                        value_type = NODE_NUMBER;
                    } else {
                        printf("\033[33m⚠️  Error: 'fan_speed' debe ser un número, no '%s'. Revisa el valor asignado.\033[0m\n", value);
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
                    printf("\033[31m⛔ Error crítico: 'fan_speed' fuera de rango (0-100). Valor recibido: %d. Ejecución abortada.\033[0m\n", val);
                    exit(1);
                }
                gpu_fan_speed = val;
                printf("\033[36m✅ Velocidad del ventilador GPU establecida a: %d%%\033[0m\n", gpu_fan_speed);
            } else {
                printf("\033[33m⚠️  Error: 'fan_speed' debe ser un número, no '%s'. Revisa el valor asignado.\033[0m\n", value);
            }
        }
        else if (strcmp(node->value, "clocks") == 0) {
            // Para clocks, verificar si es número o variable numérica
            if (value_type == NODE_IDENTIFIER) {
                const char* var_value = get_variable_value(value);
                if (var_value) {
                    value = (char*)var_value;
                    if (is_variable_number(node->children[0]->value)) {
                        value_type = NODE_NUMBER;
                    } else {
                        printf("\033[33m⚠️  Error: 'clocks' debe ser un número, no '%s'. Revisa el valor asignado.\033[0m\n", value);
                        return;
                    }
                } else {
                    printf("\033[31m⛔ Error crítico: La variable '%s' no está definida. Ejecución abortada.\033[0m\n", value);
                    exit(1);
                }
            }
            
            if (value_type == NODE_NUMBER) {
                int val = atoi(value);
                if (val < 0 || val > 3000) {
                    printf("\033[31m⛔ Error crítico: 'clocks' fuera de rango (0-3000). Valor recibido: %d. Ejecución abortada.\033[0m\n", val);
                    exit(1);
                }
                printf("\033[36m✅ Frecuencia de reloj GPU establecida a: %d MHz\033[0m\n", val);
            } else {
                printf("\033[33m⚠️  Error: 'clocks' debe ser un número, no '%s'. Revisa el valor asignado.\033[0m\n", value);
            }
        }
        else if (strcmp(node->value, "persist") == 0) {
            // Para persist mode, verificar si es string o variable
            if (value_type == NODE_IDENTIFIER) {
                // Verificar si es un valor válido directamente
                if (strcmp(value, "on") == 0) {
                    printf("\033[36m✅ Modo persistente GPU activado\033[0m\n");
                    return;
                } else if (strcmp(value, "off") == 0) {
                    printf("\033[36m✅ Modo persistente GPU desactivado\033[0m\n");
                    return;
                }
                
                // Si no es on/off, buscar como variable
                const char* var_value = get_variable_value(value);
                if (var_value) {
                    value = (char*)var_value;
                } else {
                    printf("\033[31m⛔ Error crítico: La variable '%s' no está definida. Ejecución abortada.\033[0m\n", value);
                    exit(1);
                }
            }
            
            if (strcmp(value, "on") == 0) {
                printf("\033[36m✅ Modo persistente GPU activado\033[0m\n");
            } else if (strcmp(value, "off") == 0) {
                printf("\033[36m✅ Modo persistente GPU desactivado\033[0m\n");
            } else {
                printf("\033[33m⚠️  Error: 'persist mode' debe ser 'on' o 'off', no '%s'. Revisa el valor asignado.\033[0m\n", value);
            }
        }
        else {
            // Fuzzy match para parámetros desconocidos
            const char* sugerido = sugerir_palabra(node->value, parametros_validos, num_parametros, 2);
            if (sugerido) {
                printf("¿Quisiste decir: %s?\n", sugerido);
            } else {
                printf("Parámetro desconocido: %s\n", node->value);
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

// Interpretar un comando GPU
void interpret_gpu_command(ASTNode* node) {
    // Fuzzy match para comandos GPU
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
            printf("¿Quisiste decir: %s?\n", sugerido);
            comando_a_ejecutar = sugerido; // Usar el comando sugerido
        } else {
            printf("Comando GPU desconocido: %s\n", node->value);
            return;
        }
    }
    
    // Ejecutar el comando (original o sugerido)
    if (strcmp(comando_a_ejecutar, "status") == 0) {
        printf("\033[36m📊 Estado actual de la GPU:\n");
        printf("   Modo: %s\n", gpu_mode);
        printf("   Límite de potencia: %d%%\n", gpu_power_limit);
        printf("   Velocidad del ventilador: %d%%\033[0m\n", gpu_fan_speed);
    }
    else if (strcmp(comando_a_ejecutar, "reset") == 0) {
        gpu_power_limit = 100;
        gpu_fan_speed = 50;
        strncpy(gpu_mode, "normal", sizeof(gpu_mode) - 1);
        gpu_mode[sizeof(gpu_mode) - 1] = '\0'; // Asegurar null-terminator
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
        printf("   status - Mostrar estado de la GPU\n");
        printf("   reset - Resetear a valores por defecto\n");
        printf("   vars - Mostrar variables definidas\n");
        printf("   mode: [quiet/balanced/performance] - Cambiar modo\n");
        printf("   power_limit: [número] - Establecer límite de potencia\n");
        printf("   fan_speed: [número] - Establecer velocidad del ventilador\n");
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
        printf("¿Quisiste decir: %s?\n", sugerido); // Sugerencia simple
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

