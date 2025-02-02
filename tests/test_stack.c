#include "munit.h"
#include "stack.h"

munit_case(RUN, test_stack, {
  stack_t *stack = stack_new(10);
  int a = 5;
  stack_push(stack, &a);
  assert_int(stack->count, ==, 1);
  assert_ptr(stack_pop(stack), ==, &a);

  stack_free(stack);
});
