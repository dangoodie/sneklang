#include "parser.h"
#include "../lexer/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

parser_t *parser_new(lexer_t *lexer) {
  parser_t *parser = malloc(sizeof(parser_t));
  if (parser == NULL) {
    return NULL;
  }

  parser->lexer = lexer;
  parser->root = malloc(sizeof(ast_root_t));
  if (parser->root == NULL) {
    free(parser);
    return NULL;
  }

  parser->root->nodes = malloc(sizeof(ast_node_t *) * MAX_NODES);
  if (parser->root->nodes == NULL) {
    free(parser->root);
    free(parser);
    return NULL;
  }
  parser->root->count = 0;
  parser_advance(parser);

  return parser;
}

void parser_free(parser_t *parser) {
  if (parser == NULL) {
    return;
  }

  for (int i = 0; i < parser->root->count; i++) {
    ast_free_node(parser->root->nodes[i]);
  }

  free(parser->root->nodes);
  free(parser->root);
  free(parser);
}

void parser_advance(parser_t *parser) {
  if (parser->current && parser->current->type == TOKEN_EOF) {
    return;
  }

  token_t *next = lexer_next_token(parser->lexer);
  if (next == NULL) {
    fprintf(stderr, "Parser Error: Unexpected end of file\n");
    return;
  }
  parser->current = next;
}

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

