#include "vm.h"

#include <stdlib.h>
#include <string.h>

void vm_init(VM *vm)
{
    vm->stack = (Frame **)malloc(VM_STACK_SIZE * sizeof(Frame *));
    vm->stack_len = 0;

    vm->heap_free_list = NULL;
    vm->heap_alloc_list = NULL;
}

void vm_cleanup(VM *vm)
{
    free(vm->stack);
    vm->stack = NULL;

    Block *block = vm->heap_free_list;
    while (block) {
        Block *next = block->next;
        free(block);
        block = next;
    }
}

void vm_cycle(VM *vm)
{
    // Fetch-decode-execute cycle
    VMInstruction ins = vm->code[vm->ip++];
    switch (ins.opcode) {
    default: {
        log_error("unimplemented opcode %d\n", ins.opcode);
        break;
    }
    }
}

void *vm_stack_alloc(VM *vm, Frame *frame)
{
    // Check for stack overflow
    if (vm->stack_len >= VM_STACK_SIZE) {
        log_error("VM stack overflow\n");
        return NULL;
    }

    // Return a pointer to the stack frame
    vm->stack[vm->stack_len++] = frame;
    return (void *)frame;
}

void vm_stack_free(VM *vm, Frame *frame)
{
    // Find the frame in the stack
    int i;
    for (i = 0; i < vm->stack_len; i++) {
        if (vm->stack[i] == frame) {
            break;
        }
    }

    if (i == vm->stack_len) {
        log_error("failed to free, frame not found in stack\n");
        return;
    }

    // Shift the stack down
    for (int j = i; j < vm->stack_len - 1; j++) {
        vm->stack[j] = vm->stack[j + 1];
    }

    vm->stack_len--;
}

void *vm_alloc_block(VM *vm, size_t size)
{
    Block *block;
    if (vm->heap_free_list == NULL) {
        // Allocate a new block
        block = (Block *)aligned_alloc(VM_PAGE_SIZE, sizeof(Block));
        if (block == NULL) {
            log_error("failed to allocate memory for block\n");
            return NULL;
        }
    } else {
        // Reuse a block from the free list
        log_warn("reusing block at %p\n", (void *)vm->heap_free_list);
        block = vm->heap_free_list;
        vm->heap_free_list = block->next;
    }

    // Add the block to the list of allocated blocks
    block->next = vm->heap_alloc_list;
    vm->heap_alloc_list = block;

    block->obj = object_create(OBJ_UNKNOWN);
    return (void *)block;
}

void vm_free_block(VM *vm, void *pblock)
{
    if (pblock == NULL) {
        log_error("cannot free a NULL block\n");
        return;
    }

    log_info("freeing block at %p\n", pblock);
    Block *block = (Block *)pblock;

    // Remove the block from the list of allocated blocks
    Block **cur = &vm->heap_alloc_list;
    while (*cur && *cur != block) {
        cur = &(*cur)->next;
    }

    if (*cur) {
        *cur = block->next;
    } else {
        log_error("block not found in allocated list\n");
    }

    // Add the block to the free list
    block->next = vm->heap_free_list;
    block->obj = NULL;
    vm->heap_free_list = block;
}