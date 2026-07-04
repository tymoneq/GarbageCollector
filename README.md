# Simple Garbage Collector

A minimal implementation of a mark-and-sweep garbage collector in C, inspired by [Baby's First Garbage Collector](https://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/) by Bob Nystrom.

## Overview

This project implements a basic garbage collection system that manages memory allocation for a simple virtual machine. It demonstrates core garbage collection concepts including:

- **Object allocation and management** - Creating and tracking objects
- **Mark-and-sweep algorithm** - Identifying reachable objects and freeing unreachable ones
- **Stack-based root tracking** - Using a virtual stack as the root set for reachability
- **Automatic GC triggering** - Running garbage collection when allocation threshold is reached
- **Support for composite objects** - Handling both primitive integers and pairs (linked structures)

## Features

- **Lightweight VM implementation** - A simple virtual machine with a stack and object pool
- **Two object types**:
  - `OBJ_INT` - Integer values
  - `OBJ_PAIR` - Pairs of objects (can create nested structures)
- **Automatic garbage collection** - GC runs automatically when max allocations threshold is exceeded
- **Reachability-based collection** - Only collects objects unreachable from the stack
- **Cycle detection** - Safely handles circular references between objects
- **Comprehensive test suite** - Multiple test cases validating GC correctness

## Project Structure

```
├── gc.h              # Header file with VM and Object definitions
├── gc.c              # Core garbage collector implementation
├── main.c            # Test suite and example usage
├── Makefile          # Build configuration
└── README.md         # This file
```

## Building

### Prerequisites
- GCC compiler
- GNU Make

### Compilation

Build using the provided Makefile with sanitizers enabled:

```bash
make run
```

Or manually:

```bash
gcc -Wshadow -Wall -fsanitize=address,leak,undefined -g gc.c main.c -o gc_test
./gc_test
```

### Build Flags Explanation

The Makefile uses comprehensive sanitizers for debugging:
- `-Wshadow` - Warn about shadowed variables
- `-Wall` - Enable all common warnings
- `-fsanitize=address,leak,undefined` - Runtime memory and undefined behavior detection
- `-g` - Include debug symbols for debugging
- `-D_GLIBCXX_DEBUG` - Enable C++ standard library debugging

## Usage

### API Overview

#### VM Creation and Cleanup

```c
VM *newVM();           // Create a new virtual machine
void freeVM(VM* vm);   // Free VM and all allocated objects
```

#### Stack Operations

```c
void push(VM *vm, Object *value);    // Push object onto stack
Object *pop(VM *vm);                 // Pop object from stack
```

#### Object Creation

```c
// Push an integer onto the stack
void pushInt(VM *vm, int intValue);

// Create a pair from top two stack elements
// Pops two objects, creates a pair containing them, pushes the pair
Object *pushPair(VM *vm);

// Manual object creation
Object *newObject(VM *vm, ObjectType type);
```

#### Garbage Collection

```c
void gc(VM *vm);       // Run garbage collection immediately
void mark(Object *object);          // Mark an object as reachable
void markAll(VM *vm);               // Mark all stack-reachable objects
void sweep(VM *vm);                 // Sweep and free unreachable objects
```

### Example Usage

```c
#include "gc.h"
#include <stdio.h>

int main() {
    VM* vm = newVM();
    
    // Push some integers
    pushInt(vm, 42);
    pushInt(vm, 100);
    
    // Create a pair containing the two integers
    // This pops both integers and pushes back a pair
    pushPair(vm);
    
    // Create nested structures
    pushInt(vm, 1);
    pushInt(vm, 2);
    pushPair(vm);
    
    // Push another pair on top
    pushPair(vm);  // Now we have a pair of pairs
    
    // Objects on the stack are preserved during GC
    gc(vm);
    
    printf("Allocations after GC: %d\n", vm->currentAllocations);
    
    // Pop objects off stack
    Object* obj = pop(vm);
    
    // Without references, the pair will be collected on next GC
    gc(vm);
    
    return 0;
}
```

## How It Works

### Mark-and-Sweep Algorithm

The garbage collector operates in two phases:

#### 1. Mark Phase
- Starts from the VM stack (root set)
- Recursively marks all reachable objects
- Uses the `marked` field in each object to track marked status

```c
void markAll(VM *vm) {
    for (int i = 0; i < vm->stackSize; i++)
        mark(vm->stack[i]);
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
```

#### 2. Sweep Phase
- Iterates through all allocated objects
- Frees objects that were not marked as reachable
- Unmarks remaining objects for future GC cycles

```c
void sweep(VM *vm) {
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
```

### Automatic Triggering

GC runs automatically when:
- The current allocation count reaches `maxAllocations` threshold
- This happens before creating a new object in `newObject()`

```c
if (vm->currentAllocations >= vm->maxAllocations) {
    gc(vm);
}
```

## Configuration

Edit `gc.h` to adjust GC behavior:

```c
#define STACK_MAX 256        // Maximum stack depth
#define MAX_ALLOCATION 10    // GC trigger threshold (adjust for testing)
```

## Test Suite

The program includes comprehensive tests validating GC correctness:

- **testPreserveRoots** - Verifies objects on stack are preserved
- **testCollectUnreached** - Verifies unreachable objects are freed
- **testReachabilityNested** - Tests nested object structures
- **testCycles** - Ensures circular references don't crash GC
- **testAutoGcTrigger** - Verifies GC triggers at allocation threshold

Run tests:

```bash
make run
```

All tests should pass without memory leaks or errors.

## Memory Safety

The project uses GCC sanitizers to catch:
- **Address Sanitizer** - Buffer overflows, use-after-free, memory leaks
- **Undefined Behavior Sanitizer** - Integer overflows, invalid casts
- **Leak Detector** - Unreleased memory

All tests should run cleanly with no sanitizer warnings.

## Limitations

- **Fixed-size stack** - Stack limited to `STACK_MAX` entries
- **Simple object model** - Only supports integers and pairs
- **Single-threaded** - Not thread-safe
- **Stop-the-world collection** - Entire program pauses during GC
- **No incremental collection** - Full mark-sweep each cycle
- **No generational collection** - All objects treated equally

## Educational Value

This implementation serves as an excellent learning resource for:
- Understanding how garbage collectors work
- Learning mark-and-sweep algorithms
- Practicing memory management in C
- Exploring reachability analysis
- Building virtual machines

## References

- [Baby's First Garbage Collector](https://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/) - Original blog post
- [Garbage Collection: Algorithms for Automatic Dynamic Memory Management](https://www.elsevier.com/books/garbage-collection/jones/978-0-12-417156-5) - Comprehensive GC theory book

## License

This is an educational implementation based on publicly available tutorials.
