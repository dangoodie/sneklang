#include "../src/lexer/lexer.h"
#include "munit/munit.h"
#include <stdio.h>

// ✅ Test: Integer Lexing
MunitResult test_lexer_int(const MunitParameter params[], void *user_data) {
  lexer_t *lexer = lexer_new("42");
  token_t *token = lexer_next_token(lexer);
  munit_assert_int(token->type, ==, TOKEN_INT);
  munit_assert_string_equal(token->lexeme, "42");
  token_free(token);
  lexer_free(lexer);
  return MUNIT_OK;
}

// ✅ Test: Float Lexing
MunitResult test_lexer_float(const MunitParameter params[], void *user_data) {
  lexer_t *lexer = lexer_new("3.14");
  token_t *token = lexer_next_token(lexer);
  munit_assert_int(token->type, ==, TOKEN_FLOAT);
  munit_assert_string_equal(token->lexeme, "3.14");
  token_free(token);
  lexer_free(lexer);
  return MUNIT_OK;
}

// ✅ Test: String Lexing
MunitResult test_lexer_string(const MunitParameter params[], void *user_data) {
  lexer_t *lexer = lexer_new("\"hello\"");
  token_t *token = lexer_next_token(lexer);
  munit_assert_int(token->type, ==, TOKEN_STRING);
  munit_assert_string_equal(token->lexeme, "hello");
  token_free(token);
  lexer_free(lexer);
  return MUNIT_OK;
}

// ✅ Test: Operators ( +, -, *, /, >, <, = )
MunitResult test_lexer_operators(const MunitParameter params[],
                                 void *user_data) {
  lexer_t *lexer = lexer_new("+ - * / > < =");
  token_t *tokens[7];

  for (int i = 0; i < 7; i++) {
    tokens[i] = lexer_next_token(lexer);
  }

  munit_assert_int(tokens[0]->type, ==, TOKEN_PLUS);
  munit_assert_int(tokens[1]->type, ==, TOKEN_MINUS);
  munit_assert_int(tokens[2]->type, ==, TOKEN_STAR);
  munit_assert_int(tokens[3]->type, ==, TOKEN_SLASH);
  munit_assert_int(tokens[4]->type, ==, TOKEN_GREATER);
  munit_assert_int(tokens[5]->type, ==, TOKEN_LESS);
  munit_assert_int(tokens[6]->type, ==, TOKEN_EQUAL);

  for (int i = 0; i < 7; i++) {
    token_free(tokens[i]);
  }
  lexer_free(lexer);
  return MUNIT_OK;
}

// ✅ Test: Brackets & Punctuation
MunitResult test_lexer_brackets(const MunitParameter params[],
                                void *user_data) {
  lexer_t *lexer = lexer_new("() [] ,");
  token_t *tokens[5];

  for (int i = 0; i < 5; i++) {
    tokens[i] = lexer_next_token(lexer);
  }

  munit_assert_int(tokens[0]->type, ==, TOKEN_LPAREN);
  munit_assert_int(tokens[1]->type, ==, TOKEN_RPAREN);
  munit_assert_int(tokens[2]->type, ==, TOKEN_LBRACKET);
  munit_assert_int(tokens[3]->type, ==, TOKEN_RBRACKET);
  munit_assert_int(tokens[4]->type, ==, TOKEN_COMMA);

  for (int i = 0; i < 5; i++) {
    token_free(tokens[i]);
  }

  lexer_free(lexer);
  return MUNIT_OK;
}

// ✅ Test: Identifiers (Variable Names)
MunitResult test_lexer_identifiers(const MunitParameter params[],
                                   void *user_data) {
  lexer_t *lexer = lexer_new("variableName");
  token_t *token = lexer_next_token(lexer);
  munit_assert_int(token->type, ==, TOKEN_IDENTIFIER);
  munit_assert_string_equal(token->lexeme, "variableName");
  token_free(token);
  lexer_free(lexer);
  return MUNIT_OK;
}

// ✅ Full Script Test
const char *test_script = "x = 42 + 3.14\n"
                          "message = \"Hello, world!\"\n"
                          "vector = [1.0, 2.5, -3.6]\n"
                          "if x > 10 print(message)\n";

