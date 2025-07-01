#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/interpreter.h"

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
        // Actualizar variable existente
        strcpy(var->value, value);
        var->is_number = is_number;
    } else if (num_variables < MAX_VARIABLES) {
        // Crear nueva variable
        strcpy(variables[num_variables].name, name);
        strcpy(variables[num_variables].value, value);
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
        
        // Si el valor es un identificador, buscar la variable
        if (value_type == NODE_IDENTIFIER) {
            const char* var_value = get_variable_value(value);
            if (var_value) {
                value = (char*)var_value; // Usar el valor de la variable
                // Verificar si la variable es numérica usando el nombre original
                if (is_variable_number(node->children[0]->value)) {
                    value_type = NODE_NUMBER;
                } else {
                    value_type = NODE_STRING;
                }
            } else {
                printf("❌ Error: Variable '%s' no definida\n", value);
                return;
            }
        }
        
        if (strcmp(node->value, "modo") == 0) {
            strcpy(gpu_mode, value);
            printf("✅ Modo GPU cambiado a: %s\n", gpu_mode);
        }
        else if (strcmp(node->value, "power_limit") == 0) {
            if (value_type == NODE_NUMBER) {
                gpu_power_limit = atoi(value);
                printf("✅ Límite de potencia GPU establecido a: %d%%\n", gpu_power_limit);
            } else {
                printf("❌ Error: power_limit debe ser un número, no '%s'\n", value);
            }
        }
        else if (strcmp(node->value, "fan_speed") == 0) {
            if (value_type == NODE_NUMBER) {
                gpu_fan_speed = atoi(value);
                printf("✅ Velocidad del ventilador GPU establecida a: %d%%\n", gpu_fan_speed);
            } else {
                printf("❌ Error: fan_speed debe ser un número, no '%s'\n", value);
            }
        }
        else {
            printf("📝 Variable desconocida: %s = %s\n", node->value, value);
        }
    }
}

// Interpretar una asignación (ej: "mi_potencia = 80")
void interpret_assignment(ASTNode* node) {
    if (node->num_children > 0 && node->children[0]) {
        char* value = node->children[0]->value;
        NodeType value_type = node->children[0]->type;
        
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
    if (strcmp(node->value, "status") == 0) {
        printf("📊 Estado actual de la GPU:\n");
        printf("   Modo: %s\n", gpu_mode);
        printf("   Límite de potencia: %d%%\n", gpu_power_limit);
        printf("   Velocidad del ventilador: %d%%\n", gpu_fan_speed);
    }
    else if (strcmp(node->value, "reset") == 0) {
        gpu_power_limit = 100;
        gpu_fan_speed = 50;
        strcpy(gpu_mode, "normal");
        printf("🔄 GPU reseteada a configuración por defecto\n");
    }
    else if (strcmp(node->value, "-") == 0) {
        // El guión "-" indica configuración, no hace nada por sí solo
    }
    else if (strcmp(node->value, "#") == 0) {
        // Los comentarios no hacen nada
    }
    else if (strcmp(node->value, "hola") == 0) {
        printf("👋 ¡Hola! Bienvenido al controlador de GPU\n");
    }
    else if (strcmp(node->value, "mundo") == 0) {
        printf("🌍 ¡Hola mundo desde GLX!\n");
    }
    else if (strcmp(node->value, "vars") == 0) {
        printf("📋 Variables definidas:\n");
        if (num_variables == 0) {
            printf("   (ninguna variable definida)\n");
        } else {
            for (int i = 0; i < num_variables; i++) {
                printf("   %s = %s (%s)\n", 
                       variables[i].name, 
                       variables[i].value,
                       variables[i].is_number ? "número" : "texto");
            }
        }
    }
    else if (strcmp(node->value, "help") == 0) {
        printf("📚 Comandos disponibles:\n");
        printf("   status - Mostrar estado de la GPU\n");
        printf("   reset - Resetear a valores por defecto\n");
        printf("   vars - Mostrar variables definidas\n");
        printf("   modo: [quiet/performance/normal] - Cambiar modo\n");
        printf("   power_limit: [número] - Establecer límite de potencia\n");
        printf("   fan_speed: [número] - Establecer velocidad del ventilador\n");
        printf("   variable = valor - Definir una variable\n");
    }
    else {
        printf("❌ Comando GPU desconocido: %s (escribe 'help' para ver comandos disponibles)\n", node->value);
    }
}
