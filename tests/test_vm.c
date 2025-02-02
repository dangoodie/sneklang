#include "munit.h"
#include "vm.h"
#include "sneknew.h"

munit_case(RUN, test_gc, {
  vm_t *vm = vm_new();
  snek_object_t *obj = new_snek_integer(vm, 42);
  vm_collect_garbage(vm);
  assert_false(boot_is_freed(obj));

  vm_free(vm);
  assert_true(boot_is_freed(obj));
});
