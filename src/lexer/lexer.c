#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create a new lexer instance
lexer_t *lexer_new(char *src) {
  lexer_t *lexer = malloc(sizeof(lexer_t));
  if (!lexer) {
    fprintf(stderr, "Lexer Error: Failed to allocate memory\n");
    exit(1);
  }
  lexer->start = src;
  lexer->current = src;
  lexer->line = 1;

  // Initialize the indentation stack
  lexer->indent_top = 0;
  lexer->indent_stack[lexer->indent_top] = 0;
  lexer->is_new_line = 1;

  // Initialize the keyword table
  lexer->keywords = keyword_table_new();
  init_keywords(lexer);

  return lexer;
}

// Free the lexer instance
void lexer_free(lexer_t *lexer) {
  if (lexer) {
    keyword_table_free(lexer->keywords);
    free(lexer);
  }
}

// Advance and return the next character in the source
char lexer_advance(lexer_t *lexer) {
  if (*lexer->current == '\0')
    return '\0';
  return *(lexer->current++);
}

// Peek at the current character without consuming it
char lexer_peek(lexer_t *lexer) {
  return (*lexer->current == '\0') ? '\0' : *(lexer->current);
}

// Skip whitespace
void lexer_skip_whitespace(lexer_t *lexer) {
  while (*lexer->current == ' ' || *lexer->current == '\t') {
    lexer->current++;
  }
}

// Function to extract tokens
token_t *lexer_next_token(lexer_t *lexer) {
  lexer->start = lexer->current;

  if (lexer->is_new_line) {
    lexer_skip_whitespace(lexer);
    int new_indent = lexer->current - lexer->start;
    int current_indent = lexer->indent_stack[lexer->indent_top];

    if (new_indent > current_indent) {
      // Indentation increased
      lexer->indent_stack[++lexer->indent_top] = new_indent;
      lexer->is_new_line = 0;
      return token_new(TOKEN_INDENT, "", 0, lexer->line);
    } else if (new_indent < current_indent) {
      // Indentation decreased
      lexer->indent_top--;
      lexer->is_new_line = 0;
      return token_new(TOKEN_DEDENT, "", 0, lexer->line);
    } else {
      // Indentation remains the same
      lexer->is_new_line = 0;
    }
  }

  lexer_skip_whitespace(lexer);
  char c = lexer_advance(lexer);
  int line = lexer->line;

  // Handle newlines
  if (c == '\n') {
    lexer->is_new_line = 1;
    lexer->line++;
    return token_new(TOKEN_EOL, "", 0, line);
  }

  // Handle end of file
  if (c == '\0') {
    if (lexer->indent_top > 0) {
      lexer->indent_top--;
      return token_new(TOKEN_DEDENT, "", 0, line);
    }
    return token_new(TOKEN_EOF, "", 0, line);
  }

  switch (c) {
  case '+':
    return token_new(TOKEN_PLUS, "+", 1, line);
  case '*':
    return token_new(TOKEN_STAR, "*", 1, line);
  case '/':
    return token_new(TOKEN_SLASH, "/", 1, line);
  case '(':
    return token_new(TOKEN_LPAREN, "(", 1, line);
  case ')':
    return token_new(TOKEN_RPAREN, ")", 1, line);
  case '[':
    return token_new(TOKEN_LBRACKET, "[", 1, line);
  case ']':
    return token_new(TOKEN_RBRACKET, "]", 1, line);
  case '{':
    return token_new(TOKEN_LBRACE, "{", 1, line);
  case '}':
    return token_new(TOKEN_RBRACE, "}", 1, line);
  case ':':
    return token_new(TOKEN_COLON, ":", 1, line);
  case ';':
    return token_new(TOKEN_SEMICOLON, ";", 1, line);
  case ',':
    return token_new(TOKEN_COMMA, ",", 1, line);
  case '>':
    if (lexer_peek(lexer) == '=') {
      lexer_advance(lexer);
      return token_new(TOKEN_GREATER_EQUAL, ">=", 2, line);
    }
    return token_new(TOKEN_GREATER, ">", 1, line);
  case '<':
    if (lexer_peek(lexer) == '=') {
      lexer_advance(lexer);
      return token_new(TOKEN_LESS_EQUAL, "<=", 2, line);
    }
    return token_new(TOKEN_LESS, "<", 1, line);
  case '=':
    if (lexer_peek(lexer) == '=') {
      lexer_advance(lexer);
      return token_new(TOKEN_EQUAL_EQUAL, "==", 2, line);
    }
    return token_new(TOKEN_EQUAL, "=", 1, line);
  case '!':
    if (lexer_peek(lexer) == '=') {
      lexer_advance(lexer);
      return token_new(TOKEN_BANG_EQUAL, "!=", 2, line);
    }
    return token_new(TOKEN_BANG, "!", 1, line);
  case '-':
    if (isdigit(lexer_peek(lexer))) {
      return parse_number(lexer, c);
    }
    return token_new(TOKEN_MINUS, "-", 1, line);

  // Handle numbers (int and float)
  default:
    if (isdigit(c)) {
      return parse_number(lexer, c);
    }
    // Handle strings
    if (c == '"') {
      char buffer[256];
      int i = 0;

      while (lexer_peek(lexer) != '"' && lexer_peek(lexer) != '\0') {
        buffer[i++] = lexer_advance(lexer);
      }

      if (lexer_peek(lexer) == '"') {
        lexer_advance(lexer); // Consume closing quote
      } else {
        fprintf(stderr, "Lexer Error: Unterminated string on line %d\n", line);
        exit(1);
      }

      buffer[i] = '\0';

      return token_new(TOKEN_STRING, buffer, i, line);
    }

    // Handle identifiers
    if (isalpha(c) || c == '_') {
      char buffer[64];
      int i = 0;

      buffer[i++] = c;
      while (isalnum(lexer_peek(lexer)) || lexer_peek(lexer) == '_') {
        buffer[i++] = lexer_advance(lexer);
      }
      buffer[i] = '\0';

      // Check if the identifier is a reserved keyword
      token_type_t type = lookup(lexer->keywords, buffer);

      return token_new(type, buffer, i, line);
    }

    // Unknown token
    fprintf(stderr, "Lexer Error: Unknown character '%c' on line %d\n", c,
            line);
    exit(1);
  }
}

