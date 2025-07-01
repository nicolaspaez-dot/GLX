#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"

// Función principal del interpreter
// Recibe un nodo del AST y lo ejecuta
void interpret_ast(ASTNode* node);

// Funciones específicas para cada tipo de nodo
void interpret_program(ASTNode* node);
void interpret_declaration(ASTNode* node);
void interpret_assignment(ASTNode* node);
void interpret_identifier(ASTNode* node);
void interpret_number(ASTNode* node);
void interpret_string(ASTNode* node);
void interpret_gpu_command(ASTNode* node);

#endif // INTERPRETER_H
