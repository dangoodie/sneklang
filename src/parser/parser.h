#include "../lexer/lexer.h"
#include <stdbool.h>


// Struct for the parser
typedef struct {
    lexer_t *lexer;   // Lexer instance
    token_t *current; // Current token 
} parser_t;

parser_t *parser_new(lexer_t *lexer);
void parser_free(parser_t *parser);
token_t *parser_peek(parser_t *parser);
token_t *parser_advance(parser_t *parser);
bool parser_match(parser_t *parser, token_type_t type);
void parser_expect(parser_t *parser, token_type_t type, const char *error_msg);
void parse_expression(parser_t *parser);
void parse_if_statement(parser_t *parser);
