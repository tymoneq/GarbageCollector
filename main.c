#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// Implementing garbage collector from this blog
//  https://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/

typedef enum { OBJ_INT, OBJ_PAIR } ObjectType;

typedef struct sObject {
  unsigned char marked;
  ObjectType type;

  union {
    int value;

    struct {
      struct sObject* head;
      struct sObject* tail;
    };
  };
} Object;

#define STACK_MAX 256

typedef struct {
  Object* stack[STACK_MAX];
  int stackSize;
} VM;

VM* newVM() {
  VM* vm = malloc(sizeof(VM));
  vm->stackSize = 0;
  return vm;
}

void push(VM* vm, Object* value) {
  assert(vm->stackSize < STACK_MAX && "Stack overflow!");
  vm->stack[vm->stackSize++] = value;
}

Object* pop(VM* vm) {
  assert(vm->stackSize > 0 && "Stack underflow");
  return vm->stack[vm->stackSize--];
}

Object* newObject(VM* vm, ObjectType type) {
  Object* object = malloc(sizeof(Object));
  object->type = type;
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
  object->head = pop(vm);
  object->tail = pop(vm);

  push(vm, object);
  return object;
}

void mark(Object* object) {
  if (object->marked)
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
