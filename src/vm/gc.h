#pragma once
#include "../objects/snekobject.h"
#include "../stack/stack.h"

void vm_collect_garbage(vm_t *vm);
void mark(vm_t *vm);
void trace(vm_t *vm);
void sweep(vm_t *vm);
void trace_blacken_object(stack_t *gray_objects, snek_object_t *ref);
void trace_mark_object(stack_t *gray_objects, snek_object_t *obj);
