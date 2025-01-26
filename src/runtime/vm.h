#ifndef TARO_VM
#define TARO_VM

#include "object.h"
#include "opcode.h"

#define VM_STACK_SIZE 16384 // 16KB
#define VM_PAGE_SIZE 4096

/**
 * Contiguous memory block aligned to 4KB pages
 */
typedef struct Block
{
    Object *obj;
    struct Block *next;
} Block;

/**
 * Stack frame
 */
typedef struct Frame
{
    int addr; // Instruction pointer
    Object **locals;
    int locals_count;
    int return_addr;
    int *saved_regs;
    int saved_regs_count;

    // Function parameters, if any
    Object **params;
    int params_count;

    // Parent frame
    struct Frame *parent;
} Frame;

typedef struct
{
    size_t ip;

    // VM instructions
    VMInstruction *code;

    // Stack
    Frame **stack;
    int stack_len;

    Block *heap_free_list;  // Free list of heap memory
    Block *heap_alloc_list; // List of allocated heap memory
} VM;

void vm_init(VM *vm);
void vm_cleanup(VM *vm);
void vm_cycle(VM *vm);
void *vm_stack_alloc(VM *vm, Frame *frame);
void vm_stack_free(VM *vm, Frame *frame);

/**
 * Allocate a block of heap-allocated memory of size `size`
 */
void *vm_alloc_block(VM *vm, size_t size);

/**
 * Free a block of heap-allocated memory
 */
void vm_free_block(VM *vm, void *pblock);

#endif