#ifndef TARO_VM_STACK_FRAME
#define TARO_VM_STACK_FRAME

#include "value.h"

/**
 * Stack frame
 */
typedef struct Frame
{
    int addr; // Instruction pointer
    Value **locals;
    int locals_count;
    int return_addr;
    int *saved_regs;
    int saved_regs_count;
    Value **params;
    int params_count;

    // Parent frame
    struct Frame *parent;
} Frame;

#endif