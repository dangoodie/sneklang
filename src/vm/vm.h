#pragma once

#include "snekobject.h"
#include "stack.h"

typedef struct VirtualMachine {
  // stack frames: stack_t frame_t
  stack_t *frames;

  // These are the rest of the objects: stack_t snek_object_t
  stack_t *objects;
} vm_t;

typedef struct StackFrame {
  stack_t *references;
} frame_t;

/// Our main functions for garbage collection.
void mark(vm_t *vm);
void trace(vm_t *vm);
void sweep(vm_t *vm);

void vm_collect_garbage(vm_t *vm);

/// Helper functions for `trace`
void trace_blacken_object(stack_t *gray_objects, snek_object_t *ref);
void trace_mark_object(stack_t *gray_objects, snek_object_t *ref);

/// This is the function that gets called to actually do the garbage collection,
/// but is just composed of `mark`, `trace`, and `sweep`.
///
/// Don't worry, it's not going to delete your code (hopefully!)
void vm_collect_garbage(vm_t *vm);

/// Already implemented
vm_t *vm_new();
void vm_free(vm_t *vm);
void vm_track_object(vm_t *vm, snek_object_t *obj);

frame_t *vm_new_frame(vm_t *vm);
void vm_frame_push(vm_t *vm, frame_t *frame);
frame_t *vm_frame_pop(vm_t *vm);

void frame_free(frame_t *frame);

// Marks the object as referenced in the current stack frame.
void frame_reference_object(frame_t *frame, snek_object_t *obj);
