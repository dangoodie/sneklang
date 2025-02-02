#include "munit/munit.h"
#include "../src/lexer/lexer.h"
#include <stdio.h>

// ✅ Test: Integer Lexing
MunitResult test_lexer_int(const MunitParameter params[], void *user_data)
{
    lexer_t *lexer = lexer_new("42");
    token_t *token = malloc(sizeof(token_t));
    *token = lexer_next_token(lexer);
    munit_assert_int(token->type, ==, TOKEN_INT);
    munit_assert_string_equal(token->lexeme, "42");
    token_free(token);
    lexer_free(lexer);
    return MUNIT_OK;
}

// ✅ Test: Float Lexing
MunitResult test_lexer_float(const MunitParameter params[], void *user_data)
{
    lexer_t *lexer = lexer_new("3.14");
    token_t *token = malloc(sizeof(token_t));
    *token = lexer_next_token(lexer);
    munit_assert_int(token->type, ==, TOKEN_FLOAT);
    munit_assert_string_equal(token->lexeme, "3.14");
    token_free(token);
    lexer_free(lexer);
    return MUNIT_OK;
}

// ✅ Test: String Lexing
MunitResult test_lexer_string(const MunitParameter params[], void *user_data)
{
    lexer_t *lexer = lexer_new("\"hello\"");
    token_t *token = malloc(sizeof(token_t));
    *token = lexer_next_token(lexer);
    munit_assert_int(token->type, ==, TOKEN_STRING);
    munit_assert_string_equal(token->lexeme, "hello");
    token_free(token);
    lexer_free(lexer);
    return MUNIT_OK;
}

// ✅ Test: Operators ( +, -, *, /, >, <, = )
MunitResult test_lexer_operators(const MunitParameter params[], void *user_data)
{
    lexer_t *lexer = lexer_new("+ - * / > < =");
    token_t *tokens[7];

    for (int i = 0; i < 7; i++)
    {
        tokens[i] = malloc(sizeof(token_t));
        *tokens[i] = lexer_next_token(lexer);
    }

    munit_assert_int(tokens[0]->type, ==, TOKEN_PLUS);
    munit_assert_int(tokens[1]->type, ==, TOKEN_MINUS);
    munit_assert_int(tokens[2]->type, ==, TOKEN_STAR);
    munit_assert_int(tokens[3]->type, ==, TOKEN_SLASH);
    munit_assert_int(tokens[4]->type, ==, TOKEN_GREATER);
    munit_assert_int(tokens[5]->type, ==, TOKEN_LESS);
    munit_assert_int(tokens[6]->type, ==, TOKEN_EQUAL);

    for (int i = 0; i < 7; i++)
    {
        token_free(tokens[i]);
    }
    lexer_free(lexer);
    return MUNIT_OK;
}

// ✅ Test: Brackets & Punctuation
MunitResult test_lexer_brackets(const MunitParameter params[], void *user_data)
{
    lexer_t *lexer = lexer_new("() [] ,");
    token_t *tokens[5];

    for (int i = 0; i < 5; i++)
    {
        tokens[i] = malloc(sizeof(token_t));
        *tokens[i] = lexer_next_token(lexer);
    }

    munit_assert_int(tokens[0]->type, ==, TOKEN_LPAREN);
    munit_assert_int(tokens[1]->type, ==, TOKEN_RPAREN);
    munit_assert_int(tokens[2]->type, ==, TOKEN_LBRACKET);
    munit_assert_int(tokens[3]->type, ==, TOKEN_RBRACKET);
    munit_assert_int(tokens[4]->type, ==, TOKEN_COMMA);

    for (int i = 0; i < 5; i++)
    {
        token_free(tokens[i]);
    }

    lexer_free(lexer);
    return MUNIT_OK;
}

// ✅ Test: Identifiers (Variable Names)
MunitResult test_lexer_identifiers(const MunitParameter params[], void *user_data)
{
    lexer_t *lexer = lexer_new("variableName");
    token_t *token = malloc(sizeof(token_t));
    *token = lexer_next_token(lexer);
    munit_assert_int(token->type, ==, TOKEN_IDENTIFIER);
    munit_assert_string_equal(token->lexeme, "variableName");
    token_free(token);
    lexer_free(lexer);
    return MUNIT_OK;
}

// ✅ Full Script Test
const char *test_script =
    "x = 42 + 3.14\n"
    "message = \"Hello, world!\"\n"
    "vector = [1.0, 2.5, -3.6]\n"
    "if x > 10 print(message)\n";

MunitResult test_lexer_full_script(const MunitParameter params[], void *user_data)
{
    lexer_t *lexer = lexer_new(test_script);
    token_t *tokens[20];

    for (int i = 0; i < 20; i++)
    {
        tokens[i] = malloc(sizeof(token_t));
        *tokens[i] = lexer_next_token(lexer);
    }

    // Expected token types in order
    token_type_t expected_types[] = {
        TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_INT, TOKEN_PLUS, TOKEN_FLOAT, TOKEN_EOF,
        TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_STRING, TOKEN_EOF,
        TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_LBRACKET, TOKEN_FLOAT, TOKEN_COMMA, TOKEN_FLOAT, TOKEN_COMMA, TOKEN_FLOAT, TOKEN_RBRACKET, TOKEN_EOF,
        TOKEN_IDENTIFIER, TOKEN_GREATER, TOKEN_INT, TOKEN_IDENTIFIER, TOKEN_LPAREN, TOKEN_IDENTIFIER, TOKEN_RPAREN, TOKEN_EOF,
        TOKEN_EOF};

    int expected_count = sizeof(expected_types) / sizeof(expected_types[0]);

    printf("\n### Debugging Tokens ###\n");
    for (int i = 0; i < expected_count; i++)
    {
        printf("Token %d: Got type %d, Expected type %d, Lexeme: '%s'\n",
               i, tokens[i]->type, expected_types[i], tokens[i]->lexeme);
    }
    
    // Validate token order
    for (int i = 0; i < expected_count; i++)
    {
        munit_assert_int(tokens[i]->type, ==, expected_types[i]);
    }

    for (int i = 0; i < 20; i++)
    {
        token_free(tokens[i]);
    }

    lexer_free(lexer);
    return MUNIT_OK;
}