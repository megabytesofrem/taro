#include "vm.h"
#include "gc.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../util/logger.h"

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

    // Copy the opcode handlers into the VM
    _VMHandlers handlers = {
        _nop,  _load_int, _load_float, _load_string, _store_local, _load_local, _addi,
        _subi, _mul,      _div,        _call,        _return,      _jump,       _jump_z,
    };

    memcpy(vm->handlers, handlers, sizeof(_VMHandlers));

    vm->code      = NULL;
    vm->code_size = 0;
    vm->stack     = (Frame **)malloc(VM_STACK_MAX_SIZE * sizeof(Frame *));
    vm->stack_len = 0;

    vm->heap         = NULL;
    vm->gc_counter   = 0;
    vm->gc_threshold = gc_threshold;
    vm->stop_gc      = false;

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
        entry->free      = true;
        free(entry);
        entry = next;
    }

    pthread_mutex_destroy(&vm->gc_mutex);
}

void vm_load(VM *vm, VMInstruction *code, int len)
{
    if (vm->code != NULL) {
        free(vm->code);
    }

    // Allocate memory for new instructions
    vm->code = (VMInstruction *)malloc(len * sizeof(VMInstruction));
    if (!vm->code) {
        log_error("VM: failed to allocate memory for code\n");
        exit(EXIT_FAILURE);
    }

    // Copy the code into the VM
    memcpy(vm->code, code, len * sizeof(VMInstruction));
    vm->code_size = len;

    // Debug output
    for (int i = 0; i < len; i++) {
        printf("%d: %d\n", i, vm->code[i].opcode);
    }

    log_info("VM: loaded %d instructions\n", len);
}

void vm_cycle(VM *vm)
{
    // Fetch-decode-execute cycle

    if (vm->code == NULL) {
        log_error("VM: no code loaded\n");
        return;
    }

    if (vm->ip >= vm->code_size) {
        log_error("VM: ip out of bounds\n");
        return;
    }

    VMInstruction ins = vm->code[vm->ip];
    if (ins.opcode >= OPCODE_COUNT) {
        return;
    }

    if (vm->handlers[ins.opcode] == NULL) {
        log_error("VM: null handler for opcode: %d\n", ins.opcode);
        return;
    }

    log_info("VM: ip: %d, opcode: %d\n", vm->ip, ins.opcode);
    vm->handlers[ins.opcode](vm, &ins);

    vm->ip++;
}

void *vm_stack_push(VM *vm, Frame *frame)
{
    // Check for stack overflow
    if (vm->stack_len >= VM_STACK_MAX_SIZE) {
        log_error("VM: stack overflow\n");
        return NULL;
    }

    // Return a pointer to the stack frame
    vm->stack[vm->stack_len++] = frame;
    return (void *)frame;
}

void vm_stack_free(VM *vm, Frame *frame)
{
    // Find the frame in the stack
    int frame_index;

    for (frame_index = 0; frame_index < vm->stack_len; frame_index++) {
        if (vm->stack[frame_index] == frame) {
            break;
        }
    }

    if (frame_index == vm->stack_len) {
        log_error("VM: free failed, frame not found in stack\n");
        return;
    }

    // Shift the stack down
    for (int j = frame_index; j < vm->stack_len - 1; j++) {
        vm->stack[j] = vm->stack[j + 1];
    }

    vm->stack_len--;
}

Value *vm_heap_alloc(VM *vm, size_t size)
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
        log_error("VM: failed to allocate memory for block\n");
        return NULL;
    }

    // Add the entry to the list of allocated blocks
    entry->next  = vm->heap;
    vm->heap     = entry;
    entry->value = value_create(TY_UNKNOWN);
    entry->free  = false;

    vm->gc_counter++;
    return entry->value;
}

void vm_heap_free(VM *vm, void *pblock)
{
    if (pblock == NULL) {
        log_error("VM: failed to free, cannot free NULL block\n");
        return;
    }

#ifdef GC_DEBUG
    log_info("VM: freeing entry at %p\n", pblock);
#endif

    HeapObject *entry = (HeapObject *)pblock;

    // Prevent double-free, only free if we haven't already
    if (!entry->free) {
        if (entry->value) {
            if (entry->value->s_children) {
                free(entry->value->s_children);
            }
            free(entry->value);
        }

        entry->free = true;
        // vm->gc_counter--;
    }
}

/* Opcode handling */

void _nop(VM *vm, VMInstruction *ins)
{
    log_info("NOP\n");
}

void _load_int(VM *vm, VMInstruction *ins)
{
    // log_info("LOAD_INT %d\n", ins->operands[0].int_value);
}

void _load_float(VM *vm, VMInstruction *ins)
{
    // log_info("LOAD_FLOAT %f\n", ins->operands[0].float_value);
}

void _load_string(VM *vm, VMInstruction *ins)
{
    // log_info("LOAD_STRING %s\n", ins->operands[0].string_value);
}

void _store_local(VM *vm, VMInstruction *ins)
{
    // log_info("STORE_LOCAL %d\n", ins->operands[0].int_value);
}

void _load_local(VM *vm, VMInstruction *ins)
{
    // log_info("LOAD_LOCAL %d\n", ins->operands[0].int_value);
}

void _addi(VM *vm, VMInstruction *ins)
{
    log_info("ADDI\n");
}

void _subi(VM *vm, VMInstruction *ins)
{
    log_info("SUBI\n");
}

void _mul(VM *vm, VMInstruction *ins)
{
    log_info("MUL\n");
}

void _div(VM *vm, VMInstruction *ins)
{
    log_info("DIV\n");
}

void _call(VM *vm, VMInstruction *ins)
{
    log_info("CALL\n");
}

void _return(VM *vm, VMInstruction *ins)
{
    log_info("RETURN\n");
}

void _jump(VM *vm, VMInstruction *ins)
{
    log_info("JUMP\n");
}

void _jump_z(VM *vm, VMInstruction *ins)
{
    log_info("JUMP_Z\n");
}
