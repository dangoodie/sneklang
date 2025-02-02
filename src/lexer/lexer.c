#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create a new lexer instance
lexer_t *lexer_new(char *src)
{
    lexer_t *lexer = malloc(sizeof(lexer_t));
    if (!lexer)
    {
        fprintf(stderr, "Lexer Error: Failed to allocate memory\n");
        exit(1);
    }
    lexer->start = src;
    lexer->current = src;
    lexer->line = 1;
    return lexer;
}

// Free the lexer instance
void lexer_free(lexer_t *lexer)
{
    if (lexer)
    {
        free(lexer);
    }
}

// Advance and return the next character in the source
char lexer_advance(lexer_t *lexer)
{
    if (*lexer->current == '\0')
        return '\0';
    return *(lexer->current++);
}

// Peek at the current character without consuming it
char lexer_peek(lexer_t *lexer)
{
    return (*lexer->current == '\0') ? '\0' : *(lexer->current);
}

// Skip whitespace
void lexer_skip_whitespace(lexer_t *lexer)
{
    while (*lexer->current == ' ' || *lexer->current == '\t' || *lexer->current == '\r')
    {
        lexer->current++;
    }
}

// Function to extract tokens
token_t *lexer_next_token(lexer_t *lexer)
{
    lexer_skip_whitespace(lexer);

    lexer->start = lexer->current; // Mark the start of the token
    char c = lexer_advance(lexer);
    int line = lexer->line;

    switch (c)
    {
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
    case '>':
        return token_new(TOKEN_GREATER, ">", 1, line);
    case '<':
        return token_new(TOKEN_LESS, "<", 1, line);
    case ',':
        return token_new(TOKEN_COMMA, ",", 1, line);
    case '=':
        return token_new(TOKEN_EQUAL, "=", 1, line);
    case '\n':
        return token_new(TOKEN_EOL, "", 0, line);
    case '\0':
        return token_new(TOKEN_EOF, "", 0, line);

    case '-':
        if (isdigit(lexer_peek(lexer)))
        {
            return parse_number(lexer);
        }
        return token_new(TOKEN_MINUS, "-", 1, line);
    // Handle numbers (int and float)
    default:
        if (isdigit(c))
        {
            return parse_number(lexer);
        }
        // Handle strings
        if (c == '"')
        {
            char buffer[256];
            int i = 0;

            while (lexer_peek(lexer) != '"' && lexer_peek(lexer) != '\0')
            {
                buffer[i++] = lexer_advance(lexer);
            }

            if (lexer_peek(lexer) == '"')
            {
                lexer_advance(lexer); // Consume closing quote
            }
            else
            {
                fprintf(stderr, "Lexer Error: Unterminated string on line %d\n", line);
                exit(1);
            }

            buffer[i] = '\0';

            return token_new(TOKEN_STRING, buffer, i, line);
        }

        // Handle identifiers
        if (isalpha(c) || c == '_')
        {
            char buffer[64];
            int i = 0;

            buffer[i++] = c;
            while (isalnum(lexer_peek(lexer)) || lexer_peek(lexer) == '_')
            {
                buffer[i++] = lexer_advance(lexer);
            }
            buffer[i] = '\0';

            return token_new(TOKEN_IDENTIFIER, buffer, i, line);
        }

        // Unknown token
        fprintf(stderr, "Lexer Error: Unknown character '%c' on line %d\n", c,
                line);
        exit(1);
    }
}

token_t *parse_number(lexer_t *lexer)
{
    char buffer[64]; // Ensure a buffer large enough for floats
    int i = 0;
    int has_dot = 0;

    buffer[i++] = lexer->start[0]; // Include the first character in the lexeme

    while (isdigit(lexer_peek(lexer)) || (lexer_peek(lexer) == '.' && !has_dot))
    {
        if (lexer_peek(lexer) == '.')
        {
            has_dot = 1;
        }
        buffer[i++] = lexer_advance(lexer);
    }
    buffer[i] = '\0';

    if (has_dot)
    {
        return token_new(TOKEN_FLOAT, buffer, i, lexer->line);
    }
    else
    {
        return token_new(TOKEN_INT, buffer, i, lexer->line);
    }
}

token_t *token_new(token_type_t type, char *lexeme, int length, int line)
{
    token_t *token = malloc(sizeof(token_t));
    if (token == NULL)
    {
        fprintf(stderr, "Token Error: Failed to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    token->type = type;
    token->line = line;

    // Allocate memory for the lexeme
    token->lexeme = malloc(length + 1);
    if (token->lexeme == NULL)
    {
        fprintf(stderr, "Token Error: Failed to allocate memory\n");
        free(token);
        exit(EXIT_FAILURE);
    }

    memcpy(token->lexeme, lexeme, length);
    token->lexeme[length] = '\0';
    return token;
}

// Print a token
void token_print(token_t token)
{
    printf("Token { type: %d, lexeme: '%s', line: %d }\n", token.type,
           token.lexeme, token.line);
}

// Free a token
void token_free(token_t *token)
{
    if (token)
    {
        free(token->lexeme);
        free(token);
    }
}