token_t *parse_number(lexer_t *lexer, char c) {
  char buffer[64]; // Ensure a buffer large enough for floats
  int i = 0;
  int has_dot = 0;
  lexer->start = lexer->current;

  buffer[i++] = c;
  while (isdigit(lexer_peek(lexer)) || (lexer_peek(lexer) == '.' && !has_dot)) {
    if (lexer_peek(lexer) == '.') {
      has_dot = 1;
    }
    buffer[i++] = lexer_advance(lexer);
  }
  buffer[i] = '\0';

  if (has_dot) {
    return token_new(TOKEN_FLOAT, buffer, i, lexer->line);
  } else {
    return token_new(TOKEN_INT, buffer, i, lexer->line);
  }
}

token_t *token_new(token_type_t type, char *lexeme, int length, int line) {
  token_t *token = malloc(sizeof(token_t));
  if (token == NULL) {
    fprintf(stderr, "Token Error: Failed to allocate memory\n");
    exit(EXIT_FAILURE);
  }

  token->type = type;
  token->line = line;

  // Allocate memory for the lexeme
  token->lexeme = malloc(length + 1);
  if (token->lexeme == NULL) {
    fprintf(stderr, "Token Error: Failed to allocate memory\n");
    free(token);
    exit(EXIT_FAILURE);
  }

  memcpy(token->lexeme, lexeme, length);
  token->lexeme[length] = '\0';

  // Add literals for identifiers, numbers and strings
  if (type == TOKEN_INT) {
    token->integer = atoi(lexeme);
  } else if (type == TOKEN_FLOAT) {
    token->floating = atof(lexeme);
  } 

  return token;
}

