#include "gc.h"
#include "vm.h"

#include <stdint.h>
#include <stdio.h>

void vm_collect_garbage(vm_t *vm) {
  mark(vm);
  trace(vm);
  sweep(vm);
}

void mark(vm_t *vm) {
  if (!vm || !vm->frames || !vm->objects) {
    fprintf(stderr, "[ERROR] VM, frames, or objects are NULL!\n");
    return;
  }

  for (size_t i = 0; i < vm->frames->count; i++) {
    frame_t *frame = vm->frames->data[i];

    if (!frame) {
      fprintf(stderr, "[ERROR] Frame %zu is NULL!\n", i);
      continue;
    }
    if (!frame->references || !frame->references->data) {
      fprintf(stderr, "[ERROR] Frame %zu has NULL references or data!\n", i);
      continue;
    }

    for (size_t j = 0; j < frame->references->count; j++) {
      snek_object_t *obj = frame->references->data[j];

      // 🚨 Additional Safety Check
      if (!obj) {
        fprintf(stderr,
                "[WARNING] Null object reference at frame %zu, index %zu\n", i,
                j);
        continue;
      }

      if ((uintptr_t)obj < 0x1000 ||
          (uintptr_t)obj > 0x7fffffffffff) { // Check for valid address range
        fprintf(stderr,
                "[ERROR] Detected invalid object pointer at frame %zu, index "
                "%zu: %p\n",
                i, j, obj);
        continue;
      }

      if (obj->is_marked) {
        fprintf(stderr,
                "[WARNING] Object already marked at frame %zu, index %zu\n", i,
                j);
        continue;
      }

      obj->is_marked = true;
    }
  }
}

void trace(vm_t *vm) {
  stack_t *gray_objects = stack_new(8);
  if (gray_objects == NULL) {
    return;
  }

  for (size_t i = 0; i < vm->objects->count; i++) {
    snek_object_t *obj = vm->objects->data[i];
    if (obj->is_marked) {
      stack_push(gray_objects, obj);
    }
  }

  while (gray_objects->count > 0) {
    trace_blacken_object(gray_objects, stack_pop(gray_objects));
  }

  stack_free(gray_objects);
}

void trace_blacken_object(stack_t *gray_objects, snek_object_t *ref) {
  snek_object_t *obj = ref;

  switch (obj->kind) {
  case INTEGER:
  case FLOAT:
  case STRING:
    break;
  case VECTOR3: {
    snek_vector_t vec = obj->data.v_vector3;
    if (vec.x)
      trace_mark_object(gray_objects, vec.x);
    if (vec.y)
      trace_mark_object(gray_objects, vec.y);
    if (vec.z)
      trace_mark_object(gray_objects, vec.z);
    break;
  }
  case ARRAY: {
    for (size_t i = 0; i < obj->data.v_array.size; i++) {
      if (obj->data.v_array.elements[i])
        trace_mark_object(gray_objects, obj->data.v_array.elements[i]);
    }
    break;
  }
  }
}

void trace_mark_object(stack_t *gray_objects, snek_object_t *obj) {
  if (obj == NULL || obj->is_marked)
    return;

  stack_push(gray_objects, obj);
  obj->is_marked = true;
}

void sweep(vm_t *vm) {
  int writeIndex = 0;
  for (size_t i = 0; i < vm->objects->count; i++) {
    snek_object_t *obj = vm->objects->data[i];
    if (obj->is_marked) {
      obj->is_marked = false;
      vm->objects->data[writeIndex++] = obj;
    } else {
      snek_object_free(obj);
    }
  }
  vm->objects->count = writeIndex;
}
