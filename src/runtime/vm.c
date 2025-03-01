/**
 * Main virtual machine that interprets its own bytecode format
 *
 * Authors:
 * - Charlotte (megabytesofrem)
 */

#include "vm.h"
#include "bytecode.h"
#include "gc.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "../util/arena.h"
#include "../util/logger.h"

// Background GC thread function
static void *gc_thread_func(void *arg) {
    VM *vm = (VM *)arg;

    while (!vm->stop_gc) {
        pthread_mutex_lock(&vm->gc_mutex);
        if (vm->mem.gc_counter >= vm->mem.gc_threshold) {
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

void vm_init(VM *vm, int gc_threshold) {
    vm->ip = 0;

    vm->code      = NULL;
    vm->code_size = 0;
    vm->mem.sp    = 0;

    vm->mem.stack_top = vm->mem.stack;
    vm->mem.heap      = NULL;

    vm->mem.gc_counter   = 0;
    vm->mem.gc_threshold = gc_threshold;
    vm->stop_gc          = false;

    vm->string_tbl = hashtable_create();

    pthread_mutex_init(&vm->gc_mutex, NULL);
    if (pthread_create(&vm->gc_thread, NULL, gc_thread_func, vm) != 0) {
        log_error("failed to create GC thread\n");
    }

    // Detach so the thread runs in the background
    pthread_detach(vm->gc_thread);
}

void vm_cleanup(Arena *arena, VM *vm) {
    hashtable_free(vm->string_tbl);

    pthread_mutex_lock(&vm->gc_mutex);
    vm->stop_gc = true;
    pthread_mutex_unlock(&vm->gc_mutex);

    HeapObj *entry = vm->mem.heap;
    while (entry) {
        HeapObj *next = entry->next;
        entry->free   = true;
        free(entry);
        entry = next;
    }

    pthread_mutex_destroy(&vm->gc_mutex);
    arena_destroy(arena);
}

void vm_load(Arena *arena, VM *vm, uint8_t *code, size_t len) {
    if (vm->code != NULL) {
        free(vm->code);
    }

    // Allocate memory for code
    vm->code = (VMInstruction *)malloc(len * sizeof(VMInstruction));
    read_bytecode_stream(code, len, vm->code, &vm->code_size);

    log_info("VM: loaded %d instructions\n", vm->code_size);
}

void vm_cycle(Arena *arena, VM *vm) {
    // Fetch-decode-execute cycle

    if (vm->code == NULL) {
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

    log_info("VM: ip: %d, opcode: %d\n", vm->ip, ins.opcode);

    vm_decode(arena, vm, &ins);
    vm->ip++;
}

void vm_decode(Arena *arena, VM *vm, VMInstruction *ins) {
    Value *a, *b;

    switch (ins->opcode) {
    case NOP:
        log_info("VM: NOP\n");
        break;
    case PUSH_I:
        log_info("VM: PUSHI %d\n", as_int(ins->operands[0]));
        stack_push(&vm->mem, &new_int(ins->operands[0].int_value));
        break;
    case PUSH_F:
        log_info("VM: PUSHF %f\n", as_float(ins->operands[0]));
        stack_push(&vm->mem, &new_float(ins->operands[0].float_value));
        break;
    case POP:
        log_info("VM: POP\n");
        stack_pop(&vm->mem);
        break;
    case J:
        log_info("VM: J %d\n", ins->operands[0].int_value);
        vm->ip = ins->operands[0].int_value;
        break;
    case JEQ:
        log_info("VM: JEQ\n");
        if (vm->eq == 1) {
            vm->ip = ins->operands[0].int_value;
        }
        break;
    case JNE:
        log_info("VM: JNE\n");
        if (vm->eq == 0) {
            vm->ip = ins->operands[0].int_value;
        }
        break;
    case JLT:
        log_info("VM: JLT\n");
        if (vm->dif == -1) {
            vm->ip = ins->operands[0].int_value;
        }
        break;
    case JGR:
        log_info("VM: JGR\n");
        if (vm->dif == 1) {
            vm->ip = ins->operands[0].int_value;
        }
        break;
    case ADD_I:
        a = stack_pop(&vm->mem);
        b = stack_pop(&vm->mem);
        log_info("VM: ADDI %d %d\n", b->data.int_value, a->data.int_value);
        stack_push(&vm->mem, &new_int(b->data.int_value + a->data.int_value));
        stack_dump(&vm->mem);
        break;
    case SUB_I:
        a = stack_pop(&vm->mem);
        b = stack_pop(&vm->mem);
        log_info("VM: SUBI %d %d\n", b->data.int_value, a->data.int_value);
        stack_push(&vm->mem, &new_int(b->data.int_value - a->data.int_value));
        stack_dump(&vm->mem);
        break;
    case MUL_I:
        a = stack_pop(&vm->mem);
        b = stack_pop(&vm->mem);
        log_info("VM: MUL %d %d\n", b->data.int_value, a->data.int_value);
        stack_push(&vm->mem, &new_int(b->data.int_value * a->data.int_value));
        stack_dump(&vm->mem);
        break;
    case DIV_I:
        a = stack_pop(&vm->mem);
        b = stack_pop(&vm->mem);
        log_info("VM: DIV %d %d\n", b->data.int_value, a->data.int_value);
        stack_push(&vm->mem, &new_int(b->data.int_value / a->data.int_value));
        stack_dump(&vm->mem);
        break;

    default:
        log_error("VM: unknown opcode %d\n", ins->opcode);
        break;
    }
}