// Print a token
void token_print(token_t token) {
  printf("Token { type: %s, lexeme: '%s', line: %d }\n",
         token_type_to_string(token.type), token.lexeme, token.line);
}

// Free a token
void token_free(token_t *token) {
  if (token) {
    free(token->lexeme);
    free(token);
  }
}

char *token_type_to_string(token_type_t type) {
  switch (type) {
  case TOKEN_INT:
    return "TOKEN_INT";
  case TOKEN_FLOAT:
    return "TOKEN_FLOAT";
  case TOKEN_STRING:
    return "TOKEN_STRING";
  case TOKEN_IDENTIFIER:
    return "TOKEN_IDENTIFIER";
  case TOKEN_PLUS:
    return "TOKEN_PLUS";
  case TOKEN_MINUS:
    return "TOKEN_MINUS";
  case TOKEN_STAR:
    return "TOKEN_STAR";
  case TOKEN_SLASH:
    return "TOKEN_SLASH";
  case TOKEN_EQUAL:
    return "TOKEN_EQUAL";
  case TOKEN_EQUAL_EQUAL:
    return "TOKEN_EQUAL_EQUAL";
  case TOKEN_BANG:
    return "TOKEN_BANG";
  case TOKEN_BANG_EQUAL:
    return "TOKEN_BANG_EQUAL";
  case TOKEN_GREATER:
    return "TOKEN_GREATER";
  case TOKEN_GREATER_EQUAL:
    return "TOKEN_GREATER_EQUAL";
  case TOKEN_LESS:
    return "TOKEN_LESS";
  case TOKEN_LESS_EQUAL:
    return "TOKEN_LESS_EQUAL";
  case TOKEN_LPAREN:
    return "TOKEN_LPAREN";
  case TOKEN_RPAREN:
    return "TOKEN_RPAREN";
  case TOKEN_LBRACKET:
    return "TOKEN_LBRACKET";
  case TOKEN_RBRACKET:
    return "TOKEN_RBRACKET";
  case TOKEN_COMMA:
    return "TOKEN_COMMA";
  case TOKEN_COLON:
    return "TOKEN_COLON";
  case TOKEN_SEMICOLON:
    return "TOKEN_SEMICOLON";
  case TOKEN_EOF:
    return "TOKEN_EOF";
  case TOKEN_EOL:
    return "TOKEN_EOL";
  case TOKEN_INDENT:
    return "TOKEN_INDENT";
  case TOKEN_DEDENT:
    return "TOKEN_DEDENT";
  case TOKEN_IF:
    return "TOKEN_IF";
  case TOKEN_ELSE:
    return "TOKEN_ELSE";
  case TOKEN_ELIF:
    return "TOKEN_ELIF";
  case TOKEN_WHILE:
    return "TOKEN_WHILE";
  case TOKEN_FOR:
    return "TOKEN_FOR";
  case TOKEN_BREAK:
    return "TOKEN_BREAK";
  case TOKEN_CONTINUE:
    return "TOKEN_CONTINUE";
  case TOKEN_RETURN:
    return "TOKEN_RETURN";
  case TOKEN_VOID:
    return "TOKEN_VOID";
  case TOKEN_INT_KEYWORD:
    return "TOKEN_INT_KEYWORD";
  case TOKEN_FLOAT_KEYWORD:
    return "TOKEN_FLOAT_KEYWORD";
  case TOKEN_STRING_KEYWORD:
    return "TOKEN_STRING_KEYWORD";
  case TOKEN_BOOL_KEYWORD:
    return "TOKEN_BOOL_KEYWORD";
  case TOKEN_VECTOR_3_KEYWORD:
    return "TOKEN_VECTOR_3_KEYWORD";
  case TOKEN_ARRAY_KEYWORD:
    return "TOKEN_ARRAY_KEYWORD";
  case TOKEN_OR:
    return "TOKEN_OR";
  case TOKEN_AND:
    return "TOKEN_AND";
  case TOKEN_TRUE:
    return "TOKEN_TRUE";
  case TOKEN_FALSE:
    return "TOKEN_FALSE";
  case TOKEN_NULL_KEYWORD:
    return "TOKEN_NULL_KEYWORD";
  case TOKEN_DEF:
    return "TOKEN_DEF";
  case TOKEN_CLASS:
    return "TOKEN_CLASS";
  case TOKEN_IMPORT:
    return "TOKEN_IMPORT";
  default:
    return "TOKEN_UNKNOWN";
  }
}

