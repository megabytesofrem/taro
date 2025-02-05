#ifndef TARO_VM_STACK_FRAME
#define TARO_VM_STACK_FRAME

#include "value.h"

/**
 * Stack frame
 */
typedef struct Frame
{
    int pc;
    Value locals[16];
    int locals_count;

    // Parent frame
    struct Frame *parent;
} Frame;

#endif