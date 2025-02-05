#ifndef TARO_VM_OPCODE
#define TARO_VM_OPCODE

#include "frame.h"
#include "value.h"

#define _DEFINE_INSTRUCTION(_op, _size, _frame, ...)                                     \
    (VMInstruction)                                                                      \
    {                                                                                    \
        .opcode = _op, .operands_count = _size, .frame = _frame,                         \
        .operands = {__VA_ARGS__},                                                       \
    }

/* Define a VM instruction */
#define INST(_op, _size, ...) _DEFINE_INSTRUCTION(_op, _size, NULL, {__VA_ARGS__})

/* Define an instruction with an attached stack frame */
#define INST_FRAME(_op, _size, _frame, ...)                                              \
    _DEFINE_INSTRUCTION(_op, _size, _frame, __VA_ARGS__)

enum VMOpcode
{
    /* See VM.md for documentation */
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_CMP,
    OP_JUMP,
    OP_JUMP_IF,
    OP_ALLOC,
    OP_PUSH_FRAME,
    OP_LOCAL_SET,
    OP_LOCAL_GET,
    OP_CALL,
};

typedef struct VMInstruction
{
    enum VMOpcode opcode;
    int operands_count;

    Frame *frame;
    Value operands[8];
} VMInstruction;

#endif