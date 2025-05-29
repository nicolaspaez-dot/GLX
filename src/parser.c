#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// Parsear una declaración o asignación
ASTNode* parse_statement(Parser* parser) {
    char* token = current_token(parser);
    if (!token) return NULL;

    // Por ahora, asumimos que cualquier token seguido de ":" es una declaración
    char* next_token = (parser->current_pos + 1 < parser->num_tokens) ? 
                      parser->tokens[parser->current_pos + 1] : NULL;
    
    if (next_token && strcmp(next_token, ":") == 0) {
        // Es una declaración
        ASTNode* node = create_node(NODE_DECLARATION, token);
        advance_token(parser); // Consumir identificador
        advance_token(parser); // Consumir ":"
        if (parser->current_pos < parser->num_tokens) {
            ASTNode* value_node = create_node(NODE_IDENTIFIER, parser->tokens[parser->current_pos]);
            add_child(node, value_node);
            advance_token(parser);
        }
        return node;
    }

    // Por ahora, cualquier otro token lo tratamos como un comando GPU
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
