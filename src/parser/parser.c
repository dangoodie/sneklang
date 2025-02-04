#include "parser.h"
#include <stdlib.h>
#include <string.h>

ast_node_t *ast_new_node(ast_node_type_t type) {
  ast_node_t *node = calloc(1, sizeof(ast_node_t));
  if (node == NULL) {
    return NULL;
  }

  node->type = type;
  return node;
}

ast_node_t *ast_new_literal_node(int value) {
  ast_node_t *node = ast_new_node(NODE_LITERAL);
  if (node == NULL) {
    return NULL;
  }

  node->literal.value = value;
  return node;
}

ast_node_t *ast_new_binary_op_node(char op, ast_node_t *left, ast_node_t *right) {
  ast_node_t *node = ast_new_node(NODE_BINARY_OP);
  if (node == NULL) {
    return NULL;
  }

  node->binary_op.op = op;
  node->binary_op.left = left;
  node->binary_op.right = right;
  return node;
}

ast_node_t *ast_new_unary_op_node(char op, ast_node_t *operand) {
  ast_node_t *node = ast_new_node(NODE_UNARY_OP);
  if (node == NULL) {
    return NULL;
  }

  node->unary_op.op = op;
  node->unary_op.operand = operand;
  return node;
}

ast_node_t *ast_new_variable_node(char *name, ast_node_t *value) {
  ast_node_t *node = ast_new_node(NODE_VARIABLE);
  if (node == NULL) {
    return NULL;
  }

  node->variable.name = strdup(name);
  node->variable.value = value;
  return node;
}

ast_node_t *ast_new_assignment_node(char *name, ast_node_t *value) {
  ast_node_t *node = ast_new_node(NODE_ASSIGNMENT);
  if (node == NULL) {
    return NULL;
  }

  node->assignment.name = strdup(name);
  node->assignment.value = value;
  return node;
}

ast_node_t *ast_new_declaration_node(char *name, char *type, ast_node_t *value) {
  ast_node_t *node = ast_new_node(NODE_DECLARATION);
  if (node == NULL) {
    return NULL;
  }

  node->declaration.name = strdup(name);
  node->declaration.type = strdup(type);
  node->declaration.value = value;
  return node;
}

void ast_free_node(ast_node_t *node) {
  if (node == NULL) {
    return;
  }

  switch (node->type) {
    case NODE_LITERAL:
      break;
    case NODE_BINARY_OP:
      ast_free_node(node->binary_op.left);
      ast_free_node(node->binary_op.right);
      break;
    case NODE_UNARY_OP:
      ast_free_node(node->unary_op.operand);
      break;
    case NODE_VARIABLE:
      free(node->variable.name);
      ast_free_node(node->variable.value);
      break;
    case NODE_ASSIGNMENT:
      free(node->assignment.name);
      ast_free_node(node->assignment.value);
      break;
    case NODE_DECLARATION:
      free(node->declaration.name);
      free(node->declaration.type);
      ast_free_node(node->declaration.value);
      break;
  }

  free(node);
}
