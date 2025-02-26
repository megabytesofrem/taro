#ifndef TARO_VM_MEMORY_H
#define TARO_VM_MEMORY_H

#include "value.h"

#define VM_STACK_MAX_SIZE 16384 // 16KB

/**
 * An object on the heap with a pointer to the next object
 */
typedef struct HeapObj {
    Value *value;
    struct HeapObj *next;

    bool free;
} HeapObj;

typedef struct VMMem {
    size_t sp;

    Value stack[VM_STACK_MAX_SIZE];
    Value *stack_top;
    HeapObj *heap;

    // GC related
    int gc_counter;
    int gc_threshold;
} VMMem;

void stack_push(VMMem *mem, Value *value);
Value *stack_pop(VMMem *mem);

void stack_dump(VMMem *mem);

Value *heap_alloc(VMMem *mem, size_t size);
void heap_free(VMMem *mem, void *pentry);

#endif
