/**
 * Main virtual machine -- the kernel of Taro.
 *
 * Authors:
 * - Charlotte (megabytesofrem)
 */

#include "vm.h"
#include "gc.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <openssl/evp.h>

#include "../util/logger.h"

// Background GC thread function
static void *gc_thread_func(void *arg)
{
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

void vm_init(VM *vm, int gc_threshold)
{
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

void vm_cleanup(VM *vm)
{
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

    log_info("VM: ip: %d, opcode: %d\n", vm->ip, ins.opcode);

    vm_decode(vm, &ins);
    vm->ip++;
}

void vm_decode(VM *vm, VMInstruction *ins)
{
    switch (ins->opcode) {
    case NOP:
        log_info("VM: NOP\n");
        break;
    case PUSHI:
        log_info("VM: PUSHI %d\n", as_int(ins->operands[0]));
        stack_push(&vm->mem, &new_int(ins->operands[0].int_value));
        break;
    case PUSHF:
        log_info("VM: PUSHF %f\n", as_float(ins->operands[0]));
        stack_push(&vm->mem, &new_float(ins->operands[0].float_value));
        break;
    case POP:
        log_info("VM: POP\n");
        stack_pop(&vm->mem);
        break;
    case STORES: {
        // Store the value in the string table
        log_info("VM: STORES %s %s\n", as_string(ins->operands[0]),
                 as_string(ins->operands[1]));

        char *key = as_string(ins->operands[0]);
        char *val = as_string(ins->operands[1]);
        ht_set_string(vm->string_tbl, key, val);
        break;
    }
    case LOADS: {
        // Load the value from the string table onto the stack
        char *key = as_string(ins->operands[0]);
        char *val = ht_get_string(vm->string_tbl, key);
        log_info("VM: LOADS %s\n", key);

        if (val == NULL) {
            log_error("VM: key not found in string table\n");
            break;
        }

        stack_push(&vm->mem, &new_string(val));
        stack_dump(&vm->mem);

        break;
    }

    /* Addressing */
    case J:
        log_info("VM: J %d\n", ins->operands[0].int_value);
        vm->ip = ins->operands[0].int_value;
        break;
    case J_REL:
        log_info("VM: J_REL %d\n", ins->operands[0].int_value);
        vm->ip += ins->operands[0].int_value;
        break;
    case J_EQ:
        log_info("VM: J_EQ\n");
        break;

    /* Arithmetic */
    case ADDI: {
        Value *a = stack_pop(&vm->mem);
        Value *b = stack_pop(&vm->mem);
        log_info("VM: ADDI %d %d\n", b->data.int_value, a->data.int_value);
        stack_push(&vm->mem, &new_int(b->data.int_value + a->data.int_value));
        stack_dump(&vm->mem);
        break;
    }
    case SUBI: {
        Value *a = stack_pop(&vm->mem);
        Value *b = stack_pop(&vm->mem);
        log_info("VM: SUBI %d %d\n", b->data.int_value, a->data.int_value);
        stack_push(&vm->mem, &new_int(b->data.int_value - a->data.int_value));
        stack_dump(&vm->mem);
        break;
    }
    case MUL: {
        Value *a = stack_pop(&vm->mem);
        Value *b = stack_pop(&vm->mem);
        log_info("VM: MUL %d %d\n", b->data.int_value, a->data.int_value);
        stack_push(&vm->mem, &new_int(b->data.int_value * a->data.int_value));
        stack_dump(&vm->mem);
        break;
    }
    case DIV: {
        Value *a = stack_pop(&vm->mem);
        Value *b = stack_pop(&vm->mem);
        log_info("VM: DIV %d %d\n", b->data.int_value, a->data.int_value);
        stack_push(&vm->mem, &new_int(b->data.int_value / a->data.int_value));
        stack_dump(&vm->mem);
        break;
    }

    default:
        log_error("VM: unknown opcode %d\n", ins->opcode);
        break;
    }
}