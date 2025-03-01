#ifndef TARO_VM_CORE_H
#define TARO_VM_CORE_H

#include "../util/arena.h"
#include "../util/hashtable.h"
#include "stackframe.h"
#include "value.h"
#include "vm_mem.h"

#include <pthread.h>
#include <stdint.h>

#define VM_DEFAULT_GC_THRESHOLD 1000
#define OPCODE_COUNT (24 + 1)

enum VMOpcode {
    NOP,
    SETL,
    GETL,
    PUSH_I,
    PUSH_F,
    POP,
    STORES,
    LOADS,
    CMP_I,
    CMP_F,
    J,
    JEQ,
    JNE,
    JLT,
    JGR,
    ADD_I,
    SUB_I,
    MUL_I,
    DIV_I,
    ADD_F,
    SUB_F,
    MUL_F,
    DIV_F,
    CALL,
    RET
};

typedef struct VMOperand {
    enum RuntimeValueType type;

    union {
        int int_value;
        float float_value;
        char *string_value;
    };
} VMOperand;

typedef struct VMInstruction {
    enum VMOpcode opcode;
    int operands_count;

    VMOperand operands[3];
} VMInstruction;

typedef struct VM {
    size_t ip;
    int eq, dif;

    VMInstruction *code;
    size_t code_size;

    // Memory
    VMMem mem;
    Hashtable *string_tbl;

    // GC related
    pthread_mutex_t gc_mutex;
    pthread_t gc_thread;
    bool stop_gc;
} VM;

void vm_init(VM *vm, int gc_threshold);
void vm_load(Arena *arena, VM *vm, uint8_t *code, size_t len);
void vm_cleanup(Arena *arena, VM *vm);
void vm_cycle(Arena *arena, VM *vm);
void vm_decode(Arena *arena, VM *vm, VMInstruction *ins);

#endif
