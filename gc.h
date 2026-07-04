#define STACK_MAX 256
#define MAX_ALLOCATION 10

typedef enum { OBJ_INT, OBJ_PAIR } ObjectType;

typedef struct sObject {
  unsigned char marked;
  ObjectType type;
  struct sObject *next;

  union {
    int value;

    struct {
      struct sObject *head;
      struct sObject *tail;
    };
  };
} Object;

typedef struct {
  Object *firstObject;
  Object *stack[STACK_MAX];
  int stackSize;
  int currentAllocations;
  int maxAllocations;

} VM;

VM *newVM();
void push(VM *vm, Object *value);
Object *pop(VM *vm);
Object *newObject(VM *vm, ObjectType type);
void pushInt(VM *vm, int intValue);
Object *pushPair(VM *vm);
void mark(Object *object);
void markAll(VM *vm);
void sweep(VM *vm);
void gc(VM *vm);