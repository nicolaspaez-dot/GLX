#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/interpreter.h"
#include "../include/utils.h"

// Función auxiliar para imprimir el AST
void print_ast(ASTNode* node, int depth) {
    if (!node) return;

    // Imprimir indentación
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    // Imprimir tipo de nodo y valor
    printf("- Type: ");
    switch (node->type) {
        case NODE_PROGRAM: printf("PROGRAM"); break;
        case NODE_DECLARATION: printf("DECLARATION"); break;
        case NODE_ASSIGNMENT: printf("ASSIGNMENT"); break;
        case NODE_IDENTIFIER: printf("IDENTIFIER"); break;
        case NODE_NUMBER: printf("NUMBER"); break;
        case NODE_STRING: printf("STRING"); break;
        case NODE_GPU_COMMAND: printf("GPU_COMMAND"); break;
        case NODE_RUN_COMMAND: printf("RUN_COMMAND"); break;
    }
    if (node->value) {
        printf(", Value: %s", node->value);
    }
    printf("\n");

    // Imprimir nodos hijos
    for (int i = 0; i < node->num_children; i++) {
        print_ast(node->children[i], depth + 1);
    }
}

int main(int argc, char* argv[]) {
    FILE* archivo;
    char linea[256];
    const char* nombre_archivo = "gx_programs/ejemplo.gx";
    
    // Verificar si se pasó el comando help
    if (argc > 1 && strcmp(argv[1], "help") == 0) {
        printf("\033[36m📚 GLX - Controlador de GPU\n");
        printf("Uso: gx [comando] [archivo.gx]\n\n");
        printf("Comandos disponibles:\n");
        printf("  help                    - Mostrar esta ayuda\n");
        printf("  status                  - Mostrar estado de la GPU\n");
        printf("  reset                   - Resetear GPU a valores por defecto\n");
        printf("  vars                    - Mostrar variables definidas\n\n");
        printf("Parámetros de GPU:\n");
        printf("  run mode: [quiet/balanced/performance] - Aplicar modo\n");
        printf("  dynamic_boost: [0/1]    - Activar/desactivar Dynamic Boost\n");
        printf("  cpu_max_perf: [0-100]   - Rendimiento máximo de CPU\n");
        printf("  cpu_min_perf: [0-100]   - Rendimiento mínimo de CPU\n");
        printf("  turbo_boost: [0/1]      - Activar/desactivar Turbo Boost\n");
        printf("  persist_mode: [0/1]     - Activar/desactivar Persistence Mode\n");
        printf("  battery_conservation: [0/1] - Activar/desactivar conservación de batería\n");
        printf("  fnlock: [0/1]           - Activar/desactivar FnLock\n\n");
        printf("Variables:\n");
        printf("  variable = valor        - Definir una variable\n\n");
        printf("Ejemplos:\n");
        printf("  gx help                 - Mostrar esta ayuda\n");
        printf("  gx status               - Mostrar estado actual\n");
        printf("  gx archivo.gx           - Ejecutar archivo GLX\033[0m\n");
        return 0;
    }
    
    // Verificar si se pasó el comando status
    if (argc > 1 && strcmp(argv[1], "status") == 0) {
        printf("\033[36m📊 Estado actual del sistema:\033[0m\n");
        
        // Información de GPU
        char* gpu_info = execute_system_command("nvidia-smi --query-gpu=name,power.draw,fan.speed,temperature.gpu,clocks.current.graphics,power.limit --format=csv,noheader,nounits");
        
        if (gpu_info) {
            printf("   🎮 GPU: %s\033[0m\n", gpu_info);
            free(gpu_info);
        } else {
            printf("   ⚠️  GPU: No se pudo obtener información\033[0m\n");
        }
        
        // Información de CPU y sistema
        char* cpu_max = execute_system_command("cat /sys/devices/system/cpu/intel_pstate/max_perf_pct");
        char* cpu_min = execute_system_command("cat /sys/devices/system/cpu/intel_pstate/min_perf_pct");
        char* dynamic_boost = execute_system_command("cat /sys/devices/system/cpu/intel_pstate/hwp_dynamic_boost");
        char* turbo_boost = execute_system_command("cat /sys/devices/system/cpu/intel_pstate/no_turbo");
        char* battery_status = execute_system_command("cat /sys/class/power_supply/ACAD/online");
        
        if (cpu_max && cpu_min && dynamic_boost && turbo_boost && battery_status) {
            // Eliminar saltos de línea
            cpu_max[strcspn(cpu_max, "\n")] = 0;
            cpu_min[strcspn(cpu_min, "\n")] = 0;
            dynamic_boost[strcspn(dynamic_boost, "\n")] = 0;
            turbo_boost[strcspn(turbo_boost, "\n")] = 0;
            battery_status[strcspn(battery_status, "\n")] = 0;
            
            printf("   🖥️  CPU Max Performance: %s%%\n", cpu_max);
            printf("   🖥️  CPU Min Performance: %s%%\n", cpu_min);
            printf("   ⚡ Dynamic Boost: %s\n", strcmp(dynamic_boost, "1") == 0 ? "ON" : "OFF");
            printf("   🚀 Turbo Boost: %s\n", strcmp(turbo_boost, "1") == 0 ? "OFF" : "ON");
            printf("   🔋 Estado de batería: %s\n", strcmp(battery_status, "1") == 0 ? "Enchufada" : "Con batería");
            
            free(cpu_max);
            free(cpu_min);
            free(dynamic_boost);
            free(turbo_boost);
            free(battery_status);
        } else {
            printf("   ⚠️  CPU/Sistema: No se pudo obtener información completa\033[0m\n");
            if (cpu_max) free(cpu_max);
            if (cpu_min) free(cpu_min);
            if (dynamic_boost) free(dynamic_boost);
            if (turbo_boost) free(turbo_boost);
            if (battery_status) free(battery_status);
        }
        
        return 0;
    }
    
    // Verificar si se pasó el comando reset
    if (argc > 1 && strcmp(argv[1], "reset") == 0) {
        printf("\033[36m🔄 GPU reseteada a configuración por defecto\033[0m\n");
        return 0;
    }
    
    // Verificar si se pasó el comando vars
    if (argc > 1 && strcmp(argv[1], "vars") == 0) {
        printf("\033[36m📋 Variables definidas:\n");
        printf("   (ninguna variable definida)\033[0m\n");
        return 0;
    }
    
    // Verificar si se pasó el comando run
    if (argc > 1 && strcmp(argv[1], "run") == 0) {
        if (argc < 3) {
            printf("\033[31m❌ Error: Uso: gx run mode:[quiet/balanced/performance]\033[0m\n");
            return 1;
        }
        
        // Verificar que el segundo argumento sea "mode:X"
        if (strncmp(argv[2], "mode:", 5) != 0) {
            printf("\033[31m❌ Error: Uso: gx run mode:[quiet/balanced/performance]\033[0m\n");
            return 1;
        }
        
        // Crear un comando temporal para procesar
        char temp_command[256];
        snprintf(temp_command, sizeof(temp_command), "run %s", argv[2]);
        
        // Tokenizar el comando
        int cantidad_tokens = 0;
        char** tokens = lexer_tokenize(temp_command, &cantidad_tokens);
        
        // Parsear y ejecutar
        ASTNode* ast = parser_parse(tokens, cantidad_tokens);
        interpret_ast(ast);
        
        // Limpieza
        parser_free_ast(ast);
        liberar_tokens(tokens, cantidad_tokens);
        
        return 0;
    }
    
    if (argc > 1) {
        nombre_archivo = argv[1];
    } else {
        printf("[INFO] No se especificó archivo .gx, usando por defecto: %s\n", nombre_archivo);
    }

    archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) {
        perror("No se pudo abrir el archivo");
        return 1;
    }

    while (fgets(linea, sizeof(linea), archivo)) {
        // Ignorar líneas vacías y comentarios
        char* ptr = linea;
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        if (*ptr == '#' || *ptr == '\0' || *ptr == '\n') {
            continue;
        }

        printf("\nProcesando línea: %s", linea);

        // Fase 1: Lexer
        int cantidad_tokens = 0;
        char** tokens = lexer_tokenize(linea, &cantidad_tokens);

        printf("Tokens encontrados:\n");
        for (int i = 0; i < cantidad_tokens; i++) {
            printf("  Token[%d]: %s\n", i, tokens[i]);
        }

        // Fase 2: Parser
        printf("\nÁrbol de sintaxis abstracta (AST):\n");
        ASTNode* ast = parser_parse(tokens, cantidad_tokens);
        print_ast(ast, 0);

        // Fase 3: Interpreter
        printf("\n🎯 Ejecutando comando:\n");
        interpret_ast(ast);

        // Limpieza
        parser_free_ast(ast);
        liberar_tokens(tokens, cantidad_tokens);
    }

    fclose(archivo);
    return 0;
}
