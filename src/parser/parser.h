#include "../lexer/lexer.h"

typedef enum {
  NODE_LITERAL,
  NODE_BINARY_OP,
  NODE_UNARY_OP,
  NODE_VARIABLE,
  NODE_ASSIGNMENT,
  NODE_DECLARATION,
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
      struct ASTNode *value;
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

// Memory management
ast_node_t *ast_new_node(ast_node_type_t type);
ast_node_t *ast_new_literal_node(int value);
ast_node_t *ast_new_binary_op_node(char op, ast_node_t *left,
                                   ast_node_t *right);
ast_node_t *ast_new_unary_op_node(char op, ast_node_t *operand);
ast_node_t *ast_new_variable_node(char *name, ast_node_t *value);
ast_node_t *ast_new_assignment_node(char *name, ast_node_t *value);
ast_node_t *ast_new_declaration_node(char *name, char *type, ast_node_t *value);
void ast_free_node(ast_node_t *node);

// Parser function prototypes
ast_node_t *parse_expression(lexer_t *lexer);
ast_node_t *parse_primary(lexer_t *lexer);
ast_node_t *parse_term(lexer_t *lexer);
ast_node_t *parse_factor(lexer_t *lexer);
ast_node_t *parse_declaration(lexer_t *lexer);
ast_node_t *parse_assignment(lexer_t *lexer);
ast_node_t *parse_statement(lexer_t *lexer);
