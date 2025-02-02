#include "vm.h"
#include "snekobject.h"
#include "stack.h"

void vm_collect_garbage(vm_t *vm) {
  mark(vm);
  trace(vm);
  sweep(vm);
}

void sweep(vm_t *vm) {
  for (int i = 0; i < vm->objects->count; i++) {
    snek_object_t *obj = vm->objects->data[i];
    if(obj->is_marked == true) {
      obj->is_marked = false;
    } else {
      snek_object_free(obj);
      vm->objects->data[i] = NULL;
    }
  }
  stack_remove_nulls(vm->objects);
}

// don't touch below this line

void mark(vm_t *vm) {
  for (size_t i = 0; i < vm->frames->count; i++) {
    frame_t *frame = vm->frames->data[i];
    for (size_t j = 0; j < frame->references->count; j++) {
      snek_object_t *obj = frame->references->data[j];
      obj->is_marked = true;
    }
  }
}

void trace(vm_t *vm) {
  stack_t *gray_objects = stack_new(8);
  if (gray_objects == NULL) {
    return;
  }

  // Get previously marked objects (which are the roots)
  for (int i = 0; i < vm->objects->count; i++) {
    snek_object_t *obj = vm->objects->data[i];
    if (obj->is_marked) {
      stack_push(gray_objects, obj);
    }
  }

  // Trace through the objects
  while (gray_objects->count > 0) {
    trace_blacken_object(gray_objects, stack_pop(gray_objects));
  }

  // Clean up after ourselves :)
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
    trace_mark_object(gray_objects, vec.x);
    trace_mark_object(gray_objects, vec.y);
    trace_mark_object(gray_objects, vec.z);
    break;
  }
  case ARRAY: {
    for (size_t i = 0; i < obj->data.v_array.size; i++) {
      trace_mark_object(gray_objects, obj->data.v_array.elements[i]);
    }
    break;
  }
  }
}

void trace_mark_object(stack_t *gray_objects, snek_object_t *obj) {
  if (obj == NULL || obj->is_marked) {
    return;
  }

  stack_push(gray_objects, obj);
  obj->is_marked = true;
}

void frame_reference_object(frame_t *frame, snek_object_t *obj) {
  stack_push(frame->references, obj);
}

vm_t *vm_new() {
  vm_t *vm = malloc(sizeof(vm_t));
  if (vm == NULL) {
    return NULL;
  }

  vm->frames = stack_new(8);
  vm->objects = stack_new(8);
  return vm;
}

void vm_free(vm_t *vm) {
  // Free the stack frames, and then their container
  for (int i = 0; i < vm->frames->count; i++) {
    frame_free(vm->frames->data[i]);
  }
  stack_free(vm->frames);

  // Free the objects, and then their container
  for (int i = 0; i < vm->objects->count; i++) {
    snek_object_free(vm->objects->data[i]);
  }
  stack_free(vm->objects);

  free(vm);
}

void vm_frame_push(vm_t *vm, frame_t *frame) {
  stack_push(vm->frames, frame);
}

frame_t *vm_frame_pop(vm_t *vm) {
  return stack_pop(vm->frames);
}

frame_t *vm_new_frame(vm_t *vm) {
  frame_t *frame = malloc(sizeof(frame_t));
  frame->references = stack_new(8);

  vm_frame_push(vm, frame);
  return frame;
}

void frame_free(frame_t *frame) {
  stack_free(frame->references);
  free(frame);
}

void vm_track_object(vm_t *vm, snek_object_t *obj) {
  stack_push(vm->objects, obj);
}
