#ifndef TARO_OPCODE_IMPL
#define TARO_OPCODE_IMPL

#include "value.h"

enum VMOpcode
{
    OP_PUSH,
    OP_POP,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_JUMP,
    OP_JUMP_IF,
    OP_PUSH_FRAME,
    OP_POP_FRAME,
    OP_END,
};

typedef struct VMInstruction
{
    enum VMOpcode opcode;
    int operands_count;
    Value **operands;
} VMInstruction;

#endif