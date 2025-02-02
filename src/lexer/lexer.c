#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Create a new lexer instance
lexer_t *lexer_new(const char *src)
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
    return *(lexer->current) ? *(lexer->current++) : '\0';
}

// Peek at the current character without consuming it
char lexer_peek(lexer_t *lexer)
{
    return *(lexer->current);
}

// Skip whitespace
void lexer_skip_whitespace(lexer_t *lexer)
{
    while (isspace(*lexer->current))
    {
        if (*lexer->current == '\n')
        {
            lexer->line++;
        }
        lexer->current++;
    }
}

// Function to extract tokens
token_t lexer_next_token(lexer_t *lexer)
{
    lexer_skip_whitespace(lexer);

    lexer->start = lexer->current; // Mark the start of the token
    char c = lexer_advance(lexer);
    int line = lexer->line;

    switch (c)
    {
    case '+':
        return (token_t){TOKEN_PLUS, "+", 1, line};
    case '-':
        return (token_t){TOKEN_MINUS, "-", 1, line};
    case '*':
        return (token_t){TOKEN_STAR, "*", 1, line};
    case '/':
        return (token_t){TOKEN_SLASH, "/", 1, line};
    case '(':
        return (token_t){TOKEN_LPAREN, "(", 1, line};
    case ')':
        return (token_t){TOKEN_RPAREN, ")", 1, line};
    case '[':
        return (token_t){TOKEN_LBRACKET, "[", 1, line};
    case ']':
        return (token_t){TOKEN_RBRACKET, "]", 1, line};
    case ',':
        return (token_t){TOKEN_COMMA, ",", 1, line};

    case '\0':
        return (token_t){TOKEN_EOF, "EOF", 3, line};

    // Handle numbers (int and float)
    default:
        if (isdigit(c))
        {
            char buffer[32];
            int i = 0;
            int has_dot = 0;

            buffer[i++] = c;
            while (isdigit(lexer_peek(lexer)) || (lexer_peek(lexer) == '.' && !has_dot))
            {
                if (lexer_peek(lexer) == '.')
                {
                    has_dot = 1;
                }
                buffer[i++] = lexer_advance(lexer);
            }
            buffer[i] = '\0';

            return (token_t){has_dot ? TOKEN_FLOAT : TOKEN_INT, strdup(buffer), i, line};
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

            return (token_t){TOKEN_STRING, strdup(buffer), i, line};
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

            return (token_t){TOKEN_IDENTIFIER, strdup(buffer), i, line};
        }

        // Unknown token
        fprintf(stderr, "Lexer Error: Unknown character '%c' on line %d\n", c, line);
        exit(1);
    }
}
