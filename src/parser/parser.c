#include "parser.h"
#include <stdlib.h>
#include <stdio.h>


// New parser instance
parser_t *parser_new(lexer_t *lexer) {
    parser_t *parser = malloc(sizeof(parser_t));
    if (!parser) {
        fprintf(stderr, "Parser Error: Failed to allocate memory\n");
        exit(1);
    }
    parser->lexer = lexer;
    parser->current = lexer_next_token(lexer);
    return parser;
}

// Free the parser instance
void parser_free(parser_t *parser) {
    if (parser) {
        if (parser->lexer) {
            lexer_free(parser->lexer);
        }
        free(parser);
    }
}

// Peek at the current token
token_t *parser_peek(parser_t *parser) {
    return parser->current;
}

// Advance to the next token
token_t *parser_advance(parser_t *parser) {
    if (parser->current->type != TOKEN_EOF) {
        token_free(parser->current);
        parser->current = lexer_next_token(parser->lexer);
    }
    return parser->current;
}

// Match the current token type
bool parser_match(parser_t *parser, token_type_t type) {
    if (parser->current->type == type) {
        parser_advance(parser);
        return true;
    }
    return false;
}

// Expect a certain token type or report an error
void parser_expect(parser_t *parser, token_type_t type, const char *error_msg) {
    if (parser->current->type == type) {
        printf("Matched token: %s\n", parser->current->lexeme);
        parser_advance(parser);
    } else {
        fprintf(stderr, "Parser Error: %s at line %d\n", error_msg, parser->current->line);
        exit(1);
    }
}

// Parse an expression
void parse_expression(parser_t *parser) {
    token_t *left = parser_advance(parser);

    if (parser_match(parser, TOKEN_PLUS) || parser_match(parser, TOKEN_MINUS) ||
        parser_match(parser, TOKEN_STAR) || parser_match(parser, TOKEN_SLASH)) {
        token_t *op = parser_advance(parser);
        token_t *right = parser_advance(parser);
        printf("Parsed binary expression: %s %s %s\n", left->lexeme, op->lexeme, right->lexeme);
    } else {
        printf("Parsed single value: %s\n", left->lexeme);
    }
}


// Parse an if statement
void parse_if_statement(parser_t *parser) {
    parser_expect(parser, TOKEN_IF, "Expected 'if' keyword");

    // Parse the condition (just a single value for now)
    parse_expression(parser);

    // Expect closing parenthesis and colon
    parser_expect(parser, TOKEN_COLON, "Expected ':' after condition");
    parser_expect(parser, TOKEN_EOL, "Expected newline after ':'");
    parser_expect(parser, TOKEN_INDENT, "Expected indent after newline");

    // Parse the body of the if statement
    while(parser_peek(parser)->type != TOKEN_DEDENT) {
        parse_expression(parser);
    }

    printf("Parsed if statement\n");
}