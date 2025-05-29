#ifndef PARSER_H
#define PARSER_H

// Tipos de nodos del AST
typedef enum {
    NODE_PROGRAM,      // Programa completo
    NODE_DECLARATION,  // Declaración de variable
    NODE_ASSIGNMENT,   // Asignación
    NODE_IDENTIFIER,   // Identificador/variable
    NODE_NUMBER,       // Número literal
    NODE_STRING,       // String literal
    NODE_GPU_COMMAND   // Comando específico para GPU
} NodeType;

// Estructura para un nodo del AST
typedef struct ASTNode {
    NodeType type;
    char* value;       // Valor del nodo (si aplica)
    struct ASTNode** children;  // Array de nodos hijos
    int num_children;  // Cantidad de nodos hijos
} ASTNode;

// Estructura para el parser
typedef struct {
    char** tokens;     // Array de tokens
    int num_tokens;    // Cantidad de tokens
    int current_pos;   // Posición actual en el array de tokens
} Parser;

// Funciones principales del parser
ASTNode* parser_parse(char** tokens, int num_tokens);
void parser_free_ast(ASTNode* node);

// Funciones auxiliares para crear y manipular nodos
ASTNode* create_node(NodeType type, const char* value);
void add_child(ASTNode* parent, ASTNode* child);

#endif
