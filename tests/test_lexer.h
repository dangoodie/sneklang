#pragma once
#include "../src/lexer/lexer.h"
#include "munit/munit.h"

MunitResult test_lexer_int(const MunitParameter params[], void *user_data);
MunitResult test_lexer_float(const MunitParameter params[], void *user_data);
MunitResult test_lexer_string(const MunitParameter params[], void *user_data);
MunitResult test_lexer_operators(const MunitParameter params[],
                                 void *user_data);
MunitResult test_lexer_brackets(const MunitParameter params[], void *user_data);
MunitResult test_lexer_identifiers(const MunitParameter params[],
                                   void *user_data);
MunitResult test_lexer_full_script(const MunitParameter params[],
                                   void *user_data);
MunitResult test_lexer_literals(const MunitParameter params[], void *user_data);