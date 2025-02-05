#ifndef TARO_VM_RUNTIME
#define TARO_VM_RUNTIME

#include "frame.h"
#include "value.h"

#include <pthread.h>

#define VM_DEFAULT_GC_THRESHOLD 1000
#define VM_OBJECT_TABLE_SIZE 1024
#define VM_STACK_MAX_SIZE 16384 // 16KB

#define OPCODE_COUNT (JUMP_Z + 1)

enum VMOpcode
{
    NOP         = 0x00,
    LOAD_INT    = 0x01,
    LOAD_FLOAT  = 0x02,
    LOAD_STRING = 0x03,
    STORE_LOCAL = 0x04,
    LOAD_LOCAL  = 0x05,
    ADDI        = 0x06,
    SUBI        = 0x07,
    MUL         = 0x08,
    DIV         = 0x09,
    CALL        = 0x0A,
    RETURN      = 0x0B,
    JUMP        = 0x0C,
    JUMP_Z      = 0x0D,
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

    // VMOperand operands[3];

} VMInstruction;

/**
 * An object on the heap with a pointer to the next object
 */
typedef struct HeapObject
{
    Value *value;
    struct HeapObject *next;

    bool free;
} HeapObject;

typedef struct VM
{
    size_t ip;
    int flag;

    // Opcode handlers
    void (*handlers[OPCODE_COUNT])(struct VM *, VMInstruction *);
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

typedef void (*_VMHandler)(VM *, VMInstruction *);
typedef void (*_VMHandlers[OPCODE_COUNT])(VM *, VMInstruction *);

void vm_init(VM *vm, int gc_threshold);
void vm_load(VM *vm, VMInstruction *code, int len);
void vm_cleanup(VM *vm);
void vm_cycle(VM *vm);
void *vm_stack_push(VM *vm, Frame *frame);
void vm_stack_free(VM *vm, Frame *frame);

/**
 * Allocate an object on the heap
 */
Value *vm_heap_alloc(VM *vm, size_t size);

/**
 * Free an object on the heap
 */
void vm_heap_free(VM *vm, void *pentry);

/* Define the opcode handlers */

void _nop(VM *vm, VMInstruction *ins);
void _load_int(VM *vm, VMInstruction *ins);
void _load_float(VM *vm, VMInstruction *ins);
void _load_string(VM *vm, VMInstruction *ins);
void _store_local(VM *vm, VMInstruction *ins);
void _load_local(VM *vm, VMInstruction *ins);
void _addi(VM *vm, VMInstruction *ins);
void _subi(VM *vm, VMInstruction *ins);
void _mul(VM *vm, VMInstruction *ins);
void _div(VM *vm, VMInstruction *ins);
void _call(VM *vm, VMInstruction *ins);
void _return(VM *vm, VMInstruction *ins);
void _jump(VM *vm, VMInstruction *ins);
void _jump_z(VM *vm, VMInstruction *ins);

#endif
