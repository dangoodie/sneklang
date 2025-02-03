#pragma once

#include <stdbool.h>
#define INDENT_STACK_SIZE 128

// Define all token types
typedef enum {
  TOKEN_INT,        // 42
  TOKEN_FLOAT,      // 3.14
  TOKEN_STRING,     // "hello, world"
  TOKEN_PLUS,       // +
  TOKEN_MINUS,      // -
  TOKEN_STAR,       // *
  TOKEN_SLASH,      // /
  TOKEN_LPAREN,     // (
  TOKEN_RPAREN,     // )
  TOKEN_LBRACKET,   // [
  TOKEN_RBRACKET,   // ]
  TOKEN_LBRACE,     // {
  TOKEN_RBRACE,     // }
  TOKEN_COLON,      // :
  TOKEN_GREATER,    // >
  TOKEN_LESS,       // <
  TOKEN_COMMA,      // ,
  TOKEN_EQUAL,      // =
  TOKEN_IDENTIFIER, // variable names (foo, bar)
  TOKEN_EOF,        // end of file
  TOKEN_EOL,        // end of line
  TOKEN_INDENT,     // indent
  TOKEN_DEDENT,     // dedent
  TOKEN_ERROR       // error token (for unrecognized characters)
} token_type_t;

// Structure to hold a token
typedef struct {
  token_type_t type; // The type of token
  char *lexeme;      // The actual lexeme (string representation)
  int line;          // Line number where the token was found
} token_t;

// Lexer structure to keep track of the lexing process
typedef struct {
  char *start;                         // Start of the current lexeme
  char *current;                       // Current position in the source
  int line;                            // Current line number
  int indent_stack[INDENT_STACK_SIZE]; // Stack to keep track of indentation
  int indent_top;                      // Top of the indentation stack
  int is_new_line;                     // Flag to check if we are at a new line
} lexer_t;

// Function prototypes
lexer_t *lexer_new(char *source);          // Create a new lexer
void lexer_free(lexer_t *lexer);           // Free the lexer memory
token_t *lexer_next_token(lexer_t *lexer); // Fetch the next token
token_t *token_new(token_type_t type, char *lexeme, int length,
                   int line);          // Create a new token
void token_print(token_t token);       // Print a token
void token_free(token_t *token);       // Free a token
token_t *parse_number(lexer_t *lexer, char c); // Parse a number
char *token_type_to_string(token_type_t type); // Convert a token type to a string
