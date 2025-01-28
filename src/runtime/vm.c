#include "vm.h"
#include "gc.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Background GC thread function
static void *gc_thread_func(void *arg)
{
    VM *vm = (VM *)arg;

    while (!vm->stop_gc) {
        pthread_mutex_lock(&vm->gc_mutex);
        if (vm->gc_counter >= vm->gc_threshold) {
            pthread_mutex_unlock(&vm->gc_mutex);
            gc_collect(vm);
        } else {
            pthread_mutex_unlock(&vm->gc_mutex);
        }

        // Sleep for a short duration to prevent busy waiting
        usleep(100000); // 100ms
    }

    log_info("GC thread exiting...\n");
    return NULL;
}

void vm_init(VM *vm, int gc_threshold)
{
    vm->ip = 0;
    vm->code = NULL;
    vm->code_size = 0;
    vm->stack = (Frame **)malloc(VM_STACK_MAX_SIZE * sizeof(Frame *));
    vm->stack_len = 0;

    vm->heap = NULL;
    vm->gc_counter = 0;
    vm->gc_threshold = gc_threshold;
    vm->stop_gc = false;

    pthread_mutex_init(&vm->gc_mutex, NULL);
    if (pthread_create(&vm->gc_thread, NULL, gc_thread_func, vm) != 0) {
        log_error("failed to create GC thread\n");
    }

    // Detach so the thread runs in the background
    pthread_detach(vm->gc_thread);
}

void vm_cleanup(VM *vm)
{
    pthread_mutex_lock(&vm->gc_mutex);
    vm->stop_gc = true;
    pthread_mutex_unlock(&vm->gc_mutex);

    free(vm->stack);
    vm->stack = NULL;

    HeapObject *entry = vm->heap;
    while (entry) {
        HeapObject *next = entry->next;
        entry->free = true;
        free(entry);
        entry = next;
    }

    pthread_mutex_destroy(&vm->gc_mutex);
}

void vm_load(VM *vm, VMInstruction *code, int len)
{
    vm->code = code;
    vm->code_size = len;

    log_info("loaded %d instructions\n", vm->code_size);
}

void vm_cycle(VM *vm)
{
    // Fetch-decode-execute cycle

    if (vm->code == NULL) {
        log_error("no code loaded\n");
        return;
    }

    if (vm->ip >= vm->code_size) {
        log_error("instruction pointer out of bounds\n");
        return;
    }

    VMInstruction ins = vm->code[vm->ip++];

    switch (ins.opcode) {
    default: {
        printf("unimplemented opcode %d\n", ins.opcode);
        break;
    }
    }
}

void *vm_stack_alloc(VM *vm, Frame *frame)
{
    // Check for stack overflow
    if (vm->stack_len >= VM_STACK_MAX_SIZE) {
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
    int frameno;

    for (frameno = 0; frameno < vm->stack_len; frameno++) {
        if (vm->stack[frameno] == frame) {
            break;
        }
    }

    if (frameno == vm->stack_len) {
        log_error("failed to free, frame not found in stack\n");
        return;
    }

    // Shift the stack down
    for (int j = frameno; j < vm->stack_len - 1; j++) {
        vm->stack[j] = vm->stack[j + 1];
    }

    vm->stack_len--;
}

void *vm_heap_alloc(VM *vm, size_t size)
{
    HeapObject *entry = vm->heap;

    // Try to find a free space
    while (entry != NULL) {
        if (entry->free) {
            entry->free = false;
            return (void *)entry;
        }

        entry = entry->next;
    }

    // Allocate a new entry, if no free block is found
    entry = (HeapObject *)malloc(sizeof(HeapObject) + size);
    if (entry == NULL) {
        log_error("failed to allocate memory for block\n");
        return NULL;
    }

    // Add the entry to the list of allocated blocks
    entry->next = vm->heap;
    vm->heap = entry;
    entry->obj = value_create(TY_UNKNOWN);
    entry->free = false;

    vm->gc_counter++;
    return (void *)entry;
}

void vm_heap_free(VM *vm, void *pblock)
{
    if (pblock == NULL) {
        log_error("cannot free a NULL block\n");
        return;
    }

#ifdef GC_DEBUG
    log_info("freeing entry at %p\n", pblock);
#endif

    HeapObject *entry = (HeapObject *)pblock;

    // Prevent double-free, only free if we haven't already
    if (!entry->free) {
        if (entry->obj) {
            if (entry->obj->s_children) {
                free(entry->obj->s_children);
            }
            free(entry->obj);
        }

        entry->free = true;
        // vm->gc_counter--;
    }
}