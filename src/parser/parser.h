#pragma once
#include "../lexer/lexer.h"

#define MAX_NODES 1024

typedef enum {
  NODE_LITERAL,
  NODE_BINARY_OP,
  NODE_UNARY_OP,
  NODE_VARIABLE,
  NODE_ASSIGNMENT,
  NODE_DECLARATION,
  // NODE_FUNCTION
} ast_node_type_t;

typedef struct ASTNode {
  ast_node_type_t type;
  union {
    struct {
      int value;
    } literal;
    struct {
      char op;
      struct ASTNode *left;
      struct ASTNode *right;
    } binary_op;
    struct {
      char op;
      struct ASTNode *operand;
    } unary_op;
    struct {
      char *name;
    } variable;
    struct {
      char *name;
      struct ASTNode *value;
    } assignment;
    struct {
      char *name;
      char *type;
      struct ASTNode *value;
    } declaration;
  };
} ast_node_t;

typedef struct ASTRoot {
  ast_node_t **nodes;
  int count;
} ast_root_t;

typedef struct Parser {
  lexer_t *lexer;
  ast_root_t *root;
  token_t *current;
} parser_t;

// Memory management
ast_node_t *ast_new_node(ast_node_type_t type);
ast_node_t *ast_new_literal_node(int value);
ast_node_t *ast_new_binary_op_node(char op, ast_node_t *left,
                                   ast_node_t *right);
ast_node_t *ast_new_unary_op_node(char op, ast_node_t *operand);
ast_node_t *ast_new_variable_node(char *name);
ast_node_t *ast_new_assignment_node(char *name, ast_node_t *value);
ast_node_t *ast_new_declaration_node(char *name, char *type, ast_node_t *value);
void ast_free_node(ast_node_t *node);

// Parser function prototypes
parser_t *parser_new(lexer_t *lexer);
void parser_free(parser_t *parser);
void parser_advance(parser_t *parser);
ast_root_t *parse_root(parser_t *parser);
ast_node_t *parse_statement(parser_t *parser);
ast_node_t *parse_expression(parser_t *parser);
ast_node_t *parse_assignment(parser_t *parser);
ast_node_t *parse_declaration(parser_t *parser);
ast_node_t *parse_primary(parser_t *parser);
ast_node_t *parse_unary(parser_t *parser);
ast_node_t *parse_term(parser_t *parser);
ast_node_t *parse_factor(parser_t *parser);

// Debugging
void print_ast(ast_node_t *node, int depth, int is_right, int *branch_stack);
void print_ast_tree(ast_node_t *root);
