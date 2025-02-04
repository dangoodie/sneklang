#pragma once

#include <stdbool.h>
#define INDENT_STACK_SIZE 128

// Define all token types
typedef enum {
  TOKEN_ERROR = -1, // error token (for unrecognized characters)
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
  TOKEN_SEMICOLON,  // ;
  TOKEN_GREATER,    // >
  TOKEN_GREATER_EQUAL, // >=
  TOKEN_LESS,       // <
  TOKEN_LESS_EQUAL, // <=
  TOKEN_COMMA,      // ,
  TOKEN_EQUAL,      // =
  TOKEN_EQUAL_EQUAL,// ==
  TOKEN_BANG,       // !
  TOKEN_BANG_EQUAL, // !=
  TOKEN_IDENTIFIER, // variable names (foo, bar)
  TOKEN_EOF,        // end of file
  TOKEN_EOL,        // end of line
  TOKEN_INDENT,     // indent
  TOKEN_DEDENT,     // dedent

  // Reserved keywords
  TOKEN_IF,
  TOKEN_ELSE,
  TOKEN_ELIF,
  TOKEN_WHILE,
  TOKEN_FOR,
  TOKEN_BREAK,
  TOKEN_CONTINUE,
  TOKEN_RETURN,
  TOKEN_VOID,
  TOKEN_INT_KEYWORD,
  TOKEN_FLOAT_KEYWORD,
  TOKEN_STRING_KEYWORD,
  TOKEN_BOOL_KEYWORD,
  TOKEN_VECTOR_3_KEYWORD,
  TOKEN_ARRAY_KEYWORD,
  TOKEN_OR,
  TOKEN_AND,
  TOKEN_TRUE,
  TOKEN_FALSE,
  TOKEN_NULL_KEYWORD,
  TOKEN_DEF,
  TOKEN_CLASS,
  TOKEN_IMPORT,

  // End of reserved keywords
  TOTAL_TOKEN_COUNT
} token_type_t;

// Hash table the keyword lookup
#define KEYWORD_COUNT 256

typedef struct {
  char *keyword;
  token_type_t type;
  struct keyword_node_t *next;
} keyword_node_t;

typedef struct {
  keyword_node_t *table[KEYWORD_COUNT];
} keyword_table_t;

// Structure to hold a token
typedef struct {
  token_type_t type; // The type of token
  char *lexeme;      // The actual lexeme (string representation)
  union literal
  {
    int integer;
    double floating;
    char *string;
  };
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
  keyword_table_t *keywords;           // Hash table to store keywords
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

// Keyword lookup functions
unsigned int hash(const char *keyword); // Hash function
void insert(keyword_table_t *table, const char *keyword, token_type_t type); // Insert a keyword
token_type_t lookup(keyword_table_t *table, const char *keyword); // Lookup a keyword
keyword_table_t *keyword_table_new(); // Create a new keyword table
void keyword_table_free(keyword_table_t *table); // Free a keyword table
void init_keywords(lexer_t *lexer); // Initialize the keyword table