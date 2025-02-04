#include "munit/munit.h"
#include "test_lexer.h"
#include "test_stack.h"
#include "test_vm.h"

MunitTest tests[] = {
    // VM Tests
    {"/test_vm", test_gc, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_stack", test_stack, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},

    // Lexer Tests
    {"/lexer/int", test_lexer_int, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/lexer/float", test_lexer_float, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/lexer/string", test_lexer_string, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/lexer/operators", test_lexer_operators, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/lexer/brackets", test_lexer_brackets, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},
    {"/lexer/identifiers", test_lexer_identifiers, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/lexer/full_script", test_lexer_full_script, NULL, NULL,
     MUNIT_TEST_OPTION_NONE, NULL},
    {"/lexer/literals", test_lexer_literals, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL},

    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE,
     NULL} // Null-terminated array
};

static const MunitSuite suite = {"Sneklang Tests", // Suite name
                                 tests,            // Tests array
                                 NULL,             // No sub-suites
                                 1, // Runs tests in a single process
                                 MUNIT_SUITE_OPTION_NONE};

int main(int argc, char *argv[]) {
  return munit_suite_main(&suite, NULL, argc, argv);
}
