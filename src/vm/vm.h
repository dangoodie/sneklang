#pragma once

#include "../stack/stack.h"

typedef struct SnekObject snek_object_t;

typedef struct VirtualMachine {
  stack_t *frames;  // Stack of function call frames
  stack_t *objects; // Stack of allocated objects for GC
} vm_t;

typedef struct StackFrame {
  stack_t *references; // Tracks local references
} frame_t;

/// VM Lifecycle Management
vm_t *vm_new();
void vm_free(vm_t *vm);

/// Stack Frame Management
void vm_frame_push(vm_t *vm, frame_t *frame);
frame_t *vm_frame_pop(vm_t *vm);
frame_t *vm_new_frame(vm_t *vm);
void frame_free(frame_t *frame);

/// Object Management
void vm_track_object(vm_t *vm, snek_object_t *obj);
void frame_reference_object(frame_t *frame, snek_object_t *obj);
