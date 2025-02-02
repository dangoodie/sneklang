#include "test_vm.h"
#include "../src/objects/sneknew.h"
#include "../src/vm/gc.h"
#include "../src/vm/vm.h"
#include "munit/munit.h"

MunitResult test_gc(const MunitParameter params[], void *user_data) {
  vm_t *vm = vm_new();
  frame_t *f1 = vm_new_frame(vm);

  snek_object_t *s = new_snek_string(vm, "I wish I knew how to read.");
  frame_reference_object(f1, s);
  vm_collect_garbage(vm);
  // nothing should be collected because
  // we haven't freed the frame
  munit_assert_int(vm->objects->count, ==, 1);

  frame_free(vm_frame_pop(vm));
  vm_collect_garbage(vm);
  // now the string should be collected
  munit_assert_int(vm->objects->count, ==, 0);

  vm_free(vm);

  return MUNIT_OK;
}
