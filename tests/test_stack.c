#include "test_stack.h"

MunitResult test_stack(const MunitParameter params[], void *user_data) {
  stack_t *stack = stack_new(10);
  int a = 5;

  stack_push(stack, &a);
  munit_assert_int(stack->count, ==, 1);
  munit_assert_ptr(stack_pop(stack), ==, &a);

  stack_free(stack);
  return MUNIT_OK;
}
