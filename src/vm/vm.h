#ifndef TARO_VM_RUNTIME
#define TARO_VM_RUNTIME

#include "frame.h"
#include "opcode.h"
#include "value.h"

#include <pthread.h>

#define VM_DEFAULT_GC_THRESHOLD 1000
#define VM_OBJECT_TABLE_SIZE 1024
#define VM_STACK_MAX_SIZE 16384 // 16KB

#define OPCODE_COUNT 11

/**
 * An object on the heap with a pointer to the next object
 */
typedef struct HeapObject
{
    Value *value;
    struct HeapObject *next;

    bool free;
} HeapObject;

typedef struct VM
{
    size_t ip;
    int flag;

    // Opcode handlers
    void (*handlers[OPCODE_COUNT])(struct VM *, VMInstruction *);

    // VM instructions
    VMInstruction *code;
    int code_size;

    // Stack
    Frame **stack;
    int stack_len;

    // GC - see gc.c for implementation
    pthread_mutex_t gc_mutex;
    pthread_t gc_thread;
    bool stop_gc;
    int gc_counter;
    int gc_threshold;

    HeapObject *heap;
} VM;

typedef void (*_VMHandler)(VM *, VMInstruction *);
typedef void (*_VMHandlers[OPCODE_COUNT])(VM *, VMInstruction *);

void vm_init(VM *vm, int gc_threshold);
void vm_load(VM *vm, VMInstruction *code, int len);
void vm_cleanup(VM *vm);
void vm_cycle(VM *vm);
void *vm_stack_push(VM *vm, Frame *frame);
void vm_stack_free(VM *vm, Frame *frame);

/**
 * Allocate an object on the heap
 */
Value *vm_heap_alloc(VM *vm, size_t size);

/**
 * Free an object on the heap
 */
void vm_heap_free(VM *vm, void *pentry);

/* Define the opcode handlers */

void _op_add(VM *vm, VMInstruction *ins);
void _op_sub(VM *vm, VMInstruction *ins);
void _op_mul(VM *vm, VMInstruction *ins);
void _op_div(VM *vm, VMInstruction *ins);
void _op_cmp(VM *vm, VMInstruction *ins);
void _op_jump(VM *vm, VMInstruction *ins);
void _op_jump_if(VM *vm, VMInstruction *ins);
void _op_alloc(VM *vm, VMInstruction *ins);
void _op_push_frame(VM *vm, VMInstruction *ins);
void _op_local_set(VM *vm, VMInstruction *ins);
void _op_local_get(VM *vm, VMInstruction *ins);
void _op_call(VM *vm, VMInstruction *ins);

#endif
