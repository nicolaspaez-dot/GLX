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
