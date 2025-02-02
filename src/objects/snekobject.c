#include <string.h>

#include "snekobject.h"
#include "sneknew.h"

void snek_object_free(snek_object_t *obj) {
  switch (obj->kind) {
    case INTEGER:
    case FLOAT:
      break;
    case STRING:
      free(obj->data.v_string);
      break;
    case VECTOR3: {
      break;
    }
    case ARRAY: {
      snek_array_t *array = &obj->data.v_array;
      free(array->elements);

      break;
    }
  }

  free(obj);
}

bool snek_array_set(snek_object_t *array, size_t index, snek_object_t *value) {
  if (array == NULL || value == NULL) {
    return false;
  }

  if (array->kind != ARRAY) {
    return false;
  }

  if (index >= array->data.v_array.size) {
    return false;
  }

  array->data.v_array.elements[index] = value;
  return true;
}

snek_object_t *snek_array_get(snek_object_t *array, size_t index) {
  if (array == NULL) {
    return NULL;
  }

  if (array->kind != ARRAY) {
    return NULL;
  }

  if (index >= array->data.v_array.size) {
    return NULL;
  }

  // Set the value directly now (already checked size constraint)
  return array->data.v_array.elements[index];
}

snek_object_t *snek_add(vm_t *vm, snek_object_t *a, snek_object_t *b) {
  if (a == NULL || b == NULL) {
    return NULL;
  }

  switch (a->kind) {
  case INTEGER:
    switch (b->kind) {
    case INTEGER:
      return new_snek_integer(vm, a->data.v_int + b->data.v_int);
    case FLOAT:
      return new_snek_float(vm, (float)a->data.v_int + b->data.v_float);
    default:
      return NULL;
    }
  case FLOAT:
    switch (b->kind) {
    case FLOAT:
      return new_snek_float(vm, a->data.v_float + b->data.v_float);
    default:
      return snek_add(vm, b, a);
    }
  case STRING:
    switch (b->kind) {
    case STRING: {
      int a_len = strlen(a->data.v_string);
      int b_len = strlen(b->data.v_string);
      int len = a_len + b_len + 1;
      char *dst = malloc(len * sizeof(char));
      dst[0] = '\0';

      strcat(dst, a->data.v_string);
      strcat(dst, b->data.v_string);

      snek_object_t *obj = new_snek_string(vm, dst);
      free(dst);

      return obj;
    }
    default:
      return NULL;
    }
  case VECTOR3:
    switch (b->kind) {
    case VECTOR3:
      return new_snek_vector3(
          vm,
          snek_add(vm, a->data.v_vector3.x, b->data.v_vector3.x),
          snek_add(vm, a->data.v_vector3.y, b->data.v_vector3.y),
          snek_add(vm, a->data.v_vector3.z, b->data.v_vector3.z)
      );
    default:
      return NULL;
    }
  case ARRAY:
    switch (b->kind) {
    case ARRAY: {
      size_t a_len = a->data.v_array.size;
      size_t b_len = b->data.v_array.size;
      size_t length = a_len + b_len;

      snek_object_t *array = new_snek_array(vm, length);

      for (int i = 0; i < a_len; i++) {
        snek_array_set(array, i, snek_array_get(a, i));
      }

      for (int i = 0; i < b_len; i++) {
        snek_array_set(array, i + a_len, snek_array_get(b, i));
      }

      return array;
    }
    default:
      return NULL;
    }
  default:
    return NULL;
  }
}