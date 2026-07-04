#include "gc.h"
#include <assert.h>
#include <stdlib.h>

// Implementing garbage collector from this blog
//  https://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/

VM* newVM() {
  VM* vm = malloc(sizeof(VM));
  vm->firstObject = NULL;
  vm->stackSize = 0;
  vm->currentAllocations = 0;
  vm->maxAllocations = MAX_ALLOCATION;
  return vm;
}

void push(VM* vm, Object* value) {
  assert(vm->stackSize < STACK_MAX && "Stack overflow!");
  vm->stack[vm->stackSize++] = value;
}

Object* pop(VM* vm) {
  assert(vm->stackSize > 0 && "Stack underflow");
  return vm->stack[--vm->stackSize];
}

Object* newObject(VM* vm, ObjectType type) {
  Object* object = malloc(sizeof(Object));
  object->type = type;

  object->next = vm->firstObject;
  vm->firstObject = object;

  if (vm->currentAllocations >= vm->maxAllocations) {
    gc(vm);
  }
  vm->currentAllocations++;

  return object;
}

void pushInt(VM* vm, int intValue) {
  Object* object = newObject(vm, OBJ_INT);
  object->marked = 0;
  object->value = intValue;
  push(vm, object);
}

Object* pushPair(VM* vm) {
  Object* object = newObject(vm, OBJ_PAIR);
  object->marked = 0;
  object->tail = pop(vm);
  object->head = pop(vm);

  push(vm, object);
  return object;
}

void mark(Object* object) {
  if (object != NULL && object->marked)
    return;

  object->marked = 1;

  if (object->type == OBJ_PAIR) {
    mark(object->head);
    mark(object->tail);
  }
}

void markAll(VM* vm) {
  for (int i = 0; i < vm->stackSize; i++)
    mark(vm->stack[i]);
}

void sweep(VM* vm) {
  Object** object = &vm->firstObject;
  while (*object) {
    if (!((*object)->marked)) {
      Object* unreached = *object;
      *object = unreached->next;
      vm->currentAllocations--;
      free(unreached);
    } else {
      (*object)->marked = 0;
      object = &(*object)->next;
    }
  }
}

void gc(VM* vm) {
  markAll(vm);
  sweep(vm);
}