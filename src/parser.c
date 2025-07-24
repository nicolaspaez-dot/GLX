#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/parser.h"

// Crear un nuevo nodo del AST
ASTNode* create_node(NodeType type, const char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->children = NULL;
    node->num_children = 0;
    return node;
}

// Agregar un hijo a un nodo
void add_child(ASTNode* parent, ASTNode* child) {
    parent->children = realloc(parent->children, (parent->num_children + 1) * sizeof(ASTNode*));
    parent->children[parent->num_children] = child;
    parent->num_children++;
}

// Inicializar el parser
Parser* init_parser(char** tokens, int num_tokens) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    parser->tokens = tokens;
    parser->num_tokens = num_tokens;
    parser->current_pos = 0;
    return parser;
}

// Obtener el token actual
char* current_token(Parser* parser) {
    if (parser->current_pos >= parser->num_tokens) return NULL;
    return parser->tokens[parser->current_pos];
}

// Avanzar al siguiente token
void advance_token(Parser* parser) {
    parser->current_pos++;
}

// Verificar si un string es un número
int is_number(const char* str) {
    if (!str || strlen(str) == 0) return 0;
    
    // Verificar si todos los caracteres son dígitos
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

// Crear un nodo del tipo correcto basado en el valor
ASTNode* create_value_node(const char* value) {
    if (is_number(value)) {
        return create_node(NODE_NUMBER, value);
    } else if (value && value[0] == '"' && value[strlen(value)-1] == '"' && strlen(value) >= 2) {
        // Si empieza y termina con comillas dobles, es un string
        // Crear el string sin las comillas
        char* sin_comillas = (char*)malloc(strlen(value) - 1);
        strncpy(sin_comillas, value + 1, strlen(value) - 2);
        sin_comillas[strlen(value) - 2] = '\0';
        ASTNode* node = create_node(NODE_STRING, sin_comillas);
        free(sin_comillas);
        return node;
    } else {
        // Si no es número ni string, es un identificador (variable)
        return create_node(NODE_IDENTIFIER, value);
    }
}

// Parsear una declaración o asignación
ASTNode* parse_statement(Parser* parser) {
    char* token = current_token(parser);
    if (!token) return NULL;

    // Ignorar comentarios (líneas que empiecen con #)
    if (strcmp(token, "#") == 0) {
        // Saltar toda la línea (todos los tokens restantes)
        while (parser->current_pos < parser->num_tokens) {
            advance_token(parser);
        }
        return NULL; // No crear nodo para comentarios
    }

    // Verificar si es una declaración (token seguido de ":")
    char* next_token = (parser->current_pos + 1 < parser->num_tokens) ? 
                      parser->tokens[parser->current_pos + 1] : NULL;
    
    if (next_token && strcmp(next_token, ":") == 0) {
        // Es una declaración
        ASTNode* node = create_node(NODE_DECLARATION, token);
        advance_token(parser); // Consumir identificador
        advance_token(parser); // Consumir ":"
        
        if (parser->current_pos < parser->num_tokens) {
            char* value = parser->tokens[parser->current_pos];
            ASTNode* value_node = create_value_node(value);
            add_child(node, value_node);
            advance_token(parser);
        }
        return node;
    }

    // Verificar si es una asignación (token seguido de "=")
    if (next_token && strcmp(next_token, "=") == 0) {
        // Es una asignación de variable
        ASTNode* node = create_node(NODE_ASSIGNMENT, token);
        advance_token(parser); // Consumir nombre de variable
        advance_token(parser); // Consumir "="
        
        if (parser->current_pos < parser->num_tokens) {
            char* value = parser->tokens[parser->current_pos];
            ASTNode* value_node = create_value_node(value);
            add_child(node, value_node);
            advance_token(parser);
        }
        return node;
    }

    // Verificar si es un número
    if (is_number(token)) {
        ASTNode* node = create_node(NODE_NUMBER, token);
        advance_token(parser);
        return node;
    }

    // Por defecto, tratar como comando GPU
    ASTNode* node = create_node(NODE_GPU_COMMAND, token);
    advance_token(parser);
    return node;
}

// Función principal de parsing
ASTNode* parser_parse(char** tokens, int num_tokens) {
    Parser* parser = init_parser(tokens, num_tokens);
    ASTNode* root = create_node(NODE_PROGRAM, NULL);

    // Parsear cada statement
    while (parser->current_pos < parser->num_tokens) {
        ASTNode* statement = parse_statement(parser);
        if (statement) {
            add_child(root, statement);
        }
    }

    free(parser);
    return root;
}

// Liberar la memoria del AST
void parser_free_ast(ASTNode* node) {
    if (!node) return;

    for (int i = 0; i < node->num_children; i++) {
        parser_free_ast(node->children[i]);
    }

    free(node->value);
    free(node->children);
    free(node);
}