MunitResult test_lexer_full_script(const MunitParameter params[],
                                   void *user_data) {
  // Test script with deeper indentation levels and dedentation
  const char *test_script = "x: float = 42 + 3.14\n"
                            "message: string = \"Hello, world!\"\n"
                            "vector: vector_3 = [1.0, 2.5, -3.6]\n"
                            "\n" // Empty line
                            "if x > 10:\n"
                            "    print(message)\n"
                            "    if x < 50:\n"
                            "        print(\"Nested if\")\n"
                            "    print(\"Back to first level\")\n"
                            "if x == 42:\n"
                            "    print(\"Another if statement\")\n"
                            "print(\"Back to global scope\")\n";

  lexer_t *lexer = lexer_new(test_script);

  // Expected token types in order
  token_type_t expected_types[] = {
      TOKEN_IDENTIFIER, TOKEN_COLON,      TOKEN_IDENTIFIER, TOKEN_EQUAL,
      TOKEN_INT,        TOKEN_PLUS,       TOKEN_FLOAT,      TOKEN_EOL,

      TOKEN_IDENTIFIER, TOKEN_COLON,      TOKEN_IDENTIFIER, TOKEN_EQUAL,
      TOKEN_STRING,     TOKEN_EOL,

      TOKEN_IDENTIFIER, TOKEN_COLON,      TOKEN_IDENTIFIER, TOKEN_EQUAL,
      TOKEN_LBRACKET,   TOKEN_FLOAT,      TOKEN_COMMA,      TOKEN_FLOAT,
      TOKEN_COMMA,      TOKEN_FLOAT,      TOKEN_RBRACKET,   TOKEN_EOL,

      TOKEN_EOL, // Empty line

      TOKEN_IDENTIFIER, TOKEN_IDENTIFIER, TOKEN_GREATER,    TOKEN_INT,
      TOKEN_COLON,      TOKEN_EOL,

      TOKEN_INDENT,     TOKEN_IDENTIFIER, TOKEN_LPAREN,     TOKEN_IDENTIFIER,
      TOKEN_RPAREN,     TOKEN_EOL,

      TOKEN_IDENTIFIER, TOKEN_IDENTIFIER, TOKEN_LESS,       TOKEN_INT,
      TOKEN_COLON,      TOKEN_EOL,

      TOKEN_INDENT,     TOKEN_IDENTIFIER, TOKEN_LPAREN,     TOKEN_STRING,
      TOKEN_RPAREN,     TOKEN_EOL,        TOKEN_DEDENT,

      TOKEN_IDENTIFIER, TOKEN_LPAREN,     TOKEN_STRING,     TOKEN_RPAREN,
      TOKEN_EOL,        TOKEN_DEDENT,

      TOKEN_IDENTIFIER, TOKEN_IDENTIFIER, TOKEN_EQUAL,      TOKEN_EQUAL,
      TOKEN_INT,        TOKEN_COLON,      TOKEN_EOL,

      TOKEN_INDENT,     TOKEN_IDENTIFIER, TOKEN_LPAREN,     TOKEN_STRING,
      TOKEN_RPAREN,     TOKEN_EOL,        TOKEN_DEDENT,

      TOKEN_IDENTIFIER, TOKEN_LPAREN,     TOKEN_STRING,     TOKEN_RPAREN,
      TOKEN_EOL,

      TOKEN_EOF};

  int expected_count = sizeof(expected_types) / sizeof(expected_types[0]);
  token_t *tokens[expected_count];

  // Tokenize and store results
  for (int i = 0; i < expected_count; i++) {
    tokens[i] = lexer_next_token(lexer);
  }

  // Validate token order and correctness
  for (int i = 0; i < expected_count; i++) {
    munit_assert_int(tokens[i]->type, ==, expected_types[i]);
  }

  // Clean up memory
  for (int i = 0; i < expected_count; i++) {
    token_free(tokens[i]);
  }

  lexer_free(lexer);
  return MUNIT_OK;
}
