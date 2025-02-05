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

    // Copy the opcode handlers into the VM
    _VMHandlers handlers = {
        [OP_ADD] = _op_add,
        [OP_SUB] = _op_sub,
        [OP_MUL] = _op_mul,
        [OP_DIV] = _op_div,
        [OP_CMP] = _op_cmp,
        [OP_JUMP] = _op_jump,
        [OP_JUMP_IF] = _op_jump_if,
        [OP_ALLOC] = _op_alloc,
        [OP_PUSH_FRAME] = _op_push_frame,
        [OP_LOCAL_SET] = _op_local_set,
        [OP_LOCAL_GET] = _op_local_get,
        // [OP_CALL] = _op_call,
    };

    memcpy(vm->handlers, handlers, sizeof(handlers));

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
    vm->handlers[ins.opcode](vm, &ins);
}

void *vm_stack_push(VM *vm, Frame *frame)
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
        log_error("failed to allocate memory for block\n");
        return NULL;
    }

    // Add the entry to the list of allocated blocks
    entry->next = vm->heap;
    vm->heap = entry;
    entry->value = value_create(TY_UNKNOWN);
    entry->free = false;

    vm->gc_counter++;
    return entry->value;
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

void _op_add(VM *vm, VMInstruction *ins)
{
}

void _op_sub(VM *vm, VMInstruction *ins)
{
}

void _op_mul(VM *vm, VMInstruction *ins)
{
}

void _op_div(VM *vm, VMInstruction *ins)
{
}

void _op_cmp(VM *vm, VMInstruction *ins)
{
    // Compare two values and set flag
    Value *a = &ins->operands[0];
    Value *b = &ins->operands[1];

    if (a->type != b->type) {
        log_error("cannot compare values of different types\n");
        return;
    }

    if (a->type == TY_INT) {
        vm->flag = a->data.int_value == b->data.int_value;
    } else if (a->type == TY_FLOAT) {
        vm->flag = a->data.float_value == b->data.float_value;
    } else {
        log_error("unsupported type for comparison\n");
    }
}

void _op_jump(VM *vm, VMInstruction *ins)
{
    // Jump to the specified instruction
    vm->ip = ins->operands[0].data.int_value;
}

void _op_jump_if(VM *vm, VMInstruction *ins)
{
    // Jump if the condition is true
    if (vm->flag == 1) {
        vm->ip = ins->operands[1].data.int_value;
    }
}

void _op_alloc(VM *vm, VMInstruction *ins)
{
    // Allocate a new object on the heap
    int size = ins->operands[0].data.int_value;

    Value *value = vm_heap_alloc(vm, size);
    Frame *cur_frame = vm->stack[vm->stack_len - 1];

    if (cur_frame->locals_count >= VM_STACK_MAX_SIZE) {
        log_error("frame locals overflow\n");
        return;
    }

    cur_frame->locals[cur_frame->locals_count++] = value;
}

void _op_push_frame(VM *vm, VMInstruction *ins)
{
    // Push a new frame onto the stack
    Frame *frame = (Frame *)malloc(sizeof(Frame));

    vm_stack_push(vm, frame);
}

void _op_local_set(VM *vm, VMInstruction *ins)
{
    // Set a local variable in the current frame
    Frame *cur_frame = vm->stack[vm->stack_len - 1];
    Value *value = &ins->operands[0];

    if (cur_frame->locals_count >= VM_STACK_MAX_SIZE) {
        log_error("frame locals overflow\n");
        return;
    }

    cur_frame->locals[cur_frame->locals_count++] = value;
}

void _op_local_get(VM *vm, VMInstruction *ins)
{
    // Get a local variable from the current frame
    Frame *cur_frame = vm->stack[vm->stack_len - 1];

    if (cur_frame->locals_count == 0) {
        log_error("frame locals underflow\n");
        return;
    }

    // Value *value = cur_frame->locals[--cur_frame->locals_count];
}

void _op_call(VM *vm, VMInstruction *ins)
{
    // Call a function
}