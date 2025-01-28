#ifndef TARO_VM
#define TARO_VM

#include "opcode.h"
#include "value.h"

#include <pthread.h>

#define VM_DEFAULT_GC_THRESHOLD 1000
#define VM_STACK_MAX_SIZE 16384 // 16KB

/**
 * An object on the heap with a pointer to the next object
 */
typedef struct HeapObject
{
    Value *obj;
    struct HeapObject *next;

    bool free;
} HeapObject;

/**
 * Stack frame
 */
typedef struct Frame
{
    int addr; // Instruction pointer
    Value **locals;
    int locals_count;
    int return_addr;
    int *saved_regs;
    int saved_regs_count;
    Value **params;
    int params_count;

    // Parent frame
    struct Frame *parent;
} Frame;

typedef struct
{
    size_t ip;

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

void vm_init(VM *vm, int gc_threshold);
void vm_load(VM *vm, VMInstruction *code, int len);
void vm_cleanup(VM *vm);
void vm_cycle(VM *vm);
void *vm_stack_alloc(VM *vm, Frame *frame);
void vm_stack_free(VM *vm, Frame *frame);

/**
 * Allocate an object on the heap
 */
void *vm_heap_alloc(VM *vm, size_t size);

/**
 * Free an object on the heap
 */
void vm_heap_free(VM *vm, void *pentry);

#endif