ast_node_t *ast_new_binary_op_node(char op, ast_node_t *left,
                                   ast_node_t *right) {
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

ast_node_t *ast_new_variable_node(char *name) {
  ast_node_t *node = ast_new_node(NODE_VARIABLE);
  if (node == NULL) {
    return NULL;
  }

  node->variable.name = strdup(name);
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

ast_node_t *ast_new_declaration_node(char *name, char *type,
                                     ast_node_t *value) {
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

ast_root_t *parse_root(parser_t *parser) {
  ast_root_t *root = parser->root;

  while (parser->current && parser->current->type != TOKEN_EOF) {
    // Skip empty lines
    while (parser->current && parser->current->type == TOKEN_EOL) {
      parser_advance(parser);
    }

    // Break if we reach the end of the file
    if (parser->current->type == TOKEN_EOF) {
      break;
    }

    ast_node_t *node = parse_statement(parser);
    if (node == NULL) {
      printf("Parser Error: Statement return NULL\n");
      return NULL;
    }
    root->nodes[root->count++] = node;
  }
  return root;
}

ast_node_t *parse_statement(parser_t *parser) {
  token_t *token = parser->current;
  ast_node_t *node = NULL;
  switch (token->type) {
    //  case TOKEN_DEF:
    //    return parse_function(parser);
  case TOKEN_IDENTIFIER:
    node = parse_assignment(parser);
    break;
    // case TOKEN_CLASS:
    //  return parse_class(parser);
    // case TOKEN_IMPORT:
    //  return parse_import(parser);
  default:
    node = parse_expression(parser);
  }

  if (parser->current && parser->current->type == TOKEN_EOL) {
    parser_advance(parser); // Consume the EOL token
  }

  return node;
}

ast_node_t *parse_assignment(parser_t *parser) {
  token_t *identifier = parser->current;

  if (identifier->type != TOKEN_IDENTIFIER) {
    fprintf(stderr, "Parser Error: Expected an identifier on line %d\n",
            identifier->line);
    return NULL;
  }

  parser_advance(parser); // Consume the identifier
  token_t *next = parser->current;

  if (!next) {
    fprintf(stderr, "Parser Error: Unexpected end of file after identifier.\n");
    return NULL;
  }

  // **Check for Variable Declaration (x: int = value)**
  if (next->type == TOKEN_COLON) {
    parser_advance(parser); // Consume `:`
    token_t *type_token = parser->current;

    if (!type_token) {
      fprintf(stderr,
              "Parser Error: Expected type identifier after ':' on line %d\n",
              type_token ? type_token->line : -1);
      return NULL;
    }

    parser_advance(parser); // Consume type identifier

    // **Check if there's an '=' (declaration with assignment)**
    ast_node_t *value = NULL;
    if (parser->current && parser->current->type == TOKEN_EQUAL) {
      parser_advance(parser); // Consume `=`
      value = parse_expression(parser);
      if (!value) {
        fprintf(stderr,
                "Parser Error: Invalid expression after '=' on line %d\n",
                parser->current->line);
        return NULL;
      }
    }

    return ast_new_declaration_node(strdup(identifier->lexeme),
                                    strdup(type_token->lexeme), value);
  }

  // **Check for Variable Assignment (x = value)**
  if (next->type == TOKEN_EQUAL) {
    parser_advance(parser); // Consume `=`
    ast_node_t *value = parse_expression(parser);
    if (!value) {
      fprintf(stderr, "Parser Error: Invalid expression after '=' on line %d\n",
              parser->current->line);
      return NULL;
    }

    return ast_new_assignment_node(strdup(identifier->lexeme), value);
  }

  // **If neither `:` nor `=` follows, it's an error**
  fprintf(
      stderr,
      "Parser Error: Unexpected token '%s' after identifier '%s' on line %d\n",
      next->lexeme, identifier->lexeme, next->line);
  return NULL;
}

ast_node_t *parse_expression(parser_t *parser) {
  ast_node_t *node = parse_term(parser);

  while (parser->current->type == TOKEN_PLUS ||
         parser->current->type == TOKEN_MINUS) {
    token_t *op = parser->current;
    parser_advance(parser); // Consume the operator
    ast_node_t *right = parse_term(parser);
    if (right == NULL) {
      return NULL;
    }

    if (!op->lexeme) {
      fprintf(stderr, "Parser Error: Expected operator on line %d\n", op->line);
      return NULL;
    }

    ast_node_t *new_node = ast_new_binary_op_node(op->lexeme[0], node, right);
    if (new_node == NULL) {
      return NULL;
    }

    node = new_node;
  }

  return node;
}

ast_node_t *parse_term(parser_t *parser) {
  ast_node_t *node = parse_factor(parser);

  while (parser->current->type == TOKEN_STAR ||
         parser->current->type == TOKEN_SLASH) {
    token_t *op = parser->current;
    parser_advance(parser); // Consume the operator
    ast_node_t *right = parse_factor(parser);
    if (right == NULL) {
      return NULL;
    }

    ast_node_t *new_node = ast_new_binary_op_node(op->lexeme[0], node, right);
    if (new_node == NULL) {
      return NULL;
    }

    node = new_node;
  }

  return node;
}

ast_node_t *parse_factor(parser_t *parser) {
  token_t *token = parser->current;
  switch (token->type) {
  case TOKEN_INT:
    parser_advance(parser); // Consume the integer
    return ast_new_literal_node(token->integer);
  case TOKEN_LPAREN: {
    parser_advance(parser); // Consume the '('
    ast_node_t *node = parse_expression(parser);
    if (node == NULL) {
      return NULL;
    }

    if (!parser->current || parser->current->type != TOKEN_RPAREN) {
      fprintf(stderr, "Parser Error: Expected ')' on line %d\n",
              parser->current->line);
      return NULL;
    }
    parser_advance(parser); // Consume the ')'
    return node;
  }
  case TOKEN_MINUS:
  case TOKEN_BANG: {
    token_t *op = parser->current;
    parser_advance(parser); // Consume the operator
    ast_node_t *operand = parse_factor(parser);
    if (operand == NULL) {
      return NULL;
    }

    return ast_new_unary_op_node(op->lexeme[0], operand);
  }
  case TOKEN_IDENTIFIER:
    return ast_new_variable_node(token->lexeme);
  default:
    fprintf(stderr, "Parser Error: Unexpected token on line %d\n", token->line);
    return NULL;
  }
}

void print_ast(ast_node_t *node, int depth, int is_last, int branch_stack[]) {
    if (!node)
        return;

    // Print the vertical structure correctly
    for (int i = 0; i < depth - 1; i++) {
        if (branch_stack[i]) {
            printf("│   ");  // Draw vertical lines for continuing branches
        } else {
            printf("    ");  // Add space for alignment
        }
    }

    // Draw the correct branch (`├──` for middle nodes, `└──` for last node)
    if (depth > 0) {
        printf(is_last ? "└── " : "├── ");
    }

    // Print node type
    switch (node->type) {
    case NODE_LITERAL:
        printf("Literal: %d\n", node->literal.value);
        break;
    case NODE_BINARY_OP:
        printf("Binary Op: %c\n", node->binary_op.op);
        branch_stack[depth] = !is_last;  // Mark if we need a vertical line
        print_ast(node->binary_op.left, depth + 1, 0, branch_stack);
        print_ast(node->binary_op.right, depth + 1, 1, branch_stack);
        break;
    case NODE_UNARY_OP:
        printf("Unary Op: %c\n", node->unary_op.op);
        print_ast(node->unary_op.operand, depth + 1, 1, branch_stack);
        break;
    case NODE_VARIABLE:
        printf("Variable: %s\n", node->variable.name);
        break;
    case NODE_ASSIGNMENT:
        printf("Assignment: %s\n", node->assignment.name);
        print_ast(node->assignment.value, depth + 1, 1, branch_stack);
        break;
    case NODE_DECLARATION:
        printf("Declaration: %s : %s\n", node->declaration.name, node->declaration.type);
        if (node->declaration.value) {
            print_ast(node->declaration.value, depth + 1, 1, branch_stack);
        }
        break;
    }
}

// **Wrapper Function to Initialize Vertical Line Tracking**
void print_ast_tree(ast_node_t *root) {
    int branch_stack[100] = {0};  // Track active vertical lines
    print_ast(root, 0, 0, branch_stack);
}

