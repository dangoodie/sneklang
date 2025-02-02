#include "munit/munit.h"
#include "test_vm.h"
#include "test_stack.h"

MunitTest tests[] = {
    {"/test_vm", test_gc, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/test_stack", test_stack, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL} // Null-terminated array
};

static const MunitSuite suite = {
    "Sneklang Tests", // Suite name
    tests,            // Tests array
    NULL,             // No sub-suites
    1,                // Runs tests in a single process
    MUNIT_SUITE_OPTION_NONE};

int main(int argc, char *argv[])
{
  return munit_suite_main(&suite, NULL, argc, argv);
}
