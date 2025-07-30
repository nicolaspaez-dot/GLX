#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/interpreter.h"

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
        printf("  mode: [quiet/balanced/performance] - Cambiar modo\n");
        printf("  power_limit: [0-150]    - Establecer límite de potencia\n");
        printf("  fan_speed: [0-100]      - Establecer velocidad del ventilador\n");
        printf("  clocks: [0-3000]        - Establecer frecuencia de reloj\n");
        printf("  persist: [on/off]       - Activar/desactivar modo persistente\n\n");
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
        printf("\033[36m📊 Estado actual de la GPU:\n");
        printf("   Modo: normal\n");
        printf("   Límite de potencia: 100%%\n");
        printf("   Velocidad del ventilador: 50%%\033[0m\n");
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
