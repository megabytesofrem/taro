#ifndef TARO_VM_CORE_H
#define TARO_VM_CORE_H

#include "../util/hashtable.h"
#include "frame.h"
#include "value.h"
#include "vm_mem.h"

#include <pthread.h>

#define VM_DEFAULT_GC_THRESHOLD 1000
#define OPCODE_COUNT (ALLOCA + 1)

enum VMOpcode
{
    NOP,
    PUSHI,
    PUSHF,
    POP,
    STORES, /* Store string*/
    LOADS,  /* Load string */

    /* Comparison */
    CMP,
    J,     /* Jump to a label*/
    J_REL, /* Jump relative to IP */
    J_EQ,  /* Jump if equal */
    J_NE,  /* Jump if not equal */
    J_Z,   /* Jump if zero */

    /* Arithmetic */
    ADDI,
    SUBI,
    MUL,
    DIV,
    ADDF,
    SUBF,

    /* Function */
    CALL,
    RET,
    STOREL, /* Store local variable */
    LOADL,  /* Load local variable */
    ALLOCA, /* Allocate memory */
};

typedef struct VMOperand
{
    enum TrValueType type;

    union {
        int int_value;
        float float_value;
        char *string_value;
    };
} VMOperand;

typedef struct VMInstruction
{
    enum VMOpcode opcode;
    int operands_count;

    VMOperand operands[3];
} VMInstruction;

typedef struct VM
{
    size_t ip;
    int flag;

    VMInstruction *code;
    int code_size;

    // Memory
    VMMem mem;

    Hashtable *string_tbl;

    // GC related
    pthread_mutex_t gc_mutex;
    pthread_t gc_thread;
    bool stop_gc;
} VM;

typedef void (*_VMHandler)(VM *, VMInstruction *);
typedef void (*_VMHandlers[OPCODE_COUNT])(VM *, VMInstruction *);

void vm_init(VM *vm, int gc_threshold);
void vm_load(VM *vm, VMInstruction *code, int len);
void vm_cleanup(VM *vm);
void vm_cycle(VM *vm);
void vm_decode(VM *vm, VMInstruction *ins);

#endif
