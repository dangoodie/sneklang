#include "munit/munit.h"
#include "../src/lexer/lexer.h"
#include <stdlib.h> // Needed for free()

// Test: Integer Lexing
MunitResult test_lexer_int(const MunitParameter params[], void *user_data)
{
    lexer_t *lexer = lexer_new("42");
    token_t token = lexer_next_token(lexer);
    munit_assert_int(token.type, ==, TOKEN_INT);
    munit_assert_string_equal(token.lexeme, "42");

    free((char *)token.lexeme); // Free dynamically allocated lexeme

    lexer_free(lexer);
    return MUNIT_OK;
}

// Test: Float Lexing
MunitResult test_lexer_float(const MunitParameter params[], void *user_data)
{
    lexer_t *lexer = lexer_new("3.14");
    token_t token = lexer_next_token(lexer);

    munit_assert_int(token.type, ==, TOKEN_FLOAT);
    munit_assert_string_equal(token.lexeme, "3.14");

    free(token.lexeme);
    lexer_free(lexer);
    return MUNIT_OK;
}

// Test: String Lexing
MunitResult test_lexer_string(const MunitParameter params[], void *user_data)
{
    lexer_t *lexer = lexer_new("\"hello\"");
    token_t token = lexer_next_token(lexer);

    munit_assert_int(token.type, ==, TOKEN_STRING);
    munit_assert_string_equal(token.lexeme, "hello");

    free(token.lexeme);
    lexer_free(lexer);
    return MUNIT_OK;
}

// Test: Operators
MunitResult test_lexer_operators(const MunitParameter params[], void *user_data)
{
    lexer_t *lexer = lexer_new("+ - * /");

    token_t token = lexer_next_token(lexer);
    munit_assert_int(token.type, ==, TOKEN_PLUS);

    token = lexer_next_token(lexer);
    munit_assert_int(token.type, ==, TOKEN_MINUS);

    token = lexer_next_token(lexer);
    munit_assert_int(token.type, ==, TOKEN_STAR);

    token = lexer_next_token(lexer);
    munit_assert_int(token.type, ==, TOKEN_SLASH);

    lexer_free(lexer);
    return MUNIT_OK;
}

// Test: Brackets
MunitResult test_lexer_brackets(const MunitParameter params[], void *user_data)
{
    lexer_t *lexer = lexer_new("() []");

    token_t token = lexer_next_token(lexer);
    munit_assert_int(token.type, ==, TOKEN_LPAREN);

    token = lexer_next_token(lexer);
    munit_assert_int(token.type, ==, TOKEN_RPAREN);

    token = lexer_next_token(lexer);
    munit_assert_int(token.type, ==, TOKEN_LBRACKET);

    token = lexer_next_token(lexer);
    munit_assert_int(token.type, ==, TOKEN_RBRACKET);

    lexer_free(lexer);
    return MUNIT_OK;
}

// Test: Identifiers
MunitResult test_lexer_identifiers(const MunitParameter params[], void *user_data)
{
    lexer_t *lexer = lexer_new("varName");
    token_t token = lexer_next_token(lexer);

    munit_assert_int(token.type, ==, TOKEN_IDENTIFIER);
    munit_assert_string_equal(token.lexeme, "varName");

    free(token.lexeme);
    lexer_free(lexer);
    return MUNIT_OK;
}