// Hashmap functions for reserved keywords

// Hash function djb2 by Dan Bernstein
unsigned int hash(const char *keyword) {
  unsigned long hash = 5381;
  int c;
  while ((c = *keyword++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash % KEYWORD_COUNT;
}

// insert a keyword into the hashmap
void insert(keyword_table_t *table, const char *keyword, token_type_t type) {
  unsigned int index = hash(keyword);
  keyword_node_t *new_node = malloc(sizeof(keyword_node_t));

  new_node->keyword = strdup(keyword);
  new_node->type = type;
  new_node->next = table->table[index];
  table->table[index] = new_node;
}

token_type_t lookup(keyword_table_t *table, const char *keyword) {
  unsigned int index = hash(keyword);
  keyword_node_t *node = table->table[index];

  while (node != NULL) {
    if (strcmp(node->keyword, keyword) == 0) {
      return node->type;
    }
    node = node->next;
  }

  return TOKEN_IDENTIFIER;
}

// Create a new keyword table
keyword_table_t *keyword_table_new() {
  keyword_table_t *table = malloc(sizeof(keyword_table_t));
  if (table == NULL) {
    fprintf(stderr, "Keyword Table Error: Failed to allocate memory\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < KEYWORD_COUNT; i++) {
    table->table[i] = NULL;
  }

  return table;
}

// Free a keyword table
void keyword_table_free(keyword_table_t *table) {
  if (table) {
    for (int i = 0; i < KEYWORD_COUNT; i++) {
      keyword_node_t *node = table->table[i];
      while (node != NULL) {
        keyword_node_t *next = node->next;
        free(node->keyword);
        free(node);
        node = next;
      }
    }
    free(table);
  }
}

// Initialize the keyword table with reserved keywords
void init_keywords(lexer_t *lexer) {
  insert(lexer->keywords, "if", TOKEN_IF);
  insert(lexer->keywords, "else", TOKEN_ELSE);
  insert(lexer->keywords, "elif", TOKEN_ELIF);
  insert(lexer->keywords, "while", TOKEN_WHILE);
  insert(lexer->keywords, "for", TOKEN_FOR);
  insert(lexer->keywords, "break", TOKEN_BREAK);
  insert(lexer->keywords, "continue", TOKEN_CONTINUE);
  insert(lexer->keywords, "return", TOKEN_RETURN);
  insert(lexer->keywords, "void", TOKEN_VOID);
  insert(lexer->keywords, "int", TOKEN_INT_KEYWORD);
  insert(lexer->keywords, "float", TOKEN_FLOAT_KEYWORD);
  insert(lexer->keywords, "string", TOKEN_STRING_KEYWORD);
  insert(lexer->keywords, "bool", TOKEN_BOOL_KEYWORD);
  insert(lexer->keywords, "vector_3", TOKEN_VECTOR_3_KEYWORD);
  insert(lexer->keywords, "array", TOKEN_ARRAY_KEYWORD);
  insert(lexer->keywords, "or", TOKEN_OR);
  insert(lexer->keywords, "and", TOKEN_AND);
  insert(lexer->keywords, "true", TOKEN_TRUE);
  insert(lexer->keywords, "false", TOKEN_FALSE);
  insert(lexer->keywords, "null", TOKEN_NULL_KEYWORD);
  insert(lexer->keywords, "def", TOKEN_DEF);
  insert(lexer->keywords, "class", TOKEN_CLASS);
  insert(lexer->keywords, "import", TOKEN_IMPORT);
}
